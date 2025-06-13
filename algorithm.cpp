#include <iostream>
#include "Header.h"

using namespace std;



// Преобразование инфиксной записи в постфиксную (обратную польскую)
// Принимает строку с инфиксным выражением, возвращает вектор токенов в постфиксной записи
vector<string> Tree::infixToPostfix(const string& expr) {
    validateInfixExpression(expr); // Проверяем корректность выражения
    vector<string> postfix; // Выходной вектор
    stack<char> ops; // Стек для операторов
    bool unary = true; // Флаг для определения унарного минуса (true если следующий оператор может быть унарным)

    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (isspace(c)) continue; // Пропускаем пробелы

        if (c == '(') {
            ops.push(c);
            unary = true; // После открывающей скобки может идти унарный минус
        }
        else if (c == ')') {
            // Выталкиваем все операторы до открывающей скобки
            while (!ops.empty() && ops.top() != '(') {
                postfix.push_back(string(1, ops.top()));
                ops.pop();
            }
            ops.pop(); // Удаляем саму открывающую скобку
            unary = false; // После закрывающей скобки не может быть унарного минуса
        }
        else if (isdigit(c)) {
            // Собираем все цифры числа
            string num;
            while (i < expr.size() && isdigit(expr[i])) {
                num += expr[i++];
            }
            i--;
            postfix.push_back(num);
            unary = false; // После числа не может быть унарного минуса
        }
        else if (isalpha(c)) {
            // Обработка переменных
            postfix.push_back(string(1, c));
            unary = false;
        }
        else if (c == '-' && unary) {
            // Обработка унарного минуса
            ops.push('u');
        }
        else {
            // Обработка бинарных операторов
            while (!ops.empty() && precedence(ops.top()) >= precedence(c)) {
                postfix.push_back(string(1, ops.top()));
                ops.pop();
            }
            ops.push(c);
            unary = true; // После оператора может идти унарный минус
        }
    }

    // Выталкиваем оставшиеся операторы из стека
    while (!ops.empty()) {
        postfix.push_back(string(1, ops.top()));
        ops.pop();
    }

    return postfix;
}

// Проверка приоритета операторов
// Возвращает числовой приоритет для оператора (чем выше число, тем выше приоритет)
int Tree::precedence(char op) {
    if (op == 'u') return 3; // Наивысший приоритет для унарного минуса
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0; // Для скобок и других символов
}

// Построение дерева выражения из постфиксной записи
// Принимает вектор токенов в постфиксной записи, возвращает корень дерева
Node* Tree::buildTreeFromPostfix(const vector<string>& postfix) {
    stack<Node*> st; // Стек для узлов дерева
    for (const auto& token : postfix) {
        if (isdigit(token[0])) {
            // Обработка чисел
            try {
                int num = stoi(token);
                st.push(new Node(NodeType::NUMBER, num, '\0'));
            }
            catch (out_of_range) {
                cout << "Ошибка: Одно из чисел превышает значение в 2.147.483.647\n";
                system("pause");
                Node* error = new Node(NodeType::NUMBER, 0, '0');
                return error;
            }
        }
        else if (isalpha(token[0]) && token != "u") {
            // Обработка переменных
            st.push(new Node(NodeType::VARIABLE, 0, token[0]));
        }
        else if (token == "u") {
            // Обработка унарного минуса
            Node* node = new Node(NodeType::UNARY_MINUS, 0, 'u');
            node->left = st.top(); st.pop();
            st.push(node);
        }
        else {
            // Обработка бинарных операторов
            Node* node = new Node(NodeType::OPERATOR, 0, token[0]);
            node->right = st.top(); st.pop();
            if (token[0] != 'u') { // Для бинарных операторов берем и левый операнд
                node->left = st.top(); st.pop();
            }
            st.push(node);
        }
    }

    return st.top(); // Корень дерева
}

// Применение оператора к операндам с проверкой переполнения
// Возвращает результат операции или INT_MAX при переполнении
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
        if (b == 0) throw runtime_error("Деление на 0");
        return a / b;
    case 'u': return -b; // Обработка унарного минуса
    }
}

// Рекурсивное вычисление значения поддерева
int Tree::evaluateNode(Node* node) {
    if (node->type == NodeType::NUMBER) {
        return node->value;
    }
    if (node->type == NodeType::UNARY_MINUS) {
        return -evaluateNode(node->left);
    }
    if (node->type == NodeType::VARIABLE) {
        return variables[node->op]; // Получаем значение переменной из словаря
    }
    // Вычисляем левое и правое поддеревья для бинарных операторов
    int leftVal = 0;
    if (node->left) {
        leftVal = evaluateNode(node->left);
    }
    int rightVal = evaluateNode(node->right);
    return applyOp(leftVal, rightVal, node->op);
}

// Рекурсивная проверка на равенство двух поддеревьев
bool Tree::isEqualTrees(Node* t1, Node* t2) {
    if (!t1 && !t2) return true; // Оба узла пустые
    if (!t1 || !t2) return false; // Один из узлов пустой

    if (t1->type != t2->type) return false; // Разные типы узлов

    // Сравнение значений в зависимости от типа узла
    if (t1->type == NodeType::NUMBER) {
        return t1->value == t2->value;
    }
    if (t1->type == NodeType::VARIABLE) {
        return t1->op == t2->op;
    }
    if (t1->type == NodeType::UNARY_MINUS) {
        return isEqualTrees(t1->left, t2->left);
    }
    if (t1->op != t2->op) return false; // Разные операторы

    // Рекурсивное сравнение левых и правых поддеревьев
    return isEqualTrees(t1->left, t2->left) && isEqualTrees(t1->right, t2->right);
}

// Проверка на возможность вынесения общего множителя
// Возвращает тип 1 или 2, или -1 если вынес невозможен невозможна
int Tree::isFactorizable(Node* node, char op,
    Node*& f1, Node*& f2, Node*& f3) {
    if (!node || node->type != NodeType::OPERATOR || (node->op != '+') && (node->op != '-')) {
        return -1;
    }

    Node* left = node->left;
    Node* right = node->right;

    if (left && left->type == NodeType::OPERATOR && left->op == '*' &&
        right && right->type == NodeType::OPERATOR && right->op == '*' && node->op == op) {

        // Случай 1: f1*f3 +- f2*f3
        if (isEqualTrees(left->right, right->right)) {
            f1 = left->left;
            f2 = right->left;
            f3 = left->right;
            return 1;
        }
        // Случай 2: f1*f2 +- f1*f3
        else if (isEqualTrees(left->left, right->left)) {
            f1 = left->left;
            f2 = right->right;
            f3 = left->right;
            return 2;
        }
    }
    return -1;
}

// Рекурсивное копирование дерева
Node* Tree::copyTree(Node* node) {
    if (!node) return nullptr;

    // Создаем новый узел с теми же параметрами
    Node* newNode = new Node(node->type, node->value, node->op);
    // Рекурсивно копируем левое и правое поддеревья
    newNode->left = copyTree(node->left);
    newNode->right = copyTree(node->right);
    return newNode;
}

// Рекурсивное упрощение дерева выражения
Node* Tree::simplifyTree(Node* node) {
    if (!node) return nullptr;

    // Сначала упрощаем поддеревья
    node->left = simplifyTree(node->left);
    node->right = simplifyTree(node->right);

    Node* f1 = nullptr;
    Node* f2 = nullptr;
    Node* f3 = nullptr;

    // Попытка упрощения для сложения или вычитания
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

    // Упрощение унарного минуса
    if (node->type == NodeType::UNARY_MINUS) {
        // Если под унарным минусом число, вычисляем его
        if (node->left && node->left->type == NodeType::NUMBER) {
            int val = -node->left->value;
            deleteTree(node);
            return new Node(NodeType::NUMBER, val, '\0');
        }
        // Если под унарным минусом другой унарный минус, убираем оба
        if (node->left && node->left->type == NodeType::UNARY_MINUS) {
            Node* result = node->left->left;
            node->left->left = nullptr; // Чтобы не удалилось при deleteTree
            deleteTree(node);
            return simplifyTree(result);
        }
    }

    return node; // Возвращаем неизмененный узел, если упрощение невозможно
}