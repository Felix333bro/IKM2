#include <iostream>
#include "Header.h"

using namespace std;



// �������������� ��������� ������ � ����������� (�������� ��������)
// ��������� ������ � ��������� ����������, ���������� ������ ������� � ����������� ������
vector<string> Tree::infixToPostfix(const string& expr) {
    validateInfixExpression(expr); // ��������� ������������ ���������
    vector<string> postfix; // �������� ������
    stack<char> ops; // ���� ��� ����������
    bool unary = true; // ���� ��� ����������� �������� ������ (true ���� ��������� �������� ����� ���� �������)

    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (isspace(c)) continue; // ���������� �������

        if (c == '(') {
            ops.push(c);
            unary = true; // ����� ����������� ������ ����� ���� ������� �����
        }
        else if (c == ')') {
            // ����������� ��� ��������� �� ����������� ������
            while (!ops.empty() && ops.top() != '(') {
                postfix.push_back(string(1, ops.top()));
                ops.pop();
            }
            ops.pop(); // ������� ���� ����������� ������
            unary = false; // ����� ����������� ������ �� ����� ���� �������� ������
        }
        else if (isdigit(c)) {
            // �������� ��� ����� �����
            string num;
            while (i < expr.size() && isdigit(expr[i])) {
                num += expr[i++];
            }
            i--;
            postfix.push_back(num);
            unary = false; // ����� ����� �� ����� ���� �������� ������
        }
        else if (isalpha(c)) {
            // ��������� ����������
            postfix.push_back(string(1, c));
            unary = false;
        }
        else if (c == '-' && unary) {
            // ��������� �������� ������
            ops.push('u');
        }
        else {
            // ��������� �������� ����������
            while (!ops.empty() && precedence(ops.top()) >= precedence(c)) {
                postfix.push_back(string(1, ops.top()));
                ops.pop();
            }
            ops.push(c);
            unary = true; // ����� ��������� ����� ���� ������� �����
        }
    }

    // ����������� ���������� ��������� �� �����
    while (!ops.empty()) {
        postfix.push_back(string(1, ops.top()));
        ops.pop();
    }

    return postfix;
}

// �������� ���������� ����������
// ���������� �������� ��������� ��� ��������� (��� ���� �����, ��� ���� ���������)
int Tree::precedence(char op) {
    if (op == 'u') return 3; // ��������� ��������� ��� �������� ������
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0; // ��� ������ � ������ ��������
}

// ���������� ������ ��������� �� ����������� ������
// ��������� ������ ������� � ����������� ������, ���������� ������ ������
Node* Tree::buildTreeFromPostfix(const vector<string>& postfix) {
    stack<Node*> st; // ���� ��� ����� ������
    for (const auto& token : postfix) {
        if (isdigit(token[0])) {
            // ��������� �����
            try {
                int num = stoi(token);
                st.push(new Node(NodeType::NUMBER, num, '\0'));
            }
            catch (out_of_range) {
                cout << "������: ���� �� ����� ��������� �������� � 2.147.483.647\n";
                system("pause");
                Node* error = new Node(NodeType::NUMBER, 0, '0');
                return error;
            }
        }
        else if (isalpha(token[0]) && token != "u") {
            // ��������� ����������
            st.push(new Node(NodeType::VARIABLE, 0, token[0]));
        }
        else if (token == "u") {
            // ��������� �������� ������
            Node* node = new Node(NodeType::UNARY_MINUS, 0, 'u');
            node->left = st.top(); st.pop();
            st.push(node);
        }
        else {
            // ��������� �������� ����������
            Node* node = new Node(NodeType::OPERATOR, 0, token[0]);
            node->right = st.top(); st.pop();
            if (token[0] != 'u') { // ��� �������� ���������� ����� � ����� �������
                node->left = st.top(); st.pop();
            }
            st.push(node);
        }
    }

    return st.top(); // ������ ������
}

// ���������� ��������� � ��������� � ��������� ������������
// ���������� ��������� �������� ��� INT_MAX ��� ������������
int Tree::applyOp(int a, int b, char op) {
    switch (op) {
    case '+': {
        try {
            int check = safeAdd(a, b);
            return a + b;
        }
        catch (const exception& e) {
            cout << e.what();
            return INT_MAX;
        }
    }
    case '-': {
        try {
            int check = safeSub(a, b);
            return a - b;
        }
        catch (const exception& e) {
            cout << e.what();
            return INT_MAX;
        }
    }
    case '*':
        try {
            int check = safeMul(a, b);
            return a * b;
        }
        catch (const exception& e) {
            cout << e.what();
            return INT_MAX;
        }
    case '/':
        if (b == 0) throw runtime_error("������� �� 0");
        return a / b;
    case 'u': return -b; // ��������� �������� ������
    }
}

// ����������� ���������� �������� ���������
int Tree::evaluateNode(Node* node) {
    if (node->type == NodeType::NUMBER) {
        return node->value;
    }
    if (node->type == NodeType::UNARY_MINUS) {
        return -evaluateNode(node->left);
    }
    if (node->type == NodeType::VARIABLE) {
        return variables[node->op]; // �������� �������� ���������� �� �������
    }
    // ��������� ����� � ������ ���������� ��� �������� ����������
    int leftVal = 0;
    if (node->left) {
        leftVal = evaluateNode(node->left);
    }
    int rightVal = evaluateNode(node->right);
    return applyOp(leftVal, rightVal, node->op);
}

// ����������� �������� �� ��������� ���� �����������
bool Tree::isEqualTrees(Node* t1, Node* t2) {
    if (!t1 && !t2) return true; // ��� ���� ������
    if (!t1 || !t2) return false; // ���� �� ����� ������

    if (t1->type != t2->type) return false; // ������ ���� �����

    // ��������� �������� � ����������� �� ���� ����
    if (t1->type == NodeType::NUMBER) {
        return t1->value == t2->value;
    }
    if (t1->type == NodeType::VARIABLE) {
        return t1->op == t2->op;
    }
    if (t1->type == NodeType::UNARY_MINUS) {
        return isEqualTrees(t1->left, t2->left);
    }
    if (t1->op != t2->op) return false; // ������ ���������

    // ����������� ��������� ����� � ������ �����������
    return isEqualTrees(t1->left, t2->left) && isEqualTrees(t1->right, t2->right);
}

// �������� �� ����������� ��������� ������ ���������
// ���������� ��� 1 ��� 2, ��� -1 ���� ����� ���������� ����������
int Tree::isFactorizable(Node* node, char op,
    Node*& f1, Node*& f2, Node*& f3) {
    if (!node || node->type != NodeType::OPERATOR || (node->op != '+') && (node->op != '-')) {
        return -1;
    }

    Node* left = node->left;
    Node* right = node->right;

    if (left && left->type == NodeType::OPERATOR && left->op == '*' &&
        right && right->type == NodeType::OPERATOR && right->op == '*' && node->op == op) {

        // ������ 1: f1*f3 +- f2*f3
        if (isEqualTrees(left->right, right->right)) {
            f1 = left->left;
            f2 = right->left;
            f3 = left->right;
            return 1;
        }
        // ������ 2: f1*f2 +- f1*f3
        else if (isEqualTrees(left->left, right->left)) {
            f1 = left->left;
            f2 = right->right;
            f3 = left->right;
            return 2;
        }
    }
    return -1;
}

// ����������� ����������� ������
Node* Tree::copyTree(Node* node) {
    if (!node) return nullptr;

    // ������� ����� ���� � ���� �� �����������
    Node* newNode = new Node(node->type, node->value, node->op);
    // ���������� �������� ����� � ������ ����������
    newNode->left = copyTree(node->left);
    newNode->right = copyTree(node->right);
    return newNode;
}

// ����������� ��������� ������ ���������
Node* Tree::simplifyTree(Node* node) {
    if (!node) return nullptr;

    // ������� �������� ����������
    node->left = simplifyTree(node->left);
    node->right = simplifyTree(node->right);

    Node* f1 = nullptr;
    Node* f2 = nullptr;
    Node* f3 = nullptr;

    // ������� ��������� ��� �������� ��� ���������
    if (isFactorizable(node, '+', f1, f2, f3) == 2 || isFactorizable(node, '-', f1, f2, f3) == 2) {
        Node* newOp = new Node(NodeType::OPERATOR, 0, node->op);
        newOp->left = copyTree(f3);
        newOp->right = copyTree(f2);

        Node* newNode = new Node(NodeType::OPERATOR, 0, '*');
        newNode->left = copyTree(f1);
        newNode->right = newOp;

        deleteTree(node);
        return newNode;
    }
    if (isFactorizable(node, '+', f1, f2, f3) == 1 || isFactorizable(node, '-', f1, f2, f3) == 1) {
        Node* newOp = new Node(NodeType::OPERATOR, 0, node->op);
        newOp->left = copyTree(f1);
        newOp->right = copyTree(f2);

        Node* newNode = new Node(NodeType::OPERATOR, 0, '*');
        newNode->left = newOp;
        newNode->right = copyTree(f3);

        deleteTree(node);
        return newNode;
    }

    // ��������� �������� ������
    if (node->type == NodeType::UNARY_MINUS) {
        // ���� ��� ������� ������� �����, ��������� ���
        if (node->left && node->left->type == NodeType::NUMBER) {
            int val = -node->left->value;
            deleteTree(node);
            return new Node(NodeType::NUMBER, val, '\0');
        }
        // ���� ��� ������� ������� ������ ������� �����, ������� ���
        if (node->left && node->left->type == NodeType::UNARY_MINUS) {
            Node* result = node->left->left;
            node->left->left = nullptr; // ����� �� ��������� ��� deleteTree
            deleteTree(node);
            return simplifyTree(result);
        }
    }

    return node; // ���������� ������������ ����, ���� ��������� ����������
}