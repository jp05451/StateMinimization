#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>

#pragma once

using namespace std;

class script
{
public:
    int inputNumber;
    int outputNumber;
    int stateNumber;
    string beginState;
};

class stateOutput
{
public:
    string nextState;
    string output;
};

class stateBlock
{
    map<string, stateBlock *> nextState;
    bool is_unusable = true;
};

class StateMinimization
{
public:
    StateMinimization(string fileName);

    void tableBuild();

private:
    script inputScript;
    set<string> stateName;
    map<string, map<string, stateOutput>> stateList;
    map<string, map<string, stateBlock>> stateTable;
    set<string> outputCondition;
    set<string> inputCondition;
};

StateMinimization::StateMinimization(string fileName)
{
    fstream inputFile;
    inputFile.open(fileName, ios::in);

    string command;
    inputFile >> command;
    inputFile >> command;
    do
    {
        if (command[0] == '.')
        {
            int temp;
            switch (command[1])
            {
            case 'i':
                inputFile >> inputScript.inputNumber;
                break;
            case 'o':
                inputFile >> inputScript.outputNumber;
                break;
            case 's':
                inputFile >> inputScript.stateNumber;
                break;
            case 'r':
                inputFile >> inputScript.beginState;
                break;
            default:
                inputFile >> command;
                break;
            }
        }
        else
        {
            string inputState, outputState, outputValue;
            inputFile >> inputState;
            inputFile >> outputState;
            inputFile >> outputValue;

            stateName.insert(inputState);
            inputCondition.insert(command);
            outputCondition.insert(outputValue);
            stateList[inputState][command].nextState = outputState;
            stateList[inputState][command].output = outputValue;
        }
        inputFile >> command;
    } while (command != ".end_kiss");
    inputFile.close();
}

void StateMinimization::tableBuild()
{
}