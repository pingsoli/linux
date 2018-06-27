/*
 * raw socket demostration.
 * NOTE: needs root privilege.
 *
 * How to capture the data ?
 * Using tcpdump tool.
 * $ sudo tcpdump -xx -e -i ens33 ether dst host 00:11:22:33:44:55
 *
 * -xx          - show the send data as hex format.
 * -e           - show ethernet info
 * -i interface - your interface of NIC
 * ether dst host 00:11:22:33:44:55 - specify mac address
 */
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

/* Ethernet header format                                                  */
/* +-------------------------+--------------------+------------+---------+ */
/* | Destination MAC Address | Source MAC Address | Ether type | Payload | */
/* +-------------------------+--------------------+------------+---------+ */
/* |<------- 6 bytes ------->|<------ 6 bytes --->|<- 2 bytes->|           */
/* |<-------------------- 14 bytes ----------------------------|           */
/*                                                                         */
/* the sending data likes the following.                                   */
/* 0x0000:  0011 2233 4455 000c 2952 935e 0800 0a0b                        */
/* 0x0010:  0c0d                                                           */
/*                                                                         */
/* 0x0800 is IPv4 type                                                     */
/* 0x0a 0x0b 0x0c 0x0d are payload                                         */

#define MY_DEST_MAC0   0x00
#define MY_DEST_MAC1   0x11
#define MY_DEST_MAC2   0x22
#define MY_DEST_MAC3   0x33
#define MY_DEST_MAC4   0x44
#define MY_DEST_MAC5   0x55

#define DEFAULT_IF     "ens33"
#define BUF_SIZ        1024

int main(int argc, char *argv[])
{
  int sockfd;
  struct ifreq if_idx;
  struct ifreq if_mac;
  int tx_len = 0;
  char sendbuf[BUF_SIZ];
  struct ether_header *eh = (struct ether_header *) sendbuf;
  struct iphdr *iph = (struct iphdr *) (sendbuf + sizeof(struct ether_header));
  struct sockaddr_ll socket_address;
  char ifName[IFNAMSIZ];

  /* Get interface name */
  if (argc > 1)
    strcpy(ifName, argv[1]);
  else
    strcpy(ifName, DEFAULT_IF);

  /*
   * Open a raw socket, more detail: man 7 packet
   *
   * What does IPPROTO_RAW do ?
   * If you set it, you can edit the header and payload of you IP packet.
   */
  if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
    perror("socket");
  }

  /* Get the index of the interface to send on */
  memset(&if_idx, 0, sizeof(struct ifreq));
  strncpy(if_idx.ifr_name, ifName, IFNAMSIZ - 1);
  if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
    perror("SIOCGIFINDEX");

  /* Get the mac address of the interface to send on */
  memset(&if_mac, 0, sizeof(struct ifreq));
  strncpy(if_mac.ifr_name, ifName, IFNAMSIZ -1 );
  if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
    perror("SIOCGIFHWADDR");

  /* Construct the ethernet header */
  memset(sendbuf, 0, BUF_SIZ);
  /* Ethernet header */
  eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
  eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
  eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
  eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
  eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
  eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
  eh->ether_dhost[0] = MY_DEST_MAC0;
  eh->ether_dhost[1] = MY_DEST_MAC1;
  eh->ether_dhost[2] = MY_DEST_MAC2;
  eh->ether_dhost[3] = MY_DEST_MAC3;
  eh->ether_dhost[4] = MY_DEST_MAC4;
  eh->ether_dhost[5] = MY_DEST_MAC5;
  /* Ethernet field, next is IPv4 */
  eh->ether_type = htons(ETH_P_IP);
  tx_len += sizeof(struct ether_header);

  /* Packet data */
  sendbuf[tx_len++] = 0x0A;
  sendbuf[tx_len++] = 0x0B;
  sendbuf[tx_len++] = 0x0C;
  sendbuf[tx_len++] = 0x0D;

  /* Index of the network device */
  socket_address.sll_ifindex = if_idx.ifr_ifindex;
  /* Address length */
  socket_address.sll_halen = ETH_ALEN;

  /* Destination MAC */
  socket_address.sll_addr[0] = MY_DEST_MAC0;
  socket_address.sll_addr[1] = MY_DEST_MAC1;
  socket_address.sll_addr[2] = MY_DEST_MAC2;
  socket_address.sll_addr[3] = MY_DEST_MAC3;
  socket_address.sll_addr[4] = MY_DEST_MAC4;
  socket_address.sll_addr[5] = MY_DEST_MAC5;

  /* Send the raw ethernet packet */
  if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address,
        sizeof(struct sockaddr_ll)) < 0) {
    printf("Send failed\n");
  }

  return 0;
}
