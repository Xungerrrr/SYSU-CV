#include <iostream>
#include "Hough.h"

using namespace std;

/* The constructor. */
Hough::Hough(char *in, CImg<unsigned char> edge_image, int threshold, float f, int minR, int maxR) {
	if(VERBOSE) cout << "Reading the image " << in << "." << endl;
	infilename = in;
	src.load(infilename);
	edge = edge_image;
	houghThreshold = threshold;
	w = src._width;
	h = src._height;
	fraction = f;
	minRadius = minR;
	maxRadius = maxR;
	circle.assign(w, h, 1, 3, 0);
	cimg_forXY(edge, x, y) {
		circle(x, y, 0) = edge(x, y);
		circle(x, y, 1) = edge(x, y);
		circle(x, y, 2) = edge(x, y);
	}
}

/* Start the circle detection process. */
void Hough::circle_detection() {
    if(VERBOSE) cout << "Doing hough transformation." << endl;
    hough_transform();

	/****************************************************************************
	* Write out the circle image to a file.
	****************************************************************************/
    output();
}

void Hough::output() {
	sprintf(outfilename, "%s_t_%d_f_%.2fedge.jpg", infilename,
			houghThreshold, fraction);
	if(VERBOSE) cout << "Writing the circle image in the file " 
					 << outfilename << "." << endl;
	circle.save(outfilename);
	sprintf(outfilename, "%s_t_%d_f_%.2fsrc.jpg", infilename,
			houghThreshold, fraction);
	if(VERBOSE) cout << "Writing the circle image in the file " 
					 << outfilename << "." << endl;
	src.save(outfilename);
}

void Hough::hough_transform() {
	int maxDis = sqrt(w * w + h * h);
	const unsigned char blue[] = {0, 0, 255};
	const unsigned char red[] = {255, 0, 0};
	for (int r = minRadius; r < maxRadius; r += 1) {
		hough.assign(w, h, 1, 1, 0);
		peakGroups.clear();
		cimg_forXY(edge, x, y) {
			if (edge(x, y) == EDGE) {
				for (int t = 0; t < 360; t++) {
					double theta = (t * M_PI) / 180;
					int a = x + r * cos(theta);
					int b = y + r * sin(theta);
					if (a >= 0 && a < w && b >= 0 && b < h) {
						hough(a, b)++;
					}
					
				}
			}
		}
		
		cimg_forXY(hough, a, b) {
			if (hough(a, b) >= houghThreshold) { // 大于阈值
				bool flag = true;
				for (int i = 0; i < peakGroups.size(); i++) {
					for (int j = 0; j < peakGroups[i].size(); j++) {
						if (sqrt(pow(peakGroups[i][j].a - a, 2) + pow(peakGroups[i][j].b - b, 2)) < 5) {
							// 有局部分组区域，则将点加入分组
							flag = false;
							Point temp = Point(a, b, hough(a, b));
							peakGroups[i].push_back(temp);
							break;
						}
					}
					if (!flag) {
						break;
					} 
                }  
                if (flag) {
                	// 没有局部分组区域，则创建新分组，将点加入
                	vector<Point> newGroup;
                	Point temp = Point(a, b, hough(a, b));
                	newGroup.push_back(temp);
                	peakGroups.push_back(newGroup);
                }
			}
		}
		for (int i = 0; i < peakGroups.size(); i++) {
			int a = 0, b = 0, v = 0;
			for (int j = 0; j < peakGroups[i].size(); j++) {
				a += peakGroups[i][j].a;
				b += peakGroups[i][j].b;
				v += peakGroups[i][j].v;
			}
			if (v >  fraction * M_PI * r) {
				// 如果分组内投票值之和超过给定的比例，则该分组构成一个圆。
				// 圆心坐标为分组内坐标的平均值
				a /= peakGroups[i].size();
				b /= peakGroups[i].size();
				bool flag = true;
				for (int j = 0; j < circles.size(); j++) {
					if (pow(circles[j].a - a, 2) + pow(circles[j].b - b, 2) < r * r / 9 &&
						abs(circles[j].v - r) < r / 10) {
						// 如果该圆与已有的圆接近，则算作同一个圆
						// 计算两个圆的坐标和半径平均值，替代原有圆的圆心坐标和半径
						flag = false;
						circles[j].a = (circles[j].a + a) / 2;
						circles[j].b = (circles[j].b + b) / 2;
						circles[j].v = (circles[j].v + r) / 2;
					}
				}
				if (flag) {
					// 如果不存在接近的圆，则新建一个圆
					Point newCircle(a, b, r);
					circles.push_back(newCircle);
				}
			}
		}
	}
	for (int i = 0; i < circles.size(); i++) {
		src.draw_circle(circles[i].a, circles[i].b, circles[i].v, blue, 1, 1);
		circle.draw_circle(circles[i].a, circles[i].b, circles[i].v, blue, 1, 1);
	}
	cimg_forXY(edge, x, y) {
		if (circle(x, y, 0) == blue[0] &&
			circle(x, y, 1) == blue[1] &&
			circle(x, y, 2) == blue[2] &&
			edge(x, y) == EDGE) {
			circle(x, y, 0) = red[0];
			circle(x, y, 1) = red[1];
			circle(x, y, 2) = red[2];
		}
	}

	cout << "Number of circles: " << circles.size() << endl;
}