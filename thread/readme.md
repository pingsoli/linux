# threads in linux

POSIX thread library: Native POSIX Thread Library (NPTL).

All threads within a process has the same address space.  
Threads in the same process share:  
 a) Process instructions  
 b) Most data  
 c) Open files descriptors  
 d) Signals and Signal handler  
 e) current working directory  
 f) User and group id  
  
Each thread has a unique:  
 a) Thread ID  
 b) set of registers, stack pointer  
 c) signal mask  
 d) priority  
 e) Return value: errno  
