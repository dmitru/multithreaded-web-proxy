
#include "request_handler.h"
#include "utils.h"
#include "http_utils.h"

void* handle_client_connection(void* arg)
{
    auto client_info = (HostInfo *)arg;
    auto client_sd = client_info->socket_fd;

	auto *http_message = read_http_message_from_socket(client_sd);

	std::stringstream msg_stream;
	msg_stream << "Received request from " << client_info->hostname << ":" << client_info->port << ":\n"
			   << http_message->to_string();
	log(msg_stream.str());

    close(client_sd);

    delete client_info;

    return arg;
}