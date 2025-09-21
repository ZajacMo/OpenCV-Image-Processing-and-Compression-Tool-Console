# OpenCV ͼ������ѹ������ / OpenCV Image Processing and Compression Tool

## ��Ŀ��� / Project Introduction
����һ������OpenCV�⿪���ĻҶ�ͼ��ѹ�����ѹϵͳ������Ŀʵ����һ�ֻ��ڷֿ��ͼ��ѹ���㷨�������ڱ���ͼ��������ͬʱ����Ч�ؼ�Сͼ���ļ��Ĵ�С����Ŀ֧��ѹ�������ĵ�������ƽ��ѹ���ʺ�ͼ��������

This is a grayscale image compression and decompression system developed based on the OpenCV library. The project implements a block-based image compression algorithm that can effectively reduce the size of image files while maintaining image quality. It supports adjusting compression parameters to balance compression ratio and image quality.

## ϵͳ�ܹ� / System Architecture

### ��������� / Core Class Design
��Ŀ�������������ƣ���Ҫ�������¼��������ࣺ

The project adopts object-oriented design and mainly includes the following core classes:

1. **Compress_gray**����Ҫ��ѹ��/��ѹ�࣬����ͼ���ѹ������ѹ������ͼ���
2. **Block**��ͼ���Ļ��࣬�����˿�Ļ������ԺͲ���
3. **RBlock**�����ع���ͼ����࣬�̳���Block��ʵ���˾���Ŀ��жϺ��ؽ�����
4. **Bits**�����������ݴ����࣬����������ݵĴ洢�Ͷ�д

1. **Compress_gray**: Main compression/decompression class responsible for image compression, decompression, saving and loading
2. **Block**: Base class for image blocks, defining basic properties and operations of blocks
3. **RBlock**: Reconstructable image block class, inheriting from Block, implementing specific block judgment and reconstruction methods
4. **Bits**: Binary data processing class responsible for storing, reading and writing encoded data

### ��Ҫ�������� / Main Data Flow
1. **ѹ������**����ȡͼ�� �� �ֿ鴦�� �� ���� �� ����Ϊѹ���ļ�
2. **��ѹ����**����ȡѹ���ļ� �� ���� �� �ؽ�ͼ��� �� ���ɽ�ѹͼ��

1. **Compression process**: Read image �� Block processing �� Encoding �� Save as compressed file
2. **Decompression process**: Read compressed file �� Decoding �� Reconstruct image blocks �� Generate decompressed image

## �����ص� / Features

- **��Чѹ��**�����ڷֿ��ѹ���㷨���ܹ���Ч��С�ļ���С
- **�ɵ�����**��ͨ��sigma��������ѹ��������ѹ���ʵ�ƽ��
- **��������**���ṩPSNR����ֵ����ȣ����㣬����ѹ��ͼ������
- **��������**��֧������������ͼ���������ܱ���

- **Efficient compression**: Block-based compression algorithm that can effectively reduce file size
- **Adjustable parameters**: Control the balance between compression quality and compression ratio through the sigma parameter
- **Performance evaluation**: Provide PSNR (Peak Signal-to-Noise Ratio) calculation to evaluate compressed image quality
- **Batch processing**: Support batch processing of multiple images and generate performance reports

## ��Ŀ�ṹ / Project Structure

```
test_openCV/
������ .vs/                # Visual Studio�����ļ� / Visual Studio configuration files
������ .vscode/            # VSCode�����ļ� / VSCode configuration files
������ images/             # ����ͼ���ļ��� / Test images folder
������ Compress_gray.cpp   # ѹ����ʵ�� / Compression class implementation
������ Compress_gray.h     # ѹ����ͷ�ļ� / Compression class header file
������ Block.cpp           # ͼ������ʵ�� / Image block base class implementation
������ Block.h             # ͼ������ͷ�ļ� / Image block base class header file
������ RBlock.cpp          # ���ع���ʵ�� / Reconstructable block implementation
������ RBlock.h            # ���ع���ͷ�ļ� / Reconstructable block header file
������ Bits.cpp            # �����ƴ���ʵ�� / Binary processing implementation
������ Bits.h              # �����ƴ���ͷ�ļ� / Binary processing header file
������ main.cpp            # ��������� / Main program entry
������ test_openCV.sln     # Visual Studio��������ļ� / Visual Studio solution file
```

## ���Ĺ������ / Detailed Core Functions

### 1. ͼ��ѹ�� / Image Compression

ѹ��������Ҫͨ��`Compress_gray`���`compress()`����ʵ�֡��÷������ȶ�ͼ����зֿ鴦��Ȼ���ÿ������б��룬��󽫱��������ݱ���Ϊѹ���ļ���

The compression process is mainly implemented through the `compress()` method of the `Compress_gray` class. This method first processes the image in blocks, then encodes each block, and finally saves the encoded data as a compressed file.

�ؼ����� / Key steps:
- ͼ��Ԥ���� / Image preprocessing
- �ֿ���Һͱ�� / Block searching and marking
- ����� / Block encoding
- ���ݴ洢 / Data storage

### 2. ͼ���ѹ / Image Decompression

��ѹ����ͨ��`Compress_gray`���`uncompress()`����ʵ�֡��÷�����ѹ���ļ��ж�ȡ���ݣ�������ؽ�ͼ��飬������������Ľ�ѹͼ��

The decompression process is implemented through the `uncompress()` method of the `Compress_gray` class. This method reads data from the compressed file, reconstructs image blocks after decoding, and finally generates a complete decompressed image.

�ؼ����� / Key steps:
- ��ȡѹ������ / Read compressed data
- �������Ϣ / Decode block information
- �ؽ�ͼ��� / Reconstruct image blocks
- ���ɽ�ѹͼ�� / Generate decompressed image

### 3. �������� / Performance Evaluation

��Ŀ�ṩ�˶�������ָ��������

The project provides multiple performance metrics for evaluation:

- **ѹ����**��ѹ�����ļ���С��ԭ�ļ���С�ı�ֵ
- **����ʱ��**��ѹ���ͽ�ѹ���̵ĺ�ʱ
- **PSNR**����ֵ����ȣ�����ѹ��ͼ����ԭʼͼ������ƶ�

- **Compression ratio**: Ratio of compressed file size to original file size
- **Processing time**: Time consumed by compression and decompression processes
- **PSNR**: Peak Signal-to-Noise Ratio, measuring the similarity between compressed image and original image

## ʹ�÷��� / Usage

### ���뻷�� / Compilation Environment
- Visual Studio
- OpenCV�� / OpenCV library

### �����÷� / Basic Usage

#### ����ͼ��ѹ�����ѹ / Single Image Compression and Decompression

```cpp
// ѹ�� / Compression
Compress_gray cg;
cg.init_compress(cv::imread("./images/lena512.bmp", 0), 5);  // �ڶ�������Ϊsigmaֵ / The second parameter is sigma value
cg.compress();
cg.save("compressedIMG.nam");

// ��ѹ / Decompression
Compress_gray ucg;
ucg.init_uncompress("compressedIMG.nam");
cv::Mat res = ucg.uncompress();
cv::imshow("��ѹͼ��", res);  // "Decompressed Image"
cv::waitKey(0);
```

#### �������������ܲ��� / Batch Processing and Performance Testing

```cpp
// ����Ҫ���Ե�ͼ���sigmaֵ / Define images and sigma values for testing
string fileName[] = {"100gray","197gray","baboonzyp512","barbarazyp512","broke256","decho256","f16","flightzyp512","lena512","lenazyp512"};
int sigma[] = {1,5,10,20,30,50};

// ��������������ܱ��� / Batch processing and output performance report
for(int i = 0; i < 10; i++) {
    for(int j = 0; j < 6; j++) {
        getRes(fileName[i], sigma[j]);
    }
}
```

### ����˵�� / Parameter Description

- **sigmaֵ**������ѹ��������ѹ���ʵ�ƽ�����
  - sigmaԽС��ѹ������Խ�ߣ���ѹ����Խ��
  - sigmaԽ��ѹ������Խ�ͣ���ѹ����Խ��

- **sigma value**: Parameter controlling the balance between compression quality and compression ratio
  - Smaller sigma: higher compression quality but lower compression ratio
  - Larger sigma: lower compression quality but higher compression ratio

## ����ָ��˵�� / Performance Metrics Description

������������ʱ����������������ָ�꣺

When running batch tests, the output results include the following metrics:

| ָ�� / Metric | ˵�� / Description |
|--------------|-------------------|
| ͼ���� / Image name | ����ͼ����ļ��� / File name of the test image |
| ԭͼ�ߴ� / Original size | ԭʼͼ��ķֱ��� / Resolution of the original image |
| �������sigma�� / Allowed error (sigma) | ѹ��ʱʹ�õ�sigma����ֵ / Sigma parameter value used during compression |
| ѹ�����С / Compressed size | ѹ���ļ��Ĵ�С���ֽڣ� / Size of the compressed file (bytes) |
| ѹ���� / Compression ratio | ѹ��Ч�ʣ�ԽС��ʾѹ��Ч��Խ�� / Compression efficiency, smaller value indicates better compression |
| ѹ��ʱ�� / Compression time | ѹ���������ĵ�ʱ�䣨�룩 / Time consumed by compression process (seconds) |
| ��ѹʱ�� / Decompression time | ��ѹ�������ĵ�ʱ�䣨�룩 / Time consumed by decompression process (seconds) |
| PSNR | ��ֵ����ȣ�����ͼ��������ֵԽ�߱�ʾ����Խ�� / Peak Signal-to-Noise Ratio, measuring image quality, higher value indicates better quality |

## �Ż����� / Optimization Suggestions

1. **���̴߳���**�����Կ���������̴߳����ߴ�ͼ����ߴ����ٶ�
2. **ѹ���㷨�Ż�**��̽������Ч�Ŀ�ָ�ͱ������
3. **�ڴ����**�����ڴ�ߴ�ͼ�񣬿����Ż��ڴ�ʹ�÷�ʽ
4. **ɫ��ͼ��֧��**��Ŀǰ��Ҫ֧�ֻҶ�ͼ�񣬿�����չ֧�ֲ�ɫͼ���ѹ��

1. **Multi-threaded processing**: Consider introducing multi-threading to process large-sized images and improve processing speed
2. **Compression algorithm optimization**: Explore more efficient block segmentation and encoding strategies
3. **Memory management**: For large-sized images, optimize memory usage
4. **Color image support**: Currently mainly supports grayscale images, can be extended to support color image compression

## ע������ / Notes

1. ȷ������ȷ����OpenCV�� / Ensure that the OpenCV library is correctly configured
2. ѹ���ļ���ʽΪ�Զ����.nam��ʽ / The compressed file format is a custom .nam format
3. ����ͼ��Ӧ����imagesĿ¼�� / Test images should be placed in the images directory
4. �ϴ��sigmaֵ���ܵ���ͼ�����������½� / Larger sigma values may cause significant degradation in image quality