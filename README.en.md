# OBF Grade Entering

[Portuguese Version](README.md)

Until 2007, the Brazilian Physics Olympiad (OBF) made available to schools which hosted its first phase a software for entering grades when *offline*, as an alternative to the *online* grade entering which was also available through a Web system. *Offline* grade was pretty important in Brazilian locations which could not afford a stable connection to the Internet.

This repository contains the complete source code of the client software used in OBF's 2007 edition. This was the first year in which a crossplatform software (Linux, BSD and Windows), written in C++, was adopted. In the previous years, a software written in Delphi was used, which could be run exclusively on Windows. The server code (written in PHP) unfortunately was lost.

The purpose of this repository is purely archaeological, as this code is completely retired by now. Our aim is to preserve this part of OBF's history. Unfortunately, back then we did not have the habit of using version control for small projects, and for this reason the code was uploaded in a single commit.

## Features

Despite the lack of experience of the developer and despite the fact that it was written in a hurry, during a full semester of undergraduate studies, the software has some interesting features:

* The file generated for sending to OBF's organizers was signed with HMAC. We aimed to provide a little protection (little because the key was not so strong) against adulteration of the grades during the transit of the file in a physical media.
* The file could be sent to the server by anyone. It was not needed to post it by mail directly to the OBF's organizers. The person responsible for the test in some location with a precarious connection to the Internet could trust the file to someone who could afford a stable connection. This person then only needed to upload the file to a website using his browser.
* It was possible to send data to the server directly from the client software itself. Some people preferred to enter the grades in the client instead of entering them in the website, despite possessing a stable connection.
* When the grades were sent, the server always issued a HMAC-signed receipt, with a QRCode to ease reading if needed. Besides being saved to the database, everyting was logged to files (including old data which could have been removed by the user) to allow for recovering if needed.
