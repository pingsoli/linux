# Chapter 16 Nonblocking IO
  
read(), write(), accept() and connect()  
the operations read() and write() socket are nonblocking  
  
connect() is nonblocking  
execute other operations when three-way handshake  
  
accept()  
what happens when accept() uncomplete connection?  
accept() will block when the fd is destoryed until next completed connection.  
