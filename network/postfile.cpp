/*
 * postfile.cpp
 *
 * HTTP File Posting Routines.
 *
 * Copyright (c) 2007 Paulo Matias.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) AND CONTRIBUTOR(S) ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR(S) OR CONTRIBUTOR(S) BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>

#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#else
extern "C" {
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
}
#define closesocket(fd) close(fd)
#endif



using namespace std;


namespace Network {

    void PostFile(FILE *fp, const char *filename, const char *fieldname,
                  const char *host, const int port, const char *path, std::string &response,
                  void(*progress)(void *cb_data, float p), void *cb_data) {

        const char *boundary = "---------------------------8378577591595262562761005572";

        // Resolve hostname.
        struct hostent *hp = gethostbyname(host);
        if(!hp) {
            throw("Não foi possível localizar o servidor.\n"
                  "Verifique sua conexão com a Internet.");
        }

        // Fill in addr struct.
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        memcpy(&addr.sin_addr, hp->h_addr, hp->h_length);

        // Create socket.
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd < 0) {
            throw("Não foi possível alocar um socket no sistema.\n"
                  "Tente fechar alguns programas ou reiniciar o sistema.");
        }

        // Connect.
        if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            closesocket(sockfd);
            throw("Não foi possível conectar ao servidor.\n"
                  "Verifique sua conexão com a Internet.");
        }

        // Get filesize.
        fseek(fp, 0, SEEK_END);
        long filesize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        // Generate multipart header.
        ostringstream mphdr;
        mphdr << "--" << boundary << "\r\n";
        mphdr << "Content-Disposition: form-data; name=\"" << fieldname;
        mphdr << "\"; filename=\"" << filename << "\"\r\n";
        mphdr << "Content-Type: application/octet-stream\r\n\r\n";

        // Generate multipart footer.
        ostringstream mpftr;
        mpftr << "\r\n--" << boundary << "--\r\n";

        // Generate HTTP header.
        ostringstream httphdr;
        httphdr << "POST " << path << " HTTP/1.1\r\n";
        httphdr << "Host: " << host << "\r\n";
        httphdr << "Connection: close\r\n";
        httphdr << "Content-Type: multipart/form-data; boundary=" << boundary << "\r\n";
        httphdr << "Content-Length: " << (mphdr.str().size() + mpftr.str().size() + filesize) << "\r\n\r\n";

        // Send HTTP and multipart headers.
        send(sockfd, httphdr.str().data(), httphdr.str().size(), 0);
        send(sockfd, mphdr.str().data(), mphdr.str().size(), 0);

        // Send file.
        char buffer[128];
        size_t bytes_read, bytes_sent = 0;

        while(!feof(fp)) {
            bytes_read = fread(buffer, 1, sizeof(buffer), fp);
            bytes_sent += send(sockfd, buffer, bytes_read, 0);
            if(progress) progress(cb_data, ((float)bytes_sent)/filesize);
        }

        // Send multipart footer.
        send(sockfd, mpftr.str().data(), mpftr.str().size(), 0);

        // Receive response.
        int buflen;
        response = "";
        while((buflen = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
            buffer[buflen] = 0;
            response += buffer;
        }

        // Close socket.
        closesocket(sockfd);

    }

}
