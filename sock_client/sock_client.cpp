// zmq_wlan_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <thread>
#include "czmq.h"
#include "../data_types.h"

#pragma  comment(lib,"ws2_32.lib")

void fn_sock_client()
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    // Creating socket file descriptor 
    if (client_fd == 0)
    {
        DWORD err = GetLastError();
        printf("socket failed: error %d\n", err);
        return;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SOCK_SERVER_LISTEN_PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form 
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0)
    {
        DWORD err = GetLastError();
        printf("inet_pton failed: Invalid address or address not supported. Error %d\n", err);
        return;
    }

    ////set the socket in non-blocking
    //unsigned long iMode = 1;
    //if (ioctlsocket(client_fd, FIONBIO, &iMode) != NO_ERROR)
    //{
    //    DWORD err = GetLastError();
    //    printf("ioctlsocket failed with error: %ld\n", err);
    //}

    int idx = 0;
    printf("connecting ...\n");
    while (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        idx++;
        DWORD err = GetLastError();
        printf("[%d] connect failed: error %d\n", idx, err);
    }

    int len = 10240;
    char* buffer = new char[len];
    buffer[0] = 0;

    int valread;
    while (true)
    {
        valread = read(client_fd, buffer, len);
        printf("%s\n", buffer);
        send(client_fd, buffer, strlen(buffer), 0);
    }

    delete buffer;
    buffer = nullptr;

    closesocket(client_fd);
}

int main()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(2, 2);
    int err = WSAStartup(wVersionRequested, &wsaData);
    printf("Win sock initialized %s\n", err == 0 ? "successfully" : "failed");

    std::thread org_sock_r(fn_sock_client);

    getchar();

    WSACleanup();

    return 0;
}

