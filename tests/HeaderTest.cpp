//
// Created by Thomas Burgaud on 2019-01-22.
//

#include "server/api/Heading.hpp"
#include "server/header/ResponseHeading.hpp"
#include <gtest/gtest.h>

TEST(Header, Header_Basic_header_Test) {
	dems::header::Heading responseHeading;

	responseHeading.setHeader("key", "value");
	EXPECT_EQ("value", responseHeading.getHeader("key"));
}

TEST(Header, Header_Add_Set_Header_Test) {
	dems::header::Heading responseHeading;

	responseHeading.setHeader("Key", "Value");
	EXPECT_EQ("Value", responseHeading.getHeader("Key"));
	responseHeading.setHeader("Key", "Value1");
//	EXPECT_EQ("Value,Value1", responseHeading.getHeader("Key"));
}

TEST(Header, Header_Add_Get_Header_Test) {
	dems::header::Heading responseHeading;

	responseHeading.setHeader("Key", "Value");
	EXPECT_EQ("Value", responseHeading.getHeader("Key"));
	responseHeading.setHeader("Key", "Value1");
	//EXPECT_EQ("Value,Value1", responseHeading.getHeader("Key"));
	EXPECT_EQ("", responseHeading["Unset_Key"]);
}

TEST(Header, Header_operator_hook) {
	dems::header::Heading responseHeading;

	responseHeading["Key"] = "Value";
	EXPECT_EQ("Value", responseHeading.getHeader("Key"));
	responseHeading["Key"] = "Value1";
	EXPECT_EQ("Value1", responseHeading.getHeader("Key"));
	EXPECT_EQ("Value1", responseHeading["Key"]);
	EXPECT_EQ("", responseHeading["Unset_Key"]);
}

TEST(Header, Header_reference) {
	dems::header::Heading responseHeading;

	responseHeading["Key"] = "Value";
	EXPECT_EQ("Value", responseHeading["Key"]);
	auto &ref = responseHeading["Key"];
	ref = "Modified value";
	EXPECT_EQ("Modified value", responseHeading["Key"]);
}

TEST(Header, Header_getWholeHeader) {
	dems::header::Heading responseHeading;

	responseHeading["Key"] = "Value";
	responseHeading.setHeader("Key1", "Value1");
	EXPECT_EQ("Key1:Value1\r\nKey:Value\r\n", responseHeading.getWholeHeaders());
}
