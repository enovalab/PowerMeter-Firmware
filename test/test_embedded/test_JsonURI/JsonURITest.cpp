#include "Data/JsonURI.h"

#include "Diagnostics/ExceptionTrace.h"
#include <gtest/gtest.h>
#include <Arduino.h>
#include <LittleFS.h>

#include <dirent.h>
void printDirectoryHierarchy(const std::string& directoryPath = "", int level = 0)
{
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(directoryPath.c_str())) == nullptr) {
        std::cout << "Invalid directory path: " << directoryPath << std::endl;
        return;
    }

    while ((entry = readdir(dir)) != nullptr) {
        if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..") {
            continue;
        }

        std::string fullPath = directoryPath + "/" + entry->d_name;

        if (entry->d_type == DT_DIR) {
            // Print the current directory with appropriate indentation
            std::cout << std::string(level, '\t') << "[+] " << entry->d_name << std::endl;

            // Recursively call the function for subdirectories
            printDirectoryHierarchy(fullPath, level + 1);
        } else {
            // Print files within the directory with indentation
            std::cout << std::string(level, '\t') << " - " << entry->d_name << std::endl;
        }
    }

    closedir(dir);
}


using Data::JsonURI;

const std::string testFilePath = "/JsonURITest/test/test.json";
const json::json_pointer testJsonPointer("/0/lol");
const json testData = {
    {"foo", 1.0},
    {"bar", 2.0},
    {"baz", 3.0}
};

struct JsonURITest : public testing::Test
{
    void TearDown() override
    {
        LittleFS.format();
        EXPECT_FALSE(LittleFS.exists(testFilePath.c_str()));
    }

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
    uut.setFilePath("/JsonURITest1.json");
    EXPECT_EQ("/JsonURITest1.json", uut.getFilePath());

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
    uut.setFilePath("/doesnot.exist");
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

TEST_F(JsonURITest, checkEraseAtJsonLevel)
{
    JsonURI anotherURI(testFilePath, testJsonPointer.parent_pointer());
    uut.serialize(testData);
    anotherURI.serialize(testData);
    std::cout << JsonURI(testFilePath).deserialize().flatten().dump(2) << std::endl;
    EXPECT_NO_THROW(uut.deserialize());
    EXPECT_NO_THROW(anotherURI.deserialize());
    uut.erase();
    EXPECT_THROW(uut.deserialize(), json::exception);
    EXPECT_NO_THROW(anotherURI.deserialize());
}

TEST_F(JsonURITest, checkEraseAtFileLevel)
{
    try
    {
        JsonURI anotherURI("/JsonURITest/anotherFile.json", testJsonPointer);
        uut.serialize(testData);
        anotherURI.serialize(testData);
        std::cout << "serialized" << std::endl;
        printDirectoryHierarchy();
        EXPECT_TRUE(LittleFS.exists(testFilePath.c_str()));
        EXPECT_TRUE(LittleFS.exists(anotherURI.getFilePath().c_str()));
        uut.erase();
        std::cout << "erased" << std::endl;
        printDirectoryHierarchy();
        EXPECT_FALSE(LittleFS.exists(testFilePath.c_str()));
        EXPECT_TRUE(LittleFS.exists(anotherURI.getFilePath().c_str()));
    }
    catch(...)
    {
        FAIL() << Diagnostics::ExceptionTrace::what();
    }
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