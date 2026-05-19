#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include "TreeNode.h"

using namespace std;

// Структура переменной
struct VarInfo {
    string name;
    string type;
    bool declared;
    bool initialized;
    VarInfo()
        : name(""), type(""), declared(false), initialized(false) {
    }

    VarInfo(string n, string t)
        : name(n), type(t), declared(false), initialized(false) {
    }
};

// Структура области видимости
struct Scope {
    string name;
    int level;
    Scope* parent;
    unordered_map<string, VarInfo> variables;
};

// Структура тетрады
struct Tetrad {
    string op;     
    string arg1;   
    string arg2;    
    string result;  

    Tetrad(string o, string a1, string a2, string r)
        : op(o), arg1(a1), arg2(a2), result(r) {
    }
};

class SemanticAnalyzer {
private:
    vector<Tetrad> tetrad;          
    unordered_map<string, VarInfo> symbolTable; 
    int tempCounter;                       
    int labelCounter;

    Scope* currentScope;
    vector<Scope*> allScopes;
    ofstream outputFile;

    string generateTemp();
    string generateLabel();
    void checkVariableDeclared(string& varName);
    void checkVariableInitialized(string& varName);

    void analyzeNode(TreeNode* node);
    void analyzeAnnouncement(TreeNode* node);
    void analyzeAssignment(TreeNode* node);
    void analyzeExpression(TreeNode* node, string& resultPlace);
    void analyzeCycle(TreeNode* node);
    void analyzeOutput(TreeNode* node);
    void analyzeInput(TreeNode* node);

    void analyzeSeparator(TreeNode* node, string& resultPlace);
    void analyzeElement(TreeNode* node, string& resultPlace);
    ostream& getOutput();

public:
    SemanticAnalyzer(TreeNode*);
    void analyze(TreeNode* syntaxTree);
    void printTetrad();
    void printSymbolTable();
    vector<Tetrad>& getTetrads();
};
