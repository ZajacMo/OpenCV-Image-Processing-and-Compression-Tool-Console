#pragma once
#include <bitset>
#include <vector>
#include <fstream>
#include <string.h>
#include <cmath>
#define PERLEN 1024//ÿ��vector�д洢�Ķ����Ƴ���

class Bits {

    friend class Compress_gray;

private:

    //�洢�Ķ���������
    std::vector<std::bitset<PERLEN>> bits;

    //��ȡ�Ķ���������
    char* loadedInfo;

protected:

    //bits�е�vector����
    int vecLen, index;

    //��ȡʱ�������ı��ܳ���
    long long readLen;

    long long readIndex;

protected:

    Bits();

    //�����������ͷ��ڴ�
    ~Bits();

    //��ȡ�ļ����ݳ�ʼ��Bits��
    void init(const std::string& filename, const int& size = 2);

    //��int���ͳ���Ϊlen��infoת��Ϊ�����ƣ���д��bits
    void write(const int& info, const int& len);

    //��bits�����һλ1��0
    void add(const std::bitset<1>& bit);

    //��bits�еĶ��������ݱ���Ϊ��Ϊfilename���ļ�
    void save(const std::string& filename);

    //��ȡbits�еĳ���Ϊlen�Ķ��������ݣ�ת��Ϊint����
    int read(  const int& len);

};

