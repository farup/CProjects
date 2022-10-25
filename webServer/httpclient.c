#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h> 
#include <arpa/inet.h> // Contains function for converting network addresses. 


/*
#include <unistd.h>
#include <netdb.h>

#include <string.h>
#include <sys/stat.h>
#include <fonti.h>
*/

int main(int argc, char *argv) {

    char *address; 
    address = argv[1]; // Store the first command line argument in that pointer/char
    
    /* Socket endpoint for client */
    // Create a socket 
    int client_socket; 
    client_socket = socket(AF_INET, SOCK_STREAM, 0); //Internet socekt, TCP socekt Stream 

    // Specify an address for the socket
    struct sockaddr_in remote_address; 
    remote_address.sin_family = AF_INET; // Set family of address, knows what type it is
    remote_address.sin_port = htons(8001); // Port want to connect to remotly
    
    // Converst string address to format for address struct. Remote address is or struct storing, the address information sin etc...
    inet_aton(address, &remote_address.sin_addr.s_addr); 
    
    connect(client_socket, (struct sockaddr *) &remote_address, sizeof(remote_address)); // Caster sockeaddr_in til sockaddr (& = address of)

    // recive data form the server
    char request[] = "GET / HTTP/1.1\r\n\r\n"; // '/'get root location of this server ish
    char response[4096];

    send(client_socket, request, sizeof(request), 0); 
    recv(client_socket, &response, sizeof(response), 0); 

    //print what we got back fro the server
    printf("Response from the server: %s\n", response); 

    //close the socket
    close(client_socket);
    return 0; 
}