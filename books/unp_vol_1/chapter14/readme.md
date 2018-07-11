# Chapter 14 Advanced I/O functions

socket timeout three ways implementation  
1) alarm() function  
2) select()  
3) SO_RCVTIMEO and SO_SNDTIMEO socket options  
  
How much data is queued?  
use standard IO to send and receive socket stream  
get socket stream buffer size information using MSG_PEEK option and ioctl() function  
  
talk about /dev/poll and kqueue  
linux has no implementation about /dev/poll and kqueue, we ignore it  
