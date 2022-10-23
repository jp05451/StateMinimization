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
    int lines;
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
    StateMinimization(string inputKiss, string outputKiss, string outputDot);                                 // minimize kiss and output dot
    StateMinimization(string _inputKiss, string _outputDot) : inputKiss(_inputKiss), outputDot(_outputDot){}; // kiss to dot
    void begin();
    void tableBuild();
    void outputConditionBuild();
    void tableMinimization();
    void findInitialFalse(const string &out);
    void listMinimization();
    void printTable(string describe);
    void printList();
    void usableCross();
    void KissToDot(string);
    void readKissScript(string);

    void stateToDot();
    void stateToKiss();

    script inputScript;
    set<string> stateName;
    map<string, map<string, stateOutput>> stateList;
    map<string, map<string, stateBlock>> stateTable;
    set<string> outputCondition;
    set<string> inputCondition;
    string inputKiss;
    string outputKiss;
    string outputDot;

    int lines;
};

void StateMinimization::KissToDot(string kissFile)
{
    readKissScript(kissFile);
    stateToDot();
}

void StateMinimization::readKissScript(string kissFile)
{
    fstream inputFile;

    inputFile.open(kissFile, ios::in);
    if (!inputFile.is_open())
    {
        cout << kissFile << " read Error" << endl;
        exit(0);
    }

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
            case 'p':
                inputFile >> inputScript.lines;
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

StateMinimization::StateMinimization(string _inputKiss, string _outputKiss, string _outputDot)
{
    fstream inputFile;
    inputKiss = _inputKiss;
    outputKiss = _outputKiss;
    outputDot = _outputDot;

    readKissScript(inputKiss);
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
    stateToDot();
    stateToKiss();
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

    // get minimize
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

                    // get table cross
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

void StateMinimization::stateToDot()
{
    fstream outputFile;
    outputFile.open(outputDot, ios::out | ios::trunc);

    cout << endl
         << outputDot << endl;

    outputFile << "digraph STG {" << endl;
    cout << "digraph STG {" << endl;
    outputFile << "\trankdir=LR;" << endl;
    cout << "\trankdir=LR;" << endl;

    outputFile << "\tINIT [shape=point];" << endl;
    cout << "\tINIT [shape=point];" << endl;

    int count = 0;

    for (auto &name : stateName)
    {
        count++;
        outputFile << "\t" << name << " [label=\"" << name << "\"];" << endl;
        cout << "\t" << name << " [label=\"" << name << "\"];" << endl;
    }

    inputScript.stateNumber = count;

    outputFile << "\tINIT -> " << inputScript.beginState << ";" << endl;
    cout << "\tINIT -> " << inputScript.beginState << ";" << endl;

    count = 0;
    for (auto &list : stateList)
    {
        for (auto input : list.second)
        {
            count++;
            outputFile << "\t" << list.first << " -> " << input.second.nextState << " [label=\"" << input.first << "/" << input.second.output << "\"];" << endl;
            cout << "\t" << list.first << " -> " << input.second.nextState << " [label=\"" << input.first << "/" << input.second.output << "\"];" << endl;
        }
    }
    outputFile << "}";
    cout << "}" << endl
    ;

    inputScript.lines = count;

    outputFile.close();
}

void StateMinimization::stateToKiss()
{
    fstream file;
    file.open(outputKiss, ios::out | ios::trunc);
    cout << endl
         << outputKiss << endl;
    file << ".start_kiss" << endl;
    cout << ".start_kiss" << endl;

    file << ".i " << inputScript.inputNumber << endl;
    cout << ".i " << inputScript.inputNumber << endl;

    file << ".o " << inputScript.outputNumber << endl;
    cout << ".o " << inputScript.outputNumber << endl;

    file << ".p " << inputScript.lines << endl;
    cout << ".p " << inputScript.lines << endl;

    file << ".s " << inputScript.stateNumber << endl;
    cout << ".s " << inputScript.stateNumber << endl;

    file << ".r " << inputScript.beginState << endl;
    cout << ".r " << inputScript.beginState << endl;

    for (auto &state : stateList)
    {
        for (auto &input : state.second)
        {
            file << input.first << " " << state.first << " " << input.second.nextState << " " << input.second.output << endl;
            cout << input.first << " " << state.first << " " << input.second.nextState << " " << input.second.output << endl;
        }
    }
    file << ".end_kiss" << endl;
    cout << ".end_kiss" << endl;
    file.close();
}