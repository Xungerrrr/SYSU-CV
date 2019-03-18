#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include "Cutting.h"

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

int main() {
	vector<string> filenames = getFiles("./Result/Step2/"); // 读取图像文件名
    for (int i = 0; i < filenames.size(); i++) {
        string filename = "./Result/Step2/" + filenames[i];
        const char *infilename = filename.c_str();
        const char *outputname = filenames[i].substr(0, filenames[i].find(".")).c_str();

        // 读取图像
        cout << "Loading image '" << infilename << "'." << endl;
		CImg<unsigned char> image(infilename);

        // 切割数字
		Cutting cutting(image, outputname);
        // 切割行
		cutting.divideToLines();
        // 切割列
		cutting.divideToSingleNumber();
        // 保存切割出的单个数字
		cutting.save();
	}
	
    return 0;
}