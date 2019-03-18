#include "HistEq.h"
#include <iostream>

// 测试函数
void test(const char* path) {
	char outputPath[20];
	// 读入图像
	HistEq img = HistEq(path);
	// 计算图像直方图
    img.calculateHist();
    // 计算原图与结果的映射关系
    img.calculateMap();
    // 应用均衡效果
    img.apply();
    // 输出结果
    sprintf(outputPath, "%s_output.jpg", path);
    img.save(outputPath);
}

int main() {
	// 输入图片的路径
    char inputPath[20];
    cout << "Input image path:" << endl;
    cin >> inputPath;
    test(inputPath);
    return 0;
}
