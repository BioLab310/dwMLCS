// Created by D_Kuan

#include "basic.h"
#include "upper.h"

// dwMLCS算法
void dwMLCS(uint16_t*** sucTbls, const vector<string>& sequences, int lowerLen);

// 输出最终的MLCS
void printMlcs(vector<string>& mlcs);
