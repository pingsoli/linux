# Chapter 28: Raw socket

**non-Raw socket Vs. Raw socket**
```
non-Raw socket: means you can just determine Transport Layer payload.
Raw socket: means you can determine every section of packet, either header or
payload.

In programming:
socket(AF_INET, RAW_SOCKET,...) means L3 socket, Network Layer Protocol = IPv4
socket(AF_IPX, RAW_SOCKET,...) means L3 socket, Network Layer Protocol = IPX
socket(AF_INET6, RAW_SOCKET,...) means L3 socket, Network Layer Protocol = IPv6
socket(AF_PACKET, RAW_SOCKET,...) means L2 socket, Data-link Layer Protocol = Ethernet

reference from
https://stackoverflow.com/questions/14774668/what-is-raw-socket-in-socket-programming
```
  
**What does IP_HDRINCL do ?**
```
if (setsocktopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0) {
  perror("custom");
}

if IP_HDRINCL enabled, the user supplies an IP header in front of the user data.
Valid only for SOCK_RAW sockets. see raw(7) for more information.
```


