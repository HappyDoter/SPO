#include <iostream>
#include <exception>
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "Interpreter.h"

int main()
{
    setlocale(LC_ALL, "ru");
    Lexer analyzer("input.txt");
    analyzer.getNextToken();
    Parser a(analyzer);
    a.printTree(a.getSyntaxTree(), "", true);
    SemanticAnalyzer b(a.getSyntaxTree());
    b.printTetrad();
    b.printSymbolTable();

    cout << "\t\t\tРезультат интерпретации" << endl;
    Interpreter interpreter(b.getTetrads());
    interpreter.run();
}
