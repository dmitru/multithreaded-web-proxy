
#include "request_handler.h"
#include "utils.h"
#include "http_utils.h"

void* handle_client_connection(void* arg)
{
    HostInfo *client_info = (HostInfo *)arg;
    int client_sd = client_info->socket_fd;

    // Receive incoming client's request
	HttpMessage *http_message = read_http_message_from_socket(client_sd);

	std::stringstream msg_stream;
	msg_stream << "Received request from " << client_info->hostname << ":" << client_info->port << ":\n"
			   << http_message->to_log_string();
	log(msg_stream.str());

	// Extract request path on the target server that client wishes to access
	std::string request_path = http_message->get_request_url();
	std::vector<std::string> url_parts = split(request_path.substr(1), '/');
	std::string redirect_to = url_parts[0];
	std::string redirect_path = "/" + url_parts[1];

	log("Redirecting message to: " + redirect_to);
	log("Path on target server: " + redirect_path);

	// Modify the HTTP message before sending it to the target server
	HttpMessage redirected_message(*http_message);
	size_t request_path_pos = redirected_message.header.request_status_line.find(request_path);
	redirected_message.header.request_status_line.replace(request_path_pos, request_path.size(), redirect_path);
	redirected_message.header.headers["Host"] = redirect_to;
	//redirected_message.header.headers["Connection"] = "close";

	log("Redirected request to " + redirect_to + ":\n" + redirected_message.to_log_string());

	// Send the modified HTTP message to target server
	int target_sockfd = create_socket_to_server(redirect_to);
	if (target_sockfd < 0)
	{
		// An error occured, TODO: send HTTP 500 back to client
		return NULL;
	}

	log("Sending message to target server...");
	if (send_to_socket(target_sockfd, redirected_message.to_string()) < 0) {
		log("Error while sending modified HTTP message to target server");
		return NULL;
	}
	log("done");

	// Receive target server's reply
	HttpMessage *http_response_from_target_server = read_http_message_from_socket(target_sockfd);
	if (http_response_from_target_server == NULL) {
		return NULL;
	}

	log("Received response from target server:\n'" + http_response_from_target_server->to_log_string() + "'");

	// Modify the reply before sending it back to the client

	// Send the target server's reply to the client
	if (send_to_socket(client_sd, http_response_from_target_server->to_string()) < 0) {
		log("Error while sending target server's reply back to client");
		return NULL;
	}

	// Close the client connection, clean up the resources
    close(client_sd);
    delete client_info;
    delete http_message;

    return NULL;
}