#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

const size_t num_bubbles{ 4 };

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
    cv::Mat neg_image = cv::Scalar{ 255 } - image;
    double black_median = calcMedian(neg_image);
    double white_median = calcMedian(image);

    cv::Mat black_mask;
    cv::threshold(neg_image, black_mask, black_median, 255, cv::ThresholdTypes::THRESH_TOZERO);
    cv::GaussianBlur(black_mask, black_mask, cv::Size(3, 3), 0.9);
    cv::imwrite(black_path, black_mask);

    cv::Mat white_mask;
    cv::threshold(image, white_mask, white_median + 20, 255, cv::ThresholdTypes::THRESH_TOZERO);
    cv::imwrite(white_path, white_mask);

    imshowStack("image" + std::to_string(index), x, y, image, neg_image, black_mask, white_mask);
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

    std::vector<cv::Mat> bubbles_kit = getAllBubbles(src_dir, main_name, main_ext);
    writeAllMasks(bubbles_kit, dst_dir, main_name, "_black", "_white", main_ext);

    cv::waitKey(0);
}
