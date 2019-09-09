// zmq_wlan_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <thread>
#include "czmq.h"
#include "../data_types.h"

#pragma  comment(lib,"ws2_32.lib")

std::string remote_server_ip;

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
    if (inet_pton(AF_INET, remote_server_ip.c_str(), &serv_addr.sin_addr) <= 0)
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

    int iResult;
    do
    {
        iResult = recv(client_fd, buffer, len, 0);
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());
        printf("%s\n", buffer);

        // echo
        send(client_fd, buffer, iResult, 0);

    } while (iResult > 0);

    delete buffer;
    buffer = nullptr;

    closesocket(client_fd);
}

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        printf("The IP of remote server is expected\n");
        getchar();
        return -1;
    }

    // get inputed server ip
    remote_server_ip = argv[1];

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

