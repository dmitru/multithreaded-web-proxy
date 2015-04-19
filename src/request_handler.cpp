
#include "request_handler.h"
#include "utils.h"
#include "http_utils.h"

void* handle_client_connection(void* arg)
{
    HostInfo *client_info = (HostInfo *)arg;
    int client_sd = client_info->socket_fd;

	HttpMessage *http_message = read_http_message_from_socket(client_sd);

	std::stringstream msg_stream;
	msg_stream << "Received request from " << client_info->hostname << ":" << client_info->port << ":\n"
			   << http_message->to_log_string();
	log(msg_stream.str());

	std::string request_url = http_message->get_request_url().substr(1);
	std::vector<std::string> url_parts = split(request_url, '/');
	std::string redirect_to = url_parts[0];
	std::string redirect_path = "/" + url_parts[1];

	log("Redirecting message to: " + redirect_to);
	log("Path on target server: " + redirect_path);

    close(client_sd);

    delete client_info;
    delete http_message;

    return arg;
}