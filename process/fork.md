# Creating process

more detail: man fork  
the child process and the parent process run in seperate memory spaces.  
process id (pid)  

### What data is shared between the parent and child processes ?
1) share same text segment. (they have same code, but different logic generally)  
2) share file descriptors. (sharing of the open files between them after fork)  

### What will be inherited from the parent process in child process ?
1) Real user ID, group ID, effective user ID and effective group ID.  
2) Session ID.  
3) Controlling terminal.  
4) The set-user-ID and set-group-ID.  
5) Current working directory.  
6) Root directory.  
7) File mode creation mask.  
8) Signal mask and dispositions.  
9) The close-on-exec flag for any open file descriptors.  
10) Environment.  
11) Memory mappings.  
12) Attached shared memory segments.  
13) Resource limits.  

the difference:  
1) The return value from fork().  
2) Process IDs are different.  
3) They have different patent process ID.  
4) File locks set by the parent are not inherited by the child.  
5) Pending alarms are cleared for the child.  
6) The set of pending signals for the child is set to the empty set.  

### FAQ
#### Why parent and child processes' output to same place ?
Because they share the open files.  

### Reference
APUE  
