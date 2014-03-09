

UNIX domain sockets  
Internet domain sockets  

Modern operating systems support at least the following domains:  
The UNIX (AF_UNIX) domain  
The IPv4 (AF_INET) domain  
The IPv6 (AF_INET6) domain  

Every sockets implementation provides at least two types of sockets: stream and
datagram.

Stream sockets (SOCK_STREAM) provide a reliable, bidirectional, byte-stream
communication channel.  
Datagram sockets (SOCK_DGRAM) allow data to be exchanged in the form of
messages called datagrams.  
Raw sockets (SOCK_RAW)

In the Internet domain, datagram sockets employ the User Datagram Protocol
(UDP), and stream sockets (usually) employ the Transmission Control Protocol
(TCP).

Socket system call:  
socket()  
bind()  
listen()  
accept()  
connect()  

Socket I/O:
read(), write()  
send(), recv()  
sendto(), recvfrom()  

By default, these system calls block if the I/O operation can’t be completed
immediately. Nonblocking I/O is also possible, by using the fcntl() F_SETFL
operation to enable the O_NONBLOCK open file status flag.

--------------------------------------------------------------------------------

# Stream Sockets

<pre>
Passive socket:              Active socket:  
   (Server)                     (Client)  

  socket()                      socket()  
    |                              |  
  bind()                           |  
    |                              |  
  listen()                         |  
    |                              |  
  accept()                      connect()  
    |                              |  
  read()                         write()  
    |                              |  
  write()                        read()  
    |                              |  
  close()                        close()  
</pre>

# Datagram Sockets

`
  Server:                       Client:  

  socket()                      socket()  
    |                              |  
  bind()                           |  
    |                              |  
 recvfrom()                     sendto()  
    |                              |  
  sendto()                     recvfrom()  
    |                              |  
  close()                       close()  
`

We can use connect() with Datagram sockets.

--------------------------------------------------------------------------------



