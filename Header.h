#include <stack>
#include <map>
#include <string>
#include <vector>
#include <cctype>
#include <sstream>
#include <set>


using namespace std;

// ������������ ��� ����� �����
enum NodeType {
    NUMBER,
    VARIABLE,
    OPERATOR,
    UNARY_MINUS
};

// ��������� ���� ������
struct Node {
    NodeType type;
    int value; // ��� �����
    char op;   // ��� ���������� � ����������
    Node* left;
    Node* right;

    Node(NodeType t, int v, char o) : type(t), value(v), op(o), left(nullptr), right(nullptr) {}
};

class Tree {
private:
    Node* root;
    map<char, int> variables;

    // �������� ���������� ����������
    int precedence(char op);

    // ���������� ��������� � ���������
    int applyOp(int a, int b, char op);

    // �������� ������������ ��� ��������
    int safeAdd(int a, int b);

    // �������� ������������ ��� ���������
    int safeMul(int a, int b);

    // �������� ������������ ��� ���������
    int safeSub(int a, int b);

    // ����������� �������� ������
    void deleteTree(Node* node);

    // ����������� ������
    Node* copyTree(Node* node);

    // ���������� ������ �� ����������� ������
    Node* buildTreeFromPostfix(const vector<string>& postfix);

    // �������� ������������ ��������� ������
    void validateInfixExpression(const string& expr);

    // �������������� ��������� ������ � �����������
    vector<string> infixToPostfix(const string& expr);

    // ���������� �������� ���������
    int evaluateNode(Node* node);

    // �������� �� ������������ ������� (f1*f3 � f2*f3) ��� (f1*f2 � f1*f3)
    int isFactorizable(Node* node, char op,
        Node*& f1, Node*& f2, Node*& f3);

    // �������� �� ��������� ���� �����������
    bool isEqualTrees(Node* t1, Node* t2);

    // ��������� ������
    Node* simplifyTree(Node* node);

    // �������������� ������ � ������
    string treeToString(Node* node);

    // ������ ������ 
    void printTree(Node* node, string rpref = "", string cpref = "", string lpref = "");

public:
    Tree(const string& formula);

    // �������� ������������
    Tree& operator=(const Tree& other);

    // ����������
    ~Tree();

    // ��������� �������� ����������
    void setVariable(char name, int value);

    // ���������� �������� �������
    int evaluate();

    // ��������� �������
    void simplify();

    // ������ �������
    string toString();

    // ������ ������
    void printTree();

    // ������ ����������
    void printVariables();
    // �������� ������
    void deleteTree();
};

// �������� ����� ��� ����������������� ����������
int checkInputInterface(string x);

// ���� int � ���������
int inputNumberInt();

// ������� �� �������������
void Memo();