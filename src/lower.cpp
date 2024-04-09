// Created by D_Kuan

#include "lower.h"

// ����Ȩ�����е�Ȩ��
int computeWeight(boost::unordered_map<vector<uint16_t>, uint32_t>& hashTable, vector<WeightNode>& DAG,
                  uint32_t& maxId, vector<uint32_t>& idOfCurExtendNodes, vector<uint32_t>& idOfNextExtendNodes,
                  const vector<string>& weightSequences) {
    // ��ʼ��
    hashTable.clear();
    DAG.clear();
    maxId = 0;
    idOfCurExtendNodes.clear();
    idOfNextExtendNodes.clear();

    vector<uint16_t> sourcePoint(SEQ_NUM, 0);
    hashTable.emplace(sourcePoint, maxId++); // ��Դ�ڵ���ӵ�hashtable��
    DAG.emplace_back(sourcePoint, 0, 0); // ��Դ�ڵ���ӵ�DAGͼ,�����λ��

    vector<uint16_t> endPoint(SEQ_NUM, UINT16_MAX);
    hashTable.emplace(endPoint, maxId++); // �սڵ���ӵ�hashtable��
    DAG.emplace_back(endPoint, 0, 0); // �սڵ���ӵ�DAG��,��һ��λ��

    // �ȴ�Դ�ڵ㿪ʼ��չ��Դ�ڵ����������λ�ã��սڵ���һ������λ��
    idOfCurExtendNodes.emplace_back(0);

    // ��ú�̱�
    uint16_t*** sucTbls;
    getSucTbls(&sucTbls, weightSequences);

    vector<vector<uint16_t>> sucPoints;
    sucPoints.reserve(SIGMA_NUM);

    while (!idOfCurExtendNodes.empty()) {
        idOfNextExtendNodes.clear();

        for (const auto& curNodeId: idOfCurExtendNodes) {

            // �������һ��ʱ����޲��ԣ���������趨��WEIGHT_TIME_BOUND����(�ڴ�С��1.5G���ڴ�ʣ�಻��10%)
            if (clock() - WEIGHT_START_TIME > WEIGHT_TIME_BOUND || (getFreeMemory() <= 1536 && getMemoryPercentage() < 0.1)) {

                // �ͷź�̱�
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
                if (sucNodeIter == hashTable.end()) { // ��hashtable�в�����
                    // ���µ�ǰ�ڵ�ĺ��
                    DAG[curNodeId].idOfSucNodes.emplace_back(maxId);
                    idOfNextExtendNodes.emplace_back(maxId);
                    // ���ú�̽ڵ���ӵ�DAGͼ�У���ӵ�DAG��ĩβ
                    DAG.emplace_back(sucPoint, 1, 0);
                    // ���ú�̽ڵ���ӵ�hashtable��
                    hashTable.emplace(sucPoint, maxId);
                    ++maxId;
                } else { // ��hashtable�д���
                    // ���µ�ǰ�ڵ�ĺ�̺ͺ�̽ڵ�����
                    ++(DAG[sucNodeIter->second].indegree);
                    DAG[curNodeId].idOfSucNodes.emplace_back(sucNodeIter->second);
                }
            }
        }
        idOfCurExtendNodes = idOfNextExtendNodes;
    }

    // ������DAGͼ֮����Ҫͨ��ͼ�Ĳ�α�����ÿ���ڵ��curLen
    queue<uint32_t> Q;
    Q.push(0); // ��Դ�ڵ㿪ʼ����DAG
    while (!Q.empty()) {

        // �������һ��ʱ����޲��ԣ���������趨��WEIGHT_TIME_BOUND����(�ڴ�С��1.5G���ڴ�ʣ�಻��10%)
        if (clock() - WEIGHT_START_TIME > WEIGHT_TIME_BOUND || (getFreeMemory() <= 1536 && getMemoryPercentage() < 0.1)) {

            // �ͷź�̱�
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
                // ��������ʱ��Ҳ���յ�����һ���ڵ��ˣ�Ҳ����һ�ˣ�������󳤶���Ҫ��һ
                DAG[sucNodeId].curLen = max<int>(DAG[sucNodeId].curLen, DAG[curNodeId].curLen + 1);
                if (sucNodeId != 1) {
                    Q.push(sucNodeId);
                }
            }
        }
    }

    // �ͷź�̱�
    for (int i = 0; i < SEQ_NUM; ++i) {
        for (int j = 0; j < SIGMA_NUM; ++j) {
            free(sucTbls[i][j]);
        }
        free(sucTbls[i]);
    }
    free(sucTbls);

    return DAG[1].curLen - 1; // �������սڵ�ĳ���һ
}

// ͨ��Ȩ�ؼ����½磬ÿ�������չzeta���ڵ�
// ÿ��ѡȡǰzeta��sumС��
int computeLower(const vector<string>& sequences, const int strategy, const int num, int zeta) {
    WEIGHT_START_TIME = clock();

    // ���Ǽ���Ȩ�����е�Ȩ����Ҫ�Ľṹ
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

    int maxLen = 0; // ���յ��½糤��
    vector<uint16_t> sourcePoint(SEQ_NUM, 0);
    vector<uint16_t> endPoint(SEQ_NUM, UINT16_MAX);
    vector<string> weightSequences;
    weightSequences.reserve(SEQ_NUM);
    vector<vector<uint16_t>> sucPoints;
    sucPoints.reserve(SIGMA_NUM);

    // ���ݲ�ͬ�Ĳ��Ի�ò�ͬ�ĺ�̱�
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
            // ����sum
            int sum;
            for (const auto& sucPoint: sucPoints) {
                // �����ǰ��̵㲻����һ����
                if (nextLevel->find(sucPoint) == nextLevel->end()) {
                    sum = 0;
                    for (uint16_t item: sucPoint) {
                        sum += item;
                    }
                    // �����zeta���Ը��ģ������޶�ÿ�����ֻ��ӵ��zeta���ڵ�
                    if (candidates.size() == zeta) {
                        // �����ǰ��̵�ķ���ͱ���һ���е�ÿ�������ͬʱ��һ��ǡ���Ѿ�zeta���ڵ��ˣ���ֱ������
                        if (sum >= candidates.top().second) continue;
                        // ����ɾ���Ѷ�����󷽲���Ǹ�ƥ��㣬ͬʱ����һ����ɾ����
                        nextLevel->erase(candidates.top().first);
                        candidates.pop();
                        // ͬʱ���ú�̵���ӵ���һ���У�ͬʱ��ӵ����ȼ�������
                        nextLevel->emplace(sucPoint, 0);
                        candidates.emplace(sucPoint, sum);
                    } else { // ��һ��δ��zeta���ڵ㣬��ֱ����ӵ���һ������ȼ�������
                        nextLevel->emplace(sucPoint, 0);
                        candidates.emplace(sucPoint, sum);
                    }
                }
            }
        }

        // ���㵱ǰ�ڵ�ͺ�̵�֮���Ȩ��
        for (const auto& curNode: *curLevel) {
            for (auto& sucNode: *nextLevel) {
                weightSequences.clear();
                int weight = -1;
                int left;
                int right;
                if (sucNode.first == endPoint) { // �����ǰ�ڵ�ĺ�̽ڵ����սڵ�
                    for (int i = 0; i < SEQ_NUM; ++i) {
                        left = curNode.first[i];
                        // ���Ȩ�����е��������Ǹ��ַ�������ԭ���е����һ���ַ�����ôȨ��Ϊ0,��Ϊ��Ҫ��left+1��λ�ÿ�ʼ��ȡȨ������
                        if (left == sequences[i].length() - 1) {
                            weight = 0;
                            break;
                        }
                        weightSequences.emplace_back(" ");
                        // append����������ҿ���
                        // ���ǽ�ȡ���еĴ�left+1��ʼ��right
                        weightSequences[i].append(sequences[i].begin() + left + 1, sequences[i].end());
                    }
                    if (weight == -1) {
                        weight = computeWeight(hashTable, DAG, maxId, idOfCurExtendNodes, idOfNextExtendNodes,
                                               weightSequences);

                        if (weight == -2) { // ����ʱ��
                            // �ͷź�̱�
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
                            weight = -3; // �������ǵ�ǰ�ڵ�ĺ��
                            break;
                        }
                        weightSequences.emplace_back(" ");
                        // append����������ҿ���
                        // ���ǽ�ȡ�����д�left+1��ʼ��right
                        weightSequences[i].append(sequences[i].begin() + left + 1, sequences[i].begin() + right + 1);
                    }
                    if (weight == -1) {
                        weight = computeWeight(hashTable, DAG, maxId, idOfCurExtendNodes, idOfNextExtendNodes,
                                               weightSequences);

                        if (weight == -2) {
                            // �ͷź�̱�
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
        // ��һ��ɾ���յ�
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
    // �ͷź�̱�
    for (int i = 0; i < SEQ_NUM; ++i) {
        for (int j = 0; j < SIGMA_NUM; ++j) {
            free(sucTbls[i][j]);
        }
        free(sucTbls[i]);
    }
    free(sucTbls);
    return maxLen;
}

// ���ټ����½�
int quickLower(uint16_t*** sucTbls, int theta) {
    boost::unordered_set<vector<uint16_t>> level1;
    boost::unordered_set<vector<uint16_t>> level2;
    boost::unordered_set<vector<uint16_t>>* curLevel = &level1;
    boost::unordered_set<vector<uint16_t>>* nextLevel = &level2;
    priority_queue<vector<uint16_t>, vector<vector<uint16_t>>, cmp1> pq; // �����

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
            // ��������ĺ�����յ㣬�򲻷�����һ��
            if (sucPoints[0] == endPoint) {
                continue;
            }
            for (const auto& sucPoint: sucPoints) {
                // �����ǰ��̵㲻����һ����
                if (nextLevel->find(sucPoint) == nextLevel->end()) {
                    // ���㵱ǰ��̵��sum��
                    uint32_t sucSum = 0;
                    for (uint16_t num: sucPoint) sucSum += num;
                    if (pq.size() == theta) {
                        // �����ǰ��̵��sum�ͱ���һ���е�ÿ��sum����ͬʱ��һ��ǡ���Ѿ�theta���ڵ��ˣ���ֱ������
                        if (sucSum >= topSum) continue;
                        // ����ɾ���Ѷ������sum���Ǹ�ƥ��㣬ͬʱ����һ����ɾ����
                        nextLevel->erase(pq.top());
                        pq.pop();
                        // ͬʱ���ú�̵���ӵ���һ���У�ͬʱ��ӵ����ȼ�������
                        nextLevel->insert(sucPoint);
                        pq.push(sucPoint);
                        // ͬʱ���¶Ѷ�Ԫ�ص�sum��
                        topSum = 0;
                        for (uint16_t num: pq.top()) topSum += num;
                    } else { // ��һ��δ��theta���ڵ㣬��ֱ����ӵ���һ������ȼ�������
                        nextLevel->insert(sucPoint);
                        pq.push(sucPoint);
                        if (sucSum > topSum) topSum = sucSum;
                    }
                }
                // �����ǰ��̵�����һ���У���ֱ������
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
    // ��ȥԴ���ǲ�
    return len - 1;
}
