#include "Morphing.h"

Morphing::Morphing(const char* sourcePath, const char* targetPath) {
	source.load(sourcePath);
	target.load(targetPath);
}

// 获取图像标记点
void Morphing::getPoints() {
	selectPoints(source, sourcePoint, "1.txt");
	selectPoints(target, targetPoint, "2.txt");
}

// 根据标记点，获取图像的三角划分
void Morphing::getTriangles() {
	int size = sourcePoint.size();
	// 遍历所有三角形
	for (int i = 0; i < size - 2; i++) {
		for (int j = i + 1; j < size - 1; j++) {
			for (int k = j + 1; k < size; k++) {
				Point A = sourcePoint[i];
				Point B = sourcePoint[j];
				Point C = sourcePoint[k];
				Triangle tri(A, B, C, i, j, k);
				bool valid = true;

				// 遍历所有标记点
				for (int m = 0; m < size; m++) {
					Point temp = sourcePoint[m];
					if (m == i || m == j || m == k) 
						continue;
					double k1 = (double)(A.y - B.y) / (A.x - B.x);
                    double k2 = (double)(A.y - C.y) / (A.x - C.x);
                    // 如果有标记点在三角形的外接圆内，或者三角形三点在一条直线上，则三角形不符合要求
                    if (isPointInCircle(temp, tri) || k1 == k2 || (A.x == B.x && B.x == C.x)) {
                        valid = false;
                        break;
                    }
				}
				// 若符合要求，则加入到集合中
				if (valid) {
					sourceTriangle.push_back(tri);
				}
			}
		}
	}
	// 根据原图三角形顶点的下标，形成目标图的三角划分，以保证三角形一一对应
	for (int i = 0; i < sourceTriangle.size(); i++) {
		int index1 = sourceTriangle[i].index[0];
		int index2 = sourceTriangle[i].index[1];
		int index3 = sourceTriangle[i].index[2];
		Triangle tri(targetPoint[index1], targetPoint[index2], targetPoint[index3],
			index1, index2, index3);
		targetTriangle.push_back(tri);
	}
}

// 计算所有过渡帧
void Morphing::calculateMiddleFrames() {
	// 加入第一帧
	morphing.push_back(source);
	// 计算所有中间帧
	for (int i = 1; i < TOTAL_FRAME - 1; i++) {
		double a = (double)i / (TOTAL_FRAME - 1);	// 线性变换的权重a
		CImg<unsigned char> middle(target.width(), target.height(), 1, 3, 0);	// 当前帧的图像
		std::vector<Triangle> middleTriangle;		// 当前帧的三角划分
		std::vector<CImg<double> > middleToSource;	// 当前帧的三角形到原图三角形的仿射变换的所有变换矩阵的集合
		std::vector<CImg<double> > middleToTarget;	// 当前帧的三角形到目标图三角形的仿射变换的所有变换矩阵的集合

		// 根据权重a，得到当前帧的三角划分
		for (int j = 0; j < sourceTriangle.size(); j++) {
			// 得到三角形顶点下标
			int index1 = sourceTriangle[j].index[0];
			int index2 = sourceTriangle[j].index[1];
			int index3 = sourceTriangle[j].index[2];
			// 计算三角形顶点坐标
			int mid_x1 = (1 - a) * sourcePoint[index1].x + a * targetPoint[index1].x + 0.5;
			int mid_y1 = (1 - a) * sourcePoint[index1].y + a * targetPoint[index1].y + 0.5;
			int mid_x2 = (1 - a) * sourcePoint[index2].x + a * targetPoint[index2].x + 0.5;
			int mid_y2 = (1 - a) * sourcePoint[index2].y + a * targetPoint[index2].y + 0.5;
			int mid_x3 = (1 - a) * sourcePoint[index3].x + a * targetPoint[index3].x + 0.5;
			int mid_y3 = (1 - a) * sourcePoint[index3].y + a * targetPoint[index3].y + 0.5;
			// 得到中间三角形
			Triangle tri(Point(mid_x1, mid_y1), Point(mid_x2, mid_y2), Point(mid_x3, mid_y3), 
				index1, index2, index3);
			middleTriangle.push_back(tri);
		}

		// 得到每一个三角形到原图和目标图对应三角形的变换矩阵
		for (int j = 0; j < middleTriangle.size(); j++) {
			CImg<double> toSource = calculateTransformMatrix(middleTriangle[j], sourceTriangle[j]);
			middleToSource.push_back(toSource);
			CImg<double> toTarget = calculateTransformMatrix(middleTriangle[j], targetTriangle[j]);
			middleToTarget.push_back(toTarget);
		}
		// 生成中间帧
		cimg_forXY(middle, x, y) {
			for (int j = 0; j < middleTriangle.size(); j++) {
				Triangle tri = middleTriangle[j];
				Point P(x, y);
				// 判断像素属于哪个中间三角形
				if (isPointInTriangle(P, tri)) {
					CImg<double> mid_xy(1, 3, 1, 1, 1);
					CImg<double> src_xy(1, 3, 1, 1, 1);
					CImg<double> tar_xy(1, 3, 1, 1, 1);
					mid_xy(0) = x;
					mid_xy(1) = y;
					
					// 使用变换矩阵求出对应原图和目标图的坐标
					src_xy = middleToSource[j] * mid_xy;
					tar_xy = middleToTarget[j] * mid_xy;
					
					// 根据权重a，求得过渡像素值
					cimg_forC(middle, c) {
						middle(x, y, c) = (1 - a) * source((int)(src_xy(0) + 0.5), (int)(src_xy(1) + 0.5), c) + 
										a * target((int)(tar_xy(0) + 0.5), (int)(tar_xy(1) + 0.5), c);
					}
					break;
				}
			}
		}
		// 加入中间帧
		morphing.push_back(middle);
	}
	// 加入最后一帧
	morphing.push_back(target);
}

// 保存过渡动画
void Morphing::save() {
	morphing.save_gif_external("result.gif", 13);
}

// 从文件中读取或手动标注标记点
void Morphing::selectPoints(CImg<unsigned char>& src, std::vector<Point>& points, const char* file) {
	ifstream input(file);
	ofstream output;
	double red[] = {255, 0, 0};
	CImg<unsigned char> img = src;
	if (input.fail()) {
		output.open(file);
		CImgDisplay sourceDisp(img, "Click a point");
		while (!sourceDisp.is_closed()) {
			sourceDisp.wait();
			if (sourceDisp.button() && sourceDisp.mouse_y() >= 0 && sourceDisp.mouse_x() >= 0) {
				points.push_back(Point(sourceDisp.mouse_x(), sourceDisp.mouse_y()));
				img.draw_circle(sourceDisp.mouse_x(), sourceDisp.mouse_y(), 5, red);
				img.display(sourceDisp);
				output << sourceDisp.mouse_x() << " " << sourceDisp.mouse_y() << endl;
			}
		}
	}
	else {
		string line;
		double x, y;
		while (getline(input, line)) {
			stringstream ss(line);
			ss >> x >> y;
			points.push_back(Point(x, y));
			img.draw_circle(x, y, 5, red);
		}
		img.display();
	}
	input.close();
	points.push_back(Point(0, 0));
	points.push_back(Point(0, img.height() - 1));
	points.push_back(Point(img.width() - 1, 0));
	points.push_back(Point(img.width() - 1, img.height() - 1));
	points.push_back(Point(0, img.height() / 2));
	points.push_back(Point(img.width() / 2, 0));
	points.push_back(Point(img.width() - 1, img.height() / 2));
	points.push_back(Point(img.width() / 2, img.height() - 1));
}

// 判断一个点是否在三角形内
bool Morphing::isPointInTriangle(Point& P, Triangle& tri) {
	Point A = tri.p1;
	Point B = tri.p2;
	Point C = tri.p3;

	Point PA = Point(A.x - P.x, A.y - P.y);
	Point PB = Point(B.x - P.x, B.y - P.y);
	Point PC = Point(C.x - P.x, C.y - P.y);

    double PA_PB = (PA.x * PB.y - PA.y * PB.x);
    double PB_PC = (PB.x * PC.y - PB.y * PC.x);
    double PC_PA = (PC.x * PA.y - PC.y * PA.x);

    // 利用向量积的方向判断点是否在三角形内
    if (PA_PB >= 0 && PB_PC >= 0 && PC_PA >= 0 ||
    	PA_PB <= 0 && PB_PC <= 0 && PC_PA <= 0)
    	return true;
    return false;
}

// 判断一个点是否在三角形的外接圆内（重心法）
bool Morphing::isPointInCircle(Point& P, Triangle& tri) {
    double x1 = tri.p1.x;
    double x2 = tri.p2.x;
    double x3 = tri.p3.x;
    double y1 = tri.p1.y;
    double y2 = tri.p2.y;
    double y3 = tri.p3.y;

    double D = 2 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
    double x = ((pow(x1, 2) + pow(y1, 2)) * (y2 - y3) + (pow(x2, 2) + pow(y2, 2)) * (y3 - y1) + (pow(x3, 2) + pow(y3, 2)) * (y1 - y2)) / D;
    double y = ((pow(x1, 2) + pow(y1, 2)) * (x3 - x2) + (pow(x2, 2) + pow(y2, 2)) * (x1 - x3) + (pow(x3, 2) + pow(y3, 2)) * (x2 - x1)) / D;

    double radius = sqrt(pow((x1 - x), 2) + pow((y1 - y), 2));
    double distance = sqrt(pow((P.x - x), 2) + pow((P.y - y), 2));

    if (distance <= radius)
    	return true;
    return false;
}

// 给定变换前后的三角形，计算仿射变换的矩阵(线性代数方法)
CImg<double> Morphing::calculateTransformMatrix(Triangle from, Triangle to) {
	CImg<double> A(3, 3, 1, 1, 1);
    CImg<double> x(1, 3, 1, 1, 0), y(1, 3, 1, 1, 0);
    CImg<double> t1(1, 3, 1, 1, 0), t2(1, 3, 1, 1, 0);

    A(0, 0) = from.p1.x;
    A(1, 0) = from.p1.y;
    A(0, 1) = from.p2.x;
    A(1, 1) = from.p2.y;
    A(0, 2) = from.p3.x;
    A(1, 2) = from.p3.y;

    x(0, 0) = to.p1.x;
    x(0, 1) = to.p2.x;
    x(0, 2) = to.p3.x;
    y(0, 0) = to.p1.y;
    y(0, 1) = to.p2.y;
    y(0, 2) = to.p3.y;

    t1 = x.solve(A);	//A * t1 = x
    t2 = y.solve(A);	//A * t2 = y

    CImg<double> transform(3, 3, 1, 1, 0);
    transform(2, 2) = 1;
    for (int i = 0; i < 3; i++) {
        transform(i, 0) = t1(0, i);
        transform(i, 1) = t2(0, i);
    }
    
    return transform;
}