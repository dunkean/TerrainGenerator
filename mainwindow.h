#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/core/mat.hpp>
#include "richdem/common/Array2D.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void display(cv::Mat mat);
    void generate_map(int width, int height, QString tpl);
//    richdem::Array2D<float> toArray(const cv::Mat& mat);
//    cv::Mat toMat(const richdem::Array2D<float>& array);
    cv::Mat generate_noise(int width, int height, double seed, double frequency);
    cv::Mat generate_tpl_mask(int width, int height);
    cv::Mat generate_erosion_map(cv::Mat &merged, int width, int height);
    cv::Mat generate_rivers(cv::Mat &eroded, int width, int height);
    cv::Mat generate_sea(cv::Mat &eroded, int width, int height);

protected:
    void keyPressEvent(QKeyEvent *event);

private:
    QImage * image;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
