// zmq_wlan_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <thread>
//#include <Windows.h>   // LARGE_INTEGER, QueryPerformanceFrequency
#include "czmq.h"
#include "../data_types.h"

#pragma  comment(lib,"ws2_32.lib")


void fn_zmq_server_pub()
{
    std::string url_send = "@tcp://*:" + std::to_string(ZMQ_SERVER_SEND_PORT);
    zsock_t* sock_pub = zsock_new_pub(url_send.c_str());
    if (sock_pub == nullptr)
    {
        int err = zmq_errno();
        printf("socket failed: %s\n", zmq_strerror(err));
        return;
    }
    
    std::string cmd = "DATA.SDK";
    std::string id = "abc";

    printf("publish listenning url: %s\n", url_send.c_str());

    int frm_index = 0;
    while (true)
    {
        frm_index++;
        id = std::to_string(frm_index);

        sMessage msg;
        memset(&msg, 0, sizeof(msg));
        snprintf(msg.msg, sizeof(msg.msg), "[%d] Hello from zmq server.");

        // 上送cpu时间戳
        LARGE_INTEGER time_stamp;
        QueryPerformanceCounter(&time_stamp);
        msg.time_stamp = time_stamp.QuadPart;

        // 发送
        zsock_send(sock_pub, ZMQ_MSG_FMT, cmd.c_str(), id.c_str(), &msg, sizeof(msg));
        printf("[pub]  cmd: %s, id: %s, data len: %d\n", cmd.c_str(), id.c_str(), sizeof(msg));

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    zsock_destroy(&sock_pub);
}

void fn_zmq_server_recv()
{
    std::string msg_type = "";      // 所有消息
    std::string url_recv = "@tcp://*:" + std::to_string(ZMQ_SERVER_RECV_PORT);
    zsock_t* sock_sub = zsock_new_sub(url_recv.c_str(), ZMQ_MSG_TYP);
    if (sock_sub == nullptr)
    {
        int err = zmq_errno();
        printf("socket failed: %s\n", zmq_strerror(err));
        return;
    }

    printf("receive listenning url: %s\n", url_recv.c_str());

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

        LARGE_INTEGER __cpuFreq, time_stamp;
        QueryPerformanceFrequency(&__cpuFreq);
        QueryPerformanceCounter(&time_stamp);
        sMessage* msg = (sMessage*)data;
        double latency = (time_stamp.QuadPart - msg->time_stamp) * 1000.0 / __cpuFreq.QuadPart; // ms

        //printf("[recv] cmd: %s, id: %s, eclipsed: %0.2f ms, [client CPU freq: %jd, time stamp: %jd]\n", cmd, id, latency, msg->recv_cpu_freq, msg->recv_time_stamp);
        printf("[recv] cmd: %s, id: %s, eclipsed: %0.2f ms\n", cmd, id, latency);

        zstr_free(&cmd);
        zstr_free(&id);
        zstr_free(&data);
    }
    zsock_destroy(&sock_sub);
}

int main()
{
    printf("ZMQ server app\n");
    std::thread zmq_p(fn_zmq_server_pub);
    std::thread zmq_r(fn_zmq_server_recv);

    getchar();

    return 0;
}

