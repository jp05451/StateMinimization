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
public:
    map<string, stateBlock *> nextState;
    map<string, string[2]> nextStateName;
    bool usable = true;
};

class StateMinimization
{
public:
    StateMinimization(string fileName);
    void begin();
    void tableBuild();
    void outputConditionBuild();
    void tableMinimization();
    void findInitialFalse(const string &out);
    void listMinimization();
    void printTable(string describe);
    void printList();
    void usableCross();

    void kissToDot(const map<string, map<string, stateOutput>> &);

    script inputScript;
    set<string> stateName;
    map<string, map<string, stateOutput>> stateList;
    map<string, map<string, stateBlock>> stateTable;
    set<string> outputCondition;
    set<string> inputCondition;
    string fileName;
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
            stateList[inputState][command].nextState = outputState;
            stateList[inputState][command].output = outputValue;
        }
        inputFile >> command;
    } while (command != ".end_kiss");
    inputFile.close();
}

void StateMinimization::begin()
{
    outputConditionBuild();
    printList();
    for (auto &out : outputCondition)
    {
        cout << out << endl;
        tableBuild();
        findInitialFalse(out);
        printTable("after initial false");

        tableMinimization();
        printList();
        stateTable.clear();
    }
    kissToDot(stateList);
}

void StateMinimization::outputConditionBuild()
{
    for (auto &s : stateList)
    {
        string output;
        for (auto &out : s.second)
        {
            output += out.second.output;
        }
        outputCondition.insert(output);
    }
}

void StateMinimization::tableBuild()
{
    for (auto &s1 : stateList)
    {
        for (auto &s2 : stateList)
        {
            for (auto &input : inputCondition)
            {
                string nextS1 = s1.second[input].nextState;
                string nextS2 = s2.second[input].nextState;
                stateTable[s1.first][s2.first].nextState[input] = &stateTable[nextS1][nextS2];
                stateTable[s2.first][s1.first].nextState[input] = &stateTable[nextS2][nextS1];
                stateTable[s1.first][s2.first].nextStateName[input][0] = nextS1;
                stateTable[s1.first][s2.first].nextStateName[input][1] = nextS2;
                stateTable[s2.first][s1.first].nextStateName[input][0] = nextS2;
                stateTable[s2.first][s1.first].nextStateName[input][1] = nextS1;
            }
        }
    }
}

void StateMinimization::tableMinimization()
{

    //get minimize
    bool hasChange = true;
    while (hasChange == true)
    {
        hasChange = false;
        for (auto &s1 : stateTable)
        {
            for (auto &s2 : s1.second)
            {
                for (auto input : inputCondition)
                {
                    if (s2.second.nextState[input]->usable == false && s2.second.usable == true)
                    {
                        cout << s2.first << "-" << s1.first << endl;

                        s2.second.usable = false;
                        hasChange = true;
                    }
                }
            }
        }
    }
    printTable("before erase");

    cout << endl;
    for (auto &s1 : stateTable)
    {
        for (auto &s2 : s1.second)
        {
            if (s2.second.usable == true)
            {
                if (s1.first != s2.first)
                {
                    stateList.erase(s2.first);
                    stateName.erase(s2.first);

                    for (auto &L1 : stateList)
                    {
                        for (auto &L2 : L1.second)
                        {
                            if (L2.second.nextState == s2.first)
                            {
                                L2.second.nextState = s1.first;
                            }
                        }
                    }

                    //get table cross
                    for (auto &t1 : stateTable)
                    {
                        if (t1.first != s2.first)
                        {
                            stateTable[t1.first][s2.first].usable = false;
                            stateTable[s2.first][t1.first].usable = false;
                        }
                    }
                }
            }
        }
    }
}

void StateMinimization::findInitialFalse(const string &out)
{
    vector<string> cross;
    for (auto &s : stateList)
    {
        string tempOutput;
        for (auto &o : s.second)
        {
            tempOutput += o.second.output;
        }
        if (tempOutput != out)
        {
            cross.push_back(s.first);
        }
    }
    for (auto &c : cross)
    {
        for (auto &s : stateName)
        {
            stateTable[c][s].usable = false;
            stateTable[s][c].usable = false;
        }
        stateTable[c][c].usable = true;
    }
}

void StateMinimization::printTable(string describe)
{
    cout << describe << endl;
    for (auto &s : stateName)
    {
        cout << "\t" << s;
    }
    cout << endl;
    for (auto &s1 : stateTable)
    {
        cout << s1.first << "\t";
        for (auto &input : inputCondition)
        {
            if (input != *(inputCondition.begin()))
            {
                cout << "\t";
            }
            for (auto &s2 : s1.second)
            {
                if (s2.second.usable == true)
                {
                    cout << s2.second.nextStateName[input][0] << "-";
                    cout << s2.second.nextStateName[input][1] << "\t";
                }
                else
                {
                    if (input == *(inputCondition.begin()))
                    {
                        cout << "X\t";
                    }
                    else
                    {
                        cout << "\t";
                    }
                }
            }
            cout << endl;
        }
        cout << endl;
    }
}

void StateMinimization::printList()
{
    for (auto &input : inputCondition)
    {
        cout << "\t" << input;
    }
    cout << endl;
    for (auto &s1 : stateList)
    {
        cout << s1.first << "\t";
        for (auto &s2 : s1.second)
        {
            cout << s2.second.nextState << " ";
            cout << s2.second.output << "\t";
        }
        cout << endl;
    }
}

void StateMinimization::usableCross()
{
    for (auto &c : stateTable)
    {
        stateTable[c.first][c.first].usable = true;
    }
}

void StateMinimization::kissToDot(const map<string, map<string, stateOutput>> &inputList)
{
    fstream outputFile;
    outputFile.open("output.dot", ios::out | ios::trunc);

    outputFile << "digraph STG {" << endl;
    outputFile << "rankdir=LR;" << endl;

    outputFile << "   INIT [shape=point];" << endl;
    for (auto &name : stateName)
    {
        outputFile << name << " [label=\"" << name << "\"];" << endl;
    }
    outputFile << "INIT -> " << inputScript.beginState << ";" << endl;
    for (auto &list : inputList)
    {
        for (auto input : list.second)
        {
            outputFile << list.first << " -> " << list.second[input.first].nextState << "[label=\"" << input.first << "/" << list.second[input.first].output << "\"];" << endl;
        }
    }
    outputFile << "}";

    outputFile.close();
}