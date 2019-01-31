//
// Created by bilel fourati on 2019-01-31.
//

#pragma once

#include <variant>

namespace api {

namespace header {

class IHeading {
public:
	virtual ~IHeading() = default;

	/*virtual std::string &getHeader(const std::string &headerName) const = 0;

	virtual std::string &getStatusMessage() const = 0;

	virtual void setHeader(const std::string &key, const std::string &value) = 0;

	virtual void setStatusMessage(const std::string &message) = 0;*/
};


class Heading : public IHeading {
public:
	~Heading() = default;

	Heading() = default;

public:
	std::string str{"Content-Length: 28\r\n\r\n"};
};

struct request {
	std::string method;
	std::string path;
	std::string httpVersion;
};

struct response {
	std::string httpVersion;
	std::string statusCode;
	std::string message;
};

struct HTTPMessage {
	std::variant<request, response> variant;
	std::unique_ptr<IHeading> header;
	std::string body;
};

}

}
