#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

#include "sysmonitor.h"
#include "network.h"

#define CARRIER_DIR     "/sys/class/net"
#define PROC_NET_DEV    "/proc/net/dev"

int init_net(net_t *net);
int update_net(net_t *net);
int free_net(net_t *net);
int update_speed_str(net_t *net, int interval);


static struct speed calculate_speed(uint64_t old, uint64_t new, int interval);
static net_dev_t *get_net_status(net_t *net);


int init_net(net_t *net)
{
        list_init(&net->dev_list);
        return 0;
}


int update_net(net_t *net)
{
        get_net_status(net);
        return 0;
}


int free_net(net_t *net)
{
        list_t *cur, *head = &net->dev_list;
        list_for_each(cur, head) {
                cur = list_remove(cur);
                net_dev_t *dev = entry_of(cur, net_dev_t, list);
                free(dev);
        }
        return 0;
}


struct speed calculate_speed(uint64_t old, uint64_t new, int interval)
{
        struct speed speed;
        double value = (new - old) / (interval / 1000.0);

        memset(&speed, 0, sizeof(struct speed));
        g_print("-> speed = %f\n", value);
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
        char    line[MAX_FILE_LINE_LEN] = {0};
        FILE    *fp = NULL;

        sprintf(line, CARRIER_DIR "/%s/carrier", dev_name);
        if ((fp = fopen(line, "r")) == NULL)
                return -1;

        fgets(line, MAX_FILE_LINE_LEN, fp);
        fclose(fp);

        return strncmp("1", line, 1) ? 0 : 1;
}


static net_dev_t *get_net_status(net_t *net)
{
        char    line[MAX_FILE_LINE_LEN] = {0};
        FILE    *fp = NULL;
        list_t  *head, *cur, *tmp;
        int online_num = 0, dev_num = 0;

        cur = head = &net->dev_list;

        if ((fp = fopen(PROC_NET_DEV, "r")) == NULL)
                return NULL;

        fgets(line, MAX_FILE_LINE_LEN, fp);
        fgets(line, MAX_FILE_LINE_LEN, fp);
        memset(line, 0, MAX_FILE_LINE_LEN);
        for (dev_num = 0; fgets(line, MAX_FILE_LINE_LEN, fp); dev_num++) {
                net_dev_t *dev = NULL;
                if (cur->next == head) {
                        dev = (net_dev_t *)calloc(1, sizeof(net_dev_t));
                        list_init(&dev->list);
                        list_add(cur, &dev->list);
                } else {
                        dev = entry_of(cur->next, net_dev_t, list);
                }

                dev->pre_recv_pkgs  = dev->recv_pkgs;
                dev->pre_recv_bytes = dev->recv_bytes;
                dev->pre_send_pkgs  = dev->send_pkgs;
                dev->pre_send_bytes = dev->send_bytes;


                sscanf(line, "%s %"PRIu64" %"PRIu64" %*s %*s %*s %*s "
                             "%*s %*s %"PRIu64" %"PRIu64,
                             dev->dev_name, &dev->recv_bytes, &dev->recv_pkgs,
                             &dev->send_bytes, &dev->send_pkgs);

                if ((dev->online = get_net_link_status(dev->dev_name)))
                        online_num ++;

                cur = cur->next;
        }

        fclose(fp);

        if (net) {
                net->online_num = online_num;
                net->dev_num = dev_num;
        }
        list_for_each_continue_safe(cur, tmp, head) {
                cur = list_remove(cur);
                free(entry_of(cur, net_dev_t, list));
        }

        return NULL;
}


int update_speed_str(net_t *net, int interval)
{
        net->pre_recv_pkgs  = net->recv_pkgs;
        net->pre_recv_bytes = net->recv_bytes;
        net->pre_send_pkgs  = net->send_pkgs;
        net->pre_send_bytes = net->send_bytes;

        net->recv_pkgs  = 0;
        net->recv_bytes = 0;
        net->send_pkgs  = 0;
        net->send_bytes = 0;

        list_t *head = &net->dev_list, *cur = NULL;

        list_for_each(cur, head) {
                net_dev_t *dev = NULL;
                dev = entry_of(cur, net_dev_t, list);
                if (dev->online) {
                        net->recv_pkgs  += dev->recv_pkgs;
                        net->recv_bytes += dev->recv_bytes;
                        net->send_pkgs  += dev->send_pkgs;
                        net->send_bytes += dev->send_bytes;
                }
        }

        struct speed speed;

        speed = calculate_speed(net->pre_recv_bytes, net->recv_bytes, interval);
        if (speed.value < CONST_EPSILON)
                sprintf(net->recv_speed, "%.2f %s " STR_DOWNLINK_SIGN_IDLE,
                        speed.value, speed.unit);
        else
                sprintf(net->recv_speed, "%.2f %s " STR_DOWNLINK_SIGN_BUSY,
                        speed.value, speed.unit);

        speed = calculate_speed(net->pre_send_bytes, net->send_bytes, interval);
        if (speed.value < CONST_EPSILON)
                sprintf(net->send_speed, "%.2f %s " STR_UPLINK_SIGN_IDLE,
                        speed.value, speed.unit);
        else
                sprintf(net->send_speed, "%.2f %s " STR_UPLINK_SIGN_BUSY,
                        speed.value, speed.unit);


        return 0;
}
