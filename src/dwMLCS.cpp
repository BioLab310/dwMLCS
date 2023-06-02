#include "dwMLCS.h"

uint32_t sumNums = 1; // 总共的节点数量
uint32_t sumNumsWithDel = 1; // 使用删除策略下，当前结点数量
uint32_t maxSumNumsWithDel = 1; // 使用删除策略下动态DAG中节点最多数量

void dwMLCS(uint16_t*** sucTbls, const vector<string>& sequences, int lowerLen) {
    vector<boost::unordered_map<vector<uint16_t>, vector<string>>> SumHashVector(SEQ_LEN_SUM + 2);
    SumHashVector[0].emplace(vector<uint16_t>(SEQ_NUM, 0), vector<string>());

    uint16_t*** upperTbls;
    getUpperTbls(&upperTbls, sequences);

    vector<vector<uint16_t>> sucPoints;
    sucPoints.reserve(SIGMA_NUM);
    vector<uint32_t> sums;
    sums.reserve(SIGMA_NUM);
    vector<uint16_t> endPoint(SEQ_NUM, UINT16_MAX);

    for (int i = 0; i <= SEQ_LEN_SUM; ++i) {
        if (SumHashVector[i].empty()) {
            continue;
        }

        for (const auto& curNode: SumHashVector[i]) {
            sucPoints.clear();
            sums.clear();
            getSucPointsWithSum(sucPoints, sums, curNode.first, sucTbls);

            int j = 0;
            uint32_t lenSumOfSucNode;
            for (const auto& sucPoint: sucPoints) {
                lenSumOfSucNode = sums[j++];
                auto sucNodeIter = SumHashVector[lenSumOfSucNode].find(sucPoint);
                if (sucNodeIter != SumHashVector[lenSumOfSucNode].end()) {
                    int curNodeLen = curNode.second[0].length();
                    int sucNodeLen = sucNodeIter->second[0].length();
                    if (curNodeLen + 1 < sucNodeLen) {
                        continue;
                    }
                    if (curNodeLen + 1 > sucNodeLen) {
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
                } else {
                    bool isEndPoint = false;
                    if (sucPoint == endPoint) {
                        isEndPoint = true;
                    }
                    int curLen;
                    if (curNode.second.empty()) {
                        curLen = 0;
                    } else {
                        curLen = curNode.second[0].length();
                    }
                    if (isEndPoint) {
                        if (curLen < lowerLen) {
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

                            if (curLen + 1 + upperLen < lowerLen) {
                                continue;
                            }
                        }
                    }
                    char charOfSucNode;
                    if (sucPoint[0] == UINT16_MAX) {
                        charOfSucNode = '$';
                    } else {
                        charOfSucNode = sequences[0][sucPoint[0]];
                    }
                    if (SumHashVector[lenSumOfSucNode].empty()) {
                        SumHashVector[lenSumOfSucNode].reserve(SumHashVector[i].bucket_count() >> 2);
                    }
                    auto iter = SumHashVector[lenSumOfSucNode].emplace(sucPoint, vector<string>());
                    if (!curLen) {
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

                    sumNums++;
                    sumNumsWithDel++;
                }
            }
        }
        maxSumNumsWithDel = max<uint32_t>(maxSumNumsWithDel, sumNumsWithDel);
        sumNumsWithDel -= SumHashVector[i].size();

        boost::unordered_map<vector<uint16_t>, vector<string>>().swap(SumHashVector[i]);
    }

    printMlcs(SumHashVector[SEQ_LEN_SUM + 1].begin()->second);
}

void printMlcs(vector<string>& mlcs) {
    ofstream ofs;
    ofs.open(WRITE_PATH, ios::out);
    int size = mlcs.size();
    cout << "The sum of nodes:" << sumNums << endl;
    cout << "The max sum of nodes:" << maxSumNumsWithDel << endl;
    cout << "The number of MLCS:" << size << endl;
    cout << "The length of MLCS:" << mlcs[0].length() - 1 << endl;
    for (int i = 0; i < size; ++i) {
        mlcs[i].pop_back();
        ofs << mlcs[i] << endl;
    }
    ofs.close();
}













