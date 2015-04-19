
#include "utils.h"
#include "http_utils.h"

// The maximum length of HTTP request is 8190, according to Apache docs
const int HTTP_REQUEST_MAX_LENGTH = 8200;

HttpHeader make_http_header_from_string(const std::string &str)
{
	HttpHeader header;
	std::stringstream sstream(str);
	std::getline(sstream, header.request_status_line, '\n');

	std::string line;
	while (std::getline(sstream, line, '\n')) {
		int colon_pos = line.find(':');
		if (colon_pos != std::string::npos) {
			auto header_name = line.substr(0, colon_pos);
			auto header_value = line.substr(colon_pos + 1);
			header.headers[header_name] = header_value;
		} else {
			std::stringstream msgstream;
			msgstream << "Malformed HTTP header line: " << line;
			log(msgstream.str());
		}
	}

	return header;
}

HttpMessage* read_http_message_from_socket(int sd)
{
	char buffer[HTTP_REQUEST_MAX_LENGTH];
    int bytes_read = 0;

    std::string header_string;
    std::string body_string;

    // Reading HTTP header request from the client
    do
    {
        int bytes_read_this_iteration = recv(sd, buffer + bytes_read, sizeof(buffer), 0);
        if (bytes_read_this_iteration < 0) {
        	log("Error in recv() while reading data from the client's socket");
        	return nullptr;
        }
        bytes_read += bytes_read_this_iteration;

        // Check if the end of HTTP header was encountered
        char *headers_end = strnstr(buffer, "\r\n\r\n", bytes_read);
        if (headers_end != NULL) {
       		// Reached end of headers
			*headers_end = '\0';
			header_string = std::string(buffer);

			buffer[bytes_read] = '\0';
			if ((headers_end + 4 - buffer) < bytes_read) {
				// We've read a part of message's body - append it to body_string
				body_string += std::string(headers_end + 4);
			}

			break;
        }

    }
    while (true);

    HttpHeader http_header = make_http_header_from_string(header_string);

    // If there's Content-Length header, read the body
    if (http_header.headers.find("Content-Length") != http_header.headers.end()) {
    	int content_length = atoi(http_header.headers["Content-Length"].c_str());
    	do {
			int bytes_read_this_iteration = recv(sd, buffer, sizeof(buffer), 0);
	        if (bytes_read_this_iteration < 0) {
	        	log("Error in recv() while reading data from the client's socket");
	        	return nullptr;
	        }

	        if (bytes_read_this_iteration == 0) {
	        	// No more data from the client
	        	break;
	        } else {
	        	buffer[bytes_read_this_iteration] = '\0';
	        	body_string += std::string(buffer);
	        }
    	} while (true);
    } else {
    	body_string = "";
    }

    return new HttpMessage { http_header, body_string };
}

std::string HttpMessage::get_request_url() const 
{
	auto parts = split_all(this->header.request_status_line, ' ');
	if (parts.size() > 1) {
		return parts[1];
	} else {
		return "/";
	}
}

std::string HttpMessage::to_log_string() const
{
	std::stringstream sstream;
	sstream << "Status/request line:\n\t" << this->header.request_status_line << "\n" << "Headers:\n";
	for (auto iterator = this->header.headers.begin(); iterator != this->header.headers.end(); iterator++) {
    	sstream << "\t" << iterator->first << ": " << iterator->second << "\n";
	}
	sstream << "Body: " << this->body.size() << " bytes long, omitted in logs";
	return sstream.str();
}

std::string HttpMessage::to_string() const
{
	std::stringstream sstream;
	sstream << this->header.request_status_line << "\r\n";
	for (auto iterator = this->header.headers.begin(); iterator != this->header.headers.end(); iterator++) {
    	sstream << iterator->first << ": " << iterator->second << "\r\n";
	}
	sstream << "\r\n";
	sstream << this->body;
	return sstream.str();
}
