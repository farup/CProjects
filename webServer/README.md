# Web Server 


## Introduction 

Sockets are low level endpoint used for processing information across a network.
Network protocol HTTP rely on sockets underneath to make connections. 
Server waits for incoming client request by listening to a specified port.
Once a request is received, the server acceps a connection from the client socke to complete the connection.

This project focuses on creating a multithreaded socket, which cordinates request with a ring buffer and semaphores. 



## My Solution - Quick Overview 

Bind server socket to previous determined addrres, listen for connections.
```bash 
bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
listen(server_socket, 5); // 2. arg; how many socekts can be waiting for this socket   
```




## Test 1 
**1. Compile program**
```bash
C:\Users\...>gcc -pthread -o mtwwd mtwwd.c bbuffer.c sem.c
C:\Users\...>./mtwwd doc/index.html 8009 10 5
```

gcc -pthread -o mtwwd mtwwd.c bbuffer.c sem.c
./mtwwd doc/index.html 8009 10 5
Code snippet above show how to compile and run code.














