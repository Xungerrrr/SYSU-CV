#include "Warping.h"
#include "Canny.h"
#include <iostream>
#include <vector>
#include <string>

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

// 生成文件夹，删除存储信息的文本文件
void init() {
	if (_access("Result", 0) == -1)
		_mkdir("Result");
	if (_access("Result/Step1", 0) == -1)
		_mkdir("Result/Step1");
	if (_access("Result/Step2", 0) == -1)
		_mkdir("Result/Step2");
    if (_access("Result/Step3", 0) == -1)
		_mkdir("Result/Step3");
    if (_access("Result/Step4", 0) == -1)
		_mkdir("Result/Step4");
    if (_access("corner.txt", 0) == 0)
        remove("corner.txt");
    if (_access("imageDir.txt", 0) == 0)
        remove("imageDir.txt");
    
}

int main() {
    init();
    vector<string> filenames = getFiles("./ImageData/"); // 读取图像文件名
    for (int i = 0; i < filenames.size(); i++) {
        string filename = "./ImageData/" + filenames[i];
        const char *infilename = filename.c_str();
        const char *outputname = filenames[i].c_str();
        float sigma = 2,                /* Standard deviation of the gaussian kernel. */
            tlow = 0.97,                /* Fraction of the high threshold in hysteresis. */
            thigh = 0.97;               /* High hysteresis threshold control. The actual
                                            threshold is the (100 * thigh) percentage point
                                            in the histogram of the magnitude of the
                                            gradient image that passes non-maximal
                                            suppression. */
        int houghThreshold = 200,
            range = 80;

        ofstream out;
        out.open("corner.txt", std::ios::app);
        if (out.is_open()) {
            out << outputname;
            out.close();
        }

        // 加载图片
        cout << "Loading image '" << infilename << "'." << endl;
        CImg<unsigned char> src(infilename);

        // Canny边缘检测
        Canny cny(src, sigma, tlow, thigh);
        CImg<unsigned char> edge = cny.edge_detection();

        // 霍夫变换
        Hough hough(edge, houghThreshold, range);
        vector<Point> intersections = hough.line_detection();

        // A4纸矫正
        Warping warping(intersections, src);
        warping.getPointPair();
        warping.calculateHomography();
        CImg<unsigned char> warpingResult = warping.warp();

        // 输出结果
        cout << "Saving warped image to 'Result/Step1/" << outputname << endl;
        char result[30];
        sprintf(result, "Result\\Step1\\%s", outputname);
        warpingResult.save(result);
    }
    return 0;
}