#include "MyImg.h"

// 测试函数（不使用CImg函数调用）
void test_myimg_1() {
    // step1: 读入1.bmp文件，并用CImg.display()显示
    MyImg img = MyImg("1.bmp");
    img.display();

    // step2: 把1.bmp文件的白色区域变成红色，黑色区域变成绿色
    img.change_color();

    // step3: 在图上绘制一个圆形区域，圆心坐标(50, 50)，半径为30，填充颜色为蓝色
    img.draw_blue_circle_1();

    // step4: 在图上绘制一个圆形区域，圆心坐标(50, 50)，半径为3，填充颜色为黄色
    img.draw_yellow_circle_1();

    // step5: 在图上绘制一条长为100的直线段，起点坐标为(0, 0)，方向角为35，直线的颜色为蓝色
    img.draw_line_1();

    // step6: 把上面的操作结果保存为2(1).bmp
    img.save("2(1).bmp");
}

// 测试函数（使用CImg函数调用）
void test_myimg_2() {
    // step1: 读入1.bmp文件，并用CImg.display()显示
    MyImg img = MyImg("1.bmp");
    img.display();

    // step2: 把1.bmp文件的白色区域变成红色，黑色区域变成绿色
    img.change_color();

    // step3: 在图上绘制一个圆形区域，圆心坐标(50, 50)，半径为30，填充颜色为蓝色
    img.draw_blue_circle_2();

    // step4: 在图上绘制一个圆形区域，圆心坐标(50, 50)，半径为3，填充颜色为黄色
    img.draw_yellow_circle_2();

    // step5: 在图上绘制一条长为100的直线段，起点坐标为(0, 0)，方向角为35，直线的颜色为蓝色
    img.draw_line_2();

    // step6: 把上面的操作结果保存为2(1).bmp
    img.save("2(2).bmp");
}

int main() {
    test_myimg_1();
    test_myimg_2();
    return 0;
}
