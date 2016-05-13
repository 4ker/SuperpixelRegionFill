#ifndef LOADIMAGE_H
#define LOADIMAGE_H

#include <QWidget>
#include <QLabel>
#include <QDebug>
#include <QMouseEvent>
#include <QPoint>
#include <cv.h>

class QAction;
class QMenu;
class QPoint;

namespace Ui {
class LoadImage;
}

class ImageLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ImageLabel(QWidget *parent = 0);
    ~ImageLabel(){}

    enum MODE {
        LINETRACING,
        SUPERPIXEL,
    } mode;

    void loadImage( QString filePath_ );
    void showOrigin();
    void showContour();
    void showFill();
    void showCarved();
    void showRoute();
    void mousePressEvent( QMouseEvent *e );

private slots:
    void setToLinetracing() { mode = LINETRACING; }
    void setToSuperpixel() { mode = SUPERPIXEL; }
    void showMenu( QPoint pos );

private:
    QString filePath;
    cv::Mat origin, contour, fill, carved, route;
    QMenu *menu;
    QAction *superpixel;
    QAction *linetracing;
};

class LoadImage : public QWidget
{
    Q_OBJECT

public:
    explicit LoadImage(QWidget *parent = 0);
    ~LoadImage();

    void loadImage( QString filePath );

private slots:
    void on_pushButton_loadimg_clicked( );
    void on_pushButton_origin_clicked( );
    void on_pushButton_carved_clicked( );
    void on_pushButton_contour_clicked( );
    void on_pushButton_fill_clicked( );
    void on_pushButton_route_clicked( );

private:
    Ui::LoadImage *ui;
};

#endif // LOADIMAGE_H