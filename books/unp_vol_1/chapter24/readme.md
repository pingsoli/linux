# Chapter 24: Out-of-Band data

UDP don't have out-of-band data, but TCP does.  
  
what is out-of-data ?  
Send the data "quickly", ASAP. That means high priority data.  
  
urgent mode. this mode is provided for tcp sends data quickly.  
MSG_OOB flag  

```
send(fd, "a", 1, MSG_OOB)
```

**receive buffer is full, does receiver can receive the out-of-data ?**  
Yes, receiver can. If buffer is full, and the receiver advertise the window of 0  
to the sending TCP, and the urgent notification is sent without any data.  
so, the urgent notification is always send to the peet TCP.  
  
the sequence of old OOB data and new OOB data?  
the old OOB data will be discarded.  

**Something to keep in mind:**  
1) IPv4 doesn't really send OOB data on a separate channel, or at a different  
priority. It's just a flag on the packet.  
2) OOB data is extremely limited -- 1 byte!  
3) OOB data can be received either inline or separately depending on socket  
options.  
4) An "exception" signaling OOB data may occur even if the next read doesn't  
contain the OOB data(the network stack on the sender may flag any already queued  
data, so the other side will know there's OOB ASAP). This is often handled by  
entering a "drain" loop where you discard data until the actual OOB data is  
available.  
  

**what happened when sending multiple bytes one time ?**  
```
send(fd, "abc", 3, MSG_OOB);

receive buffer
+---+---+---+---------------+
| a | b | c | .. left space |
+---+---+---+---------------+
            ^
            |
        URG pointer

only read one byte, so 'c' is OOB data, 'ab' is regular data.
```
  
**events in poll() ?**  
If you want to use poll, you must watch the out-of-band data, you should specify  
the POLLPRI to represent out-of-band data. If the out-of-band data arrives, you  
can read from sockfd.  

**sockatmark() function ?**  
sockatmark function determine whether socket is at out-of-data mark.  
In FTP, we want to cancel the transmitting. Here is the solution:  
send out-of-band data, and use sockatmark check the state of OOB data, if OOB  
data arrives, we stop transmit immediately.  
  
**socket SO_OOBINLINE options ?**  
if this option is set, out-of-band data received on the socket is placed in the  
normal input queue. (we can detect the OOB data with sockatmark() function)  
```
int on = 0;
setsockopt(listenfd, SOL_SOCKET, SO_OOBINLINE, &on, sizeof on);
```
