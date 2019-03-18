#include "Cutting.h"

void Cutting::divideToLines() {
    histY.assign(1, binary.height(), 1, 1, 0);
    vector<int> inflection;
    inflection.push_back(0);
    
    cimg_forY(histY, y) {
        cimg_forX(binary, x) {
            if (binary(x, y) == 255) {
                histY(y)++;
            }
        }
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
        cimg_forXY(row, x, y) {
            row(x, y) = binary(x, cutY[i - 1] + y + 1);
            if (row(x, y) == 255)
                num++;
        }
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
    for (int i = 0; i < rows.size(); i++) {
        CImg<unsigned char> row = rows[i];
        vector<CImg<unsigned char> > rowNumbers;
        vector<int> cutX;
        histX.assign(binary.width(), 1, 1, 1, 0);
        vector<int> inflection;
        inflection.push_back(0);
        cimg_forX(histX, x) {
            cimg_forY(row, y) {
                if (row(x, y) == 255) {
                    histX(x)++;
                }
            }
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
            cimg_forXY(number, x, y) {
                number(x, y) = row(cutX[j - 1] + x + 1, y);
                if (number(x, y) == 255)
                    num++;
            }
            if ((float)num / ((float)numWidth * (float)row.height()) > 0.003) {
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
                CImg<unsigned char> cutNum(width, height, 1, 1, 0);
                cimg_forXY(cutNum, x, y) {
                    cutNum(x, y) = number(x + minX, y + minY);
                }
                CImg<unsigned char> squareNum;
                if (width < height) {
                    int gap = (height - width) / 2;
                    minX -= gap;
                    maxX += gap;
                    width = maxX - minX + 1;
                    squareNum.assign(width, height, 1, 1, 0);
                    cimg_forXY(cutNum, x, y) {
                        squareNum(x + gap, y) = cutNum(x, y);
                    }
                }
                else if (width > height) {
                    int gap = (width - height) / 2;
                    minY -= gap;
                    maxY += gap;
                    height = maxY - minY + 1;
                    squareNum.assign(width, height, 1, 1, 0);
                    cimg_forXY(cutNum, x, y) {
                        squareNum(x, y + gap) = cutNum(x, y);
                    }
                }
                else {
                    squareNum = cutNum;
                }
                rowNumbers.push_back(squareNum);
                tempCutX.push_back(cutX[j - 1]);
            }
        }
        cutX.clear();
        for (int j = 0; j < tempCutX.size(); j++) {
            cutX.push_back(tempCutX[j]);
        }
        for (int j = 0; j < rowNumbers.size(); j++) {
            // rowNumbers[j].display();
        }
        numbers.push_back(rowNumbers);
    }
}
void Cutting::save() {
    ofstream out;
    ofstream imageList;
    char dir[23];
    sprintf(dir, "Step3\\%s\\imageList.txt", outputname);
    imageList.open(dir);
    for(int i = 0; i < numbers.size(); i++) {
        for(int j = 0; j < numbers[i].size(); j++) {
            char result[100];
            sprintf(result, "Step3\\%s\\row%d_col%d.jpg", outputname, i, j);
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
    out.open("imageDir.txt", std::ios::app);
    if (out.is_open()) {
        out << "Step3\\" << outputname << endl;
        out.close();
    }
}