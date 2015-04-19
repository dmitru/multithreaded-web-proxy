
#pragma once

#include "utils.h"

struct HttpHeader {
	std::string request_status_line;
	std::map<std::string, std::string> headers;
};

struct HttpMessage {
	HttpHeader header;
	std::string body;

	std::string to_string() const;
};

HttpMessage* read_http_message_from_socket(int socket_descriptor);