#Nat rush.
This is simple utility generate reverse connection to client side server (VNC for example).
If your client can connect to your server, program generate simple socket channel to him server.

#Config file example
Client side
```
[General]
address=vnc.onlinepbx.ru
port=6900
rawAddress=127.0.0.1
rawPort=5900
process=winvnc.exe
processArgs=-run
```
Client connect to vnc.onlinepbx.ru to port 6900 and create connection to localhost port 5900. 
Plus started application winvnc whith args "-run". (If you need not start application set process=none).

Server side
```
[General]
LogLevel=3
MainPort=6900
```
![CC license](https://i.creativecommons.org/l/by-nc-sa/4.0/88x31.png)Creative Commons «Attribution-NonCommercial-ShareAlike»
