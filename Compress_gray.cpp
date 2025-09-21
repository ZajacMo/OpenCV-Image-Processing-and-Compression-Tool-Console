#include "Compress_gray.h"


Compress_gray::Compress_gray()
{
    sigma = 10;
    isMarked = nullptr;
}

//初始化（压缩）
void Compress_gray::init_compress(const cv::Mat& imge, const double& sigma_) {
    sigma = sigma_;
    img = imge;
    rows = img.rows;
    cols = img.cols * img.channels();
    newMatrix(isMarked, rows, cols, false);
    newMatrix(encoded_img, rows, cols);
}

//初始化（解压）
void Compress_gray::init_uncompress(const std::string filename, const int& size) {
    data.init(filename, size);

    //读取尺寸
    rows = data.read(16);
    cols = data.read(16);

    //判断是否为彩色图像
    //if (data.read(1)) {
    //    cols = 3 * rows;
    //    isColorful = true;
    //}
    //else {
    //    cols = rows;
    //    isColorful = false;
    //}
    //初始化参数
    result = cv::Mat::zeros(rows, cols, CV_8UC1);
    sigma = 10;
    newMatrix(isMarked, rows, cols, false);
    newMatrix(encoded_img, rows, cols);
}

//析构函数，释放内存
Compress_gray::~Compress_gray()
{
    deleteMatrix(isMarked, rows);
    deleteMatrix(encoded_img, rows);
}

//快速删除矩阵，防止内存泄漏
template<typename t>
void Compress_gray::deleteMatrix(t**& p, const int& len1) {
    if (p) {
        for (int i = 0; i < len1; i++)
            delete[] p[i];
        delete[] p;
    }
}

//快速创建矩阵
template<typename t>
void Compress_gray::newMatrix(t**& p, const int& len1, const int& len2, t init) {
    p = new t * [len1];
    for (int i = 0; i < len1; i++) {
        p[i] = new t[len2];
        for (int j = 0; j < len2; j++)
            p[i][j] = init;
    }
}

//设置编码后的图像
uchar& Compress_gray::setEncodedImg(cv::Point& p) {
    return encoded_img[p.x][p.y];
}

//判断是否为配对的点（解码用）
bool Compress_gray::isPeer(const cv::Point& lt, const cv::Point& rb) {

    //首先判断是否被标记，并不是孤立点（3）和矩形起始点（1）
    if (!isMarked[rb.x][rb.y] && (encoded_img[rb.x][rb.y] == 2 || !encoded_img[rb.x][rb.y])) {

        //遍历rb所在列
        for (int i = lt.x; i < rb.x; i++)
            if (isMarked[i][rb.y] || encoded_img[i][rb.y] && !(lt.x == i && lt.y == rb.y))
                return false;

        //遍历rb所在行
        for (int j = lt.y; j < rb.y; j++)
            if (isMarked[rb.x][j] || encoded_img[rb.x][j] && !(lt.x == rb.x && lt.y == j))
                return false;

        return true;

    }
    return false;
}

//对给定的左上角坐标，寻找右下角坐标（编码用）
cv::Point Compress_gray::findRb(const cv::Point& lt) {
    cv::Point rb = lt;
    //沿对角线扩展
    while (rb.x + 1 < rows && rb.y + 1 < cols &&
           RBlock(lt, cv::Point(rb.x + 1, rb.y + 1)).isBlock(img, isMarked, sigma)) {
        rb.x++;
        rb.y++;
    }
    //水平扩展
    while (rb.y + 1 < cols && RBlock(lt, cv::Point(rb.x, rb.y + 1)).isBlock(img, isMarked, sigma))
        rb.y++;
    //垂直扩展
    while (rb.x + 1 < rows && RBlock(lt, cv::Point(rb.x + 1, rb.y)).isBlock(img, isMarked, sigma))
        rb.x++;

    return rb;
}


//对给定的左上角坐标，寻找一个矩形（解码用）
RBlock Compress_gray::findBlock(const cv::Point& lt, int& blockNum) {
    cv::Point rb = lt;
    //除了孤立点矩形
    if (encoded_img[lt.x][lt.y] != 3)
    {
        //水平扩展
        while (rb.y + 1 < cols && isPeer(lt, cv::Point(rb.x, rb.y + 1)))
            rb.y++;
        //垂直扩展
        while (rb.x + 1 < rows && isPeer(lt, cv::Point(rb.x + 1, rb.y)))
            rb.x++;
    }

    //如果此处报错则应检查编码解码是否存在问题
    if (blockNum >= allBlocks.size())
        throw std::runtime_error("blockNum out of range");

    //更新读取的块的rb
    allBlocks[blockNum].rb = rb;

    //根据解压结果为图片上色
    allBlocks[blockNum].regain(result, encoded_img);

    return allBlocks[blockNum];
}



//将图像编码为0123的数字矩阵
std::vector<RBlock> Compress_gray::encode() {
    std::vector<RBlock> blocks;
    //遍历图像
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            //首先排除标记过的点
            if (!isMarked[i][j])
            {
                //找到矩形的右下角
                cv::Point start = cv::Point(i, j), end = findRb(start);

                //创建block
                RBlock block(img, start, end);

                //孤立点
                if (start.x == end.x && start.y == end.y)
                {
                    setEncodedImg(start) = 3;
                }
                //其他矩形
                else if (start.x <= end.x && start.y <= end.y)
                {
                    setEncodedImg(start) = 1;
                    setEncodedImg(end) = 2;
                }

                //保存block
                blocks.push_back(block);

                //标记已经编码的区域
                block.mark(isMarked);

                //更新到标记过的block的末尾，提高遍历效率
                j = end.y;
            }
        }
    
    return blocks;
}

//对0123的数字矩阵解码
std::vector<RBlock> Compress_gray::decode() {

    //解码
    std::vector<RBlock> blocks;
    int blockNum = 0;

    //遍历矩阵
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            //找到未被标记的点
            if (!isMarked[i][j])
            {
                //找到矩形的右下角
                RBlock block = findBlock(cv::Point(i, j), blockNum);

                //保存block
                blocks.push_back(block);

                //标记已经解码的区域
                block.mark(isMarked);
                blockNum++;

                //更新到标记过的block的末尾，提高遍历效率
                j = block.rb.y;
            }
        }

    return blocks;
}


//将图像压缩为二进制字符串
void Compress_gray::compress()
{
    //记录开始时间
    QueryPerformanceCounter(&time[0]);
    //data.write((int)log2(rows), 4);
    data.write(rows, 16);
    data.write(cols, 16);

    //编码
    allBlocks = encode();

  //  for(int  i = 0; i < rows; i++){
		//for (int j = 0; j < cols; j++)
		//	std::cout<<(int)encoded_img[i][j]<<"  ";
  //      std::cout<<std::endl;
  //      }
  //  std::cout << "\n";
				

    //记录编码后的图像
    for (int i = 0; i < rows; i++)
    {
        int last_position = 0;
        bool isNull = true;
        for (int j = 0; j < cols; j++) {
            int value = encoded_img[i][j];
            if (0 == value) {
                //该行没有被编码的像素时赋0
                if (j == cols - 1)
                    data.add(0);
            }
            else {
                //该行有被编码的像素时先赋1，再赋码
                data.add(1);
                if (1 == value)
                    data.add(1);
                else if (2 == value) {
                    data.add(0);
                    data.add(1);
                }
                else if (3 == value) {
                    data.add(0);
                    data.add(0);
                }
                //计算记录位置差需要的位数
                //last_position是下标，故在last_position不为0时，需要多减去1
                int b = ceil(log((double)(cols - last_position - !isNull)) / log(2.0));

                //防止b为0
                if (!b) b++;

                //计算位置差
                int gap = j - last_position - !isNull;

                //记录该行已有编码像素
                isNull = false;

                //记录当前位置
                last_position = j;

                //按位运算记录位置差
                data.write(gap, b);

                //记录颜色
                //孤立点只记录一个颜色
                if (value == 3)
                    data.write(img.at<uchar>(i, j), getDepth());
                //矩形记录四个颜色
                else if (value == 1) {
                    for (int m = 0; m < allBlocks.size(); m++) {
                        if (allBlocks[m].lt == cv::Point(i, j)) {
                            data.write(allBlocks[m].color[0][0], getDepth());
                            data.write(allBlocks[m].color[0][1], getDepth());
                            data.write(allBlocks[m].color[1][0], getDepth());
                            data.write(allBlocks[m].color[1][1], getDepth());
                            break;
                        }
                    }
                }

            }
        }
    }

    //记录结束时间
    QueryPerformanceCounter(&time[1]);
}

//将二进制字符串解压为图像
cv::Mat& Compress_gray::uncompress()
{
    //记录开始时间
    QueryPerformanceCounter(&time[0]);
    bool isNull = true;
    //上个非零元素的位置
    int col = 0, row = 0, value = 0;
    while ( data.readIndex < data.readLen)
    {
        //读取是否为0
        if (!data.read(1)) {
            row++;
            if (row >= rows)
                break;
            isNull = true;
            col = 0;
            continue;
        }
        else {
            if (data.read(1))
                value = 1;
            else
                value = (data.read(1)? 2 : 3);
        }

        //计算记录位置差需要的位数
        int b = ceil(log((double)(cols - col - !isNull)) / log(2.0));
        if (!b) b++;

        //读取位置差
        int gap = data.read(b);

        //计算当前位置
        col += gap + !isNull;

        //记录该行已有编码像素
        isNull = false;

        //赋值
        encoded_img[row][col] = value;

        //读取颜色
        if (value == 3) {
            //孤立点
            allBlocks.push_back(
                RBlock(
                    cv::Point(row, col),
                    cv::Point(0, 0),
                    data.read(getDepth())));
        }
        else if (value == 1) {
            //矩形
            uchar** p = nullptr;
            newMatrix(p, 2, 2);
            p[0][0] = data.read(getDepth());
            p[0][1] = data.read(getDepth());
            p[1][0] = data.read(getDepth());
            p[1][1] = data.read(getDepth());
            allBlocks.push_back(
                RBlock(cv::Point(row, col), cv::Point(0, 0), p));
            deleteMatrix(p, 2);
        }

        //如果该行最后一个像素被编码，就换行
        if (col == (cols - 1))
        {
            row++;
            isNull = true;
            col = 0;
        }
    }
    decode();
    QueryPerformanceCounter(&time[1]);
    return result;
}


//PSNR
double Compress_gray::getPSNR(cv::Mat img1, cv::Mat img2) {
    double mid = 0;;
    for (int i = 0; i < img1.rows; i++)
        for (int j = 0; j < img1.cols*img1.channels(); j++) {
            mid += pow(img1.at<uchar>(i, j) - img2.at<uchar>(i, j), 2);
        }
    return 10 * log10f(pow(255., 2) * img1.cols * img1.rows *img1.channels() / mid);
}

int Compress_gray::getDepth() {
    switch (img.depth()) {
    case CV_8U:
    case CV_8S:
        return 8;
    case CV_16U:
    case CV_16S:
        return  16;
    case CV_32S:
    case CV_32F:
        return 32;
    case CV_64F:
        return 64;
    default:
        return -1; // 未知或不支持的色深
    }
}

void Compress_gray::save(const std::string& filename) {
    data.save(filename);
}

long long Compress_gray::getReadLen() const {
    return data.readLen;
}

long long Compress_gray::getWriteLen() const {
    return data.vecLen * PERLEN + data.index; 
}

double Compress_gray::getTime() const{
    LARGE_INTEGER tmp;
    QueryPerformanceFrequency(&tmp);
    return (time[1].QuadPart - time[0].QuadPart) * 1.0 / tmp.QuadPart;
}

double Compress_gray::getRate()const {
    return getWriteLen() / (rows * cols * img.channels() * 8.0);
}
