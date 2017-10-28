#include <opencv2/opencv.hpp>

using namespace cv;

//����Ĺ�һ�����ƥ���㷨��û���õ��κ������ֶΣ���ncc��������ʱ���Լ��23������

void ncc(Mat& srcImage, Mat& templImage, Mat& result)
{
	int rows = srcImage.rows - templImage.rows + 1;
	assert(templImage.rows <= srcImage.rows);
	int cols = srcImage.cols - templImage.cols + 1;
	assert(templImage.cols <= srcImage.cols);
	result.create(rows, cols, CV_32FC1);

	Mat src, templ;
	cvtColor(srcImage, src, COLOR_BGR2GRAY);
	cvtColor(templImage, templ, COLOR_BGR2GRAY);

	double sum_templ = 0.;
	for (int i = 0; i < templ.rows; i++)
	{
		uchar* templ_ptr = templ.ptr<uchar>(i);
		for (int j = 0; j < templ.cols; j++)
		{
			sum_templ += (double)(templ_ptr[j] * templ_ptr[j]);
		}
	}

	double num = 0., den = 0.;
	for (int i = 0; i < result.rows; i++)
	{
		float* result_ptr = result.ptr<float>(i);                       //��Ϊresult��������CV_32FC1���������Ӧ��ָ����ָ�������Ϊfloat����uchar��double�������
		for (int j = 0; j < result.cols; j++)
		{

			for (int m = 0; m < templ.rows; m++)
			{
				uchar* templ_ptr = templ.ptr<uchar>(m);
				uchar* src_ptr = src.ptr<uchar>(i+m);
				for (int n = 0; n < templ.cols; n++)
				{
					num += (double)(templ_ptr[n] * src_ptr[j + n]);
					den += (double)(src_ptr[j + n] * src_ptr[j + n]);
				}
			}
			result_ptr[j] = (float)(num / (sqrt(sum_templ)*sqrt(den)));
			num = 0.;
			den = 0.;
		}
	}
}

int main()
{
	Mat srcImage = imread("C:\\Users\\LIUU\\Pictures\\src.jpg");
	Mat templImage = imread("C:\\Users\\LIUU\\Pictures\\roi_1.jpg");

	double time = static_cast<double>(getTickCount());
	Mat resultMap;
	ncc(srcImage, templImage, resultMap);

	normalize(resultMap, resultMap, 0, 1,NORM_MINMAX);                 //һ���һ������NORM_MINMAX������ÿ��Ԫ��������һ����Χ�ڣ����������Ĭ�ϵ�ģʽ�ǹ�һ��������
	imshow("bb", resultMap);
	time = ((double)getTickCount() - time) / getTickFrequency();
	std::cout << time;                                                            //ֻ��¼ncc�㷨�����ѵ�ʱ�䣬�������õ�ģ���ԭͼ����ʱ23������
	

	double minVal, maxVal;
	Point minPos, maxPos;
	minMaxLoc(resultMap, &minVal, &maxVal, &minPos, &maxPos);

	rectangle(srcImage, maxPos, Point(maxPos.x + templImage.cols, maxPos.y + templImage.rows),Scalar(0,0,255));
	imshow("aa", srcImage);
	waitKey(0);
}