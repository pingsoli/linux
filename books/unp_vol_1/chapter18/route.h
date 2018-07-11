#ifndef ROUTE_H_
#define ROUTE_H_

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

void get_rtaddr(int, struct sockaddr*, struct sockaddr **);
const char *sock_masktop(struct sockaddr*, socklen_t);
char *sock_ntop_host(const struct sockaddr *, socklen_t);

#endif /* ROUTE_H_ */
