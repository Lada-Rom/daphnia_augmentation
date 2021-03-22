#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

int main() {

    std::string source_filename = "../../data/Male_Day13.avi";
    cv::VideoCapture cap(source_filename);

    if (!cap.isOpened()) {
        std::cout << "Error opening video stream or file" << std::endl;
        return -1;
    }

    cv::Mat frame;
    cv::Mat bubble = cv::imread("../../data/bubble.png");
    cv::Mat bubble2 = cv::imread("../../data/bubble2.png");
    cv::Mat light = cv::imread("../../data/light.png");
    cv::Rect roi(1100, 150, bubble.cols, bubble.rows);

    while (true) {
        cap >> frame;
        if (frame.empty())
            break;

//        frame(roi) -= bubble;
//        frame(roi) += light;
        cv::addWeighted(frame(roi), 0.5, bubble2, 0.5, 0, frame(roi));

        cv::imshow("Frame", frame);
        char key = (char)cv::waitKey(100);
        if (key == 27)
            break;
    }

    cap.release();
    cv::destroyAllWindows();

}
