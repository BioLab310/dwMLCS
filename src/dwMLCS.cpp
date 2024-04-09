// Created by D_Kuan

#include "dwMLCS.h"

uint32_t SUM_NUMS = 1; // 总共的节点数量
uint32_t SUM_NUMS_WITH_DEL = 1; // 使用删除策略下，当前动态DAG中节点数量
uint32_t MAX_SUM_NUMS_WITH_DEL = 1; // 使用删除策略下动态DAG中节点最多数量

// dwMLCS算法
void dwMLCS(uint16_t*** sucTbls, const vector<string>& sequences, int lowerLen) {
    // 定义SumHashVector并初始化
    vector<boost::unordered_map<vector<uint16_t>, vector<string>>> SumHashVector(SEQ_LEN_SUM + 2);
    // 初始化SumHashVector[0],添加源点
    SumHashVector[0].emplace(vector<uint16_t>(SEQ_NUM, 0), vector<string>());

    // 获得上界表
    uint16_t*** upperTbls;
    getUpperTbls(&upperTbls, sequences);

    vector<vector<uint16_t>> sucPoints;
    sucPoints.reserve(SIGMA_NUM);
    vector<uint32_t> sums;
    sums.reserve(SIGMA_NUM);
    vector<uint16_t> endPoint(SEQ_NUM, UINT16_MAX);

    // 遍历SumHashVector，遍历到倒数第二层，最后一层只有一个节点是终点
    for (int i = 0; i <= SEQ_LEN_SUM; ++i) {
        // 如果当前层为空，跳过当前层
        if (SumHashVector[i].empty()) {
            continue;
        }

        // 遍历SumHashVector[i]层的节点
        for (const auto& curNode: SumHashVector[i]) {
            sucPoints.clear();
            sums.clear();
            getSucPointsWithSum(sucPoints, sums, curNode.first, sucTbls);

            int j = 0;
            uint32_t lenSumOfSucNode;
            for (const auto& sucPoint: sucPoints) {
                lenSumOfSucNode = sums[j++];
                auto sucNodeIter = SumHashVector[lenSumOfSucNode].find(sucPoint);
                // 如果后继点在SumHashVector中存在
                if (sucNodeIter != SumHashVector[lenSumOfSucNode].end()) {
                    // 当前点不可能是源点
                    int curNodeLen = curNode.second[0].length();
                    int sucNodeLen = sucNodeIter->second[0].length();
                    if (curNodeLen + 1 < sucNodeLen) { // 小于后继点长度,什么都不做
                        continue;
                    }
                    if (curNodeLen + 1 > sucNodeLen) { // 大于后继点长度,后继点的MLCSs清空
                        sucNodeIter->second.clear();
                    }
                    char charOfSucNode;
                    if (sucPoint[0] == UINT16_MAX) {
                        charOfSucNode = '$';
                    } else {
                        charOfSucNode = sequences[0][sucPoint[0]];
                    }
                    for (const auto& mlcsOfCurNode: curNode.second) {
                        sucNodeIter->second.emplace_back(mlcsOfCurNode);
                        sucNodeIter->second.back().push_back(charOfSucNode);
                    }
                } else { // 如果后继点不存在
                    bool isEndPoint = false;
                    if (sucPoint == endPoint) { // 如果后继是终点
                        isEndPoint = true;
                    }
                    int curLen;
                    if (curNode.second.empty()) { // 如果当前节点是源点
                        curLen = 0;
                    } else {
                        curLen = curNode.second[0].length();
                    }
                    if (isEndPoint) { // 考虑后继点是不是终点的情况
                        if (curLen < lowerLen) { // 直接被过滤掉
                            continue;
                        }
                        lowerLen = curLen;
                    } else {
                        if (curLen + 1 >= lowerLen) {
                            lowerLen = curLen + 1;
                        } else {
                            int upperLen;

                            if (curLen + 1 < (lowerLen >> 1)) {
                                upperLen = computeUpper_1(sucPoint, sequences, upperTbls);
                            } else {
                                upperLen = computeUpper_2(sucPoint, upperTbls);
                            }

                            if (curLen + 1 + upperLen < lowerLen) { // 直接被过滤掉
                                continue;
                            }
                        }
                    }
                    // 更新后继点的MLCSs
                    char charOfSucNode;
                    if (sucPoint[0] == UINT16_MAX) {
                        charOfSucNode = '$';
                    } else {
                        charOfSucNode = sequences[0][sucPoint[0]];
                    }
                    // 提前扩充bucket的大小，防止rehash浪费时间
                    if (SumHashVector[lenSumOfSucNode].empty()) {
                        SumHashVector[lenSumOfSucNode].reserve(SumHashVector[i].bucket_count() >> 2);
                    }
                    // 向SumHashVector中后继点那层,添加进去
                    auto iter = SumHashVector[lenSumOfSucNode].emplace(sucPoint, vector<string>());
                    if (!curLen) { // 当前点是源点
                        iter.first->second.emplace_back("");
                        iter.first->second.back().push_back(charOfSucNode);
                    } else {
                        int size = curNode.second.size();
                        iter.first->second.reserve(size);
                        for (int k = 0; k < size; ++k) {
                            iter.first->second.emplace_back(curNode.second[k]);
                            iter.first->second.back().push_back(charOfSucNode);
                        }
                    }

                    // 只使用路径剪枝策略而未使用删除策略的节点数量
                    SUM_NUMS++;
                    // 使用路径剪枝策略以及删除策略，处理完当前层之后动态DAG中的节点数量
                    SUM_NUMS_WITH_DEL++;
                }
            }
        }
        // 某个时刻动态DAG中的结点最多数量
        MAX_SUM_NUMS_WITH_DEL = max<uint32_t>(MAX_SUM_NUMS_WITH_DEL, SUM_NUMS_WITH_DEL);
        SUM_NUMS_WITH_DEL -= SumHashVector[i].size();

        // 释放当前层的空间
        boost::unordered_map<vector<uint16_t>, vector<string>>().swap(SumHashVector[i]);
    }

    printMlcs(SumHashVector[SEQ_LEN_SUM + 1].begin()->second);

    // 最后不释放后继表和上界表也可以,因为程序也运行完了
}

// 输出最终的MLCS
void printMlcs(vector<string>& mlcs) {
    ofstream ofs;
    ofs.open(WRITE_PATH, ios::out);
    int size = mlcs.size();
    cout << "The sum of nodes:" << SUM_NUMS << endl;
    cout << "The maximum number of nodes at any time:" << MAX_SUM_NUMS_WITH_DEL << endl;
    cout << "The number of MLCS:" << (mlcs[0].length() == 1 ? 0 : size) << endl;
    cout << "The length of MLCS:" << mlcs[0].length() - 1 << endl;
    for (int i = 0; i < size; ++i) {
        mlcs[i].pop_back();
        ofs << mlcs[i] << endl;
    }
    ofs.close();
}













