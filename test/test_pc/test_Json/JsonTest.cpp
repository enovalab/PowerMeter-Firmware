#include <json.hpp>
#include <iostream>
#include <fstream>

int main()
{
    std::ifstream file("test.json");
    json data = json::parse(file);
    // data = json();
    
    std::cout << data.dump(2) << std::endl;
}