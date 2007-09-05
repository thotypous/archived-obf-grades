#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#endif

#include "crypto/hmac.h"
#include "core.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>

#include "FL/Fl_SQL_Table.H"
#include "sqlite/sql.h"

#include "gui/gui.h"

int main(int argc, char **argv) {

#ifdef WIN32
    WSAData wsaData;
    WSAStartup(MAKEWORD(1, 1), &wsaData);
#endif

    Janela_Principal janela;
    janela.show();


    int ret = Fl::run();

#ifdef WIN32
    WSACleanup();
#endif

    return ret;

}
