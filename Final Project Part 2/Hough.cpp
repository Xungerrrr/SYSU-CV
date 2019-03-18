#include <iostream>
#include "Hough.h"

using namespace std;

/* The constructor. */
Hough::Hough(CImg<unsigned char> edge_image, int threshold, int r) {
	edge = edge_image;
	houghThreshold = threshold;
	range = r;
	w = edge._width;
	h = edge._height;

	int maxDis = sqrt(w * w + h * h);
	hough.assign(360, maxDis, 1, 1, 0);
}

/* Start the edge detection process. */
vector<Point> Hough::line_detection() {
    if(VERBOSE) cout << "Doing hough transformation." << endl;
    hough_transform();

    if(VERBOSE) cout << "Finding peak points." << endl;
    find_peak_points();

    if(VERBOSE) cout << "Intersections are:" << endl;
    draw_intersections();
    return intersections;
}

void Hough::hough_transform() {
	cimg_forXY(edge, x, y) {
		if (edge(x, y) == EDGE) {
			cimg_forX(hough, t) {
				double theta = (t * M_PI) / 180;
				int p = x * cos(theta) + y * sin(theta);
				if (p >= 0 && p <= hough._height) {
					hough(t, p) += 1;
				}
			}
		}
	}
}

void Hough::find_peak_points() {
	while (peaks.size() != 4) {
		if (peaks.size() > 4) houghThreshold += 5;
		else if (peaks.size() != 0) houghThreshold -= 5;
		peaks.clear();
		cout << "   Finding with threshold: " << houghThreshold << endl;
		cimg_forXY(hough, t, p) {
			if (hough(t, p) >= houghThreshold) {
				bool flag = true;
				int xUp, xDown, yLeft, yRight;
				if (t != 0) {
					double theta = (t * M_PI) / 180;
					double m = -cos(theta) / sin(theta);
					double b = p / sin(theta);
					xUp = (0 - b) / m;
					xDown = (h - 1 - b) / m;
					yLeft = b;
					yRight = m * (w - 1) + b;
				}
				else {
					xUp = p;
					xDown = p;
				}
				if (xUp >= 0 && xUp < w ||
					xDown >= 0 && xDown < w ||
					yLeft >= 0 && yLeft < h ||
					yRight >= 0 && yRight < h) {
					// 直线在图像内
					for (int i = 0; i < peaks.size(); i++) {
						if (sqrt(pow(peaks[i].a - t, 2) + pow(peaks[i].b - p, 2)) < range ||
							sqrt(pow(abs(peaks[i].a - t) - 360, 2) + pow(peaks[i].b - p, 2)) < range) {
							// 存在局部最大值，比较大小
							flag = false;
							if (peaks[i].v < hough(t, p)) {
								// 大于局部最大值，则替换
								Point temp = Point(t, p, hough(t, p));
								peaks[i] = temp;
							}
						}
					}  
					if (flag) {
						// 不存在局部最大值，加入新的最大值
						Point temp = Point(t, p, hough(t, p));
						peaks.push_back(temp);
					}
				}
			}
		}
	}
}

void Hough::draw_intersections() {
	const unsigned char blue[] = {0, 0, 255};
	for (int i = 0; i < peaks.size() - 1; i++) {
		double theta1, p1, m1, b1;
		bool isVertical1;
		if (peaks[i].a != 0 && peaks[i].a != 180) {
			theta1 = (peaks[i].a * M_PI) / 180;
			p1 = peaks[i].b;
			m1 = -cos(theta1) / sin(theta1);
			b1 = p1 / sin(theta1);
			isVertical1 = false;
		}
		else {
			isVertical1 = true;
		}
		for (int j = i + 1; j < peaks.size(); j++) {
			double theta2, p2, m2, b2;
			bool isVertical2;
			if (peaks[j].a != 0 && peaks[j].a != 180) {
				theta2 = (peaks[j].a * M_PI) / 180;
				p2 = peaks[j].b;
				m2 = -cos(theta2) / sin(theta2);
				b2 = p2 / sin(theta2);
				isVertical2 = false;
			}
			else {
				isVertical2 = true;
			}
			int x = -1, y = -1;
			// 求两直线交点
			if (!isVertical1 && !isVertical2) {
				if (m1 != m2) {
					x = (b2 - b1) / (m1 - m2);
					y = (m1*b2 - m2*b1) / (m1 - m2);
				}
			}
			else if (isVertical1 && !isVertical2) {
				x = peaks[i].b;
				y = m2 * x + b2;
			}
			else if (isVertical2 && !isVertical1) {
				x = peaks[j].b;
				y = m1 * x + b1;
			}
            if (x >= 0 && x < w && y >= 0 && y < h) {
            	cout << "(" << x << ", " << y << ")" << endl;
            	Point point(x, y, 0);
            	intersections.push_back(point);
            }
		}
	}
}