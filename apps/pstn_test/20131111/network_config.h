#ifndef _NETWORK_CONFIG_H_
#define _NETWORK_CONFIG_H_

#include "common_tools.h"
#include "internetwork_communication.h"
#include "terminal_register.h"

#define READONLY_ROOTFS 1
#define USER_REGISTER 0

// pad和6410交互包结构
struct s_pad_and_6410_msg
{
    char head[2];                  // 数据头
    unsigned short len;            // 数据长度 = 命令字长度 + 有效数据长度
    unsigned char cmd;             // 命令字
    char *data;                    // 有效数据 
    char check;                    // 校验位
};

// 6410和5350交互包结构
struct s_6410_and_5350_msg
{
    char head[2];                  // 数据头
    unsigned short len;            // 数据长度 = 有效数据长度（两个字节低位在前）
    char *data;                    // 有效数据 
    char check;                    // 校验位
};

// 命令结构体
struct s_cmd
{
    unsigned char cmd_word;        // 命令字
    unsigned char cmd_bit;         // 命令更改标志位
    char cmd_key[32];              // 命令关键字
    char set_cmd[64];              // 设置命令
    char set_value[64];            // 设置命令值
    char set_cmd_and_value[128];   // 设置命令 + 设置命令值
    char get_cmd[64];              // 查询命令
};

struct class_network_config
{
    int *serial_pad_fd;
    int *serial_5350_fd;
    int *server_pad_fd;
    int *server_base_fd;
    char *network_flag; // 是否已经注册成功
    char *cmd;
    char *pad_cmd;
    volatile char pthread_flag;
    
    #if BOARDTYPE == 5350
    void (* init_cmd_list)();
    #endif
    int (* init_env)();
    int (* send_msg_to_pad)(int fd, char cmd, char *data);
    int (* recv_msg_from_pad)(int fd, struct s_pad_and_6410_msg *a_pad_and_6410_msg);
    int (* network_settings)(int fd, int cmd_count, char cmd_word);
    
    #if BOARDTYPE == 6410 
    int (* send_msg_to_5350)(int fd, char *cmd, unsigned short len);
    int (* recv_msg_from_5350)(int fd, struct s_6410_and_5350_msg *a_6410_and_5350_msg);
    int (* get_pppoe_state)(int fd);
//    int (* save_current_route_state)(int fd, char *route_state_buf, unsigned short *route_state_buf_len);
//    int (* recovery_route)(int fd, char *buf, unsigned short buf_len);
//    int (* check_route_config)(int fd, char *route_state_buf, unsigned short route_state_buf_len, char index);
    int (* route_config)(int fd, int index);
    
    #elif BOARDTYPE == 5350
    int (* get_wan_state)();
    int (* get_pppoe_state)();
//    int (* save_current_route_state)()
//    int (* recovery_route)(unsigned char cmd_count)
//    int (* check_route_config)(unsigned char cmd_count)
//    int (* get_pid_by_process_name)(char *process_name)
    int (* config_route_take_effect)();
    int (* route_config)(int index);
    int (* route_config2)(int index);
    #elif BOARDTYPE == 9344
    int (* get_wan_state)();
    int (* config_route_take_effect)();
    int (* route_config)(int index);
    int (* route_config2)(int index);
    #endif
    
    char base_sn[35];
    char base_mac[18];
    char base_ip[16];
    struct s_cmd cmd_list[37];
};

extern struct class_network_config network_config;
#endif
