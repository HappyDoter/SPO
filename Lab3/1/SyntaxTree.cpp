#include "SyntaxTree.h"
#include <iostream>
#include <iomanip>

void SyntaxTree::printTree(const std::shared_ptr<SyntaxNode>& node, int depth, bool isLast) {
    // Print indentation
    for (int i = 0; i < depth - 1; ++i) {
        std::cout << "    ";
    }

    if (depth > 0) {
        std::cout << (isLast ? "└── " : "├── ");
    }

    // Print node type and value
    std::string typeStr;
    switch (node->type) {
    case NodeType::PROGRAM: typeStr = "PROGRAM"; break;
    case NodeType::ANNOUNCEMENT: typeStr = "ANNOUNCEMENT"; break;
    case NodeType::ASSIGNMENT: typeStr = "ASSIGNMENT"; break;
    case NodeType::CYCLE: typeStr = "CYCLE"; break;
    case NodeType::INPUT: typeStr = "INPUT"; break;
    case NodeType::OUTPUT: typeStr = "OUTPUT"; break;
    case NodeType::EXPRESSION: typeStr = "EXPRESSION"; break;
    case NodeType::IDENTIFIER: typeStr = "IDENTIFIER"; break;
    case NodeType::CONSTANT: typeStr = "CONSTANT"; break;
    case NodeType::OPERATOR: typeStr = "OPERATOR"; break;
    case NodeType::KEYWORD: typeStr = "KEYWORD"; break;
    case NodeType::BINARY_OPERATION: typeStr = "BINARY_OP"; break;
    case NodeType::UNARY_OPERATION: typeStr = "UNARY_OP"; break;
    }

    std::cout << typeStr;
    if (!node->value.empty()) {
        std::cout << " [" << node->value << "]";
    }
    std::cout << std::endl;

    // Print children
    for (size_t i = 0; i < node->children.size(); ++i) {
        printTree(node->children[i], depth + 1, i == node->children.size() - 1);
    }
}

void SyntaxTree::print() {
    std::cout << "\n=== СИНТАКСИЧЕСКОЕ ДЕРЕВО ===" << std::endl;
    printTree(root);
    std::cout << "============================\n" << std::endl;
}