#include "LoadImage.h"
#include "ui_LoadImage.h"

#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QMenu>
#include <QAction>

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

static int pixel( const cv::Mat &mat, int x, int y )
{   
    if ( mat.empty() ) { return 0; }
    int r = mat.rows;
    int c = mat.cols;
    if ( x < 0 || x >= c || y < 0 || y >= r ) {
        return 0xFF00;
    }

    cv::Vec3b p = mat.at<cv::Vec3b>(y, x);
    return ((uint)p[0]+(uint)p[1]+(uint)p[2])/3;
}

LoadImage::LoadImage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoadImage)
{

    ui->setupUi(this);
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

void LoadImage::on_pushButton_carved_clicked()
{
    ui->image_label->showCarved();
}

void LoadImage::on_pushButton_contour_clicked()
{
    ui->image_label->showContour();
}

void LoadImage::on_pushButton_fill_clicked()
{
    ui->image_label->showFill();
}

void LoadImage::on_pushButton_route_clicked()
{
    ui->image_label->showRoute();
}

void ImageLabel::loadImage( QString filePath_ )
{
    origin = cv::imread( filePath_.toStdString(), cv::IMREAD_UNCHANGED );
    carved = origin.clone();
    route = origin.clone();
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
    seeds( carved, 2, 2, 4, labels, count );
    //                labels->contour
    labelContourMask( labels, contour, false );
    // origin ----------------------------> contour
    carved.setTo(cv::Scalar(255, 255, 255), contour);

    showCarved();
    filePath = filePath_;
}

void ImageLabel::showOrigin()
{
    if ( origin.empty() ) { return; }
    setPixmap( QPixmap::fromImage(
            Utils::cvMat2QImage( origin ) ) );
}

void ImageLabel::showCarved()
{
    if ( carved.empty() ) { return; }
    setPixmap( QPixmap::fromImage(
            Utils::cvMat2QImage( carved ) ) );
}

void ImageLabel::showContour()
{
    if ( contour.empty() ) { return; }
    setPixmap( QPixmap::fromImage(
            Utils::cvMat2QImage( contour ) ) );
}

void ImageLabel::showFill()
{
    if ( fill.empty() ) { return; }
    setPixmap( QPixmap::fromImage(
            Utils::cvMat2QImage( fill ) ) );
}

void ImageLabel::showRoute()
{
    if ( route.empty() ) { return; }
    setPixmap( QPixmap::fromImage(
            Utils::cvMat2QImage( route ) ) );
}


static std::vector<cv::Point2i> connectBeginEnd( const cv::Mat &mat, cv::Point2i begin, cv::Point2i end )
{
    int dirX = (end.x-begin.x>0) ? 1 : -1;
    int dirY = (end.y-begin.y>0) ? 1 : -1;
    const int dx[3] = { dirX, dirX,    0 };
    const int dy[3] = {    0, dirY, dirY };
    std::vector<cv::Point2i> pts0;
    std::vector<cv::Point2i> pts1;
    while( (end.x-begin.x)*dirX > 0 && (end.y-begin.y)*dirY > 0 ) {
        int x, y, dist, hit;

        // update begin
        x = begin.x;
        y = begin.y;
        dist = qAbs( pixel(mat,x,y) - pixel(mat,x+dx[0],y+dy[0]) );
        hit = 0;
        for ( int i = 1; i < sizeof(dx); ++i ) {
            int distance = qAbs( pixel(mat,x,y) - pixel(mat,x+dx[i],y+dy[i]) );
            if ( distance < dist ) {
                dist = distance;
                hit = i;
            }
        }
        begin.x += dx[hit];
        begin.y += dy[hit];
        pts0.push_back( begin );

        // update end
        x = end.x;
        y = end.y;
        dist = qAbs( pixel(mat,x,y) - pixel(mat,x-dx[0],y-dy[0]) );
        hit = 0;
        for ( int i = 1; i < sizeof(dx); ++i ) {
            int distance = qAbs( pixel(mat,x,y) - pixel(mat,x-dx[i],y-dy[i]) );
            if ( distance < dist ) {
                dist = distance;
                hit = i;
            }
        }
        end.x -= dx[hit];
        end.y -= dy[hit];
        pts1.push_back( end );
    }
    while( (end.y-begin.y)*dirY > 0 ) {
        begin.y += dirY;
        pts0.push_back( begin );
        end.y -= dirY;
        pts1.push_back( end );
    }
    while( (end.x-begin.x)*dirX > 0 ) {
        begin.x += dirX;
        pts0.push_back( begin );
        end.x -= dirX;
        pts1.push_back( end );
    }

    for ( int i = pts1.size() - 1; i >= 0; --i ) {
        pts0.push_back( pts1.at(i) );
    }
    return pts0;
}

void ImageLabel::mousePressEvent( QMouseEvent *e )
{
    if ( e->button() == Qt::RightButton ) {
        return;
    }
    if ( mode == SUPERPIXEL ) {
        if ( origin.empty() ) { return; }
        QPoint point = e->pos();
        cv::Point2i pt( point.x(), point.y() );
        RegionSeedFill rsf;
        rsf.ccaBoundaryFill( pt, contour, fill );
        showFill();
        return;
    }
    if ( mode == LINETRACING ) {
        static bool close = false;
        static QPoint pts[2];
        if ( close ) {
            pts[1] = e->pos();
            std::vector<cv::Point2i> curve = connectBeginEnd( origin, 
                 cv::Point2i(pts[0].x(),pts[0].y()),
                 cv::Point2i(pts[1].x(),pts[1].y()) );
            uchar *ptr = route.data;
            for( int i = 0; i < curve.size(); ++i ) {
                int x = curve.at(i).x;
                int y = curve.at(i).y;
                int loc = y * route.cols + x;
                ptr[3*loc+0] = 0;
                ptr[3*loc+1] = 0;
                ptr[3*loc+2] = 255;
            }
            showRoute();
        } else {
            pts[0] = e->pos();
        }
        close = !close;
    }
}

ImageLabel::ImageLabel(QWidget *parent /*= 0*/)
  : mode( SUPERPIXEL )
{
    menu = new QMenu;

    superpixel = new QAction( "superpixel", this );
    linetracing = new QAction( "linetracing", this );
    connect( superpixel, SIGNAL(triggered()),
             this, SLOT(setToSuperpixel()) );
    connect( linetracing, SIGNAL(triggered()),
             this, SLOT(setToLinetracing()) );
    menu->addAction( superpixel );
    menu->addAction( linetracing );

    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL(customContextMenuRequested(QPoint)),
             this, SLOT(showMenu(QPoint)) );
}

void ImageLabel::showMenu( QPoint pos )
{
    menu->exec( mapToGlobal(pos) );
}