#include "Data/JsonURI.h"

#include <gtest/gtest.h>
#include <Arduino.h>
#include <LittleFS.h>


using Data::JsonURI;

const std::string testFilePath = "JsonURITest.json";
const json::json_pointer testJsonPointer = "/1/bar"_json_pointer;
const json testData = {
    {"foo", 1.0},
    {"bar", 2.0},
    {"baz", 3.0},
    {"buz", nullptr}
};

struct JsonURITest : public testing::Test
{
    JsonURI uut = JsonURI(testFilePath, testJsonPointer);
};


TEST_F(JsonURITest, empty)
{
    JsonURI empty;
    EXPECT_EQ("", empty.getFilePath());
    EXPECT_EQ(""_json_pointer, empty.getJsonPointer());
    EXPECT_EQ("", static_cast<std::string>(empty));

    empty = JsonURI("");
    EXPECT_EQ("", empty.getFilePath());
    EXPECT_EQ(""_json_pointer, empty.getJsonPointer());
    EXPECT_EQ("", static_cast<std::string>(empty));
}

TEST_F(JsonURITest, construct)
{
    JsonURI testURI("foo/bar.json#/hello/1/world");
    EXPECT_EQ("foo/bar.json", testURI.getFilePath());
    EXPECT_EQ("/hello/1/world"_json_pointer, testURI.getJsonPointer());
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

TEST_F(JsonURITest, shouldCreateAndEraseDirectory)
{
    std::cout << "Should be empty" << std::endl;
    EXPECT_FALSE(LittleFS.exists(filePath.c_str()));
    printDirectoryHierarchy();

    std::cout << "Serializing..." << std::endl;
    JsonURI uut(filePath, jsonPointer);
    uut.serialize(testData);
    JsonURI("/foo/yeay").serialize(testValue);
    printDirectoryHierarchy();
    EXPECT_TRUE(LittleFS.exists(filePath.c_str()));

    std::cout << "Erasing..." << std::endl;
    uut.erase();
    printDirectoryHierarchy();
    // EXPECT_FALSE(LittleFS.exists(filePath.c_str()));
}

void setup()
{
    Serial.begin(115200);
    LittleFS.begin(true, "");
    LittleFS.format();

    testing::InitGoogleTest();
    RUN_ALL_TESTS();
}

void loop()
{}