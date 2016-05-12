#ifndef UTILS_H
#define UTILS_H

namespace cv {
    class Mat;
}

class QImage;

class Utils
{
public:
    static QImage cvMat2QImage(const cv::Mat& mat);
    static cv::Mat QImage2cvMat( const QImage &image );
};

#endif  // UTILS_H