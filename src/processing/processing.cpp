#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

const size_t num_bubbles{ 10 };

void showWindow(const cv::Mat& image, std::string name = "Image",
    size_t x = 0, size_t y = 0) {
    imshow(name, image);
    cv::moveWindow(name, x, y);
}

void imshowStack(const std::string& name, size_t x, size_t y,
    const cv::Mat& mat1, const cv::Mat& mat2, const cv::Mat& mat3, const cv::Mat& mat4) {
    cv::Mat general = cv::Mat::zeros(mat1.rows, 4 * mat1.cols, CV_8UC1);
    cv::Rect roi;

    roi = cv::Rect(0 * mat1.cols, 0, mat1.cols, mat1.rows);
    mat1.copyTo(general(roi));

    roi = cv::Rect(1 * mat1.cols, 0, mat1.cols, mat1.rows);
    mat2.copyTo(general(roi));

    roi = cv::Rect(2 * mat1.cols, 0, mat1.cols, mat1.rows);
    mat3.copyTo(general(roi));

    roi = cv::Rect(3 * mat1.cols, 0, mat1.cols, mat1.rows);
    mat4.copyTo(general(roi));

    showWindow(general, name, x, y);
}

int calcMedian(const cv::Mat& mat) {
    double sum{};
    for (size_t i{}; i < mat.cols; ++i)
        for (size_t j{}; j < mat.rows; ++j) {
            sum += mat.at<uchar>(j, i);
        }

    return sum / (mat.cols * mat.rows);
}

void writeMasks(const cv::Mat& image,
    const std::string& black_path, const std::string& white_path,
    size_t x, size_t y, size_t index) {
    //black mask
    cv::Mat source;
    cv::Mat source_blured;

    cv::medianBlur(image, source_blured, 13);
    image.convertTo(source, CV_32FC1, 1.0 / 255);
    source_blured.convertTo(source_blured, CV_32FC1, 1.0 / 255);

    cv::Mat source_diff = source_blured - source;
    cv::Mat black_mask = source_diff / source_blured;

    cv::threshold(black_mask, black_mask, 0.04, 255, cv::ThresholdTypes::THRESH_TOZERO);
    cv::imwrite(black_path, black_mask);

    //white mask
    cv::Mat negative = 255 - image;
    cv::Mat neg_blured;

    cv::medianBlur(negative, neg_blured, 13);
    negative.convertTo(negative, CV_32FC1, 1.0 / 255);
    neg_blured.convertTo(neg_blured, CV_32FC1, 1.0 / 255);

    cv::Mat neg_diff = neg_blured - negative;
//    cv::Mat white_mask = neg_diff;
    cv::Mat white_mask = neg_diff / neg_blured;

    cv::threshold(white_mask, white_mask, 0.05, 255, cv::ThresholdTypes::THRESH_TOZERO);
    cv::imwrite(white_path, white_mask);

    imshowStack("image" + std::to_string(index), x, y, image, negative * 255, source_diff * 255, neg_diff * 255);
}

std::vector<cv::Mat> getAllBubbles(const std::string& dir, const std::string& name, const std::string& ext) {
    std::string fullname = dir + name;
    std::vector<cv::Mat> kit;

    for (size_t i{}; i < num_bubbles; ++i) {
        kit.push_back(cv::imread(fullname + std::to_string(i) + ext, cv::IMREAD_GRAYSCALE));
    }

    return kit;
}

void writeAllMasks(const std::vector<cv::Mat> kit, const std::string& dir, const std::string& name,
    const std::string& black_label_name, const std::string& white_label_name, const std::string& ext) {
    std::string fullname = dir + name;

    std::string black_name;
    std::string white_name;
    for (size_t i{}; i < num_bubbles; ++i) {
        black_name = fullname + std::to_string(i) + black_label_name + ext;
        white_name = fullname + std::to_string(i) + white_label_name + ext;
        writeMasks(kit[i], black_name, white_name, 50, i * 75 + 50, i);
    }
}

int main() {
    const std::string src_dir = "../../../data/bubbles/";
    const std::string dst_dir = "../../../data/masks/";
    const std::string main_name = "bubble";
    const std::string main_ext = ".png";
    const std::string sub_ext = ".tiff";

    std::vector<cv::Mat> bubbles_kit = getAllBubbles(src_dir, main_name, main_ext);
    writeAllMasks(bubbles_kit, dst_dir, main_name, "_black", "_white", sub_ext);

    cv::waitKey(0);
}
