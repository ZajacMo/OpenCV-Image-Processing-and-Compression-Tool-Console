#include "Block.h"
Block::Block()
{
	//Ϊ��ɫ��͸���ȸ���ֵ
	color[0][0] = color[0][1] = color[1][0] = color[1][1] = -1;
}

Block::Block(const cv::Point lt, const cv::Point rb)
{
	//Ϊ���ϽǺ����½Ǹ���ֵ
	this->lt = lt;
	this->rb = rb;
}

Block::~Block()
{

}