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
