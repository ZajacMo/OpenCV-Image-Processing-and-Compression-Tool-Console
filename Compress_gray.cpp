#include "Compress_gray.h"


Compress_gray::Compress_gray()
{
    sigma = 10;
    isMarked = nullptr;
}

//��ʼ����ѹ����
void Compress_gray::init_compress(const cv::Mat& imge, const double& sigma_) {
    sigma = sigma_;
    img = imge;
    rows = img.rows;
    cols = img.cols * img.channels();
    newMatrix(isMarked, rows, cols, false);
    newMatrix(encoded_img, rows, cols);
}

//��ʼ������ѹ��
void Compress_gray::init_uncompress(const std::string filename, const int& size) {
    data.init(filename, size);

    //��ȡ�ߴ�
    rows = data.read(16);
    cols = data.read(16);

    //�ж��Ƿ�Ϊ��ɫͼ��
    //if (data.read(1)) {
    //    cols = 3 * rows;
    //    isColorful = true;
    //}
    //else {
    //    cols = rows;
    //    isColorful = false;
    //}
    //��ʼ������
    result = cv::Mat::zeros(rows, cols, CV_8UC1);
    sigma = 10;
    newMatrix(isMarked, rows, cols, false);
    newMatrix(encoded_img, rows, cols);
}

//�����������ͷ��ڴ�
Compress_gray::~Compress_gray()
{
    deleteMatrix(isMarked, rows);
    deleteMatrix(encoded_img, rows);
}

//����ɾ�����󣬷�ֹ�ڴ�й©
template<typename t>
void Compress_gray::deleteMatrix(t**& p, const int& len1) {
    if (p) {
        for (int i = 0; i < len1; i++)
            delete[] p[i];
        delete[] p;
    }
}

//���ٴ�������
template<typename t>
void Compress_gray::newMatrix(t**& p, const int& len1, const int& len2, t init) {
    p = new t * [len1];
    for (int i = 0; i < len1; i++) {
        p[i] = new t[len2];
        for (int j = 0; j < len2; j++)
            p[i][j] = init;
    }
}

//���ñ�����ͼ��
uchar& Compress_gray::setEncodedImg(cv::Point& p) {
    return encoded_img[p.x][p.y];
}

//�ж��Ƿ�Ϊ��Եĵ㣨�����ã�
bool Compress_gray::isPeer(const cv::Point& lt, const cv::Point& rb) {

    //�����ж��Ƿ񱻱�ǣ������ǹ����㣨3���;�����ʼ�㣨1��
    if (!isMarked[rb.x][rb.y] && (encoded_img[rb.x][rb.y] == 2 || !encoded_img[rb.x][rb.y])) {

        //����rb������
        for (int i = lt.x; i < rb.x; i++)
            if (isMarked[i][rb.y] || encoded_img[i][rb.y] && !(lt.x == i && lt.y == rb.y))
                return false;

        //����rb������
        for (int j = lt.y; j < rb.y; j++)
            if (isMarked[rb.x][j] || encoded_img[rb.x][j] && !(lt.x == rb.x && lt.y == j))
                return false;

        return true;

    }
    return false;
}

//�Ը��������Ͻ����꣬Ѱ�����½����꣨�����ã�
cv::Point Compress_gray::findRb(const cv::Point& lt) {
    cv::Point rb = lt;
    //�ضԽ�����չ
    while (rb.x + 1 < rows && rb.y + 1 < cols &&
           RBlock(lt, cv::Point(rb.x + 1, rb.y + 1)).isBlock(img, isMarked, sigma)) {
        rb.x++;
        rb.y++;
    }
    //ˮƽ��չ
    while (rb.y + 1 < cols && RBlock(lt, cv::Point(rb.x, rb.y + 1)).isBlock(img, isMarked, sigma))
        rb.y++;
    //��ֱ��չ
    while (rb.x + 1 < rows && RBlock(lt, cv::Point(rb.x + 1, rb.y)).isBlock(img, isMarked, sigma))
        rb.x++;

    return rb;
}


//�Ը��������Ͻ����꣬Ѱ��һ�����Σ������ã�
RBlock Compress_gray::findBlock(const cv::Point& lt, int& blockNum) {
    cv::Point rb = lt;
    //���˹��������
    if (encoded_img[lt.x][lt.y] != 3)
    {
        //ˮƽ��չ
        while (rb.y + 1 < cols && isPeer(lt, cv::Point(rb.x, rb.y + 1)))
            rb.y++;
        //��ֱ��չ
        while (rb.x + 1 < rows && isPeer(lt, cv::Point(rb.x + 1, rb.y)))
            rb.x++;
    }

    //����˴�������Ӧ����������Ƿ��������
    if (blockNum >= allBlocks.size())
        throw std::runtime_error("blockNum out of range");

    //���¶�ȡ�Ŀ��rb
    allBlocks[blockNum].rb = rb;

    //���ݽ�ѹ���ΪͼƬ��ɫ
    allBlocks[blockNum].regain(result, encoded_img);

    return allBlocks[blockNum];
}



//��ͼ�����Ϊ0123�����־���
std::vector<RBlock> Compress_gray::encode() {
    std::vector<RBlock> blocks;
    //����ͼ��
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            //�����ų���ǹ��ĵ�
            if (!isMarked[i][j])
            {
                //�ҵ����ε����½�
                cv::Point start = cv::Point(i, j), end = findRb(start);

                //����block
                RBlock block(img, start, end);

                //������
                if (start.x == end.x && start.y == end.y)
                {
                    setEncodedImg(start) = 3;
                }
                //��������
                else if (start.x <= end.x && start.y <= end.y)
                {
                    setEncodedImg(start) = 1;
                    setEncodedImg(end) = 2;
                }

                //����block
                blocks.push_back(block);

                //����Ѿ����������
                block.mark(isMarked);

                //���µ���ǹ���block��ĩβ����߱���Ч��
                j = end.y;
            }
        }
    
    return blocks;
}

//��0123�����־������
std::vector<RBlock> Compress_gray::decode() {

    //����
    std::vector<RBlock> blocks;
    int blockNum = 0;

    //��������
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
        {
            //�ҵ�δ����ǵĵ�
            if (!isMarked[i][j])
            {
                //�ҵ����ε����½�
                RBlock block = findBlock(cv::Point(i, j), blockNum);

                //����block
                blocks.push_back(block);

                //����Ѿ����������
                block.mark(isMarked);
                blockNum++;

                //���µ���ǹ���block��ĩβ����߱���Ч��
                j = block.rb.y;
            }
        }

    return blocks;
}


//��ͼ��ѹ��Ϊ�������ַ���
void Compress_gray::compress()
{
    //��¼��ʼʱ��
    QueryPerformanceCounter(&time[0]);
    //data.write((int)log2(rows), 4);
    data.write(rows, 16);
    data.write(cols, 16);

    //����
    allBlocks = encode();

  //  for(int  i = 0; i < rows; i++){
		//for (int j = 0; j < cols; j++)
		//	std::cout<<(int)encoded_img[i][j]<<"  ";
  //      std::cout<<std::endl;
  //      }
  //  std::cout << "\n";
				

    //��¼������ͼ��
    for (int i = 0; i < rows; i++)
    {
        int last_position = 0;
        bool isNull = true;
        for (int j = 0; j < cols; j++) {
            int value = encoded_img[i][j];
            if (0 == value) {
                //����û�б����������ʱ��0
                if (j == cols - 1)
                    data.add(0);
            }
            else {
                //�����б����������ʱ�ȸ�1���ٸ���
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
                //�����¼λ�ò���Ҫ��λ��
                //last_position���±꣬����last_position��Ϊ0ʱ����Ҫ���ȥ1
                int b = ceil(log((double)(cols - last_position - !isNull)) / log(2.0));

                //��ֹbΪ0
                if (!b) b++;

                //����λ�ò�
                int gap = j - last_position - !isNull;

                //��¼�������б�������
                isNull = false;

                //��¼��ǰλ��
                last_position = j;

                //��λ�����¼λ�ò�
                data.write(gap, b);

                //��¼��ɫ
                //������ֻ��¼һ����ɫ
                if (value == 3)
                    data.write(img.at<uchar>(i, j), getDepth());
                //���μ�¼�ĸ���ɫ
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

    //��¼����ʱ��
    QueryPerformanceCounter(&time[1]);
}

//���������ַ�����ѹΪͼ��
cv::Mat& Compress_gray::uncompress()
{
    //��¼��ʼʱ��
    QueryPerformanceCounter(&time[0]);
    bool isNull = true;
    //�ϸ�����Ԫ�ص�λ��
    int col = 0, row = 0, value = 0;
    while ( data.readIndex < data.readLen)
    {
        //��ȡ�Ƿ�Ϊ0
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

        //�����¼λ�ò���Ҫ��λ��
        int b = ceil(log((double)(cols - col - !isNull)) / log(2.0));
        if (!b) b++;

        //��ȡλ�ò�
        int gap = data.read(b);

        //���㵱ǰλ��
        col += gap + !isNull;

        //��¼�������б�������
        isNull = false;

        //��ֵ
        encoded_img[row][col] = value;

        //��ȡ��ɫ
        if (value == 3) {
            //������
            allBlocks.push_back(
                RBlock(
                    cv::Point(row, col),
                    cv::Point(0, 0),
                    data.read(getDepth())));
        }
        else if (value == 1) {
            //����
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

        //����������һ�����ر����룬�ͻ���
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
        return -1; // δ֪��֧�ֵ�ɫ��
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
