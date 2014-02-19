/*************************************************************************
    > File Name: request_cmd.c
    > Author: yangjilong
    > mail1: yangjilong65@163.com
    > mail2: yang.jilong@handaer.com
    > Created Time: 2014年01月18日 星期六 18时00分31秒
**************************************************************************/
#include "request_cmd.h"
static time_t start_time;
static unsigned short dev_count = 0;
void *dev_info = NULL;

/**
 * 初始化请求命令
 */
static int init();

/**
 * 请求命令字分析 网络设置 和 注册
 */
static int request_cmd_0x01_02_03_07_08_09_0A(struct s_terminal_dev_register * terminal_dev_register);

/**
 * 请求命令字分析 0x04 询问
 */
static int request_cmd_0x04(struct s_terminal_dev_register * terminal_dev_register);

/**
 * 请求命令字分析 0x05: 5350 生成默认SSID; 9344 注册命令，PAD将随机生成的4字节串码发给base
 */
static int request_cmd_0x05(struct s_terminal_dev_register * terminal_dev_register);

/**
 * 请求命令字分析 0x06 5350时 注销默认SSID；9344时 注册命令，智能设备将 设备名称、id、mac发送给BASE
 */
static int request_cmd_0x06(struct s_terminal_dev_register * terminal_dev_register);

/**
 * 请求命令字分析 0x0B 网络设置询问
 */
static int request_cmd_0x0B(struct s_terminal_dev_register * terminal_dev_register);

/**
 * 请求命令字分析 0x0C 恢复出厂
 */
static int request_cmd_0x0C(struct s_terminal_dev_register * terminal_dev_register);

/**
 * 请求命令字分析 0x0D 查看Base版本信息
 */
static int request_cmd_0x0D(struct s_terminal_dev_register * terminal_dev_register);

#if CTSI_SECURITY_SCHEME == 2
/**
 * 请求命令字分析 0x0E 获取身份令牌
 */
static int request_cmd_0x0E(struct s_terminal_dev_register * terminal_dev_register);

/**
 * 请求命令字分析 0x0F 获取位置令牌
 */
static int request_cmd_0x0F(struct s_terminal_dev_register * terminal_dev_register);

/**
 * 请求命令字分析 0x50 重新生成身份令牌
 */
static int request_cmd_0x50(struct s_terminal_dev_register * terminal_dev_register);

/**
 * 请求命令字分析 0x51 重新生成位置令牌
 */
static int request_cmd_0x51(struct s_terminal_dev_register * terminal_dev_register);
#endif // CTSI_SECURITY_SCHEME == 2

/**
 * 请求命令字分析 0x52 查看WAN口
 */
static int request_cmd_0x52(struct s_terminal_dev_register * terminal_dev_register);

/**
 * 请求命令字分析 0x53 取消当前操作
 */
static int request_cmd_0x53(struct s_terminal_dev_register * terminal_dev_register);

#if BOARDTYPE == 9344
/**
 * 请求命令字分析 0x54 串码对比，智能设备将输入的串码发送到base，base进行对比
 */
static int request_cmd_0x54(struct s_terminal_dev_register * terminal_dev_register);

/**
 * 请求命令字分析 0x55 PAD扫描“发送注册申请”的设备
 */
static int request_cmd_0x55(struct s_terminal_dev_register * terminal_dev_register);

/**
 * 请求命令字分析 0x56 查询已经注册的设备
 */
static int request_cmd_0x56(struct s_terminal_dev_register * terminal_dev_register);

/**
 * 请求命令字分析 0x57 注销命令，删除匹配序列号的行
 */
static int request_cmd_0x57(struct s_terminal_dev_register * terminal_dev_register);

#endif // BOARDTYPE == 9344

/**
 * 请求命令字分析
 */
static int request_cmd_analyse(struct s_terminal_dev_register * terminal_dev_register);

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/**
 * 获取注册状态
 */
static int get_register_state();

/**
 * 初始化化表
 */
static int init_data_table(struct s_data_table *data_table);

#if BOARDTYPE == 9344
/**
 * 取消mac地址绑定
 */
static int cancel_mac_and_ip_bind();

/**
 * 获取BASE中存储的信息给PAD
 */
static int get_success_buf(char *buf);

/**
 * 判断此设备是否在此表中
 * 0:不存在
 * 1:存在
 * < 0:错误
 */
static int is_in_dev_table(unsigned short dev_count, void *dev_info, struct s_dev_register *dev_register);

/**
 * 添加指定行 （内存）
 */
static int add_dev_info(unsigned short *dev_count, void *dev_info, struct s_dev_register *dev_register);

/**
 * 更新指定行串码和串码时间
 */
static int update_dev_info_code(unsigned short dev_count, void *dev_info, struct s_dev_register *dev_register);

/**
 * 删除指定行（内存）
 */
static int delete_dev_info(unsigned short *dev_count, void *dev_info, struct s_dev_register *dev_register);

/**
 * 查询内存表（内存）
 */
static int select_dev_info(unsigned short dev_count, void *dev_info, char *buf, unsigned short len);

/**
 * 串码对比
 */
static int dev_code_comparison(unsigned short dev_count, void *dev_info, struct s_dev_register *dev_register);

/**
 * 添加指定行 （数据库）
 */
static int database_insert_dev_info(struct s_dev_register *dev_register);

/**
 * 删除指定行 （数据库）
 */
static int database_delete_dev_info(struct s_dev_register *dev_register);

/**
 * 查询注册成功的设备
 */
static int database_select_dev_info(char *buf, unsigned short len);

#endif // BOARDTYPE == 9344
/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/


struct class_request_cmd request_cmd = 
{
    .init = init,
    .request_cmd_0x01_02_03_07_08_09_0A = request_cmd_0x01_02_03_07_08_09_0A,
    
    .request_cmd_0x04 = request_cmd_0x04,
    .request_cmd_0x05 = request_cmd_0x05,
    .request_cmd_0x06 = request_cmd_0x06,
    .request_cmd_0x0B = request_cmd_0x0B,
    .request_cmd_0x0C = request_cmd_0x0C,
    .request_cmd_0x0D = request_cmd_0x0D,
    
    #if CTSI_SECURITY_SCHEME == 2
    .request_cmd_0x0E = request_cmd_0x0E,
    .request_cmd_0x0F = request_cmd_0x0F,
    .request_cmd_0x50 = request_cmd_0x50,
    .request_cmd_0x51 = request_cmd_0x51,
    #endif // CTSI_SECURITY_SCHEME == 2
    
    #if BOARDTYPE == 9344
    .request_cmd_0x52 = request_cmd_0x52,
    .request_cmd_0x53 = request_cmd_0x53,
    .request_cmd_0x54 = request_cmd_0x54,
    .request_cmd_0x55 = request_cmd_0x55,
    .request_cmd_0x56 = request_cmd_0x56,
    .request_cmd_0x57 = request_cmd_0x57,
    #endif // BOARDTYPE == 9344
    
    .request_cmd_analyse = request_cmd_analyse,
    .init_data_table = init_data_table,
    
    #if BOARDTYPE == 9344
    .cancel_mac_and_ip_bind = cancel_mac_and_ip_bind,
    #endif // BOARDTYPE == 9344
};

/**
 * 初始化请求命令
 */
int init()
{
    common_tools.work_sum->work_sum = 1;
    start_time = time(0);
return 0;
}

/**
 * 请求命令字分析 网络设置 和 注册
 */
int request_cmd_0x01_02_03_07_08_09_0A(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;
    unsigned char cmd = (unsigned char)terminal_dev_register->cmd_word;
    unsigned char register_state = (unsigned char)terminal_dev_register->data_table.register_state;
    
    PRINT("cmd = %02X, (unsigned char)terminal_dev_register->cmd_word = %02X\n", cmd, (unsigned char)terminal_dev_register->cmd_word);
    if ((cmd == 0x07) || (cmd == 0x08) || (cmd == 0x09) || (cmd == 0x0A)) // 无线网络设置和网络设置
    {
        if (register_state != 0x00)
        {
            res = NO_INIT_ERR;
            OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "no init!", res);
            return res;
        }
        *network_config.network_flag = 1;
        PRINT("*network_config.network_flag = %d\n", *network_config.network_flag);
    }

    if ((res = network_config.send_msg_to_pad(fd, 0x00, NULL, 0)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }

    pthread_mutex_lock(&terminal_dev_register->mutex);

    pthread_cond_signal(&terminal_dev_register->cond);

    pthread_mutex_unlock(&terminal_dev_register->mutex);


    pthread_mutex_lock(&terminal_dev_register->mutex);
    terminal_dev_register->mutex_lock_flag = 1;
    pthread_cond_wait(&terminal_dev_register->cond, &terminal_dev_register->mutex);

    if (terminal_dev_register->pad_cmd_handle_id == 0)
    {
        // 线程创建失败
        res = PTHREAD_CREAT_ERR;
    }
    pthread_mutex_unlock(&terminal_dev_register->mutex);

    return res;
}

/**
 * 请求命令字分析 0x04 询问
 */
int request_cmd_0x04(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0, ret = 0;
    int fd = terminal_dev_register->fd;
    unsigned char register_state = (unsigned char)terminal_dev_register->data_table.register_state;

    struct s_pad_and_6410_msg pad_and_6410_msg;
    memset(&pad_and_6410_msg, 0, sizeof(struct s_pad_and_6410_msg));
    pad_and_6410_msg.head[0] = 0x5A;
    pad_and_6410_msg.head[1] = 0xA5;

    int i = 0;
    char *buf = NULL;
    char buf_tmp[256] = {0}; 
    char pad_mac[18] = {0};
    
    char columns_name[1][30] = {0};
    char columns_value[1][100] = {0};
    unsigned short insert_len = 0;
    
    // 说明此时正在设置
    if (terminal_dev_register->config_now_flag == 1)
    {
        register_state = 0xFA;
    }

    switch (register_state)
    {
        case 0xFA: // 说明此时正在设置
        {
            pthread_mutex_lock(&network_config.recv_mutex);
            break;
        }
        case 0xFB:
        {
             // 此时需查看pad_mac是否为空
            if (strlen(terminal_dev_register->data_table.pad_mac) != 12)
            {
                // 获取10.10.10.100的mac
                strcpy(buf_tmp, "arping -c 1 -w 1 -I br0 10.10.10.100 | grep '\\[' | awk '{print $5}'");
                if ((res = common_tools.get_cmd_out(buf_tmp, pad_mac, sizeof(pad_mac))) < 0)
                {
                    OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "get_cmd_out failed!", res);
                    break;
                }
                memset(buf_tmp, 0, sizeof(buf_tmp));

                if (strlen(pad_mac) > 0)
                {
                    memcpy(buf_tmp, pad_mac + 1, strlen(pad_mac) - 2);
                    memset(pad_mac, 0, sizeof(pad_mac));
                    memcpy(pad_mac, buf_tmp, strlen(buf_tmp));
                    memset(buf_tmp, 0, sizeof(buf_tmp));
                    
                    PRINT("pad_mac = %s\n", pad_mac);
                    // 转换成正常格式MAC地址 00:AA:11:BB:CC:DD
                    if ((res = common_tools.mac_format_conversion(pad_mac)) < 0)
                    {
                        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "mac_format_conversion failed", res);
                        break;
                    }
                    common_tools.mac_del_colon(pad_mac);
                    PRINT("pad_mac = %s, terminal_dev_register->data_table.pad_mac = %s\n", pad_mac, terminal_dev_register->data_table.pad_mac);
                    if (memcmp(terminal_dev_register->data_table.pad_mac, pad_mac, strlen(terminal_dev_register->data_table.pad_mac)) == 0)
                    {
                        *network_config.pad_cmd = 0xFD;
                        *network_config.cmd = 0xFD;
                        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "Local area network normal!", 0);
                    }
                }
            }
            if ((unsigned char)*network_config.pad_cmd != 0xFD)
            {
                register_state = 0xFB;
                break;
            }
        }
        case 0xFD:
        {
            #if CHECK_WAN_STATE == 1

            #if 1
            if ((res = common_tools.get_network_state(common_tools.config->terminal_server_ip, 1, 1)) < 0)
            #else
            if ((res = common_tools.get_network_state(common_tools.config->wan_check_name, 1, 1)) < 0)
            #endif
            {
                OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "get_network_state failed!", res);
            }
            else
            {
                *network_config.pad_cmd = 0xFE;
                *network_config.cmd = 0xFE;
                OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "Wide network normal!", 0);
            }

            #else // 不检测WAN口，直接置为0xFE，代表注册没有进行
            *network_config.pad_cmd = 0xFE;
            *network_config.cmd = 0xFE;
            #endif

            if ((unsigned char)*network_config.pad_cmd != 0xFE)
            {
                register_state = 0xFD;
                break;
            }
        }
        case 0xFE:
        {
            #if USER_REGISTER == 0
            *network_config.pad_cmd = 0x00;
            *network_config.cmd = 0x00;
            #else
            if ((res = terminal_register.user_register(terminal_dev_register->data_table.pad_sn, terminal_dev_register->data_table.pad_mac)) < 0)
            {
                OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "user_register failed!", res);
                break;
            }

            memset(columns_name[0], 0, sizeof(columns_name[0]));
            memset(columns_value[0], 0, sizeof(columns_value[0]));
            memcpy(columns_name[0], "register_state", strlen("register_state"));

            sprintf(columns_value[0], "%d", (unsigned char)*network_config.pad_cmd);
            buf_len = strlen(columns_value[0]);
            terminal_dev_register->data_table = *network_config.pad_cmd;
            for (i = 0; i < common_tools.config->repeat; i++)
            {
                #if BOARDTYPE == 6410 || BOARDTYPE == 9344
                if ((res = database_management.update(1, columns_name, columns_value, &buf_len)) < 0)
                {
                    OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "sqlite3_update failed!", res);
                    continue;
                }
                #elif BOARDTYPE == 5350
                if ((res = nvram_interface.insert(RT5350_FREE_SPACE, 1, columns_name, columns_value, &buf_len)) < 0)
                {
                    OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "nvram_insert failed!", res);
                    continue;
                }
                #endif
                break;
            }

            if (res < 0)
            {
                break;
            }
            memset(network_config.base_sn, 0, sizeof(network_config.base_sn));
            memset(network_config.base_mac, 0, sizeof(network_config.base_mac));
            memset(network_config.base_ip, 0, sizeof(network_config.base_ip));
            memcpy(network_config.base_sn, columns_name[2], strlen(columns_name[2]));
            memcpy(network_config.base_mac, columns_name[3], strlen(columns_name[3]));
            memcpy(network_config.base_ip, columns_name[4], strlen(columns_name[4]));

            len = strlen("network_config.base_sn:") + strlen("network_config.base_mac:") + strlen("network_config.base_ip:") +
                strlen(network_config.base_sn) + strlen(network_config.base_mac) + strlen(network_config.base_ip) +
                strlen(terminal_register.respond_pack->base_user_name) + strlen(terminal_register.respond_pack->base_password) +
                strlen(terminal_register.respond_pack->pad_user_name) + strlen(terminal_register.respond_pack->pad_password) +
                strlen(terminal_register.respond_pack->sip_ip_address) + strlen(terminal_register.respond_pack->sip_port) +
                strlen(terminal_register.respond_pack->heart_beat_cycle) + strlen(terminal_register.respond_pack->business_cycle) + 12;
            PRINT("len = %d\n", len);

            if ((buf = (char *)malloc(len)) == NULL)
            {
                res = MALLOC_ERR;
                OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "malloc failed", res);
                break;
            }

            memset(buf, 0, len);
            sprintf(buf, "network_config.base_sn:%s,network_config.base_mac:%s,network_config.base_ip:%s,%s,%s,%s,%s,%s,%s,%s,%s,", network_config.base_sn, network_config.base_mac, network_config.base_ip,
                terminal_register.respond_pack->base_user_name, terminal_register.respond_pack->base_password,
                terminal_register.respond_pack->pad_user_name, terminal_register.respond_pack->pad_password,
                terminal_register.respond_pack->sip_ip_address, terminal_register.respond_pack->sip_port,
                terminal_register.respond_pack->heart_beat_cycle, terminal_register.respond_pack->business_cycle);

    	    PRINT("%s\n", buf);
            if ((res = network_config.send_msg_to_pad(fd, 0x00, buf, strlen(buf))) < 0)
            {
                OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
                break;
            }
            #endif

            if (*network_config.pad_cmd != 0x00)
            {
                register_state = 0xFE;
                break;
            }
        }
        case 0x00: // 当已经初始化成功时，PAD询问，BASE把SSID等信息发送到PAD
        {
            #if BOARDTYPE == 9344
            // 打包
            if ((res = get_success_buf(buf)) < 0)
            {
                OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "get_success_buf failed", res);
            }
            #endif
            register_state = 0x00;
            break;
        }
        default:
        {
            res = DATA_ERR;
            OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "Data does not match", res);
            break;
        }
    }

    if (register_state == 0xFA)
    {
        pthread_mutex_unlock(&network_config.recv_mutex);
    }

    if (res < 0)
    {
        goto EXIT;
    }

    for (i = 0; i < common_tools.config->repeat; i++)
    {
        if (pad_and_6410_msg.data != NULL)
        {
            free(pad_and_6410_msg.data);
            pad_and_6410_msg.data = NULL;
        }

        // 数据发送到PAD
        if ((res = network_config.send_msg_to_pad(fd, register_state, buf, 0)) < 0)
        {
            OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
            continue;
        }
        if ((res = network_config.recv_msg_from_pad(fd, &pad_and_6410_msg)) < 0)
        {
            OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "recv_msg_from_pad failed", res);
            continue;
        }
        PRINT("pad_and_6410_msg.cmd = %d\n", pad_and_6410_msg.cmd);
        if ((pad_and_6410_msg.data != NULL) || (pad_and_6410_msg.cmd != 0x00))
        {
            res = WRITE_ERR;
            continue;
        }
        break;
    }

EXIT:
    if (pad_and_6410_msg.data != NULL)
    {
        free(pad_and_6410_msg.data);
        pad_and_6410_msg.data = NULL;
    }

    if (buf != NULL)
    {
        free(buf);
        buf = NULL;
    }

    if ((register_state != *network_config.pad_cmd) && (register_state != 0XFA) && ((unsigned char)*network_config.cmd != 0xFF)) // 插入数据库
    {
        memset(columns_name[0], 0, sizeof(columns_name[0]));
        memset(columns_value[0], 0, sizeof(columns_value[0]));
        memcpy(columns_name[0], "register_state", strlen("register_state"));

        sprintf(columns_value[0], "%d", register_state);
        insert_len = strlen(columns_value[0]);
        PRINT("columns_value[0] = %s\n", columns_value[0]);

        for (i = 0; i < common_tools.config->repeat; i++)
        {
            #if BOARDTYPE == 6410 || BOARDTYPE == 9344
            if ((ret = database_management.update(1, columns_name, columns_value, &insert_len)) < 0)
            {
                OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "sqlite3_update failed!", ret);
                continue;
            }
            #elif BOARDTYPE == 5350
            if ((ret = nvram_interface.update(RT5350_FREE_SPACE, 1, columns_name, columns_value, &insert_len)) < 0)
            {
                OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "nvram_update failed!", ret);
                continue;
            }
            #endif
            break;
        }
        if (ret < 0)
        {
            res = ret;
            PRINT("insert failed!\n");
        }
        terminal_dev_register->data_table.register_state = register_state;
    }
    return res;
}

/**
 * 请求命令字分析 0x05: 5350 生成默认SSID; 9344 注册命令，PAD将随机生成的4字节串码发给base
 */
int request_cmd_0x05(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;
    int len = terminal_dev_register->length;
    unsigned char register_state = (unsigned char)terminal_dev_register->data_table.register_state;
#if BOARDTYPE == 9344
	   struct s_dev_register dev_register;
	   memset(&dev_register, 0, sizeof(struct s_dev_register));
#endif
    int i = 0;
    int index = 0;
    if (register_state != 0x00) // PAD没有注册成功
    {
        res = NO_INIT_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "no init!", res);
        return res;
    }

    #if BOARDTYPE == 5350
    // 生成默认SSID
    if ((res = network_config.send_msg_to_pad(fd, 0x00, NULL, 0)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }

    for (i = 0; i < sizeof(network_config.cmd_list) / sizeof(struct s_cmd); i++)
    {
        // 初始化设备项
        switch (network_config.cmd_list[i].cmd_word)
        {
            case 0x46: // SSID3
            {
                index++;
                network_config.cmd_list[i].cmd_bit = index;
                sprintf(network_config.cmd_list[i].set_value, "%s", terminal_dev_register->data_table.default_ssid);
                sprintf(network_config.cmd_list[i].set_cmd_and_value, "%s%s", network_config.cmd_list[i].set_cmd, network_config.cmd_list[i].set_value);
                PRINT("%2d %s %s\n", network_config.cmd_list[i].cmd_bit, network_config.cmd_list[i].set_value, network_config.cmd_list[i].set_cmd_and_value);
                break;
            }
            case 0x47: // SSID3 密码
            {
                index++;
                network_config.cmd_list[i].cmd_bit = index;
                sprintf(network_config.cmd_list[i].set_value, "%s", terminal_dev_register->data_table.default_ssid_password);
                sprintf(network_config.cmd_list[i].set_cmd_and_value, "%s%s", network_config.cmd_list[i].set_cmd, network_config.cmd_list[i].set_value);
                PRINT("%2d %s %s\n", network_config.cmd_list[i].cmd_bit, network_config.cmd_list[i].set_value, network_config.cmd_list[i].set_cmd_and_value);
                break;
            }
            case 0x45:
            {
                index++;
                network_config.cmd_list[i].cmd_bit = index;
                memcpy(network_config.cmd_list[i].set_value, "WPAPSKWPA2PSK;WPAPSKWPA2PSK;WPAPSKWPA2PSK", strlen("WPAPSKWPA2PSK;WPAPSKWPA2PSK;WPAPSKWPA2PSK"));
                sprintf(network_config.cmd_list[i].set_cmd_and_value, "%s%s", network_config.cmd_list[i].set_cmd, network_config.cmd_list[i].set_value);
                PRINT("%2d %s %s\n", network_config.cmd_list[i].cmd_bit, network_config.cmd_list[i].set_value, network_config.cmd_list[i].set_cmd_and_value);
                break;
            }
            case 0x34:
            {
                index++;
                network_config.cmd_list[i].cmd_bit = index;
                memcpy(network_config.cmd_list[i].set_value, "1;0;0", strlen("1;0;0"));
                sprintf(network_config.cmd_list[i].set_cmd_and_value, "%s%s", network_config.cmd_list[i].set_cmd, network_config.cmd_list[i].set_value);
                PRINT("%2d %s %s\n", network_config.cmd_list[i].cmd_bit, network_config.cmd_list[i].set_value, network_config.cmd_list[i].set_cmd_and_value);
                break;
            }
            case 0x35:
            {
                index++;
                network_config.cmd_list[i].cmd_bit = index;
                memcpy(network_config.cmd_list[i].set_value, "AES;AES;AES", strlen("AES;AES;AES"));
                sprintf(network_config.cmd_list[i].set_cmd_and_value, "%s%s", network_config.cmd_list[i].set_cmd, network_config.cmd_list[i].set_value);
                PRINT("%2d %s %s\n", network_config.cmd_list[i].cmd_bit, network_config.cmd_list[i].set_value, network_config.cmd_list[i].set_cmd_and_value);
                break;
            }
            case 0x36:
            {
                index++;
                network_config.cmd_list[i].cmd_bit = index;
                memcpy(network_config.cmd_list[i].set_value, "3600;3600;3600", strlen("3600;3600;3600"));
                sprintf(network_config.cmd_list[i].set_cmd_and_value, "%s%s", network_config.cmd_list[i].set_cmd, network_config.cmd_list[i].set_value);
                PRINT("%2d %s %s\n", network_config.cmd_list[i].cmd_bit, network_config.cmd_list[i].set_value, network_config.cmd_list[i].set_cmd_and_value);
                break;
            }
            case 0x37:
            {
                index++;
                network_config.cmd_list[i].cmd_bit = index;
                memcpy(network_config.cmd_list[i].set_value, "3", strlen("3"));
                sprintf(network_config.cmd_list[i].set_cmd_and_value, "%s%s", network_config.cmd_list[i].set_cmd, network_config.cmd_list[i].set_value);
                PRINT("%2d %s %s\n", network_config.cmd_list[i].cmd_bit, network_config.cmd_list[i].set_value, network_config.cmd_list[i].set_cmd_and_value);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    // 设置路由
    if ((res = network_config.route_config2(index)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "route_config failed!", res);
        return res;
    }

    sleep(1);
    system("reboot");

    #elif BOARDTYPE == 9344
    
    if (len != (34 + 12 + 4)) // 序列号 mac 串码
    {
        res = P_LENGTH_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "length error!", res);
        return res;
    }
    // 注册命令，PAD将随机生成的4字节串码发给base
    memset(&dev_register, 0, sizeof(struct s_dev_register));
    memcpy(&(dev_register.dev_mac), terminal_dev_register->data + 34, 12);
    memcpy(&(dev_register.dev_code), terminal_dev_register->data + 34 + 12, 4);

    if ((res = update_dev_info_code(dev_count, dev_info, &dev_register)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "update_dev_info_code failed!", res);
        return res;
    }

    if ((res = network_config.send_msg_to_pad(fd, 0x00, NULL, 0)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }

    PRINT("after send_msg_to_pad\n");
    #endif
    return res;
}

/**
 * 请求命令字分析 0x06 5350时 注销默认SSID；9344时 注册命令，智能设备将 设备名称、id、mac发送给BASE
 */
int request_cmd_0x06(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;
    int len = terminal_dev_register->length;
    unsigned char register_state = (unsigned char)terminal_dev_register->data_table.register_state;
#if BOARDTYPE == 9344
	struct s_dev_register dev_register;
	memset(&dev_register, 0, sizeof(struct s_dev_register));
#endif
    
    int i = 0;
    int index = 0;
    unsigned short length = 0;
    if (register_state != 0x00) // PAD没有注册成功
    {
        res = NO_INIT_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "no init!", res);
        return res;
    }

#if BOARDTYPE == 5350
    // 注销默认SSID
    if ((res = network_config.send_msg_to_pad(fd, 0x00, NULL, 0)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }

    for (i = 0; i < sizeof(network_config.cmd_list) / sizeof(struct s_cmd); i++)
    {
        // 初始化设备项
        switch (network_config.cmd_list[i].cmd_word)
        {
            case 0x46: // SSID3
            {
                index++;
                network_config.cmd_list[i].cmd_bit = index;
                strcpy(network_config.cmd_list[i].set_value, "\"\"");
                sprintf(network_config.cmd_list[i].set_cmd_and_value, "%s%s", network_config.cmd_list[i].set_cmd, network_config.cmd_list[i].set_value);
                PRINT("%2d %s %s\n", network_config.cmd_list[i].cmd_bit, network_config.cmd_list[i].set_value, network_config.cmd_list[i].set_cmd_and_value);
                break;
            }
            case 0x47: // SSID3 密码
            {
                index++;
                network_config.cmd_list[i].cmd_bit = index;
                strcpy(network_config.cmd_list[i].set_value, "\"\"");
                sprintf(network_config.cmd_list[i].set_cmd_and_value, "%s%s", network_config.cmd_list[i].set_cmd, network_config.cmd_list[i].set_value);
                PRINT("%2d %s %s\n", network_config.cmd_list[i].cmd_bit, network_config.cmd_list[i].set_value, network_config.cmd_list[i].set_cmd_and_value);
                break;
            }
            case 0x45:
            {
                index++;
                network_config.cmd_list[i].cmd_bit = index;
                memcpy(network_config.cmd_list[i].set_value, "WPAPSKWPA2PSK;WPAPSKWPA2PSK", strlen("WPAPSKWPA2PSK;WPAPSKWPA2PSK"));
                sprintf(network_config.cmd_list[i].set_cmd_and_value, "%s%s", network_config.cmd_list[i].set_cmd, network_config.cmd_list[i].set_value);
                PRINT("%2d %s %s\n", network_config.cmd_list[i].cmd_bit, network_config.cmd_list[i].set_value, network_config.cmd_list[i].set_cmd_and_value);
                break;
            }
            case 0x34:
            {
                index++;
                network_config.cmd_list[i].cmd_bit = index;
                memcpy(network_config.cmd_list[i].set_value, "1;0", strlen("1;0"));
                sprintf(network_config.cmd_list[i].set_cmd_and_value, "%s%s", network_config.cmd_list[i].set_cmd, network_config.cmd_list[i].set_value);
                PRINT("%2d %s %s\n", network_config.cmd_list[i].cmd_bit, network_config.cmd_list[i].set_value, network_config.cmd_list[i].set_cmd_and_value);
                break;
            }
            case 0x35:
            {
                index++;
                network_config.cmd_list[i].cmd_bit = index;
                memcpy(network_config.cmd_list[i].set_value, "AES;AES", strlen("AES;AES"));
                sprintf(network_config.cmd_list[i].set_cmd_and_value, "%s%s", network_config.cmd_list[i].set_cmd, network_config.cmd_list[i].set_value);
                PRINT("%2d %s %s\n", network_config.cmd_list[i].cmd_bit, network_config.cmd_list[i].set_value, network_config.cmd_list[i].set_cmd_and_value);
                break;
            }
            case 0x36:
            {
                index++;
                network_config.cmd_list[i].cmd_bit = index;
                memcpy(network_config.cmd_list[i].set_value, "3600;3600", strlen("3600;3600"));
                sprintf(network_config.cmd_list[i].set_cmd_and_value, "%s%s", network_config.cmd_list[i].set_cmd, network_config.cmd_list[i].set_value);
                PRINT("%2d %s %s\n", network_config.cmd_list[i].cmd_bit, network_config.cmd_list[i].set_value, network_config.cmd_list[i].set_cmd_and_value);
                break;
            }
            case 0x37:
            {
                index++;
                network_config.cmd_list[i].cmd_bit = index;
                memcpy(network_config.cmd_list[i].set_value, "2", strlen("2"));
                sprintf(network_config.cmd_list[i].set_cmd_and_value, "%s%s", network_config.cmd_list[i].set_cmd, network_config.cmd_list[i].set_value);
                PRINT("%2d %s %s\n", network_config.cmd_list[i].cmd_bit, network_config.cmd_list[i].set_value, network_config.cmd_list[i].set_cmd_and_value);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    if ((res = network_config.route_config2(index)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "route_config failed!", res);
        return res;
    }
    sleep(1);
	system("reboot");
    #elif BOARDTYPE == 9344
    
    // 注册命令，智能设备将 设备名称、id、mac发送给BASE
    memset(&dev_register, 0, sizeof(struct s_dev_register));
    length = terminal_dev_register->data[index++];
    length += terminal_dev_register->data[index++] << 8;
    PRINT("length = %d %02X\n", length, length);
    if (length > len)
    {
        res = P_DATA_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "data error!", res);
        return res;
    }
    memcpy(dev_register.dev_name, terminal_dev_register->data + index, length);
    index += length;
    
    length = terminal_dev_register->data[index++];
    length += terminal_dev_register->data[index++] << 8;
    PRINT("length = %d %02X\n", length, length);
    if (length != 8)
    {
        res = P_DATA_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "data error!", res);
        return res;
    }
    memcpy(dev_register.dev_id, terminal_dev_register->data + index, length);
    index += length;

    length = terminal_dev_register->data[index++];
    length += terminal_dev_register->data[index++] << 8;
    PRINT("length = %d %02X\n", length, length);
    if (length != 12)
    {
        res = P_DATA_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "data error!", res);
        return res;
    }
    memcpy(dev_register.dev_mac, terminal_dev_register->data + index, length);

    if ((res = add_dev_info(&dev_count, dev_info, &dev_register)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "add_dev_info failed!", res);
        return res;
    }

    if ((res = network_config.send_msg_to_pad(fd, 0x00, NULL, 0)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }
#endif
    return res;
}

/**
 * 请求命令字分析 0x0B 网络设置询问
 */
int request_cmd_0x0B(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;
    unsigned char register_state = (unsigned char)terminal_dev_register->data_table.register_state;
    
    char columns_name[1][30] = {0};
    char columns_value[1][100] = {0};
    
    int i = 0, j = 0;
    int index = 0;
    char buf[256] = {0};
    char send_buf[1024] = {0};
    char config_cmd[128] = {0};
    if (register_state != 0x00) // PAD没有注册成功
    {
        res = NO_INIT_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "no init!", res);
        return res;
    }

    
    #if BOARDTYPE == 5350
    memcpy(columns_name[0], "wanConnectionMode", strlen("wanConnectionMode"));
    if ((res = nvram_interface.select(NVRAM_RT5350, 1, columns_name, columns_value)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "nvram_select failed!", res);
        return res;
    }
    PRINT("columns_value[0] = %s\n", columns_value[0]);
    if ((strcmp("\"\"", columns_value[0]) == 0) || (strlen(columns_value[0]) == 0))
    {
        memset(columns_value[0], 0, sizeof(columns_value[0]));
        sprintf(columns_value[0], "There is no (%s) record!", columns_name[0]);
        res = NULL_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, columns_value[0], res);
        return res;
    }
    memcpy(buf, columns_value[0], strlen(columns_value[0]));
    #elif BOARDTYPE == 9344
    char mode[32] = {0};
    if ((res = common_tools.get_cmd_out("cfg -s | grep WAN_MODE:=", mode, sizeof(mode))) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "get_cmd_out failed!", res);
        return res;
    }
    memcpy(buf, mode + strlen("WAN_MODE:="), strlen(mode) - strlen("WAN_MODE:=") - 1);
    #endif


    PRINT("connect mode = %s\n", buf);
    memset(send_buf, 0, sizeof(send_buf));
    #if BOARDTYPE == 5350
    if (memcmp(buf, "STATIC", strlen("STATIC")) == 0)
    {
        strcpy(send_buf, "mode:STATIC,");
        for (i = 0; i < sizeof(network_config.cmd_list) / sizeof(struct s_cmd); i++)
        {
            //PRINT("network_config.cmd_list[i].cmd_word = %02X, index = %d, i = %d\n", network_config.cmd_list[i].cmd_word, index, i);
            switch (network_config.cmd_list[i].cmd_word)
            {
                case 0x14:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x15:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x16:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x17:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x18:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x1D:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x30:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x31:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x45:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
            }
            if (index == 9)
            {
                break;
            }
        }
    }
    else if (memcmp(buf, "DHCP", strlen("DHCP")) == 0)
    {
        strcpy(send_buf, "mode:DHCP,");
        for (i = 0; i < sizeof(network_config.cmd_list) / sizeof(struct s_cmd); i++)
        {
            switch (network_config.cmd_list[i].cmd_word)
            {
                case 0x19:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x1D:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x30:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x31:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x45:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
            }
            if (index == 5)
            {
                break;
            }
        }
    }
    else if (memcmp(buf, "PPPOE", strlen("PPPOE")) == 0)
    {
        strcpy(send_buf, "mode:PPPOE,");
        for (i = 0; i < sizeof(network_config.cmd_list) / sizeof(struct s_cmd); i++)
        {
            switch (network_config.cmd_list[i].cmd_word)
            {
                case 0x1A:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x1B:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x30:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x31:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x45:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
            }
            if (index == 5)
            {
                break;
            }
        }
    }
    #elif BOARDTYPE == 9344
    if (memcmp(buf, "static", strlen("static")) == 0)
    {
        strcpy(send_buf, "mode:STATIC,");
        for (i = 0; i < sizeof(network_config.cmd_list) / sizeof(struct s_cmd); i++)
        {
            //PRINT("network_config.cmd_list[i].cmd_word = %02X, index = %d, i = %d\n", network_config.cmd_list[i].cmd_word, index, i);
            switch (network_config.cmd_list[i].cmd_word)
            {
                case 0x15:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x16:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x17:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x18:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x1E: // SSID2
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x31: // 密码
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x3D: // 加密
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x40: // 加密
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
            }
            if (index == 8)
            {
                break;
            }
        }
    }
    else if (memcmp(buf, "dhcp", strlen("dhcp")) == 0)
    {
        strcpy(send_buf, "mode:DHCP,");
        for (i = 0; i < sizeof(network_config.cmd_list) / sizeof(struct s_cmd); i++)
        {
            switch (network_config.cmd_list[i].cmd_word)
            {
                case 0x1E: // SSID2
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x31: // 密码
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x3D: // 加密
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x40: // 加密
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
            }
            if (index == 4)
            {
                break;
            }
        }
    }
    else if (memcmp(buf, "pppoe", strlen("pppoe")) == 0)
    {
        strcpy(send_buf, "mode:PPPOE,");
        for (i = 0; i < sizeof(network_config.cmd_list) / sizeof(struct s_cmd); i++)
        {
            switch (network_config.cmd_list[i].cmd_word)
            {
                case 0x1A:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x1B:
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x1E: // SSID2
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x31: // 密码
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x3D: // 加密
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
                case 0x40: // 加密
                {
                    index++;
                    network_config.cmd_list[i].cmd_bit = index;
                    break;
                }
            }
            if (index == 6)
            {
                break;
            }
        }
    }
    #endif
    else
    {
        res = DATA_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "data err", res);
        return res;
    }
    
    char tmp[64] = {0};
    memset(buf, 0, sizeof(buf));
    for (i = 0, j = 1; i < sizeof(network_config.cmd_list) / sizeof(struct s_cmd); i++)
    {
        if (j == network_config.cmd_list[i].cmd_bit)
        {
            PRINT("network_config.cmd_list[%d].cmd_bit = %02X, index = %d, j = %d\n", i, network_config.cmd_list[i].cmd_bit, index, j);
            memset(buf, 0, sizeof(buf));
            memset(tmp, 0, sizeof(tmp));
            
            #if BOARDTYPE == 5350
            memset(buf, 0, sizeof(buf));
            if ((res = nvram_interface.select(NVRAM_RT5350, 1, (char (*)[30])network_config.cmd_list[i].cmd_key, (char (*)[100])buf)) < 0)
            {
                OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "nvram_select failed!", res);
                break;
            }
            if (strlen(buf) == 0)
            {
                strcpy(buf, "\"\"");
            }
            PRINT("buf = %s\n", buf);
            
            #elif BOARDTYPE == 9344
            snprintf(config_cmd, sizeof(config_cmd), "cfg -s | grep %s:=", network_config.cmd_list[i].cmd_key);
            if ((res = common_tools.get_cmd_out(config_cmd, tmp, sizeof(tmp))) < 0)
            {
                OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "get_cmd_out failed!\n", res);
                break;
            }
            memcpy(buf, tmp + 2 + strlen(network_config.cmd_list[i].cmd_key), strlen(tmp) - 2 - strlen(network_config.cmd_list[i].cmd_key) - 1);
            #endif //BOARDTYPE == 9344
            
            PRINT("network_config.cmd_list[%d].cmd_key = %s, buf = %s\n", i, network_config.cmd_list[i].cmd_key, buf);
            switch (network_config.cmd_list[i].cmd_word)
            {
                case 0x15:
                {
                    strcat(send_buf, "ip:");
                    strncat(send_buf, buf, strlen(buf));
                    strcat(send_buf, ",");
                    j++;
                    break;
                }
                case 0x16:
                {
                    strcat(send_buf, "mask:");
                    strncat(send_buf, buf, strlen(buf));
                    strcat(send_buf, ",");
                    j++;
                    break;
                }
                case 0x17:
                {
                    strcat(send_buf, "gateway:");
                    strncat(send_buf, buf, strlen(buf));
                    strcat(send_buf, ",");
                    j++;
                    break;
                }
                case 0x18:
                {
                    strcat(send_buf, "dns1:");
                    strncat(send_buf, buf, strlen(buf));
                    strcat(send_buf, ",");
                    j++;
                    break;
                }
                case 0x1A:
                {
                    strcat(send_buf, "pppoe_user:");
                    strncat(send_buf, buf, strlen(buf));
                    strcat(send_buf, ",");
                    j++;
                    break;
                }
                case 0x1B:
                {
                    strcat(send_buf, "pppoe_pass:");
                    strncat(send_buf, buf, strlen(buf));
                    strcat(send_buf, ",");
                    j++;
                    break;
                }
                case 0x1E:
                {
                    strcat(send_buf, "ssid2:");
                    strncat(send_buf, buf, strlen(buf));
                    strcat(send_buf, ",");
                    j++;
                    break;
                }
                case 0x31:
                {
                    strcat(send_buf, "pass:");
                    strncat(send_buf, buf, strlen(buf));
                    strcat(send_buf, ",");
                    j++;
                    break;
                }
                case 0x3D:
                {
                    strcat(send_buf, "auth:");
                    strncat(send_buf, buf, strlen(buf));
                    j++;
                    break;
                }
                case 0x40:
                {
                    strncat(send_buf, buf, strlen(buf));
                    strcat(send_buf, ",");
                    j++;
                    break;
                }
            }
        }

        if (j > index)
        {
            break;
        }
        else if (i == (sizeof(network_config.cmd_list) / sizeof(struct s_cmd) - 1))
        {
            i = 0;
        }
    }
    PRINT("send_buf = %s\n", send_buf);
    if ((res = network_config.send_msg_to_pad(fd, 0x00, send_buf, strlen(send_buf))) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }
    
    return res;
}

/**
 * 请求命令字分析 0x0C 恢复出厂
 */
int request_cmd_0x0C(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;

    if ((res = network_config.send_msg_to_pad(fd, 0x00, NULL, 0)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }
    sleep(1);

    #if BOARDTYPE == 5350
    system("kill -9 `ps | grep cacm | sed '/grep/'d | awk '{print $1}'`");
    system("nvram_set freespace register_state 251");
    #if SMART_RECOVERY == 1// 路由智能恢复
    system("nvram_set backupspace register_state 251");
    #endif
    system("ralink_init clear 2860");
    system("reboot");
    #elif BOARDTYPE == 9344
    // 1.情况数据库
    if ((res = database_management.clear()) < 0)
    {
        PRINT("sqlite3_clear_table failed!\n");
        return res;
    }
    // 2.取消绑定
    cancel_mac_and_ip_bind();
    // 3.
    system("cfg -x");
    sleep(1);
    system("reboot");
    #endif
    
    return res;
}

/**
 * 请求命令字分析 0x0D 查看Base版本信息
 */
int request_cmd_0x0D(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;
    
    char buf[256] = {0};
    if ((res = common_tools.get_cmd_out("cat /proc/version", buf, sizeof(buf))) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "get_cmd_out failed!", res);
        return res;
    }

    if ((res = network_config.send_msg_to_pad(fd, 0x00, buf, strlen(buf))) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }
    return res;
}

#if CTSI_SECURITY_SCHEME == 2
/**
 * 请求命令字分析 0x0E 获取身份令牌
 */
int request_cmd_0x0E(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;
    unsigned char register_state = (unsigned char)terminal_dev_register->data_table.register_state;
    char device_token[16] = {0};
    
    if (register_state != 0x00) // PAD没有注册成功
    {
        res = NO_INIT_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "no init!", res);
        return res;
    }

    if ((res = terminal_authentication.return_device_token(device_token)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "return_device_token failed!", res);
        return res;
    }

    if ((res = network_config.send_msg_to_pad(fd, 0x00, device_token, 16)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }
    return res;
}
/**
 * 请求命令字分析 0x0F 获取位置令牌
 */
int request_cmd_0x0F(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;
    unsigned char register_state = (unsigned char)terminal_dev_register->data_table.register_state;
    char position_token[16] = {0};
    
    if (register_state != 0x00) // PAD没有注册成功
    {
        res = NO_INIT_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "no init!", res);
        return res;
    }

    if ((res = terminal_authentication.return_position_token(position_token)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "return_position_token failed!", res);
        return res;
    }

    if ((res = network_config.send_msg_to_pad(fd, 0x00, position_token, 16)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }
    return res;
}
/**
 * 请求命令字分析 0x50 重新生成身份令牌
 */
int request_cmd_0x50(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;
    unsigned char register_state = (unsigned char)terminal_dev_register->data_table.register_state;
    char device_token[16] = {0};
    
    if (register_state != 0x00) // PAD没有注册成功
    {
        res = NO_INIT_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "no init!", res);
        return res;
    }

    if ((res = terminal_authentication.rebuild_device_token(device_token)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "rebuild_device_token failed!", res);
        return res;
    }

    if ((res = network_config.send_msg_to_pad(fd, 0x00, device_token, 16)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }
    return res;
}
/**
 * 请求命令字分析 0x51 重新生成位置令牌
 */
int request_cmd_0x51(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;
    unsigned char register_state = (unsigned char)terminal_dev_register->data_table.register_state;
    char position_token[16] = {0};
    
    if (register_state != 0x00) // PAD没有注册成功
    {
        res = NO_INIT_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "no init!", res);
        return res;
    }

    if ((res = terminal_authentication.rebuild_position_token(position_token)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "rebuild_position_token failed!", res);
        return res;
    }

    if ((res = network_config.send_msg_to_pad(fd, 0x00, position_token, 16)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }
    return res;
}
#endif // CTSI_SECURITY_SCHEME == 2

/**
 * 请求命令字分析 0x52 查看WAN口
 */
int request_cmd_0x52(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;
    unsigned char register_state = (unsigned char)terminal_dev_register->data_table.register_state;
    
    if ((res = network_config.get_wan_state()) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "get_wan_state failed!", res);
        return res;
    }
}

/**
 * 请求命令字分析 0x53 取消当前操作
 */
int request_cmd_0x53(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;
    unsigned char register_state = (unsigned char)terminal_dev_register->data_table.register_state;
    
    if (terminal_dev_register->config_now_flag == 1)
    {
        #if BOARDTYPE == 5350
        pthread_mutex_lock(&nvram_interface.mutex);
        #endif
        pthread_cancel(terminal_dev_register->pad_cmd_handle_id);
        PRINT("after pthread_cancel!\n");

        #if BOARDTYPE == 5350
        pthread_mutex_unlock(&nvram_interface.mutex);
        #endif
        // 判断锁状态(需要跟踪)
        if(pthread_mutex_trylock(&network_config.recv_mutex) == EBUSY)
        {
            PRINT("pad_cmd_handle pthread lock now!\n");
            pthread_mutex_unlock(&network_config.recv_mutex);
        }
        else
        {
            PRINT("serial pthread lock now!\n");
            pthread_mutex_unlock(&network_config.recv_mutex);
        }
    }

    /*
    if ((res = network_config.send_msg_to_pad(fd, 0x00, NULL, 0)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;;
    }
    */
    return res;
}

#if BOARDTYPE == 9344
/**
 * 请求命令字分析 0x54 串码对比，智能设备将输入的串码发送到base，base进行对比
 */
int request_cmd_0x54(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;
    unsigned char register_state = (unsigned char)terminal_dev_register->data_table.register_state;
    
    int index = 1;
    unsigned short length = 0;

#if BOARDTYPE == 9344
    struct s_dev_register dev_register;
    memset(&dev_register, 0, sizeof(struct s_dev_register));
#endif

    if (register_state != 0x00) // PAD没有注册成功
    {
        res = NO_INIT_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "no init!", res);
        return res;
    }
    
    memset(&dev_register, 0, sizeof(struct s_dev_register));
    length = terminal_dev_register->data[index++] + terminal_dev_register->data[index++] << 8;
    memcpy(dev_register.dev_mac, terminal_dev_register->data + index, length);
    index += length;
    
    length = terminal_dev_register->data[index++] + terminal_dev_register->data[index++] << 8;
    memcpy(dev_register.dev_id, terminal_dev_register->data + index, length);
    index += length;

    length = terminal_dev_register->data[index++] + terminal_dev_register->data[index++] << 8;
    memcpy(&dev_register.dev_code, terminal_dev_register->data + index, length);
    
    if ((res = dev_code_comparison(dev_count, dev_info, &dev_register)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "dev_code_comparison failed!", res);
        return res;
    }

    if ((res = network_config.send_msg_to_pad(fd, 0x00, NULL, 0)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }

    // 从内存中删除
    if ((res = delete_dev_info(&dev_count, dev_info, &dev_register)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "delete_dev_info failed", res);
        return res;
    }
    
    // 插入数据库
    if ((res = database_insert_dev_info(&dev_register)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "database_insert_dev_info failed", res);
        return res;
    }
    return res;
}
/**
 * 请求命令字分析 0x55 PAD扫描“发送注册申请”的设备
 */
int request_cmd_0x55(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;
    unsigned char register_state = (unsigned char)terminal_dev_register->data_table.register_state;
    
    char buf[1024] = {0};

    if (register_state != 0x00) // PAD没有注册成功
    {
        res = NO_INIT_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "no init!", res);
        return res;
    }
        
    if ((res = select_dev_info(dev_count, dev_info, buf, sizeof(buf))) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "select_dev_info failed!", res);
        return res;
    }

    if ((res = network_config.send_msg_to_pad(fd, 0x00, buf, 0)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }
    return res;
}
/**
 * 请求命令字分析 0x56 查询已经注册的设备
 */
int request_cmd_0x56(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;
    unsigned char register_state = (unsigned char)terminal_dev_register->data_table.register_state;
    
    char buf[1024] = {0};

    if (register_state != 0x00) // PAD没有注册成功
    {
        res = NO_INIT_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "no init!", res);
        return res;
    }
        
    if ((res = database_select_dev_info(buf, sizeof(buf))) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "database_select_dev_info failed!", res);
        return res;
    }

    if ((res = network_config.send_msg_to_pad(fd, 0x00, buf, 0)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }
    return res;
}
/**
 * 请求命令字分析 0x57 注销命令，删除匹配序列号的行
 */
int request_cmd_0x57(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    int fd = terminal_dev_register->fd;
    unsigned char register_state = (unsigned char)terminal_dev_register->data_table.register_state;
    
    char buf[1024] = {0};
#if BOARDTYPE == 9344
    struct s_dev_register dev_register;
    memset(&dev_register, 0, sizeof(struct s_dev_register));
#endif
    
    if (register_state != 0x00) // PAD没有注册成功
    {
        res = NO_INIT_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "no init!", res);
        return res;
    }
    
    memcpy(dev_register.dev_mac, terminal_dev_register->data + 35, 12);
    if ((res = database_delete_dev_info(&dev_register)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "database_delete_dev_info failed!", res);
        return res;
    }

    if ((res = network_config.send_msg_to_pad(fd, 0x00, NULL, 0)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed", res);
        return res;
    }
    return res;
}
#endif // BOARDTYPE == 9344


/**
 * 请求命令字分析
 */
static int request_cmd_analyse(struct s_terminal_dev_register * terminal_dev_register)
{
    int res = 0;
    char *buf = NULL;
    
    struct timeval tpstart, tpend;
    memset(&tpstart, 0, sizeof(struct timeval));
    memset(&tpend, 0, sizeof(struct timeval));

    int fd = terminal_dev_register->fd;
    int mode = terminal_dev_register->mode;
    
    struct s_pad_and_6410_msg pad_and_6410_msg;
    memset(&pad_and_6410_msg, 0, sizeof(struct s_pad_and_6410_msg));
    pad_and_6410_msg.head[0] = 0x5A;
    pad_and_6410_msg.head[1] = 0xA5;
    
    int index = 0;
    unsigned char cmd = 0;
    
    memset(&tpstart, 0, sizeof(struct timeval));
    gettimeofday(&tpstart, NULL); // 得到当前时间
    
    // 接收pad发送的数据
    if ((res = network_config.recv_msg_from_pad(fd, &pad_and_6410_msg)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "recv_msg_from_pad failed", res);
        goto EXIT;
    }
    
    PRINT("pad_and_6410_msg.cmd = %02X, mode = %02X\n", pad_and_6410_msg.cmd, mode);
    switch (mode)
    {
        case 0:
        {
            switch (pad_and_6410_msg.cmd)
            {
                case 0x01:   //0x01: 动态IP + 注册
                case 0x02:   //0x02: 静态IP + 注册
                case 0x03:   //0x03: PPPOE + 注册
                case 0x04:   //0x04: 询问当前设置状态
                case 0x52:   //0x52: 查看WAN口
                case 0x53:   //0x53: 取消当前配置
                {
                    cmd = pad_and_6410_msg.cmd;
                    break;
                }
                default:
                {
                    res = P_DATA_ERR;
                    OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "Data does not match", res);
                    break;
                }
            }
            break;
        }
        case 1:
        {
            switch (pad_and_6410_msg.cmd)
            {
                case 0x01:   //0x01: 动态IP + 注册
                case 0x02:   //0x02: 静态IP + 注册
                case 0x03:   //0x03: PPPOE + 注册
                case 0x04:   //0x04: 询问当前设置状态
                #if BOARDTYPE == 5350
                case 0x05:   //0x05: 生成默认ssid，用于外围设备的接入
                case 0x06:   //0x06: 注销默认ssid
                #elif BOARDTYPE == 9344
                case 0x05:   //0x05: 注册命令，PAD将随机生成的4字节串码发给base
                case 0x06:   //0x06: 注册命令，智能设备将 设备名称、id、mac发送给BASE
                #endif
                case 0x07:   //0x07: 无线网络设置
                case 0x08:   //0x08: 动态IP
                case 0x09:   //0x09: 静态IP
                case 0x0A:   //0x0A: PPPOE
                case 0x0B:   //0x0B: 网络设置询问和无线设置询问
                case 0x0C:   //0x0C: 恢复出厂（终端初始化前的状态）
                case 0x0D:   //0x0D: 查看Base版本
                #if CTSI_SECURITY_SCHEME == 2
                case 0x0E:   //0x0E: 获取身份令牌
                case 0x0F:   //0x0F: 获取位置令牌
                case 0x50:   //0x50: 重新生成身份令牌
                case 0x51:   //0x51: 重新生成身份令牌
                #endif
                case 0x52:   //0x52: 查看WAN口
                case 0x53:   //0x53: 取消当前配置
                #if BOARDTYPE == 9344
                case 0x54:   //0x54：串码对比，智能设备将输入的串码发送到base，base进行对比
                case 0x55:   //0x55：PAD扫描“发送注册申请”的设备
                case 0x56:   //0x56：查询已经注册的设备
                case 0x57:   //0x57：注销命令，删除匹配序列号的行
                #endif
                {
                    cmd = pad_and_6410_msg.cmd;
                    break;
                }
                default:
                {
                    res = P_DATA_ERR;
                    OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "Data does not match", res);
                    break;
                }
            }
            break;
        }
        default:
        {
            res = P_DATA_ERR;
            OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "Data does not match", res);
            break;
        }
    }

    if (res < 0)
    {
        goto EXIT;
    }
    
    if ((res = get_register_state()) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "get_register_state failed!", res);
        return res;
    }
    
    terminal_dev_register->data_table.register_state = (unsigned char)res;
    *network_config.pad_cmd = terminal_dev_register->data_table.register_state;
    PRINT("register_state = %02X, *network_config.pad_cmd = %02X\n", terminal_dev_register->data_table.register_state, (unsigned char)*network_config.pad_cmd);

    // 不是智能设备都要验证 序列号 // 所有都有验证 序列号
    #if BOARDTYPE == 9344
    if ((pad_and_6410_msg.cmd != 0x06) && (pad_and_6410_msg.cmd != 0x54) && (mode == 1)) // 网络通道
    #endif
    {
        switch (pad_and_6410_msg.cmd)
        {
            case 0x01: // 设置命令时
            case 0x02:
            case 0x03:
            case 0x07:
            case 0x08:
            case 0x09:
            case 0x0A:
            {
                index = 1;
                break;
            }
            default:
            {
                index = 0;
                break;
            }
        }

        if (strlen(terminal_dev_register->data_table.pad_sn) != 34)
        {
            res = NO_RECORD_ERR;
            OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "There is no (pad_sn) record!", res);
            goto EXIT;
        }
        PRINT("pad_and_6410_msg.data = %s, terminal_dev_register->data_table.pad_sn = %s\n", pad_and_6410_msg.data + index, terminal_dev_register->data_table.pad_sn);

        if (memcmp(terminal_dev_register->data_table.pad_sn, pad_and_6410_msg.data + index, strlen(terminal_dev_register->data_table.pad_sn)) != 0)
        {
            // 要记录一个状态，用于确定是否要恢复出厂
            if ((unsigned char)*network_config.pad_cmd == 0xFC) //广域网设置成功，由于pad和base不是合法设备
            {
                *network_config.cmd = 0xFB;
                *network_config.pad_cmd = 0xFB;
                OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "new pad!", 0);
            }
            else
            {
                res = P_DATA_ERR;
                OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "data error!", res);
                goto EXIT;
            }
        }
    }
    PRINT("pad_and_6410_msg.len = %d\n", pad_and_6410_msg.len);
    terminal_dev_register->length = pad_and_6410_msg.len - 1;
    memset(terminal_dev_register->data, 0, sizeof(terminal_dev_register->data));
    memcpy(terminal_dev_register->data, pad_and_6410_msg.data, pad_and_6410_msg.len);
    
    // 判断命令字
    switch (cmd)
    {
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x07:
        case 0x08:
        case 0x09:
        case 0x0A:
        {
            terminal_dev_register->cmd_word = cmd;
            terminal_dev_register->cmd_count = pad_and_6410_msg.data[0];
            res = request_cmd_0x01_02_03_07_08_09_0A(terminal_dev_register);
            break;
        }
        case 0x04:
        {
            res = request_cmd_0x04(terminal_dev_register);
            break;
        }
        case 0x05:
        {
            res = request_cmd_0x05(terminal_dev_register);
            break;
        }
        case 0x06:
        {
            res = request_cmd_0x06(terminal_dev_register);
            break;
        }
        case 0x0B:
        {
            res = request_cmd_0x0B(terminal_dev_register);
            break;
        }
        case 0x0C:
        {
            res = request_cmd_0x0C(terminal_dev_register);
            break;
        }
        case 0x0D:
        {
            res = request_cmd_0x0D(terminal_dev_register);
            break;
        }
        case 0x0E:
        {
            res = request_cmd_0x0E(terminal_dev_register);
            break;
        }
        case 0x0F:
        {
            res = request_cmd_0x0F(terminal_dev_register);
            break;
        }
        case 0x50:
        {
            res = request_cmd_0x50(terminal_dev_register);
            break;
        }
        case 0x51:
        {
            res = request_cmd_0x51(terminal_dev_register);
            break;
        }
        case 0x52:
        {
            res = request_cmd_0x52(terminal_dev_register);
            break;
        }
        case 0x53:
        {
            res = request_cmd_0x53(terminal_dev_register);
            break;
        }
        #if BOARDTYPE == 9344
        case 0x54:
        {
            res = request_cmd_0x54(terminal_dev_register);
            break;
        }
        case 0x55:
        {
            res = request_cmd_0x55(terminal_dev_register);
            break;
        }
        case 0x56:
        {
            res = request_cmd_0x56(terminal_dev_register);
            break;
        }
        case 0x57:
        {
            res = request_cmd_0x57(terminal_dev_register);
            break;
        }
        #endif // BOARDTYPE == 9344
        default:
        {
            res = P_DATA_ERR;
            OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "Data does not match", res);
            break;
        }
    }

EXIT:
    // 等于1时说明此时正在设置
    if ((terminal_dev_register->config_now_flag != 1) || (cmd == 0x53))
    {
        network_config.init_cmd_list();  // 初始化命令结构体
    }
    
    PRINT("res = %d\n", res);
    if (res < 0)
    {
        // 获取用户提示
        if (common_tools.get_user_prompt(res, &buf) < 0)
        {
            OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "get_user_prompt failed", res);
            if (buf != NULL)
            {
                free(buf);
                buf = NULL;
            }
        }
        if (buf != NULL)
        {
            PRINT("buf = %s\n", buf);
        }

        if (network_config.send_msg_to_pad(fd, 0xFF, buf, strlen(buf)) < 0)
        {
            OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send_msg_to_pad failed!", res);
        }
        else
        {
            OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "send success!", 0);
        }
        close(fd);
        terminal_dev_register->fd = 0;
    }
    else 
    {
        if (terminal_dev_register->config_now_flag != 1)
        {
            close(fd);
            terminal_dev_register->fd = 0;
        }
    }
    
    if (buf != NULL)
    {
        free(buf);
        buf = NULL;
    }
    if (pad_and_6410_msg.data != NULL)
    {
        free(pad_and_6410_msg.data);
        pad_and_6410_msg.data = NULL;
    }
    
    if (*network_config.server_pad_fd > 0)
    {
        close(*network_config.server_pad_fd);
        *network_config.server_pad_fd = 0;
    }
    
    gettimeofday(&tpend, NULL);
    common_tools.work_sum->work_failed_sum++;
    common_tools.make_menulist(start_time, tpstart, tpend);

    common_tools.work_sum->work_sum++;

    network_config.pthread_flag = 0;
    return 0;
}


/**
 * 获取注册状态
 */
int get_register_state()
{
    int res = 0;
    char column_name[1][30] = {"register_state"};
    char column_value[1][100] = {0};
    
    #if BOARDTYPE == 9344
    if ((res = database_management.select(1, column_name, column_value)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "sqlite3_select failed!", res);
        return res;
    }
    #elif BOARDTYPE == 5350
    if ((res = nvram_interface.select(RT5350_FREE_SPACE, 1, column_name, column_value)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "nvram_select failed!", res);
        return res;
    }
    #endif
    
    res = atoi(column_value[0]);
    PRINT("register_state = %02X\n", (unsigned char)res);
    return (unsigned char)res;
}
 
/**
 * 初始化化表
 */
int init_data_table(struct s_data_table *data_table)
{
    if (data_table == NULL)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "data_table is NULL!", NULL_ERR);
        return NULL_ERR;
    }
    memset(data_table, 0, sizeof(struct s_data_table));
    
    int res = 0;
    char columns_name[4][30] = {"base_sn", "base_mac", "pad_sn", "pad_mac"};
    char columns_value[4][100] = {0};

    #if BOARDTYPE == 9344
    if ((res = database_management.select(4, columns_name, columns_value)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "sqlite3_select failed!", res);
        return res;
    }
    #elif BOARDTYPE == 5350
    if ((res = nvram_interface.select(RT5350_FREE_SPACE, 4, columns_name, columns_value)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "nvram_select failed!", res);
        return res;
    }
    #endif

    memcpy(data_table->base_sn, columns_value[0], sizeof(data_table->base_sn) - 1);
    memcpy(data_table->base_mac, columns_value[1], sizeof(data_table->base_mac) - 1);
    memcpy(data_table->pad_sn, columns_value[2], sizeof(data_table->pad_sn) - 1);
    memcpy(data_table->pad_mac, columns_value[3], sizeof(data_table->pad_mac) - 1);
    if ((res = get_register_state()) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "get_register_state failed!", res);
        return res;
    }
    data_table->register_state = (unsigned char)res;
    
    #if BOARDTYPE == 5350
    strcpy(data_table->default_ssid, "handaer_wifi_register");
    strcpy(data_table->default_ssid_password, "12345678");
    #endif
    PRINT("data_table->base_sn = %s\n", data_table->base_sn);
    PRINT("data_table->base_mac = %s\n", data_table->base_mac);
    PRINT("data_table->pad_sn = %s\n", data_table->pad_sn);
    PRINT("data_table->pad_mac = %s\n", data_table->pad_mac);
    PRINT("data_table->register_state = %02X\n", data_table->register_state);
    return 0;
}

#if BOARDTYPE == 9344

/**
 * 取消mac地址绑定
 */
int cancel_mac_and_ip_bind()
{
    int res = 0;
    char *cmd = "cfg -s | grep \"ADD_MAC:=\"";
    char mac[64] = {0};
    char buf[128] = {0};

    if ((res = common_tools.get_cmd_out(cmd, mac, sizeof(mac))) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "get_cmd_out failed!", res);
        return res;
    }
    mac[strlen(mac) - 1] = '\0';
    PRINT("mac = %s\n", mac);
    
    if (strlen(mac) == 0)
    {
        PRINT("no bind!\n");
        return 0;
    }
    
    sprintf(buf, "cfg -a DELXXX=\"%s\"", mac + strlen("ADD_MAC:="));
    PRINT("buf = %s\n", buf);
    system(buf);
    system("/usr/sbin/del_addr"); // 取消静态绑定
    return 0;
}

/**
 * 获取BASE中存储的信息给PAD
 */
int get_success_buf(char *buf)
{
    int res = 0;
    int len = 0;
    #if USER_REGISTER
    char columns_name[13][30] = {"base_sn", "base_mac", "base_ip", "base_user_name", "base_password",
        "pad_user_name", "pad_password", "sip_ip", "sip_port", "heart_beat_cycle",
        "business_cycle", "ssid_user_name", "ssid_password"};
    char columns_value[13][100] = {0};

    if ((res = database_management.select(13, columns_name, columns_value)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "sqlite3_select failed!", res);
        return res;
    }

    len = strlen("base_sn:") + strlen("base_mac:") + strlen("base_ip:") +
        strlen("base_user_name:") + strlen("base_password:") + strlen("pad_user_name:") +
        strlen("pad_password:") + strlen("sip_ip:") + strlen("sip_port:") +
        strlen("heart_beat_cycle:") + strlen("business_cycle:") + strlen("ssid_user_name:") +
        strlen("ssid_password:") +
        strlen(columns_value[0]) + strlen(columns_value[1]) + strlen(columns_value[2]) +
        strlen(columns_value[3]) + strlen(columns_value[4]) + strlen(columns_value[5]) +
        strlen(columns_value[6]) + strlen(columns_value[7]) + strlen(columns_value[8]) +
        strlen(columns_value[9]) + strlen(columns_value[10]) + strlen(columns_value[11]) +
        strlen(columns_value[12]) + 14;

    PRINT("len = %d\n", len);
    if ((buf = (char *)malloc(len)) == NULL)
    {
        res = MALLOC_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "malloc failed", res);
        return res;
    }

    memset(buf, 0, len);
    sprintf(buf, "base_sn:%s,base_mac:%s,base_ip:%s,base_user_name:%s,base_password:%s,pad_user_name:%s, \
        pad_password:%s,sip_ip:%s,sip_port:%s,heart_beat_cycle:%s,business_cycle:%s,ssid_user_name:%s,ssid_password:%s",
        columns_value[0], columns_value[1], columns_value[2], columns_value[3], columns_value[4], columns_value[5],
        columns_value[6], columns_value[7], columns_value[8], columns_value[9], columns_value[10], columns_value[11],
        columns_value[12]);
    #else

    char columns_name[5][30] = {"base_sn", "base_mac", "base_ip", "ssid_user_name", "ssid_password"};
    char columns_value[5][100] = {0};

    if ((res = database_management.select(3, columns_name, columns_value)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "sqlite3_select failed!", res);
        return res;
    }

    len = strlen("base_sn:") + strlen("base_mac:") + strlen("base_ip:") + strlen("ssid_user_name") + strlen("ssid_password") +
        strlen(columns_value[0]) + strlen(columns_value[1]) + strlen(columns_value[2]) + strlen(columns_value[3]) + strlen(columns_value[4]) + 6;

    PRINT("len = %d\n", len);
    if ((buf = (char *)malloc(len)) == NULL)
    {
        res = MALLOC_ERR;
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "malloc failed", res);
        return res;
    }

    memset(buf, 0, len);
    sprintf(buf, "base_sn:%s,base_mac:%s,base_ip:%s,ssid_user_name:%s,ssid_password:%s,", columns_value[0], columns_value[1], columns_value[2], columns_value[3], columns_value[4]);
    #endif
    PRINT("%s\n", buf);
    return 0;
}

/**
 * 判断此设备是否在此表中
 * 0:不存在
 * 1:存在
 * < 0:错误
 */
int is_in_dev_table(unsigned short dev_count, void *dev_info, struct s_dev_register *dev_register)
{
    if (dev_info == NULL)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "data is NULL!", NULL_ERR);
        return NULL_ERR;
    }
    if (dev_register == NULL)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "data is NULL!", NULL_ERR);
        return NULL_ERR;
    }
    int i = 0;
    struct s_dev_register *dev_tmp;

    for (i = 0; i < dev_count; i++)
    {
        dev_tmp = (struct s_dev_register *)(dev_info + i * sizeof(struct s_dev_register));
        if (memcmp(dev_tmp->dev_mac, dev_register->dev_mac, strlen(dev_register->dev_mac)) == 0)
        {
            PRINT("This dev in table!\n");
            return 1;
        }
    }
    return 0;
}

/**
 * 添加指定行 （内存）
 */
int add_dev_info(unsigned short *dev_count, void *dev_info, struct s_dev_register *dev_register)
{
    int res = 0;
    int malloc_len = sizeof(struct s_dev_register);

    if (dev_register == NULL)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "data is NULL!", NULL_ERR);
        return NULL_ERR;
    }

    if (dev_info == NULL)
    {
        if ((dev_info = malloc(malloc_len)) == NULL)
        {
            OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "malloc failed", MALLOC_ERR);
            return MALLOC_ERR;
        }
    }
    else
    {
        res = is_in_dev_table(*dev_count, dev_info, dev_register);
        PRINT("res = %d\n", res);
        switch (res)
        {
            case 0: // 不存在
            {
                if ((dev_info = realloc(dev_info, malloc_len)) == NULL)
                {
                    OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "realloc failed", MALLOC_ERR);
                    return MALLOC_ERR;
                }
                break;
            }
            case 1: // 已存在
            {
                return 0;
            }
            default:
            {
                return res;
            }
        }
    }
    memcpy(dev_info + *dev_count * malloc_len, dev_register, malloc_len);
    *dev_count++;
    return 0;
}

/**
 * 更新指定行串码和串码时间
 */
int update_dev_info_code(unsigned short dev_count, void *dev_info, struct s_dev_register *dev_register)
{
    if (is_in_dev_table(dev_count, dev_info, dev_register) != 1)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "data error!", DATA_ERR);
        return DATA_ERR;
    }

    int i = 0;
    struct s_dev_register *dev_tmp;
    for (i = 0; i < dev_count; i++)
    {
        dev_tmp = (struct s_dev_register *)(dev_info + i * sizeof(struct s_dev_register));
        if (memcmp(dev_tmp->dev_mac, dev_register->dev_mac, strlen(dev_register->dev_mac)) == 0)
        {
            PRINT("dev_register->dev_code = %08X\n", dev_register->dev_code);
            dev_tmp->dev_code = dev_register->dev_code;
            dev_tmp->end_time = time(NULL) + 1800;  // 有效期30分钟
            return 0;
        }
    }
    return DATA_ERR;
}

/**
 * 删除指定行（内存）
 */
int delete_dev_info(unsigned short *dev_count, void *dev_info, struct s_dev_register *dev_register)
{
    int res = 0;
    res = is_in_dev_table(*dev_count, dev_info, dev_register);
    PRINT("res = %d\n", res);
    switch (res)
    {
        case 0:
        {
            return 0;
            break;
        }
        case 1:
        {
            break;
        }
        default:
        {
            return res;
        }
    }

    int i = 0;
    struct s_dev_register *dev_tmp;
    for (i = 0; i < *dev_count; i++)
    {
        dev_tmp = (struct s_dev_register *)(dev_info + i * sizeof(struct s_dev_register));
        PRINT("dev_tmp->dev_mac = %s\n", dev_tmp->dev_mac);
        if (memcmp(dev_tmp->dev_mac, dev_register->dev_mac, strlen(dev_register->dev_mac)) == 0)
        {
            memcpy(dev_tmp, dev_info + sizeof(struct s_dev_register), *dev_count - i - 1);
            *dev_count--;
            free(dev_info + *dev_count * sizeof(struct s_dev_register));
            return 0;
        }
    }
    return DATA_ERR;
}

/**
 * 查询内存表（内存）
 */
int select_dev_info(unsigned short dev_count, void *dev_info, char *buf, unsigned short len)
{
    if (dev_info == NULL)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "data is NULL!", NULL_ERR);
        return NULL_ERR;
    }
    if (buf == NULL)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "data is NULL!", NULL_ERR);
        return NULL_ERR;
    }

    int i = 0;
    unsigned short buf_len = 0;
    struct s_dev_register *dev_tmp;
    for (i = 0; i < dev_count; i++)
    {
        dev_tmp = (struct s_dev_register *)(dev_info + i * sizeof(struct s_dev_register));

        snprintf(buf + buf_len, len - buf_len, "%s,%s\n", dev_tmp->dev_name, dev_tmp->dev_id);
        buf_len = strlen(dev_tmp->dev_name) + strlen(dev_tmp->dev_id) + 2;
    }
    return 0;
}

/**
 * 串码对比
 */
int dev_code_comparison(unsigned short dev_count, void *dev_info, struct s_dev_register *dev_register)
{
    if (dev_info == NULL)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "data is NULL!", NULL_ERR);
        return NULL_ERR;
    }
    if (dev_register == NULL)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "data is NULL!", NULL_ERR);
        return NULL_ERR;
    }

    int i = 0;
    char dev_existent = 0;

    struct s_dev_register *dev_tmp;
    for (i = 0; i < dev_count; i++)
    {
        dev_tmp = (struct s_dev_register *)(dev_info + i * sizeof(struct s_dev_register));
        if (memcmp(dev_tmp->dev_mac, dev_register->dev_mac, strlen(dev_register->dev_mac)) == 0)
        {
            if ((dev_tmp->end_time - time(NULL)) < 0)
            {
                OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "dev_code timeout!", OVERDUE_ERR);
                return OVERDUE_ERR;
            }
            if (dev_tmp->dev_code != dev_register->dev_code)
            {
                OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "dev_code error!", IDENTIFYING_CODE_ERR);
                return IDENTIFYING_CODE_ERR;
            }
            dev_existent = 1;
            break;
        }
    }
    if (dev_existent == 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "non existent error!", NON_EXISTENT_ERR);
        return NON_EXISTENT_ERR;
    }
    return 0;
}

/**
 * 添加指定行 （数据库）
 */
int database_insert_dev_info(struct s_dev_register *dev_register)
{
    int res = 0;
    char columns_name[4][30] = {"device_name", "device_id", "device_mac", "device_code"};
    char columns_value[4][100] = {0};
    unsigned short columns_value_len[4] = {0};

    memcpy(columns_value[0], dev_register->dev_name, strlen(dev_register->dev_name));
    memcpy(columns_value[1], dev_register->dev_id, strlen(dev_register->dev_id));
    memcpy(columns_value[2], dev_register->dev_mac, strlen(dev_register->dev_mac));
    sprintf(columns_value[3], "%d", dev_register->dev_code);

    columns_value_len[0] = strlen(columns_value[0]);
    columns_value_len[1] = strlen(columns_value[1]);
    columns_value_len[2] = strlen(columns_value[2]);
    columns_value_len[3] = strlen(columns_value[3]);

    if ((res = database_management.insert(4, columns_name, columns_value, columns_value_len)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "sqlite3_insert failed!", res);
        return res;
    }
    return 0;
}

/**
 * 删除指定行 （数据库）
 */
int database_delete_dev_info(struct s_dev_register *dev_register)
{
    int res = 0;
    char columns_name[1][30] = {"device_mac"};
    char columns_value[1][100] = {0};

    memcpy(columns_value[0], dev_register->dev_mac, strlen(dev_register->dev_mac));
    if ((res = database_management.delete_row(1, columns_name, columns_value)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "sqlite3_delete_row failed!", res);
        return res;
    }
    return 0;
}

/**
 * 查询注册成功的设备
 */
int database_select_dev_info(char *buf, unsigned short len)
{
    int res = 0;
    char columns_name[2][30] = {"dev_name", "device_id"};
    if ((res = database_management.select_table(2, columns_name, buf, len)) < 0)
    {
        OPERATION_LOG(__FILE__, __FUNCTION__, __LINE__, "sqlite3_select_table failed!", res);
        return res;
    }
    return 0;
}

#endif // BOARDTYPE == 9344