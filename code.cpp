#include <iostream>
#include "Header.h"


using namespace std;


// ��������� �������� ���������� � ������� ����������
void Tree::setVariable(char name, int value) {
    variables[name] = value;
}

// ����������� �������� ������
void Tree::deleteTree(Node* node) {
    if (node) {
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
}

// �������� ������������ ��������� ������ ���������
void Tree::validateInfixExpression(const string& expr) {
    stack<char> brackets;         // ���� ��� �������� ������
    int bracketDepth = 0;         // ������� ����������� ������
    bool expectOperand = true;    // ���� �������� �������� (true) ��� ��������� (false)
    bool prevWasUnaryMinus = false; // ���� �������� ������

    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (isspace(c)) continue; // ���������� �������

        // ��������� ����������� ������
        if (c == '(') {
            if (!expectOperand && c != '-') {
                throw runtime_error("��������� '('");
            }
            brackets.push(c);
            bracketDepth++;
            if (bracketDepth > 10) {
                throw runtime_error("������� �������� ������ ��������� 10");
            }
            expectOperand = true;
            prevWasUnaryMinus = false;
        }
        // ��������� ����������� ������
        else if (c == ')') {
            if (brackets.empty() || brackets.top() != '(') {
                throw runtime_error("��������� )");
            }
            if (expectOperand && !prevWasUnaryMinus) {
                throw runtime_error("������� ����� ����� �����");
            }
            brackets.pop();
            bracketDepth--;
            expectOperand = false;
            prevWasUnaryMinus = false;
        }
        // ��������� ���� (�����)
        else if (isdigit(c)) {
            if (!expectOperand) {
                throw runtime_error("����������� �����");
            }
            // ���������� ��� ����� �����
            while (i < expr.size() && isdigit(expr[i])) i++;
            i--;
            expectOperand = false;
            prevWasUnaryMinus = false;
        }
        // ��������� ���������� (����)
        else if (isalpha(c)) {
            if (!expectOperand) {
                throw runtime_error("����������� ����������");
            }
            expectOperand = false;
            prevWasUnaryMinus = false;
        }
        // ��������� �������� ������
        else if (c == '-' && expectOperand) {
            // �������� �� ������������� ������� �����
            if (prevWasUnaryMinus) {
                throw runtime_error("������������� ������� ����� ��� ���������� �������");
            }
            // ������� ���������� ������� ������
            size_t j = i + 1;
            int minusCount = 1;
            while (j < expr.size() && expr[j] == '-') {
                minusCount++;
                j++;
            }
            if (minusCount > 1) {
                throw runtime_error("������������� ������� ����� ��� ���������� �������");
            }

            prevWasUnaryMinus = true;
        }
        // ��������� �������� ����������
        else if (c == '+' || c == '-' || c == '*' || c == '/') {
            if (expectOperand) {
                if (c != '-') {
                    throw runtime_error("����������� ��������");
                }
            }
            else {
                expectOperand = true;
                prevWasUnaryMinus = false;
            }
        }
        // ��������� ������������ ��������
        else {
            throw runtime_error("������������ ������ � ���������");
        }
    }

    // �������� ������� ������
    if (!brackets.empty()) {
        throw runtime_error("�� ������� )");
    }

    // �������� ������������� ���������
    if (expectOperand) {
        throw runtime_error("�������� ���������");
    }
}


// �������������� ������ � ������ (��������� �������)
string Tree::treeToString(Node* node) {
    if (!node) return "";

    // ��������� ��������� ����
    if (node->type == NodeType::NUMBER) {
        return to_string(node->value);
    }
    // ��������� ����������
    if (node->type == NodeType::VARIABLE) {
        return string(1, node->op);
    }
    // ��������� �������� ������
    if (node->type == NodeType::UNARY_MINUS) {
        string child = treeToString(node->left);
        if (node->left && (node->left->type == NodeType::OPERATOR ||
            node->left->type == NodeType::UNARY_MINUS)) {
            child = "(" + child + ")";
        }
        return "-" + child;
    }

    // ��������� �������� ����������
    string left = treeToString(node->left);
    string right = treeToString(node->right);

    // ���������� ������ ��� ������������� ��� ������ ��������
    if (node->left && node->left->type == NodeType::OPERATOR &&
        precedence(node->left->op) < precedence(node->op)) {
        left = "(" + left + ")";
    }

    // ���������� ������ ��� ������������� ��� ������� ��������
    if (node->right && node->right->type == NodeType::OPERATOR &&
        precedence(node->right->op) <= precedence(node->op)) {
        right = "(" + right + ")";
    }

    return left + " " + node->op + " " + right;
}

// ������ ������
void Tree::printTree(Node* node, string rpref, string cpref, string lpref) {
    string symvol1 = "\\-", symvol2 = "/-", symvol3 = "| ";
    if (!node) return;

    // ����������� ����� ������� ���������
    if (node->right)
        printTree(node->right, rpref + "  ", rpref + symvol2, rpref + symvol3);

    // ����� �������� ����
    if (node->type == NodeType::OPERATOR) {
        cout << cpref << "(" << node->op << ")" << endl;
    }
    else if (node->type == NodeType::VARIABLE) {
        cout << cpref << "(" << node->op << ")" << endl;
    }
    else if (node->type == NodeType::UNARY_MINUS) {
        cout << cpref << "(u)" << endl;
    }
    else {
        cout << cpref << "(" << to_string(node->value) << ")" << endl;
    }

    // ����������� ����� ������ ���������
    if (node->left)
        printTree(node->left, lpref + symvol3, lpref + symvol1, lpref + "  ");
}




Tree::Tree(const string& formula) {
    auto postfix = infixToPostfix(formula);
    root = buildTreeFromPostfix(postfix);
}


// �������� ������������
Tree& Tree::operator=(const Tree& other) {
    if (this != &other) {
        deleteTree(root);
        root = copyTree(other.root);
        variables = other.variables;
    }
    return *this;
}

// ����������
Tree::~Tree() {
    deleteTree(root);
}

// ���������� �������� �������
int Tree::evaluate() {
return evaluateNode(root);

}

// ��������� �������
void Tree::simplify() {
    root = simplifyTree(root);
}

// ������ �������
string Tree::toString() {
    return treeToString(root);
}

// ������ ����������
void Tree::printVariables() {
    if (!variables.empty()) {
        for (auto i : variables) {
            cout << i.first << "=" << i.second << "; ";
        }
        cout << endl;
    }
}

// �������� ������
void Tree::deleteTree() {
    root = nullptr;
    variables.clear();

}

// ������ ������
void Tree::printTree() {
    printTree(root);
}

// �������� ����� ��� ����������������� ����������
int checkInputInterface(string x) {
    if (x.size() == 1 && isdigit(x[0])) {
        return int(x[0] - 48);
    }
    return -1;
}

// ���� int � ���������
int inputNumberInt() {
    int num;
    while (true) {
        cin >> num;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "������! ������� ����� �����.\n";
        }
        else {
            cin.ignore(10000, '\n');
            return num;
        }
    }
}

// ������� �� �������������
void Memo() {
    system("cls");
    cout << "��������� ������ ��� ����� ��������� �����:\n1. ��������� '(' ��� ')' - (2+3, 2+3), ((2+3)\n2. �� ������� ) - (1+2, 1+(2/3\n3. ������� �������� ������ ��������� 10 - (((((((((((1+1)+1)+1)+1)+1)+1)+1)+1)+1)+1)+1)\n";
    cout << "4. �������� ��������� - 1+1+\n5. ������������ ������ � ��������� - !34}324{, ������ \n6. ����������� ����� - 1+3 3\n7. ����������� ���������� - 1+fd\n";
    cout << "8. ����������� �������� - 1++2, 4//2\n9. ���� �� ����� ��������� �������� � 2.147.483.647 - 3000000000+1\n10. ������� �� 0 - 5/0\n11. ������������� ������� ����� - 5+--2, --2\n12. ������� ����� ����� ����� - (2+3-)";
    cout << "\n��������� ������ ��� ������ �������� ���������:\n1. ������������ int, ��������� �������� - ���������, ����� � ���� ���������� �������� ������\n��������� ���������� �����, �� �������� � �������� int (" << INT_MIN << "," << INT_MAX << ")\n";
}


// �������� ������������ ��� ��������
int Tree::safeAdd(int a, int b) {
    if (b > 0 && a > INT_MAX - b) {
        throw runtime_error("������������ int, ��������� �������� ");
    }
    if (b < 0 && a < INT_MIN - b) {
        throw runtime_error("������������ int, ��������� �������� ");
    }
    return a + b;
}

// �������� ������������ ��� ���������
int Tree::safeMul(int a, int b) {
    if (a > 0) {
        if (b > 0 && a > INT_MAX / b) {
            throw runtime_error("������������ int, ��������� �������� ");
        }
        if (b < 0 && b < INT_MIN / a) {
            throw runtime_error("������������ int, ��������� �������� ");
        }
    }
    else {
        if (b > 0 && a < INT_MIN / b) {
            throw runtime_error("������������ int, ��������� �������� ");
        }
        if (b < 0 && a < INT_MAX / b) {
            throw runtime_error("������������ int, ��������� �������� ");
        }
    }
    return a * b;
}

// �������� ������������ ��� ���������
int Tree::safeSub(int a, int b) {
    if (b < 0 && a > INT_MAX + b) {
        throw runtime_error("������������ int, ��������� �������� ");
    }
    if (b > 0 && a < INT_MIN + b) {
        throw runtime_error("������������ int, ��������� �������� ");
    }
    return a - b;
}