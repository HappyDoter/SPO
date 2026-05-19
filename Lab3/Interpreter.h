#include <string>
#include <vector>
#include <unordered_map>
#include "SemanticAnalyzer.h"

using namespace std;

class Interpreter {
private:
    vector<Tetrad> tetrads;
    unordered_map<string, int> memory;
    unordered_map<string, int> labels;

    bool isIntegerLiteral(string& value);
    int getValue(string& operand);
    void setValue(string& name, int value);
    int divide(int left, int right, string& op);
    int power(int base, int exponent);
    int getLabelIndex(string& labelName);
    void collectLabels();

public:
    Interpreter(vector<Tetrad>& tetrads);
    void run();
};
