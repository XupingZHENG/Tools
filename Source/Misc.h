#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

template<typename ElemType>
bool equals(const std::vector<ElemType>& lhs, const std::vector<ElemType>& rhs)
{
    if (lhs.size() != rhs.size())
        return false;

    int size = (int)lhs.size();
    for (int i = 0; i < size; i++)
    {
        if (lhs[i] != rhs[i])
            return false;
    }

    return true;
}

inline cv::Mat IplImageToMat(const IplImage* image, bool copyData)
{
    if (!image)
        return cv::Mat();

    int imageDepth = IPL2CV_DEPTH(image->depth);
    int type = CV_MAKETYPE(imageDepth, image->nChannels);
    cv::Mat ret(image->height, image->width, type, image->imageData, image->widthStep);
    return copyData ? ret.clone() : ret;
}

inline IplImage MatToIplImageHeader(const cv::Mat& mat)
{
    CV_Assert(mat.dims <= 2);
    IplImage img;
    cvInitImageHeader(&img, mat.size(), cvIplDepth(mat.flags), mat.channels());
    cvSetData(&img, mat.data, (int)mat.step[0]);
    return img;
}

inline IplImage* MatToIplImage(const cv::Mat& mat)
{
    IplImage img = MatToIplImageHeader(mat);
    return cvCloneImage(&img);
}

inline void rotateImageClockWise90(IplImage** image)
{
    IplImage* src = *image;
    IplImage* dst = cvCreateImage(cv::Size(src->height, src->width), src->depth, src->nChannels);
    cvTranspose(src, dst);
    cvFlip(dst, 0, 1);
    *image = dst;
    cvReleaseImage(&src);
}

inline void imshowResize(const std::string& winName, const cv::Mat& image, double scale = 0.5)
{
    cv::Mat temp;
    if (scale == 1)
        temp = image;
    else
        cv::resize(image, temp, cv::Size(), scale, scale, scale < 0.5 ? cv::INTER_AREA : cv::INTER_LINEAR);
    cv::imshow(winName, temp);
}

template<typename DataType>
inline void drawPoints(cv::Mat& image, const std::vector<cv::Point_<DataType> >& points, cv::Scalar color, int thickness = 8)
{
    int size = (int)points.size();
    for (int i = 0; i < size; i++)
        cv::circle(image, points[i], thickness, color, -1);
}

template<typename DataType>
inline void drawLineSegments(cv::Mat& image, const std::vector<cv::Vec<DataType, 4> >& lineSegs, cv::Scalar color)
{
    int size = (int)lineSegs.size();
    for (int i = 0; i < size; i++)
        cv::line(image, cv::Point(lineSegs[i][0], lineSegs[i][1]), cv::Point(lineSegs[i][2], lineSegs[i][3]), color);
}

template<typename DataType>
inline void drawLineSegmentsHighlightEndPoints(cv::Mat& image, const std::vector<cv::Vec<DataType, 4> >& lineSegs, cv::Scalar color)
{
    int size = (int)lineSegs.size();
    for (int i = 0; i < size; i++)
    {
        cv::Point p1(lineSegs[i][0], lineSegs[i][1]);
        cv::Point p2(lineSegs[i][2], lineSegs[i][3]);
        cv::line(image, p1, p2, color);
        cv::circle(image, p1, 3, color);
        cv::circle(image, p2, 3, color);
    }
}

template<typename DataType>
inline void drawTransformPoints(cv::Mat& colorImage, const std::vector<cv::Point_<DataType> >& points,
    const cv::Mat& A, cv::Scalar color)
{
    CV_Assert(colorImage.data && colorImage.type() == CV_8UC3);
    CV_Assert(A.rows == 2 && A.cols == 3 && A.type() == CV_64FC1);
    int numPoints = (int)points.size();
    const double* a = A.ptr<double>();
    for (int i = 0; i < numPoints; i++)
    {
        double srcx = points[i].x;
        double srcy = points[i].y;
        double dstx = srcx * a[0] + srcy * a[1] + a[2];
        double dsty = srcx * a[3] + srcy * a[4] + a[5];
        cv::circle(colorImage, cv::Point(dstx, dsty), 16, cv::Scalar(255, 0, 255), 2);
    }
}

template<typename DataType>
inline void flipPoints(const std::vector<cv::Point_<DataType> >& src,
    std::vector<cv::Point_<DataType> >& dst, cv::Size size, bool flipTopDown)
{
    int length = (int)src.size();

    if (&src != &dst)
    {
        dst.clear();
        dst.resize(length);
    }

    if (flipTopDown)
    {
        for (int i = 0; i < length; i++)
        {
            dst[i].x = src[i].x;
            dst[i].y = size.height - src[i].y;
        }
    }
    else
    {
        for (int i = 0; i < length; i++)
        {
            dst[i].x = size.width - src[i].x;
            dst[i].y = src[i].y;
        }
    }
}

template<typename DataType>
inline void rotatePoints180Degrees(const std::vector<cv::Point_<DataType> >& src,
    std::vector<cv::Point_<DataType> >& dst, cv::Size size)
{
    int length = (int)src.size();

    if (&src != &dst)
    {
        dst.clear();
        dst.resize(length);
    }

    DataType width = size.width, height = size.height;
    for (int i = 0; i < length; i++)
    {
        dst[i].x = width - src[i].x;
        dst[i].y = height - src[i].y;
    }
}

template<typename DataType>
inline void drawRects(cv::Mat& image, const std::vector<cv::Rect_<DataType> >& rects, cv::Scalar color, int thickness = 1)
{
    int size = (int)rects.size();
    for (int i = 0; i < size; i++)
        cv::rectangle(image, rects[i], color, thickness);
}

inline void drawConnComps(const cv::Mat& labels, const std::vector<int>& indexes, cv::Mat& connCompImage)
{
    CV_Assert(labels.data && labels.type() == CV_32SC1);

    int rows = labels.rows, cols = labels.cols;
    connCompImage.create(rows, cols, CV_8UC1);
    connCompImage.setTo(0);

    cv::Mat pred;
    int size = (int)indexes.size();
    for (int i = 0; i < size; i++)
    {
        pred = (labels == indexes[i]);
        connCompImage.setTo(255, pred);
    }
}

inline void drawConnComps(const cv::Mat& labels, const std::vector<int>& indexes, cv::Mat& connCompImage, cv::Mat& minAreaRectImage)
{
    CV_Assert(labels.data && labels.type() == CV_32SC1);

    int rows = labels.rows, cols = labels.cols;
    connCompImage.create(rows, cols, CV_8UC1);
    connCompImage.setTo(0);
    minAreaRectImage.create(rows, cols, CV_8UC1);
    minAreaRectImage.setTo(0);

    cv::Mat pred;
    int size = (int)indexes.size();
    for (int i = 0; i < size; i++)
    {
        pred = (labels == indexes[i]);
        connCompImage.setTo(255, pred);
        //cv::imshow("curr", connCompImage);

        cv::RotatedRect rotRect = minAreaRect(pred);
        cv::Point2f vertexes[4];
        rotRect.points(vertexes);
        for (int j = 0; j < 4; j++)
            cv::line(minAreaRectImage, vertexes[j], vertexes[(j + 1) % 4], 255);
        //cv::imshow("curr rect", minAreaRectImage);

        //cv::waitKey(0);
    }
}

inline void drawConnCompRects(const cv::Mat& stats, cv::Size size, cv::Mat& rectsImage, bool fill = false)
{
    CV_Assert(stats.data && stats.type() == CV_32SC1);

    int rows = size.height, cols = size.width;
    rectsImage.create(rows, cols, CV_8UC1);
    rectsImage.setTo(0);

    int numComps = stats.rows;
    for (int i = 1; i < numComps; i++)
    {
        cv::Rect rect = *(const cv::Rect*)stats.ptr<unsigned char>(i);
        cv::rectangle(rectsImage, rect, 255, fill ? -1 : 1);
    }
}

inline void drawRotatedRect(cv::Mat& image, const cv::RotatedRect& rect, cv::Scalar color, int thick = 4)
{
    cv::Point2f vertexes[4];
    rect.points(vertexes);
    for (int i = 0; i < 4; i++)
        cv::line(image, vertexes[i], vertexes[(i + 1) % 4], color, thick);
}

inline void normalizeImageHeight(cv::Mat& image, int height)
{
    double scale = double(height) / image.rows;
    int width = cvRound(scale * image.cols);
    cv::resize(image, image, cv::Size(width, height));
}

inline void normalizeImageWidth(cv::Mat& image, int width)
{
    double scale = double(width) / image.cols;
    int height = cvRound(scale * image.rows);
    cv::resize(image, image, cv::Size(width, height));
}

inline void normalizeImageMaxLength(cv::Mat& image, int maxLength)
{
    if (image.rows > image.cols)
        normalizeImageHeight(image, maxLength);
    else
        normalizeImageWidth(image, maxLength);
}

inline void normalizeImageHeight(const cv::Mat& src, int height, cv::Mat& dst)
{
    double scale = double(height) / src.rows;
    int width = cvRound(scale * src.cols);
    cv::resize(src, dst, cv::Size(width, height));
}

inline void normalizeImageWidth(const cv::Mat& src, int width, cv::Mat& dst)
{
    double scale = double(width) / src.cols;
    int height = cvRound(scale * src.rows);
    cv::resize(src, dst, cv::Size(width, height));
}

inline void normalizeImageMaxLength(cv::Mat& src, int maxLength, cv::Mat& dst)
{
    if (src.rows > src.cols)
        normalizeImageHeight(src, maxLength, dst);
    else
        normalizeImageWidth(src, maxLength, dst);
}

class Timer
{
public:
    Timer()
        : begTime(cv::getTickCount()), endTime(cv::getTickCount()), 
          freq(cv::getTickFrequency()), elapsedTime(0)
    {};

    void begin() 
    { 
        begTime = cv::getTickCount(); 
    };

    void end() 
    { 
        endTime = cv::getTickCount(); 
        elapsedTime = double(endTime - begTime) / freq;
    };

    double elapsed() const
    {
        return elapsedTime;
    }

private:
    long long int begTime, endTime;
    double freq, elapsedTime;
};

class AccumTimer
{
public:
    AccumTimer() 
    {
        clear();
    }

    void begin()
    {
        t.begin();
    }

    void end()
    {
        t.end();
        accum += t.elapsed();
        count++;
    }

    double elapsed() const
    {
        return accum;
    }

    double num() const
    {
        return count;
    }

    double avgElapsed() const
    {
        return count ? accum / count : 0;
    }

    void clear()
    {
        count = 0;
        accum = 0;
    }

private:
    int count;
    double accum;
    Timer t;
};

inline bool horiOverlap(const cv::Rect& lhs, const cv::Rect& rhs)
{
    int left = std::max(lhs.x, rhs.x);
    int right = std::min(lhs.x + lhs.width, rhs.x + rhs.width);
    return left < right;
}

inline bool vertOverlap(const cv::Rect& lhs, const cv::Rect& rhs)
{
    int top = std::max(lhs.y, rhs.y);
    int bottom = std::min(lhs.y + lhs.height, rhs.y + rhs.height);
    return top < bottom;
}

inline bool readSingleLineFile(const std::string& path, std::string& content)
{
    content.clear();
    std::ifstream f(path);
    if (!f)
        return false;
    std::getline(f, content);
    f.close();
    return true;
}

inline bool writeSingleLineFile(const std::string& path, const std::string& content)
{
    std::ofstream f(path);
    if (!f)
        return false;
    f << content;
    f.close();
    return true;
}

inline std::string toString(int i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}