#include "LoadImage.h"
#include <QApplication>
#include <QPoint>
#include <QString>
#include <QDebug>
#include <opencv2/opencv.hpp> 

int main( int argc, char **argv )
{
    QApplication a( argc, argv );
    LoadImage li;
    li.show();
    return a.exec();
}