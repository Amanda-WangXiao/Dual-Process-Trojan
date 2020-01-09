# Dual-Process-Trojan
A simple Trojan program. It's just for coding exercise rather than attacking others' computer.  
# Introduction  
•	Established a remote communication between the server and the client through socket.  
  
•	Realized the control of the server side through the client’s command, including remote message transmission, file transfer, remote CMD control authority acquisition, etc..  
  
•	Made the Trojan can detect a current idle port and use the existing port to start socket communication. 
  
•	Obtained the method to modify the registry to achieve the default start of the Trojan and associated it with all kinds of file which were opening.  
  
•	Prevented the natural close of the Trojan.  
  
通过socket建立服务端与客户端的远程通信，通过远程命令实现客户端对服务器的控制，其中包括远程消息传送、文件传输、远程cmd控制权限获取等。可以检测当前闲置端口并使用已存在端口进行socket通讯，修改注册表实现木马的默认启动，与各类文件打开方式相关联，实现了木马的双进程守护及防止进程结束。  
# Environment  
Windows 10  
Microsoft Visual Studio Pro 2013  
# Demo  
https://youtu.be/fP13xDHcK7M  
