// Created by dekuan

#include "lower.h"

// 计算权重序列的权重
int computeWeight(boost::unordered_map<vector<uint16_t>, uint32_t>& hashTable, vector<WeightNode>& DAG,
                  uint32_t& maxId, vector<uint32_t>& idOfCurExtendNodes, vector<uint32_t>& idOfNextExtendNodes,
                  const vector<string>& weightSequences) {
    hashTable.clear();
    DAG.clear();
    maxId = 0;
    idOfCurExtendNodes.clear();
    idOfNextExtendNodes.clear();

    vector<uint16_t> sourcePoint(SEQ_NUM, 0);
    hashTable.emplace(sourcePoint, maxId++);
    DAG.emplace_back(sourcePoint, 0, 0);

    vector<uint16_t> endPoint(SEQ_NUM, UINT16_MAX);
    hashTable.emplace(endPoint, maxId++);
    DAG.emplace_back(endPoint, 0, 0);

    idOfCurExtendNodes.emplace_back(0);

    uint16_t*** sucTbls;
    getSucTbls(&sucTbls, weightSequences);

    vector<vector<uint16_t>> sucPoints;
    sucPoints.reserve(SIGMA_NUM);

    while (!idOfCurExtendNodes.empty()) {
        idOfNextExtendNodes.clear();

        for (const auto& curNodeId: idOfCurExtendNodes) {

            if ((clock() - WEIGHT_START_TIME) > WEIGHT_TIME_BOUND) {
                for (int i = 0; i < SEQ_NUM; ++i) {
                    for (int j = 0; j < SIGMA_NUM; ++j) {
                        free(sucTbls[i][j]);
                    }
                    free(sucTbls[i]);
                }
                free(sucTbls);
                return -2;
            }

            sucPoints.clear();
            getSucPoints(sucPoints, DAG[curNodeId].matchPoint, sucTbls);

            for (const auto& sucPoint: sucPoints) {
                auto sucNodeIter = hashTable.find(sucPoint);
                if (sucNodeIter == hashTable.end()) {
                    DAG[curNodeId].idOfSucNodes.emplace_back(maxId);
                    idOfNextExtendNodes.emplace_back(maxId);
                    DAG.emplace_back(sucPoint, 1, 0);
                    hashTable.emplace(sucPoint, maxId);
                    ++maxId;
                } else {
                    ++(DAG[sucNodeIter->second].indegree);
                    DAG[curNodeId].idOfSucNodes.emplace_back(sucNodeIter->second);
                }
            }
        }
        idOfCurExtendNodes = idOfNextExtendNodes;
    }

    queue<uint32_t> Q;
    Q.push(0);
    while (!Q.empty()) {

        if ((clock() - WEIGHT_START_TIME) > WEIGHT_TIME_BOUND) {
            for (int i = 0; i < SEQ_NUM; ++i) {
                for (int j = 0; j < SIGMA_NUM; ++j) {
                    free(sucTbls[i][j]);
                }
                free(sucTbls[i]);
            }
            free(sucTbls);
            return -2;
        }

        uint32_t curNodeId = Q.front();
        Q.pop();
        for (const auto& sucNodeId: DAG[curNodeId].idOfSucNodes) {
            if (--DAG[sucNodeId].indegree == 0 || sucNodeId == 1) {
                DAG[sucNodeId].curLen = max<int>(DAG[sucNodeId].curLen, DAG[curNodeId].curLen + 1);
                if (sucNodeId != 1) {
                    Q.push(sucNodeId);
                }
            }
        }
    }

    for (int i = 0; i < SEQ_NUM; ++i) {
        for (int j = 0; j < SIGMA_NUM; ++j) {
            free(sucTbls[i][j]);
        }
        free(sucTbls[i]);
    }
    free(sucTbls);

    return DAG[1].curLen - 1;
}

// 通过权重计算下界，每次向后扩展zeta个节点
// 每次选取前zeta个sum小的
int computeLower(const vector<string>& sequences, const int strategy, const int num, int zeta) {
    WEIGHT_START_TIME = clock();

    boost::unordered_map<vector<uint16_t>, uint32_t> hashTable;
    vector<WeightNode> DAG;
    uint32_t maxId = 0;
    vector<uint32_t> idOfCurExtendNodes;
    vector<uint32_t> idOfNextExtendNodes;

    boost::unordered_map<vector<uint16_t>, int> level1;
    boost::unordered_map<vector<uint16_t>, int> level2;
    boost::unordered_map<vector<uint16_t>, int>* curLevel = &level1;
    boost::unordered_map<vector<uint16_t>, int>* nextLevel = &level2;
    priority_queue<pair<vector<uint16_t>, int>, vector<pair<vector<uint16_t>, int>>, cmp2> candidates;

    int maxLen = 0;
    vector<uint16_t> sourcePoint(SEQ_NUM, 0);
    vector<uint16_t> endPoint(SEQ_NUM, UINT16_MAX);
    vector<string> weightSequences;
    weightSequences.reserve(SEQ_NUM);
    vector<vector<uint16_t>> sucPoints;
    sucPoints.reserve(SIGMA_NUM);

    uint16_t*** sucTbls;
    if (strategy == 1) {
        getSucTblsByStep(&sucTbls, sequences, num);
    } else {
        getSucTblsBySection(&sucTbls, sequences, num);
    }

    curLevel->emplace(sourcePoint, 0);
    while (!curLevel->empty()) {
        for (const auto& curNode: *curLevel) {
            sucPoints.clear();
            getSucPoints(sucPoints, curNode.first, sucTbls);
            int sum;
            for (const auto& sucPoint: sucPoints) {
                if (nextLevel->find(sucPoint) == nextLevel->end()) {
                    sum = 0;
                    for (uint16_t item: sucPoint) {
                        sum += item;
                    }
                    if (candidates.size() == zeta) {
                        if (sum >= candidates.top().second) continue;
                        nextLevel->erase(candidates.top().first);
                        candidates.pop();
                        nextLevel->emplace(sucPoint, 0);
                        candidates.emplace(sucPoint, sum);
                    } else {
                        nextLevel->emplace(sucPoint, 0);
                        candidates.emplace(sucPoint, sum);
                    }
                }
            }
        }

        for (const auto& curNode: *curLevel) {
            for (auto& sucNode: *nextLevel) {
                weightSequences.clear();
                int weight = -1;
                int left;
                int right;
                if (sucNode.first == endPoint) {
                    for (int i = 0; i < SEQ_NUM; ++i) {
                        left = curNode.first[i];
                        if (left == sequences[i].length() - 1) {
                            weight = 0;
                            break;
                        }
                        weightSequences.emplace_back(" ");
                        weightSequences[i].append(sequences[i].begin() + left + 1, sequences[i].end());
                    }
                    if (weight == -1) {
                        weight = computeWeight(hashTable, DAG, maxId, idOfCurExtendNodes, idOfNextExtendNodes,
                                               weightSequences);

                        if (weight == -2) {
                            for (int i = 0; i < SEQ_NUM; ++i) {
                                for (int j = 0; j < SIGMA_NUM; ++j) {
                                    free(sucTbls[i][j]);
                                }
                                free(sucTbls[i]);
                            }
                            free(sucTbls);
                            return -2;
                        }
                    }
                } else {
                    for (int i = 0; i < SEQ_NUM; ++i) {
                        left = curNode.first[i];
                        right = sucNode.first[i];
                        if (left >= right) {
                            weight = -3;
                            break;
                        }
                        weightSequences.emplace_back(" ");
                        weightSequences[i].append(sequences[i].begin() + left + 1, sequences[i].begin() + right + 1);
                    }
                    if (weight == -1) {
                        weight = computeWeight(hashTable, DAG, maxId, idOfCurExtendNodes, idOfNextExtendNodes,
                                               weightSequences);

                        if (weight == -2) {
                            for (int i = 0; i < SEQ_NUM; ++i) {
                                for (int j = 0; j < SIGMA_NUM; ++j) {
                                    free(sucTbls[i][j]);
                                }
                                free(sucTbls[i]);
                            }
                            return -2;
                        }
                    }
                }
                if (weight != -3) {
                    sucNode.second = max<int>(sucNode.second, curNode.second + weight);
                }
            }
        }

        auto iter = nextLevel->find(endPoint);
        if (iter != nextLevel->end()) {
            maxLen = max<int>(iter->second, maxLen);
            nextLevel->erase(iter);
        }
        boost::unordered_map<vector<uint16_t>, int>* temp = curLevel;
        curLevel = nextLevel;
        nextLevel = temp;
        nextLevel->clear();
        priority_queue<pair<vector<uint16_t>, int>, vector<pair<vector<uint16_t>, int>>, cmp2>().swap(candidates);

    }

    for (int i = 0; i < SEQ_NUM; ++i) {
        for (int j = 0; j < SIGMA_NUM; ++j) {
            free(sucTbls[i][j]);
        }
        free(sucTbls[i]);
    }
    free(sucTbls);
    return maxLen;
}

// 近似下界,每次之产生一个后继点向后迭代计算
// Big-MLCS
int BigMLCS(uint16_t*** sucTbls) {
    int level = 0;
    vector<uint16_t> curMatchPoint(SEQ_NUM, 0);
    vector<uint16_t> endPoint(SEQ_NUM, UINT16_MAX);
    vector<vector<uint16_t>> sucPoints;
    sucPoints.reserve(SIGMA_NUM);

    while (curMatchPoint != endPoint) {
        sucPoints.clear();
        getSucPoints(sucPoints, curMatchPoint, sucTbls);

        int index;
        if (sucPoints.size() == 1) {
            index = 0;
        } else {
            index = selectOnePoint(sucPoints);
        }

        ++level;
        curMatchPoint = sucPoints[index];
    }
    return level - 1;
}

// 近似下界，每次产生两个后继点向后迭代计算
// Best-MLCS
// 每层中的节点个数默认设置为256个
int BestMLCS(uint16_t*** sucTbls, int theta) {
    boost::unordered_set<vector<uint16_t>> level1;
    boost::unordered_set<vector<uint16_t>> level2;
    boost::unordered_set<vector<uint16_t>>* curLevel = &level1;
    boost::unordered_set<vector<uint16_t>>* nextLevel = &level2;
    priority_queue<vector<uint16_t>, vector<vector<uint16_t>>, cmp1> pq;

    int len = 0;
    uint32_t topSum = 0;
    vector<uint16_t> sourcePoint(SEQ_NUM, 0);
    vector<uint16_t> endPoint(SEQ_NUM, UINT16_MAX);
    vector<vector<uint16_t>> sucPoints;
    sucPoints.reserve(SIGMA_NUM);

    curLevel->insert(sourcePoint);

    while (!curLevel->empty()) {
        for (const auto& matchPoint: *curLevel) {
            sucPoints.clear();
            getSucPoints(sucPoints, matchPoint, sucTbls);
            if (sucPoints[0] == endPoint) {
                continue;
            }
            for (const auto& sucPoint: sucPoints) {
                if (nextLevel->find(sucPoint) == nextLevel->end()) {
                    uint32_t sucSum = 0;
                    for (uint16_t num: sucPoint) sucSum += num;
                    if (pq.size() == theta) {
                        if (sucSum >= topSum) continue;
                        nextLevel->erase(pq.top());
                        pq.pop();
                        nextLevel->insert(sucPoint);
                        pq.push(sucPoint);
                        topSum = 0;
                        for (uint16_t num: pq.top()) topSum += num;
                    } else {
                        nextLevel->insert(sucPoint);
                        pq.push(sucPoint);
                        if (sucSum > topSum) topSum = sucSum;
                    }
                }
            }
        }
        boost::unordered_set<vector<uint16_t>>* temp = curLevel;
        curLevel = nextLevel;
        nextLevel = temp;
        nextLevel->clear();
        priority_queue<vector<uint16_t>, vector<vector<uint16_t>>, cmp1>().swap(pq);
        topSum = 0;
        ++len;
    }

    return len - 1;
}