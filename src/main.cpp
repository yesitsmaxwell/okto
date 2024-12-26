#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
using namespace std;

bool checkForDec(string str) {
    try {
        if (stoi(str) * 1.0 == stod(str)) {
            return false;
        } else {
            return true;
        }    
    } catch (const std::invalid_argument& e) {
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    }
}

bool checkForInt(string str) {
    try {
        if (stoi(str) * 1.0 == stod(str)) {
            return true;
        } else {
            return false;
        }
    } catch (const std::invalid_argument& e) {
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    }
}

bool checkForStr(string str) {
    //if (string[0] == '"' && string.back() == '"') return true;
}

string encodeInt(int input) {

}

string encodeDec(double input) {

}

string encodeStr(string input) {

}

int decodeInt(string input) {

}

double decodeDec(string input) {

}

string decodeStr(string input) {

}

string findType(string str, vector<string> terms, vector<string> list) {
    for (int i = 0; i < list.size(); i++) {
        if (str == terms[i]) {
            return list[i];
        }
    }
    return "Error";
}

void error(string str) {
    cout << "\u001b[31m Error running code: " << str << "\u001b[39m" << endl;
}

bool isVerbose = false;

void verbose(string str) {
    if (isVerbose) cout << "\u001b[36m Info: " << str << "\u001b[39m" << endl;
}

int main(int argc, char *argv[]) {
    cout << "Oktolang interpreter\n";
    vector<string> vars;
    vector<string> varTypes;
    vector<string> varContents;
    vector<string> functions = {"exit", "log", "type", "run", "verbose", "str", "dec", "int"};
    bool running = true;
    string in;
    int lineCounter = 0;
    int maxLines = 0;
    vector<string> lines;
    bool fromFile = false;
    if (argc > 1) {
        fromFile = true;
        ifstream inFile(argv[1]);
        string inLines;
        while (getline(inFile, inLines)) {
            if (inLines[0] == '#') continue;
            lines.push_back(inLines);
            maxLines ++;
        }
    }
    while (running) {
        if (argc == 1) {
            cout << "> ";
            getline(cin, in);
        } else {
            if (lineCounter == maxLines) break;
            in = lines[lineCounter];
            lineCounter ++;
        }
        vector<string> terms = {""};
        vector<string> types;
        int termNumber = 0;
        bool stringOpen = false;
        
        // Split everything up
        verbose("Splitting everything up...");
        for (int i = 0; i < in.length(); i++) {
            if (in[i] == ' ' && !stringOpen) {
                if (!terms[termNumber].empty()) {
                    termNumber++;
                    terms.push_back("");
                }
            } 
            else {
                terms[termNumber] += in[i];
            }
            
            if (in[i] == '"') {
                stringOpen = !stringOpen;
            }

        }
        // Assign types to everything
        verbose("Assigning types...");
        for (int i = 0; i < terms.size(); i++) {
            bool typeFound = false;
            string keyword = terms[i];
            for (int x = 0; x < functions.size(); x++) {
                if (keyword == functions[x]) {
                    types.push_back("function");
                    typeFound = true;
                }
            }
            if (!typeFound) {
                for (int x = 0; x < vars.size(); x++) {
                    if (keyword == vars[x]) {
                        types.push_back("variable");
                        typeFound = true;
                    }
                }
            }
            if (!typeFound) {
                if (keyword == "+") { types.push_back("plus"); }
                else if (keyword == "-") { types.push_back("minus"); }
                else if (keyword == "*") { types.push_back("multiply"); }
                else if (keyword == "/") { types.push_back("divide"); }
                else if (keyword == "++") { types.push_back("increment"); }
                else if (keyword == "--") { types.push_back("decrement"); }
                else if (keyword == "=") { types.push_back("equals"); }
                else if (keyword[0] == '"') { types.push_back("str"); } 
                else if (checkForInt(keyword)) { types.push_back("int"); } 
                else if (checkForDec(keyword)) { types.push_back("dec"); } 
                else { types.push_back("unknown"); }
            }
        }
         
        // Start interpreting code
        verbose("Running code...");
        if (terms[0] == "exit") {
            verbose("Exit command run");
            if (terms.size() < 2) {
                return 0;
            } else if (stoi(terms[1])) {
                return stoi(terms[1]);
            } else if (terms[1] == "0") {
                return 0;
            } else {
                error("Exit arguments must be integers");
            }
        } else if (terms[0] == "log") {
            verbose("Log command run");
            if (terms.size() > 1) {
                if (types[1] == "str" || types[1] == "int" || types[1] == "dec") {
                    verbose("Logging directly");
                    cout << terms[1] << endl;
                } else if (types[1] == "variable") {
                    verbose("Logging a variable");
                    for (int i = 0; i < vars.size(); i++) {
                        if (vars[i] == terms[1]) {
                            verbose("Logging variable " + vars[i] + " with type " + varTypes[i] + " and value " + varContents[i]);
                            cout << varContents[i] << endl;
                            break;
                        }
                    }
                } else if (types[1] == "function") {
                    error("for now, functions cannot be logged");
                } else if (types[1] == "unknown") {
                    error("that type is unknown");
                }
            } else {
                error("log requires an argument");
            }
        } else if (terms[0] == "type") {
            verbose("Type command run");
            if (terms.size() < 2) {
                error("type requires an argument");
            } else {
                cout << types[1] << endl;
            }
        } else if (terms[0] == "str") {
            verbose("String command run");
            if (terms.size() < 2) {
                error("variable must have a name");
            } else {
                if (terms.size() == 3) {
                    error("when defining a variable, set what the variable means");
                } else if (terms[2] != "=") {
                    error("when defining a variable, use '='");
                } else if (types[3] != "str") {
                    error("you've initialised a string, but set it's value to something else");
                } else if (types[1] != "unknown") {
                    error("variable is already initialised");
                } else {
                    verbose("String is being defined...");
                    vars.push_back(terms[1]);
                    varTypes.push_back("str");
                    varContents.push_back(terms[3]);
                    if (isVerbose) {
                        for (int i = 0; i < vars.size(); i++) {
                            if (vars[i] == terms[1]) {
                                verbose("Variable " + vars[i] + " with type " + varTypes[i] + " defined as " + varContents[i]);
                                break;
                            }
                        }
                    }
                }
            }
        } else if (terms[0] == "dec") {
            verbose("Decimal command run");
            if (terms.size() < 2) {
                error("variable must have a name");
            } else {
                if (terms.size() == 3) {
                    error("when defining a variable, set what the variable means");
                } else if (terms[2] != "=") {
                    error("when defining a variable, use '='");
                } else if (!(types[3] == "dec" || types[3] == "int")) {
                    error("you've initialised a decimal, but set it's value to something else");
                } else {
                    verbose("Decimal is being defined...");
                    vars.push_back(terms[1]);
                    varTypes.push_back("dec");
                    varContents.push_back(terms[3]);
                    if (isVerbose) {
                        for (int i = 0; i < vars.size(); i++) {
                            if (vars[i] == terms[1]) {
                                verbose("Variable " + vars[i] + " with type " + varTypes[i] + " defined as " + varContents[i]);
                                break;
                            }
                        }
                    }
                }
            }
        } else if (terms[0] == "int") {
            verbose("Integer command run");
            if (terms.size() < 2) {
                error("variable must have a name");
            } else {
                if (terms.size() == 3) {
                    error("when defining an integer, set what the variable means");
                } else if (terms[2] != "=") {
                    error("when defining a variable, use '='");
                } else if (types[3] != "int") {
                    error("you've initialised a number, but set it's value to something else");
                } else {
                    verbose("Number is being defined...");
                    vars.push_back(terms[1]);
                    varTypes.push_back("int");
                    varContents.push_back(terms[3]);
                    if (isVerbose) {
                        for (int i = 0; i < vars.size(); i++) {
                            if (vars[i] == terms[1]) {
                                verbose("Variable " + vars[i] + " with type " + varTypes[i] + " defined as " + varContents[i]);
                                break;
                            }
                        }
                    }
                }
            }
        } else if (terms[0] == "run") {
            if (terms.size() < 2) {
                error("run requires an argument");
            } else {
                string inputStr = terms[1];
                char input[inputStr.length() + 1];
                strcpy(input, inputStr.c_str());
                int returnCode = system(input);
            }
        } else if (terms[0] == "verbose") {
            isVerbose = !isVerbose;
        } else if (terms[0] == "help") {
            cout << "Oktolang Help\nBuilt In Functions:\n help: This current help function\n log:  Log something to the command line\n type: Find the type of the input\n str:  Define a string\n num:  Define a number\n";
        } else {
            bool variableFound = false;
            for (int i = 0; i < vars.size(); i++) {
                if (vars[i] == terms[0]) {
                    variableFound = true;
                    verbose("Found variable " + vars[i] + " with value " + varContents[i] + " and type " + varTypes[i]);
                    if (varTypes[i] == "int" || varTypes[i] == "dec") {
                        if (types[1] == "increment") {
                            verbose("Incrementing variable " + vars[i] + " with value " + varContents[i]);
                            if (varTypes[i] == "dec") varContents[i] = to_string(stod(varContents[i]) + 1);
                            if (varTypes[i] == "int") varContents[i] = to_string(stoi(varContents[i]) + 1);
                            verbose("New value of " + vars[i] + " is " + varContents[i]);
                        } else if (types[1] == "equals") { 
                            if (types[3] == "add") {
                                verbose("Adding " + vars[2] + " and " + vars[4]);
                                if (types[2] == "variable") {
                                    for (int i = 0; i < vars.size(); i++) {
                                        if (types[2] == vars[i]) {
                                            terms[2] = vars[i];
                                        }
                                        break;
                                    }
                                }
                                if (types[4] == "variable") {
                                    for (int i = 0; i < vars.size(); i++) {
                                        if (types[4] == vars[i]) {
                                            terms[4] = vars[i];
                                        }
                                        break;
                                    }
                                }
                                verbose("Adding " + vars[2] + " and " + vars[4]);
                            }
                        }
                    }
                    break;
                }
            }
            if (!variableFound && terms[0] != "") { 
                error("I don't know how that works");
                if (fromFile) {
                    return 1;
                }
            }
        }
    }
    return 0;
}
