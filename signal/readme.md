# signal
signals are software interrupts.

**sigprocmask() function**  
1) set signal set.(SIG_SETMASK)  
2) block signal.(SIG_BLOCK)  
3) unblock signal.(SIG_UNBLOCK)  
  
**sigsuspend() function**  
sleep until the signal handler returned, exclude the blocked signals  
  
**sigpending() function**  
find out the signal is pending not handled.  
  
sigaction() and relative structure and function.  
  
## Race Condition in Signal
please see the race_condition directory examples.

**Reliable and unreliable signals ?**  
In earlier Unix system, the signal may be lost, they are called unreliable  
signals. In modern system, the signals are implemented to be reliable.  
  
**Something must keep in mind**  
1) signals are **asynchronous events**. they occur at random times to the process.  
2) SIGKILL and SIGSTOP cannot be caught, blocked, or ignored.  
3) only three signal dispositions:  
  a. ignore the signal.  
  b. catch the signal.  
  c. apply default signal action.  
  
  
**Any ways to make same signals queued ?**  
no way.  
