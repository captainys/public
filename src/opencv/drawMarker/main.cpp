#include <ysclass.h>
#include <ysbitmap.h>

#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>


int main(void)
{
	auto dict=cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_1000);
	for(int i=0; i<10; ++i)
	{
		cv::Mat img(200,200,CV_8UC3);
		cv::aruco::drawMarker(dict,i,200,img,1);

		YsString str;
		str.Printf("DICT_4X4_1000_%03d.png",i);
		cv::imwrite(str.data(),img);
	}

	dict=cv::aruco::getPredefinedDictionary(cv::aruco::DICT_5X5_1000);
	for(int i=0; i<10; ++i)
	{
		cv::Mat img(200,200,CV_8UC3);
		cv::aruco::drawMarker(dict,i,200,img,1);

		YsString str;
		str.Printf("DICT_5X5_1000_%03d.png",i);
		cv::imwrite(str.data(),img);
	}

	return 0;
}

