#include "ArResource.hpp"
#include <algorithm>

void ArResource::FourCh2ThreeCh(cv::Mat& img_rgb)
{
    if (img_src.channels() == 4) cv::cvtColor(img_src, img_rgb, cv::COLOR_BGRA2BGR);
    else img_rgb = img_src.clone();
}

void ArResource::Chromakey(cv::Mat& img_base)
{
    cv::Mat img_mask;       // 背景の領域
    cv::Mat img_mask_inv;   // 背景ではない領域
    cv::Mat img_rgb;
    cv::Mat img_src_resized_trimmed;
    cv::Mat img_src_resized;
    FourCh2ThreeCh(img_rgb);

    int abs_pos_x = (int)(img_base.cols * position_x);    //X
    int abs_pos_y = (int)(img_base.rows * position_y);    //Y
    int abs_width = (int)(img_base.cols * width);          //width
    int abs_height = (int)(img_base.rows * height);         //height
    
    cv::resize(img_rgb, img_src_resized, cv::Size(abs_width, abs_height), cv::INTER_AREA);

    cv::inRange(
        img_src_resized,
        cv::Scalar(min_b * 255, min_g * 255, min_r * 255),
        cv::Scalar(max_b * 255, max_g * 255, max_r * 255),
        img_mask);
    
    
    // カーネルの定義
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernel_size, kernel_size));
    
    // 膨張処理
    if (morph == "erode") cv::erode(img_mask, img_mask, kernel);
    else if (morph == "dilate") cv::dilate(img_mask, img_mask, kernel);
    cv::cvtColor(img_mask, img_mask, cv::COLOR_GRAY2BGR);
    cv::bitwise_not(img_mask, img_mask_inv);
    cv::bitwise_and(img_src_resized, img_mask_inv, img_src_resized_trimmed);
    cv::Rect roi
    (
        abs_pos_x,    //X
        abs_pos_y,    //Y
        abs_width,      //width
        abs_height    //height
    );
    cv::Mat img_roi = img_base(roi);
    cv::Mat img_roi_trimmed;
    cv::bitwise_and(img_roi, img_mask, img_roi_trimmed);
    cv::bitwise_or(img_src_resized_trimmed, img_roi_trimmed, img_roi);
    img_roi.copyTo(img_base(roi));
}

void ArResource::PersepectiveTransform(cv::Mat& img_base)
{
    bool chromakey = true;

    cv::Mat img_rgb;
    // ポリゴンの頂点座標を定義(切り抜き用)
    std::vector<cv::Point> polygon;
    polygon.push_back(cv::Point(bottom_left[0] * img_base.cols, bottom_left[1] * img_base.rows));
    polygon.push_back(cv::Point(bottom_right[0] * img_base.cols, bottom_right[1] * img_base.rows));
    polygon.push_back(cv::Point(top_right[0] * img_base.cols, top_right[1] * img_base.rows));
    polygon.push_back(cv::Point(top_left[0] * img_base.cols, top_left[1] * img_base.rows));
    // ポリゴンの頂点座標を定義(透視変換用)
    cv::Point2f pts1[] = {
        cv::Point2f(0,img_src.rows),cv::Point2f(img_src.cols,img_src.rows), 
        cv::Point2f(img_src.cols,0),cv::Point2f(0,0) };
    cv::Point2f pts2[4];
    std::copy(polygon.begin(), polygon.end(), pts2);
    FourCh2ThreeCh(img_rgb);

    cv::Mat img_mask = cv::Mat::zeros(img_base.size(), CV_8UC3);
    cv::fillPoly(img_mask, std::vector<std::vector<cv::Point>>{polygon}, cv::Scalar(255, 255, 255));
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    
    //予定している変換後座標と実際の変換後座標はfloatの計算過程で若干違ってきてしまうため補正
    cv::erode(img_mask, img_mask, kernel);

    cv::Mat img_mask_inv; //ポリゴン領域外のマスク
    cv::bitwise_not(img_mask, img_mask_inv);

    // 透視変換
    cv::Mat psp_mat = cv::getPerspectiveTransform(pts1, pts2);
    cv::Mat img_temp = cv::Mat::zeros(img_base.size(), CV_8UC3);
    cv::warpPerspective(img_rgb, img_temp, psp_mat, img_temp.size(), cv::INTER_LINEAR);

    cv::Mat img_mask2;//モデルの外～ポリゴンの内側
    if (combine == "perspective + chromakey")
    {
        cv::inRange(
            img_temp,
            cv::Scalar(min_b * 255, min_g * 255, min_r * 255),
            cv::Scalar(max_b * 255, max_g * 255, max_r * 255),
            img_mask2);
        cv::cvtColor(img_mask2, img_mask2, cv::COLOR_GRAY2BGR);
        cv::bitwise_or(img_mask_inv, img_mask2, img_mask_inv); //モデルの外側全域
    }
    cv::bitwise_not(img_mask_inv, img_mask); //モデルの内側 or ポリゴンの内側
    cv::bitwise_and(img_base, img_mask_inv, img_base);
    cv::bitwise_and(img_temp, img_mask, img_temp);
    cv::bitwise_or(img_base, img_temp, img_base);

}

ArResource::~ArResource()
{
    if (hwnd) hwnd = NULL;
}


/// <summary>
/// WindowハンドルからMat型へ変換する
/// </summary>
bool ArResource::Hwnd2Mat()
{
    bool ret = true;

    HDC hwindowDC, hwindowCompatibleDC;

    int height, width, srcheight, srcwidth;
    HBITMAP hbwindow;
    hwindowDC = GetDC(hwnd);
    hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);
    RECT windowsize;    // get the height and width of the screen
    GetClientRect(hwnd, &windowsize);

    srcheight = windowsize.bottom;
    srcwidth = windowsize.right;
    height = windowsize.bottom;  // change this to whatever size you want to resize to
    width = windowsize.right;

    img_src.create(height, width, CV_8UC4);

    // create a bitmap
    hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
    
    try
    {
        BITMAPINFOHEADER bi;

        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = width;
        bi.biHeight = -height;  // this is the line that makes the bitmap un-inverted
        bi.biPlanes = 1;
        bi.biBitCount = 32;
        bi.biCompression = BI_RGB;
        bi.biSizeImage = 0;
        bi.biXPelsPerMeter = 0;
        bi.biYPelsPerMeter = 0;
        bi.biClrUsed = 0;
        bi.biClrImportant = 0;

        // use the previously created device context with the bitmap
        SelectObject(hwindowCompatibleDC, hbwindow);
        // copy from the window device context to the bitmap device context
        StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY);
        GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, img_src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    }
    catch (...)
    {
        ret = false;
    }
    
    // avoid memory leaks
    if (hbwindow) DeleteObject(hbwindow);
    if (hwindowCompatibleDC) DeleteDC(hwindowCompatibleDC);
    if (hwindowDC) ReleaseDC(hwnd, hwindowDC);
    return ret;
}

/// <summary>
/// リソースの取得
/// </summary>
bool ArResource::GetResource()
{
    if (image_type == "window")
    {
        hwnd = FindWindowA(NULL, image_name.c_str());
        if (!hwnd) { std::cerr << "can't find the window" << std::endl; std::cin.get(); return false; }
    }
    else
    {
        img_src = cv::imread(image_name);
    }
    return true;
}

bool ArResource::GetTexture()
{
    if (image_type == "window")
    {
        if(!Hwnd2Mat()) { std::cerr << "failed to capture" << std::endl; std::cin.get(); return false; }
    }
    return true;
}

