#include "SemanticAnalyzer.h"
#include <iomanip>
#include <cctype>

namespace {
    bool isComparisonOp(string& op) {
        return op == "=" || op == "<>" || op == "<" ||
            op == ">" || op == "<=" || op == ">=";
    }

    bool isBinaryOp(string& op) {
        return op == "or" || op == "xor" || op == "and" || op == "&" ||
            op == "+" || op == "-" || op == "mod" ||
            op == "*" || op == "/" || op == "\\" || op == "^" ||
            isComparisonOp(op);
    }
}

ostream& SemanticAnalyzer::getOutput() {
    if (outputFile.is_open()) {
        return outputFile;
    }
    return cout;
}

SemanticAnalyzer::SemanticAnalyzer(TreeNode* syntaxTree)
    : tempCounter(0), labelCounter(0), currentScope(nullptr) {
    outputFile.open("D:\\Study\\СПО\\Lab3\\semantic_output.txt");
    currentScope = new Scope{"global", 0, nullptr, {} };
    allScopes.push_back(currentScope);
    analyze(syntaxTree);
}

// Генерация имени временной переменной
string SemanticAnalyzer::generateTemp() {
    return "t" + to_string(tempCounter++);
}

// Генерация имени метки
string SemanticAnalyzer::generateLabel() {
    return "L" + to_string(labelCounter++);
}

// Проверка, объявлена ли переменная
void SemanticAnalyzer::checkVariableDeclared(string& varName) {
    Scope* scope = currentScope;
    while (scope != nullptr) {
        if (scope->variables.find(varName) != scope->variables.end()) {
            return;
        }
        scope = scope->parent;
    }
    cout << "Семантическая ошибка: переменная '" + varName + "' не объявлена";
    exit(0);
}

// Проверка, инициализирована ли переменная
void SemanticAnalyzer::checkVariableInitialized(string& varName) {
    Scope* scope = currentScope;
    while (scope != nullptr) {
        auto it = scope->variables.find(varName);
        if (it != scope->variables.end()) {
            if (!it->second.initialized) {
                cout << "Семантическая ошибка: переменная '" + varName + "' не инициализирована";
                exit(0);
            }
            return;
        }
        scope = scope->parent;
    }
    cout << "Семантическая ошибка: переменная '" + varName + "' не объявлена";
    exit(0);
}

// Основной метод семантического анализа
void SemanticAnalyzer::analyze(TreeNode* syntaxTree) {
    if (syntaxTree) {
        for (auto child : syntaxTree->children) {
            analyzeNode(child);
        }
    }
    else {
        cout << "Синтаксическое дерево пустое" << endl;
        exit(0);
    }
    getOutput() << "Семантических ошибок не обнаружено" << endl;
}

// Анализ узла дерева
void SemanticAnalyzer::analyzeNode(TreeNode* node) {
    if (!node) return;

    if (node->value == "DIM") {
        analyzeAnnouncement(node);
    }
    else if (node->children.size() == 2 && node->children[1]->value == "readline") {
        analyzeInput(node);
    }
    else if (node->value == "=" || node->value == "+=" || node->value == "-=" ||
        node->value == "*=" || node->value == "/=" || node->value == "\\=" ||
        node->value == "^=" || node->value == "&=") {
        analyzeAssignment(node);
    }
    else if (node->value == "while") {
        analyzeCycle(node);
    }
    else if (node->value == "writeline") {
        analyzeOutput(node);
    }
}

// Анализ объявления переменной
void SemanticAnalyzer::analyzeAnnouncement(TreeNode* node) {
    TreeNode* asNode = node->children[0];

    string varName = asNode->children[0]->value;
    string typeName = asNode->children[1]->value;

    for ( auto& scope : allScopes) {
        if (scope->variables.find(varName) != scope->variables.end()) {
            cout << "Семантическая ошибка: переменная '" + varName + "' уже объявлена";
            exit(0);
        }
    }

    // Добавление переменной в текущую область видимости
    VarInfo varInfo(varName, typeName);
    varInfo.declared = true;
    currentScope->variables[varName] = varInfo;

    // Если есть инициализация
    if (asNode->children[1]->children.size() > 0) {
        TreeNode* initNode = asNode->children[1]->children[0];
        if (initNode->value == "=" && initNode->children.size() > 0) {
            string tempResult;
            analyzeExpression(initNode->children[0], tempResult);

            tetrad.push_back(Tetrad("=", tempResult, "", varName));
            currentScope->variables[varName].initialized = true;
        }
    }
}

// Анализ присваивания
void SemanticAnalyzer::analyzeAssignment(TreeNode* node) {
    string varName = node->children[0]->value;
    string op = node->value;

    checkVariableDeclared(varName);

    string tempResultExp;
    analyzeExpression(node->children[1], tempResultExp);

    // Поиск переменной в областях видимости
    Scope* scope = currentScope;
    while (scope != nullptr) {
        auto it = scope->variables.find(varName);
        if (it != scope->variables.end()) {
            scope->variables[varName].initialized = true;
            break;
        }
        scope = scope->parent;
    }

    if (op == "=") {
        tetrad.push_back(Tetrad("=", tempResultExp, "", varName));
    }
    else {
        string tempVar = generateTemp();
        tetrad.push_back(Tetrad("=", varName, "", tempVar));

        string opSymbol = op.substr(0, 1);
        string resultTemp = generateTemp();
        tetrad.push_back(Tetrad(opSymbol, tempVar, tempResultExp, resultTemp));

        tetrad.push_back(Tetrad("=", resultTemp, "", varName));
    }
}

// Анализ выражения
void SemanticAnalyzer::analyzeExpression(TreeNode* node, string& resultPlace) {
    if (!node) {
        resultPlace = "";
        return;
    }

    // Унарные операции
    if (((node->value == "+" || node->value == "-") && node->children.size() == 1) ||
        (node->value == "not" && node->children.size() == 1)) {
        analyzeElement(node, resultPlace);
        return;
    }

    // Бинарные операции, включая операции сравнения
    if (node->children.size() == 2 && isBinaryOp(node->value)) {
        analyzeSeparator(node, resultPlace);
        return;
    }

    // Если у узла есть дети, но это не распознанная операция — считаем ошибкой
    if (!node->children.empty()) {
        cout << "Семантическая ошибка: неподдерживаемый узел выражения '"
            << node->value << "'";
        exit(0);
    }

    // Лист дерева: идентификатор / константа / readline
    resultPlace = node->value;

    if (resultPlace == "readline") {
        string tempRead = generateTemp();
        tetrad.push_back(Tetrad("READ", "", "", tempRead));
        resultPlace = tempRead;
        return;
    }

    if (!resultPlace.empty() &&
        (isalpha(static_cast<unsigned char>(resultPlace[0])) || resultPlace[0] == '_')) {
        checkVariableInitialized(resultPlace);
    }
}

// Анализ бинарной операции
void SemanticAnalyzer::analyzeSeparator(TreeNode* node, string& resultPlace) {
    string leftResult, rightResult;

    analyzeExpression(node->children[0], leftResult);
    analyzeExpression(node->children[1], rightResult);

    resultPlace = generateTemp();
    tetrad.push_back(Tetrad(node->value, leftResult, rightResult, resultPlace));
}

// Анализ элемента
void SemanticAnalyzer::analyzeElement(TreeNode* node, string& resultPlace) {
    if (!node) return;

    // Унарный минус/плюс
    if ((node->value == "-" || node->value == "+") && node->children.size() == 1) {
        string childResult;
        analyzeExpression(node->children[0], childResult);

        if (node->value == "-") {
            resultPlace = generateTemp();
            tetrad.push_back(Tetrad("UMINUS", childResult, "", resultPlace));
        }
        else {
            resultPlace = childResult;
        }
    }
    else if (node->value == "not" && node->children.size() == 1) {
        string childResult;
        analyzeExpression(node->children[0], childResult);

        resultPlace = generateTemp();
        tetrad.push_back(Tetrad("NOT", childResult, "", resultPlace));
    }
}

// Анализ цикла
void SemanticAnalyzer::analyzeCycle(TreeNode* node) {
    // Создание новой области видимости для цикла
    Scope* loopScope = new Scope{ "while_loop", currentScope->level + 1, currentScope, {} };
    currentScope = loopScope;
    allScopes.push_back(loopScope);

    // Генерация меток
    string startLabel = generateLabel();
    string endLabel = generateLabel();

    // Метка начала цикла
    tetrad.push_back(Tetrad("LABEL", startLabel, "", ""));

    // Анализ условия цикла как обычного выражения
    string conditionTemp;
    analyzeExpression(node->children[0], conditionTemp);
    tetrad.push_back(Tetrad("JF", conditionTemp, "", endLabel));

    // Анализ тела цикла
    for (int i = 1; i < node->children.size(); i++) {
        TreeNode* child = node->children[i];
        if (child->value == "Body_Cycle") {
            continue;
        }
        else if (child->value != "") {
            analyzeNode(child);
        }
    }

    // Безусловный переход на начало цикла
    tetrad.push_back(Tetrad("JMP", "", "", startLabel));

    // Метка конца цикла
    tetrad.push_back(Tetrad("LABEL", endLabel, "", ""));

    // Восстанавливаем предыдущую область видимости
    currentScope = loopScope->parent;
}

// Анализ вывода
void SemanticAnalyzer::analyzeOutput(TreeNode* node) {
    if (node->children.empty()) {
        tetrad.push_back(Tetrad("WRITELN", "", "", ""));
        return;
    }

    for (auto child : node->children) {
        string resultPlace;
        analyzeExpression(child, resultPlace);

        if (!resultPlace.empty()) {
            tetrad.push_back(Tetrad("WRITE", resultPlace, "", ""));
        }
    }
}

// Анализ ввода
void SemanticAnalyzer::analyzeInput(TreeNode* node) {
    checkVariableDeclared(node->children[0]->value);

    tetrad.push_back(Tetrad("READ", "", "", node->children[0]->value));

    // Отметка переменной как инициализированной
    Scope* scope = currentScope;
    while (scope != nullptr) {
        auto it = scope->variables.find(node->children[0]->value);
        if (it != scope->variables.end()) {
            it->second.initialized = true;
            break;
        }
        scope = scope->parent;
    }
}

// Вывод тетрад
void SemanticAnalyzer::printTetrad() {
    ostream& out = getOutput();
    out << "\n\t\t\tТетрады" << endl;
    out << setw(7) << "Номер" << setw(16) << "Операция" << setw(13) << "Арг1"
        << setw(11) << "Арг2" << setw(18) << "Результат" << endl;

    for (int i = 0; i < tetrad.size(); i++) {
         Tetrad& q = tetrad[i];
        out << setw(5) << i << setw(15) << q.op << setw(15)
            << q.arg1 << setw(11) << q.arg2 << setw(15) << q.result << endl;
    }
}

// Вывод таблицы символов
void SemanticAnalyzer::printSymbolTable() {
    ostream& out = getOutput();
    out << "\n\t\t\t\t\tТаблица символов" << endl;
    out << setw(11) << "Область" << setw(12) << "Уровень" << setw(10) << "Имя"
        << setw(15) << "Тип" << setw(20) << "Объявлена"
        << setw(20) << "Инициализирована" << endl;

    for ( auto& scope : allScopes) {
        for ( auto& entry : scope->variables) {
             VarInfo& var = entry.second;
            out << setw(10) << scope->name << setw(10) << scope->level << setw(15)
                << var.name << setw(15) << var.type << setw(15)
                << (var.declared ? "Да" : "Нет") << setw(15)
                << (var.initialized ? "Да" : "Нет") << endl;
        }
    }
}
vector<Tetrad>& SemanticAnalyzer::getTetrads() {
    return tetrad;
}
