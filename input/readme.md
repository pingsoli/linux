# Input Events

more detail: google 'input subsystem'.
https://www.kernel.org/doc/html/latest/input/input_uapi.html

## How to listen input events ?

/dev/input directory
```
$ la /dev/input
total 0
drwxr-xr-x  4 root root     260 Jun 20 08:28 .
drwxr-xr-x 19 root root    4.0K Jun 21 08:27 ..
drwxr-xr-x  2 root root      80 Jun 20 08:28 by-id
drwxr-xr-x  2 root root     140 Jun 20 08:28 by-path
crw-rw----  1 root input 13, 64 Jun 20 08:28 event0
crw-rw----  1 root input 13, 65 Jun 20 08:28 event1
crw-rw----  1 root input 13, 66 Jun 20 08:28 event2
crw-rw----  1 root input 13, 67 Jun 20 08:28 event3
crw-rw----  1 root input 13, 68 Jun 20 08:28 event4
crw-rw----  1 root input 13, 63 Jun 20 08:28 mice
crw-rw----  1 root input 13, 32 Jun 20 08:28 mouse0
crw-rw----  1 root input 13, 33 Jun 20 08:28 mouse1
crw-rw----  1 root input 13, 34 Jun 20 08:28 mouse2
```
