#include "RBlock.h"

RBlock::RBlock() {
}

RBlock::RBlock(const cv::Point lt, const cv::Point rb, uchar p) :Block(lt, rb) {
	color[0][0] = p;
}
RBlock::RBlock(const cv::Point lt, const cv::Point rb, uchar** p) :Block(lt, rb) {
	if (p)
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				color[i][j] = p[i][j];
}

RBlock::RBlock(const cv::Mat& img, const cv::Point lt, const cv::Point rb) :Block(lt, rb) {
	color[0][0] = img.at<uchar>(lt.x, lt.y);
	color[0][1] = img.at<uchar>(lt.x, rb.y);
	color[1][0] = img.at<uchar>(rb.x, lt.y);
	color[1][1] = img.at<uchar>(rb.x, rb.y);
}

inline uchar RBlock::getMedian(const uchar& a, const uchar& b, const double rate) const {
	return a + (b - a) * rate;
}

inline double RBlock::getGamma(const int& a, const int& b, const int& i) const {
	if (b == a)
		return 0;
	return (i - a) / (double)(b - a);
}

bool RBlock::isBlock(const cv::Mat& img, bool** const isMarked, const  int& xgm)const {
	//获取四个角的颜色
	uchar point_color[2][3] = { {img.at<uchar>(lt.x, lt.y),					img.at<uchar>(lt.x, rb.y)},
														{img.at<uchar>(rb.x, lt.y),			img.at<uchar>(rb.x, rb.y) } };

	//遍历每一行
	for (int i = lt.x; i <= rb.x; i++) {

		//计算i在lt.x和rb.x之间的比例
		double gamma_i = getGamma(lt.x, rb.x, i);

		//计算i在lt和lb之间的颜色
		point_color[0][2] = getMedian(point_color[0][0], point_color[1][0], gamma_i);

		//计算i在rt和rb之间的颜色
		point_color[1][2] = getMedian(point_color[0][1], point_color[1][1], gamma_i);

		//遍历每一列
		for (int j = lt.y; j <= rb.y; j++) {

			//如果这个点已经被标记，就返回false
			if (isMarked[i][j])
				return false;

			//计算j在lt.y和rb.y之间的比例
			double gamma_j = getGamma(lt.y, rb.y, j);

			//计算j在两中间点之间的颜色
			double gest = getMedian(point_color[0][2], point_color[1][2], gamma_j);

			//如果颜色差距大于xgm，就返回false
			if (abs(img.at<uchar>(i, j) - gest) > xgm)
				return false;
		}
	}
	return true;
}

void RBlock::mark(bool** isMarked) const {
	for (int i = lt.x; i <= rb.x; i++)
		for (int j = lt.y; j <= rb.y; j++)
			isMarked[i][j] = true;
}



void RBlock::regain(cv::Mat& img, uchar** encoded_img) {
	if (lt == rb)
	{
		img.at<uchar>(lt.x, lt.y) = color[0][0];
		return;
	}
	uchar point_color[2][3] = { {color[0][0], color[0][1]},
														 {color[1][0], color[1][1]}};
	//遍历每一行
	for (int i = lt.x; i <= rb.x; i++) {

		//计算i在lt.x和rb.x之间的比例
		double gamma_i = getGamma(lt.x, rb.x, i);

		//计算i在lt和lb之间的颜色
		point_color[0][2] = getMedian(point_color[0][0], point_color[1][0], gamma_i);

		//计算i在rt和rb之间的颜色
		point_color[1][2] = getMedian(point_color[0][1], point_color[1][1], gamma_i);

		//遍历每一列
		for (int j = lt.y; j <= rb.y; j++) {

			//计算j在lt.y和rb.y之间的比例
			double gamma_j = getGamma(lt.y, rb.y, j);

			//计算j在两中间点之间的颜色
			img.at<uchar>(i, j) = getMedian(point_color[0][2], point_color[1][2], gamma_j);
		}
	}
}