#include <string>
#include <utility>
#include <array>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>


using std::fstream;
using std::ofstream;
using std::string;
using std::pair;
using std::array;
using std::vector;
using std::map;
using std::cerr;

//forward declarations
class Value;
class MapVal;
Value* getValue(fstream& fin);

//some typedefs for better logical understanding of the code
typedef string NetlistNodeID;
typedef string TopologyID;
typedef MapVal Topology;
typedef map<TopologyID, Topology*> TopologyList;


//abstract base class for all possible values that a key can have in a json file
class Value
{
private:
public:
    virtual string getType() = 0;
    virtual void print(ofstream& fin) = 0;
};

//just string value
class StringVal : public Value
{
private:
    string val;
public:
    StringVal(string v) : val(v)
    {

    }
    string getVal() const
    {
        return val;
    }

    void setVal()
    {

    }
    string getType()
    {
        return "StringVal";
    }
    void print(ofstream& fout)
    {
        fout << "\"" << val << "\"";
    }
};

class NumericVal : public Value
{
private:
    double val;
public:
    NumericVal(double v) : val(v)
    {

    }
    string getType()
    {
        return "NumericVal";
    }
    double getVal() const
    {
        return val;
    }
    void print(ofstream& fout)
    {
        fout << val;
    }
};

class MapVal : public Value
{
private:
    map<string, Value*> keyVal;
public:
    string getType()
    {
        return "MapVal";
    }

    void insertItem(string key, Value* valPtr)
    {
        keyVal[key] = valPtr;
    }
    Value* returnPtr(string key)
    {
    return keyVal[key];
    }
    void print(ofstream& fout)
    {
        fout << "{\n";
        bool putComma = false;
        for(auto& pr : keyVal)
        {
            if(putComma)
                fout << ",\n";
            else
                putComma = true;
            fout << "\"" << pr.first << "\"" << ": ";
            pr.second->print(fout);
        }
        fout << "\n}";
    }
    Value* getItem(string key)
    {
        return keyVal[key];
    }
    map<string, Value*>& getReference()
    {
        return keyVal;
    }
};

class MapList : public Value
{
private:
    vector<MapVal*> maps;
public:
    string getType()
    {
        return "MapList";
    }
    void insertMap(MapVal* mp)
    {
        maps.push_back(mp);
    }
    void print(ofstream& fout)
    {
        fout << "[\n";
        bool putComma = false;
        for(auto& pr : maps)
        {
            if(putComma)
                fout << ",\n";
            else
                putComma = true;
            pr->print(fout);
        }
        fout << "\n]";
    }
    vector<MapVal*> getMaps()
    {
        return maps;
    }
};

void errorExit(bool condition, string msg)
{
    if(condition)
    {
        cerr << msg;
        exit(EXIT_FAILURE);
    }
}

string getKey(fstream& fin)
{
    char c;
    string str;

    fin >> c;
    errorExit(c != '"', "Error in getKey(): Invalid structure!\n");
    while(true)
    {
        fin >> c;
        if(c == '"')
            break;
        str.push_back(c);
    }
    return str;
}

Value* getString(fstream& fin)
{
    char c;
    string str;
    fin >> c;
    errorExit(c != '"', "Error in getString(): Invalid structure!\n");
    while(true)
    {
        fin >> c;
        if(c == '"')
            break;
        str.push_back(c);
    }
    return new StringVal(str);
}

Value* getMap(fstream& fin)
{
    char c;
    string key;
    fin >> c;
    errorExit(c != '{', "Error in getMap(): Invalid structure!\n");

    MapVal* mp = new MapVal();

    while(true)
    {
        if(c == '}')
            return mp;
        key = getKey(fin);
        fin >> c;
        mp->insertItem(key, getValue(fin));
        fin >> c;
    }
}
Value* getMapList(fstream& fin)
{
    char c;
    fin >> c;
    errorExit(c != '[', "Error in getMapList(): Invalid structure!\n");
    
    MapList* mpList = new MapList();

    while (true)
    {
        mpList->insertMap(dynamic_cast<MapVal*>(getMap(fin)));
        fin >> c;
        if(c == ']')
            return mpList;
    }
}



Value* getNumber(fstream& fin)
{
    char c;
    double num;
    fin >> num;
    return new NumericVal(num);
}

Value* getValue(fstream& fin)
{
    char c;
    fin >> c;
    fin.putback(c);
    switch (c)
    {
    case '"': //value is string
        return getString(fin);
        break;
    
    case '[': //value is list of components
        return getMapList(fin);
        break;

    case '{': //value is a component's value
        return getMap(fin);
        break;
    
    default: //value is numerical
        return getNumber(fin);
        break;
    }
}

