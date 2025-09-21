#include "Bits.h"
#include <iostream>

Bits::Bits() {
    bits.emplace_back(0);
    vecLen = 0;
    index = -1;
    readLen = 0;
}

//读取文件内容初始化Bits类(解压时)
void Bits::init(const std::string& filename, const int& size) {
    std::ifstream in(filename, std::ios::binary | std::ios::in);
    loadedInfo = new char[pow(1024, size) * 10];
    in >> loadedInfo;
    in.close();
    readLen = strlen(loadedInfo);
    vecLen = index =  0;
    readIndex = -1;
}

//析构函数，释放内存
Bits::~Bits() {
    if (loadedInfo) 
		delete[] loadedInfo;
}

//将int类型长度为len的info转化为二进制，并写入bits
void Bits::write(const int& info, const int& len) {
    for (int i = len - 1; i >= 0; i--) {
        if (info & (1 << i))
            add(1);
        else
            add(0);
    }
}


//在bits中添加一位1或0
void Bits::add(const std::bitset<1>& bit) {
    if (++index == PERLEN) {
        bits.emplace_back(0);
        vecLen++;
        index = 0;
    }
    bits[vecLen][index] = bit[0];
}

//将bits中的二进制内容保存为名为filename的文件
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

//读取bits中的长度为len的二进制内容，转化为int类型
int Bits::read(const int& len) {
    int decNum = 0;
    for (int i = 0; i < len; i++) {
        if (loadedInfo[++readIndex] == '1')
            decNum += 1 << (len - i - 1);
    }
    return decNum;
}