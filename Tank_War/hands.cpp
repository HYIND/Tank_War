//#include <iostream>
//#include <vector>
//#include "opencv2/features2d/features2d.hpp"
//#include "opencv2/core/core.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/calib3d/calib3d.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/imgproc/types_c.h"
//#include "opencv2/core/utility.hpp"
//#include "opencv2/video/tracking.hpp"
//#include "opencv2/videoio.hpp"
////#include "puttextzh.h"
//#define PI 3.1415926
//
//using namespace std;
//using namespace cv;
//
////ȫ�ֱ���
//Mat frame, result;//frame����ÿ֡ԭͼ��result�������Ʒָ�
//vector<Mat> tpls;//����ģ��
//int hand = 0;//��ǰʶ���ģ���±�
//vector<Point>path;//�˶��켣
//int awayCount = 0;//�켣ƫ�����
//Rect preRect = Rect(Point(-1, -1), Point(-1, -1));//��һ֡�ľ��ο�
//
////����
//Point Match();//ģ��ƥ��
//void imageblur(Mat& src, Mat& dst, Size size, int threshold);//ͼƬ��Ե�⻬����
//Rect outLine(int thresh);//�ҳ��ֲ���Ӿ���
//Point centerOfRect(Rect tRect);//�ҳ����ε����ĵ�
//float getDistance(Point p1, Point p2);//�����������
//void drawLine(Rect tRect);//�����˶��켣
//
////ͼƬ��Ե�⻬����
////size��ʾȡ��ֵ�Ĵ��ڴ�С��threshold��ʾ�Ծ�ֵͼ����ж�ֵ������ֵ
//void imageblur(Mat& src, Mat& dst, Size size, int threshold)
//{
//	int height = src.rows;
//	int width = src.cols;
//	blur(src, dst, size);
//	for (int i = 0; i < height; i++)
//	{
//		uchar* p = dst.ptr<uchar>(i);
//		for (int j = 0; j < width; j++)
//		{
//			if (p[j] < threshold)
//				p[j] = 0;
//			else p[j] = 255;
//		}
//	}
//}
//
////thresh�����������ɸѡ��ͨ��
//Rect outLine(int thresh) {
//	//GaussianBlur(result, result, cv::Size(5, 5), 3, 3);//��˹�˲�
//	//��ȡ����
//	Mat threshold_output = Mat::zeros(result.rows, result.cols, CV_8UC1);
//	vector<vector<Point>> contours;
//	vector<Rect> rects;//���ʴ�С����Ӿ���
//	vector<Vec4i> hierachy;
//	//��Ե���
//	findContours(result, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//	vector<vector<Point>> contours_poly(contours.size());
//
//
//	Mat tImage = Mat::zeros(threshold_output.size(), CV_8UC3);
//	frame.copyTo(tImage);
//
//	for (unsigned int i = 0; i < contours.size(); i++)
//		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
//
//	int maxArea = 0;//ѡ�������ͨ��
//	Rect matchRect = Rect(Point(-1, -1), Point(-1, -1));
//	for (unsigned int i = 0; i < contours.size(); i++) {
//
//		Rect rect = boundingRect(contours[i]);// ȥ��С����ͨ��
//		int tArea = rect.area();
//		if (tArea < thresh)
//			continue;
//		rects.push_back(rect);//������ʴ�С����ͨ����ο�
//		//rectangle(frame, maxRect, Scalar(0, 255, 0), 2, LINE_8);
//
//		drawContours(tImage, contours_poly, i, Scalar(255, 0, 0), 1, 8, hierachy, 0, Point());
//	}
//	//imshow("����", tImage);
//	Point p = Match();
//	p.x += 50;
//	p.y += 100;
//	for (unsigned int i = 0; i < rects.size(); i++) {//���ģ��ƥ��õ��Ľ����þ��ο��غ�
//		if (p.x > rects[i].tl().x && p.x<rects[i].br().x && p.y>rects[i].tl().y && p.y < rects[i].br().y)
//			matchRect = rects[i];
//	}
//	//ƽ�����δ�С��ȡ��ǰ������ǰһ֡���ε�ƽ���ߴ�,���Ͷ���
//	if (preRect.x != -1) {
//		int k = 0.7;
//		matchRect.width = round(matchRect.width * (1 - k) + preRect.width * k);
//		matchRect.height = round(matchRect.height * (1 - k) + preRect.height * k);
//		preRect = matchRect;
//	}
//	rectangle(frame, matchRect, Scalar(0, 255, 0), 2, LINE_8);//������Ӿ��ο�
//	return matchRect;
//}
//
//Point Match() {
//	//cv.TM_SQDIFF_NORMED, cv.TM_CCORR_NORMED, cv.TM_CCOEFF_NORMED]��������ƥ���㷨
//	Point temploc;
//	double match_max = -1;//ƥ��̶�
//	//��ÿ������ģ�����ƥ��
//	for (int i = 0; i < tpls.size(); i++) {
//		Mat match;
//		int match_cols = result.cols - tpls[i].cols + 1;
//		int match_rows = result.rows - tpls[i].rows + 1;
//		match.create(match_cols, match_rows, CV_32FC1);
//		matchTemplate(result, tpls[i], match, TM_CCOEFF_NORMED, Mat());
//		Point minloc;
//		Point maxloc;
//		double mymin, mymax;
//		minMaxLoc(match, &mymin, &mymax, &minloc, &maxloc, Mat());
//		//ȡ���ƥ��
//		if (match_max < mymax) {
//			match_max = mymax;
//			temploc = maxloc;
//			hand = i;//����ģ���±�
//		}
//	}
//	if (hand != tpls.size() - 1)
//		//cout << match_max <<"  "<<hand<< endl;
//		rectangle(frame, Rect(temploc.x, temploc.y, 10, 10), Scalar(0, 0, 255), 2, LINE_AA);
//	return temploc;
//}
//
//Point centerOfRect(Rect rect) {
//	return Point(rect.x + round(rect.width / 2.0), rect.y + round(rect.height / 2.0));
//}
//
//float getDistance(Point p1, Point p2) {
//	return sqrtf(powf((p1.x - p2.x), 2) + powf((p1.y - p2.y), 2));
//}
//
//
//void drawLine(Rect tRect) {
//	if (tRect.x != -1) {
//		//Point p = centerOfRect(tRect);
//		Point p = tRect.tl();
//		//�������̫����Ϊ���
//		if (!path.empty() && getDistance(p, path.back()) > 30.0) {
//			awayCount++;
//			if (awayCount >= 4) {//ƫ�������������ֲ������λ�ƣ����»���
//				awayCount = 0;
//				path.clear();
//			}
//		}
//		else {
//			path.push_back(p);
//		}
//	}
//	polylines(frame, path, false, Scalar(0, 0, 255), 3);//���ݵ����л���
//}
//
//int* getFeature(int len) {//len��ʾ�������г���
//	int* features = new int[len];
//	int k = floor(path.size() / len);
//	if (k == 0)return NULL;//��¼ʱ��̫����Ϊ��Ч
//	float xSum = 0, ySum = 0;
//	for (int i = 1; i < len; i++) {
//		xSum += path[i * k].x;
//		ySum += path[i * k].y;
//	}
//	float xc = xSum / len, yc = ySum / len;
//	cout << "features:";
//	for (int i = 0; i < len; i++) {
//		float angle = fastAtan2(path[i * k].y - yc, path[i * k].x - xc);//fastAtan2���ؽǶȣ�0-360
//		//16������ɢ��
//		for (int j = 0; j < 16; j++) {
//			if (angle >= j * 22.5 && angle <= (j + 1) * 22.5)
//				features[i] = j;
//		}
//		cout << features[i] << ",";
//	}
//	cout << endl;
//	path.clear();
//	return features;
//}
//
//int main()
//{
//	VideoCapture cap(0);//������ͷ
//	if (!cap.isOpened())
//	{
//		return -1;
//	}
//
//	Mat ycrcb_image;//YCbCr��ɫ�ռ�
//	Mat Y, Cr, Cb;//��Ӧ����ͨ��
//	vector<Mat> channels;
//	bool stop = false;
//	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));//���ڸ�ʴ�����͵Ĳ���
//	Mat a = imread("D:/Desktop/Hand/Hand/x64/Debug/hands/h0.jpg", CV_8UC1);
//	if (a.empty())  //�ж��Ƿ�������
//	{
//		cout << "Can not read this image !" << endl;
//		return -1;
//	}
//	tpls.push_back(imread("D:/Desktop/Hand/Hand/x64/Debug/hands/h0.jpg", CV_8UC1));//ģ��
//	tpls.push_back(imread("D:/Desktop/Hand/Hand/x64/Debug/hands/h1.jpg", CV_8UC1));//ģ��
//	tpls.push_back(imread("D:/Desktop/Hand/Hand/x64/Debug/hands/h5.jpg", CV_8UC1));//ģ��
//	int tSize = tpls.size();
//	for (int i = 0; i < tSize; i++) {
//		tpls.push_back(Mat());
//		resize(tpls[i], tpls[i + tSize], Size(), 2, 2);//ÿ��ģ��2���ߴ�
//	}
//
//	while (!stop)
//	{
//		cap >> frame;                       //��ȡ��Ƶ֡
//		flip(frame, frame, 1);				//����ת
//		//imshow("ԭʼͼ��",frame);
//		GaussianBlur(frame, frame, cv::Size(5, 5), 5, 5);//��˹�˲�
//		//imshow("��˹�˲�",frame);
//		//ת����ɫ�ռ䲢�ָ���ɫͨ��
//		cvtColor(frame, ycrcb_image, CV_BGR2YCrCb);
//		split(ycrcb_image, channels); //CV_EXPORTS void split(const Mat& m, vector<Mat>& mv );
//		Y = channels.at(0);
//		Cr = channels.at(1);
//		Cb = channels.at(2);
//
//		//һ���ͼ���ļ���ʽʹ�õ��� Unsigned 8bits��CvMat�����Ӧ�Ĳ������;���CV_8UC1��CV_8UC2��CV_8UC3������1��2��3��ʾͨ������Ʃ��RGB3ͨ������CV_8UC3��
//		result.create(frame.rows, frame.cols, CV_8UC1);
//
//		//Otsu��ֵ��ֵ���ָ�ͼ��
//		threshold(Cr, result, 0, 255, THRESH_BINARY + THRESH_OTSU);
//		//imshow("Otsu", result);
//
//		//���������ȸ�ʴ������
//		erode(result, result, element, Point(-1, -1), 2);
//		dilate(result, result, element, Point(-1, -1), 2);
//
//		//�ղ����������ͺ�ʴ
//		dilate(result, result, element, Point(-1, -1), 2);
//		erode(result, result, element, Point(-1, -1), 2);
//		//imshow("��̬ѧ����", result);
//
//		//ƽ����Ե
//		imageblur(result, result, Size(5, 5), 240);
//
//		//����
//		Rect tRect = outLine(10000);
//
//		string tip;
//		if (hand == tpls.size() - 1) {//����ָ����ֹͣ��¼
//			tip = "ֹͣ��¼";
//			if (hand == tpls.size() - 1) {
//				if (!path.empty()) {
//					int* features = getFeature(24);//�õ���������
//				}
//			}
//		}
//		else {
//			tip = "��ʼ��¼";
//			//���켣
//			drawLine(tRect);
//		}
//		// putTextZH(frame, tip.data() , Point(500, 100), Scalar(0, 255, 0), 30, "΢���ź�");
//		int fps = cap.get(CAP_PROP_FPS);
//		putText(frame, "fps" + to_string(fps), Point(10, 50), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 1);
//		imshow("frame", frame);
//		imshow("result", result);
//
//		if (waitKey(30) >= 0)
//			stop = true;
//	}
//
//	cv::waitKey();
//	// �ͷ����������ڴ�
//	cv::destroyAllWindows();
//	cap.release();
//	return 0;
//}