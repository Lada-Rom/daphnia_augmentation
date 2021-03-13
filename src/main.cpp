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
    while (true) {
        cap >> frame;
        if (frame.empty())
            break;
        cv::imshow("Frame", frame);
        char key = (char)cv::waitKey(1);
        if (key == 27)
            break;
    }

    cap.release();
    cv::destroyAllWindows();
}
