//
// Created by Thomas Burgaud on 2019-03-09.
//

#include <gtest/gtest.h>
#include "Utils/Utils.hpp"

TEST(Split, Split_Basic_Test) {
	std::string value("A string");

	auto returnSplit = zia::utils::split(std::move(value), ' ');
	std::vector<std::string> expectedValue{"A", "string"};
	EXPECT_EQ(expectedValue, returnSplit);
}

TEST(Split, Split_mutltiple_split_Test) {
	std::string value("This is a test string");

	auto returnSplit = zia::utils::split(std::move(value), ' ');
	std::vector<std::string> expectedValue{"This", "is", "a", "test", "string"};
	EXPECT_EQ(expectedValue, returnSplit);
}

TEST(Split, Split_with_multiple_char_Test) {
	std::string value("This:is:a:test:string");

	auto returnSplit = zia::utils::split(std::move(value), ':');
	std::vector<std::string> expectedValue{"This", "is", "a", "test", "string"};
	EXPECT_EQ(expectedValue, returnSplit);
}

TEST(Split, Split_with_unexpected_char_Test) {
	std::string value("This:is:a:test:string");

	auto returnSplit = zia::utils::split(std::move(value), '.');
	std::vector<std::string> expectedValue{"This:is:a:test:string"};
	EXPECT_EQ(expectedValue, returnSplit);
}

TEST(Split, Split_with_zero_Test) {
	std::string value("This:is:a:test:string");

	auto returnSplit = zia::utils::split(std::move(value), 0);
	std::vector<std::string> expectedValue{"This:is:a:test:string"};
	EXPECT_EQ(expectedValue, returnSplit);
}
