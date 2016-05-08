﻿#include "skin_color.hpp"

Mat findMaxContours(Mat skin, Mat frame, Rect &temp) {

	int largest_area = 0;
	int largest_contour_index = 0;

	vector<vector<Point>> contours; // Vector for storing contour
	vector<Vec4i> hierarchy;
	findContours(skin, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the images

	for (int i = 0; i < contours.size(); i++) // iterate through each contour.
	{
		double a = contourArea(contours[i], false);  //  Find the area of contour
		if (a > largest_area) {
			largest_area = a;
			largest_contour_index = i;                //Store the index of largest contour
			temp = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
		}
	}
	Mat black(skin.rows, skin.cols, CV_8UC1, Scalar::all(0));//bulid black Mat
	drawContours(black, contours, largest_contour_index, Scalar(255, 255, 255), CV_FILLED, 8, hierarchy);
	temp.x = temp.x + (temp.width / 4);
	temp.y = temp.y + (temp.height / 4);
	temp.width = temp.width / 2;
	temp.height = temp.height / 2;
	rectangle(frame, temp, Scalar(0, 255, 0), 1, 8, 0);
	imshow("src", frame);//source image
	imshow("largest Contour", black);// Largest Contour
	skin = black;
	return black;
}


double heartBeat(VideoCapture cap, clock_t &time, bool &t, int &Heartbeat, double &red_avg, double &pre_red_avg) {
	Mat frame, skin;
	cap >> frame;
	flip(frame, frame, 1);
	//resize(frame, frame, Size(frame.cols/2,frame.rows/2));
	skin = skincolor(frame);

	Rect temp;
	skin = findMaxContours(skin, frame, temp);
	imshow("skin", skin);

	///////////////////
	double real_heartbeat = 0; //每分鐘心跳
							   ///////////////////
	double red_total = 0.0, passthru = 0.0;
	for (int i = temp.y; i < temp.y + temp.height; i++) {
		for (int j = temp.x; j < temp.x + temp.width; j++) {
			if (skin.ptr<uchar>(i, j)[0] == 255) {//if it's Skin do the process of adding red_scale for averaging
				red_total += (int)frame.ptr<uchar>(i, j)[2];
				passthru++;
			}
		}
	}
	red_avg = red_total / passthru;
	if (pre_red_avg == 0)
		pre_red_avg = red_avg;
	//cout << fixed << setprecision(3);
	if (red_avg - red_thres > pre_red_avg) {
		if (!t) {
			Heartbeat++;
			time = clock();
			//cout << Heartbeat / 2 << endl;
			real_heartbeat = Heartbeat / 2 / (time / (double)(CLOCKS_PER_SEC)) * 60;
			//cout << "心跳:" << Heartbeat / 2 / (time / (double)(CLOCKS_PER_SEC)) * 60 << "每分鐘" << endl;
			t = !t;
		}
	}
	else if (red_avg + red_thres < pre_red_avg) {
		if (t) {
			Heartbeat++;
			time = clock();
			//cout << Heartbeat / 2 << endl;
			real_heartbeat = Heartbeat / 2 / (time / (double)(CLOCKS_PER_SEC)) * 60;
			//cout << "心跳:" << Heartbeat / 2 / (time / (double)(CLOCKS_PER_SEC)) * 60 << "每分鐘" << endl;
			t = !t;
		}
	}
	pre_red_avg = red_avg;
	return (Heartbeat / 2 / (time / (double)(CLOCKS_PER_SEC)) * 60);
}

int main() {
	clock_t time;
	VideoCapture cap(0);
	if (!cap.isOpened()) return -1;
	bool t = true;
	int Heartbeat = 0;
	double red_avg = 0, pre_red_avg = 0;
	for (;;) {
		printf("%.2lf\n", heartBeat(cap, time, t, Heartbeat, red_avg, pre_red_avg));
		if (waitKey(30) >= 0) break;
	}
	return 0;
}
