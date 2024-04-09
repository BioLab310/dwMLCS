// Created by D_Kuan

#include "basic.h"
#include "lower.h"
#include "dwMLCS.h"

int SEQ_NUM; // 序列条数
int SIGMA_NUM; // 字符集大小
uint32_t SEQ_LEN_SUM; // 序列长度总和
int MIN_LEN = UINT16_MAX; // 获得序列的最小长度

string SIGMA; // 字符集集合

string READ_PATH = "./input.txt"; // 默认输入路径
string WRITE_PATH = "./mlcs.txt"; // 默认输出路径

long WEIGHT_TIME_BOUND = LONG_MAX; // 求某一个权重的时间限制
clock_t WEIGHT_START_TIME; // 某个权重下界开始时间

int main(int argc, char* argv[]) {
    clock_t start = clock();

    int strategy; // 选择策略
    int num; // 对应于文章后继表部分的α和β

    // 选择运行那种方法：
    // A：dwMLCS的整体算法
    // B：自动求下界，不需要指定策略和参数
    // C: 指定某种策略求下界
    char method;

    for (int i = 1; i < argc; i += 2) {
        // -D参数代表读取序列的个数
        if (argv[i][0] == '-' && argv[i][1] == 'D') {
            SEQ_NUM = stoi(argv[i + 1]);
        }
        // -R参数代表输入路径
        else if (argv[i][0] == '-' && argv[i][1] == 'R') {
            READ_PATH = argv[i + 1];
        }
        // -W参数代表输出路径
        else if (argv[i][0] == '-' && argv[i][1] == 'W') {
            WRITE_PATH = argv[i + 1];
        }
        // 其中-S和-N必须同时传参，如果不指定-S和-N则使用经验计算下界
        // -S参数代表选择的下界策略
        else if (argv[i][0] == '-' && argv[i][1] == 'S') {
            strategy = stoi(argv[i + 1]);
        }
        // -N参数代表选择的策略以及对应的step或者section
        else if (argv[i][0] == '-' && argv[i][1] == 'N') {
            num = stoi(argv[i + 1]);
        }
        // -M参数代表选择是求mlcs还是求下界
        else if (argv[i][0] == '-' && argv[i][1] == 'M') {
            method = argv[i + 1][0];
        }
    }

    // 输入序列
    vector<string> sequences;
    readSeq(sequences);

    uint16_t*** sucTbls;
    getSucTbls(&sucTbls, sequences);
    // dwMLCS的整体算法
    if (method == 'A') {
        clock_t lowerStartTime = clock();
        int lowerLen = quickLower(sucTbls);
        WEIGHT_TIME_BOUND = 240000000; // 某个权重计算最多4分钟
        int nums[4] = {2, 3, 8, 10};
        for (const int step: nums) {
            int curLowerLen = computeLower(sequences, 1, step);
            if (curLowerLen == -2) {
                break;
            }
            lowerLen = max<int>(lowerLen, computeLower(sequences, 1, step));
        }
        WEIGHT_TIME_BOUND = 5000000;
        if (clock() - lowerStartTime < 60000000) {
            lowerLen = max<int>(lowerLen, computeLower(sequences, 2, 60));
        }
        cout << "Lower bound length:" << lowerLen << endl;
        cout << "Lower bound time cost:" << (clock() - lowerStartTime) / 1000 << "ms" << endl;

        dwMLCS(sucTbls, sequences, lowerLen);

        cout << "Total time cost:" << (clock() - start) / 1000 << "ms" << endl;
    }
    // 自动求下界，不需要指定策略和参数
    else if (method == 'B') {
        clock_t lowerStartTime = clock();
        int lowerLen = quickLower(sucTbls);
        WEIGHT_TIME_BOUND = 240000000; // 某个权重计算最多4分钟
        int nums[4] = {2, 3, 8, 10};
        for (const int step: nums) {
            lowerLen = max<int>(lowerLen, computeLower(sequences, 1, step));
        }
        WEIGHT_TIME_BOUND = 5000000;
        if (clock() - lowerStartTime < 60000000) {
            lowerLen = max<int>(lowerLen, computeLower(sequences, 2, 60));
        }
        cout << "Lower bound length:" << lowerLen << endl;
        cout << "Lower bound time cost:" << (clock() - lowerStartTime) / 1000 << "ms" << endl;
    }
    // 指定某种策略求下界，需要传入适当的α或者β大小，否则计算下界的时间会很长
    else if (method == 'C') {
        clock_t lowerStartTime = clock();
        WEIGHT_TIME_BOUND = 240000000; // 某个权重计算最多4分钟
        int lowerLen = computeLower(sequences, strategy, num);
        if (lowerLen == -2) {
            cout << "The lower bound cannot be calculated based on the parameters passed in the limited time or limited memory."
                    "Please change the parameter size and try again." << endl;
        } else {
            cout << "Lower bound length:" << lowerLen << endl;
            cout << "Lower bound time cost:" << (clock() - lowerStartTime) / 1000 << "ms" << endl;
        }
    }
    return 0;
}
