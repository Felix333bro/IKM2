#include <iostream>
#include "Header.h"

using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");
    bool status = true;
    Tree tree("0");
    set<char> unique_vars;
    while (status) {
        system("cls");
        tree.printTree();
        cout << "\nИнфиксная запись выражения - "<< tree.toString();
        cout << "\nЗначения переменных: ";
        tree.printVariables();
        cout << "\nЗначение выражения при указанных значений переменных - ";
        int resultExpression = tree.evaluate();
        cout <<" "<< resultExpression << endl;;

        
        cout << "\n1 - Заполнить дерево выражений\n2 - Изменить значения переменных\n3 - Упростить дерево выражений\n4 - Очистить дерево выражений\n5 - Памятка по использованию\n0 - Выход\n";
        string selest;
        getline(cin, selest);
        int selestInt = checkInputInterface(selest);
        switch (selestInt) {

        case(1): {
            try {
                system("cls");
                cout << "Введите инфиксную запись выражения - ";
                string infix;
                getline(cin, infix);
                Tree copy(infix);
                tree = copy;
                unique_vars.clear();
                for (char c : infix) {
                    if (isalpha(c)) {
                        unique_vars.insert(c);
                    }
                }
                system("cls");
                for (char var : unique_vars) {
                    cout << "Введите значение для переменной " << var << ": ";
                    int value = inputNumberInt();
                    system("cls");
                    tree.setVariable(var, value);
                }
            }
            catch (const exception& e) {
                cout << "Ошибка: " << e.what() << endl;
                system("pause");
            }
            break;
        }

        case(2): {
            system("cls");
            for (char var : unique_vars) {
                cout << "Введите значение для переменной " << var << ": ";
                int value = inputNumberInt();
                tree.setVariable(var, value);
                system("cls");
            }
            break;
        }

        case(3):
            tree.simplify();
            break;

        case(4): {
            Tree copy("0");
            tree = copy;
            break;
        }

        case(5):
            system("cls");
            Memo();
            system("pause");
            break;

        case(0):
            status = false;
            break;

        default:
            break;
        }
    }
    return 0;
}