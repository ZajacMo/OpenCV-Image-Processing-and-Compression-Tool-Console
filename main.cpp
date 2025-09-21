#include "Compress_gray.h"
#include <iostream>
using namespace std;
//cv::Mat Compress_gray::img ;
void getRes(string fileName, int sigma);

int main() {
	//ѹ��
	// Compress_gray cg;
	//cg.init_compress(cv::imread("./images/f16.bmp"  ,0), 5);
	//cg.init_compress(cv::imread("./images/lena512.bmp",0), 5);
	 //cg.init_compress(cv::imread("./images/PrintScreen.png", 0), 5);
	//cg.init_compress(cv::imread("./images/100gray.bmp",0), 5);
	//cg.init_compress(cv::imread("./images/197gray.bmp",0), 5);
	//cg.init_compress(cv::imread("./images/flightzyp512.bmp",-1), 20);
	//cg.init_compress(cv::imread("./images/lenaColorful.bmp",0), 20);
	//cv::namedWindow("ԭͼ�Ҷ�ͼ��", 0);
	//cv::imshow("ԭͼ�Ҷ�ͼ��", cg.img);
	//cg.compress();
	//cg.save("compressedIMG.nam");
	//std::cout<<"ѹ����ʱ��"<<cg.getTime()<<std::endl
	//	<<"ѹ�����С��"<<cg.getWriteLen()<<std::endl
	//	<<"ѹ���ʣ�"<<cg.getRate();

	////��ѹ
	//Compress_gray ucg;
	//ucg.init_uncompress("compressedIMG.nam");
	//cv::Mat res = ucg.uncompress();
	//cv::namedWindow("ѹ����ָ�ͼ��", 0);
	//cv::imshow("ѹ����ָ�ͼ��", res);
	//std::cout<<"��ѹ��ʱ��"<<ucg.getTime()<<std::endl
	//				<< "ѹ���ʣ�" << ucg.getReadLen()/ (cg.img.rows * cg.img.cols * cg.img.channels()*8.0) << std::endl
	//				<< "PSNR��" << Compress_gray::getPSNR(cg.img, res) << std::endl;
	//cv::waitKey(0);
	string fileName[] = {"100gray","197gray","baboonzyp512","barbarazyp512","broke256","decho256","f16","flightzyp512","lena512","lenazyp512"};
	int sigma[] = {1,5,10,20,30,50};
	cout<<"ͼ����	ԭͼ�ߴ�	�������sigma��	ѹ�����С	ѹ����	ѹ��ʱ��	��ѹʱ��	PSNR"<<endl;
	for(int i = 0; i < 10; i++) {
		for(int j = 0; j < 6; j++) {
			getRes(fileName[i], sigma[j]);
		}
	}

	return 0;
}

void getRes(string fileName, int sigma) {
	Compress_gray cg;
	cg.init_compress(cv::imread("./images/" + fileName + ".bmp", 0), sigma);
	cg.compress();
	cg.save("compressedIMG.nam");
	std::cout << fileName + ".bmp  " << cg.img.rows << "*" << cg.img.cols << "	 " << sigma << "	" << cg.getWriteLen() << "	" << cg.getRate() << "	" << cg.getTime() << "	";
	Compress_gray ucg;
	ucg.init_uncompress("compressedIMG.nam");
	cv::Mat res = ucg.uncompress();
	std::cout << ucg.getTime() << "	" << Compress_gray::getPSNR(cg.img, res) << std::endl;
}