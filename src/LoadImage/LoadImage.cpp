#include "LoadImage.h"
#include "ui_LoadImage.h"
#include <QFileDialog>
#include <QDir>
#include <QDebug>

#include <cv.h>
#include <opencv2/opencv.hpp> 
#include "Utils.h"
#include "seeds_opencv.h" 
#include "region_seed_fill.h"

#include "Configs.h"

#ifndef IMAGE_DIR
#define IMAGE_DIR ""
#endif

#ifndef DATA_DIR
#define DATA_DIR ""
#endif

LoadImage::LoadImage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoadImage)
{

    ui->setupUi(this);
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}

LoadImage::~LoadImage()
{
    delete ui;
}

void LoadImage::loadImage( QString filePath )
{
    setWindowTitle( filePath );
    ui->image_label->loadImage( filePath );
}

void LoadImage::on_pushButton_loadimg_clicked()
{
    QString fileName = QFileDialog::getOpenFileName( 0, QString(), DATA_DIR,
                                                     tr("Images (*.png *.jpg)") );
    loadImage( fileName );
}

void LoadImage::on_pushButton_origin_clicked()
{
    ui->image_label->showOrigin();
}

void LoadImage::on_pushButton_contour_clicked()
{
    ui->image_label->showContour();
}

void LoadImage::on_pushButton_fill_clicked()
{
    ui->image_label->showFill();
}

void ImageLabel::loadImage( QString filePath_ )
{
    origin = cv::imread( filePath_.toStdString(), cv::IMREAD_UNCHANGED );
    if ( origin.empty() ) { 
        setText( QString("Error at laoding image <%1>").arg(filePath_) );
        filePath.clear();
        origin.release();
        fill.release();
        contour.release();
        return;
    }

    cv::Mat labels;
    int count;
    //     origin --------> labels
    seeds( origin, 2, 2, 4, labels, count );
    //                labels->contour
    labelContourMask( labels, contour, false );
    // origin ----------------------------> contour
    origin.setTo(cv::Scalar(255, 255, 255), contour);

    showOrigin();
    filePath = filePath_;
}

void ImageLabel::showOrigin()
{
    if ( origin.empty() ) {
        qDebug() << "empty origin";
        return;
    }
    setPixmap( QPixmap::fromImage(
            Utils::cvMat2QImage( origin ) ) );
}

void ImageLabel::showContour()
{
    if ( contour.empty() ) {
        qDebug() << "empty contour";
        return;
    }
    setPixmap( QPixmap::fromImage(
            Utils::cvMat2QImage( contour ) ) );
}

void ImageLabel::showFill()
{
    if ( fill.empty() ) {
        qDebug() << "empty fill";
        return;
    }
    setPixmap( QPixmap::fromImage(
            Utils::cvMat2QImage( fill ) ) );
}

void ImageLabel::mousePressEvent( QMouseEvent *e )
{
    if ( origin.empty() ) {
        qDebug() << "Empty image";
        return;
    }
    QPoint point = e->pos();
    qDebug() << point << point.x() << point.y();
    cv::Point2i pt( point.x(), point.y() );
    RegionSeedFill rsf;
    rsf.ccaBoundaryFill( pt, contour, fill );
    showFill();
}
