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
static void free_net_dev(net_dev_t *devs);
static net_dev_t *get_net_status(net_t *net);


int init_net(net_t *net)
{
        net->devs = get_net_status(NULL);
        return 0;
}


int update_net(net_t *net)
{
        get_net_status(net);
        return 0;
}


int free_net(net_t *net)
{
        free_net_dev(net->devs);
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


static void free_net_dev(net_dev_t *devs)
{
        net_dev_t *cur = NULL;
        while (devs) {
                cur = devs;
                devs = devs->next;
                free(cur);
        }
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
        int online_num = 0, dev_num = 0;

        net_dev_t       dummy;
        net_dev_t       *devs = net ? net->devs : NULL;
        net_dev_t       *cur = devs,  *last = devs;

        if ((fp = fopen(PROC_NET_DEV, "r")) == NULL)
                return NULL;

        last = &dummy;
        last->next = cur = devs;

        for (int i = 0; fgets(line, MAX_FILE_LINE_LEN, fp); ++i) {
                if (i > 1) {
                        if (!cur)
                                cur = (net_dev_t *) calloc(1, sizeof(net_dev_t));

                        cur->pre_recv_pkgs  = cur->recv_pkgs;
                        cur->pre_recv_bytes = cur->recv_bytes;
                        cur->pre_send_pkgs  = cur->send_pkgs;
                        cur->pre_send_bytes = cur->send_bytes;

                        sscanf(line, "%s %"PRIu64" %"PRIu64" %*s %*s %*s %*s "
                                     "%*s %*s %"PRIu64" %"PRIu64,
                                      cur->dev_name, &cur->recv_bytes, &cur->recv_pkgs,
                                      &cur->send_bytes, &cur->send_pkgs);

                        dev_num++;
                        if ((cur->online = get_net_link_status(cur->dev_name)))
                                online_num ++;

                        last->next = cur;
                        last = cur;
                        cur = cur->next;
                }
        }

        if (net) {
                net->online_num = online_num;
                net->dev_num = dev_num;
        }
        free_net_dev(last->next);
        last->next = NULL;

        return dummy.next;
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

        net_dev_t *cur = net->devs;
        while (cur) {
                if (cur->online) {
                        net->recv_pkgs  += cur->recv_pkgs;
                        net->recv_bytes += cur->recv_bytes;
                        net->send_pkgs  += cur->send_pkgs;
                        net->send_bytes += cur->send_bytes;
                }
                cur = cur->next;
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
