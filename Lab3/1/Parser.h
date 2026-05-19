#include <unordered_map>
#include <iostream>
#include <fstream>
#include "TreeNode.h"
using namespace std;
class Lexer;

class Parser {
private:
	Lexer* lexer;
	TreeNode* syntaxTree;
	ofstream outputFile;
	pair<int, int> currentElement;


	TreeNode* announcement();
	TreeNode* assignment();
	TreeNode* cycle();
	TreeNode* input();
	TreeNode* output();
	TreeNode* expression();
	TreeNode* element();

	TreeNode* Conditions();
	TreeNode* OrXor();
	TreeNode* And();
	TreeNode* Ampersand();
	TreeNode* PlusMinus();
	TreeNode* Mod();
	TreeNode* Remainder();
	TreeNode* MultiplDiv();
	TreeNode* Degree();



	pair<int, int> getNextElement();
	pair<int, int> getPrevElement();
	string getElement();
	string getElementPos(int);
	bool checkElement(int, int);
	ostream& getOutput();
	TreeNode* start(int);
public:
	Parser(Lexer&);
	TreeNode* getSyntaxTree() {
		return syntaxTree;
	}
	void printTree(TreeNode* node, string prefix = "", bool isLast = true);
};