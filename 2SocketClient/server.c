#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT 8000
#define BUFFER_SIZE 1024

// Function Prototypes
int create_server_socket();
void bind_and_listen(int server_sock, const char *ip, int port);
void handle_client(int client_sock);
void start_server(int server_sock);

int main(int argc, char *argv[])
{
    const char *ip_address = DEFAULT_IP;
    int port = DEFAULT_PORT;

    // Handle command-line arguments
    if (argc >= 2)
    {
        ip_address = argv[1];
    }
    if (argc >= 3)
    {
        port = atoi(argv[2]);
    }

    // Create the server socket
    int server_sock = create_server_socket();

    // Bind the socket and start listening
    bind_and_listen(server_sock, ip_address, port);

    // Start accepting and handling client connections
    start_server(server_sock);

    // Close the server socket
    close(server_sock);
    return 0;
}

// Function to create a server socket
int create_server_socket()
{
    int server_sock;
    printf("Creating socket...\n");
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully (fd: %d).\n", server_sock);
    return server_sock;
}

// Function to bind the socket and start listening
void bind_and_listen(int server_sock, const char *ip, int port)
{
    struct sockaddr_in server_addr;

    // Set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Convert IP string to binary form
    printf("Binding to IP %s and port %d...\n", ip, port);
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid IP address");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Bind the socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    printf("Socket bound to IP %s and port %d.\n", ip, port);

    // Start listening for incoming connections
    printf("Listening for incoming connections...\n");
    if (listen(server_sock, 5) < 0)
    {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
}

// Function to handle communication with a client
void handle_client(int client_sock)
{
    char buffer[BUFFER_SIZE];
    int bytes_read;

    printf("Client connected.\n");

    // Echo back messages from the client
    while ((bytes_read = read(client_sock, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytes_read] = '\0'; // Null-terminate the received data
        printf("Received: %s\n", buffer);

        // Echo the message back to the client
        if (write(client_sock, buffer, bytes_read) < 0)
        {
            perror("Error sending data to client");
            break;
        }
    }

    if (bytes_read < 0)
    {
        perror("Error reading from client");
    }

    printf("Client disconnected.\n");
    close(client_sock);
}

// Function to start accepting and handling client connections
void start_server(int server_sock)
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_sock;

    printf("Server is running and listening for connections.\n");

    while (1)
    {
        printf("Waiting for a new connection...\n");
        if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len)) < 0)
        {
            perror("Accept failed");
            continue;
        }

        // Print client information
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("New connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        // Handle the client in a separate function
        handle_client(client_sock);
    }
}
