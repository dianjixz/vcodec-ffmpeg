/*
   Copyright (C) 2012, 2013, 2023
   Andy Warner
   This file is part of the sockstr class library.

   The sockstr class library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The sockstr class library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the sockstr library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

// echoserver.cpp
//
// This example spawns a server socket thread that accepts client connections.
// Then it reads messages from client and optionally echoes them back.
//
// This program relies on posix threads and is only known to work on 
// linux for now.

#include <sockstr/Socket.h>

#include <cerrno>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <thread>
#include <vector>
#include <unistd.h>
using namespace sockstr;
using std::cout;
using std::endl;


bool exit_server = false;
Socket serverSock;

// Params is the structure passed between threads
struct Params {
    Params(int _port, bool _binary, bool _echo, Stream* _sock = 0)
        : port(_port), binary(_binary), echo(_echo), clientSock(_sock) {}
    Params(Params* p) : port(p->port), binary(p->binary), echo(p->echo), 
                        clientSock(p->clientSock) { }

    int port;
    bool binary;
    bool echo;
    Stream* clientSock;
};

#include <iostream>


#include "vcodec.hpp"
#include <opencv2/opencv.hpp>
vcodec *adecoder_h265;
void get_pictor(size_t count, void *data, size_t len)
{
    adecoder_h265->yuv2rgb(data);
    printf("get c:%d h:%d   w:%d    len:%d\r\n",count,adecoder_h265->rgb.h, adecoder_h265->rgb.w, adecoder_h265->rgb.len);
    cv::Mat src(adecoder_h265->rgb.h,adecoder_h265->rgb.w, CV_8UC3, (void*)(adecoder_h265->rgb.data));
    cv::Mat dst;
    cv::cvtColor(src, dst, cv::COLOR_RGB2BGR);
    cv::imshow("123",dst);
    cv::waitKey(3);
}

Stream* allclientSock;
int my_read(void *buf, size_t n)
{
    return (int)allclientSock->read(buf, n);
}
int my_eof(void *buf, size_t n)
{
    return (int)allclientSock->eof();
}
vcodec encoder_h265 = vcodec(my_read, my_eof, "hevc", get_pictor);
void client_handler(Params* params) {
    cout << "Client process started." << endl;
    allclientSock = params->clientSock;
    int totalRead = 0;

    while (!exit_server && allclientSock->good()) {
        adecoder_h265 = &encoder_h265;
        encoder_h265.decode();

        exit(-1);

        // while (!allclientSock->eof())
        // {
        //     char buf[512];
        //     allclientSock->read(buf, 1);
        //     // int mk = allclientSock->gcount();
        //     for (size_t i = 0; i < mk; i++)
        //     {
        //         printf("%c",buf[i]);
        //     }
            
        // }
        


        // if (params->binary) {
        //     char buf[512];
        //     int sz = clientSock->read(buf, sizeof(buf));
        //     if (sz > 0) {
        //         if (params->echo) {
        //             cout << std::string(buf, sz) << endl;
        //         } else {
        //             cout << "Echoing " << sz << " bytes." << endl;
        //         }
        //         // clientSock->write(buf, sz);
        //         totalRead += sz;
        //     }
        // } else {
        //     std::string strbuf;
        //     clientSock->read(strbuf, EOF);
        //     *clientSock << strbuf;
        // }
    }

    if (allclientSock->good()) {
        allclientSock->close();
    }
    delete allclientSock;
    delete params;

    cout << "Client exiting... total bytes read=" << totalRead << endl;
}


// The echo server runs in the main process thread.  It spawns a thread for
// each client connection
void server_process(Params* params) {
    cout << "Server connecting to port " << params->port << endl;

    SocketAddr saddr(params->port);
    if (!serverSock.open(saddr, Socket::modeReadWrite)) {
        cout << "Error opening server socket: " << errno << endl;
        return;
    }

    std::vector<std::thread> threads;
    while (!exit_server && serverSock.good()) {
        Stream* clientSock = serverSock.listen();
        if (clientSock && clientSock->good()) {
            Params *clientParams = new Params(params);
            clientParams->clientSock = clientSock;
            
            threads.emplace_back(client_handler, clientParams);
        }
    }

    if (serverSock.good()) {
        serverSock.close();
    }
    for (auto& thr : threads) {
        if (thr.joinable()) {
            thr.join();
        }
    }
    threads.clear();
}

// Signal handler
void quitit(int sig) {
    cout << endl << "Exiting...";
    // Exit as cleanly as possible
    exit_server = true;
    serverSock.close();
    cout << endl;
}


int main(int argc, char* argv[]) {
    Params params(10001, true, true);

    int opt;
    while ((opt = getopt(argc, argv, "as")) != -1) {
        switch (opt) {
            case 'a':
                params.binary = false;
                break;
            case 's':
                params.echo = false;
                break;
            default:
                cout << "Usage:  echoserver [ -as ] <port>" << endl
                     << "          -a for string reads on socket" << endl
                     << "          -s = only display summary on stdout" << endl;
                return 1;
        }
    }

    if (optind < argc) {
        params.port = atoi(argv[optind]);
    }
    
    cout << "Using " << (params.binary ? "block" : "string") << " copy, "
         << (params.echo ? "echoing" : "not echoing") << " contents." << endl;

    signal(SIGINT, quitit);
    signal(SIGQUIT, quitit);

    server_process(&params);
    return 0;
}


// g++ -std=c++17 -Wall -g -O0 -DTARGET_LINUX=1 -I.. -I../include -c echoserver.cpp
// g++ -pthread -lssl -lcrypto  echoserver.o ../src/libsockstr.a /usr/lib/x86_64-linux-gnu/libssl.so /usr/lib/x86_64-linux-gnu/libcrypto.so   -o echoserver
