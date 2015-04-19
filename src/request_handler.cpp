
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
			   << http_message->to_log_string();
	log(msg_stream.str());

	auto request_url = http_message->get_request_url().substr(1);
	auto url_parts = split(request_url, '/');
	auto redirect_to = url_parts[0];
	auto redirect_path = "/" + url_parts[1];

	log("Redirecting message to: " + redirect_to);
	log("Path on target server: " + redirect_path);

    close(client_sd);

    delete client_info;
    delete http_message;

    return arg;
}