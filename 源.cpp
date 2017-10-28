#include <opencv2/opencv.hpp>

using namespace cv;

//纯粹的归一化相关匹配算法，没有用到任何提速手段，纯ncc函数运行时间大约在23秒左右

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
		float* result_ptr = result.ptr<float>(i);                       //因为result的类型是CV_32FC1，所以其对应的指针所指向的类型为float，用uchar和double都会出错
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

	normalize(resultMap, resultMap, 0, 1,NORM_MINMAX);                 //一般归一化常用NORM_MINMAX，即将每个元素限制在一定范围内，但这个函数默认的模式是归一化二范数
	imshow("bb", resultMap);
	time = ((double)getTickCount() - time) / getTickFrequency();
	std::cout << time;                                                            //只记录ncc算法所花费的时间，对于我用的模板和原图，耗时23秒左右
	

	double minVal, maxVal;
	Point minPos, maxPos;
	minMaxLoc(resultMap, &minVal, &maxVal, &minPos, &maxPos);

	rectangle(srcImage, maxPos, Point(maxPos.x + templImage.cols, maxPos.y + templImage.rows),Scalar(0,0,255));
	imshow("aa", srcImage);
	waitKey(0);
}