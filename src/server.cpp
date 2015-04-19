#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <pthread.h>

const char *USAGE_STRING = "Usage: ./server <PORT NUMBER>";

void log(const std::string &msg)
{
    auto timestamp = std::time(NULL);
    auto timestamp_string = std::string(std::asctime(std::localtime(&timestamp)));
    timestamp_string.erase(timestamp_string.size() - 2);
    std::cout << "[ " << timestamp_string << " ]: " << msg << std::endl;
}

void print_error_and_die(const std::string &msg, int exit_status=1)
{
    perror(msg.c_str());
    exit(exit_status);
}

void print_usage_and_die(int exit_status=1)
{
    std::cerr << USAGE_STRING << std::endl;
    exit(exit_status);
}

struct ParsedArguments
{
    int port;
};

ParsedArguments parse_arguments(int argc, char *argv[])
{
    if (argc > 2) {
        print_usage_and_die();
    }

    ParsedArguments arguments;
    arguments.port = (argc > 1)? atoi(argv[1]) : 8888;
    return arguments;
}

int create_listening_socket(struct sockaddr_in *socket_address)
{
    int listening_socket;

    if ((listening_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0 ) 
        print_error_and_die("Error while creating socket");

    int on = 1;
    if (setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        print_error_and_die("Error while calling setsockopt(...,SO_REUSEADDR,...) for listening socket");
    }

    if (bind(listening_socket, (struct sockaddr*)socket_address, sizeof(*socket_address)) != 0) {
        std::stringstream ss;
        ss << "Error while binding server socket to the port " << socket_address->sin_port;
        print_error_and_die(ss.str());
    }

    if (listen(listening_socket, 32) != 0 )
        print_error_and_die("Error while in listen() call");

    return listening_socket;
}

struct sockaddr_in create_listening_socket_address(const ParsedArguments &arguments)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(arguments.port);
    addr.sin_addr.s_addr = INADDR_ANY;
    return addr;
}

struct HostInfo {
  std::string hostname = "";
  int port = -1;
  int socket_fd;
};

HostInfo* wait_for_client_and_accept(int listening_socket)
{
    auto *client_info = new HostInfo();
  
    struct sockaddr_in client_address;
    auto addr_size = static_cast<socklen_t>(sizeof(client_address));
    client_info->socket_fd = accept(listening_socket, (struct sockaddr*)&client_address, &addr_size);

    char client_host[INET_ADDRSTRLEN];
    if (inet_ntop(AF_INET, &client_address.sin_addr.s_addr, client_host, sizeof(client_host)) != NULL) {
       client_info->hostname = inet_ntoa(client_address.sin_addr);
       client_info->port = ntohs(client_address.sin_port);
       std::stringstream ss;
       ss << "New connection from " << client_info->hostname << ":" << client_info->port;
       log(ss.str());
    } else {
       log("New connection, but unable to get address of the client");
    }

    return client_info;
}

/**
 * This function is called in separate thread for each client request and
 * processes the request.
 */
void* client_connection_worker(void* arg)
{   char line[100];
    int bytes_read;
    auto client_info = (HostInfo *)arg;
    auto client_sd = client_info->socket_fd;

    do
    {
        bytes_read = recv(client_sd, line, sizeof(line), 0);
        send(client_sd, line, bytes_read, 0);
    }
    while (strncmp(line, "bye\r", 4) != 0);
    close(client_sd);

    delete client_info;

    return arg;
}

int main(int argc, char* argv[])
{
    ParsedArguments arguments = parse_arguments(argc, argv);
    log("Launching server...");

    struct sockaddr_in listening_socket_address = create_listening_socket_address(arguments);
    int listening_socket = create_listening_socket(&listening_socket_address);

    log("Started listening to client connecitons " + std::to_string(arguments.port));

    // Entering the main loop, waiting for clients to connect
    while (true)
    {
        auto client_info = wait_for_client_and_accept(listening_socket);

        pthread_t client_thread;
        if ( pthread_create(&client_thread, NULL, client_connection_worker, client_info) != 0 ) {
            print_error_and_die("Error while spawning thread for processing client's request");
        } else {
            pthread_detach(client_thread);
        }
    }
}
