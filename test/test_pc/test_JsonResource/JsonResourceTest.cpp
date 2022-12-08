#include "System/JsonResource.h"
#include "System/JsonResource.cpp"

#include "Logging/Log.h"
#include "Logging/Log.cpp"

#include <gtest/gtest.h>
#include <filesystem>

System::JsonResource uut("JsonResourceTest.json#/test");

TEST(JsonResourceTest, filePath)
{
    EXPECT_STREQ("JsonResourceTest.json", uut.getFilePath().c_str());
    std::string newPath = "JsonResourceTest1.json";
    uut.setFilePath(newPath);
    EXPECT_EQ(newPath, uut.getFilePath());
}


TEST(JsonResourceTest, jsonPointer)
{
    EXPECT_STREQ("/test", uut.getJsonPointer().to_string().c_str());
    json::json_pointer newJsonPtr = "/test1/1";
    uut.setJsonPointer(newJsonPtr);
    EXPECT_EQ(newJsonPtr, uut.getJsonPointer());
}


TEST(JsonResourceTest, serialize)
{
    json testData = {
        {"foo", 1.0},
        {"bar", 2.0},
        {"baz", 3.0},
        {"buz", nullptr}
    };
    
    uut.serialize(testData);
}

TEST(JsonResourceTest, deserialize)
{
    json actual = uut.deserialize();
    json expected = {
        {"foo", 1.0},
        {"bar", 2.0},
        {"baz", 3.0},
        {"buz", nullptr}
    };
    EXPECT_EQ(expected, actual);
}

TEST(JsonResourceTest, deleteTestFile)
{
    std::filesystem::remove("JsonResourceTest1.json");
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}