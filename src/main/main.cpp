#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

const size_t num_bubbles{ 4 };

const std::string main_dir = "../../../data/";
const std::string masks_subdir = "masks/";
const std::string video_subdir = "videos/";

const std::string main_name = "bubble";
const std::string black_label_name = "_black";
const std::string white_label_name = "_white";
const std::string main_ext = ".tiff";

std::vector<cv::Mat> getKit(const std::string& label_name) {
    std::vector<cv::Mat> kit;
    std::string fullname = main_dir + masks_subdir + main_name;

    cv::Mat new_image;
    for (size_t i{}; i < num_bubbles; ++i) {
        new_image = cv::imread(
            fullname + std::to_string(i) + label_name + main_ext, cv::IMREAD_ANYDEPTH);
        kit.push_back(new_image);
    }

    return kit;
}

void showBubble(cv::Mat& frame, 
    const cv::Mat& black, const cv::Mat& white,
    double alpha, double beta, size_t x, size_t y) {
    cv::Rect roi(x, y, black.cols, black.rows);
    cv::Mat float_frame;
    cv::medianBlur(frame(roi), float_frame, 9);
    float_frame.convertTo(float_frame, CV_32FC1);

    frame(roi) -= black.mul(float_frame);
    frame(roi) += white.mul(float_frame);
}

void showAllBubbles(cv::Mat& frame,
    const std::vector<cv::Mat>& black_kit, 
    const std::vector<cv::Mat>& white_kit, 
    size_t step, const std::vector<size_t>& borders, bool draw_border = true) {

    size_t left_border = borders[0];
    size_t right_border = frame.cols - borders[1];
    size_t upper_border = borders[2];
    size_t lower_border = frame.rows - borders[3];

    if (draw_border) {
        cv::Rect rect = cv::Rect(
            borders[0], borders[2],
            right_border - left_border, lower_border - upper_border);
        cv::rectangle(frame, rect, cv::Scalar{ 0, 0, 0 });
    }

    //if bubbles too much, it can multyplies
    size_t x = left_border, y = upper_border;
    for (size_t i{}; i < num_bubbles; ++i) {
        showBubble(frame, black_kit[i], white_kit[i], 0.4, 0.2, x, y);
        if (x + step + black_kit[i].cols < right_border) {
            x += step;
        }
        else {
            x = left_border;
            if (y + step < lower_border)
                y += step;
            else
                continue;
        }
    }
}

void playVideoWithBubbles(cv::VideoCapture& cap, size_t step) {
    cv::Mat frame;

    //borders: left, right, up, down
    std::vector<size_t> borders{110, 120, 60, 60};
    std::vector<cv::Mat> bubble_black_kit = getKit(black_label_name);
    std::vector<cv::Mat> bubble_white_kit = getKit(white_label_name);

    while (true) {
        cap >> frame;
        if (frame.empty())
            break;
        cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

        showAllBubbles(frame, bubble_black_kit, bubble_white_kit, step, borders);

        cv::imshow("Frame", frame);
        char key = (char)cv::waitKey(10);
        if (key == 27)
            break;
    }
}

int main() {
    std::string video_name = "Male_Day13.avi";
    std::string source_filename = main_dir + video_subdir + video_name;
    cv::VideoCapture cap(source_filename);

    if (!cap.isOpened()) {
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }

    playVideoWithBubbles(cap, 150);

    cap.release();
    cv::destroyAllWindows();

}
