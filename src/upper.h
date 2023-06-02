// Created by dekuan

#include "basic.h"

class UpperNode {
public:
    uint16_t upperLen;
    uint16_t idx;
    char sigma;

    UpperNode() {}

    UpperNode(uint16_t upperLen, uint16_t idx, char sigma)
            : upperLen(upperLen), idx(idx), sigma(sigma) {}
};

// 大根堆
struct cmp {
    bool operator()(const UpperNode& o1, const UpperNode& o2) {
        return o1.upperLen < o2.upperLen;
    }
};

// 只对最大的两条序列求mlcs长度，剩下的用上界表的min，最后加起来
int computeUpper_1(const vector<uint16_t>& sucPoint, const vector<string>& sequences, uint16_t*** upperTbls);

int computeUpper_2(const vector<uint16_t>& sucPoint, uint16_t*** upperTbls);

void printUpperTbls(uint16_t*** upperTbls, const vector<string>& sequences);