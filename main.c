/*
 * Copyright (C) 2015 Bryan Christ <bryan.christ@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */


#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#ifdef __linux
#include <linux/if_packet.h>
#include <net/if.h>
#endif

#define FLAG_SHOW_IPV4  (1 << 0)
#define FLAG_SHOW_IPV6  (1 << 1)
#define FLAG_SHOW_MAC   (1 << 2)

struct _config_s
{
    char            *iface;
    unsigned int    flags;
};

static int
parse_commands(int argc, char **argv, struct _config_s *config);

static void
print_usage(void);


int
main(int argc, char **argv)
{
    struct ifaddrs      *ifaddr;
    struct ifaddrs      *ifa;
    struct sockaddr_ll  *saddr;
    int                 family;
    char                host[NI_MAXHOST];
    char                mac[20];
    struct _config_s    config;
    char                *pos;
    int                 len;
    int                 i;

    memset(&config, 0, sizeof(config));

    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    parse_commands(argc, argv, &config);

    /*
        Walk through linked list, maintaining head pointer so we
        can free list later
    */

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
        continue;

        family = ifa->ifa_addr->sa_family;

        /*
            Display interface name and family (including symbolic
            form of the latter for the common families)
        */

        if (config.iface != NULL)
        {
            // user specified an inteface of intrest.  skip all others.
            if (strcmp(config.iface, ifa->ifa_name) !=0 ) continue;
        }

        if (family == AF_PACKET && config.flags & FLAG_SHOW_MAC)
        {
            // skip loopback devices.  they dont have a hwaddr
            if (ifa->ifa_flags & IFF_LOOPBACK) continue;

            saddr = (struct sockaddr_ll*)ifa->ifa_addr;

            memset(mac, 0 , sizeof(mac));
            len = 0;
            for(i = 0;i < 6;i++)
            {
                len += sprintf(mac + len, "%02X%s",
                    saddr->sll_addr[i], i < 5 ? ":":"");
            }

            printf("%s %s\n", ifa->ifa_name, mac);
        }

        if (family == AF_INET || family == AF_INET6)
        {
            if ((family == AF_INET) && !(config.flags & FLAG_SHOW_IPV4))
                continue;

            if ((family == AF_INET6) && !(config.flags & FLAG_SHOW_IPV6))
                continue;

            pos = NULL;
            memset(host, 0, sizeof(host));

            // TODO:  check return value and handle gracefully
            getnameinfo(ifa->ifa_addr, (family == AF_INET) ?
                sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6),
                host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

            // linux inserts a %<ifname> suffix on IPv6 addresses.  trim out.
            pos = strchr(host, '%');
            if(pos != NULL) *pos = '\0';

            printf("%s %s\n", ifa->ifa_name, host);
        }
    }

    freeifaddrs(ifaddr);
    exit(EXIT_SUCCESS);
}

static int
parse_commands(int argc, char **argv, struct _config_s *config)
{
    static struct option    long_options[] =
    {
        {"iface",   required_argument,  0,  'i'},
        {"ipv4",    required_argument,  0,  '4'},
        {"ipv6",    required_argument,  0,  '6'},
        {"mac",     required_argument,  0,  'm'},
        {"help",    no_argument,        0,  'h'},
        {0,         0,                  0,  0}
    };

    int c;

    optind = 0;

    for (;;)
    {
        c = getopt_long(argc, argv, "i:46mh", long_options, NULL);
        if (c == -1) break;

        switch (c)
        {
            case 'h':
                print_usage();
                exit(0);

            case 'i':
                config->iface = strdup(optarg);
                break;

            case '4':
                config->flags |= FLAG_SHOW_IPV4;
                break;

            case '6':
                config->flags |= FLAG_SHOW_IPV6;
                break;

            case 'm':
                config->flags |= FLAG_SHOW_MAC;
                break;

            default:
                break;
        }
    }


    return 0;
}

static void
print_usage(void)
{
    fprintf(stderr, "Usage: ifaddr [options]\n"
        "\n"
        "    -i  --iface [ifname]   the inteface to query. all if ommited.\n"
        "                           examples: eth0, p0p1\n"
        "\n"
        "    -4, --ipv4             show IP version 4 address\n"
        "    -6, --ipv6             show IP version 6 address\n"
        "    -m, --mac              show hardware address of interface\n"
        "\n"
        "    -h, --help             show this help\n"
        "\n"
        "Notice that options are separated from their arguments by\n"
        "a space and not an equal sign.\n" "\n");

    return;
}


