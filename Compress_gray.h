#pragma once
#include "RBlock.h"
#include "Bits.h"
#include <windows.h>

class Compress_gray
{
public:

	//单色原图像
	cv::Mat img;

	bool isColorful;

public:

	//构造函数
	Compress_gray();

	//带参构造函数 (压缩用）
	void init_compress(const cv::Mat& imge, const double& sigma_ = 10);

	//带参构造函数（解压用）
	void init_uncompress(const std::string filename, const int& size = 2);

	//析构函数
	~Compress_gray();

	//计算PSNR
	static double getPSNR(cv::Mat img1, cv::Mat img2);

	//压缩
	void compress();

	//解压
	cv::Mat& uncompress();

	//保存
	void save(const std::string& filename = "compressed.nam");

	//获取读取长度
	long long getReadLen() const;

	//获取写入长度
	long long getWriteLen() const;

	//获取时间
	double getTime()const;

	//获取压缩率
	double getRate()const;

protected:

	//结果图像
	cv::Mat result;

	//尺寸
	int rows , cols;

	//编码后的图像
	uchar** encoded_img;

	//二进制码
	Bits data;

	//解压后的块的集合
	std::vector<RBlock> allBlocks;

	//判断矩阵，用于标记已经编码的区域
	bool** isMarked;

	//判断阈值
	int sigma;

	//时间
	LARGE_INTEGER time[2];

protected:

	//设置编码后的图像
	uchar& setEncodedImg(cv::Point&);

	//判断两点是否是配对的对角点
	bool isPeer(const cv::Point& lt, const cv::Point& rb);

	//对给定的左上角坐标，寻找右下角坐标(编码用）
	cv::Point findRb(const cv::Point& lt);

	// 对给定的左上角坐标，寻找一个矩形（解码用）
	RBlock findBlock(const cv::Point& start, int& blockNum);

	//编码
	std::vector<RBlock> encode();

	//解码
	std::vector<RBlock> decode();

	//delete任意类型矩阵
	template<typename t> void deleteMatrix(t**&, const int& len1);

	//new任意类型矩阵
	template<typename t> void newMatrix(t**&, const int& len1, const int& len2, t init = 0);

	//获取图像深度
	int getDepth();
};

