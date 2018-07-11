# Chapter 25 Signal-Driven I/O

TCP and UDP comparision:  
  
SIGIO signal  
  
**UDP only two conditions:**  
1) data arrives  
2) async error  
  
**TCP has too much conditions:**  
1) data arrives  
2) data has been sent from the socket  
2) new connection(connection request has completed)  
3) close connection(disconnection request has been inited or completed)  
4) async error  
  
