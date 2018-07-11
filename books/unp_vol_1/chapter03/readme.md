
# Chapter 03 Socket Introduction

**Socket Address Structure**  
IPv4 and IPv6 differences  

```
/* IPv4 structure */
struct sockaddr_in
struct in_addr

/* IPv6 structure */
struct sockaddr_in6
struct in6_addr

struct sockaddr /* generic socket address structure */
struct sockaddr_storage /* new generic socket address structure */

why use a newer sockaddr_storage structure ?
beacause there are many other kinds of addresses, sockaddr is suitable for IPv4
and IPv6, but not unix, datalink address.

```

**what information is needed by connection ?**  
ip address(IPv4 and IPv6) and port number  
AF_INET Vs. AF_INET6  

```
struct sockaddr_in servaddr;
memset(&servaddr, 0, sizeof(struct sockaddr_in));

servaddr.sin_family = AF_INET;
servaddr.port = htons(13);

inet_pton(&servaddr.sin_addr, argv[1], INET_ADDRSTRLEN);
```

**Byte order**  
Big-endian and Little-endian  
  
Network byte order use what kind of endian ? big-endian.  
Why exists little-endian ?  
  
the function used to convert network byte order in linux ?  
htonl()  
htons()  
ntohl()  
ntohs()  
  
h stands for host, n is network, l is "long", s is "short"  

**Some wrapper function for socket programming**  
  
readn  
writen  
readline  
