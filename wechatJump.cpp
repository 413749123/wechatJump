#include "stdafx.h"
#include <iostream>
#include <vector>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\features2d.hpp>
using namespace std;
using namespace cv;
int main()
{
	int img_cout = 1;//ͼƬ���
for (size_t times = 0; times < 1000; times++)
{
	system("adb shell screencap -p /sdcard/jump.png && adb pull /sdcard/jump.png");//��ȡ��ͼ
	Mat imJumpRaw = imread("jump.png");//��ȡ��ͼ
	//�ü���������Լ400px
	Mat imJump(imJumpRaw.rows - 400, imJumpRaw.cols, imJumpRaw.type());
	for (size_t i = 0; i < imJump.rows; i++)
	{
		for (size_t j = 0; j < imJump.cols; j++)
		{
			imJump.at<Vec3b>(i, j) = imJumpRaw.at<Vec3b>(i+400, j);
		}
	}
	Mat imPlayer = imread("player.jpg");//��ȡ����
	
	//ģ��ƥ��ʶ������λ��
	Point playerLocation(0, 0);//���ӵײ�����λ��
	Mat res(imJump.rows-imPlayer.rows+1,imJump.cols-imPlayer.cols+1,CV_32FC1);
	matchTemplate(imJump, imPlayer, res, TM_CCOEFF_NORMED);
	double minv = 0, maxv = 0; Point minp, maxp;
	minMaxLoc(res, &minv, &maxv, &minp, &maxp);
	cout <<minv<<"  "<<maxv<< maxp <<minp<< endl;
	if (maxv > 0.5) { //���ĸ���
		playerLocation.x = maxp.x + 39;
		playerLocation.y = maxp.y + 189;
	}
	//��˹�˲�����Ե��ȡ
	Mat imTempCannied = imJump.clone();
	Canny(imJump, imTempCannied, 30, 15);
	//��ȡ���ӵ���ߴ�����
	Point2i girdTop(1000000,1000000);
	for (size_t i = 0; i < imTempCannied.rows; i++)
	{
		for (size_t j = 0; j < imTempCannied.cols; j++)
		{
			if (imTempCannied.at<uchar>(i, j) == 255)
			{
				if (i < girdTop.y) girdTop = Point2i(j, i);
			}
		}
	}
	//�������
	Rect boxRect(Point2i(0,0),Point2i(0,0));//boxRect��Ϊ������
	floodFill(imJump, Point2i(girdTop.x,girdTop.y+20), Scalar(255, 255, 255),&boxRect);
	//�������ʱ��
	Point2i nextLocation(boxRect.x + boxRect.width / 2, boxRect.y + boxRect.height / 2);
	double dis = sqrt((playerLocation.x - nextLocation.x)*(playerLocation.x - nextLocation.x) + (playerLocation.y - nextLocation.y)*(playerLocation.y - nextLocation.y));
	double t = dis*1.395;
	string strCommand = "adb shell input swipe 500 1600 500 1602 "+to_string(int(t));
	cout << strCommand << endl;
	system(strCommand.c_str());

	//���ƶ�λ��ͼ
	line(imJump, playerLocation, nextLocation, Scalar(0, 0, 255), 2);
	line(imJump, Point2i(playerLocation.x, 0), Point2i(playerLocation.x, imJump.rows), Scalar(0, 255, 0));
	line(imJump, Point2i(0, playerLocation.y), Point2i(imJump.cols, playerLocation.y), Scalar(0, 255, 0));
	line(imJump, Point2i(nextLocation.x, 0), Point2i(nextLocation.x, imJump.rows), Scalar(0, 255, 0));
	line(imJump, Point2i(0, nextLocation.y), Point2i(imJump.cols, nextLocation.y), Scalar(0, 255, 0));
	circle(imJump, playerLocation, 20, Scalar(255, 0, 0));
	circle(imJump, nextLocation, 20, Scalar(255, 0, 0));
	imwrite("img/" + to_string(img_cout)+"_"+to_string(rand())+ ".png", imJump);
	img_cout++;
	system("sleep 1");
}
	system("pause");
    return 0;
}