#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <unistd.h>
#include <pthread.h>

#include "bbuffer.h"
#include "sem.h"

int port;
char *request;
char *www_path;
int threads;
int bufferslots;

char http_header[2048];
BNDBUF *bb;

char readFile(char *file_path)
/** Method for opening/reading file **/
{

    FILE *html_data; // Create file object to hold information from file.
    char buffer[128];
    char response_data[1024] = {'\0'};     // To store data
    if (html_data = fopen(file_path, "r")) // Try and check file with read-rights
    {
        strcat(http_header, "HTTP/1.1 200 OK\r\n\n");
        printf("File found!\n");
        while (fgets(buffer, sizeof(buffer), html_data)) // Read multiline
        {
            strcat(response_data, buffer);
        }
        strcat(http_header, response_data);
    }

    else
    {
        strcat(http_header, "HTTP/1.1 404 Not Found\r\n\n");
        printf("Error. Did not find file!\n");
    }
    fclose(html_data);

    return http_header;
}

void runServer()
/** Initializing and running server socket.
    Binding and listening on given socket. **/
{
    int server_socket, client_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Define the server address, where the server will sereve data.
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;   // Specificizes it's internet protocol
    server_address.sin_port = htons(port); // Setting the portnumber
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)); // Bind the socket to our specified IP and port.
    listen(server_socket, 5);                                                        // Listen after request on given socket. 2. arg, how many socekts can be waiting for this socket.

    // Infinite while-loop. Server should continue serving and responding to request.
    while (1)
    {
        client_socket = accept(server_socket, NULL, NULL); // After listen and accpeted, we get back the client socket
        // send message to client socket
        printf("Socket accepted connection: %d\n", client_socket);
        bb_add(bb, client_socket); // Adding client to the ring buffer
    }
    // Close the socket
    close(server_socket);
}

int manageConnection(int client_socket)
/* Managing connected and accepted client socket*/
{
    send(client_socket, http_header, sizeof(http_header), 0);
    close(client_socket);
}

void *threadFunction()
/* Threads "popping" off and serving clients from  ring buffer*/
{
    while (1)
    {
        int client_socket = bb_get(bb); // After listen and accpeted, we get back the client socket
        //  send message to client socket
        manageConnection(client_socket);
        printf("[Thread %lu] Handling client %d\n", (int unsigned long)pthread_self(), client_socket);
    }
}

int main(int argc, char *argv[])
{
    if (argc > 4)
    {
        www_path = realpath(argv[1], NULL); // Creating an absoulte path from relative path
        port = atoi(argv[2]);
        threads = atoi(argv[3]);
        bufferslots = atoi(argv[4]);
    }
    else
    {
        printf("Must conatin path, portnumber, threads and bufferslots\n "); // error not working?
    }

    printf("\t Path: %s\n", www_path);
    printf("\t Port number: %d\n", port);
    printf("\t Threads: %d\n", threads);
    printf("\t Bufferslots: %d\n\n", bufferslots);

    int i;
    pthread_t th[threads];
    bb = bb_init(bufferslots);

    for (i = 0; i < threads; i++) // Creating threads
    {
        if (pthread_create(&th[i], NULL, &threadFunction, NULL) != 0)
        {
            perror("Failed to create thread");
        }
    }

    readFile(www_path);
    runServer();

    for (i = 0; i < threads; i++) // Joining threads, main thread waits before return
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Failed to join thread");
        }
    }
    return 0;
}