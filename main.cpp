#include "API.hpp"

int main()
{
    readJSON("topology.json");
    map<string, string> vect = queryDevicesWithNetlistNode("top1", "vdd");
    for(auto& itr:vect)
        std::cout << itr.first << " " << itr.second << std::endl;
    std::cout << std::endl;
}