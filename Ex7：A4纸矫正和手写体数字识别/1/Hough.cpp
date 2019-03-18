#include <iostream>
#include "Hough.h"

using namespace std;

/* The constructor. */
Hough::Hough(const char *in, CImg<unsigned char> edge_image, int threshold, int r) {
	if(VERBOSE) cout << "Reading the image " << in << "." << endl;
	infilename = in;
	src.load(infilename);
	edge = edge_image;
	houghThreshold = threshold;
	range = r;
	w = src._width;
	h = src._height;

	int maxDis = sqrt(w * w + h * h);
	hough.assign(360, maxDis, 1, 1, 0);

	line.assign(w, h, 1, 3, 0);
	cimg_forXY(edge, x, y) {
		line(x, y, 0) = edge(x, y);
		line(x, y, 1) = edge(x, y);
		line(x, y, 2) = edge(x, y);
	}
}

/* Start the edge detection process. */
vector<Point> Hough::line_detection() {
    if(VERBOSE) cout << "Doing hough transformation." << endl;
    hough_transform();

    if(VERBOSE) cout << "Finding peak points." << endl;
    find_peak_points();
    cout << peaks.size() << endl;

    if(VERBOSE) cout << "Drawing lines." << endl;
    draw_lines();

    if(VERBOSE) cout << "Drawing intersections." << endl;
    draw_intersections();

	/****************************************************************************
	* Write out the edge image to a file.
	****************************************************************************/
    output();
    return intersections;
}

void Hough::output() {
	sprintf(outfilename, "%s_t_%d_r_%dedge.jpg", infilename,
			houghThreshold, range);
	if(VERBOSE) cout << "Writing the edge image in the file " 
					 << outfilename << "." << endl;
	line.save(outfilename);
	sprintf(outfilename, "%s_t_%d_r_%dsrc.jpg", infilename,
			houghThreshold, range);
	src.save(outfilename);
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
	hough.display();
}

void Hough::find_peak_points() {
	cimg_forXY(hough, t, p) {
		if (hough(t, p) >= houghThreshold) {
			bool flag = true;
			double theta = (t * M_PI) / 180;
			double m = -cos(theta) / sin(theta);
			double b = p / sin(theta);
			int xUp, xDown, yLeft, yRight;
			xUp = (0 - b) / m;
			xDown = (h - 1 - b) / m;
			yLeft = b;
			yRight = m * (w - 1) + b;
			if (xUp >= 0 && xUp < w ||
				xDown >= 0 && xDown < w ||
				yLeft >= 0 && yLeft < h ||
				yRight >= 0 && yRight < h) {
				// 直线在图像内
				for (int i = 0; i < peaks.size(); i++) {
					if (sqrt(pow(peaks[i].a - t, 2) + pow(peaks[i].b - p, 2)) < range) {
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

void Hough::draw_lines() {
	const unsigned char blue[] = {0, 0, 255};
	const unsigned char red[] = {255, 0, 0};
	for (int i = 0; i < peaks.size(); i++) {
		double theta = (peaks[i].a * M_PI) / 180;
		double p = peaks[i].b;
		double m = -cos(theta) / sin(theta);
		double b = p / sin(theta);
		if (b > 0) {
			cout << "y = " << m << " * x + " << abs(b) << endl;
		}
		else {
			cout << "y = " << m << " * x - " << abs(b) << endl;
		}
		int xUp, xDown, yLeft, yRight;
		int xStart, xEnd, yStart, yEnd;
		xUp = (0 - b) / m;
		xDown = (h - 1 - b) / m;
		yLeft = b;
		yRight = m * (w - 1) + b;
		if (yLeft >= 0 && yLeft < h) {
			xStart = 0;
			yStart = yLeft;
			if (xUp >= 0 && xUp < w) {
				xEnd = xUp;
				yEnd = 0;
			}
			else if (xDown >= 0 && xDown < w) {
				xEnd = xDown;
				yEnd = h - 1;
			}
			else {
				xEnd = w - 1;
				yEnd = yRight;
			}
		}
		else if (yRight >= 0 && yRight < h) {
			xStart = w - 1;
			yStart = yRight;
			if (xUp >= 0 && xUp < w) {
				xEnd = xUp;
				yEnd = 0;
			}
			else if (xDown >= 0 && xDown < w) {
				xEnd = xDown;
				yEnd = h - 1;
			}
		}
		else {
			xStart = xUp;
			yStart = 0;
			xEnd = xDown;
			yEnd = h - 1;
		}
		line.draw_line(xStart, yStart, xEnd, yEnd, blue);
		src.draw_line(xStart, yStart, xEnd, yEnd, blue);
	}
	cimg_forXY(line, x, y) {
		if (line(x, y, 0) == blue[0] &&
			line(x, y, 1) == blue[1] &&
			line(x, y, 2) == blue[2] && 
			edge(x, y) == EDGE) {
			line(x, y, 0) = red[0];
			line(x, y, 1) = red[1];
			line(x, y, 2) = red[2];
		}
	}
}

void Hough::draw_intersections() {
	const unsigned char blue[] = {0, 0, 255};
	for (int i = 0; i < peaks.size() - 1; i++) {
		double theta1 = (peaks[i].a * M_PI) / 180;
		double p1 = peaks[i].b;
		double m1 = -cos(theta1) / sin(theta1);
		double b1 = p1 / sin(theta1);
		for (int j = i + 1; j < peaks.size(); j++) {
			double theta2 = (peaks[j].a * M_PI) / 180;
			double p2 = peaks[j].b;
			double m2 = -cos(theta2) / sin(theta2);
			double b2 = p2 / sin(theta2);
			int x = -1, y = -1;
			// 求两直线交点
			if (m1 != m2) {
				x = (b2 - b1) / (m1 - m2);
            	y = (m1*b2 - m2*b1) / (m1 - m2);
			}
            if (x >= 0 && x < w && y >= 0 && y < h) {
            	cout << "(" << x << ", " << y << ")" << endl;
            	line.draw_circle(x, y, 50, blue);
            	src.draw_circle(x, y, 50, blue);
            	Point point(x, y, 0);
            	intersections.push_back(point);
            }
		}
	}
}