#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

struct Params {
    size_t num_bubbles{ 10 };

    std::string main_dir = "../../../data/";
    std::string masks_subdir = "masks/";
    std::string video_subdir = "videos/";

    std::string main_name = "bubble";
    std::string black_label_name = "_black";
    std::string white_label_name = "_white";
    std::string main_ext = ".tiff";
};

class Bubble {
public:
    Bubble() = default;
    Bubble(const cv::Mat& black, const cv::Mat& white)
        : black_mask_(black), white_mask_(white) { }
    ~Bubble() = default;

    size_t rows() const { return black_mask_.rows; }
    size_t cols() const { return black_mask_.cols; }

    size_t x() const { return x_; }
    size_t y() const { return y_; }

    cv::Mat getBlackMask() const { return black_mask_; }
    cv::Mat getWhiteMask() const { return white_mask_; }

    void setPos(size_t x, size_t y) {
        x_ = x;
        y_ = y;
    }

private:
    size_t x_{};
    size_t y_{};
    cv::Mat black_mask_;
    cv::Mat white_mask_;
};

const Params param;

std::vector<cv::Mat> getKit(const std::string& label_name) {
    std::vector<cv::Mat> kit;
    std::string fullname = param.main_dir + param.masks_subdir + param.main_name;

    cv::Mat new_image;
    for (size_t i{}; i < param.num_bubbles; ++i) {
        new_image = cv::imread(
            fullname + std::to_string(i) + label_name + param.main_ext, cv::IMREAD_ANYDEPTH);
        kit.push_back(new_image);
    }

    return kit;
}

std::vector<Bubble> initializeBubbleKit(
    const std::vector<cv::Mat>& black_mask_kit,
    const std::vector<cv::Mat>& white_mask_kit) {
    std::vector<Bubble> bubble_kit;
    for (size_t i{}; i < param.num_bubbles; ++i)
        bubble_kit.push_back({ black_mask_kit[i], white_mask_kit[i] });
    return bubble_kit;
}

void showBubble(cv::Mat& frame, const Bubble& bubble) {
    cv::Rect roi(bubble.x(), bubble.y(), bubble.cols(), bubble.rows());
    cv::Mat float_frame;
    cv::medianBlur(frame(roi), float_frame, 9);
    float_frame.convertTo(float_frame, CV_32FC1);

    frame(roi) -= bubble.getBlackMask().mul(float_frame);
    frame(roi) += bubble.getWhiteMask().mul(float_frame);
}

void showAllBubbles(cv::Mat& frame, std::vector<Bubble>& bubble_kit,
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
    for (size_t i{}; i < param.num_bubbles; ++i) {
        bubble_kit[i].setPos(x, y);
        showBubble(frame, bubble_kit[i]);
        if (x + step + bubble_kit[i].cols() < right_border) {
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
    std::vector<cv::Mat> bubble_black_kit = getKit(param.black_label_name);
    std::vector<cv::Mat> bubble_white_kit = getKit(param.white_label_name);
    std::vector<Bubble> bubble_kit = initializeBubbleKit(bubble_black_kit, bubble_white_kit);

//    size_t i{};
    while (true) {
        cap >> frame;
        if (frame.empty())
            break;
//        cv::imwrite(param.main_dir + "parse/" + std::to_string(i) + ".png", frame);
        cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

        showAllBubbles(frame, bubble_kit, step, borders);

        cv::imshow("Frame", frame);
        char key = (char)cv::waitKey(10);
        if (key == 27)
            break;
//        ++i;
    }
}

int main() {
//    std::string video_name = "Male_Day13.avi";
    std::string video_name = "Female_Day13.avi";

    std::string source_filename = param.main_dir + param.video_subdir + video_name;
    cv::VideoCapture cap(source_filename);

    if (!cap.isOpened()) {
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }

    playVideoWithBubbles(cap, 150);

    cap.release();
    cv::destroyAllWindows();

}
