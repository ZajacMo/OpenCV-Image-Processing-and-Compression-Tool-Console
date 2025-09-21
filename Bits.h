#pragma once
#include <bitset>
#include <vector>
#include <fstream>
#include <string.h>
#include <cmath>
#define PERLEN 1024//每个vector中存储的二进制长度

class Bits {

    friend class Compress_gray;

private:

    //存储的二进制内容
    std::vector<std::bitset<PERLEN>> bits;

    //读取的二进制内容
    char* loadedInfo;

protected:

    //bits中的vector长度
    int vecLen, index;

    //读取时二进制文本总长度
    long long readLen;

    long long readIndex;

protected:

    Bits();

    //析构函数，释放内存
    ~Bits();

    //读取文件内容初始化Bits类
    void init(const std::string& filename, const int& size = 2);

    //将int类型长度为len的info转化为二进制，并写入bits
    void write(const int& info, const int& len);

    //在bits中添加一位1或0
    void add(const std::bitset<1>& bit);

    //将bits中的二进制内容保存为名为filename的文件
    void save(const std::string& filename);

    //读取bits中的长度为len的二进制内容，转化为int类型
    int read(  const int& len);

};

