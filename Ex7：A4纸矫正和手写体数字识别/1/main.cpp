#include "OSTU.cpp"
#include "Warping.cpp"
#include <iostream>

// 测试函数
void test(const char* path) {
	// 读取图片
	CImg<unsigned char> src(path);
	CImg<unsigned char> image(src.width(), src.height(), 1, 1, 0);

	// 转灰度图
	cimg_forXY(src, x, y) {
		int r = src(x, y, 0);
		int g = src(x, y, 1);
		int b = src(x, y, 2);
		int grey = (r * 30 + g * 59 + b * 11 + 50) / 100;
		image(x, y) = grey;
	}

	// 图像分割和边缘提取
	OSTU o(image);
	o.ostu();
	CImg<unsigned char> edge = o.divide();

	// 霍夫变换，拟合直线
	int t, r;
	cout << "input threshold and range:" << endl;
	cin >> t >> r;
	Hough hough(path, edge, t, r);
	vector<Point> intersections = hough.line_detection();

	// A4纸矫正
	Warping warping(intersections, src);
	warping.getPointPair();
	warping.calculateHomography();

	// 输出结果
	char result[20];
	sprintf(result, "%s_%s.jpg", path, "crop");
	warping.warp().save(result);
}

int main() {
	char path[20];
	cout << "input file path:" << endl;
	cin >> path;
    test(path);
    return 0;
}
