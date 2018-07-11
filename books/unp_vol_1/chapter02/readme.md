# Chapter 02 The transport layer: TCP, UDP and SCTP


**Introduction UDP, TCP and SCTP**  
TCP: three-way handshake  
SCTP: four-way handshake  

**Overview of TCP/IP protocols**  
The differences between IPv4 and IPv6.  

**Details about UDP, TCP and SCTP**
UDP: unreliable, connectionless protocol.  
TCP: realiable, connection-oriented protoco.  

**Port number? what does that mean?**  
Port number is used for distinguishing different processes on transport layer.  
well-known port number - reserved port number  

Three different standaration  
1) IANA:  
2) BSD:  
3) Solaris:  

**socket pair ?**  
{ip-address:port-number, foreign-ip-address:foreign-port-number}  
{\*.\*, \*.\*} such as {192.168.123.12:34332, 12.106.32.254:21}  

**TCP three-way handshake for creating connection**
**TCP four-way handshake for terminating connection**

Why we need four-way handshake ?  
A -> B (fin)   A send fin to B  
B -> A (ack)   B acknowlege the fin from A  
B -> A (fin)   B send fin to A  
A -> B (ack)   A acknowlege the fin from B  

what if the last ack is lost ?  
B must resend fin to A, and A will resend ack to B.

**What full-duplex mean ?**  

**Make clear of the TCP state transition diagram, which is so important for
next step.**  

**2MSL(Maximum Segment Lifetime), why we need to wait for 2MSL ?**  
It's so stupid, but worth it.  

**MSS and MTU, different thing within different layer, but indicate the size of
transimission capacity.**  
MTU = 20(ipv4 header) + 20(tcp header) + MSS  
MSS: tcp segment max size.  
MTU: ethernet datagram max size.  
the relationship between MSS and buffer size ?  

**Common Internet Applications**  
routing protocol  
bootstrap protocol  
web  
ssh  
telnet  
HTTP  
