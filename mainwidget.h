#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFileDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDebug>
#include <QBuffer>
#include <opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>


namespace Ui {
class mainWidget;
}

class mainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit mainWidget(QWidget *parent = 0);
    ~mainWidget();
    QImage resultimage;
protected:
    void closeEvent(QCloseEvent *event);//关闭事件
    bool saveFile(const QString& filename);//保存文件

private slots:
    void on_closeBtn_clicked();//关闭槽

    void on_openBtn_clicked();//打开槽

    void on_disposeBtn_clicked();//处理槽

    void on_saveBtn_clicked();//保存槽

private:
    Ui::mainWidget *ui;
    QImage *image;
    //声明新变量myImage
    QImage *myImage;

    cv::Mat Canny_check();//处理 边缘检测
    cv::Mat QImage2Mat(const QImage& qimage);//QImage转换为 Mat
    QImage Mat2QImage(const cv::Mat& mat);//Mat转换为QImage
//    QString strippedName(const QString &fullFileName);//获取较短的绝对路径
//    QString curFile;//保存当前文件路径
//    bool isUntitled;//作为当前文件是否被保存在硬盘上的标识
};

#endif // MAINWIDGET_H
