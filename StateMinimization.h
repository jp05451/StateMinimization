#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

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
class stateOutput
{
public:
    string state = "";
    bool Output = 0;
};

//state data
class state
{
public:
    string name;
    stateOutput nextState[2];
};

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
}

void StateMinimization::scriptDecode()
{
    string temp;
    stringstream ss;
    getline(inputFile, temp);
    ss << temp;
    while (temp[0] != '.')
    {
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
            ss >> inputScript.beginState;
            break;
        }
        getline(inputFile, temp);
        ss << temp;
    }
}

void StateMinimization::listBuild()
{
    stateList.resize(inputScript.stateNumber);
    bool input;
    string inputState;
    string nextState;
    bool output;
    for (int i = 0; i < inputScript.scriptLines; i++)
    {
        inputFile >> input;
    }
}