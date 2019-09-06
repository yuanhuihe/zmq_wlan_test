// zmq_wlan_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <thread>
#include <vector>
#include <tuple>
#include "czmq.h"

#include "../data_types.h"
#pragma  comment(lib,"ws2_32.lib")

void fn_sock_recv(void* param);
void fn_sock_listen(void* param)
{
    std::vector<std::tuple<std::thread*, int>>* clients = (std::vector<std::tuple<std::thread*, int>>*)param;

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // Creating socket file descriptor 
    if (server_fd == 0)
    {
        DWORD err = GetLastError();
        printf("socket failed: error %d\n", err);
        return;
    }

    // Forcefully attaching socket to the port 8089
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)))
    {
        DWORD err = GetLastError();
        printf("setsockopt failed: error %d\n", err);
        return;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(SOCK_SERVER_LISTEN_PORT);

    int addrlen = sizeof(address);
    // Forcefully attaching socket to the port 8089
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        DWORD err = GetLastError();
        printf("bind failed: error %d\n", err);
        return;
    }

    int counter = 0;
    while (true)
    {
        counter++;
        printf("listenning %d ...\n", counter);

        if (listen(server_fd, 3) < 0)
        {
            DWORD err = GetLastError();
            printf("listen failed: error %d\n", err);
            continue;
        }

        int new_socket, valread;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            DWORD err = GetLastError();
            printf("accept failed: error %d\n", err);
            continue;
        }

        // start a new thread and save its info
        clients->push_back({ new std::thread(fn_sock_recv, &new_socket), new_socket });
        printf("Accepted new link\n");
    }

    closesocket(server_fd);
}

void fn_sock_recv(void* param)
{
    int sock_fd = *(int*)param;
    if (sock_fd == 0)
    {
        DWORD err = GetLastError();
        printf("client socket error\n", err);
        return;
    }
    
    int len = 10240;
    char* buffer = new char[len];
    buffer[0] = 0;

    int iResult;
    char *hello = "Hello from client";
    do
    {
        send(sock_fd, hello, strlen(hello), 0);
        printf("Hello message sent\n");

        iResult = recv(sock_fd, buffer, len, 0);
        printf("%s\n", buffer);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }while (iResult > 0);

    delete buffer;
    buffer = nullptr;

    closesocket(sock_fd);
}

int main()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    int err = WSAStartup(wVersionRequested, &wsaData);
    printf("Socket mod\n");
    printf("Win sock initialized %s\n", err == 0 ? "successfully" : "failed");

    std::vector<std::tuple<std::thread*, int>> clients;
    std::thread org_sock_p(fn_sock_listen, &clients);

    getchar();

    for (auto item : clients)
    {
        closesocket(std::get<1>(item));
        delete std::get<0>(item);
    }

    WSACleanup();

    return 0;
}

