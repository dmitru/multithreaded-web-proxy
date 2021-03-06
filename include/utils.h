
#pragma once

#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <cctype>
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 Structures used throughout the projects
*/ 

struct ParsedArguments
{
    int port;
    std::string sites_blocklist_filename;
    std::string filter_words_list_filename;
    std::string cache_directory_path;
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

inline std::string trim(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isspace(*it))
        it++;

    std::string::const_reverse_iterator rit = s.rbegin();
    while (rit.base() != it && isspace(*rit))
        rit++;

    return std::string(it, rit.base());
}

bool is_host_blocked(const std::string &hostname, const std::string &sites_blocked_list_file);
std::string filter_words(const std::string &str, const std::string &filtered_words_list);

char rand_char();
std::string random_string(size_t length);

char *strnstr(const char *haystack, const char *needle, size_t len);

