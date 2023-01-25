#include "Data/JsonURI.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

using Data::JsonURI;

const std::string testFilePath = "JsonResourceTest.json";
const json::json_pointer testJsonPointer = "/1/bar"_json_pointer;


struct JsonURITest : public testing::Test
{
    virtual void TearDown() override
    {
        if(std::filesystem::exists(testFilePath))
            std::filesystem::remove(testFilePath);
    }

    JsonURI uut = JsonURI(testFilePath, testJsonPointer);
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

    uut.serialize(json());
    EXPECT_EQ(json(), uut.deserialize());
}


TEST_F(JsonURITest, deserializeNonexistingJsonPointer)
{
    uut.serialize(testData);
    uut.setJsonPointer("/0/does/not/exist"_json_pointer);
    EXPECT_THROW(uut.deserialize(), json::exception);
}

TEST_F(JsonURITest, deserializeNonexistingFile)
{
    uut.setFilePath("doesnot.exist");
    EXPECT_THROW(uut.deserialize(), std::runtime_error);
}


TEST_F(JsonURITest, checkAppendAndAssign)
{
    JsonURI empty;
    empty /= "/foo/bar"_json_pointer;
    EXPECT_EQ("/foo/bar"_json_pointer, empty.getJsonPointer());
    EXPECT_EQ("", empty.getFilePath());

    uut /= "/a/b"_json_pointer;
    EXPECT_EQ(testJsonPointer / "/a/b"_json_pointer, uut.getJsonPointer());
}


TEST_F(JsonURITest, checkAppend)
{
    JsonURI empty;
    JsonURI actual;
    actual = empty / "/foo/bar"_json_pointer;
    EXPECT_EQ("/foo/bar"_json_pointer, actual.getJsonPointer());

    actual = uut / "/a/b"_json_pointer;
    EXPECT_EQ(testJsonPointer / "/a/b"_json_pointer, actual.getJsonPointer());
}


int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}