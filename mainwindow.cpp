#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QDebug>
#include <QImage>
#include <QColor>
#include <QPixmap>
#include <QKeyEvent>
#include <QRandomGenerator>

#include "FastNoiseLite.h"
#include "asmOpenCV.h"
#include "opencv2/opencv.hpp"

#include "richdem/richdem.hpp"
#include "richdem/common/Array2D.hpp"
#include "richdem/common/constants.hpp"

//double _samplePadFactor = 3.5;
//int _poissonSamplerKValue = 25;
//double _fluxCapPercentile = 0.995;

//double _defaultErodeAmount = 0.1;
//double _riverFluxThreshold = 0.06;
//double _riverSmoothingFactor = 0.5;
//double _isolevel = 0.0;
//double _minIslandFaceThreshold = 35;
//bool _isHeightMapEroded = false;

//double _minSlopeThreshold = 0.07;
//double _minSlope = 0.0;
//double _maxSlope = 0.7;
//double _minSlopeAngle = 0.2;
//double _maxSlopeAngle = 1.5;
//double _minSlopeLength = 0.75;
//double _maxSlopeLength = 1.3;
//double _minVerticalSlope = -0.25;
//double _maxVerticalSlope = 0.05;

//double _fluxScoreBonus = 2.0;
//double _nearEdgeScorePenalty = 0.5;
//double _nearCityScorePenalty = 2.0;
//double _nearTownScorePenalty = 1.5;
//double _maxPenaltyDistance = 4.0;

//double _landDistanceCost = 0.2;
//double _seaDistanceCost = 0.4;
//double _uphillCost = 0.1;
//double _downhillCost = 1.0;
//double _fluxCost = 0.8;
//double _landTransitionCost = 0.0;

//int _numTerritoryBorderSmoothingInterations = 3;
//double _territoryBorderSmoothingFactor = 0.5;


std::vector<cv::Mat> imgs;
size_t index = -1;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    generate_map(800,800, "");
    index = 0;
    display(imgs[index]);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::display(cv::Mat mat){
    cv::normalize(mat, mat, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    QImage img = ASM::cvMatToQImage(mat);
    ui->label->setPixmap(QPixmap::fromImage(img));
}

void  MainWindow::keyPressEvent(QKeyEvent *event) {
    if(event->key() == Qt::Key_Left)
        index -=1;
    if(event->key() == Qt::Key_Right)
        index +=1;
    index = (index + imgs.size()) % imgs.size();

    display(imgs[index]);
}

richdem::Array2D<float> toArray(const cv::Mat& mat){
    richdem::Array2D<float> dem(mat.cols, mat.rows, 0);
    for (int y = 0; y < mat.cols; y++) {
        for (int x = 0; x < mat.rows; x++) {
            dem(x,y) = mat.at<float>(x,y);
        }
    }
    return dem;
}
cv::Mat toMat(const richdem::Array2D<float>& dem){
    cv::Mat mat(dem.height(), dem.width(), CV_32FC1);
    for (int y = 0; y < dem.width(); y++) {
        for (int x = 0; x < dem.height(); x++) {
             mat.at<float>(x,y) = dem(x,y);
        }
    }
    return mat;
}

void MainWindow::generate_map(int width, int height, QString tpl) {
    // Heightmap
    cv::Mat noise = generate_noise(width, height, 546456, 0.02);
    cv::Mat noise2 = generate_noise(width, height, 123123, 0.003);
    cv::Mat noise3 = generate_noise(width, height, 123123, 0.008);
    imgs.push_back(noise);
    imgs.push_back(noise2);
    imgs.push_back(noise3);
    cv::Mat noise4;
    cv::add(noise2, noise, noise4);
    cv::add(noise4, noise3, noise4);
    cv::normalize(noise4, noise4, 0., 1., cv::NORM_MINMAX, CV_32FC1);
    imgs.push_back(noise4);


    cv::Mat mask = generate_tpl_mask(width, height);
    cv::normalize(mask, mask, 0., 1., cv::NORM_MINMAX, CV_32FC1);
    cv::blur(mask, mask, cv::Size(30,30));
    imgs.push_back(mask);

    cv::Mat merged;
    cv::multiply(noise4, mask, merged);
    cv::normalize(merged, merged, 0., 1., cv::NORM_MINMAX, CV_32FC1);
    imgs.push_back(merged);
//    cv::normalize(noise3, merged, 0., 1., cv::NORM_MINMAX, CV_32FC1);

    cv::Mat eroded = generate_erosion_map(merged, width, height);
    imgs.push_back(eroded);

    cv::Mat rivers = generate_rivers(eroded, width, height);
    cv::normalize(rivers, rivers, 0., 1., cv::NORM_MINMAX, CV_32FC1);
    imgs.push_back(rivers);
    cv::Mat rivers_thres;
    cv::threshold(rivers, rivers_thres, 0.008, 1.0, cv::THRESH_BINARY);
    imgs.push_back(rivers_thres);
    cv::Mat inv_rivers =  cv::Scalar::all(1.0) - rivers_thres;
    imgs.push_back(inv_rivers);

    cv::Mat rivered;
    cv::multiply(eroded, inv_rivers, rivered);
    imgs.push_back(rivered);


    cv::Mat sea = generate_sea(eroded, width, height);
    imgs.push_back(sea);

    cv::Mat land;
    cv::multiply(sea, rivered, land);
    imgs.push_back(land);

    cv::Mat converted;
    cv::normalize(land, converted, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    cv::Mat im_color;
    cv::applyColorMap(converted, im_color, cv::COLORMAP_JET);
    imgs.push_back(im_color);


//    std::vector<std::vector<cv::Point> > contours;
//    std::vector<cv::Vec4i> hierarchy;
//    cv::Mat contour;
//    cv::normalize(sea, contour, 0, 255, cv::NORM_MINMAX, CV_8UC1);
//    cv::Mat canny_output;
//    int thresh = 100;
//    cv::Canny( contour, canny_output, thresh, thresh*2 );
//    cv::findContours( contour, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
//    for( size_t i = 0; i< contours.size(); i++ ) {
//        cv::drawContours(contour, contours, (int)i, 0.5, 1, cv::LINE_8, hierarchy, 0 );
//    }
//    imgs.push_back(contour);


    // Vec Map
    // generate_poisson();
    // generate_voronoi();
    // generate_delaunay();
    // align(heightmap, vecmap, water_map)

    // Symbolic map
    // generate_biomes();
    // generate_regions();
    // generate_cities();
    // generate_roads();

    // Render
    // render_geomap();
    // render_classic_map();
    // render_symbolic_map();


}

double median(cv::Mat Input){
    Input = Input.reshape(0,1); // spread Input Mat to single row
    std::vector<double> vecFromMat;
    Input.copyTo(vecFromMat); // Copy Input Mat to vector vecFromMat
    std::nth_element(vecFromMat.begin(), vecFromMat.begin() + vecFromMat.size()/2, vecFromMat.end());
    return vecFromMat[vecFromMat.size()/2];
}


cv::Mat MainWindow::generate_sea(cv::Mat &m, int, int){
    //    richdem::Array2D<float> dem = toArray(m);
    //    richdem::FillDepressionsEpsilon<richdem::Topology::D8, float>(dem);
    //    cv::Mat res = toMat(dem);

    cv::Mat res = m.clone();
//    cv::threshold(m, res, median(m), 1.0, cv::THRESH_BINARY);
    cv::threshold(m, res, 0.4, 1.0, cv::THRESH_BINARY);
    return res;
}

cv::Mat MainWindow::generate_rivers(cv::Mat &m, int width, int height){
    richdem::Array2D<float> dem(width, height, 0);
    for (int y = 0; y < width; y++) {
        for (int x = 0; x < height; x++) {
            dem(x,y) = m.at<float>(x,y);
        }
    }
    richdem::FillDepressionsEpsilon<richdem::Topology::D8, float>(dem);
    richdem::Array2D<float> accum(width, height, 1.5);
//    richdem::FA_Freeman(dem, accum, 1.1);
    richdem::FA_Rho8(dem, accum);

    cv::Mat res = m.clone();
    for (int y = 0; y < width; y++) {
        for (int x = 0; x < height; x++) {
            res.at<float>(x,y) = accum(x,y);
        }
    }
    return res;
}

double _maxErosionRate = 0.02;
double _erosionRiverFactor = 0.2;
double _erosionCreepFactor = 0.2;

cv::Mat compute_erosion_map(cv::Mat &m, int width, int height){
    richdem::Array2D<float> dem = toArray(m);
    richdem::FillDepressionsEpsilon<richdem::Topology::D8, float>(dem);
    richdem::Array2D<float> accum(width, height, 0.05);
    richdem::FA_Rho8(dem, accum);
    richdem::Array2D<float> slope_mat(width, height);
    richdem::TA_slope_riserun(dem, slope_mat);

    cv::Mat res = m.clone();
    for (int y = 0; y < width; y++) {
        for (int x = 0; x < height; x++) {
            double flux = accum(x,y);
            double slope = slope_mat(x,y);
            double river = _erosionRiverFactor * sqrt(flux) * slope;
            double creep = _erosionCreepFactor * slope * slope;
            double erosion = fmin(river + creep, _maxErosionRate);
            res.at<float>(x,y) = erosion;
        }
    }
    cv::normalize(res, res, 0, 1, cv::NORM_MINMAX, CV_32FC1);

    return res;
}

cv::Mat MainWindow::generate_erosion_map(cv::Mat &m, int width, int height)
{
    cv::Mat res = m.clone();
    int iterations = 3;
//    double strength = QRandomGenerator::global()->bounded(0.2) + 0.15;
    double strength = 0.4;
    double erosion_strength = strength / iterations;
    for (int i = 0; i < iterations; i++) {
        cv::Mat e_map = compute_erosion_map(res, width, height);
        for (int y = 0; y < width; y++) {
            for (int x = 0; x < height; x++) {
                double newlevel = res.at<float>(x,y) - (erosion_strength * e_map.at<float>(x,y));
                res.at<float>(x,y) = newlevel;
            }
        }
        cv::normalize(res, res, 0., 1., cv::NORM_MINMAX, CV_32FC1);
    }
    return res;
}


void addCone(cv::Mat &m, double px, double py, double radius, double strength, double width, double height) {
    double invradius = 1.0 / radius;
    double rsq = radius * radius;
    for (int y = 0; y < width; y++) {
        for (int x = 0; x < height; x++) {
            double dx = x - px;
            double dy = y - py;
            double dsq = dx*dx + dy*dy;
            if (dsq < rsq) {
                double dist = sqrt(dsq);
                double kernel = 1.0 - dist * invradius;
                double hval = m.at<float>(x,y);
                m.at<float>(x,y) = hval + strength*kernel;
            }
        }
    }
}

void addSlope(cv::Mat &m, double px, double py, double dirx, double diry, double radius, double strength, double width, double height) {
    for (int y = 0; y < width; y++) {
        for (int x = 0; x < height; x++) {
            double dx = x - px;
            double dy = y - py;
            double dot = dx*dirx + dy*diry;
            double distx = dx - dot*dirx;
            double disty = dy - dot*diry;
            double dist = sqrt(distx*distx + disty*disty);
            dist = fmin(dist, radius);

            double cross = dx*diry - dy*dirx;
            double min, max;
            if (cross < 0) {
                min = 0.5*strength;
                max = 0;
            } else {
                min = 0.5*strength;
                max = height;
            }

            double fieldval = min + (dist / radius)*(max - min);
            double hval = m.at<float>(x,y);
            m.at<float>(x,y) = hval + fieldval;
        }
    }
}

void addHill(cv::Mat &m, double px, double py, double r, double strength, double width, double height) {
    // coefficients for the Wyvill kernel function
    double coef1 = (4.0 / 9.0)*(1.0 / (r*r*r*r*r*r));
    double coef2 = (17.0 / 9.0)*(1.0 / (r*r*r*r));
    double coef3 = (22.0 / 9.0)*(1.0 / (r*r));
    double rsq = r*r;

    for (int y = 0; y < width; y++) {
        for (int x = 0; x < height; x++) {
            double dx = x - px;
            double dy = y - py;
            double dsq = dx*dx + dy*dy;
            if (dsq < rsq) {
                double kernel = 1.0 - coef1*dsq*dsq*dsq + coef2*dsq*dsq - coef3*dsq;
                double hval = m.at<float>(x,y);
                m.at<float>(x,y) = hval + strength*kernel;
            }
        }
    }
}


cv::Mat MainWindow::generate_tpl_mask(int width, int height) {
    cv::Mat m(width, height, CV_32FC1, 0.);
//    double pad = 5.0;
//    int n = QRandomGenerator::global()->bounded(550) + 100;
     addHill(m, width/2, height/2, width/3, 0.1, width, height);
     for (int i = 0; i < 20; i++) {
         int x = QRandomGenerator::global()->bounded(9*width/10)+width/20;
         int y = QRandomGenerator::global()->bounded(9*height/10)+height/20;
         addHill(m, x, y, width/QRandomGenerator::global()->bounded(5,10), QRandomGenerator::global()->bounded(0.1), width, height);
     }
//    int n = 450;
//    double minr = 5.0;
//    double maxr = 200.0;
//    for (int i = 0; i < n; i++) {
////        int x = QRandomGenerator::global()->bounded(width);
////        int y = QRandomGenerator::global()->bounded(height);
//        int x = QRandomGenerator::global()->bounded(9*width/10)+width/20;
//        int y = QRandomGenerator::global()->bounded(9*height/10)+height/20;
//        double r = QRandomGenerator::global()->bounded(maxr) + minr;
////        double strength = QRandomGenerator::global()->bounded(10.0) + 0.1;
//        double strength = 0.01;
//        addHill(m, x, y, r, strength, width, height);
////        if (QRandomGenerator::global()->generateDouble() > 0.5) {
////            addHill(m, x, y, r, strength, width, height);
////        }
////        else {
////            addCone(m, x, y, r, strength, width, height);
////        }
//    }
//    if (QRandomGenerator::global()->generateDouble() > 0.5) {
//        int x = QRandomGenerator::global()->bounded(width);
//        int y = QRandomGenerator::global()->bounded(height);
//        double r = QRandomGenerator::global()->bounded(6.0) + 6.0;
//        double strength = QRandomGenerator::global()->bounded(2.0) + 2.0;
//        addCone(m, x, y, r, strength, width, height);
//    }

//    if (QRandomGenerator::global()->generateDouble() > 0.1) {
//        double angle = QRandomGenerator::global()->bounded(2*3.14159);
//        double dir_x = sin(angle);
//        double dir_y = cos(angle);
//        double x = QRandomGenerator::global()->bounded(width);
//        double y = QRandomGenerator::global()->bounded(height);
//        double slopewidth = QRandomGenerator::global()->bounded(4.5) + 0.5;
//        double strength = QRandomGenerator::global()->bounded(1.0) + 2.0;
//        addSlope(m, x, y, dir_x, dir_y, slopewidth, strength, width, height);
//    }

    return m;
}



cv::Mat MainWindow::generate_noise(int width, int height, double seed, double frequency) {
    cv::Mat m(width, height, CV_32FC1);
    FastNoiseLite noise;
    noise.SetSeed(seed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFrequency(frequency);
    noise.SetFractalOctaves(10);
    noise.SetFractalLacunarity(2.0);
    noise.SetFractalGain(0.5);
    for (int y = 0; y < width; y++) {
        for (int x = 0; x < height; x++) {
            float val = noise.GetNoise((float)x, (float)y);
            m.at<float>(x,y) = (val + 1.2)/3;
        }
    }
    return m;
}
