#include "Compress_gray.h"
#include <iostream>
using namespace std;
//cv::Mat Compress_gray::img ;
void getRes(string fileName, int sigma);

int main() {
	//压缩
	// Compress_gray cg;
	//cg.init_compress(cv::imread("./images/f16.bmp"  ,0), 5);
	//cg.init_compress(cv::imread("./images/lena512.bmp",0), 5);
	 //cg.init_compress(cv::imread("./images/PrintScreen.png", 0), 5);
	//cg.init_compress(cv::imread("./images/100gray.bmp",0), 5);
	//cg.init_compress(cv::imread("./images/197gray.bmp",0), 5);
	//cg.init_compress(cv::imread("./images/flightzyp512.bmp",-1), 20);
	//cg.init_compress(cv::imread("./images/lenaColorful.bmp",0), 20);
	//cv::namedWindow("原图灰度图像", 0);
	//cv::imshow("原图灰度图像", cg.img);
	//cg.compress();
	//cg.save("compressedIMG.nam");
	//std::cout<<"压缩用时："<<cg.getTime()<<std::endl
	//	<<"压缩后大小："<<cg.getWriteLen()<<std::endl
	//	<<"压缩率："<<cg.getRate();

	////解压
	//Compress_gray ucg;
	//ucg.init_uncompress("compressedIMG.nam");
	//cv::Mat res = ucg.uncompress();
	//cv::namedWindow("压缩后恢复图像", 0);
	//cv::imshow("压缩后恢复图像", res);
	//std::cout<<"解压用时："<<ucg.getTime()<<std::endl
	//				<< "压缩率：" << ucg.getReadLen()/ (cg.img.rows * cg.img.cols * cg.img.channels()*8.0) << std::endl
	//				<< "PSNR：" << Compress_gray::getPSNR(cg.img, res) << std::endl;
	//cv::waitKey(0);
	string fileName[] = {"100gray","197gray","baboonzyp512","barbarazyp512","broke256","decho256","f16","flightzyp512","lena512","lenazyp512"};
	int sigma[] = {1,5,10,20,30,50};
	cout<<"图像名	原图尺寸	允许的误差（sigma）	压缩后大小	压缩率	压缩时间	解压时间	PSNR"<<endl;
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