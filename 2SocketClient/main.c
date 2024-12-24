#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT 8000
#define BUFFER_SIZE 1024

int setup_connection(const char *ip_addr, int port)
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    /*
    struct sockaddr_in {
        short            sin_family;    // AF_INET
        unsigned short   sin_port;      // Port number
        struct in_addr   sin_addr;      // IP address
        char             sin_zero[8];   // Padding
    };
    */

    // CREATE SOCKET //
    printf("Creating socket...\n");
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock <= 0)
    {
        /*
            AF_INET: IPv4 Internet protocols
            SOCK_STREAM: TCP connection type (reliable, ordered)
            0: Default protocol (TCP for SOCK_STREAM)
            Returns: socket file descriptor (like a file handle) or -1 on error
        */
        perror("Socket creation error\n");
        return -1;
    }
    printf("Socket created successfully (fd: %d)\n", sock);

    //  => SETUP SERVER ADDR
    printf("Setting up server address structure...\n");
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    printf("Port set to: %d\n", port);

    //  => CONVERT IP -> bin
    printf("Converting IP address %s...\n", ip_addr);
    int pton_result = inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr);
    if (pton_result <= 0)
    {
        perror("inet_pton failed with error\n");
        close(sock);
        return -1;
    }
    printf("IP address converted successfully\n");

    // CONNECT TO SERVER //
    printf("Attempting to connect to %s:%d...\n", ip_addr, port);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection failed with error\n");
        close(sock);
        return -1;
    }

    return sock;
}

int main(int argc, char *argv[])
{
    ////////////////////
    // INITIALIZATION //
    ////////////////////
    const char *ip_address = DEFAULT_IP;
    int port = DEFAULT_PORT;

    // CmdLine interaction
    if (argc >= 2)
    {
        ip_address = argv[1];
    }
    if (argc >= 3)
    {
        port = atoi(argv[2]);
    }

    ////////////////
    // CONNECTION //
    ////////////////
    int sock = setup_connection(ip_address, port);
    if (sock < 0)
    {
        return 1;
    }

    printf("Connected to server at %s:%d\n", ip_address, port);

    /////////////////
    // TERMINATION //
    /////////////////
    close(sock);
    return 0;
}