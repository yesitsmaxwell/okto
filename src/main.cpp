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
    BOOLEAN,
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
        "exit", "log", "type", "run", "verbose", "str", "dec", "int", "bool"
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

    void error(const string& str, int exitCode = 1) {
        cout << "\u001b[31m Error running code: " << str << "\u001b[39m" << endl;
        if (exitCode != 0) {
            exit(exitCode);
        }
    }

    void verbose(const string& str) {
        if (isVerbose) cout << "\u001b[36m Info: " << str << "\u001b[39m" << endl;
    }
    
    string interpolateVariables(const string& input) {
        string result;
        size_t pos = 0;
        bool inString = false;
        
        while (pos < input.length()) {
            if (input[pos] == '"') {
                inString = !inString;
                result += input[pos];
                pos++;
            } else if (pos + 1 < input.length() && input[pos] == '$' && input[pos + 1] == '{') {
                size_t end = input.find('}', pos);
                if (end != string::npos) {
                    string varName = input.substr(pos + 2, end - (pos + 2));
                    auto it = variables.find(varName);
                    if (it != variables.end()) {
                        result += it->second.toString();
                    } else {
                        error("Variable " + varName + " not found");
                        result += "${" + varName + "}";
                    }
                    pos = end + 1;
                } else {
                    result += input[pos];
                    pos++;
                }
            } else {
                result += input[pos];
                pos++;
            }
        }
        
        return result;
    }

    vector<pair<string, string>> preProcessTokens(const vector<pair<string, string>>& tokens) {
        vector<pair<string, string>> processed;
        for (const auto& [token, type] : tokens) {
            if (type == "str") {
                string processedStr = interpolateVariables(token);
                processed.push_back({processedStr, "str"});
            } else if (token[0] == '$' && token.length() > 1) {
                string varName = token.substr(1);
                auto it = variables.find(varName);
                if (it != variables.end()) {
                    processed.push_back({it->second.toString(), getTokenType(it->second.toString())});
                } else {
                    error("Variable " + varName + " not found");
                    processed.push_back({token, "unknown"});
                }
            } else {
                processed.push_back({token, type});
            }
        }
        return processed;
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

    int doIntMath(const vector<pair<string, string>>& tokens) {
        verbose("Var type is integer");
        if (tokens.size() == 2 && tokens[1].second == "incrementor") {
            verbose("Incrementing...");
            auto var = handleVariable(tokens[0].first);
            if (!std::holds_alternative<int>(var)) {
                error("Only integers can be incremented");
                return 0;
            }
            if (tokens[1].first == "++") return get<int>(var) + 1;
            if (tokens[1].first == "--") return get<int>(var) - 1;
            return 0;
        }
        if (tokens.size() == 5 && tokens[1].first == "=" && tokens[3].second == "operator") {
            verbose("Detected an operator");
            
            int left = (tokens[2].second == "variable") ? 
                get<int>(handleVariable(tokens[2].first)) : 
                stoi(tokens[2].first);
            int right = (tokens[4].second == "variable") ? 
                get<int>(handleVariable(tokens[4].first)) : 
                stoi(tokens[4].first);
            
            if (tokens[3].first == "+") return left + right;
            if (tokens[3].first == "-") return left - right;
            if (tokens[3].first == "*") return left * right;
            if (tokens[3].first == "/") {
                if (right == 0) {
                    error("Division by zero");
                    return 0;
                }
                return left / right;
            }
        }
        
        error("Invalid operation");
        return 0;
    }

public:
    variant<int, string, double, bool> executeCommand(const string& input) {
        string processedInput = interpolateVariables(input);
        auto tokens = tokenize(input);
        if (tokens.empty()) return true;
        tokens = preProcessTokens(tokens);
        
        for (int i = 0; i < tokens.size(); i++) {
            if (tokens[i].second == "str") {
                tokens[i].first.erase(tokens[i].first.begin());
                tokens[i].first.erase(tokens[i].first.begin() + tokens[i].first.size() - 1);
            }
        }
        verbose("Attempting to run function " + tokens[0].first);
        
        if (tokens[0].first == "//") {
            verbose("Comment detected. Skipping line...");
            return true;
        }

        if (tokens[0].first == "log") {
            verbose("Log function run");
            if (tokens.size() < 2) {
                error("Log requires an argument");
            }
            const auto& [token, type] = tokens[1];
            if (type == "str" || type == "int" || type == "dec") {
                cout << token << endl;
            }
            else {
                error("Type " + type + " cannot be logged");
            }
            return true;
        }
        
        else if (tokens[0].first == "exit") {
            verbose("Exit function run");
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
            return true;
        }
        
        else if (tokens[0].first == "type") {
            verbose("Type function run");
            if (tokens.size() < 2) {
                error("Type requires an argument"); 
            }
            const auto& [token, type] = tokens[1];
            cout << type << endl;
            return true;
        }

        else if (tokens[0].first == "run") {
            verbose("Run function run");
            if (tokens.size() < 2) {
                error("Run requires an argument");
            }
            if (tokens[1].second != "string") {
                error("Run argument must be string");
            }
            string inputStr = tokens[1].first;
            char input[inputStr.length() + 1];
            strcpy(input, inputStr.c_str());
            int returnCode = system(input);
            return true;
        }
        
        else if (tokens[0].first == "in") {
            verbose("In function run");
            string input;
            if (tokens.size() > 1) {
                verbose("Argument type for in is " + tokens[1].second);
                if (tokens[1].second == "str") {
                    cout << tokens[1].first;
                } else {
                    error("In argument must be string");
                }
            }
            getline(cin, input);
            verbose("Inputted " + input);
            return input;
        }
        else if (tokens[0].first == "str") {
            verbose("String function run");
            if (tokens.size() < 2) {
                error("variable must have a name");
            }
            if (tokens.size() < 4) {
                error("when defining a variable, set what the variable means");
            }

            const auto& varName = tokens[1].first;

            if (tokens[2].first != "=") {
                error("when defining a variable, use '='");
            }
            if (variables.find(varName) != variables.end()) {
                error("variable is already initialized");
            }
            if (tokens[3].second != "str") {
                error("you've initialized a string, but set it's value to a different type");
            }

            string value = tokens[3].first;
            value = value.substr(1, value.length() - 2);
            variables[varName] = Variable(VarType::STRING, value);

            verbose("String variable " + varName + " defined as " + value);
            return true;
        }

        else if (tokens[0].first == "int") {
            verbose("Integer function run");
            if (tokens.size() < 2) {
                error("variable must have a name");
            }
            if (tokens.size() < 4) {
                error("when defining a variable, set what the variable means");
            }

            const auto& varName = tokens[1].first;

            if (tokens[2].first != "=") {
                error("when defining a variable, use '='");
            }
            if (variables.find(varName) != variables.end()) {
                error("variable is already initialized");
            }
            if (tokens[3].second != "int") {
                error("you've initialized an integer, but set it's value to a different type");
            }

            int value = stoi(tokens[3].first);
            variables[varName] = Variable(VarType::INTEGER, value);

            verbose("Integer variable " + varName + " defined as " + to_string(value));
            return true;
        }

        else if (tokens[0].first == "dec") {
            verbose("Decimal function run");
            if (tokens.size() < 2) {
                error("variable must have a name");
            }
            if (tokens.size() < 4) {
                error("when defining a variable, set what the variable means");
            }

            const auto& varName = tokens[1].first;

            if (tokens[2].first != "=") {
                error("when defining a variable, use '='");
            }
            if (variables.find(varName) != variables.end()) {
                error("variable is already initialized");
            }
            if (tokens[3].second != "dec") {
                error("you've initialized a decimal, but set it's value to a different type");
            }

            double value = stod(tokens[3].first);
            variables[varName] = Variable(VarType::DECIMAL, value);

            verbose("Decimal variable " + varName + " defined as " + to_string(value));
            return true;
        }

        else if (tokens[0].first == "verbose") {
            verbose("Verbose mode disabled");
            isVerbose = !isVerbose;
            verbose("Verbose mode enabled");
            return true;
        }

        else if (tokens[0].first == "help") {
            cout << "Oktolang Help\nBuilt In Functions:\n help: This current help function\n log:  Log something to the command line\n type: Find the type of the input\n str:  Define a string\n int:  Define a whole number\n dec:  Define a number with a decimal place\n";
            return true;
        }
        
        else if (variables.find(tokens[0].first) != variables.end()) {
            verbose("Doing stuff with variables...");
            if (tokens.size() < 2) {
                error("Expected an operator");
            }
            auto& var = variables[tokens[0].first];
            if (var.type == VarType::INTEGER) {
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
                            }
                        }
                        if (tokens[2].second != "int" || tokens[4].second != "int") {
                            verbose("Detected types are " + tokens[2].second + " and " + tokens[4].second);
                            error("make sure you're adding integers and integers when setting an integer");
                        }
                        verbose("Trying to add variables");
                        var.value = stod(tokens[2].first) + stoi(tokens[4].first);
                        return true;
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
                            }
                        }
                        if (tokens[2].second != "int" || tokens[4].second != "int") {
                            verbose("Detected types are " + tokens[2].second + " and " + tokens[4].second);
                            error("make sure you're adding integers and integers when setting an integer");
                        }
                        verbose("Trying to subtract variables");
                        var.value = stod(tokens[2].first) - stoi(tokens[4].first);
                        return true;
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
                            }
                        }
                        if (tokens[2].second != "int" || tokens[4].second != "int") {
                            verbose("Detected types are " + tokens[2].second + " and " + tokens[4].second);
                            error("make sure you're adding integers and integers when setting an integer");
                        }
                        verbose("Trying to multiply variables");
                        var.value = stod(tokens[2].first) * stoi(tokens[4].first);
                        return true;
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
                            }
                        }
                        if (tokens[2].second != "int" || tokens[4].second != "int") {
                            verbose("Detected types are " + tokens[2].second + " and " + tokens[4].second);
                            error("make sure you're adding integers and integers when setting an integer");
                        }
                        if (tokens[2].first == "0" || tokens[4].first == "0") {
                            error("Don't divide by zero or the end of the universe will be upon us you idiot");
                            verbose("(please don't try any funny business i'm begging you)");
                        }
                        verbose("Trying to divide variables");
                        var.value = stod(tokens[2].first) / stoi(tokens[4].first);
                        return true;
                    }
                }
            }
            return true;
        }
        
        if (!tokens[0].first.empty()) {
            error(tokens[0].first + " isn't a function or variable. Check that the function or variable exists prior to running this line of code?");
        }
        return true;
    };
};

int main(int argc, char *argv[]) {
    Interpreter interpreter;
    if (argc > 1) {
        ifstream inFile(argv[1]);
        string line;
        while (getline(inFile, line)) {
            if (line[0] == '#') continue;
            interpreter.executeCommand(line);
        }
    } else {
        string input;
        cout << "Oktolang interpreter\n";
        while (true) {
            cout << "> ";
            getline(cin, input);
            interpreter.executeCommand(input); 
        }
    }
    return 0;
}


