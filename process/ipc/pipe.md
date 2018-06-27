# Pipe under UNIX-like systems
**half duplex pipe Vs. full duplex pipe**

>> pipes are oldest from of UNIX system IPC are provided by all UNIX systems.  
>> pipes have two limitations:  
>>   
>> 1) Historically, they have been half duplex(i.e., data flows in only one  
>> direction). Some systems now provided full-duplex pipes, but for maximum  
>> portability, we should never assume that this is the case.  
>>   
>> 2) Pipes can be used only between processes that have a common ancestor. Normally  
>> a pipe is created by a process, that process calls fork, and the pipe is used  
>> between the parent and the child.  

extract from APUE  

# References
[1] http://poincare.matf.bg.ac.rs/~ivana/courses/ps/sistemi_knjige/pomocno/apue/APUE/0201433079/ch15lev1sec2.html   
[2] http://www.cs.toronto.edu/~krueger/csc209h/lectures/Week8-Pipes-4.pdf  
