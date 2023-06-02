// Created by dekuan

#include "basic.h"

// 读取d条序列 返回序列长度和(不算空格的长度)
void readSeq(vector<string>& sequences) {
    sequences.reserve(SEQ_NUM);

    ifstream ifs(READ_PATH, ios::in);
    if (!ifs.is_open()) {
        cout << "文件打开失败,请确保有此文件或者文件路径正确!" << endl;
        exit(1);
    }

    ifs >> SIGMA;
    SIGMA_NUM = SIGMA.length();

    SEQ_LEN_SUM = 0;
    int length;
    string temp;

    for (int i = 0; i < SEQ_NUM; ++i) {
        ifs >> temp;

        MIN_LEN = min<uint16_t>(MIN_LEN, temp.length());

        sequences.emplace_back(" ");
        sequences[i].append(temp);
        length = temp.length();
        SEQ_LEN_SUM += length;
    }
    ifs.close();
}

// 获得上界表
void getUpperTbls(uint16_t**** upperTbls, const vector<string>& sequences) {
    uint16_t*** upperTbl;
    upperTbl = (uint16_t***) malloc(sizeof(uint16_t**) * SEQ_NUM);
    for (int i = 0; i < SEQ_NUM; ++i) {
        upperTbl[i] = (uint16_t**) malloc(sizeof(uint16_t*) * SIGMA_NUM);
        for (int j = 0; j < SIGMA_NUM; ++j) {
            upperTbl[i][j] = (uint16_t*) malloc(sizeof(uint16_t) * sequences[i].length());
        }
    }
    *upperTbls = upperTbl;

    for (int i = 0; i < SEQ_NUM; ++i) {
        for (int j = 0; j < SIGMA_NUM; ++j) {
            int count = 0;
            for (int k = sequences[i].length() - 1; k >= 0; --k) {
                upperTbl[i][j][k] = count;
                if (sequences[i][k] == SIGMA[j]) {
                    ++count;
                }
            }
        }
    }
}

// 获得普通后继表
void getSucTbls(uint16_t**** sucTbls, const vector<string>& sequences) {
    uint16_t*** sucTbl = (uint16_t***) malloc(sizeof(uint16_t**) * SEQ_NUM);
    for (int i = 0; i < SEQ_NUM; ++i) {
        sucTbl[i] = (uint16_t**) malloc(sizeof(uint16_t*) * SIGMA_NUM);
        for (int j = 0; j < SIGMA_NUM; ++j) {
            sucTbl[i][j] = (uint16_t*) malloc(sizeof(uint16_t) * sequences[i].length());
        }
    }

    for (int i = 0; i < SEQ_NUM; ++i) {
        for (int j = 0; j < SIGMA_NUM; ++j) {
            int length = sequences[i].length();
            sucTbl[i][j][length - 1] = UINT16_MAX;
            for (int k = length - 2; k >= 0; --k) {
                if (SIGMA[j] == sequences[i][k + 1]) {
                    sucTbl[i][j][k] = k + 1;
                } else {
                    sucTbl[i][j][k] = sucTbl[i][j][k + 1];
                }
            }
        }
    }
    *sucTbls = sucTbl;
}

// 当前节点的第step个后继
void getSucTblsByStep(uint16_t**** sucTbls, const vector<string>& sequences, const int step) {
    uint16_t*** sucTbl = (uint16_t***) malloc(sizeof(uint16_t**) * SEQ_NUM);
    for (int i = 0; i < SEQ_NUM; ++i) {
        sucTbl[i] = (uint16_t**) malloc(sizeof(uint16_t*) * SIGMA_NUM);
        for (int j = 0; j < SIGMA_NUM; ++j) {
            sucTbl[i][j] = (uint16_t*) malloc(sizeof(uint16_t) * sequences[i].length());
        }
    }

    for (int i = 0; i < SEQ_NUM; ++i) {
        int length = sequences[i].length();
        vector<vector<int>> sigmaIndexes(SIGMA_NUM, vector<int>());
        for (int j = 0; j < SIGMA_NUM; ++j) {
            sigmaIndexes[j].reserve(length);
            sigmaIndexes[j].emplace_back(0);
        }
        for (int j = 1; j < length; ++j) {
            for (int k = 0; k < SIGMA_NUM; ++k) {
                if (SIGMA[k] == sequences[i][j]) {
                    sigmaIndexes[k].emplace_back(j);
                    break;
                }
            }
        }
        int count;
        for (int j = 0; j < SIGMA_NUM; ++j) {
            count = 0;
            for (int k = 0; k < length; ++k) {
                if (SIGMA[j] == sequences[i][k]) {
                    ++count;
                }
                if (count + step >= sigmaIndexes[j].size()) {
                    sucTbl[i][j][k] = UINT16_MAX;
                } else {
                    sucTbl[i][j][k] = sigmaIndexes[j][count + step];
                }
                if (sucTbl[i][j][k] == UINT16_MAX) {
                    for (int l = k + 1; l < length; ++l) {
                        sucTbl[i][j][k] = UINT16_MAX;
                    }
                    continue;
                }
            }
        }
    }
    *sucTbls = sucTbl;
}

// 当前节点的后面section内的最后一个后继
void getSucTblsBySection(uint16_t**** sucTbls, const vector<string>& sequences, const int section) {
    uint16_t*** sucTbl;
    sucTbl = (uint16_t***) malloc(sizeof(uint16_t**) * SEQ_NUM);
    for (int i = 0; i < SEQ_NUM; ++i) {
        sucTbl[i] = (uint16_t**) malloc(sizeof(uint16_t*) * SIGMA_NUM);
        for (int j = 0; j < SIGMA_NUM; ++j) {
            sucTbl[i][j] = (uint16_t*) malloc(sizeof(uint16_t) * sequences[i].length());
        }
    }

    for (int i = 0; i < SEQ_NUM; ++i) {
        int length = sequences[i].length() - 1;
        bool flag;
        int end;
        for (int j = 0; j < SIGMA_NUM; ++j) {
            for (int k = 0; k < length; ++k) {
                flag = false;
                if (k + section <= length) {
                    end = k + section;
                } else {
                    end = length;
                }
                for (int l = end; l > k; --l) {
                    if (sequences[i][l] == SIGMA[j]) {
                        sucTbl[i][j][k] = l;
                        flag = true;
                        break;
                    }
                }
                if (!flag) {
                    sucTbl[i][j][k] = UINT16_MAX;
                }
            }
            sucTbl[i][j][length] = UINT16_MAX;
        }
    }
    *sucTbls = sucTbl;
}

// 生成某个匹配点的后继点，最多SIGMA_NUM个后继点,同时返回每个匹配点的sum和
void getSucPointsWithSum(vector<vector<uint16_t>>& sucPoints, vector<uint32_t>& sums,
                         const vector<uint16_t>& matchPoint, uint16_t*** sucTbls) {
    int count = 0;
    vector<uint16_t> sucPoint;
    sucPoint.reserve(SEQ_NUM);
    for (int i = 0; i < SIGMA_NUM; ++i) {
        sucPoint.clear();
        uint16_t index;
        uint32_t sum = 0;
        for (int j = 0; j < SEQ_NUM; ++j) {
            index = sucTbls[j][i][matchPoint[j]];
            sum += index;
            if (index == UINT16_MAX) {
                ++count;
                break;
            } else {
                sucPoint.emplace_back(index);
            }
        }
        if (index != UINT16_MAX) {
            sucPoints.emplace_back(sucPoint);
            sums.emplace_back(sum);
        }
    }
    if (count == SIGMA_NUM) {
        sucPoints.emplace_back(vector<uint16_t>(SEQ_NUM, UINT16_MAX));
        sums.emplace_back(SEQ_LEN_SUM + 1);
    }
}

// 生成某个匹配点的后继点，最多SIGMA_NUM个后继点
void getSucPoints(vector<vector<uint16_t>>& sucPoints, const vector<uint16_t>& matchPoint, uint16_t*** sucTbls) {
    vector<uint16_t> sucPoint;
    sucPoint.reserve(SEQ_NUM);
    int count = 0;
    for (int i = 0; i < SIGMA_NUM; ++i) {
        sucPoint.clear();
        uint16_t index;
        for (int j = 0; j < SEQ_NUM; ++j) {
            index = sucTbls[j][i][matchPoint[j]];
            if (index == UINT16_MAX) {
                ++count;
                break;
            } else {
                sucPoint.emplace_back(index);
            }
        }
        if (index != UINT16_MAX) {
            sucPoints.emplace_back(sucPoint);
        }
    }
    if (count == SIGMA_NUM) {
        sucPoints.emplace_back(vector<uint16_t>(SEQ_NUM, UINT16_MAX));
    }
}

// sum小优先,然后是差值小优先
int selectOnePoint(vector<vector<uint16_t>>& sucPoints) {
    int size = sucPoints.size();
    uint32_t minSum = UINT32_MAX;
    vector<int> indexs;
    indexs.reserve(size);
    vector<uint32_t> sums(size, 0);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < SEQ_NUM; ++j) {
            sums[i] += sucPoints[i][j];
        }
        if (minSum == sums[i]) {
            indexs.emplace_back(i);
        } else if (minSum > sums[i]) {
            minSum = sums[i];
            indexs.clear();
            indexs.emplace_back(i);
        }
    }
    if (indexs.size() == 1) {
        return indexs[0];
    } else {
        double avg = (double) sums[indexs[0]] / SEQ_NUM;;
        double dis = DBL_MAX;
        int index;
        for (int i = indexs.size() - 1; i >= 0; --i) {
            int index_temp = indexs[i];
            double temp;
            for (int j = 0; j < SEQ_NUM; ++j) {
                temp += abs(sucPoints[index_temp][j] - avg);
            }
            if (dis > temp) {
                dis = temp;
                index = i;
            }
        }
        return index;
    }
}

// 打印后继表,测试使用
void printSucTbls(uint16_t*** sucTbls, const vector<string>& sequences) {
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
        for (int j = 0; j < SIGMA_NUM; ++j) {
            cout << SIGMA[j] << "\t";
            for (int k = 0; k < sequences[i].length(); ++k) {
                if (sucTbls[i][j][k] == UINT16_MAX) {
                    cout << -1 << "\t";
                } else {
                    cout << sucTbls[i][j][k] << "\t";
                }
            }
            cout << endl;
        }
        cout << "-------------------------------------------------------------------" << endl;
    }
}

