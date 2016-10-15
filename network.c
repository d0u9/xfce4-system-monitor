#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

#include "sysmonitor.h"
#include "network.h"
#include "trilib/log.h"

#define CARRIER_DIR     "/sys/class/net"
#define PROC_NET_DEV    "/proc/net/dev"

int init_net(struct net *net);
int update_net(struct net *net);
int free_net(struct net *net);
int update_speed_str(struct net *net, int interval);

static struct speed calculate_speed(uint64_t old, uint64_t new, int interval);
static struct net_dev *get_net_status(struct net *net);

int init_net(struct net *net)
{
	INIT_LIST_HEAD(&net->dev_list);
	return 0;
}

int update_net(struct net *net)
{
	get_net_status(net);
	return 0;
}

int free_net(struct net *net)
{
	struct list_head *cur, *head = &net->dev_list;
	list_for_each(cur, head) {
		struct net_dev *dev = list_entry(cur, struct net_dev, list);
		list_del(cur);
		free(dev);
	}
	return 0;
}

struct speed calculate_speed(uint64_t old, uint64_t new, int interval)
{
	struct speed speed;
	double value = (new - old) / (interval / 1000.0);

	memset(&speed, 0, sizeof(struct speed));
	printl_debug("-> speed = %f\n", value);
	if (value > CONST_Gi) {
		speed.value = value / CONST_Gi;
		strncpy(speed.unit, "GB/s ", 7);
		return speed;
	}

	if (value > CONST_Mi) {
		speed.value = value / CONST_Mi;
		strncpy(speed.unit, "MB/s ", 7);
		return speed;
	}

	if (value > CONST_Ki) {
		speed.value = value / CONST_Ki;
		strncpy(speed.unit, "KB/s ", 7);
		return speed;
	}

	speed.value = value;
	strncpy(speed.unit, "B/s", 7);
	return speed;
}

static int get_net_link_status(const char *dev_name)
{
	char line[MAX_FILE_LINE_LEN] = {0};
	FILE *fp = NULL;

	sprintf(line, CARRIER_DIR "/%s/carrier", dev_name);
	if ((fp = fopen(line, "r")) == NULL)
		return -1;

	fgets(line, MAX_FILE_LINE_LEN, fp);
	fclose(fp);

	return strncmp("1", line, 1) ? 0 : 1;
}


static struct net_dev *get_net_status(struct net *net)
{
	char line[MAX_FILE_LINE_LEN] = {0};
	FILE *fp = NULL;
	int  online_num = 0, dev_num = 0;
	struct list_head *head, *cur;

	cur = head = &net->dev_list;

	if ((fp = fopen(PROC_NET_DEV, "r")) == NULL)
		return NULL;

	fgets(line, MAX_FILE_LINE_LEN, fp);
	fgets(line, MAX_FILE_LINE_LEN, fp);
	memset(line, 0, MAX_FILE_LINE_LEN);
	for (dev_num = 0; fgets(line, MAX_FILE_LINE_LEN, fp); dev_num++) {
		struct net_dev *dev = NULL;
		if (cur->next == head) {
			dev = (struct net_dev*)calloc(1, sizeof(struct net_dev));
			INIT_LIST_HEAD(&dev->list);
			list_add(&dev->list, cur);
		} else {
			dev = list_entry(cur->next, struct net_dev, list);
		}

		dev->pre_data = dev->cur_data;

		sscanf(line, "%s %"PRIu64" %"PRIu64" %*s %*s %*s %*s "
		       "%*s %*s %"PRIu64" %"PRIu64,
		       dev->dev_name, &dev->cur_data.recv_bytes,
		       &dev->cur_data.recv_pkgs, &dev->cur_data.send_bytes,
		       &dev->cur_data.send_pkgs);

		if ((dev->online = get_net_link_status(dev->dev_name)))
			online_num ++;

		cur = cur->next;
	}

	fclose(fp);

	if (net) {
		net->online_num = online_num;
		net->dev_num = dev_num;
	}


	struct net_dev *tmp_entry;
	struct net_dev *cur_entry = list_entry(cur, struct net_dev, list);
	printl_debug("%s\n", cur_entry->dev_name);
	list_for_each_entry_safe_from(cur_entry, tmp_entry, head, list) {
		list_del(&cur_entry->list);
		free(cur_entry);
	}

	return NULL;
}


int update_speed_str(struct net *net, int interval)
{
	net->pre_data = net->cur_data;

	memset(&net->cur_data, 0, sizeof(net->cur_data));

	struct list_head *head = &net->dev_list, *cur = NULL;

	list_for_each(cur, head) {
		struct net_dev *dev = NULL;
		dev = list_entry(cur, struct net_dev, list);
		if (dev->online) {
			net->cur_data.recv_pkgs  += dev->cur_data.recv_pkgs;
			net->cur_data.recv_bytes += dev->cur_data.recv_bytes;
			net->cur_data.send_pkgs  += dev->cur_data.send_pkgs;
			net->cur_data.send_bytes += dev->cur_data.send_bytes;
		}
	}

	struct speed speed;

	speed = calculate_speed(net->pre_data.recv_bytes,
				net->cur_data.recv_bytes, interval);
	if (speed.value < CONST_EPSILON)
		sprintf(net->recv_speed, "%.2f %s " STR_DOWNLINK_SIGN_IDLE,
			speed.value, speed.unit);
	else
		sprintf(net->recv_speed, "%.2f %s " STR_DOWNLINK_SIGN_BUSY,
			speed.value, speed.unit);

	speed = calculate_speed(net->pre_data.send_bytes,
				net->cur_data.send_bytes, interval);
	if (speed.value < CONST_EPSILON)
		sprintf(net->send_speed, "%.2f %s " STR_UPLINK_SIGN_IDLE,
			speed.value, speed.unit);
	else
		sprintf(net->send_speed, "%.2f %s " STR_UPLINK_SIGN_BUSY,
			speed.value, speed.unit);


	return 0;
}
