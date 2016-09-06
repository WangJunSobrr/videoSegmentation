#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "grabcutWithRect.h"
#include "VideoSnapCut.h"
#include "kcftracker.hpp"
#include <fstream>

//using namespace std;
using namespace cv;

#define K 40                   ///�ӵڼ�֡��ʼ
//#define start 1

GCApplication gt;

void on_mouse( int event, int x, int y, int flags, void* param )
{
    gt.mouseClick( event, x, y, flags, param );
}
//void snapcut();
//PolygonF* pyF;
//

int main1(void)
{
	/*��������ģ��*/
	//���ٲ�������
	bool HOG = true;
	bool FIXEDWINDOW = true;
	bool MULTISCALE = false;
	bool SILENT = true;
	bool LAB = false;
	//����ģʽ�������ã�0��ʾ����Ƶ���룬1��ʾ��֡��������
	bool inputFlag = true;
	
	Mat curFrame;
	if(inputFlag == false)
	{
		VideoCapture cap;
		cap.open("E:\\test\\03.avi");
		curFrame = gt.getKFrame(K, cap);  //�����k֡��cap���ִӵ�k֡�������
	}
	
	
	//�ؼ�֡��־
	bool flag = false;       
	bool fromKeyFlag = true;

	int start = 0;
	int count  = start;      //֡���
    char* name = new char[100];
	char* path =  "E:\\yyc\\davis\\JPEGImages\\480p\\rhino\\%05d.jpg";//"E:\\yyc\\test\\images\\%04d.jpg";//"E:\\videoCut\\SegTrackv2\\SegTrackv2\\JPEGImages\\worm\\%010d.png";   // "E:\\videoCut\\SegTrackv2\\SegTrackv2\\JPEGImages\\bird_of_paradise\\bird_of_paradise_%05d.png";//"E:\\videoCut\\SegTrackv2\\SegTrackv2\\JPEGImages\\parachute\\parachute_%05d.png";//"E:\\videoCut\\SegTrackv2\\SegTrackv2\\JPEGImages\\monkey\\%010d.png"; // "E:\\test\\images\\%04d.jpg";   //"E:\\videoCut\\SegTrackv2\\SegTrackv2\\JPEGImages\\monkey\\%010d.png";	
	
	const string keyFrameWin = "��һ֡��ؼ�֡��������";
	
	while(1)  //��Ƶ�ָ�ѭ��
	{

		if(count == start || flag == true) //��һ֡��ؼ�֡ʱ���еĴ���
		{
			gt.mVideoSnapCut->setFrameNum(count);
			if(inputFlag)
			{
				std::sprintf(name, path, count);  
				curFrame = imread(name);
			}

			gt.mVideoSnapCut -> SetCurFrameMat(curFrame);
			gt.mVideoSnapCut -> SetCurFrame(count);

			cv::namedWindow(keyFrameWin, WINDOW_AUTOSIZE);    //�ؼ�֡����
			gt.setImageAndWinName(curFrame, keyFrameWin);
			setMouseCallback(keyFrameWin, on_mouse, 0);
			gt.showImage();

			for(;;)       //�Թؼ�֡�Ķ�ν����ָ�
			{
				int c = waitKey(0);
				if(c == 'r')
				{
					cout <<"��Ǵ��������ã�"<< endl;
					gt.reset();           //ǰ���ͱ�����������������㣬���������㣬������������
					//gt.showJiaoHuImage();       //չʾͼ����Ϊ����ͼ���ϱ�����ӵ�
					gt.showImage();
				}
				else if(c == 'n')
				{
					//int iterCount = gt.getIterCount();
					//cout << "<�ؼ�֡��" << iterCount << "�ηָ�... ";
					//int newIterCount = gt.nextIter(); //�ؼ�֡�ָ���ɣ����طָ����
					//if( newIterCount > iterCount )
					//{
					//	//gt.saveCutResult();
					//	gt.showImage();
					//	Mat tmp = gt.resMask;         //�õ���ɫ�ָ���
					//	Mat tmp1 = gt.resbinMask ;    //�õ�0��1��ֵ�ָ���
					//	imshow("�ָ���", tmp);					
					//	cout << iterCount << "���>" << endl;
					//}
					//else
					//	cout << "rect must be determined>" << endl;
					char* nameg = new char[100];
					char* pathg = "E:\\yyc\\davis\\Annotations\\480p\\rhino\\%05d.png";//"E:\\videoCut\\SegTrackv2\\SegTrackv2\\JPEGImages\\parachute\\parachute_%05d.png";//"E:\\videoCut\\SegTrackv2\\SegTrackv2\\JPEGImages\\worm\\%010d.png";   // "E:\\videoCut\\SegTrackv2\\SegTrackv2\\JPEGImages\\bird_of_paradise\\bird_of_paradise_%05d.png";//"E:\\videoCut\\SegTrackv2\\SegTrackv2\\JPEGImages\\parachute\\parachute_%05d.png";//"E:\\videoCut\\SegTrackv2\\SegTrackv2\\JPEGImages\\monkey\\%010d.png"; // "E:\\test\\images\\%04d.jpg";   //"E:\\videoCut\\SegTrackv2\\SegTrackv2\\JPEGImages\\monkey\\%010d.png";	
					std::sprintf(nameg, pathg, count); 
					Mat groundtruthMask = imread(nameg,CV_LOAD_IMAGE_GRAYSCALE);					
					groundtruthMask.copyTo(gt.resbinMask);
				}
				else if(c == 's')  //������һ֡��snapcut�ָ�
				{
					count++;        //��ǰ֡�ָ���˽������֡
					flag = false;   //snapcut�ָ��֡���ǹؼ�֡
					fromKeyFlag = true;    //��һ֡�ǹؼ�֡��־λ
					break;					
				}			
			}
		}
		else //�ǹؼ�֡�Ĵ���
		{
			cout << "��" << count << "֡snapcut�ָʼ" << endl;
			gt.mVideoSnapCut->setFrameNum(count);
			Mat con , combine;
			//���������ֵ����
			if(fromKeyFlag)					//��һ֡�ǹؼ�֡
			{
				gt.resbinMask.copyTo(con);  //0��1�Ķ�ֵ����
				fromKeyFlag = false;
			}				
			else                            //��һ֡���ǹؼ�֡
				gt.mVideoSnapCut->nextContourMat.copyTo(con);
			con.copyTo(combine);
			con.copyTo(gt.mVideoSnapCut->binMask1);//���ָ��ֵ������ݵ�snapcut��
			
			gt.mVideoSnapCut -> generateContourWithRect(con);

			vector<Rect>polyRect;			
			Mat nextFrame, updateTrack;
			curFrame.release();

			sprintf(name, path, count);//�����µ�֡
		    curFrame = imread(name);
			curFrame.copyTo(nextFrame);
			curFrame.copyTo(updateTrack);                     //��ǰ֡��Ϊ���¾ֲ����ٿ��֡

			//vector<Rect> updateRects;
			//for(int i = 0; i < num - 1; i++)
			//{
			//	Rect updateRect = trackers[i].update(curFrame);
			//	updateRects.push_back(updateRect);
			//	//rectangle( updateTrack, Point( updateRect.x, updateRect.y ), Point( updateRect.x+updateRect.width, updateRect.y+updateRect.height), Scalar( 0, 0, 255 ), 1, 8 );
			//}
			//vector<Rect> updateRectsNew;
			//gt.updateRects(updateRects, updateRectsNew);
			//Mat updateTrackNew;
			//updateTrack.copyTo(updateTrackNew);
			//
			//gt.showRects(updateTrack, updateRects);
			//imshow("�ֲ����ٽ��",updateTrack);
			//gt.showRects(updateTrackNew, updateRectsNew);
			//imshow("�ֲ����ٽ�����Ȼ�",updateTrackNew);
			//waitKey();
			//��һ�׶ε������ֲ����ٿ������ȫ�ָ��ٿ��λ�ñ仯��Ӧ��̫��
			//�ڶ��׶θ��ٿ�ĵ�������canny����
			//Mat canny = gt.getCanny(curFrame);
			//waitKey();
			//�������ٿ��λ��
			/*for(int i = 0; i < num - 1; i++)
			{

			}*/

			//��������ٿ��ڵ�ͼ������Ե���

					
			/*gt.mVideoSnapCut -> SetCurFrameMat(tmp);
			gt.mVideoSnapCut -> SetCurFrame(count);*/
			gt.mVideoSnapCut -> CreateMasks();
			gt.mVideoSnapCut -> mCombinedMask = combine;
			gt.mVideoSnapCut -> RegisterCurFrameWithFrame(nextFrame);
			vector<Mat> vM = gt.mVideoSnapCut -> mAfterTransContours;      //register֮��
			vector<vector<Point>> vvP;
			Mat tmpNextFrame;
			nextFrame.copyTo(tmpNextFrame);

			vector<PolygonF*> ForegroundBorderPolygons;
			for(int i = 0; i < vM.size(); i++)
			{
				gt.transMatTocontours(vM[i], vvP);
				for(int j = 0; j < vvP.size(); j++)
				{
					PolygonF* pyF = gt.showContour(vvP[j], tmpNextFrame, VideoSnapCut::mMaskSize*2.0f/3, polyRect, 1);					
					ForegroundBorderPolygons.push_back(pyF);	//���߽緵�ص����������ϵĵ���뵽������
					imshow("����任��ľ��Ȳ�����", tmpNextFrame);

					char* path1 = "E:\\shiyanResult\\��%d֡����任��ľ��Ȳ�����.jpg";//�洢����任��ľ��Ȳ�����
					char* name1 = new char[100];
					sprintf(name1, path1, count);
					imwrite(name1, tmpNextFrame);
					delete[] name1;
				}
			}
			gt.mVideoSnapCut -> SetForegroundBorderPolygons(&ForegroundBorderPolygons);		
			gt.mVideoSnapCut -> BuildColorClassifiers(); 
			gt.mVideoSnapCut -> ClassifyPixels();
			gt.mVideoSnapCut -> SetCurFrameMat(nextFrame);
			fromKeyFlag = false;
			count++;
			//waitKey();
		}		
	}

	return 0;
}

void testContours(const Mat& con)
{
	Mat con1;
	con.copyTo(con1);
	Point p;
	for(p.x = 0; p.x < con1.cols; p.x++)
		for(p.y = 0; p.y < con1.rows; p.y++)
		{
			if(con1.at<uchar>(p) > 0)
				con1.at<uchar>(p) = 255;
			else
				con1.at<uchar>(p) = 0;
		}
	imshow("groundtruth", con1);
	waitKey();
			
	imwrite("E:\\yyc\\shiyanResult\\binres.jpg", con1);
	cv::vector<cv::vector<Point>> contours;
	cv::vector<Vec4i> hierarchy;
	findContours(con1, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
}