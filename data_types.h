#ifndef DATA_TYPES_H_
#define DATA_TYPES_H_
#include <stdint.h>

#define ZMQ_SERVER_SEND_PORT    8011                // server send port
#define ZMQ_SERVER_RECV_PORT    8012                // server receive port
#define SOCK_SERVER_LISTEN_PORT 8013                // socket server listen port
#define ZMQ_MSG_FMT             "ssb"               // zmq message format
#define ZMQ_MSG_TYP             ""                  // receive all type of message

#pragma pack(push, 1)

struct sMessage
{
    int64_t time_stamp;      // ���Ͷ�CPUʱ��
    int64_t recv_time_stamp; // ���ն�CPUʱ��
    int64_t recv_cpu_freq;   // ���ն�CPUƵ��
    char msg[100];
};

#pragma pack(pop)



#endif