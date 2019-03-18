#include "Cutting.h"

void Cutting::divideToLines() {
    histY.assign(1, binary.height(), 1, 1, 0);  // 初始化Y方向的直方图
    vector<int> inflection; // 直方图的拐点集合
    inflection.push_back(0);
    
    // 对图像在Y方向做投影，得到Y方向的直方图
    cimg_forY(histY, y) {
        cimg_forX(binary, x) {
            if (binary(x, y) == 255) {
                histY(y)++;
            }
        }
        // 将直方图中波谷的开始和结束点加入拐点集合
        if (y > 0) {
            if (histY(y) > 0 && histY(y - 1) == 0) {
                inflection.push_back(y - 1);
            }
            else if (histY(y) == 0 && histY(y - 1) > 0) {
                inflection.push_back(y);
            }
        }
        
    }
    inflection.push_back(binary.height() - 1);

    // 计算两个拐点坐标的平均值，得到分割的Y坐标
    if (inflection.size() > 2) {
        for (int i = 0; i < inflection.size() - 1; i += 2) {
            int y = (inflection[i] + inflection[i + 1]) / 2;
            cutY.push_back(y);
        }
    }
    vector<int> tempCutY;
    for (int i = 1; i < cutY.size(); i++) {
        int rowHeight = cutY[i] - cutY[i - 1];
        CImg<unsigned char> row(binary.width(), rowHeight, 1, 1, 0);
        int num = 0;
        // 分离出每一行的图像
        cimg_forXY(row, x, y) {
            row(x, y) = binary(x, cutY[i - 1] + y + 1);
            if (row(x, y) == 255)
                num++;
        }
        // 当行中白色像素的数量大于0.001的比例时，才认为该行有效
        if ((float)num / ((float)binary.width() * (float)rowHeight) > 0.001) {
            rows.push_back(row);
            tempCutY.push_back(cutY[i - 1]);
        }
    }
    cutY.clear();
    for (int i = 0; i < tempCutY.size(); i++) {
        cutY.push_back(tempCutY[i]);
    }
}

void Cutting::divideToSingleNumber() {
    // 对每一行进行切割
    for (int i = 0; i < rows.size(); i++) {
        CImg<unsigned char> row = rows[i];       // 行图像
        vector<CImg<unsigned char> > rowNumbers; // 行中的数字集合
        vector<int> cutX;
        histX.assign(binary.width(), 1, 1, 1, 0); // 初始化Y方向的直方图
        vector<int> inflection;                   // 直方图的拐点集合
        inflection.push_back(0);
        // 对图像在X方向做投影，得到X方向的直方图
        cimg_forX(histX, x) {
            cimg_forY(row, y) {
                if (row(x, y) == 255) {
                    histX(x)++;
                }
            }
            // 将直方图中波谷的开始和结束点加入拐点集合
            if (x > 0) {
                if (histX(x) > 0 && histX(x - 1) == 0) {
                    inflection.push_back(x - 1);
                }
                else if (histX(x) == 0 && histX(x - 1) > 0) {
                    inflection.push_back(x);
                }
            }
            
        }
        inflection.push_back(binary.width() - 1);
        // 计算两个拐点坐标的平均值，得到分割的X坐标
        if (inflection.size() > 2) {
            for (int j = 0; j < inflection.size() - 1; j += 2) {
                int x = (inflection[j] + inflection[j + 1]) / 2;
                cutX.push_back(x);
            }
        }
        vector<int> tempCutX;
        for (int j = 1; j < cutX.size(); j++) {
            int numWidth = cutX[j] - cutX[j - 1];
            CImg<unsigned char> number(numWidth, row.height(), 1, 1, 0);
            int num = 0;
            // 分离出每一列的图像
            cimg_forXY(number, x, y) {
                number(x, y) = row(cutX[j - 1] + x + 1, y);
                if (number(x, y) == 255)
                    num++;
            }
            // 当行中白色像素的数量大于0.008的比例时，才认为该列有效
            if ((float)num / ((float)numWidth * (float)row.height()) > 0.0008) {
                int minX = number.width() - 1, minY = number.height() - 1,
                    maxX = 0, maxY = 0;
                cimg_forXY(number, x, y) {
                    if (number(x, y) == 255) {
                        if (x < minX) minX = x;
                        if (x > maxX) maxX = x;
                        if (y < minY) minY = y;
                        if (y > maxY) maxY = y;
                    }
                }
                int width = maxX - minX + 1;
                int height = maxY - minY + 1;
                // 求出包裹数字的图像
                CImg<unsigned char> cutNum(width, height, 1, 1, 0);
                cimg_forXY(cutNum, x, y) {
                    cutNum(x, y) = number(x + minX, y + minY);
                }
                // 生成数字的正方形图像
                CImg<unsigned char> squareNum;
                // 用黑色将图像补成正方形
                if (width < height) {
                    float edge = height * 0.2;
                    int gap = (height - width) / 2 + (int)edge;
                    minX -= gap;
                    maxX += gap;
                    width = maxX - minX + 1;
                    height += (int)edge * 2;
                    squareNum.assign(width, height, 1, 1, 0);
                    cimg_forXY(cutNum, x, y) {
                        squareNum(x + gap, y + (int)edge) = cutNum(x, y);
                    }
                }
                else if (width > height) {
                    float edge = width * 0.2;
                    int gap = (width - height) / 2 + (int)edge;
                    minY -= gap;
                    maxY += gap;
                    height = maxY - minY + 1;
                    width += (int)edge * 2;
                    squareNum.assign(width, height, 1, 1, 0);
                    cimg_forXY(cutNum, x, y) {
                        squareNum(x + (int)edge, y + gap) = cutNum(x, y);
                    }
                }
                else {
                    squareNum = cutNum;
                }
                // 根据图像的大小，对数字进行膨胀，避免数字断裂和过细
                int expand = (squareNum.width() / 100 + 1) * 2; // 膨胀次数
                for (int k = 0; k < expand; k++) {
                    CImg<unsigned char> temp(squareNum.width(), squareNum.height(), 1, 1, 0);
                    // 向8邻域膨胀
                    cimg_forXY(squareNum, x, y) {
                        if (squareNum(x, y) == 255) {
                            temp(x, y) = 255;
                            if (x > 0 && y > 0) {
                                temp(x - 1, y - 1) = 255;
                                temp(x - 1, y) = 255;
                                temp(x, y - 1) = 255;
                            }
                            if (x < temp.width() - 1 && y < temp.height() - 1) {
                                temp(x, y + 1) = 255;
                                temp(x + 1, y) = 255;
                                temp(x + 1, y + 1) = 255;
                            }
                            if (x > 0 && y < temp.height() - 1) {
                                temp(x - 1, y + 1) = 255;
                            }
                            if (x < temp.height() - 1 && y > 0) {
                                temp(x + 1, y - 1) = 255;
                            }
                        }
                    }
                    squareNum = temp;
                }
                rowNumbers.push_back(squareNum); // 将图像加入到行中的数字集合
                tempCutX.push_back(cutX[j - 1]);
            }
        }
        cutX.clear();
        for (int j = 0; j < tempCutX.size(); j++) {
            cutX.push_back(tempCutX[j]);
        }
        numbers.push_back(rowNumbers);  // 将行数字集合加入到总数字集合
    }
}
void Cutting::save() {
    char outputDir[100];
    sprintf(outputDir, "Result/Step3/%s", outputname);
    // 新建输出路径
    if (_access(outputDir, 0) == -1)
		_mkdir(outputDir);
    cout << "Saving cut images to '" << outputDir << "'." << endl;
    ofstream out, imageList;
    char dir[36];
    sprintf(dir, "Result\\Step3\\%s\\imageList.txt", outputname);
    imageList.open(dir); // 使用一个文件，记录该目录下所有图像的文件名
    for(int i = 0; i < numbers.size(); i++) {
        for(int j = 0; j < numbers[i].size(); j++) {
            // 保存图像
            char result[100];
            sprintf(result, "Result\\Step3\\%s\\row%d_col%d.jpg", outputname, i, j);
            numbers[i][j].save(result);
            if (imageList.is_open()) {
                imageList << "row" << i << "_col" << j << ".jpg" << " ";
            }
        }
        if (imageList.is_open()) {
            imageList << endl;
        }
    }
    if (imageList.is_open()) {
        imageList.close();
    }
    out.open("imageDir.txt", std::ios::app); // 使用一个文件，记录图像保存的路径
    if (out.is_open()) {
        out << "Result\\Step3\\" << outputname << endl;
        out.close();
    }
}