// Created by D_Kuan

#include "lower.h"

// 计算权重序列的权重
int computeWeight(boost::unordered_map<vector<uint16_t>, uint32_t>& hashTable, vector<WeightNode>& DAG,
                  uint32_t& maxId, vector<uint32_t>& idOfCurExtendNodes, vector<uint32_t>& idOfNextExtendNodes,
                  const vector<string>& weightSequences) {
    // 初始化
    hashTable.clear();
    DAG.clear();
    maxId = 0;
    idOfCurExtendNodes.clear();
    idOfNextExtendNodes.clear();

    vector<uint16_t> sourcePoint(SEQ_NUM, 0);
    hashTable.emplace(sourcePoint, maxId++); // 将源节点添加到hashtable中
    DAG.emplace_back(sourcePoint, 0, 0); // 将源节点添加到DAG图,在零号位置

    vector<uint16_t> endPoint(SEQ_NUM, UINT16_MAX);
    hashTable.emplace(endPoint, maxId++); // 终节点添加到hashtable中
    DAG.emplace_back(endPoint, 0, 0); // 终节点添加到DAG中,在一号位置

    // 先从源节点开始扩展，源节点在零号索引位置，终节点在一号索引位置
    idOfCurExtendNodes.emplace_back(0);

    // 获得后继表
    uint16_t*** sucTbls;
    getSucTbls(&sucTbls, weightSequences);

    vector<vector<uint16_t>> sucPoints;
    sucPoints.reserve(SIGMA_NUM);

    while (!idOfCurExtendNodes.empty()) {
        idOfNextExtendNodes.clear();

        for (const auto& curNodeId: idOfCurExtendNodes) {

            // 这里添加一个时间界限策略，如果超过设定的WEIGHT_TIME_BOUND或者(内存小于1.5G和内存剩余不足10%)
            if (clock() - WEIGHT_START_TIME > WEIGHT_TIME_BOUND || (getFreeMemory() <= 1536 && getMemoryPercentage() < 0.1)) {

                // 释放后继表
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
                if (sucNodeIter == hashTable.end()) { // 在hashtable中不存在
                    // 更新当前节点的后继
                    DAG[curNodeId].idOfSucNodes.emplace_back(maxId);
                    idOfNextExtendNodes.emplace_back(maxId);
                    // 将该后继节点添加到DAG图中，添加到DAG的末尾
                    DAG.emplace_back(sucPoint, 1, 0);
                    // 将该后继节点添加到hashtable中
                    hashTable.emplace(sucPoint, maxId);
                    ++maxId;
                } else { // 在hashtable中存在
                    // 更新当前节点的后继和后继节点的入度
                    ++(DAG[sucNodeIter->second].indegree);
                    DAG[curNodeId].idOfSucNodes.emplace_back(sucNodeIter->second);
                }
            }
        }
        idOfCurExtendNodes = idOfNextExtendNodes;
    }

    // 构造完DAG图之后，需要通过图的层次遍历求每个节点的curLen
    queue<uint32_t> Q;
    Q.push(0); // 从源节点开始遍历DAG
    while (!Q.empty()) {

        // 这里添加一个时间界限策略，如果超过设定的WEIGHT_TIME_BOUND或者(内存小于1.5G和内存剩余不足10%)
        if (clock() - WEIGHT_START_TIME > WEIGHT_TIME_BOUND || (getFreeMemory() <= 1536 && getMemoryPercentage() < 0.1)) {

            // 释放后继表
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
                // 这里计算的时候也把终点算作一个节点了，也加上一了，所以最后长度需要减一
                DAG[sucNodeId].curLen = max<int>(DAG[sucNodeId].curLen, DAG[curNodeId].curLen + 1);
                if (sucNodeId != 1) {
                    Q.push(sucNodeId);
                }
            }
        }
    }

    // 释放后继表
    for (int i = 0; i < SEQ_NUM; ++i) {
        for (int j = 0; j < SIGMA_NUM; ++j) {
            free(sucTbls[i][j]);
        }
        free(sucTbls[i]);
    }
    free(sucTbls);

    return DAG[1].curLen - 1; // 不算上终节点的长度一
}

// 通过权重计算下界，每次向后扩展zeta个节点
// 每次选取前zeta个sum小的
int computeLower(const vector<string>& sequences, const int strategy, const int num, int zeta) {
    WEIGHT_START_TIME = clock();

    // 这是计算权重序列的权重需要的结构
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

    int maxLen = 0; // 最终的下界长度
    vector<uint16_t> sourcePoint(SEQ_NUM, 0);
    vector<uint16_t> endPoint(SEQ_NUM, UINT16_MAX);
    vector<string> weightSequences;
    weightSequences.reserve(SEQ_NUM);
    vector<vector<uint16_t>> sucPoints;
    sucPoints.reserve(SIGMA_NUM);

    // 根据不同的策略获得不同的后继表
    uint16_t*** sucTbls;
    if (strategy == 1) {
        getSucTblsByStep(&sucTbls, sequences, num);
    } else {
        getSucTblsBySection(&sucTbls, sequences);
    }

    curLevel->emplace(sourcePoint, 0);
    while (!curLevel->empty()) {
        for (const auto& curNode: *curLevel) {
            sucPoints.clear();
            if (strategy == 1) {
                getSucPoints(sucPoints, curNode.first, sucTbls);
            } else {
                getSucPointsOfSection(sucPoints, curNode.first, sucTbls, sequences, num);
            }
            // 计算sum
            int sum;
            for (const auto& sucPoint: sucPoints) {
                // 如果当前后继点不在下一层中
                if (nextLevel->find(sucPoint) == nextLevel->end()) {
                    sum = 0;
                    for (uint16_t item: sucPoint) {
                        sum += item;
                    }
                    // 这里的zeta可以更改，这里限定每层最多只能拥有zeta个节点
                    if (candidates.size() == zeta) {
                        // 如果当前后继点的方差和比下一层中的每个方差都大，同时下一层恰好已经zeta个节点了，则直接跳过
                        if (sum >= candidates.top().second) continue;
                        // 否则删除堆顶的最大方差的那个匹配点，同时在下一层中删除它
                        nextLevel->erase(candidates.top().first);
                        candidates.pop();
                        // 同时将该后继点添加到下一层中，同时添加到优先级队列中
                        nextLevel->emplace(sucPoint, 0);
                        candidates.emplace(sucPoint, sum);
                    } else { // 下一层未满zeta个节点，则直接添加到下一层和优先级队列中
                        nextLevel->emplace(sucPoint, 0);
                        candidates.emplace(sucPoint, sum);
                    }
                }
            }
        }

        // 计算当前节点和后继点之间的权重
        for (const auto& curNode: *curLevel) {
            for (auto& sucNode: *nextLevel) {
                weightSequences.clear();
                int weight = -1;
                int left;
                int right;
                if (sucNode.first == endPoint) { // 如果当前节点的后继节点是终节点
                    for (int i = 0; i < SEQ_NUM; ++i) {
                        left = curNode.first[i];
                        // 如果权重序列的最左面那个字符正好是原序列的最后一个字符，那么权重为0,因为我要从left+1的位置开始截取权重序列
                        if (left == sequences[i].length() - 1) {
                            weight = 0;
                            break;
                        }
                        weightSequences.emplace_back(" ");
                        // append方法是左闭右开的
                        // 但是截取序列的从left+1开始到right
                        weightSequences[i].append(sequences[i].begin() + left + 1, sequences[i].end());
                    }
                    if (weight == -1) {
                        weight = computeWeight(hashTable, DAG, maxId, idOfCurExtendNodes, idOfNextExtendNodes,
                                               weightSequences);

                        if (weight == -2) { // 代表超时了
                            // 释放后继表
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
                            weight = -3; // 代表并不是当前节点的后继
                            break;
                        }
                        weightSequences.emplace_back(" ");
                        // append方法是左闭右开的
                        // 但是截取的序列从left+1开始到right
                        weightSequences[i].append(sequences[i].begin() + left + 1, sequences[i].begin() + right + 1);
                    }
                    if (weight == -1) {
                        weight = computeWeight(hashTable, DAG, maxId, idOfCurExtendNodes, idOfNextExtendNodes,
                                               weightSequences);

                        if (weight == -2) {
                            // 释放后继表
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
        // 下一层删除终点
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
    // 释放后继表
    for (int i = 0; i < SEQ_NUM; ++i) {
        for (int j = 0; j < SIGMA_NUM; ++j) {
            free(sucTbls[i][j]);
        }
        free(sucTbls[i]);
    }
    free(sucTbls);
    return maxLen;
}

// 快速计算下界
int quickLower(uint16_t*** sucTbls, int theta) {
    boost::unordered_set<vector<uint16_t>> level1;
    boost::unordered_set<vector<uint16_t>> level2;
    boost::unordered_set<vector<uint16_t>>* curLevel = &level1;
    boost::unordered_set<vector<uint16_t>>* nextLevel = &level2;
    priority_queue<vector<uint16_t>, vector<vector<uint16_t>>, cmp1> pq; // 大根堆

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
            // 如果产生的后继是终点，则不放入下一层
            if (sucPoints[0] == endPoint) {
                continue;
            }
            for (const auto& sucPoint: sucPoints) {
                // 如果当前后继点不在下一层中
                if (nextLevel->find(sucPoint) == nextLevel->end()) {
                    // 计算当前后继点的sum和
                    uint32_t sucSum = 0;
                    for (uint16_t num: sucPoint) sucSum += num;
                    if (pq.size() == theta) {
                        // 如果当前后继点的sum和比下一层中的每个sum都大，同时下一层恰好已经theta个节点了，则直接跳过
                        if (sucSum >= topSum) continue;
                        // 否则删除堆顶的最大sum的那个匹配点，同时在下一层中删除它
                        nextLevel->erase(pq.top());
                        pq.pop();
                        // 同时将该后继点添加到下一层中，同时添加到优先级队列中
                        nextLevel->insert(sucPoint);
                        pq.push(sucPoint);
                        // 同时更新堆顶元素的sum和
                        topSum = 0;
                        for (uint16_t num: pq.top()) topSum += num;
                    } else { // 下一层未满theta个节点，则直接添加到下一层和优先级队列中
                        nextLevel->insert(sucPoint);
                        pq.push(sucPoint);
                        if (sucSum > topSum) topSum = sucSum;
                    }
                }
                // 如果当前后继点在下一层中，则直接跳过
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
    // 减去源点那层
    return len - 1;
}
