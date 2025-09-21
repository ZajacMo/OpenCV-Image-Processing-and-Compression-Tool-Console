#pragma once
#include <opencv2/opencv.hpp>
class Block
{
	friend class Compress_gray;
protected:

	//左上角和右下角坐标
	cv::Point lt, rb;

	//每个区域记录的颜色
	uchar color[2][2];

public:

	Block();

	Block(const cv::Point lt, const cv::Point rb);

	~Block();

	virtual bool isBlock(const cv::Mat& img, bool** const isMarked, const int& xgm) const = 0;

	virtual void mark(bool** isMarked) const = 0;

};

