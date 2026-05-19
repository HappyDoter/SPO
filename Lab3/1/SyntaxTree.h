#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H

#include <string>
#include <vector>
#include <memory>

enum class NodeType {
    PROGRAM,
    ANNOUNCEMENT,
    ASSIGNMENT,
    CYCLE,
    INPUT,
    OUTPUT,
    EXPRESSION,
    IDENTIFIER,
    CONSTANT,
    OPERATOR,
    KEYWORD,
    BINARY_OPERATION,
    UNARY_OPERATION
};

struct SyntaxNode {
    NodeType type;
    std::string value;
    std::vector<std::shared_ptr<SyntaxNode>> children;

    SyntaxNode(NodeType t, const std::string& v = "") : type(t), value(v) {}

    void addChild(std::shared_ptr<SyntaxNode> child) {
        children.push_back(child);
    }
};

class SyntaxTree {
private:
    std::shared_ptr<SyntaxNode> root;

    void printTree(const std::shared_ptr<SyntaxNode>& node, int depth = 0, bool isLast = true);

public:
    SyntaxTree() : root(std::make_shared<SyntaxNode>(NodeType::PROGRAM, "Program")) {}

    std::shared_ptr<SyntaxNode> getRoot() { return root; }

    void addToRoot(std::shared_ptr<SyntaxNode> node) {
        root->addChild(node);
    }

    void print();
};

#endif