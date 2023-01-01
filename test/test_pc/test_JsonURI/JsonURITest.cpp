#include "Data/JsonURI.h"
#include "Error/ExceptionStack.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

using Data::JsonURI;

const std::string testFilePath = "JsonResourceTest.json";
const json::json_pointer testJsonPointer = "/1/bar"_json_pointer;

struct JsonURITest : public testing::Test
{
    static void TearDownTestCase()
    {
        std::filesystem::remove(testFilePath);
    }

    JsonURI uut = JsonURI("JsonResourceTest.json", "/1/bar"_json_pointer);
    json testData = {
        {"foo", 1.0},
        {"bar", 2.0},
        {"baz", 3.0},
        {"buz", nullptr}
    };
};

TEST_F(JsonURITest, empty)
{
    JsonURI empty;
    EXPECT_EQ("", empty.getFilePath());
    EXPECT_EQ(""_json_pointer, empty.getJsonPointer());

    EXPECT_EQ(json(), uut.deserialize());
    empty.serialize(testData);
    EXPECT_FALSE(std::filesystem::exists(""));
}

TEST_F(JsonURITest, checkSettersAndGetters)
{
    uut.setFilePath("JsonURITest1.json");
    EXPECT_EQ("JsonURITest1.json", uut.getFilePath());

    uut.setJsonPointer("/another/json/ptr"_json_pointer);
    EXPECT_EQ("/another/json/ptr"_json_pointer, uut.getJsonPointer());
}

TEST_F(JsonURITest, serializeDeserialize)
{
    uut.serialize(testData);
    EXPECT_EQ(testData, uut.deserialize());
}


TEST_F(JsonURITest, deserializeNonexisting)
{
    uut.setJsonPointer("/does/not/exist"_json_pointer);
    json result;
    try
    {
        result = uut.deserialize();
    }
    catch(const std::exception& e)
    {
        Error::ExceptionStack_t exceptionStack = Error::ExceptionStack::get(e);
        
    }
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