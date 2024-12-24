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

int send_message(int sock, const char *message)
{
    if (send(sock, message, strlen(message), 0) < 0)
    {
        perror("Failed to send message");
        return -1;
    }
    printf("Message sent: %s\n", message);
    return 0;
}

int receive_response(int sock, char *buffer)
{
    int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0)
    {
        perror("Failed to receive message");
        return -1;
    }

    buffer[bytes_received] = '\0'; // Null-terminate the received data
    printf("Server response: %s\n", buffer);
    return 0;
}

void communication_loop(int sock)
{
    char message[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    while (1)
    {
        // Prompt the user for input
        printf("Enter message to send to server (type 'exit' to quit): ");
        fgets(message, BUFFER_SIZE, stdin);

        // Remove newline character from the input
        message[strcspn(message, "\n")] = '\0';

        // Check if the user wants to exit
        if (strcmp(message, "exit") == 0)
        {
            printf("Exiting...\n");
            break;
        }

        // Send the message to the server
        if (send_message(sock, message) < 0)
        {
            break;
        }

        // Receive the server's response
        if (receive_response(sock, buffer) < 0)
        {
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    ////////////////////
    // INITIALIZATION //
    ////////////////////
    int port = DEFAULT_PORT;
    const char *ip_address = DEFAULT_IP;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];

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

    ///////////////////
    // COMMUNICATION //
    ///////////////////
    communication_loop(sock);

    printf("Connected to server at %s:%d\n", ip_address, port);

    /////////////////
    // TERMINATION //
    /////////////////
    close(sock);
    return 0;
}