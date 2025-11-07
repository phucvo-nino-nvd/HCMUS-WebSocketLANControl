#include <opencv2/opencv.hpp>
#include <iostream>

void webcam_cap(double duration_seconds = 10) {
    cv::VideoCapture camera(0);
    if (!camera.isOpened()) {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        return;
    }

    cv::namedWindow("Webcam", cv::WINDOW_AUTOSIZE);
    cv::Mat frame;

    double start = cv::getTickCount();

    while (true) {
        camera >> frame;
        if (frame.empty()) break;

        cv::imshow("Webcam", frame);

        // kiểm tra thời gian trôi qua
        double elapsed = (cv::getTickCount() - start) / cv::getTickFrequency();
        if (elapsed >= duration_seconds) {
            std::cout << "Time up! Closing webcam...\n";
            break;
        }

        if (cv::waitKey(10) == 27) // ESC
            break;
    }

    camera.release();
    cv::destroyAllWindows();
}
