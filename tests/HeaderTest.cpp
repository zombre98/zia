//
// Created by Thomas Burgaud on 2019-01-22.
//

#include "header/ResponseHeading.hpp"
#include <gtest/gtest.h>

TEST(Header, Header_Basic_header_Test) {
	header::ResponseHeading responseHeading(http::StatusCode::OK);
	EXPECT_EQ("HTTP/1.1 200\r\n\r\n", static_cast<std::string>(responseHeading));
}

TEST(Header, Header_Add_Set_Header_Test) {
	header::ResponseHeading responseHeading(http::StatusCode::OK);
	responseHeading.setHeader("Key", "Value");
	EXPECT_EQ("HTTP/1.1 200\r\nKey:Value\r\n\r\n", static_cast<std::string>(responseHeading));
	responseHeading.setHeader("Key", "Value1");
	EXPECT_EQ("HTTP/1.1 200\r\nKey:Value,Value1\r\n\r\n", static_cast<std::string>(responseHeading));
}

TEST(Header, Header_Add_Get_Header_Test) {
	header::ResponseHeading responseHeading(http::StatusCode::OK);
	responseHeading.setHeader("Key", "Value");
	EXPECT_EQ("Value", responseHeading.getHeader("Key"));
	responseHeading.setHeader("Key", "Value1");
	EXPECT_EQ("Value,Value1", responseHeading.getHeader("Key"));
	EXPECT_EQ(std::nullopt, responseHeading.getHeader("Unset_Key"));
}

TEST(Header, Header_set_and_get_Status) {
	header::ResponseHeading responseHeading(http::StatusCode::OK);
	EXPECT_EQ(http::StatusCode::OK, responseHeading.getStatusCode());
	responseHeading.setStatusCode(http::StatusCode::Accepted);
	EXPECT_EQ(http::StatusCode::Accepted, responseHeading.getStatusCode());
}

TEST(Header, Header_operator_hook) {
	header::ResponseHeading responseHeading(http::StatusCode::OK);
	responseHeading["Key"] = "Value";
	EXPECT_EQ("Value", responseHeading.getHeader("Key"));
	responseHeading["Key"] = "Value1";
	EXPECT_EQ("Value1", responseHeading.getHeader("Key"));
	EXPECT_EQ("Value1", responseHeading["Key"]);
	EXPECT_EQ("", responseHeading["Unset_Key"]);
}
