# OpenCV 图像处理与压缩工具 / OpenCV Image Processing and Compression Tool

## 项目简介 / Project Introduction
这是一个基于OpenCV库开发的灰度图像压缩与解压系统。该项目实现了一种基于分块的图像压缩算法，可以在保持图像质量的同时，有效地减小图像文件的大小。项目支持压缩参数的调整，以平衡压缩率和图像质量。

This is a grayscale image compression and decompression system developed based on the OpenCV library. The project implements a block-based image compression algorithm that can effectively reduce the size of image files while maintaining image quality. It supports adjusting compression parameters to balance compression ratio and image quality.

## 系统架构 / System Architecture

### 核心类设计 / Core Class Design
项目采用面向对象设计，主要包含以下几个核心类：

The project adopts object-oriented design and mainly includes the following core classes:

1. **Compress_gray**：主要的压缩/解压类，负责图像的压缩、解压、保存和加载
2. **Block**：图像块的基类，定义了块的基本属性和操作
3. **RBlock**：可重构的图像块类，继承自Block，实现了具体的块判断和重建方法
4. **Bits**：二进制数据处理类，负责编码数据的存储和读写

1. **Compress_gray**: Main compression/decompression class responsible for image compression, decompression, saving and loading
2. **Block**: Base class for image blocks, defining basic properties and operations of blocks
3. **RBlock**: Reconstructable image block class, inheriting from Block, implementing specific block judgment and reconstruction methods
4. **Bits**: Binary data processing class responsible for storing, reading and writing encoded data

### 主要数据流程 / Main Data Flow
1. **压缩流程**：读取图像 → 分块处理 → 编码 → 保存为压缩文件
2. **解压流程**：读取压缩文件 → 解码 → 重建图像块 → 生成解压图像

1. **Compression process**: Read image → Block processing → Encoding → Save as compressed file
2. **Decompression process**: Read compressed file → Decoding → Reconstruct image blocks → Generate decompressed image

## 功能特点 / Features

- **高效压缩**：基于分块的压缩算法，能够有效减小文件大小
- **可调参数**：通过sigma参数控制压缩质量和压缩率的平衡
- **性能评估**：提供PSNR（峰值信噪比）计算，评估压缩图像质量
- **批量处理**：支持批量处理多个图像并生成性能报告

- **Efficient compression**: Block-based compression algorithm that can effectively reduce file size
- **Adjustable parameters**: Control the balance between compression quality and compression ratio through the sigma parameter
- **Performance evaluation**: Provide PSNR (Peak Signal-to-Noise Ratio) calculation to evaluate compressed image quality
- **Batch processing**: Support batch processing of multiple images and generate performance reports

## 项目结构 / Project Structure

```
test_openCV/
├── .vs/                # Visual Studio配置文件 / Visual Studio configuration files
├── .vscode/            # VSCode配置文件 / VSCode configuration files
├── images/             # 测试图像文件夹 / Test images folder
├── Compress_gray.cpp   # 压缩类实现 / Compression class implementation
├── Compress_gray.h     # 压缩类头文件 / Compression class header file
├── Block.cpp           # 图像块基类实现 / Image block base class implementation
├── Block.h             # 图像块基类头文件 / Image block base class header file
├── RBlock.cpp          # 可重构块实现 / Reconstructable block implementation
├── RBlock.h            # 可重构块头文件 / Reconstructable block header file
├── Bits.cpp            # 二进制处理实现 / Binary processing implementation
├── Bits.h              # 二进制处理头文件 / Binary processing header file
├── main.cpp            # 主程序入口 / Main program entry
└── test_openCV.sln     # Visual Studio解决方案文件 / Visual Studio solution file
```

## 核心功能详解 / Detailed Core Functions

### 1. 图像压缩 / Image Compression

压缩过程主要通过`Compress_gray`类的`compress()`方法实现。该方法首先对图像进行分块处理，然后对每个块进行编码，最后将编码后的数据保存为压缩文件。

The compression process is mainly implemented through the `compress()` method of the `Compress_gray` class. This method first processes the image in blocks, then encodes each block, and finally saves the encoded data as a compressed file.

关键步骤 / Key steps:
- 图像预处理 / Image preprocessing
- 分块查找和标记 / Block searching and marking
- 块编码 / Block encoding
- 数据存储 / Data storage

### 2. 图像解压 / Image Decompression

解压过程通过`Compress_gray`类的`uncompress()`方法实现。该方法从压缩文件中读取数据，解码后重建图像块，最后生成完整的解压图像。

The decompression process is implemented through the `uncompress()` method of the `Compress_gray` class. This method reads data from the compressed file, reconstructs image blocks after decoding, and finally generates a complete decompressed image.

关键步骤 / Key steps:
- 读取压缩数据 / Read compressed data
- 解码块信息 / Decode block information
- 重建图像块 / Reconstruct image blocks
- 生成解压图像 / Generate decompressed image

### 3. 性能评估 / Performance Evaluation

项目提供了多种性能指标评估：

The project provides multiple performance metrics for evaluation:

- **压缩率**：压缩后文件大小与原文件大小的比值
- **处理时间**：压缩和解压过程的耗时
- **PSNR**：峰值信噪比，衡量压缩图像与原始图像的相似度

- **Compression ratio**: Ratio of compressed file size to original file size
- **Processing time**: Time consumed by compression and decompression processes
- **PSNR**: Peak Signal-to-Noise Ratio, measuring the similarity between compressed image and original image

## 使用方法 / Usage

### 编译环境 / Compilation Environment
- Visual Studio
- OpenCV库 / OpenCV library

### 基本用法 / Basic Usage

#### 单个图像压缩与解压 / Single Image Compression and Decompression

```cpp
// 压缩 / Compression
Compress_gray cg;
cg.init_compress(cv::imread("./images/lena512.bmp", 0), 5);  // 第二个参数为sigma值 / The second parameter is sigma value
cg.compress();
cg.save("compressedIMG.nam");

// 解压 / Decompression
Compress_gray ucg;
ucg.init_uncompress("compressedIMG.nam");
cv::Mat res = ucg.uncompress();
cv::imshow("解压图像", res);  // "Decompressed Image"
cv::waitKey(0);
```

#### 批量处理与性能测试 / Batch Processing and Performance Testing

```cpp
// 定义要测试的图像和sigma值 / Define images and sigma values for testing
string fileName[] = {"100gray","197gray","baboonzyp512","barbarazyp512","broke256","decho256","f16","flightzyp512","lena512","lenazyp512"};
int sigma[] = {1,5,10,20,30,50};

// 批量处理并输出性能报告 / Batch processing and output performance report
for(int i = 0; i < 10; i++) {
    for(int j = 0; j < 6; j++) {
        getRes(fileName[i], sigma[j]);
    }
}
```

### 参数说明 / Parameter Description

- **sigma值**：控制压缩质量和压缩率的平衡参数
  - sigma越小，压缩质量越高，但压缩率越低
  - sigma越大，压缩质量越低，但压缩率越高

- **sigma value**: Parameter controlling the balance between compression quality and compression ratio
  - Smaller sigma: higher compression quality but lower compression ratio
  - Larger sigma: lower compression quality but higher compression ratio

## 性能指标说明 / Performance Metrics Description

运行批量测试时，输出结果包含以下指标：

When running batch tests, the output results include the following metrics:

| 指标 / Metric | 说明 / Description |
|--------------|-------------------|
| 图像名 / Image name | 测试图像的文件名 / File name of the test image |
| 原图尺寸 / Original size | 原始图像的分辨率 / Resolution of the original image |
| 允许的误差（sigma） / Allowed error (sigma) | 压缩时使用的sigma参数值 / Sigma parameter value used during compression |
| 压缩后大小 / Compressed size | 压缩文件的大小（字节） / Size of the compressed file (bytes) |
| 压缩率 / Compression ratio | 压缩效率，越小表示压缩效果越好 / Compression efficiency, smaller value indicates better compression |
| 压缩时间 / Compression time | 压缩过程消耗的时间（秒） / Time consumed by compression process (seconds) |
| 解压时间 / Decompression time | 解压过程消耗的时间（秒） / Time consumed by decompression process (seconds) |
| PSNR | 峰值信噪比，衡量图像质量，值越高表示质量越好 / Peak Signal-to-Noise Ratio, measuring image quality, higher value indicates better quality |

## 优化建议 / Optimization Suggestions

1. **多线程处理**：可以考虑引入多线程处理大尺寸图像，提高处理速度
2. **压缩算法优化**：探索更高效的块分割和编码策略
3. **内存管理**：对于大尺寸图像，可以优化内存使用方式
4. **色彩图像支持**：目前主要支持灰度图像，可以扩展支持彩色图像的压缩

1. **Multi-threaded processing**: Consider introducing multi-threading to process large-sized images and improve processing speed
2. **Compression algorithm optimization**: Explore more efficient block segmentation and encoding strategies
3. **Memory management**: For large-sized images, optimize memory usage
4. **Color image support**: Currently mainly supports grayscale images, can be extended to support color image compression

## 注意事项 / Notes

1. 确保已正确配置OpenCV库 / Ensure that the OpenCV library is correctly configured
2. 压缩文件格式为自定义的.nam格式 / The compressed file format is a custom .nam format
3. 测试图像应放在images目录下 / Test images should be placed in the images directory
4. 较大的sigma值可能导致图像质量明显下降 / Larger sigma values may cause significant degradation in image quality