#include <iostream>
#include <boost/units/systems/si.hpp>
#include <boost/units/io.hpp>

using namespace boost::units::si;
using namespace boost::units;

int main()
{
    quantity<electric_potential> voltageRMS = 230.0 * volt; 
    quantity<current> currentRMS = 16 * milliampere;

    std::cout << voltageRMS << std::endl;
    std::cout << voltageRMS << std::endl;
    std::cout << voltageRMS * currentRMS << std::endl;
}