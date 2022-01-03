#include "utilities.hpp"



//list of all topologies in memory
TopologyList tops;

//reads a json file and stores it into a topology in memory. Returns id of the topology
void readJSON(string fileName)
{
    string key;
    char c;
    fstream fin(fileName);
    errorExit(!fin.is_open(), "Error: couldn't open file!\n");

    MapVal* topMap = dynamic_cast<MapVal*>(getMap(fin));
    TopologyID id = dynamic_cast<StringVal*>(topMap->getItem("id"))->getVal();
    tops[id] = topMap;
    fin.close();
}

//returns false if the topology ID does not exist in memory
bool writeJSON(TopologyID topID)
{
    //return false if there is no topology associated with the given ID
    if(tops.count(topID) == 0)
        return false;

    ofstream fin(topID+".json");
    errorExit(!fin.is_open(), "Error: couldn't open file!\n");

    tops[topID]->print(fin);

    fin.close();
    return true;
}

//returns a vector of IDs to all topologies in memory
vector<TopologyID> queryTopologies()
{
    vector<TopologyID> vect;
    for(auto& itr : tops)
        vect.push_back(itr.first);
    return vect;
}

//returns false if no topology with the given ID exists and true otherwise
bool deleteTopology(TopologyID topID)
{
    if(tops.count(topID) == 0)
        return false; //return false if no topology associated with the given ID exists
    delete tops[topID];
    tops.erase(topID);
    return true;
}

//returns a map of compenents types and IDs listed in the topology
map<string, string> queryDevices(TopologyID topID)
{
    map<string, string> devices; //form of each device is (ID, type)
    if(tops.count(topID) == 0)
        return devices; //return an empty map if the given topology does not exit

    Value* ptr = tops[topID]->getItem("components");
    MapVal* ptrVal;
    MapList* ptrList;
    if(dynamic_cast<MapList*>(ptr)) //in case there exists list of components
    {
        ptrList = dynamic_cast<MapList*>(ptr);
        vector<MapVal*> maps = ptrList->getMaps();
        for(auto& itr : maps)
        {
            devices[dynamic_cast<StringVal*>(itr->getItem("id"))->getVal()] =  
            dynamic_cast<StringVal*>(itr->getItem("type"))->getVal();
        }
    }
    else if(dynamic_cast<MapVal*>(ptr)) //in case there exists only one component
    {
        ptrVal = dynamic_cast<MapVal*>(ptr);
        devices[dynamic_cast<StringVal*>(ptrVal->getItem("id"))->getVal()] =  
            dynamic_cast<StringVal*>(ptrVal->getItem("type"))->getVal();
    }
    return devices;
}

// //returns list of components of a given topology ID that are connected to a certain netlist node
map<string, string> queryDevicesWithNetlistNode(TopologyID topID, NetlistNodeID netID)
{
    map<string, string> devices; //form of each device is (ID, type)
    if(tops.count(topID) == 0)
        return devices; //return an empty map if the given topology does not exit

    Value* ptr = tops[topID]->getItem("components");
    MapVal* ptrVal;
    MapList* ptrList;
    if(dynamic_cast<MapList*>(ptr)) //in case there exists list of components
    {
        ptrList = dynamic_cast<MapList*>(ptr);
        vector<MapVal*> maps = ptrList->getMaps();
        for(auto& itr : maps)
        {
            MapVal* netPtr = dynamic_cast<MapVal*>(itr->getItem("netlist"));
            map<string, Value*> nets =  netPtr->getReference();
            for(auto& itr2 : nets)
            {
                StringVal *netVal = dynamic_cast<StringVal*>(itr2.second);
                if(netVal->getVal() == netID)
                {
                    devices[dynamic_cast<StringVal*>(itr->getItem("id"))->getVal()] =  
                        dynamic_cast<StringVal*>(itr->getItem("type"))->getVal();
                }
            }

        }
    }
    else if(dynamic_cast<MapVal*>(ptr)) //in case there exists only one component
    {
        ptrVal = dynamic_cast<MapVal*>(ptr);
        devices[dynamic_cast<StringVal*>(ptrVal->getItem("id"))->getVal()] =  
            dynamic_cast<StringVal*>(ptrVal->getItem("type"))->getVal();
    }
    return devices;
}