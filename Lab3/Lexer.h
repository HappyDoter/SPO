#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
using namespace std;


class Lexer {
private:
    ifstream file;
    char currentSymbol;
    int currentStr = 1;
    int currentPos = 0;
    int counter = 1;

    unordered_map <int, string> keywordType = {
        {1, "dim"}, {2, "as"}, {3, "integer"},
        {4, "mod"}, {5, "end"}, {6, "while"},
        {7, "readline"}, {8, "writeline"},
        {9, "and"}, {10, "or"}, {11, "xor"},
        {12, "not"}
    };

    unordered_map<int, char> specialSymbol = {
        {1, '+'}, {2, '-'}, {3, '*'},
        {4, '^'}, {5, '/'}, {6, '\\'},
        {7, '>'}, {8, '<'}, {9, '='},
        {10, '&'}, {11, '('}, {12, ')'}, {13, '\n'}
    };

    map<int, string> readWord;
    map<int, string> readConst;
    map<int, string> readKeywordType;
    map<int, char> readSpecialSymbol;
    map<pair<int, int>, char> readErrorSymbol;


    vector<pair<int, int>> analyzedInfo; // 1 - номер таблицы, 
                                         // 2 - номер строки в этой таблице
    /*
        1 - Таблица идентификаторов (readWord)
        2 - Таблица констант (readConst)
        3 - Таблица ключевых слов (readKeywordType)
        4 - Таблица специальных символов (readSpecialSymbol)
    */

    void getNextSymbol();
    void addAnalyzedInfo(int tableNum, int str);
    void addReadWordTable(string value);
    void addReadConstTable(string value);
    void addReadKeywordTypeTable(int position, string value);
    void addReadSpecialSymbolTable(int position, char value);
    void addReadErrorSymbolTable(int str, int pos, char value);
    bool isKeyword(string value);
    bool isSpecialSymbol(char value);

public:
    Lexer(string filename);
    void getNextToken();
    void printReadWord();
    void printReadConst();
    void printReadKeywordType();
    void printReadSpecialSymbol();
    void printReadErrorSymbol();
    void printAnalyzedInfo();
    void printOutputToFile(string filename);
    pair<int, int> getElAnalyzedInfo(int);
    string getElReadWord(int);
    string getElReadConst(int);
    string getElReadKeywordType(int);
    string getElReadSpecialSymbol(int);
    bool isWord(string);
    int getSizeAnalyzedInfo();
};