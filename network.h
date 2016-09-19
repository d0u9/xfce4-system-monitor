#ifndef __NETWORK_H
#define __NETWORK_H

#define MAX_DEV_NAME_LEN        32
#define MAX_SPEED_STR_LEN       32

#include <stdint.h>

struct net_dev {
        char    dev_name[MAX_DEV_NAME_LEN];
        int     online;
        uint64_t        recv_bytes;
        uint64_t        recv_pkgs;
        uint64_t        send_bytes;
        uint64_t        send_pkgs;
        uint64_t        pre_recv_bytes;
        uint64_t        pre_recv_pkgs;
        uint64_t        pre_send_bytes;
        uint64_t        pre_send_pkgs;
        struct net_dev  *next;
};
typedef struct net_dev  net_dev_t;

typedef struct {
        int     online_num;
        int     dev_num;
        uint64_t        recv_bytes;
        uint64_t        recv_pkgs;
        uint64_t        send_bytes;
        uint64_t        send_pkgs;
        uint64_t        pre_recv_bytes;
        uint64_t        pre_recv_pkgs;
        uint64_t        pre_send_bytes;
        uint64_t        pre_send_pkgs;
        char    recv_speed[MAX_SPEED_STR_LEN];
        char    send_speed[MAX_SPEED_STR_LEN];
        net_dev_t       *devs;
} net_t;

struct speed {
        char    unit[8];
        double  value;
};

extern int init_net(net_t *net);
extern int update_net(net_t *net);
extern int free_net(net_t *net);

extern int update_speed_str(net_t *net, int interval);

#endif
