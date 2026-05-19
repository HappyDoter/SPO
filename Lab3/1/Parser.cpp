#include "Parser.h"
#include "Lexer.h"
#include <string>

int analyzePos = -1;

ostream& Parser::getOutput() {
    if (outputFile.is_open()) {
        return outputFile;
    }
    return cout;
}

Parser::Parser(Lexer& lexer) {
    outputFile.open("D:\\Study\\СПО\\Lab3\\parser_output.txt");
    this->lexer = &lexer;
    syntaxTree = TreeNode::createNode("Syntax Analyzer");
    start(-1);
    getOutput() << endl << endl << "Синтаксических ошибок не обнаружено" << endl << endl;
}

pair<int, int> Parser::getNextElement() {
    bool enterFlag = false;
    if (checkElement(4, 13)) enterFlag = true;

    if (analyzePos < lexer->getSizeAnalyzedInfo() - 1) {
        analyzePos++;
        currentElement.first = lexer->getElAnalyzedInfo(analyzePos).first;
        currentElement.second = lexer->getElAnalyzedInfo(analyzePos).second;
        if (checkElement(4, 13) && enterFlag) {
            while (checkElement(4, 13)) {
                if (analyzePos < lexer->getSizeAnalyzedInfo() - 1) {
                    analyzePos++;
                    currentElement.first = lexer->getElAnalyzedInfo(analyzePos).first;
                    currentElement.second = lexer->getElAnalyzedInfo(analyzePos).second;
                }
                else {
                    currentElement.first = 0;
                    currentElement.second = 0;
                    return { 0,0 };
                }
            }
            analyzePos--;
            currentElement.first = lexer->getElAnalyzedInfo(analyzePos).first;
            currentElement.second = lexer->getElAnalyzedInfo(analyzePos).second;
            pair<int, int> nextElement = lexer->getElAnalyzedInfo(analyzePos);
            return nextElement;
        }
        else {
            pair<int, int> nextElement = lexer->getElAnalyzedInfo(analyzePos);
            return nextElement;
        }
    }

    currentElement.first = 0;
    currentElement.second = 0;
    return { 0,0 };
}

pair<int, int> Parser::getPrevElement() {
    if (analyzePos < lexer->getSizeAnalyzedInfo() - 1) {
        analyzePos--;
        currentElement.first = lexer->getElAnalyzedInfo(analyzePos).first;
        currentElement.second = lexer->getElAnalyzedInfo(analyzePos).second;
        pair<int, int> nextElement = lexer->getElAnalyzedInfo(analyzePos);
        return nextElement;
    }
    currentElement.first = 0;
    currentElement.second = 0;
    return { 0,0 };
}

bool Parser::checkElement(int table, int line = -1) {

    if (currentElement.first == table && currentElement.second == line || 
        currentElement.first == table && line == -1) {
        return true;
    }
    return false;
}

string Parser::getElement() {
    switch (currentElement.first) {
        case 1: return lexer->getElReadWord(currentElement.second);
        case 2: return lexer->getElReadConst(currentElement.second);
        case 3: return lexer->getElReadKeywordType(currentElement.second);
        case 4: return lexer->getElReadSpecialSymbol(currentElement.second);
    }
    return " ";
}

string Parser::getElementPos(int pos = -1){
    switch (lexer->getElAnalyzedInfo(pos).first) {
        case 1: return lexer->getElReadWord(lexer->getElAnalyzedInfo(pos).second);
        case 2: return lexer->getElReadConst(lexer->getElAnalyzedInfo(pos).second);
        case 3: return lexer->getElReadKeywordType(
            lexer->getElAnalyzedInfo(pos).second);
        case 4: return lexer->getElReadSpecialSymbol(
            lexer->getElAnalyzedInfo(pos).second);
    }
    return " ";
}


// <start> -> <announcement> | <assignment> | <cycle> | <input> | <output>
TreeNode* Parser::start(int a = -1) {
    TreeNode* node = nullptr;
    if (analyzePos == -1) currentElement = getNextElement();
    while (analyzePos != -1) {
        if (checkElement(3, 1)) { // DIM
            node = announcement();
            if (a == -1) syntaxTree->addChildren({ node });
        }
        else if (checkElement(1)) { // ID 
            node = assignment();
            if (a == -1) syntaxTree->addChildren({ node });
        }
        else if (checkElement(3, 8)) { // Writeline
            getNextElement();
            node = output();
            if (a == -1) syntaxTree->addChildren({ node });

        }
        else if (checkElement(3, 7)) { // Readline
            node = input();
            if (a == -1) syntaxTree->addChildren({ node });
        }
        else if (checkElement(3, 6)) { // While
            node = cycle();
            if (a == -1) syntaxTree->addChildren({ node });
        }
        else if (checkElement(4, 13)) { // \n
            getNextElement();
        }
        else if (checkElement(0, 0)) {
            return node;
        }
        else {
            cout << "Встречен неожиданный символ :" << getElement();
            exit(0);
        }

        if (a != -1) {
            return node;
        }
    }
}

// <announcement> -> "dim" < id > "as" "integer" "=" <expression> 
// | "dim" < id > "as" "integer"
TreeNode* Parser::announcement() {
    vector<TreeNode*> declarations;
    string id;
    getNextElement();
    if (checkElement(1)) {
        id = lexer->getElReadWord(lexer->getElAnalyzedInfo(analyzePos).second);
        getNextElement();
        if (checkElement(3, 2)) {
            getNextElement();
            if (checkElement(3, 3)) {
                getNextElement();
                if (checkElement(4, 9)) {
                    getNextElement();
                    declarations.push_back(TreeNode::createNode("AS", { new TreeNode(id),
                        TreeNode::createNode("INTEGER", {TreeNode::createNode("=", 
                            {expression()})})}));
                    return TreeNode::createNode("DIM", declarations);
                }
                else {
                    declarations.push_back(TreeNode::createNode("AS", { new TreeNode(id), 
                        new TreeNode("INTEGER") }));
                    return TreeNode::createNode("DIM", declarations);
                }
            }
            else {
                cout << "Указан неверный тип переменной :" << getElement() << endl;
                exit(0);
            }
        }
        else {
            cout << "Указано неверное ключевое слово :" << getElement() << endl;
            exit(0);
        }
    }
    else {
        cout << "Идентификатор не найден :" << getElement() << endl;
        exit(0);
    }
}


// <assignment> -> <id> <assignspecchar> <expression>
// <assignspecchar> -> "=" | "+=" | "-=" | "^=" | "*=" | "/=" | "\=" | "&="
TreeNode* Parser::assignment() {
    string separator;
    vector <TreeNode*> declarations;
    declarations.push_back(new TreeNode(lexer->getElReadWord(
        lexer->getElAnalyzedInfo(analyzePos).second)));
    getNextElement();
    if (checkElement(4, 9)) {
        separator = lexer->getElReadSpecialSymbol(
            lexer->getElAnalyzedInfo(analyzePos).second);
        getNextElement();
        declarations.push_back(expression());
        return TreeNode::createNode(separator, declarations);
    }
    else if (checkElement(4, 1) || checkElement(4, 2) || checkElement(4, 3) || checkElement(4, 4) || checkElement(4, 5) || checkElement(4, 6) || checkElement(4, 10)) {
        separator = lexer->getElReadSpecialSymbol(
            lexer->getElAnalyzedInfo(analyzePos).second);
        getNextElement();
        if (checkElement(4, 9)) {
            separator += lexer->getElReadSpecialSymbol(
                lexer->getElAnalyzedInfo(analyzePos).second);
            getNextElement();
            declarations.push_back(expression());
            return TreeNode::createNode(separator, declarations);
        }
        else {
            cout << "Введён неверный разделитель :" << getElement() << endl;
            exit(0);
        }
    }
    else {
        cout << "Введён неверный разделитель :" << getElement() << endl;
        exit(0);
    }
}

// <expression>::= <conditions>
TreeNode* Parser::expression() {
    vector<TreeNode*> nodes;
    vector<string> operators;
    nodes.push_back(Conditions());

    TreeNode* result = nodes[0];
    for (size_t i = 0; i < operators.size(); i++) {
        result = TreeNode::createNode(operators[i], { result, nodes[i + 1] });
    }

    return result;
}

// <conditions>::= <OrXor>{ "=" <OrXor> | "<>" <OrXor> | "<" <OrXor> | ">" <OrXor> | "<=" <OrXor> | ">=" <OrXor> } 
TreeNode* Parser::Conditions() {
    vector<TreeNode*> nodes;
    vector<string> operators;
    nodes.push_back(OrXor());
    string str;
    while (checkElement(4, 7) || checkElement(4, 8) || checkElement(4, 9)) {
        if (checkElement(4, 7)) {
            str = getElementPos(analyzePos);
            getNextElement();
            if (checkElement(4, 9)) {
                str += getElementPos(analyzePos);
                getNextElement();
            }
        } else if (checkElement(4, 8)) {
            str = getElementPos(analyzePos);
            getNextElement();
            if (checkElement(4, 7) || checkElement(4, 9)) {
                str += getElementPos(analyzePos);
                getNextElement();
            }
        } else if (checkElement(4, 9)) {
            str = getElementPos(analyzePos);
            getNextElement();
        }

        operators.push_back(str);
        nodes.push_back(OrXor());
    }

    TreeNode* result = nodes[0];
    for (size_t i = 0; i < operators.size(); i++) {
        result = TreeNode::createNode(operators[i], { result, nodes[i + 1] });
    }

    return result;
}


// <OrXor>::=  <And> {"or" <And> | "xor" <And> }
TreeNode* Parser::OrXor() {
    vector<TreeNode*> nodes;
    vector<string> operators;
    nodes.push_back(And());

    while (checkElement(3, 10) || checkElement(3, 11)) {
        operators.push_back(getElementPos(analyzePos));
        getNextElement();
        nodes.push_back(And());
    }

    TreeNode* result = nodes[0];
    for (size_t i = 0; i < operators.size(); i++) {
        result = TreeNode::createNode(operators[i], { result, nodes[i + 1] });
    }

    return result;
}

// <And>::= <Ampersand> {"and" <Ampersand>}
TreeNode* Parser::And() {
    vector<TreeNode*> nodes;
    vector<string> operators;
    nodes.push_back(Ampersand());

    while (checkElement(3, 9)) {
        operators.push_back(getElementPos(analyzePos));
        getNextElement();
        nodes.push_back(Ampersand());
    }

    TreeNode* result = nodes[0];
    for (size_t i = 0; i < operators.size(); i++) {
        result = TreeNode::createNode(operators[i], { result, nodes[i + 1] });
    }

    return result;
}

// <Ampersand>::=  <PlusMinus> {"&" <PlusMinus>}
TreeNode* Parser::Ampersand() {
    vector<TreeNode*> nodes;
    vector<string> operators;
    nodes.push_back(PlusMinus());

    while (checkElement(4, 10)) {
        operators.push_back(getElementPos(analyzePos));
        getNextElement();
        nodes.push_back(PlusMinus());
    }

    TreeNode* result = nodes[0];
    for (size_t i = 0; i < operators.size(); i++) {
        result = TreeNode::createNode(operators[i], { result, nodes[i + 1] });
    }

    return result;
}

// <PlusMinus>::=  <Mod> { "+" <Mod> | "-" <Mod> }
TreeNode* Parser::PlusMinus() {
    vector<TreeNode*> nodes;
    vector<string> operators;
    nodes.push_back(Mod());

    while (checkElement(4, 1) || checkElement(4, 2)) {
        operators.push_back(getElementPos(analyzePos));
        getNextElement();
        nodes.push_back(Mod());
    }

    TreeNode* result = nodes[0];
    for (size_t i = 0; i < operators.size(); i++) {
        result = TreeNode::createNode(operators[i], { result, nodes[i + 1] });
    }

    return result;
}

// <Mod>::=  <Remainder> {"mod" <Remainder> }
TreeNode* Parser::Mod() {
    vector<TreeNode*> nodes;
    vector<string> operators;
    nodes.push_back(Remainder());

    while (checkElement(3, 4)) {
        operators.push_back(getElementPos(analyzePos));
        getNextElement();
        nodes.push_back(Remainder());
    }

    TreeNode* result = nodes[0];
    for (size_t i = 0; i < operators.size(); i++) {
        result = TreeNode::createNode(operators[i], { result, nodes[i + 1] });
    }

    return result;
}

// <Remainder>::=  <MultiplDiv> {"/" <MultiplDiv>}
TreeNode* Parser::Remainder() {
    vector<TreeNode*> nodes;
    vector<string> operators;
    nodes.push_back(MultiplDiv());

    while (checkElement(4, 6)) {
        operators.push_back(getElementPos(analyzePos));
        getNextElement();
        nodes.push_back(MultiplDiv());
    }

    TreeNode* result = nodes[0];
    for (size_t i = 0; i < operators.size(); i++) {
        result = TreeNode::createNode(operators[i], { result, nodes[i + 1] });
    }

    return result;
}

// <MultiplDiv>::=  <Degree> { "*" <Degree> | "/" <Degree>}
TreeNode* Parser::MultiplDiv() {
    vector<TreeNode*> nodes;
    vector<string> operators;
    nodes.push_back(Degree());

    while (checkElement(4, 3) || checkElement(4, 5)) {
        operators.push_back(getElementPos(analyzePos));
        getNextElement();
        nodes.push_back(Degree());
    }

    TreeNode* result = nodes[0];
    for (size_t i = 0; i < operators.size(); i++) {
        result = TreeNode::createNode(operators[i], { result, nodes[i + 1] });
    }

    return result;
}

// <Degree>::=  <input> {"^" <input>}
TreeNode* Parser::Degree() {
    vector<TreeNode*> nodes;
    vector<string> operators;
    nodes.push_back(input());
    while (checkElement(4, 4)) {
        operators.push_back(getElementPos(analyzePos));
        getNextElement();
        nodes.push_back(input());
    }
    TreeNode* result = nodes[0];
    for (size_t i = 0; i < operators.size(); i++) {
        result = TreeNode::createNode(operators[i], { result, nodes[i + 1] });
    }

    return result;

}

// <input>::=  <element> ["readline"]
TreeNode* Parser::input() {
    vector<TreeNode*> nodes;
    vector<string> operators;
    nodes.push_back(element());

    TreeNode* result = nodes.back();
    for (int i = (int)operators.size() - 1; i >= 0; i--) {
        result = TreeNode::createNode(operators[i], { nodes[i], result });
    }

    return result;
}

// <element>::=  ["-" | "+"] (<id> | <constant> | <input> | "not" "(" <expression> ")" | ["("] <expression> [")"])
TreeNode* Parser::element() {
    string unary = "", Not = "";
    TreeNode* node = nullptr;

    if (checkElement(4, 1)) {
        unary = getElementPos(analyzePos);
        getNextElement();
    }
    else if (checkElement(4, 2)) {
        unary = getElementPos(analyzePos);
        getNextElement();
    }

    if (checkElement(1)) { 
        string value = getElementPos(analyzePos);
        node = new TreeNode(value);
        getNextElement();
    }
    else if (checkElement(2)) {
        string value = getElementPos(analyzePos);
        node = new TreeNode(value);
        getNextElement();
    }
    else if (checkElement(3, 7)) {
        string value = getElementPos(analyzePos);
        node = new TreeNode(value);
        getNextElement();
        if (getElement() == "(") {
            getNextElement();
            if (getElement() == ")") {
                getNextElement();
            }
            else {
                cout << "Отсутствует закрывающая скобка ";
                exit(0);
            }
        }
        else {
            cout << "Отсутствует открывающая скобка ";
            exit(0);
        }
    }
    else if (checkElement(3, 12)) {
        Not = getElementPos(analyzePos);
        getNextElement();

    }
    else if (checkElement(4, 11)) {
        getNextElement();
        node = expression();

        if (checkElement(4, 9)) { 
            getNextElement();
            node = expression();
        }

        if (checkElement(4, 8)) { 
            getNextElement();
            if (checkElement(4, 7)) {
                getNextElement();
                node = expression();
            }
            else if (checkElement(4, 9)) {
                getNextElement();
                node = expression();
            } else node = expression();
        }

        if (checkElement(4, 7)) {
            getNextElement();
            if (checkElement(4, 9)) {
                getNextElement();
                node = expression();
            }
            else node = expression();
        }

        if (getElement() == ")") {
            getNextElement();
        }
        else {
            cout << "Нет закрывающей скобки" ;
            exit(0);
        }
    }
    else {
        cout << "Встречен неожиданный символ :" << getElement();
        exit(0);
    }

    if (unary != "") {
        if (unary == "-") {
            node = TreeNode::createNode("-", { node });
        }
        else
            node = TreeNode::createNode("+", { node });
    }

    if (Not != "") {
        if (getElement() == "(") {
            getNextElement();
            if (getElement() == ")") {
                cout << "Отсутствует идентификатор для элемента not";
                exit(0);
            }
            else {
                node = expression();
                if (unary != "") {
                    if (unary == "-") {
                        node = TreeNode::createNode("-", { TreeNode::createNode(Not, { node})});
                    }
                    else
                        node = TreeNode::createNode("+", { TreeNode::createNode(Not, { node})});
                }
                else {
                    node = TreeNode::createNode(Not, { node });
                }
                if (getElement() == ")") {
                    getNextElement();
                }
                else {
                    cout << "Отсутствует закрывающая скобка для not";
                    exit(0);
                }
            }
        }
        else {
            cout << "Отсутствует открывающая скобка для not";
            exit(0);
        }

    }
    return node;
}


// <output>::= "writeline" "(" <expression> ")"
TreeNode* Parser::output() {
  //  vector<TreeNode*> declarations;
    bool isOpenBracket = false;
    TreeNode* node = nullptr;
    string separator = "";
    string id = " ";

    if (checkElement(4, 11)) {

        getNextElement();
        node = expression();
        if (checkElement(4, 12)) {
            getNextElement();
            return TreeNode::createNode("writeline", { node });
        }
        else if (checkElement(4,12)){
            cout << "Нет закрывающей скобки" << endl;
            exit(0);
        }
        else {
            cout << "Встречен неожиданный символ :" << getElement() << endl;
            exit(0);
        }
    }
    else {
        cout << "Нет открывающей скобки" << endl;
        exit(0);
    }
}

// <cycle> -> "while" ["("] <expression> [")"] {<start>} "end" "while"
TreeNode* Parser::cycle() {
    vector<TreeNode*> declarations, bodyCycle, cycleElement;
    TreeNode* head;
    string separator;

    getNextElement();
    cycleElement.push_back(expression());
    getNextElement();

    cycleElement.push_back(new TreeNode(""));
    cycleElement.push_back(new TreeNode(""));
    cycleElement.push_back(new TreeNode("Body_Cycle"));
    cycleElement.push_back(new TreeNode(""));
    while (!checkElement(3, 5)) {
        cycleElement.push_back(start(0));
        cycleElement.push_back(new TreeNode(""));
        if (checkElement(0, 0)) {
            cout << "Цикл не закрыт" << endl;
            exit(0);
        }
    }
    getNextElement();
    if (checkElement(3, 6)) {
        getNextElement();
        cycleElement.push_back(TreeNode::createNode("end", { new TreeNode("while") }));
        return TreeNode::createNode("while", cycleElement);
    }
    else {
        cout << "Цикл не закрыт" << endl;
        exit(0);
    }
}

void Parser::printTree(TreeNode* node, string prefix, bool isLast) {
    ostream& out = getOutput();
    if (node != nullptr) {
        if (prefix == "    ") {
            out << "    |" << endl << "    |" << endl;
            out << prefix;
        }
        else {
            out << prefix;
        }

        if (node->value == "") {
            out << (isLast ? "'--- " : "|    ");
        }
        else {
            out << (isLast ? "'--- " : "|--- ");
        }
        out << node->value << endl;


        for (size_t i = 0; i < node->children.size(); i++) {
            bool lastChild = (i == node->children.size() - 1);
            printTree(node->children[i], prefix + (isLast ? "    " : "|   "), lastChild);
        }
        
    }
}