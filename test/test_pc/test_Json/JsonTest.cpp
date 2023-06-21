#include <json.hpp>
#include <iostream>
#include <fstream>

int main()
{
    std::ifstream file("test.json");
    json data = json::parse(file);
    std::cout << data.dump(2) << std::endl;

    json::json_pointer ptr("/1/lol");

    json patch;    
    patch["/0/op"_json_pointer] = "remove";
    patch["/0/path"_json_pointer] = ptr.to_string();

    data.patch_inplace(patch);
    
    std::cout << data.dump(2) << std::endl;
    std::cout << std::boolalpha << data.empty() << std::endl;
}