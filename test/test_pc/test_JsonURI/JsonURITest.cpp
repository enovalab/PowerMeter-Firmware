#include "Data/JsonURI.h"

#include <gtest/gtest.h>
#include <filesystem>

using Data::JsonURI;

const std::string testFilePath = "JsonResourceTest.json";
const json::json_pointer testJsonPointer = "/1/bar"_json_pointer;

struct JsonURITest : public testing::Test
{
    static void TearDownTestCase()
    {
        std::filesystem::remove(testFilePath);
    }

    JsonURI uut;
    json testData = {
        {"foo", 1.0},
        {"bar", 2.0},
        {"baz", 3.0},
        {"buz", nullptr}
    };
};

TEST_F(JsonURITest, empty)
{
    EXPECT_EQ("", uut.getFilePath());
    EXPECT_EQ(""_json_pointer, uut.getJsonPointer());

    EXPECT_EQ(json(), uut.deserialize());
    uut.serialize(testData);
    EXPECT_FALSE(std::filesystem::exists(""));
}

TEST_F(JsonURITest, checkSettersAndGetters)
{
    uut.setFilePath(testFilePath);
    EXPECT_EQ(testFilePath, uut.getFilePath());

    uut.setJsonPointer(testJsonPointer);
    EXPECT_EQ(testJsonPointer, uut.getJsonPointer());
}

TEST_F(JsonURITest, checkSerializeDeserialize)
{
    uut = JsonURI(testFilePath, testJsonPointer);
    uut.serialize(testData);

    EXPECT_EQ(testData, uut.deserialize());
}


TEST_F(JsonURITest, checkDeserializeNonexisting)
{
    uut = JsonURI("Nonexisting.json");
    EXPECT_TRUE(uut.deserialize().is_null());
    uut = JsonURI("Nonexisting.json#/does/not/exist");
    EXPECT_TRUE(uut.deserialize().is_null());
}


TEST_F(JsonURITest, checkAppendAndAssign)
{
    uut.setJsonPointer(testJsonPointer);
    uut /= "/a/b"_json_pointer;
    EXPECT_EQ(testJsonPointer / "/a/b"_json_pointer, uut.getJsonPointer());
}


TEST_F(JsonURITest, checkAppend)
{
    uut.setJsonPointer(testJsonPointer);
    JsonURI actual = uut / "/a/b"_json_pointer;
    EXPECT_EQ(testJsonPointer / "/a/b"_json_pointer, actual.getJsonPointer());
}


int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}