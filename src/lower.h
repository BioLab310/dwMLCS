// Created by D_Kuan

#include "basic.h"

class WeightNode {
public:
    vector<uint16_t> matchPoint; // 匹配点
    vector<uint32_t> idOfSucNodes; // 后继节点id
    int indegree; // 入度
    int curLen;

    WeightNode() {}

    WeightNode(const vector<uint16_t>& matchPoint, int indegree, int curLength)
            : matchPoint(matchPoint), indegree(indegree), curLen(curLength) {}
};

// 大根堆
struct cmp1 {
    bool operator()(const vector<uint16_t>& v1, const vector<uint16_t>& v2) {
        uint32_t sum1 = 0;
        uint32_t sum2 = 0;
        for (uint16_t num: v1) sum1 += num;
        for (uint16_t num: v2) sum2 += num;
        return sum1 < sum2;
    }
};

// 大根堆
struct cmp2 {
    bool operator()(const pair<vector<uint16_t>, int>& p1,
                    const pair<vector<uint16_t>, int>& p2) {
        return p1.second < p2.second;
    }
};

// 计算权重序列的权重
int computeWeight(boost::unordered_map<vector<uint16_t>, uint32_t>& hashTable, vector<WeightNode>& DAG,
                  uint32_t& maxId, vector<uint32_t>& idOfCurExtendNodes, vector<uint32_t>& idOfNextExtendNodes,
                  const vector<string>& weightSequences);

// 通过权重计算下界，每次向后扩展zeta(默认256)个节点
int computeLower(const vector<string>& sequences, const int strategy, const int num, int zeta = 256);

// 近似下界,每次之产生一个后继点向后迭代计算
// Big-MLCS
int bigMlcs(uint16_t*** sucTbls);

// 近似下界，每次产生theta个后继点向后迭代计算
// BEST-MLCS
// 每层中的节点个数默认设置为256个
int bestMlcs(uint16_t*** sucTbls, int theta = 256);

// 近似下界，每次产生t个后继点向后迭代计算
// mini-MLCS
// t为初始值，mu为步长，tau为层级节点个数最大值
int miniMlcs(uint16_t*** sucTbls, int t, int mu, int tau);
