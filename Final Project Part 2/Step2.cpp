#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include "OSTU.h"

vector<string> getFiles(string dir) {
    vector<string> filenames;
    _finddata_t file;
    long lf;
    // 寻找文件夹路径
    if ((lf = _findfirst(dir.append("\\*").c_str(), &file)) == -1L) {
        cout << dir << " not found!" << endl;
    }
    else {
        // 读取路径中的文件名
        while (_findnext(lf, &file) == 0) {
            if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0)
                continue;
            filenames.push_back(file.name);
        }
    }
    _findclose(lf);
    // 文件名排序
    sort(filenames.begin(), filenames.end());
    return filenames;
}

int main(int argc, char *argv[]) {
	vector<string> filenames = getFiles("./Result/Step1/"); // 读取图像文件名
    for (int i = 0; i < filenames.size(); i++) {
        string filename = "./Result/Step1/" + filenames[i];
        const char *infilename = filename.c_str();
        const char *outputname = filenames[i].c_str();
		int edge = 50;

        // 加载图片
        cout << "Loading image '" << infilename << "'." << endl;
		CImg<unsigned char> src(infilename);
		CImg<unsigned char> image(src.width(), src.height(), 1, 1, 0);

		// 转灰度图
		cimg_forXY(src, x, y) {
			int r = src(x, y, 0);
			int g = src(x, y, 1);
			int b = src(x, y, 2);
			int grey = (r * 30 + g * 59 + b * 11 + 50) / 100;
			image(x, y) = grey;
		}

		// 二值化
		OSTU ostu = OSTU(image, edge);
		CImg<unsigned char> binaryResult = ostu.ostu();

		// 输出结果
        cout << "Saving binary image to 'Result/Step2/" << outputname << endl;
		char result[30];
		sprintf(result, "Result\\Step2\\%s", outputname);
		binaryResult.save(result);
	}
	
    return 0;
}