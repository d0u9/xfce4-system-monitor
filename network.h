#ifndef __NETWORK_H
#define __NETWORK_H

#define MAX_DEV_NAME_LEN	32
#define MAX_SPEED_STR_LEN	32

#include <stdint.h>
#include "trilib/list.h"

struct net_data {
	uint64_t recv_bytes;
	uint64_t recv_pkgs;
	uint64_t send_bytes;
	uint64_t send_pkgs;
};

struct net_dev {
	struct list_head list;
	char   dev_name[MAX_DEV_NAME_LEN];
	int    online;
	struct net_data cur_data;
	struct net_data pre_data;
};

struct net {
	int  online_num;
	int  dev_num;
	char recv_speed[MAX_SPEED_STR_LEN];
	char send_speed[MAX_SPEED_STR_LEN];
	struct net_data cur_data;
	struct net_data pre_data;
	struct list_head dev_list;
};

struct speed {
	char   unit[8];
	double value;
};

extern int init_net(struct net *net);
extern int update_net(struct net *net);
extern int free_net(struct net *net);

extern int update_speed_str(struct net *net, int interval);

#endif
