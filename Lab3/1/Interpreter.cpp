#include "Interpreter.h"
#include <cctype>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>

Interpreter::Interpreter(vector<Tetrad>& tetrads) {
    this->tetrads = tetrads;
    collectLabels();
}

bool Interpreter::isIntegerLiteral( string& value)  {
    if (value.empty()) return false;

    int pos = 0;
    if (value[0] == '-' || value[0] == '+') {
        if (value.size() == 1) return false;
        pos = 1;
    }

    for (int i = pos; i < value.size(); i++) {
        if (!isdigit(value[i])) {
            return false;
        }
    }
    return true;
}

int Interpreter::getValue(string& operand) {
    if (operand.empty()) return 0;

    if (isIntegerLiteral(operand)) {
        return stoll(operand);
    }

    auto it = memory.find(operand);
    return it->second;
}

void Interpreter::setValue(string& name, int value) {
    memory[name] = value;
}

int Interpreter::divide(int left, int right, string& op){
    if (right == 0) {
        cout << "Деление на ноль в операции '" + op + "'" << endl;
        exit(0);
    }
    return left / right;
}

int Interpreter::power(int base, int exponent) {
    if (exponent < 0) {
        cout << "Отрицательная степень не поддерживается для целочисленного интерпретатора" << endl;
        exit(0);
    }

    int result = 1;
    while (exponent > 0) {
        if (exponent % 2 == 1) result *= base;
        base *= base;
        exponent /= 2;
    }
    return result;
}

void Interpreter::collectLabels() {
    labels.clear();
    for (int i = 0; i < tetrads.size(); i++) {
        if (tetrads[i].op == "LABEL") {
            labels[tetrads[i].arg1] = i;
        }
    }
}

int Interpreter::getLabelIndex(string& labelName) {
    auto it = labels.find(labelName);
    return it->second;
}

void Interpreter::run() {
    int pc = 0;

    while (pc < tetrads.size()) {
        Tetrad& q = tetrads[pc];
        string& op = q.op;

        if (op == "LABEL") {
            pc++;
        }
        else if (op == "JMP") {
            pc = getLabelIndex(q.result);
        }
        else if (op == "JF") {
            if (getValue(q.arg1) == 0) {
                pc = getLabelIndex(q.result);
            }
            else {
                pc++;
            }
        }
        else if (op == "READ") {
            string input;
            cout << "Введите значение для " << q.result << ": ";
            if (!(cin >> input) || !isIntegerLiteral(input)) {
                cout << "Ошибка ввода целого числа" << endl;
                exit(0);
            }
            int value = stoi(input);
            setValue(q.result, value);
            pc++;
        }
        else if (op == "WRITE") {
            cout << getValue(q.arg1) << endl;
            pc++;
        }
        else if (op == "UMINUS") {
            setValue(q.result, -getValue(q.arg1));
            pc++;
        }
        else if (op == "NOT") {
            setValue(q.result, getValue(q.arg1) == 0 ? 1 : 0);
            pc++;
        }
        else if (op == "=") {
            if (q.arg2.empty()) {
                setValue(q.result, getValue(q.arg1));
            }
            else {
                setValue(q.result, getValue(q.arg1) == getValue(q.arg2) ? 1 : 0);
            }
            pc++;
        }
        else {
            int left = getValue(q.arg1);
            int right = getValue(q.arg2);
            int result = 0;

            if (op == "+") result = left + right;
            else if (op == "-") result = left - right;
            else if (op == "*") result = left * right;
            else if (op == "/") result = divide(left, right, op);
            else if (op == "\\") result = divide(left, right, op);
            else if (op == "mod") {
                if (right == 0) {
                    cout << "Деление на ноль в операции 'mod'" << endl;
                    exit(0);
                }
                result = left % right;
            }
            else if (op == "^") result = power(left, right);
            else if (op == "&") result = left & right;
            else if (op == "and") result = (left != 0 && right != 0) ? 1 : 0;
            else if (op == "or") result = (left != 0 || right != 0) ? 1 : 0;
            else if (op == "xor") result = ((left != 0) != (right != 0)) ? 1 : 0;
            else if (op == "<>") result = left != right ? 1 : 0;
            else if (op == "<") result = left < right ? 1 : 0;
            else if (op == ">") result = left > right ? 1 : 0;
            else if (op == "<=") result = left <= right ? 1 : 0;
            else if (op == ">=") result = left >= right ? 1 : 0;
            else {
                cout << "Неизвестная операция в тетраде: '" + op + "'" << endl;
                exit(0);
            }

            setValue(q.result, result);
            pc++;
        }
    }
}
