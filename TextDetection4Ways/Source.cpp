#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include<cstdio>
#include<vector>

using namespace cv;
using namespace std;

void ThinSubiteration1(Mat & pSrc, Mat & pDst) {
	int rows = pSrc.rows;
	int cols = pSrc.cols;
	pSrc.copyTo(pDst);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (pSrc.at<float>(i, j) == 1.0f) {
				/// get 8 neighbors
				/// calculate C(p)
				int neighbor0 = (int)pSrc.at<float>(i - 1, j - 1);
				int neighbor1 = (int)pSrc.at<float>(i - 1, j);
				int neighbor2 = (int)pSrc.at<float>(i - 1, j + 1);
				int neighbor3 = (int)pSrc.at<float>(i, j + 1);
				int neighbor4 = (int)pSrc.at<float>(i + 1, j + 1);
				int neighbor5 = (int)pSrc.at<float>(i + 1, j);
				int neighbor6 = (int)pSrc.at<float>(i + 1, j - 1);
				int neighbor7 = (int)pSrc.at<float>(i, j - 1);
				int C = int(~neighbor1 & (neighbor2 | neighbor3)) +
					int(~neighbor3 & (neighbor4 | neighbor5)) +
					int(~neighbor5 & (neighbor6 | neighbor7)) +
					int(~neighbor7 & (neighbor0 | neighbor1));
				if (C == 1) {
					/// calculate N
					int N1 = int(neighbor0 | neighbor1) +
						int(neighbor2 | neighbor3) +
						int(neighbor4 | neighbor5) +
						int(neighbor6 | neighbor7);
					int N2 = int(neighbor1 | neighbor2) +
						int(neighbor3 | neighbor4) +
						int(neighbor5 | neighbor6) +
						int(neighbor7 | neighbor0);
					int N = min(N1, N2);
					if ((N == 2) || (N == 3)) {
						/// calculate criteria 3
						int c3 = (neighbor1 | neighbor2 | ~neighbor4) & neighbor3;
						if (c3 == 0) {
							pDst.at<float>(i, j) = 0.0f;
						}
					}
				}
			}
		}
	}
}

void ThinSubiteration2(Mat & pSrc, Mat & pDst) {
	int rows = pSrc.rows;
	int cols = pSrc.cols;
	pSrc.copyTo(pDst);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (pSrc.at<float>(i, j) == 1.0f) {
				/// get 8 neighbors
				/// calculate C(p)
				int neighbor0 = (int)pSrc.at<float>(i - 1, j - 1);
				int neighbor1 = (int)pSrc.at<float>(i - 1, j);
				int neighbor2 = (int)pSrc.at<float>(i - 1, j + 1);
				int neighbor3 = (int)pSrc.at<float>(i, j + 1);
				int neighbor4 = (int)pSrc.at<float>(i + 1, j + 1);
				int neighbor5 = (int)pSrc.at<float>(i + 1, j);
				int neighbor6 = (int)pSrc.at<float>(i + 1, j - 1);
				int neighbor7 = (int)pSrc.at<float>(i, j - 1);
				int C = int(~neighbor1 & (neighbor2 | neighbor3)) +
					int(~neighbor3 & (neighbor4 | neighbor5)) +
					int(~neighbor5 & (neighbor6 | neighbor7)) +
					int(~neighbor7 & (neighbor0 | neighbor1));
				if (C == 1) {
					/// calculate N
					int N1 = int(neighbor0 | neighbor1) +
						int(neighbor2 | neighbor3) +
						int(neighbor4 | neighbor5) +
						int(neighbor6 | neighbor7);
					int N2 = int(neighbor1 | neighbor2) +
						int(neighbor3 | neighbor4) +
						int(neighbor5 | neighbor6) +
						int(neighbor7 | neighbor0);
					int N = min(N1, N2);
					if ((N == 2) || (N == 3)) {
						int E = (neighbor5 | neighbor6 | ~neighbor0) & neighbor7;
						if (E == 0) {
							pDst.at<float>(i, j) = 0.0f;
						}
					}
				}
			}
		}
	}
}


void crossing(unsigned char* img, int width, int height, int *feature) {
	memset(feature, 0, width*height);
	for (int i = 0; i < height - 1; i++, img += width) {
		for (int j = 0; j < width - 1; j++) {
			if (img[j] != img[j + 1])feature[i]++;
			if (img[j] != img[j + width])feature[j + height]++;
		}
	}
}

void grid(unsigned char* img, int width, int height, int *feature) {
	memset(feature, 0, (width / 4)*(height / 4));
	for (int i = 0; i < height - 1; i++, img += width) {
		for (int j = 0; j < width - 1; j++) {
			int index = j / 4 + i / 4 * (width / 4);
			if (img[j]) feature[index]++;
		}
	}
}
//ROW
void rowProjection(Mat img, int *feature, int &average) {
	for (int i = 0; i < img.rows - 1; i++) {
		for (int j = 0; j < img.cols - 1; j++) {
			if ((int)img.at<uchar>(i, j)) {
				feature[i]++;
			}
		}
	}
	for (int i = 0; i < img.rows; i++) {
		average += feature[i];
	}
	average /= img.rows;


}
//COL
void colProjection(Mat img, int *feature, int &average) {
	for (int i = 0; i < img.rows - 1; i++) {
		for (int j = 0; j < img.cols - 1; j++) {
			if ((int)img.at<uchar>(i, j)) {
				feature[j]++;
			}
		}
	}
	for (int i = 0; i < img.cols; i++) {
		average += feature[i];
	}
	average /= img.cols;

}

void profile(unsigned char* img, int width, int height, int *feature) {
	memset(feature, 0, (width * 2) + (height * 2));
	unsigned char* buffer = img;
	int* f;
	for (int i = 0; i < height; i++, buffer += width) {
		for (int j = 0; j < width - 1; j++) {
			if (buffer[j] == 0 && buffer[j + 1] == 0) {
				feature[i]++;
			}
			else
				break;
		}
		f = feature + height;
		for (int j = width - 1; j > 0; j--) {
			if (buffer[j] == 0 && buffer[j - 1] == 0) {
				f[i]++;
			}
			else
				break;
		}
	}
	for (int j = 0; j < width; j++) {
		buffer = img;
		f = feature + 2 * height;
		for (int i = 0; i < height - 1; i++, buffer += width) {
			if (buffer[j] == 0 && buffer[j + width] == 0) {
				f[j]++;
			}
			else
				break;
		}
		buffer = img + (height - 1)*width;
		f = feature + 2 * height + width;
		for (int i = height - 1; i > 0; i--) {
			if (buffer[j] == 0 && buffer[j - width] == 0) {
				f[j]++;
			}
			else
				break;
		}
	}
}

Mat1b getDisk(int M)
{
	// M positive and multiple of 3
	CV_Assert((M > 0) && ((M % 3) == 0));

	int k = M / 3;
	int rows = M * 2 + 1;
	int cols = M * 2 + 1;
	Point c(M, M);

	Mat1b strel(rows, cols, uchar(0));


	vector<Point> vertices(8);
	vertices[0].y = c.x - k;
	vertices[0].x = 0;
	vertices[1].y = c.x + k;
	vertices[1].x = 0;

	vertices[2].y = cols - 1;
	vertices[2].x = c.y - k;
	vertices[3].y = cols - 1;
	vertices[3].x = c.y + k;

	vertices[4].y = c.x + k;
	vertices[4].x = rows - 1;
	vertices[5].y = c.x - k;
	vertices[5].x = rows - 1;

	vertices[6].y = 0;
	vertices[6].x = c.y + k;
	vertices[7].y = 0;
	vertices[7].x = c.y - k;

	fillConvexPoly(strel, vertices, Scalar(1));

	return strel;

}



int main()
{
	VideoCapture cap(0);
	if (!cap.isOpened()) return -1;

	for (;;) {
		Mat frame, skin;
		cap >> frame;
		Rect position = Rect(frame.cols / 4, frame.rows / 4, frame.cols / 2, frame.rows / 6), temp;
		Mat roi(frame, position);

		Mat roiStrel;
		cvtColor(roi, roiStrel, CV_BGR2GRAY);
		Mat roiGray;
		roiStrel.copyTo(roiGray);
		Mat1b kernel = getDisk(3);
		morphologyEx(roiStrel, roiStrel, MORPH_ERODE, kernel);
		Mat roiSubtract = roiGray - roiStrel;
		double fmax, fmin;
		minMaxLoc(roiSubtract, &fmin, &fmax);

		double level = (fmax - (fmax - fmin) / 3);

		threshold(roiSubtract, roiSubtract, level, 255, THRESH_BINARY);
		rectangle(frame, position, Scalar(0, 255, 255), 1, 8, 0);

		//fill
		Mat im_floodfill = roiSubtract.clone();
		floodFill(im_floodfill, Point(0, 0), Scalar(255));
		Mat im_floodfill_inv;
		bitwise_not(im_floodfill, im_floodfill_inv);
		Mat im_out = (roiSubtract | im_floodfill_inv);


		int *featureMaxRowRoi = new  int[roiSubtract.rows]();
		int avgRow = 0;
		rowProjection(im_floodfill_inv, featureMaxRowRoi, avgRow);

		int startP = 0, endP = 0, max = 0;


		IplImage *image = cvCreateImage(cvSize(500, 500), 8, 3);
		for (int i = 0; i < im_floodfill_inv.rows - 1; i++) {
			int t_startP, t_max = 0;
			if (featureMaxRowRoi[i] >= avgRow / 2) {
				t_startP = i;
				while (i < im_floodfill_inv.rows - 1 && featureMaxRowRoi[i++] >= avgRow / 2) {
					t_max = i - t_startP;
					if (t_max > max) {
						startP = t_startP;
						endP = i;
					}
				}
			}
		}

		Mat maxColRoi(im_floodfill_inv, Rect(0, startP, im_floodfill_inv.cols, endP - startP));

		int *featureMaxColRoi = new  int[maxColRoi.cols]();
		int avgCol = 0;
		colProjection(maxColRoi, featureMaxColRoi, avgCol);

		for (int i = 0; i < maxColRoi.cols - 1; i++) {
			int t_startP, t_endP = 0;
			if (featureMaxColRoi[i] >= 3) {
				t_startP = i;
				while (featureMaxColRoi[i++] >= 3) {}
				t_endP = i;
				if (t_endP - t_startP > 5) {
					cout << t_startP << " " << t_endP << endl;
					rectangle(roi, Rect(t_startP, startP, t_endP - t_startP, endP - startP), Scalar(0, 0, 255), 1, 8, 0);
				}
			}
			//cvLine(image, cvPoint(i, 0), cvPoint(i, featureMaxColRoi[i]), cvScalar(255, 255, 255));
		}
		ThinSubiteration1(maxColRoi, maxColRoi);
		ThinSubiteration2(maxColRoi, maxColRoi);
		if (maxColRoi.cols != 0 && maxColRoi.rows != 0)//avoid crashing
			imshow("maxColRoi", maxColRoi);
		else
			continue;

		/*cvShowImage("image", image);
		cvReleaseImage(&image);*/

		imshow("flood_inv", im_floodfill_inv);
		imshow("frame", frame);
		imshow("roi", roi);
		//imshow("roiSubtract ", roiSubtract);

		if (waitKey(30) >= 0) break;
	}

	return 0;
}
