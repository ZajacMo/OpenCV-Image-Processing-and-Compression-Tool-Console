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
	//��ȡ�ĸ��ǵ���ɫ
	uchar point_color[2][3] = { {img.at<uchar>(lt.x, lt.y),					img.at<uchar>(lt.x, rb.y)},
														{img.at<uchar>(rb.x, lt.y),			img.at<uchar>(rb.x, rb.y) } };

	//����ÿһ��
	for (int i = lt.x; i <= rb.x; i++) {

		//����i��lt.x��rb.x֮��ı���
		double gamma_i = getGamma(lt.x, rb.x, i);

		//����i��lt��lb֮�����ɫ
		point_color[0][2] = getMedian(point_color[0][0], point_color[1][0], gamma_i);

		//����i��rt��rb֮�����ɫ
		point_color[1][2] = getMedian(point_color[0][1], point_color[1][1], gamma_i);

		//����ÿһ��
		for (int j = lt.y; j <= rb.y; j++) {

			//���������Ѿ�����ǣ��ͷ���false
			if (isMarked[i][j])
				return false;

			//����j��lt.y��rb.y֮��ı���
			double gamma_j = getGamma(lt.y, rb.y, j);

			//����j�����м��֮�����ɫ
			double gest = getMedian(point_color[0][2], point_color[1][2], gamma_j);

			//�����ɫ������xgm���ͷ���false
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
	//����ÿһ��
	for (int i = lt.x; i <= rb.x; i++) {

		//����i��lt.x��rb.x֮��ı���
		double gamma_i = getGamma(lt.x, rb.x, i);

		//����i��lt��lb֮�����ɫ
		point_color[0][2] = getMedian(point_color[0][0], point_color[1][0], gamma_i);

		//����i��rt��rb֮�����ɫ
		point_color[1][2] = getMedian(point_color[0][1], point_color[1][1], gamma_i);

		//����ÿһ��
		for (int j = lt.y; j <= rb.y; j++) {

			//����j��lt.y��rb.y֮��ı���
			double gamma_j = getGamma(lt.y, rb.y, j);

			//����j�����м��֮�����ɫ
			img.at<uchar>(i, j) = getMedian(point_color[0][2], point_color[1][2], gamma_j);
		}
	}
}