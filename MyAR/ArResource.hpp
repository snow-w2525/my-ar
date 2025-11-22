#pragma once
#include <iostream>
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
#include <fstream>

class ArResource
{
public:
    bool GetResource();
    bool GetTexture();
    void FourCh2ThreeCh(cv::Mat& img_rgb);
    void Chromakey(cv::Mat& img_base);
    void PersepectiveTransform(cv::Mat& img_base);
    ~ArResource();

    std::string image_name = "image.png";
    std::string image_type = "image";
    float position_x = 0;
    float position_y = 0;
    float width = 0.25f;
    float height = 0.5f;
    float min_r = 0;
    float min_g = 0;
    float min_b = 1.0f;
    float max_r = 0;
    float max_g = 0;
    float max_b = 1.0f;
    float top_left[2];
    float top_right[2];
    float bottom_left[2];
    float bottom_right[2];
    std::string morph = "erode";
    int kernel_size = 3;
    std::string combine = "chromakey";
    cv::Mat img_src;
    int marker_id = 0;
    bool aruco = false;
    bool aruco_vertical = false;
    float aruco_offset = 0.0f;
    float aruco_shift_x = 0.0f;
    float aruco_shift_y = 0.0f;
    float aruco_shift_z = 0.0f;

    bool is_visble = true;

private:
    bool Hwnd2Mat();
    HWND hwnd;
    
};
