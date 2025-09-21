#include "Block.h"
Block::Block()
{
	//为颜色和透明度赋初值
	color[0][0] = color[0][1] = color[1][0] = color[1][1] = -1;
}

Block::Block(const cv::Point lt, const cv::Point rb)
{
	//为左上角和右下角赋初值
	this->lt = lt;
	this->rb = rb;
}

Block::~Block()
{

}