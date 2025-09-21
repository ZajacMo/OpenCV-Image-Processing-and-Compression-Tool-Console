#include "Bits.h"
#include <iostream>

Bits::Bits() {
    bits.emplace_back(0);
    vecLen = 0;
    index = -1;
    readLen = 0;
}

//��ȡ�ļ����ݳ�ʼ��Bits��(��ѹʱ)
void Bits::init(const std::string& filename, const int& size) {
    std::ifstream in(filename, std::ios::binary | std::ios::in);
    loadedInfo = new char[pow(1024, size) * 10];
    in >> loadedInfo;
    in.close();
    readLen = strlen(loadedInfo);
    vecLen = index =  0;
    readIndex = -1;
}

//�����������ͷ��ڴ�
Bits::~Bits() {
    if (loadedInfo) 
		delete[] loadedInfo;
}

//��int���ͳ���Ϊlen��infoת��Ϊ�����ƣ���д��bits
void Bits::write(const int& info, const int& len) {
    for (int i = len - 1; i >= 0; i--) {
        if (info & (1 << i))
            add(1);
        else
            add(0);
    }
}


//��bits�����һλ1��0
void Bits::add(const std::bitset<1>& bit) {
    if (++index == PERLEN) {
        bits.emplace_back(0);
        vecLen++;
        index = 0;
    }
    bits[vecLen][index] = bit[0];
}

//��bits�еĶ��������ݱ���Ϊ��Ϊfilename���ļ�
void Bits::save(const std::string& filename) {
    std::ofstream out(filename, std::ios::binary | std::ios::trunc | std::ios::out);
    for (int i = 0; i < vecLen ; i++) {
        for (int j = 0; j < PERLEN; j++)
            out << bits[i][j];
    }
    for (int i = 0; i <= index; i++) {
        out << bits[vecLen][i];
    }
    out.close();
}

//��ȡbits�еĳ���Ϊlen�Ķ��������ݣ�ת��Ϊint����
int Bits::read(const int& len) {
    int decNum = 0;
    for (int i = 0; i < len; i++) {
        if (loadedInfo[++readIndex] == '1')
            decNum += 1 << (len - i - 1);
    }
    return decNum;
}