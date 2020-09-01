#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <iostream>
using namespace cv;


mainWidget::mainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainWidget)
{
    ui->setupUi(this);
    setFixedSize(1340,580);

}

mainWidget::~mainWidget()
{
    delete ui;
}

void mainWidget::closeEvent(QCloseEvent *event)//关闭事件
{
    QWidget::closeEvent(event);
}

void mainWidget::on_closeBtn_clicked()//关闭槽
{
    close();
}

void mainWidget::on_openBtn_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
                    this, "打开图片",
                    ".",
                    "图片 (*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm);;所有文件(*.*)");
    if(fileName != "")
    {
       // 将 image 变量new出来，开辟在堆区 
       QImage *image = new QImage;
        if(image->load(fileName))
        {
            QGraphicsScene *scene = new QGraphicsScene;
            scene->addPixmap(QPixmap::fromImage(*image));
            ui->formerView->setScene(scene);
            ui->formerView->resize(image->width() + 10, image->height() + 10);
            ui->formerView->show();
            // 将image传给this 指针，供后续载入图片，用canny后不会再发生闪退
            this->myImage = image;
        }
        else
        {
            QMessageBox::information(this,
                            tr("打开图像失败"),
                            tr("打开图像失败!"));
             return;
        }
    }

}

QImage mainWidget::Mat2QImage(const Mat &mat)
{
    //8-bitsunsigned,NO.OFCHANNELS=1
    if(mat.type()==CV_8UC1)
    {
       //cout<<"1"<<endl;
        //Setthecolortable(usedtotranslatecolourindexestoqRgbvalues)
        QVector<QRgb>colorTable;
        for(int i=0;i<256;i++)
            colorTable.push_back(qRgb(i,i,i));
        //CopyinputMat
        const uchar*qImageBuffer=(const uchar*)mat.data;
        //CreateQImagewithsamedimensionsasinputMat
        QImage img(qImageBuffer,mat.cols,mat.rows,mat.step,QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    //8-bitsunsigned,NO.OFCHANNELS=3
    if(mat.type()==CV_8UC3)
    {
       //cout<<"3"<<endl;
        //CopyinputMat
        const uchar *qImageBuffer=(const uchar*)mat.data;
        //CreateQImagewithsamedimensionsasinputMat
        QImage img(qImageBuffer,mat.cols,mat.rows,mat.step,QImage::Format_RGB888);
        return img.rgbSwapped();

    }
    else
    {
        qDebug()<<"ERROR:MatcouldnotbeconvertedtoQImage.";
        return QImage();
    }
}

Mat mainWidget::QImage2Mat(const QImage& qimage)
{
    Mat mat = Mat(qimage.height(), qimage.width(), CV_8UC4, (uchar*)qimage.bits(), qimage.bytesPerLine());
    Mat mat2 = Mat(mat.rows, mat.cols, CV_8UC3 );
    int from_to[] = { 0,0, 1,1, 2,2 };
    mixChannels( &mat, 1, &mat2, 1, from_to, 3 );
    return mat2;
}
Mat mainWidget::Canny_check()
{
    //载入原始图
        cv::Mat srcImage;
        srcImage = QImage2Mat(*myImage);
        //----------------------------------------------------------------------------------
        //	高阶的canny用法，转成灰度图，降噪，用canny，最后将得到的边缘作为掩码，拷贝原图到效果图上，得到彩色的边缘图
        //----------------------------------------------------------------------------------
        Mat dstImage,edge,grayImage;

        // 【1】创建与src同类型和大小的矩阵(dst)
        dstImage.create( srcImage.size(), srcImage.type() );

        // 【2】将原图像转换为灰度图像
        cvtColor( srcImage, grayImage, COLOR_BGR2GRAY );

        // 【3】先用使用 3x3内核来降噪
        blur( grayImage, edge, Size(3,3) );

        // 【4】运行Canny算子
        Canny( edge, edge, 3, 9,3 );

        //【5】将g_dstImage内的所有元素设置为0
        dstImage = Scalar::all(0);

        //【6】使用Canny算子输出的边缘图g_cannyDetectedEdges作为掩码，来将原图g_srcImage拷到目标图g_dstImage中
        srcImage.copyTo( dstImage, edge);

        return dstImage;
}

void mainWidget::on_disposeBtn_clicked()
{
    Mat resultmat = Canny_check();//resultmat完整！
    resultimage = Mat2QImage(resultmat);//将mat格式转换为Qimage格式
//    ui->label->setPixmap(QPixmap::fromImage(resultimage));//将结果在label上显示
    QGraphicsScene *scene = new QGraphicsScene;
    scene->addPixmap(QPixmap::fromImage(resultimage));
    ui->resultView->setScene(scene);
    ui->resultView->resize(resultimage.width()+10, resultimage.height()+10);
    ui->resultView->show();
}

void mainWidget::on_saveBtn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(
                    this, "保存图片",
                    ".",
                    "图片 (*.jpg *.bmp *.pbm *.pgm *.png *.ppm *.xbm *.xpm);;所有文件(*.*)");
    if(!fileName.isEmpty())
    {
        saveFile(fileName);
    }
    else
    {
        return;
    }
}

bool mainWidget::saveFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadWrite))
    {
        QMessageBox::warning(this,tr("保存文件"),tr("无法保存文件 %1:\n%2").arg(filename).arg(file.errorString()));
        return false;
    }
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    resultimage.save(&buffer, "JPG");
    file.write(ba);
    QMessageBox::information(this,tr("保存文件"),tr("文件已保存至：\n%1").arg(filename));
    return true;
}
