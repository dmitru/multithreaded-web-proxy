
#pragma once

#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>

/*
 Structures used throughout the projects
*/ 

struct ParsedArguments
{
    int port;
};

struct HostInfo {
  std::string hostname;
  int port;
  int socket_fd;
};

/*
 I/O helpers - options parsing, logging, etc
*/

ParsedArguments parse_arguments(int argc, char *argv[]);

void log(const std::string &msg);

void print_error_and_die(const std::string &msg, int exit_status=1);

void print_usage_and_die(int exit_status=1);

/*
 Wrappers around socket operations 
*/

HostInfo* wait_for_client_and_accept(int listening_socket);

struct sockaddr_in create_listening_socket_address(const ParsedArguments &arguments);

int create_listening_socket(struct sockaddr_in *socket_address);

int create_socket_to_server(const std::string &hostport);
int create_socket_to_server(const std::string &host, int port);

int send_to_socket(int target_sockfd, const std::string &data);

/*
  Miscelleneous
*/
std::vector<std::string> split(std::string source, char delimiter);
std::vector<std::string> split_all(std::string source, char delimiter);


