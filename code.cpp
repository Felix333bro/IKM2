#include <iostream>
#include "Header.h"


using namespace std;


// Установка значения переменной в словаре переменных
void Tree::setVariable(char name, int value) {
    variables[name] = value;
}

// Рекурсивное удаление дерева
void Tree::deleteTree(Node* node) {
    if (node) {
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
}

// Проверка корректности инфиксной записи выражения
void Tree::validateInfixExpression(const string& expr) {
    stack<char> brackets;         // Стек для проверки скобок
    int bracketDepth = 0;         // Глубина вложенности скобок
    bool expectOperand = true;    // Флаг ожидания операнда (true) или оператора (false)
    bool prevWasUnaryMinus = false; // Флаг унарного минуса

    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        if (isspace(c)) continue; // Пропускаем пробелы

        // Обработка открывающей скобки
        if (c == '(') {
            if (!expectOperand && c != '-') {
                throw runtime_error("Внезапная '('");
            }
            brackets.push(c);
            bracketDepth++;
            if (bracketDepth > 10) {
                throw runtime_error("Глубина вложения скобок превышает 10");
            }
            expectOperand = true;
            prevWasUnaryMinus = false;
        }
        // Обработка закрывающей скобки
        else if (c == ')') {
            if (brackets.empty() || brackets.top() != '(') {
                throw runtime_error("Внезапная )");
            }
            if (expectOperand && !prevWasUnaryMinus) {
                throw runtime_error("Унарный минус после числа");
            }
            brackets.pop();
            bracketDepth--;
            expectOperand = false;
            prevWasUnaryMinus = false;
        }
        // Обработка цифр (чисел)
        else if (isdigit(c)) {
            if (!expectOperand) {
                throw runtime_error("Неожиданное число");
            }
            // Пропускаем все цифры числа
            while (i < expr.size() && isdigit(expr[i])) i++;
            i--;
            expectOperand = false;
            prevWasUnaryMinus = false;
        }
        // Обработка переменных (букв)
        else if (isalpha(c)) {
            if (!expectOperand) {
                throw runtime_error("Неожиданная переменная");
            }
            expectOperand = false;
            prevWasUnaryMinus = false;
        }
        // Обработка унарного минуса
        else if (c == '-' && expectOperand) {
            // Проверка на множественный унарный минус
            if (prevWasUnaryMinus) {
                throw runtime_error("Множественный унарный минус без разделения скобкой");
            }
            // Подсчет количества минусов подряд
            size_t j = i + 1;
            int minusCount = 1;
            while (j < expr.size() && expr[j] == '-') {
                minusCount++;
                j++;
            }
            if (minusCount > 1) {
                throw runtime_error("Множественный унарный минус без разделения скобкой");
            }

            prevWasUnaryMinus = true;
        }
        // Обработка бинарных операторов
        else if (c == '+' || c == '-' || c == '*' || c == '/') {
            if (expectOperand) {
                if (c != '-') {
                    throw runtime_error("Неожиданный оператор");
                }
            }
            else {
                expectOperand = true;
                prevWasUnaryMinus = false;
            }
        }
        // Обработка недопустимых символов
        else {
            throw runtime_error("Недопустимый символ в выражении");
        }
    }

    // Проверка баланса скобок
    if (!brackets.empty()) {
        throw runtime_error("Не найдена )");
    }

    // Проверка завершенности выражения
    if (expectOperand) {
        throw runtime_error("Неполное выражение");
    }
}


// Преобразование дерева в строку (инфиксная нотация)
string Tree::treeToString(Node* node) {
    if (!node) return "";

    // Обработка числового узла
    if (node->type == NodeType::NUMBER) {
        return to_string(node->value);
    }
    // Обработка переменной
    if (node->type == NodeType::VARIABLE) {
        return string(1, node->op);
    }
    // Обработка унарного минуса
    if (node->type == NodeType::UNARY_MINUS) {
        string child = treeToString(node->left);
        if (node->left && (node->left->type == NodeType::OPERATOR ||
            node->left->type == NodeType::UNARY_MINUS)) {
            child = "(" + child + ")";
        }
        return "-" + child;
    }

    // Обработка бинарных операторов
    string left = treeToString(node->left);
    string right = treeToString(node->right);

    // Добавление скобок при необходимости для левого операнда
    if (node->left && node->left->type == NodeType::OPERATOR &&
        precedence(node->left->op) < precedence(node->op)) {
        left = "(" + left + ")";
    }

    // Добавление скобок при необходимости для правого операнда
    if (node->right && node->right->type == NodeType::OPERATOR &&
        precedence(node->right->op) <= precedence(node->op)) {
        right = "(" + right + ")";
    }

    return left + " " + node->op + " " + right;
}

// Печать дерева
void Tree::printTree(Node* node, string rpref, string cpref, string lpref) {
    string symvol1 = "\\-", symvol2 = "/-", symvol3 = "| ";
    if (!node) return;

    // Рекурсивный вывод правого поддерева
    if (node->right)
        printTree(node->right, rpref + "  ", rpref + symvol2, rpref + symvol3);

    // Вывод текущего узла
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

    // Рекурсивный вывод левого поддерева
    if (node->left)
        printTree(node->left, lpref + symvol3, lpref + symvol1, lpref + "  ");
}




Tree::Tree(const string& formula) {
    auto postfix = infixToPostfix(formula);
    root = buildTreeFromPostfix(postfix);
}


// Оператор присваивания
Tree& Tree::operator=(const Tree& other) {
    if (this != &other) {
        deleteTree(root);
        root = copyTree(other.root);
        variables = other.variables;
    }
    return *this;
}

// Деструктор
Tree::~Tree() {
    deleteTree(root);
}

// Вычисление значения формулы
int Tree::evaluate() {
return evaluateNode(root);

}

// Упрощение формулы
void Tree::simplify() {
    root = simplifyTree(root);
}

// Печать формулы
string Tree::toString() {
    return treeToString(root);
}

// Печать переменных
void Tree::printVariables() {
    if (!variables.empty()) {
        for (auto i : variables) {
            cout << i.first << "=" << i.second << "; ";
        }
        cout << endl;
    }
}

// Удаление дерева
void Tree::deleteTree() {
    root = nullptr;
    variables.clear();

}

// Печать дерева
void Tree::printTree() {
    printTree(root);
}

// Проверка ввода для пользовательского интерфейса
int checkInputInterface(string x) {
    if (x.size() == 1 && isdigit(x[0])) {
        return int(x[0] - 48);
    }
    return -1;
}

// Ввод int с проверкой
int inputNumberInt() {
    int num;
    while (true) {
        cin >> num;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Ошибка! Введите целое число.\n";
        }
        else {
            cin.ignore(10000, '\n');
            return num;
        }
    }
}

// Памятка по использованию
void Memo() {
    system("cls");
    cout << "Возможные ошибки при вводе инфиксной формы:\n1. Внезапная '(' или ')' - (2+3, 2+3), ((2+3)\n2. Не найдена ) - (1+2, 1+(2/3\n3. Глубина вложения скобок превышает 10 - (((((((((((1+1)+1)+1)+1)+1)+1)+1)+1)+1)+1)+1)\n";
    cout << "4. Неполное выражение - 1+1+\n5. Недопустимый символ в выражении - !34}324{, привет \n6. Неожиданное число - 1+3 3\n7. Неожиданная переменная - 1+fd\n";
    cout << "8. Неожиданный оператор - 1++2, 4//2\n9. Одно из чисел превышает значение в 2.147.483.647 - 3000000000+1\n10. Деление на 0 - 5/0\n11. Множественный унарный минус - 5+--2, --2\n12. Урарный минус после числа - (2+3-)";
    cout << "\nВозможные ошибки при выводе значения выражения:\n1. Переполнение int, результат неверный - возникает, когда в холе вычислений значения дерева\nвыражения получаются числа, не входящие в диапазон int (" << INT_MIN << "," << INT_MAX << ")\n";
}


// Проверка переполнения при сложении
int Tree::safeAdd(int a, int b) {
    if (b > 0 && a > INT_MAX - b) {
        throw runtime_error("Переполнение int, результат неверный ");
    }
    if (b < 0 && a < INT_MIN - b) {
        throw runtime_error("Переполнение int, результат неверный ");
    }
    return a + b;
}

// Проверка переполнения при умножении
int Tree::safeMul(int a, int b) {
    if (a > 0) {
        if (b > 0 && a > INT_MAX / b) {
            throw runtime_error("Переполнение int, результат неверный ");
        }
        if (b < 0 && b < INT_MIN / a) {
            throw runtime_error("Переполнение int, результат неверный ");
        }
    }
    else {
        if (b > 0 && a < INT_MIN / b) {
            throw runtime_error("Переполнение int, результат неверный ");
        }
        if (b < 0 && a < INT_MAX / b) {
            throw runtime_error("Переполнение int, результат неверный ");
        }
    }
    return a * b;
}

// Проверка переполнения при вычитании
int Tree::safeSub(int a, int b) {
    if (b < 0 && a > INT_MAX + b) {
        throw runtime_error("Переполнение int, результат неверный ");
    }
    if (b > 0 && a < INT_MIN + b) {
        throw runtime_error("Переполнение int, результат неверный ");
    }
    return a - b;
}