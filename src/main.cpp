// Created by D_Kuan

#include "basic.h"
#include "lower.h"
#include "dwMLCS.h"

int SEQ_NUM; // ��������
int SIGMA_NUM; // �ַ�����С
uint32_t SEQ_LEN_SUM; // ���г����ܺ�
int MIN_LEN = UINT16_MAX; // ������е���С����

string SIGMA; // �ַ�������

string READ_PATH = "./input.txt"; // Ĭ������·��
string WRITE_PATH = "./mlcs.txt"; // Ĭ�����·��

long WEIGHT_TIME_BOUND = LONG_MAX; // ��ĳһ��Ȩ�ص�ʱ������
clock_t WEIGHT_START_TIME; // ĳ��Ȩ���½翪ʼʱ��

int main(int argc, char* argv[]) {
    clock_t start = clock();

    int strategy; // ѡ�����
    int num; // ��Ӧ�����º�̱��ֵĦ��ͦ�

    // ѡ���������ַ�����
    // A��dwMLCS�������㷨
    // B���Զ����½磬����Ҫָ�����ԺͲ���
    // C: ָ��ĳ�ֲ������½�
    char method;

    for (int i = 1; i < argc; i += 2) {
        // -D���������ȡ���еĸ���
        if (argv[i][0] == '-' && argv[i][1] == 'D') {
            SEQ_NUM = stoi(argv[i + 1]);
        }
        // -R������������·��
        else if (argv[i][0] == '-' && argv[i][1] == 'R') {
            READ_PATH = argv[i + 1];
        }
        // -W�����������·��
        else if (argv[i][0] == '-' && argv[i][1] == 'W') {
            WRITE_PATH = argv[i + 1];
        }
        // ����-S��-N����ͬʱ���Σ������ָ��-S��-N��ʹ�þ�������½�
        // -S��������ѡ����½����
        else if (argv[i][0] == '-' && argv[i][1] == 'S') {
            strategy = stoi(argv[i + 1]);
        }
        // -N��������ѡ��Ĳ����Լ���Ӧ��step����section
        else if (argv[i][0] == '-' && argv[i][1] == 'N') {
            num = stoi(argv[i + 1]);
        }
        // -M��������ѡ������mlcs�������½�
        else if (argv[i][0] == '-' && argv[i][1] == 'M') {
            method = argv[i + 1][0];
        }
    }

    // ��������
    vector<string> sequences;
    readSeq(sequences);

    uint16_t*** sucTbls;
    getSucTbls(&sucTbls, sequences);
    // dwMLCS�������㷨
    if (method == 'A') {
        clock_t lowerStartTime = clock();
        int lowerLen = quickLower(sucTbls);
        WEIGHT_TIME_BOUND = 240000000; // ĳ��Ȩ�ؼ������4����
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
    // �Զ����½磬����Ҫָ�����ԺͲ���
    else if (method == 'B') {
        clock_t lowerStartTime = clock();
        int lowerLen = quickLower(sucTbls);
        WEIGHT_TIME_BOUND = 240000000; // ĳ��Ȩ�ؼ������4����
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
    // ָ��ĳ�ֲ������½磬��Ҫ�����ʵ��Ħ����ߦ´�С����������½��ʱ���ܳ�
    else if (method == 'C') {
        clock_t lowerStartTime = clock();
        WEIGHT_TIME_BOUND = 240000000; // ĳ��Ȩ�ؼ������4����
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
