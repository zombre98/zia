#include <gtest/gtest.h>
#include "server/buffer.hpp"

TEST(Buffer, Buffer_Buff_Int_Test) {
	zia::Buffer buff;

	buff.write<int>(4);
	auto value = buff.read<int>();
	EXPECT_EQ(4, value);
}

TEST(Buffer, Buffer_Buff_String_Test) {
	zia::Buffer buff;

	buff.write<std::string>("Value");
	auto value = buff.read<std::string>();
	EXPECT_EQ("Value", value);
}

TEST(Buffer, Buffer_Buff_Clear_Test) {
	zia::Buffer buff;

	buff.write<std::string>("Value");
	buff.clear();
	auto value = buff.read<std::string>();
	EXPECT_EQ("", value);
}

TEST(Buffer, Buffer_Buff_Get_Wroted_Size_String_Test) {
	zia::Buffer buff;
	auto str = std::string("Value");

	buff.write<std::string>(str);
	auto value = buff.getWroteSize();
	EXPECT_EQ(str.length(), value);
}

TEST(Buffer, Buffer_Buff_Get_Wroted_Size_Int_Test) {
	zia::Buffer buff;
	auto nb = 5;

	buff.write<int>(nb);
	auto value = buff.getWroteSize();
	EXPECT_EQ(sizeof(nb), value);
}

/* TEST(Buffer, Buffer_Buff_Set_Data_String_Test) {
	zia::Buffer buff;
	auto data = std::string("Some Data");

	buff.setData<std::string>(&data, data.length());
	auto value = buff.getWroteSize();
	EXPECT_EQ(data.length(), value);
	EXPECT_EQ(data, buff.read<std::string>());
}

TEST(Buffer, Buffer_Buff_Set_Data_int_Test) {
	zia::Buffer buff;
	auto data = 1234567;

	buff.setData<int>(&data, sizeof(data));
	auto value = buff.getWroteSize();
	EXPECT_EQ(sizeof(data), value);
	EXPECT_EQ(data, buff.read<int>());
} */
