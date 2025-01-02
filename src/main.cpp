#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cstring>
#include <map>
#include <variant>
#include <vector>

using namespace std;

enum class VarType {
    INTEGER,
    DECIMAL,
    STRING,
    UNKNOWN
};

struct Variable {
    VarType type;
    variant<int, double, string> value;

    Variable() : type(VarType::UNKNOWN) {}
    
    Variable(VarType t, const variant<int, double, string>& v) : type(t), value(v) {}

    string toString() const {
        switch(type) {
            case VarType::INTEGER:
                return to_string(get<int>(value));
            case VarType::DECIMAL:
                return to_string(get<double>(value));
            case VarType::STRING:
                return get<string>(value);
            default:
                return "unknown";
        }
    }
};

class Interpreter {
private:
    map<string, Variable> variables;
    bool isVerbose = false;
    const vector<string> builtInFunctions = {
        "exit", "log", "type", "run", "verbose", "str", "dec", "int"
    };

    const vector<string> operators = {
        "+", "-", "*", "/"
    };

    const vector<string> incrementors = {
        "++", "--"
    };

    const vector<string> modifiers = {
        "+=", "-=", "*=", "/="
    };

    const vector<string> comparitors = {
        "==", ">", ">=", "=<", "!=", "!<", "!>", "!=<", "!>="
    };

    bool checkForDec(const string& str) {
        try {
            return stoi(str) * 1.0 != stod(str);
        } catch (...) {
            return false;
        }
    }

    bool checkForInt(const string& str) {
        try {
            return stoi(str) * 1.0 == stod(str);
        } catch (...) {
            return false;
        }
    }

    void error(const string& str) {
        cout << "\u001b[31m Error running code: " << str << "\u001b[39m" << endl;
    }

    void verbose(const string& str) {
        if (isVerbose) cout << "\u001b[36m Info: " << str << "\u001b[39m" << endl;
    }

    vector<pair<string, string>> tokenize(const string& input) {
        vector<pair<string, string>> tokens;
        string currentToken;
        bool stringOpen = false;

        for (char c : input) {
            if (c == ' ' && !stringOpen) {
                if (!currentToken.empty()) {
                    tokens.push_back({currentToken, getTokenType(currentToken)});
                    currentToken.clear();
                }
            } else {
                currentToken += c;
            }
            if (c == '"') stringOpen = !stringOpen;
        }
        
        if (!currentToken.empty()) {
            tokens.push_back({currentToken, getTokenType(currentToken)});
        }

        return tokens;
    }

    string getTokenType(const string& token) {
        for (const auto& func : builtInFunctions) {
            if (token == func) return "function";
        }
        for (const auto& inc : incrementors) {
            if (token == inc) return "incrementor";
        }
        for (const auto& mod : modifiers) {
            if (token == mod) return "modifier";
        }
        for (const auto& op : operators) {
            if (token == op) return "operator";
        }
        for (const auto& comp : comparitors) {
            if (token == comp) return "comparitor";
        }
        if (variables.find(token) != variables.end()) return "variable";

        if (token == "=") return "equals";
        if (token[0] == '"') return "str";
        if (checkForInt(token)) return "int";
        if (checkForDec(token)) return "dec";

        return "unknown";
    }

    variant<int, string, double> handleVariable(const string variable) {
        auto& var = variables[variable];
        if (var.type == VarType::INTEGER) {
            return get<int>(var.value);
        } else if (var.type == VarType::STRING) {
            return get<string>(var.value);
        } else if (var.type == VarType::DECIMAL) {
            return get<double>(var.value);
        }
        error("Unknown variable " + variable);
        return "Error";
    }

    int doIntMath(vector<pair<string, string>> tokens) {
        verbose("Var type is integer");
        if (tokens[1].second == "incrementor") {
            verbose("Incrementing...");
            if (tokens[1].first == "++") {
                auto var = handleVariable(tokens[0].first);
                if (std::holds_alternative<int>(var)) return get<int>(var) + 1;
                else error("Only integers can be incremented");
            } else if (tokens[1].second == "--") {
                auto var = handleVariable(tokens[0].first);
                if (std::holds_alternative<int>(var)) return get<int>(var) - 1;
                else error("Only integers can be incremented");
            }
        } else if (tokens[3].second == "operator" && tokens[1].second == "equals") {
            verbose("Detected an operator");
            if (tokens[2].second != "int" || tokens[4].second != "int") {
                verbose("Detected types are " + tokens[2].second + " and " + tokens[4].second);
                error("make sure you're using integers and integers when setting an integer");
                return 0;
            }
            if (tokens[3].first == "+") {
                verbose("Adding...");
                verbose("Trying to add variables");
                return stoi(tokens[2].first) + stoi(tokens[4].first);
                return 0;
            }
            if (tokens[3].first == "-") {
                verbose("Subtracting...");
                if (tokens[2].second == "variable") {
                    verbose("Editing variable in 2nd token");
                    auto& varAdd = variables[tokens[2].first];
                    if (varAdd.type == VarType::INTEGER) {
                        tokens[2].first = to_string(get<int>(varAdd.value));
                        tokens[2].second = "int";
                    } else {
                        error("not all the variables you're adding are integers");
                        return 0;
                    }
                }
                if (tokens[4].second == "variable") {
                    verbose("Editing variable in 4th token");
                    auto& varAdd = variables[tokens[4].first];
                    if (varAdd.type == VarType::INTEGER) {
                        tokens[4].first = to_string(get<int>(varAdd.value));
                        tokens[4].second = "int";
                    } else {
                        error("not all the variables you're adding are integers");
                        return 0;
                    }
                }
                if (tokens[2].second != "int" || tokens[4].second != "int") {
                    verbose("Detected types are " + tokens[2].second + " and " + tokens[4].second);
                    error("make sure you're adding integers and integers when setting an integer");
                    return 0;
                }
                verbose("Trying to subtract variables");
                return stoi(tokens[2].first) - stoi(tokens[4].first);
                return 0;
            }
            if (tokens[3].first == "*") {
                verbose("Multiplying...");
                if (tokens[2].second == "variable") {
                    verbose("Editing variable in 2nd token");
                    auto& varAdd = variables[tokens[2].first];
                    if (varAdd.type == VarType::INTEGER) {
                        tokens[2].first = to_string(get<int>(varAdd.value));
                        tokens[2].second = "int";
                    } else {
                        error("not all the variables you're adding are integers");
                        return 0;
                    }
                }
                if (tokens[4].second == "variable") {
                    verbose("Editing variable in 4th token");
                    auto& varAdd = variables[tokens[4].first];
                    if (varAdd.type == VarType::INTEGER) {
                        tokens[4].first = to_string(get<int>(varAdd.value));
                        tokens[4].second = "int";
                    } else {
                        error("not all the variables you're adding are integers");
                        return 0;
                    }
                }
                if (tokens[2].second != "int" || tokens[4].second != "int") {
                    verbose("Detected types are " + tokens[2].second + " and " + tokens[4].second);
                    error("make sure you're adding integers and integers when setting an integer");
                    return 0;
                }
                verbose("Trying to multiply variables");
                return stoi(tokens[2].first) * stoi(tokens[4].first);
                return 0;
            }
            if (tokens[3].first == "/") {
                verbose("Dividing...");
                if (tokens[2].second == "variable") {
                    verbose("Editing variable in 2nd token");
                    auto& varAdd = variables[tokens[2].first];
                    if (varAdd.type == VarType::INTEGER) {
                        tokens[2].first = to_string(get<int>(varAdd.value));
                        tokens[2].second = "int";
                    } else {
                        error("not all the variables you're adding are integers");
                        return 0;
                    }
                }
                if (tokens[4].second == "variable") {
                    verbose("Editing variable in 4th token");
                    auto& varAdd = variables[tokens[4].first];
                    if (varAdd.type == VarType::INTEGER) {
                        tokens[4].first = to_string(get<int>(varAdd.value));
                        tokens[4].second = "int";
                    } else {
                        error("not all the variables you're adding are integers");
                        return 0;
                    }
                }
                if (tokens[2].second != "int" || tokens[4].second != "int") {
                    verbose("Detected types are " + tokens[2].second + " and " + tokens[4].second);
                    error("make sure you're adding integers and integers when setting an integer");
                    return 0;
                }
                if (tokens[2].first == "0" || tokens[4].first == "0") {
                    error("Don't divide by zero or the end of the universe will be upon us you idiot");
                    verbose("(please don't try any funny business i'm begging you)");
                    return 0;
                }
                verbose("Trying to divide variables");
                return stoi(tokens[2].first) / stoi(tokens[4].first);
                return 0;
            }
        }
    }

public:
    auto executeCommand(const string& input) {
        auto tokens = tokenize(input);
        if (tokens.empty()) return 0;

        if (tokens[0].first == "log") {
            verbose("Log command run");
            
            if (tokens.size() < 2) {
                error("log requires an argument");
                return 0;
            }

            const auto& [token, type] = tokens[1];
            if (type == "str" || type == "int" || type == "dec") {
                cout << token << endl;
            }
            else if (type == "variable") {
                auto it = variables.find(token);
                if (it != variables.end()) {
                    cout << it->second.toString() << endl;
                }
            }
            else if (type == "function") {
                error("for now, functions cannot be logged");
            }
            else if (type == "unknown") {
                error("that type is unknown");
            }
            return 0;
        }
        
        else if (tokens[0].first == "exit") {
            if (tokens.size() < 2) {
                exit(0);
            } else if (tokens.size() > 1) {
                const auto& [token, type] = tokens[1];
                if (type == "int") {
                    exit(stoi(token));
                } else {
                    error("exit argument must be integer");
                }
            }
            return 0;
        }
        
        else if (tokens[0].first == "type") {
            if (tokens.size() < 2) {
                error("type requires an argument"); 
                return 0;
            }
            const auto& [token, type] = tokens[1];
            cout << type << endl;
            return 0;
        }

        else if (tokens[0].first == "run") {
            if (tokens.size() < 2) {
                error("run requires an argument");
                return 0;
            }
            string inputStr = tokens[1].first;
            char input[inputStr.length() + 1];
            strcpy(input, inputStr.c_str());
            int returnCode = system(input);
            return 0;
        }
        else if (tokens[0].first == "str") {
            verbose("String command run");
            if (tokens.size() < 2) {
                error("variable must have a name");
                return 0;
            }
            if (tokens.size() < 4) {
                error("when defining a variable, set what the variable means");
                return 0;
            }

            const auto& varName = tokens[1].first;

            if (tokens[2].first != "=") {
                error("when defining a variable, use '='");
                return 0;
            }
            if (variables.find(varName) != variables.end()) {
                error("variable is already initialized");
                return 0;
            }
            if (tokens[3].second != "str") {
                error("you've initialized a string, but set it's value to a different type");
                return 0;
            }

            string value = tokens[3].first;
            value = value.substr(1, value.length() - 2);
            variables[varName] = Variable(VarType::STRING, value);

            verbose("String variable " + varName + "defined as " + value);
            return 0;
        }

        else if (tokens[0].first == "int") {
            verbose("Integer command run");
            if (tokens.size() < 2) {
                error("variable must have a name");
                return 0;
            }
            if (tokens.size() < 4) {
                error("when defining a variable, set what the variable means");
                return 0;
            }

            const auto& varName = tokens[1].first;

            if (tokens[2].first != "=") {
                error("when defining a variable, use '='");
                return 0;
            }
            if (variables.find(varName) != variables.end()) {
                error("variable is already initialized");
                return 0;
            }
            if (tokens[3].second != "int") {
                error("you've initialized an integer, but set it's value to a different type");
                return 0;
            }

            int value = stoi(tokens[3].first);
            variables[varName] = Variable(VarType::INTEGER, value);

            verbose("Integer variable " + varName + "defined as " + to_string(value));
            return 0;
        }

        else if (tokens[0].first == "dec") {
            verbose("Decimal command run");
            if (tokens.size() < 2) {
                error("variable must have a name");
                return 0;
            }
            if (tokens.size() < 4) {
                error("when defining a variable, set what the variable means");
                return 0;
            }

            const auto& varName = tokens[1].first;

            if (tokens[2].first != "=") {
                error("when defining a variable, use '='");
                return 0;
            }
            if (variables.find(varName) != variables.end()) {
                error("variable is already initialized");
                return 0;
            }
            if (tokens[3].second != "dec") {
                error("you've initialized a decimal, but set it's value to a different type");
                return 0;
            }

            double value = stod(tokens[3].first);
            variables[varName] = Variable(VarType::DECIMAL, value);

            verbose("Decimal variable " + varName + "defined as " + to_string(value));
            return 0;
        }

        else if (tokens[0].first == "verbose") {
            isVerbose = !isVerbose;
            return 0;
        }

        else if (tokens[0].first == "help") {
            cout << "Oktolang Help\nBuilt In Functions:\n help: This current help function\n log:  Log something to the command line\n type: Find the type of the input\n str:  Define a string\n int:  Define a whole number\n dec:  Define a number with a decimal place\n";
            return 0;
        }
        
        else if (variables.find(tokens[0].first) != variables.end()) {
            verbose("Doing stuff with variables...");
            if (tokens.size() < 2) {
                error("Expected an operator");
                return 0;
            }
            auto& var = variables[tokens[0].first];
            if (var.type == VarType::INTEGER) {
                tokens.erase(tokens.begin() + 0);
                tokens.erase(tokens.begin() + 1);
                verbose("Mathing...");
                var.value = doIntMath(tokens);

            } else if (var.type == VarType::DECIMAL) {
                if (tokens[1].second == "incrementor") {
                    if (tokens[1].first == "++") {
                        double currentValue = get<double>(var.value);
                        var.value = currentValue + 1;
                    } else if (tokens[1].second == "--") {
                        double currentValue = get<double>(var.value);
                        var.value = currentValue - 1;
                    }
                } else if (tokens[3].second == "operator" && tokens[1].second == "equals") {
                    verbose("Detected an operator");
                    if (tokens[3].first == "+") {
                        verbose("Adding...");
                        if (tokens[2].second == "variable") {
                            verbose("Editing variable in 2nd token");
                            auto& varAdd = variables[tokens[2].first];
                            if (varAdd.type == VarType::INTEGER || varAdd.type == VarType::DECIMAL) {
                                tokens[2].first = to_string(get<double>(varAdd.value));
                                tokens[2].second = "int";
                            } else {
                                error("not all the variables you're adding are integers");
                                return 0;
                            }
                        }
                        if (tokens[4].second == "variable") {
                            verbose("Editing variable in 4th token");
                            auto& varAdd = variables[tokens[4].first];
                            if (varAdd.type == VarType::INTEGER || varAdd.type == VarType::DECIMAL) {
                                tokens[4].first = to_string(get<double>(varAdd.value));
                                tokens[4].second = "int";
                            } else {
                                error("not all the variables you're adding are integers");
                                return 0;
                            }
                        }
                        if (tokens[2].second != "int" || tokens[4].second != "int") {
                            verbose("Detected types are " + tokens[2].second + " and " + tokens[4].second);
                            error("make sure you're adding integers and integers when setting an integer");
                            return 0;
                        }
                        verbose("Trying to add variables");
                        var.value = stod(tokens[2].first) + stoi(tokens[4].first);
                        return 0;
                    }
                    if (tokens[3].first == "-") {
                        verbose("Subtracting...");
                        if (tokens[2].second == "variable") {
                            verbose("Editing variable in 2nd token");
                            auto& varAdd = variables[tokens[2].first];
                            if (varAdd.type == VarType::INTEGER || varAdd.type == VarType::DECIMAL) {
                                tokens[2].first = to_string(get<double>(varAdd.value));
                                tokens[2].second = "int";
                            } else {
                                error("not all the variables you're adding are integers");
                                return 0;
                            }
                        }
                        if (tokens[4].second == "variable") {
                            verbose("Editing variable in 4th token");
                            auto& varAdd = variables[tokens[4].first];
                            if (varAdd.type == VarType::INTEGER || varAdd.type == VarType::DECIMAL) {
                                tokens[4].first = to_string(get<double>(varAdd.value));
                                tokens[4].second = "int";
                            } else {
                                error("not all the variables you're adding are integers");
                                return 0;
                            }
                        }
                        if (tokens[2].second != "int" || tokens[4].second != "int") {
                            verbose("Detected types are " + tokens[2].second + " and " + tokens[4].second);
                            error("make sure you're adding integers and integers when setting an integer");
                            return 0;
                        }
                        verbose("Trying to subtract variables");
                        var.value = stod(tokens[2].first) - stoi(tokens[4].first);
                        return 0;
                    }
                    if (tokens[3].first == "*") {
                        verbose("Multiplying...");
                        if (tokens[2].second == "variable") {
                            verbose("Editing variable in 2nd token");
                            auto& varAdd = variables[tokens[2].first];
                            if (varAdd.type == VarType::INTEGER || varAdd.type == VarType::DECIMAL) {
                                tokens[2].first = to_string(get<double>(varAdd.value));
                                tokens[2].second = "int";
                            } else {
                                error("not all the variables you're adding are integers");
                                return 0;
                            }
                        }
                        if (tokens[4].second == "variable") {
                            verbose("Editing variable in 4th token");
                            auto& varAdd = variables[tokens[4].first];
                            if (varAdd.type == VarType::INTEGER || varAdd.type == VarType::DECIMAL) {
                                tokens[4].first = to_string(get<double>(varAdd.value));
                                tokens[4].second = "int";
                            } else {
                                error("not all the variables you're adding are integers");
                                return 0;
                            }
                        }
                        if (tokens[2].second != "int" || tokens[4].second != "int") {
                            verbose("Detected types are " + tokens[2].second + " and " + tokens[4].second);
                            error("make sure you're adding integers and integers when setting an integer");
                            return 0;
                        }
                        verbose("Trying to multiply variables");
                        var.value = stod(tokens[2].first) * stoi(tokens[4].first);
                        return 0;
                    }
                    if (tokens[3].first == "/") {
                        verbose("Dividing...");
                        if (tokens[2].second == "variable") {
                            verbose("Editing variable in 2nd token");
                            auto& varAdd = variables[tokens[2].first];
                            if (varAdd.type == VarType::INTEGER || varAdd.type == VarType::DECIMAL) {
                                tokens[2].first = to_string(get<double>(varAdd.value));
                                tokens[2].second = "int";
                            } else {
                                error("not all the variables you're adding are integers");
                                return 0;
                            }
                        }
                        if (tokens[4].second == "variable") {
                            verbose("Editing variable in 4th token");
                            auto& varAdd = variables[tokens[4].first];
                            if (varAdd.type == VarType::INTEGER || varAdd.type == VarType::DECIMAL) {
                                tokens[4].first = to_string(get<double>(varAdd.value));
                                tokens[4].second = "int";
                            } else {
                                error("not all the variables you're adding are integers");
                                return 0;
                            }
                        }
                        if (tokens[2].second != "int" || tokens[4].second != "int") {
                            verbose("Detected types are " + tokens[2].second + " and " + tokens[4].second);
                            error("make sure you're adding integers and integers when setting an integer");
                            return 0;
                        }
                        if (tokens[2].first == "0" || tokens[4].first == "0") {
                            error("Don't divide by zero or the end of the universe will be upon us you idiot");
                            verbose("(please don't try any funny business i'm begging you)");
                            return 0;
                        }
                        verbose("Trying to divide variables");
                        var.value = stod(tokens[2].first) / stoi(tokens[4].first);
                        return 0;
                    }
                }
            }
            return 0;
        }
        
        if (!tokens[0].first.empty()) {
            error("I don't know how that works");
            return 0;
        }
        return 0;
    };
};

int main(int argc, char *argv[]) {
    cout << "Oktolang interpreter\n";
    Interpreter interpreter;
    
    if (argc > 1) {
        ifstream inFile(argv[1]);
        string line;
        while (getline(inFile, line)) {
            if (line[0] == '#') continue;
            if (interpreter.executeCommand(line) != 0) return 1;
        }
    } else {
        string input;
        while (true) {
            cout << "> ";
            getline(cin, input);
            if (interpreter.executeCommand(input) != 0) break;
        }
    }
    return 0;
}

