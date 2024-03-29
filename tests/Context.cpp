//
// Created by Thomas Burgaud on 2019-03-09.
//

#include <gtest/gtest.h>
#include "server/header/ResponseHeading.hpp"
#include "server/api/Stage.hpp"

TEST(Context, Context_construct_from_string) {
	std::string stringHeader("GET /index.html HTTP/1.1");
	dems::Context context;

	dems::header::fillHeading(stringHeader, context, *context.request.headers);
	auto &firstline = std::get<dems::header::Request>(context.request.firstLine);
	EXPECT_EQ("GET", firstline.method);
	EXPECT_EQ("/index.html", firstline.path);
	EXPECT_EQ("HTTP/1.1", firstline.httpVersion);
}

TEST(Context, Context_construct_from_string_with_error) {
	std::string stringHeader("random string");
	dems::Context context;

	dems::header::fillHeading(stringHeader, context, *context.request.headers);
	auto &firstline = std::get<dems::header::Request>(context.request.firstLine);
	EXPECT_EQ("", firstline.method);
	EXPECT_EQ("", firstline.path);
	EXPECT_EQ("", firstline.httpVersion);
}

TEST(Context, Context_construct_from_string_with_headers) {
	std::string stringHeader("GET /index.html HTTP/1.1\r\n"
														"key: value\r\n");
	dems::Context context;
	context.request.headers = std::make_unique<dems::header::Heading>();

	dems::header::fillHeading(stringHeader, context, *context.request.headers);
	auto &firstline = std::get<dems::header::Request>(context.request.firstLine);
	EXPECT_EQ("GET", firstline.method);
	EXPECT_EQ("/index.html", firstline.path);
	EXPECT_EQ("HTTP/1.1\r", firstline.httpVersion);
	EXPECT_EQ("value", context.request.headers->getHeader("key"));
}

TEST(Context, Context_construct_from_string_with_multiple_headers) {
	std::string stringHeader("GET /index.html HTTP/1.1\r\n"
													 "key: value,othervalue\r\n"
													 "otherkey: anothervalue\r\n");
	dems::Context context;
	context.request.headers = std::make_unique<dems::header::Heading>();

	dems::header::fillHeading(stringHeader, context, *context.request.headers);
	auto &firstline = std::get<dems::header::Request>(context.request.firstLine);
	EXPECT_EQ("GET", firstline.method);
	EXPECT_EQ("/index.html", firstline.path);
	EXPECT_EQ("HTTP/1.1\r", firstline.httpVersion);
	EXPECT_EQ("value,othervalue", context.request.headers->getHeader("key"));
	EXPECT_EQ("anothervalue", context.request.headers->getHeader("otherkey"));
}

TEST(Context, Context_construct_from_string_with_body_headers) {
	std::string stringHeader("POST /script.php HTTP/1.1\r\n"
													 "key: value,othervalue\r\n"
													 "otherkey: anothervalue\r\n\r\n"
													"This is a body with random value");
	dems::Context context;
	context.request.headers = std::make_unique<dems::header::Heading>();

	dems::header::fillHeading(stringHeader, context, *context.request.headers);
	auto &firstline = std::get<dems::header::Request>(context.request.firstLine);
	EXPECT_EQ("POST", firstline.method);
	EXPECT_EQ("/script.php", firstline.path);
	EXPECT_EQ("HTTP/1.1\r", firstline.httpVersion);
	EXPECT_EQ("value,othervalue", context.request.headers->getHeader("key"));
	EXPECT_EQ("anothervalue", context.request.headers->getHeader("otherkey"));
	EXPECT_EQ("This is a body with random value", context.request.body);
}
