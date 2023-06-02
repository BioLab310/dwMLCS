// Created by dekuan

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <algorithm>
#include <queue>
#include <time.h>
#include <cfloat>
#include <algorithm>

using namespace std;

extern int SEQ_NUM; // 序列条数
extern int SIGMA_NUM; // 字符集大小
extern uint32_t SEQ_LEN_SUM; // 序列长度总和
extern int MIN_LEN; // 获得序列的最小长度

extern string SIGMA; // 字符集集合
extern string READ_PATH; // 默认输入路径
extern string WRITE_PATH; // 默认输出路径

extern long WEIGHT_TIME_BOUND; // 求某一个权重的时间限制
extern clock_t WEIGHT_START_TIME; // 某个权重下界开始时间

// 读取d条序列 返回序列长度和(不算空格的长度)
void readSeq(vector<string>& sequences);

// 获得上界表
void getUpperTbls(uint16_t**** upperTbls, const vector<string>& sequences);

// 获得普通后继表
void getSucTbls(uint16_t**** sucTbls, const vector<string>& sequences);

// 当前节点的第step个后继
void getSucTblsByStep(uint16_t**** sucTbls, const vector<string>& sequences, const int step);

// 当前节点的后面section内的最后一个后继
void getSucTblsBySection(uint16_t**** sucTbls, const vector<string>& sequences, const int section);

// 生成某个匹配点的后继点，最多r个后继点,同时返回每个匹配点的sum和
void getSucPointsWithSum(vector<vector<uint16_t>>& sucPoints, vector<uint32_t>& sums,
                         const vector<uint16_t>& matchPoint, uint16_t*** sucTbls);

// 生成某个匹配点的后继点，最多r个后继点
void getSucPoints(vector<vector<uint16_t>>& sucPoints, const vector<uint16_t>& matchPoint, uint16_t*** sucTbls);

// sum小优先,然后是差值小优先
int selectOnePoint(vector<vector<uint16_t>>& sucPoints);

void printSucTbls(uint16_t*** sucTbls, const vector<string>& sequences);