// #include <Arduino.h>
// #include <SPIFFS.h>
#include <boost/filesystem.hpp>
// #include <iostream>
// #include <fstream>
// #include <json.hpp>

using namespace boost::filesystem;

void setup()
{
    path myPath = "/spiffs/data";
    directory_iterator it(myPath);
}

void loop()
{}