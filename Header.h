#include <stack>
#include <map>
#include <string>
#include <vector>
#include <cctype>
#include <sstream>
#include <set>


using namespace std;

// Перечисление для типов узлов
enum NodeType {
    NUMBER,
    VARIABLE,
    OPERATOR,
    UNARY_MINUS
};

// Структура узла дерева
struct Node {
    NodeType type;
    int value; // для чисел
    char op;   // для операторов и переменных
    Node* left;
    Node* right;

    Node(NodeType t, int v, char o) : type(t), value(v), op(o), left(nullptr), right(nullptr) {}
};

class Tree {
private:
    Node* root;
    map<char, int> variables;

    // Проверка приоритета операторов
    int precedence(char op);

    // Применение оператора к операндам
    int applyOp(int a, int b, char op);

    // Проверка переполнения при сложении
    int safeAdd(int a, int b);

    // Проверка переполнения при умножении
    int safeMul(int a, int b);

    // Проверка переполнения при вычитании
    int safeSub(int a, int b);

    // Рекурсивное удаление дерева
    void deleteTree(Node* node);

    // Копирование дерева
    Node* copyTree(Node* node);

    // Построение дерева из постфиксной записи
    Node* buildTreeFromPostfix(const vector<string>& postfix);

    // Проверка корректности инфиксной записи
    void validateInfixExpression(const string& expr);

    // Преобразование инфиксной записи в постфиксную
    vector<string> infixToPostfix(const string& expr);

    // Вычисление значения поддерева
    int evaluateNode(Node* node);

    // Проверка на соответствие шаблону (f1*f3 ± f2*f3) или (f1*f2 ± f1*f3)
    int isFactorizable(Node* node, char op,
        Node*& f1, Node*& f2, Node*& f3);

    // Проверка на равенство двух поддеревьев
    bool isEqualTrees(Node* t1, Node* t2);

    // Упрощение дерева
    Node* simplifyTree(Node* node);

    // Преобразование дерева в строку
    string treeToString(Node* node);

    // Печать дерева 
    void printTree(Node* node, string rpref = "", string cpref = "", string lpref = "");

public:
    Tree(const string& formula);

    // Оператор присваивания
    Tree& operator=(const Tree& other);

    // Деструктор
    ~Tree();

    // Установка значения переменной
    void setVariable(char name, int value);

    // Вычисление значения формулы
    int evaluate();

    // Упрощение формулы
    void simplify();

    // Печать формулы
    string toString();

    // Печать дерева
    void printTree();

    // Печать переменных
    void printVariables();
    // Удаление дерева
    void deleteTree();
};

// Проверка ввода для пользовательского интерфейса
int checkInputInterface(string x);

// Ввод int с проверкой
int inputNumberInt();

// Памятка по использованию
void Memo();