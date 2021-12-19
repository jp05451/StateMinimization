#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#pragma once

using namespace std;

//script data
class script
{
public:
    int inputNumber;
    int outputNumber;
    int scriptLines;
    int stateNumber;
    string beginState;
};

//nextstate data
class outputState
{
public:
    string state = "";
    bool output = 0;
};

ostream &operator<<(ostream &ostr, const outputState &b)
{
    ostr << b.state << " " << b.output;
    return ostr;
}

//state data
class state
{
public:
    string name;
    outputState nextState[2];
    bool operator==(const state &b)
    {
        return b.name == name;
    }
};
ostream &operator<<(ostream &ost, const state &b)
{
    ost << b.name << "\t" << b.nextState[0] << "\t" << b.nextState[1];
    return ost;
}

//stateMinimize class
class StateMinimization
{
public:
    StateMinimization(const string &fileName);
    void listBuild();
    void scriptDecode();
    void scriptEncode();

private:
    fstream inputFile;
    vector<state> stateList;
    //map<string, state> stateMap;
    script inputScript;
    script outputScript;
};

//********************************************************************

StateMinimization::StateMinimization(const string &fileName)
{
    inputFile.open(fileName, ios::in);
    if (!inputFile.is_open())
    {
        cout << fileName << " open failed" << endl;
        inputFile.close();
        return;
    }
    scriptDecode();
    listBuild();
    inputFile.close();
    cout << "\t0\t1" << endl;
    for (auto c : stateList)
    {
        cout << c << endl;
    }
}

void StateMinimization::scriptDecode()
{
    string temp, cmd;
    stringstream ss;

    getline(inputFile, temp);

    while (cmd != ".r")
    {
        ss.clear();
        getline(inputFile, temp);
        ss << temp;
        ss >> cmd;
        switch (temp[1])
        {
        case 'i':
            //input var number
            ss >> inputScript.inputNumber;
            break;
        case 'o':
            //output var number;
            ss >> inputScript.outputNumber;
            break;
        case 'p':
            //lines of script
            ss >> inputScript.scriptLines;
            break;
        case 's':
            //state number
            ss >> inputScript.stateNumber;
            break;
        case 'r':
            //begin state
            ss >> inputScript.beginState;
            break;
        }
    }
}

vector<state>::iterator find(vector<state>::iterator begin, vector<state>::iterator end, const state &data)
{
    vector<state>::iterator i = begin;
    for (i = begin; i != end; i = next(i, 1))
    {
        if (*i == data)
        {
            return i;
        }
        
    }
    return i;
}

void StateMinimization::listBuild()
{
    bool input;
    string line;
    stringstream ss;
    for (int i = 0; i < inputScript.scriptLines; i++)
    {
        ss.clear();
        getline(inputFile, line);
        ss << line;
        state tempState;

        ss >> input;
        ss >> tempState.name;
        vector<state>::iterator it = find(stateList.begin(), stateList.end(), tempState);
        if (it != stateList.end())
        {
            ss >> it->nextState[input].state;
            ss >> it->nextState[input].output;
        }
        else
        {
            ss >> tempState.nextState[input].state;
            ss >> tempState.nextState[input].output;
            stateList.push_back(tempState);
        }
    }
}