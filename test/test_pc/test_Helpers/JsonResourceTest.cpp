#include "System/JsonResource.h"
#include "System/JsonResource.cpp"
#include "Logging/Log.cpp"

#include <gtest/gtest.h>
#include <filesystem>

TEST(JsonResourceTest, deserialize)
{
    const std::string testFilePath = "DeserializeTest.json";
    std::ofstream testFile(testFilePath);

    EXPECT_EQ(json(), System::deserializeFromFile(testFilePath));

    json expected = R"(
        {
            "name": "Foo",
            "array": [
                2,
                2,
                5,
                93
            ],
            "obj": {
                "bar": 1.0,
                "baz": null
            }
        }
    )"_json;
    testFile << expected.dump() << std::flush;
    EXPECT_EQ(expected, System::deserializeFromFile(testFilePath));
    EXPECT_EQ(expected["obj"]["bar"], System::deserializeFromFile(testFilePath, "/obj/bar"_json_pointer));

    testFile.close();
    std::filesystem::remove(testFilePath);
}

TEST(JsonResourceTest, serialize)
{
    const std::string testFilePath = "SerializeTest.json";

    json expected = R"(
        {
            "name": "Foo",
            "array": [
                2,
                2,
                5,
                93
            ],
            "obj": {
                "bar": 1.0,
                "baz": null
            }
        }
    )"_json;

    System::serializeToFile(expected, testFilePath);

    std::ifstream testFile(testFilePath);
    EXPECT_EQ(expected, json::parse(testFile));
    testFile.close();

    json patch = 3.14;
    json::json_pointer patchLocation("/array/3");
    System::serializeToFile(patch, testFilePath, patchLocation);
    json newExpected = expected;
    newExpected[patchLocation] = patch;    
    testFile.open(testFilePath);
    EXPECT_EQ(newExpected, json::parse(testFile));

    testFile.close();
    std::filesystem::remove(testFilePath);
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}