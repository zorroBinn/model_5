#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <string>
#include <cstring>
#include <iomanip>
#include <Windows.h>
using namespace std;

struct Node {
    vector<pair<int, int>> path;  //Путь до текущей вершины
    vector<vector<int>> reducedMatrix;  //Приведённая матрица затрат
    int cost;  //Стоимость узла
    int vertex;  //Текущая вершина
    int level;  //Глубина уровня дерева
    vector<Node*> children;  //Дочерние узлы
    pair<int, int> edge;  //Выбранное ребро (i, j)
    bool selected;  //Указывает, выбрано ребро или нет
};

vector<double> theta;  //Хранит оценки стоимости узлов дерева

//Создание нового узла дерева ветвей и границ
Node* newNode(const vector<vector<int>>& parentMatrix, const vector<pair<int, int>>& path, int level, int i, int j, bool selected = true) {
    Node* node = new Node;
    node->path = path;

    if (level != 0) {
        node->path.emplace_back(i, j);
    }

    node->reducedMatrix = parentMatrix;

    //Удаляем строки и столбцы
    for (int k = 0; level != 0 && k < node->reducedMatrix.size(); k++) {
        node->reducedMatrix[i][k] = INT_MAX;
        node->reducedMatrix[k][j] = INT_MAX;
    }

    //Удаляем начальную точку
    node->reducedMatrix[j][0] = INT_MAX;
    node->level = level;
    node->vertex = j;
    node->edge = { i, j };
    node->selected = selected;

    return node;
}

//Редукция строк
int rowReduction(vector<vector<int>>& reducedMatrix, vector<int>& row) {
    fill(row.begin(), row.end(), INT_MAX);

    for (int i = 0; i < reducedMatrix.size(); i++) {
        for (int j = 0; j < reducedMatrix[i].size(); j++) {
            if (reducedMatrix[i][j] < row[i]) {
                row[i] = reducedMatrix[i][j];
            }
        }
    }

    for (int i = 0; i < reducedMatrix.size(); i++) {
        for (int j = 0; j < reducedMatrix[i].size(); j++) {
            if (reducedMatrix[i][j] != INT_MAX && row[i] != INT_MAX) {
                reducedMatrix[i][j] -= row[i];
            }
        }
    }
    return 0;
}

//Редукция столбцов
int columnReduction(vector<vector<int>>& reducedMatrix, vector<int>& col) {
    fill(col.begin(), col.end(), INT_MAX);

    for (int i = 0; i < reducedMatrix.size(); i++) {
        for (int j = 0; j < reducedMatrix[i].size(); j++) {
            if (reducedMatrix[i][j] < col[j]) {
                col[j] = reducedMatrix[i][j];
            }
        }
    }

    for (int i = 0; i < reducedMatrix.size(); i++) {
        for (int j = 0; j < reducedMatrix[i].size(); j++) {
            if (reducedMatrix[i][j] != INT_MAX && col[j] != INT_MAX) {
                reducedMatrix[i][j] -= col[j];
            }
        }
    }
    return 0;
}

//Вычисление стоимости узла после редукции
int calculateCost(vector<vector<int>>& reducedMatrix) {
    int cost = 0;
    vector<int> row(reducedMatrix.size());
    rowReduction(reducedMatrix, row);

    vector<int> col(reducedMatrix[0].size());
    columnReduction(reducedMatrix, col);

    for (int i = 0; i < reducedMatrix.size(); i++) {
        cost += (row[i] != INT_MAX) ? row[i] : 0;
        cost += (col[i] != INT_MAX) ? col[i] : 0;
    }
    return cost;
}

//Вывод решения задачи коммивояжера
void printSolution(Node* node, const vector<vector<int>>& costMatrix) {
    int totalCost = 0;
    for (const auto& step : node->path) {
        totalCost += costMatrix[step.first][step.second];
    }
    cout << endl << "Оптимальный маршрут: ";
    for (size_t i = 0; i < node->path.size(); i++) {
        cout << node->path[i].first + 1;
        if (i < node->path.size() - 1) cout << "-";
    }
    cout << "-1" << endl;
    cout << "Общая стоимость: " << totalCost << endl;
}

//Функция для сравнения узлов в очереди приоритетов
struct CompareNodes {
    bool operator()(Node* lhs, Node* rhs) const {
        return lhs->cost > rhs->cost; //Узел с меньшей стоимостью имеет более высокий приоритет
    }
};

//Вывод решения задачи коммивояжера с детализированным выводом шагов
void TSPPathPrint(Node* list, const vector<vector<int>>& CostGraphMatrix) {
    vector<bool> removedRows(CostGraphMatrix.size(), false);
    vector<bool> removedCols(CostGraphMatrix.size(), false);
    int stepMatrix = 0;
    int cost = 0;

    for (const auto& step : list->path) {
        int row = step.first;
        int col = step.second;
        stepMatrix++;

        cout << "Шаг: " << stepMatrix << endl;
        for (int k = 0; k < CostGraphMatrix.size(); k++) {
            if (!removedCols[k]) {
                cout << "\t" << k + 1;
            }
        }
        cout << endl << "\t";
        for (int i = 0; i < CostGraphMatrix.size(); i++) {
            if (!removedCols[i]) {
                cout << "-------";
            }
        }
        cout << endl;

        for (int i = 0; i < CostGraphMatrix.size(); i++) {
            if (!removedRows[i]) {
                cout << " " << i + 1 << "|";
                for (int j = 0; j < CostGraphMatrix.size(); j++) {
                    if (!removedCols[j]) {
                        if (CostGraphMatrix[i][j] == INT_MAX) {
                            cout << "\t"
                                << "INF ";
                        }
                        else {
                            cout << "\t" << CostGraphMatrix[i][j] << " ";
                        }
                    }
                }
                cout << endl;
            }
        }

        cost += CostGraphMatrix[row][col];
        cout << "Сумма приводящих констант (rhk): " << cost << endl;
        cout << "Оценка затрат: " << CostGraphMatrix[row][col] << endl;
        cout << "Ребро маршрута (h, k): (" << row + 1 << ", " << col + 1 << ")" << endl;

        int penalty = 0;
        for (int i = 0; i < CostGraphMatrix.size(); i++) {
            if (!removedRows[i]) {
                penalty += CostGraphMatrix[row][i];
            }
            if (!removedCols[i]) {
                penalty += CostGraphMatrix[i][col];
            }
        }

        cout << "Штраф за неиспользование (phk): " << penalty << endl << "" << endl;

        removedRows[row] = true;
        removedCols[col] = true;
    }

    cout << "Маршрут:" << endl;
    for (const auto& step : list->path) {
        cout << step.first + 1 << " -> " << step.second + 1 << endl;
    }
}

//Добавлена функция вывода дерева узлов
void printTree(Node* node, const string& prefix = "", bool isLast = true) {
    if (!node) return;

    //Формируем информацию о текущем узле
    string edgeInfo;
    if (node->level == 0) {
        edgeInfo = "S(0)"; //Корневой узел
    }
    else if (node->selected) {
        edgeInfo = "(" + to_string(node->edge.first + 1) + ", " + to_string(node->edge.second + 1) + ")";
    }
    else {
        edgeInfo = "!" + to_string(node->edge.first + 1) + ", " + to_string(node->edge.second + 1) + ")";
    }

    cout << prefix << (isLast ? "|__" : "|--")
        << " " << edgeInfo << ", Оценка: " << node->cost << "\n";

    //Переход к дочерним узлам (только для выбранных ребер)
    for (size_t i = 0; i < node->children.size(); i++) {
        if (node->children[i]->selected) {
            printTree(node->children[i], prefix + (isLast ? "    " : "|   "), i == node->children.size() - 1);
        }
    }
}

//Алгоритм Литтла
int solve(vector<vector<int>> costMatrix) {
    priority_queue<Node*, vector<Node*>, CompareNodes> pq;
    vector<pair<int, int>> initialPath;
    Node* root = newNode(costMatrix, initialPath, 0, -1, 0);
    root->cost = calculateCost(root->reducedMatrix);
    pq.push(root);

    vector<Node*> allNodes; //Хранение всех узлов
    allNodes.push_back(root);

    while (!pq.empty()) {
        Node* min = pq.top();
        pq.pop();

        if (min->level == costMatrix.size() - 1) {
            min->path.emplace_back(min->vertex, 0);
            TSPPathPrint(min, costMatrix);
            printSolution(min, costMatrix);

            //Печать дерева узлов
            cout << endl << "Дерево:" << endl;
            printTree(root);

            return min->cost;
        }

        for (int j = 0; j < costMatrix.size(); j++) {
            if (min->reducedMatrix[min->vertex][j] != INT_MAX) {
                Node* child = newNode(min->reducedMatrix, min->path, min->level + 1, min->vertex, j);
                child->cost = min->cost + min->reducedMatrix[min->vertex][j] + calculateCost(child->reducedMatrix);
                min->children.push_back(child); //Добавляем дочерний узел к текущему узлу
                pq.push(child);
                allNodes.push_back(child); //Добавляем узел в общий список
            }
        }
    }

    return 0;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    vector<vector<int>> costMatrix = { 
        { INT_MAX, 3, 16, 7, 5, 23 },
        { 14, INT_MAX, 23, 8, 21, 26 },
        { 11, 23, INT_MAX, 8, 29, 24 },
        { 14, 10, 16, INT_MAX, 21, 22 },
        { 29, 22, 6, 14, INT_MAX, 27 },
        { 24, 1, 25, 1, 6, INT_MAX} 
    };

    vector<vector<int>> costMatrix1 = { 
        { INT_MAX, 12, 17, 2, 18, 19 },
        { 6, INT_MAX, 0, 5, 14, 25 },
        { 18, 0, INT_MAX, 27, 18, 4 },
        { 16, 15, 0, INT_MAX, 4, 0 },
        { 0, 8, 9, 19, INT_MAX, 12 },
        { 13, 0, 8, 0, 0, INT_MAX } 
    };
    
    solve(costMatrix);

    return 0;
}
