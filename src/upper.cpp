// Created by D_Kuan

#include "upper.h"

int computeUpper_2(const vector<uint16_t>& sucPoint, uint16_t*** upperTbls) {
    int sum = 0;
    int minCount;
    for (int i = 0; i < SIGMA_NUM; ++i) {
        minCount = INT32_MAX;
        for (int j = 0; j < SEQ_NUM; ++j) {
            if (minCount > upperTbls[j][i][sucPoint[j]]) {
                minCount = upperTbls[j][i][sucPoint[j]];
            }
        }
        sum += minCount;
    }
    return sum;
}

// 只对最大的两条序列求mlcs长度，剩下的用上界表的min，最后加起来
int computeUpper_1(const vector<uint16_t>& sucPoint, const vector<string>& sequences, uint16_t*** upperTbls) {
    int sum = 0;
    UpperNode node;
    priority_queue<UpperNode, vector<UpperNode>, cmp> pq;
    for (int i = 0; i < SIGMA_NUM; ++i) {
        node.upperLen = UINT16_MAX;
        node.sigma = SIGMA[i];
        for (int j = 0; j < SEQ_NUM; ++j) {
            if (node.upperLen > upperTbls[j][i][sucPoint[j]]) {
                node.upperLen = upperTbls[j][i][sucPoint[j]];
                node.idx = j;
            }
        }
        pq.push(node);
        sum += node.upperLen;
    }

    UpperNode node1 = pq.top();
    pq.pop();
    UpperNode node2;
    while (!pq.empty()) {
        if (pq.top().idx != node1.idx) {
            node2 = pq.top();
            break;
        }
        pq.pop();
    }
    if (pq.empty()) { // 证明所有min对应的都是同一条序列
        return sum;
    }

    int left1 = sucPoint[node1.idx] + 1;
    int left2 = sucPoint[node2.idx] + 1;
    int len1 = sequences[node1.idx].length() - left1 + 1;
    int len2 = sequences[node2.idx].length() - left2 + 1;
    if (len1 <= 1 || len2 <= 1) return sum; // 后继点已经是最后一个字符了

    // 创建二维数组
    uint16_t** scoreMatrix = (uint16_t**) malloc(len1 * sizeof(uint16_t*));
    for (int i = 0; i < len1; ++i) {
        scoreMatrix[i] = (uint16_t*) malloc(len2 * sizeof(uint16_t));
    }
    // 初始化零行零列为0
    for (int i = 0; i < len1; ++i) {
        scoreMatrix[i][0] = 0;
    }
    for (int j = 0; j < len2; ++j) {
        scoreMatrix[0][j] = 0;
    }
    for (int i = 1; i < len1; ++i) {
        left2 = sucPoint[node2.idx] + 1;
        if (sequences[node1.idx][left1] != node1.sigma && sequences[node1.idx][left1] != node2.sigma) {
            for (int j = 1; j < len2; ++j) {
                scoreMatrix[i][j] = max(scoreMatrix[i - 1][j], scoreMatrix[i][j - 1]);
            }
        } else {
            for (int j = 1; j < len2; ++j) {
                if ((sequences[node2.idx][left2] == node1.sigma || sequences[node2.idx][left2] == node2.sigma)
                    && (sequences[node1.idx][left1] == sequences[node2.idx][left2])) {
                    scoreMatrix[i][j] = scoreMatrix[i - 1][j - 1] + 1;
                } else {
                    scoreMatrix[i][j] = max(scoreMatrix[i - 1][j], scoreMatrix[i][j - 1]);
                }
                ++left2;
            }
        }
        ++left1;
    }

    sum -= node1.upperLen + node2.upperLen;
    sum += scoreMatrix[len1 - 1][len2 - 1];

    // 释放二维数组
    for (int i = 0; i < len1; ++i) {
        free(scoreMatrix[i]);
    }
    free(scoreMatrix);
    return sum;
}

void printUpperTbls(uint16_t*** upperTbls, const vector<string>& sequences) {
    for (int i = 0; i < SEQ_NUM; ++i) {
        cout << "\t";
        for (int j = 0; j < sequences[i].length(); ++j) {
            cout << j << "\t";
        }
        cout << endl << "\t";
        for (int j = 0; j < sequences[i].length(); ++j) {
            cout << sequences[i][j] << "\t";
        }
        cout << endl;
        for (int j = 0; j < 4; ++j) {
            cout << SIGMA[j] << "\t";
            for (int k = 0; k < sequences[i].length(); ++k) {
                cout << upperTbls[i][j][k] << "\t";
            }
            cout << endl;
        }
        cout << "-------------------------------------------------------------------" << endl;
    }
}