//
// Created by Thomas Burgaud on 2019-03-09.
//

#include <gtest/gtest.h>
#include "server/header/ResponseHeading.hpp"
#include "server/api/Stage.hpp"

TEST(Reponse, Reponse_basic_test) {
	dems::Context context;
	context.response.headers = std::make_unique<dems::header::Heading>();

	context.response.firstLine = dems::header::Response{"HTTP/1.1", "200", ""};
	auto response = dems::header::constructResponse(context);
	EXPECT_EQ("HTTP/1.1 200 \r\n\r\n", response);
}

TEST(Reponse, Reponse_basic_with_message_test) {
	dems::Context context;
	context.response.headers = std::make_unique<dems::header::Heading>();

	context.response.firstLine = dems::header::Response{"HTTP/1.1", "200", "OK"};
	auto response = dems::header::constructResponse(context);
	EXPECT_EQ("HTTP/1.1 200 OK\r\n\r\n", response);
}

TEST(Reponse, Reponse_with_message_header_test) {
	dems::Context context;
	context.response.headers = std::make_unique<dems::header::Heading>();

	context.response.firstLine = dems::header::Response{"HTTP/1.1", "200", "OK"};
	context.response.headers->setHeader("key", "value");
	context.response.headers->setHeader("anotherkey", "value1, value2");
	auto response = dems::header::constructResponse(context);
	EXPECT_EQ("HTTP/1.1 200 OK\r\nanotherkey:value1, value2\r\nkey:value\r\n\r\n", response);
}

TEST(Reponse, Reponse_with_message_body_test) {
	dems::Context context;
	context.response.headers = std::make_unique<dems::header::Heading>();

	context.response.firstLine = dems::header::Response{"HTTP/1.1", "200", "OK"};
	context.response.headers->setHeader("key", "value");
	context.response.headers->setHeader("anotherkey", "value1, value2");
	context.response.body = "This is the response body";
	auto response = dems::header::constructResponse(context);
	EXPECT_EQ("HTTP/1.1 200 OK\r\nanotherkey:value1, value2\r\nkey:value\r\n\r\nThis is the response body", response);
}

TEST(Reponse, Reponse_with_message_body_without_header_test) {
	dems::Context context;
	context.response.headers = std::make_unique<dems::header::Heading>();

	context.response.firstLine = dems::header::Response{"HTTP/1.1", "200", "OK"};
	context.response.body = "This is the response body";
	auto response = dems::header::constructResponse(context);
	EXPECT_EQ("HTTP/1.1 200 OK\r\n\r\nThis is the response body", response);
}
