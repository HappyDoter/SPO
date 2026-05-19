#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

class TreeNode {
public:
    string value;
    vector<TreeNode*> children;

    TreeNode() {
        value = "";
        children = vector<TreeNode*>();
    }

    TreeNode(string val) {
        value = val;
    }

    // Метод для добавления нескольких детей сразу
    void addChildren(const vector<TreeNode*>& newChildren) {
        for (auto child : newChildren) {
            if (child != nullptr) {
                children.push_back(child);
            }
        }
    }

    // Метод для создания узла с готовыми детьми
    static TreeNode* createNode(string value, const vector<TreeNode*>& children = {}) {
        TreeNode* node = new TreeNode(value);
        node->addChildren(children);
        return node;
    }
};