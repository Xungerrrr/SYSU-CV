#include "ColorTransfer.h"
#include <iostream>

// 测试函数
void test(const char* sourcePath, const char* targetPath) {
    char outputPath[20];
    // 读入图像
    ColorTransfer colorTransfer = ColorTransfer(sourcePath, targetPath);
    // 转到Lab空间
    colorTransfer.RGBtoLab();
    // 计算均值
    colorTransfer.calculateMean();
    // 计算标准差
    colorTransfer.calculateSD();
    // 实现颜色转换
    colorTransfer.colorCorrection();
    // 转回RGB空间
    colorTransfer.LabtoRGB();
    // 输出结果
    sprintf(outputPath, "%s_output.jpg", sourcePath);
    colorTransfer.save(outputPath);
}

int main() {
    // 输入图片的路径
    char sourcePath[20], targetPath[20];
    cout << "Input source image path:" << endl;
    cin >> sourcePath;
    cout << "Input target image path:" << endl;
    cin >> targetPath;
    test(sourcePath, targetPath);
    return 0;
}
