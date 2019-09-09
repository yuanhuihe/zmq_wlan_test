// zmq_wlan_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <thread>
#include "czmq.h"
#include "../data_types.h"

#pragma  comment(lib,"ws2_32.lib")

std::string remote_server_ip;

void fn_zmq_recv()
{
    std::string msg_type = "";      // 所有消息
    std::string url_recv = ">tcp://" + remote_server_ip + ":" + std::to_string(ZMQ_SERVER_SEND_PORT);
    std::string url_send = ">tcp://" + remote_server_ip + ":" + std::to_string(ZMQ_SERVER_RECV_PORT);

    //std::string url_recv = "tcp://127.0.0.1:8012";
    zsock_t* sock_sub = zsock_new_sub(url_recv.c_str(), ZMQ_MSG_TYP);
    if (sock_sub == nullptr)
    {
        int err = zmq_errno();
        printf("socket failed: %s\n", zmq_strerror(err));
        return;
    }

    zsock_t* sock_send = zsock_new_pub(url_send.c_str(), ZMQ_MSG_TYP);
    if (sock_sub == nullptr)
    {
        int err = zmq_errno();
        printf("socket failed: %s\n", zmq_strerror(err));
        return;
    }

    zsock_set_rcvtimeo(sock_sub, 1000);

    printf("remote url: %s\n", url_recv.c_str());
    
    while (true)
    {
        char* cmd = nullptr;
        char* id = nullptr;
        char* data = nullptr;
        size_t len = 0;
        int rc = zsock_recv(sock_sub, ZMQ_MSG_FMT, &cmd, &id, &data, &len);
        if (rc == -1)
        {
            continue;
        }

        printf("[recv] cmd: %s, id: %s, data len: %d\n", cmd, id, len);

        LARGE_INTEGER __cpuFreq, time_stamp;
        QueryPerformanceFrequency(&__cpuFreq);
        QueryPerformanceCounter(&time_stamp);

        sMessage* msg = (sMessage*)data;
        msg->recv_time_stamp = time_stamp.QuadPart;
        msg->recv_cpu_freq = __cpuFreq.QuadPart;

        // 发送
        zsock_send(sock_send, ZMQ_MSG_FMT, cmd, id, data, len);

        zstr_free(&cmd);
        zstr_free(&id);
        zstr_free(&data);
    }
    zsock_destroy(&sock_sub);
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

    printf("Client app\n");
    std::thread zmq_r(fn_zmq_recv);

    getchar();

    return 0;
}

