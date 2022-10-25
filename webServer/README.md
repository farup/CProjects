# Web Server 

## How to run  
**1. Compile program**
```bash
C:\Users\...>gcc -pthread -o mtwwd mtwwd.c bbuffer.c sem.c
C:\Users\...>./mtwwd doc/index.html 8009 10 5
```

## Introduction 

Sockets are low level endpoint used for processing information across a network.
Network protocol HTTP rely on sockets underneath to make connections. 
Server waits for incoming client request by listening to a specified port.
Once a request is received, the server acceps a connection from the client socke to complete the connection.

This project focuses on creating a multithreaded socket. The client sockets are cordinated with self-implemented ringbuffer and semaphores. Program reads a .html file and delivers it to the client. 



## My Solution - Overview 

The program creates a "pool" of threads and assigning each a threadfunction which manages client sockets. The clients to be managed are popped of the ringbuffer, organised by increamenting and decrementing the semaphores.

Below follows some code snippets from mtwwd.c 

```bash 
bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
listen(server_socket, 5); // 2. arg; how many socekts can be waiting for this socket   
```
Code snippet shows bind of server socket to previous determined address, listen for connections.



```bash 
bb_add(bb, client_socket)  
```
Then establish an infite loop which, accepts connections, and handles those by passing to the ring buffer. 


```bash 
int client_socket = bb_get(bb);
```
... and popped of in the *threadFunction() 

Threads are also joined, such that they main function waits before returing. 



















