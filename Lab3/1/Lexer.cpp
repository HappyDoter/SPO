#include "Lexer.h"
#include <iomanip>
#include <iostream>

void Lexer::getNextSymbol() {
    if (file.get(currentSymbol)) {
        currentPos++;
        currentSymbol = tolower(currentSymbol);
    }
    else {
        currentSymbol = '\0';
    }
}

void Lexer::addAnalyzedInfo(int tableNum, int str) {
    analyzedInfo.push_back(make_pair(tableNum, str));
}

void Lexer::addReadWordTable(std::string value) {
    static int position = 1;
    for (const auto& element : readWord) {
        if (element.second == value) {
            addAnalyzedInfo(1, element.first);
            return;
        }
    }
    readWord.insert({ position, value });
    addAnalyzedInfo(1, position);
    position++;
    return;
}


void Lexer::addReadConstTable(std::string value) {
    static int position = 1;
    for (const auto& element : readConst) {
        if (element.second == value) {
            addAnalyzedInfo(2, element.first);
            return;
        }
    }
    readConst.insert({ position, value });
    addAnalyzedInfo(2, position);
    position++;
    return;
}

void Lexer::addReadKeywordTypeTable(int position, std::string value) {
    for (const auto& element : readKeywordType) {
        if (element.second == value) {
            addAnalyzedInfo(3, element.first);
            return;
        }
    }
    readKeywordType.insert({ position, value });
    addAnalyzedInfo(3, position);
    return;
}


void Lexer::addReadSpecialSymbolTable(int position, char value) {
    for (const auto& element : readSpecialSymbol) {
        if (element.second == value) {
            addAnalyzedInfo(4, element.first);
            return;
        }
    }
    readSpecialSymbol.insert({ position, value });
    addAnalyzedInfo(4, position);
    return;
}


void Lexer::addReadErrorSymbolTable(int str, int pos, char value) {
    readErrorSymbol.insert({ { str, pos }, value });
    return;
}

bool Lexer::isKeyword(std::string value) {
    for (const auto& element : keywordType) {
        if (element.second == value) {
            addReadKeywordTypeTable(element.first, value);
            return true;
        }
    }
    return false;
}

bool Lexer::isSpecialSymbol(char value) {
    for (const auto& element : specialSymbol) {
        if (element.second == value) {
            addReadSpecialSymbolTable(element.first, value);
            return true;
        }
    }
    return false;
}

Lexer::Lexer(string filename) {
    file.open(filename);
    if (!file.is_open()) {
        cout << "Файл не найден";
        exit(0);
    }
    getNextSymbol();

}

void Lexer::getNextToken() {
    while (currentSymbol != '\0') {
        string result = "";

        if (isspace(currentSymbol) != 0 && currentSymbol != '\n') {
            getNextSymbol();
            continue;
        }

        if (isdigit(currentSymbol) != 0) {
            while (isdigit(currentSymbol) != 0) {
                result += currentSymbol;
                getNextSymbol();
            }
            addReadConstTable(result);
            continue;
        }

        if (isalpha(currentSymbol) != 0 || currentSymbol == '_') {
            while (isalnum(currentSymbol) != 0 || currentSymbol == '_') {
                result += currentSymbol;
                getNextSymbol();
            }

            if (isKeyword(result) == true) {
                continue;
            }
            addReadWordTable(result);
            
            continue;
        }

        if (isSpecialSymbol(currentSymbol) == true) {
            if (currentSymbol == '\n') {
                currentStr++;
                currentPos = 0;
            }
            getNextSymbol();
            continue;
        }

        addReadErrorSymbolTable(currentStr, currentPos, currentSymbol);
        getNextSymbol();
        continue;
    }
    printOutputToFile("D:\\Study\\СПО\\Lab3\\lexer_output.txt");
    return;
}

void Lexer::printReadWord() {
    cout << setw(35) << "1 Таблица идентификаторов\n";
    cout << setw(18) << "Номер добавления" << setw(24) << "Лексема\n";
    for (const auto& element : readWord) {
        cout << setw(10) << element.first << setw(30)
            << element.second << endl;
    }
    cout << "\n\n";
}

void Lexer::printReadConst() {
    cout << setw(28) << "2 Таблица констант\n";
    cout << setw(18) << "Номер добавления" << setw(15) << "Лексема\n";
    for (const auto& element : readConst) {
        cout << setw(10) << element.first << setw(20)
            << element.second << endl;
    }
    cout << "\n\n";
}

void Lexer::printReadKeywordType() {
    cout << setw(35) << "3 Таблица ключевых слов\n";
    cout << setw(18) << "Номер добавления" << setw(25) << "Лексема\n";
    for (const auto& element : readKeywordType) {
        cout << setw(10) << element.first << setw(30)
            << element.second << endl;
    }
    cout << "\n\n";
}

void Lexer::printReadSpecialSymbol() {
    cout << setw(35) << "4 Таблица специальных символов\n";
    cout << setw(18) << "Номер добавления" << setw(16) << "Лексема\n";
    for (const auto& element : readSpecialSymbol) {
        cout << setw(10) << element.first << setw(20)
            << element.second << endl;
    }
    cout << "\n\n";
}

void Lexer::printReadErrorSymbol() {
    cout << setw(35) << "Таблица ошибок\n";
    cout << setw(16) << "Номер строки" << setw(20) << "Номер позиции"
        << setw(18) << "Лексема\n";
    for (const auto& element : readErrorSymbol) {
        cout << setw(10) << element.first.first << setw(20)
            << element.first.second << setw(20) << element.second << endl;
    }
    cout << "\n\n";
}

void Lexer::printAnalyzedInfo() {
    cout << setw(30) << "Данные анализа\n";
    cout << setw(17) << "Номер таблицы" << setw(20) << "Номер строки\n";
    for (const auto& element : analyzedInfo) {
        cout << setw(10) << element.first << setw(20)
            << element.second << endl;
    }
}

void Lexer::printOutputToFile(string filename) {
    ofstream out(filename);
    if (!out.is_open()) {
        cout << "Не получилось открыть файл лексического анализатора для записи: " << filename << endl;
        return;
    }

    streambuf* oldCout = cout.rdbuf(out.rdbuf());
    printReadWord();
    printReadConst();
    printReadKeywordType();
    printReadSpecialSymbol();
    printReadErrorSymbol();
    printAnalyzedInfo();
    cout.rdbuf(oldCout);
}

pair<int, int> Lexer::getElAnalyzedInfo(int pos) {
    return analyzedInfo[pos];
}

string Lexer::getElReadWord(int pos) {
    for (const auto& element : readWord) {
        if (element.first == pos) return element.second;
    }
}

string Lexer::getElReadConst(int pos) {
    for (const auto& element : readConst) {
        if (element.first == pos) return element.second;
    };
}

string Lexer::getElReadKeywordType(int pos) {
    for (const auto& element : readKeywordType) {
        if (element.first == pos) return element.second;
    }
}

string Lexer::getElReadSpecialSymbol(int pos) {
    string a;
    for (const auto& element : readSpecialSymbol) {
        if (element.first == pos) {
            a.push_back(element.second);
            return a;
        }
    }
}

bool Lexer::isWord(string currentElement) {
    for (const auto& element : readWord) {
        if (element.second == currentElement) return true;
    }
    return false;
}

int Lexer::getSizeAnalyzedInfo() {
    return analyzedInfo.size();
}