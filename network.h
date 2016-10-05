#ifndef __NETWORK_H
#define __NETWORK_H

#define MAX_DEV_NAME_LEN	32
#define MAX_SPEED_STR_LEN	32

#include <stdint.h>
#include "list.h"

typedef struct {
	uint64_t recv_bytes;
	uint64_t recv_pkgs;
	uint64_t send_bytes;
	uint64_t send_pkgs;
} net_data_t;

struct net_dev {
	list_t list;
	char   dev_name[MAX_DEV_NAME_LEN];
	int    online;
	net_data_t cur_data;
	net_data_t pre_data;
};
typedef struct net_dev  net_dev_t;

typedef struct {
	int  online_num;
	int  dev_num;
	char recv_speed[MAX_SPEED_STR_LEN];
	char send_speed[MAX_SPEED_STR_LEN];
	net_data_t cur_data;
	net_data_t pre_data;
	list_t dev_list;
} net_t;

struct speed {
	char   unit[8];
	double value;
};

extern int init_net(net_t *net);
extern int update_net(net_t *net);
extern int free_net(net_t *net);

extern int update_speed_str(net_t *net, int interval);

#endif
