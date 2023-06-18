#include <json.hpp>
#include <iostream>
#include <fstream>

int main()
{
    std::ifstream file("testfile.json");
    json object = json::parse(file);
    std::cout << object << std::endl;

    json::json_pointer ptr("/foo/bar");

    std::string eraseKey = ptr.back();
    ptr.pop_back();
    object.at(ptr).erase(eraseKey);

    std::cout << object << std::endl;
    std::cout << object.empty() << std::endl;
}