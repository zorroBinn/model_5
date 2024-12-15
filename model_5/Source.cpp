#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <cstring>
#include <iomanip>
#include <Windows.h>
using namespace std;

struct Node {
    vector<pair<int, int>> path;  // ���� �� ������� �������
    vector<vector<int>> reducedMatrix;  // ���������� ������� ������
    int cost;  // ��������� ����
    int vertex;  // ������� �������
    int level;  // ������� ������ ������
};

vector<double> theta;  // ������ ������ ��������� ����� ������

// �������� ������ ���� ������ ������ � ������
Node* newNode(const vector<vector<int>>& parentMatrix, const vector<pair<int, int>>& path, int level, int i, int j) {
    Node* node = new Node;
    node->path = path;

    if (level != 0) {
        node->path.emplace_back(i, j);
    }

    node->reducedMatrix = parentMatrix;

    // ������� ������ � �������
    for (int k = 0; level != 0 && k < node->reducedMatrix.size(); k++) {
        node->reducedMatrix[i][k] = INT_MAX;
        node->reducedMatrix[k][j] = INT_MAX;
    }

    // ������� ��������� �����
    node->reducedMatrix[j][0] = INT_MAX;
    node->level = level;
    node->vertex = j;

    return node;
}

// �������� �����
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

// �������� ��������
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

// ���������� ��������� ���� ����� ��������
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

// ����� �������� ��������� ������ ���������
void printBranchingTree(Node* node) {
    cout << "\n������ ��������� (����):\n";
    for (const auto& step : node->path) {
        cout << "�����: (" << step.first + 1 << ", " << step.second + 1 << ")\n";
    }
    cout << "������ ���������: " << node->cost << "\n";
}

// ����� ������� ������ ������������
void printSolution(Node* node, const vector<vector<int>>& costMatrix) {
    cout << "\n���� ����������:\n";
    int totalCost = 0;
    for (const auto& step : node->path) {
        cout << step.first + 1 << " -> " << step.second + 1 << "\n";
        totalCost += costMatrix[step.first][step.second];
    }
    cout << "\n����������� �������: ";
    for (size_t i = 0; i < node->path.size(); i++) {
        cout << node->path[i].first + 1;
        if (i < node->path.size() - 1) cout << "-";
    }
    cout << "-1\n";
    cout << "����� ���������: " << totalCost << "\n";
}

// ������� ��� ��������� ����� � ������� �����������
struct CompareNodes {
    bool operator()(Node* lhs, Node* rhs) const {
        return lhs->cost > rhs->cost; // ���� � ������� ���������� ����� ����� ������� ���������
    }
};

// ����� ������ ���� ������ ���������
void printNode(Node* node, const string& prefix, bool isLast) {
    cout << prefix;
    cout << (isLast ? "|__ " : "|-- ");

    if (node->level == 0) {
        // ������ ������ �� ����� ���������� �� ������� �������� � ������
        cout << "�������� ���� (��� ������)\n";
    }
    else {
        // ���� �������� ���������� �� ������� �������� � ������
        const auto& lastEdge = node->path.back();
        cout << "����: ";
        if (node->cost == INT_MAX) {
            cout << "!(" << lastEdge.first + 1 << ", " << lastEdge.second + 1 << ")";
        }
        else {
            cout << "(" << lastEdge.first + 1 << ", " << lastEdge.second + 1 << ")";
        }
        cout << ", ������: " << node->cost << "\n";
    }

    string newPrefix = prefix + (isLast ? "    " : "|   ");

    // ����� ���� ���� ���� (������� �������)
    for (size_t i = 0; i < node->path.size(); i++) {
        const auto& edge = node->path[i];
        cout << newPrefix << (i + 1 == node->path.size() ? "|__ " : "|-- ");
        cout << "�����: (" << edge.first + 1 << ", " << edge.second + 1 << ")\n";
    }
}

// ����� ������ ���������
void printBranchingTreeAtEnd(Node* root) {
    queue<pair<Node*, string>> q; // ������� ��� ������ �����, � �� ��������� ��� �����������
    q.push({ root, "" }); // ��������� ������

    while (!q.empty()) {
        Node* curr = q.front().first;
        string prefix = q.front().second;
        q.pop();

        bool isLast = q.empty(); // ���������, ��������� �� ��� ���� � ������� ��� �������� ������
        cout << prefix;
        cout << (isLast ? "|__ " : "|-- ");

        // ���� ��� �������� ����
        if (curr->level == 0) {
            cout << "�������� ���� (��� ������)\n";
        }
        else {
            const auto& lastEdge = curr->path.back();
            cout << "����: ";
            if (curr->cost == INT_MAX) {
                cout << "!(" << lastEdge.first + 1 << ", " << lastEdge.second + 1 << ")";
            }
            else {
                cout << "(" << lastEdge.first + 1 << ", " << lastEdge.second + 1 << ")";
            }
            cout << ", ������: " << curr->cost << "\n";
        }

        // ��������� ������� ��� �������� �����
        string newPrefix = prefix + (isLast ? "    " : "|   ");

        // ��������� �������� ���� � �������
        for (int j = 0; j < curr->reducedMatrix.size(); j++) {
            if (curr->reducedMatrix[curr->vertex][j] != INT_MAX) {
                Node* child = newNode(
                    curr->reducedMatrix, curr->path, curr->level + 1, curr->vertex, j
                );
                child->cost = curr->cost + curr->reducedMatrix[curr->vertex][j]
                    + calculateCost(child->reducedMatrix);

                q.push({ child, newPrefix });
            }
        }
    }
}

// ����� ������� ������ ������������ � ���������������� ������� �����
void TSPPathPrint(Node* list, const vector<vector<int>>& CostGraphMatrix) {
    vector<bool> removedRows(CostGraphMatrix.size(), false);
    vector<bool> removedCols(CostGraphMatrix.size(), false);
    int stepMatrix = 0;
    int cost = 0;

    for (const auto& step : list->path) {
        int row = step.first;
        int col = step.second;
        stepMatrix++;

        cout << "���: " << stepMatrix << endl;
        cout << "\t";
        for (int k = 0; k < CostGraphMatrix.size(); k++) {
            if (!removedCols[k]) {
                cout << "\t" << k + 1;
            }
        }
        cout << endl;

        for (int i = 0; i < CostGraphMatrix.size(); i++) {
            if (!removedRows[i]) {
                cout << "\t" << i + 1 << "|";
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
        cout << "����� ���������� �������� (rhk): " << cost << "\n";
        cout << "������ ������: " << CostGraphMatrix[row][col] << "\n";
        cout << "����� �������� (h, k): (" << row + 1 << ", " << col + 1 << ")\n";

        int penalty = 0;
        for (int i = 0; i < CostGraphMatrix.size(); i++) {
            if (!removedRows[i]) {
                penalty += CostGraphMatrix[row][i];
            }
            if (!removedCols[i]) {
                penalty += CostGraphMatrix[i][col];
            }
        }

        cout << "����� �� ��������������� (phk): " << penalty << "\n\n";

        removedRows[row] = true;
        removedCols[col] = true;
    }

    cout << "�������:\n";
    for (const auto& step : list->path) {
        cout << step.first + 1 << " -> " << step.second + 1 << "\n";
    }
}

// �������� ������ � ������������
int solveTSP(vector<vector<int>> costMatrix) {
    priority_queue<Node*, vector<Node*>, CompareNodes> pq;
    vector<pair<int, int>> initialPath;
    Node* root = newNode(costMatrix, initialPath, 0, -1, 0);
    root->cost = calculateCost(root->reducedMatrix);
    pq.push(root);

    vector<Node*> allNodes; // �������� ���� �����
    allNodes.push_back(root);

    while (!pq.empty()) {
        Node* min = pq.top();
        pq.pop();

        if (min->level == costMatrix.size() - 1) {
            min->path.emplace_back(min->vertex, 0);

            cout << "������� �������!\n";
            TSPPathPrint(min, costMatrix);
            printBranchingTreeAtEnd(root); // �������� ������ ��� ������ ������
            return min->cost;
        }

        for (int j = 0; j < costMatrix.size(); j++) {
            if (min->reducedMatrix[min->vertex][j] != INT_MAX) {
                Node* child = newNode(min->reducedMatrix, min->path, min->level + 1, min->vertex, j);
                child->cost = min->cost + min->reducedMatrix[min->vertex][j] + calculateCost(child->reducedMatrix);
                pq.push(child);
                allNodes.push_back(child); // ��������� ���� � ����� ������
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
    
    solveTSP(costMatrix);

    return 0;
}