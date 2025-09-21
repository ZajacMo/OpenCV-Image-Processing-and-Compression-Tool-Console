#pragma once
#include "RBlock.h"
#include "Bits.h"
#include <windows.h>

class Compress_gray
{
public:

	//��ɫԭͼ��
	cv::Mat img;

	bool isColorful;

public:

	//���캯��
	Compress_gray();

	//���ι��캯�� (ѹ���ã�
	void init_compress(const cv::Mat& imge, const double& sigma_ = 10);

	//���ι��캯������ѹ�ã�
	void init_uncompress(const std::string filename, const int& size = 2);

	//��������
	~Compress_gray();

	//����PSNR
	static double getPSNR(cv::Mat img1, cv::Mat img2);

	//ѹ��
	void compress();

	//��ѹ
	cv::Mat& uncompress();

	//����
	void save(const std::string& filename = "compressed.nam");

	//��ȡ��ȡ����
	long long getReadLen() const;

	//��ȡд�볤��
	long long getWriteLen() const;

	//��ȡʱ��
	double getTime()const;

	//��ȡѹ����
	double getRate()const;

protected:

	//���ͼ��
	cv::Mat result;

	//�ߴ�
	int rows , cols;

	//������ͼ��
	uchar** encoded_img;

	//��������
	Bits data;

	//��ѹ��Ŀ�ļ���
	std::vector<RBlock> allBlocks;

	//�жϾ������ڱ���Ѿ����������
	bool** isMarked;

	//�ж���ֵ
	int sigma;

	//ʱ��
	LARGE_INTEGER time[2];

protected:

	//���ñ�����ͼ��
	uchar& setEncodedImg(cv::Point&);

	//�ж������Ƿ�����ԵĶԽǵ�
	bool isPeer(const cv::Point& lt, const cv::Point& rb);

	//�Ը��������Ͻ����꣬Ѱ�����½�����(�����ã�
	cv::Point findRb(const cv::Point& lt);

	// �Ը��������Ͻ����꣬Ѱ��һ�����Σ������ã�
	RBlock findBlock(const cv::Point& start, int& blockNum);

	//����
	std::vector<RBlock> encode();

	//����
	std::vector<RBlock> decode();

	//delete�������;���
	template<typename t> void deleteMatrix(t**&, const int& len1);

	//new�������;���
	template<typename t> void newMatrix(t**&, const int& len1, const int& len2, t init = 0);

	//��ȡͼ�����
	int getDepth();
};

