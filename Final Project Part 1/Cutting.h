#include <iostream>
#include <vector>
#include <cstdio>
#include <fstream>
#include <list>
#include "CImg.h"
#include "Hough.h"

using namespace cimg_library;
using namespace std;

class Cutting {
private:
    char* outputname;
    CImg<float> histY, histX;
    CImg<unsigned char> binary, tagImg;
    vector<int> cutY;
    vector<int> classTagSet;
    vector<CImg<unsigned char> > rows;
    vector<vector<CImg<unsigned char> > > numbers;
public:
    Cutting(CImg<unsigned char> img, char* filename) {
        binary = img;
        outputname = filename;
    }
    void divideToLines();
    void divideToSingleNumber();
    void save();
};