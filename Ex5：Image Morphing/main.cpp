#include "Morphing.h"
#include <iostream>

// 测试函数
void test(const char* sourcePath, const char* targetPath) {
	// 读入图像
	Morphing morphing = Morphing(sourcePath, targetPath);
    // 获取标记点
	morphing.getPoints();
    // 获取三角划分
    morphing.getTriangles();
    // 计算中间帧
    morphing.calculateMiddleFrames();
    // 输出结果
    morphing.save();
}

int main() {
    test("1.jpg", "2.bmp");
    return 0;
}
