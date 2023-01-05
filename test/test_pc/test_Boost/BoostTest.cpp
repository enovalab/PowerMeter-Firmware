#include <iostream>
#include <boost/format.hpp>

using boost::format;

int main()
{   
    int age = 18;
    bool isEngineer = true;
    auto a = format("Engineer = %2% Samuel is %1% year old") % age % isEngineer;
    std::cout << a << std::endl;
}