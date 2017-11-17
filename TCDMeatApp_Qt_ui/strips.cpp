
#include "strips.h"

// Init background substractor
Ptr<BackgroundSubtractor> bg_model = createBackgroundSubtractorMOG2().dynamicCast<BackgroundSubtractor>();

// Create empy input img, foreground and background image and foreground mask.
Mat img, foregroundMask, backgroundImage, foregroundImg;

bool isComputing = false;   // Flag for whether we are computing oxygen value

int areaSpace = 40000; // When max contour area is larger then this area space, the oxygen contents will be calculated



Mat BgSubtraction(Mat img){
    // Just resize input image if you want
//    float scale = 0.5f;
//    resize(img,img,Size(img.size().width*scale,img.size().height*scale));

    // create foreground mask of proper size
    if( foregroundMask.empty() ){
        foregroundMask.create(img.size(), img.type());
    }

    // compute foreground mask 8 bit image
    // -1 is parameter that chose automatically your learning rate
    // 0 means no background update
    bg_model->apply(img, foregroundMask, 0);

    // smooth the mask to reduce noise in image
    GaussianBlur(foregroundMask, foregroundMask, Size(11,11), 3.5,3.5);

    // threshold mask to saturate at black and white values
    threshold(foregroundMask, foregroundMask, 10,255,THRESH_BINARY);

    // create black foreground image
    foregroundImg = Scalar::all(0);
    // Copy source image to foreground image only in area with white mask
    img.copyTo(foregroundImg, foregroundMask);

    //Get background image
    bg_model->getBackgroundImage(backgroundImage);

    // Show the results
//    imshow("foreground mask", foregroundMask);    // foreground mask image
//    imshow("foreground image", foregroundImg);    // foreground image

    // Show background image
//    if(!backgroundImage .empty()){
//        imshow("mean background image", backgroundImage );
////                int key5 = waitKey(40);
//    }

    return foregroundImg;
}

Mat Parameter::GetBoundingBoxByBgSub2(Mat img, int Colorspace, int DEBUG, Point2f & p1, Point2f & p2){
//Only deal with center part of each frame
//    int widthImg = img.size().width;
    int heightImg = img.size().height;
//    int startX=widthImg/5,startY=0,width=(widthImg/5)*3,height=heightImg;
    int startX=360, startY=0, width=1000-360, height=heightImg;

    Mat ROI(img, Rect(startX,startY,width,height));
    Mat croppedImage;
    ROI.copyTo(croppedImage);
    Mat iMat = croppedImage.clone();

    Mat gsMat, bwMat;   // Grayscale image & binary image
//    cvtColor(iMat, gsMat, COLOR_RGB2GRAY);
////    imshow("gsMat", gsMat);
//    threshold(gsMat, bwMat, 60, 255, THRESH_BINARY);        // Yinghan's threshold

    Scalar lowBound(50,45,50); // (1,45,175)===== (0, 45, 70)
    Scalar upperBound(180,255,255); // (180,255,255)
    // Scalar lowBound(1,45,175); // (1,45,175)
    // Scalar upperBound(180,255,255); // (180,255,255)
    cvtColor(iMat, gsMat, COLOR_RGB2HSV);
    inRange(gsMat, lowBound, upperBound, bwMat);
    dilate(bwMat, bwMat, Mat());

    if(DEBUG==1){
        imshow("bwMat" ,bwMat);
    }


    vector<vector<Point> > contours;
    Mat hierarchy;
    findContours(bwMat, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

    vector<Point> largestContour;
    double maxArea = 0;
    for (vector<Point> contour : contours){
        double area = contourArea(contour);
        if (area > maxArea){
            maxArea = area;
            largestContour = contour;
        }
    }


    // cout << "size contours : " << largestContour.size() << endl;

    Moments M = moments(largestContour);

    int cX = (int) (M.m10 / M.m00);
    int cY = (int) (M.m01 / M.m00);


    // Only when max area is larger than setting area & center of roi is in the center
    // then we compute oxygen contents
    if(maxArea>areaSpace && startX + cX>(img.cols/2)*getLeftLine() && startX + cX<(img.cols/2)+((img.cols/2)*getRightLine())){
        isComputing = true;
    }else{
        isComputing = false;
    }

    // old Scalars
    // int scalar = 25; // for smaller resolutions
    // int scalar = 100; // for larger resolutions

    // Lucien's scalar
//    int scalar = largestContour.size()/5;
//    if (scalar > 25)
//        scalar = 25;
//    else if (scalar < 5)
//        scalar = 5;

    // Rect roi = Rect((cX - scalar), (cY - scalar), (scalar*2), (scalar*2));
    int roi_x = (int) (cX - scalar);
    int roi_y = (int) (cY - scalar);
    int roi_width = (int) (scalar*2);
    int roi_height = (int) (scalar*2);

    if (cY <= scalar) {
        roi_y = 0;
        roi_height = (int) scalar + cY;
    }
    else if (cY >= (iMat.rows - scalar)) {
        roi_height = (int) scalar + (iMat.rows - (cY + 1));
    }

    if (cX <= scalar) {
        roi_x = 0;
        roi_width = (int) scalar + cX;
    }
    else if (cX >= (iMat.cols - scalar)) {
        roi_width = (int) scalar + (iMat.cols - (cX + 1));
    }

    if (roi_width < 0)
        roi_width = (int) scalar*2;
    if (roi_height < 0)
        roi_height = (int) scalar*2;

    p1 = Point2f(startX+roi_x, roi_y);
    p2 = Point2f(startX+roi_x+roi_width, roi_y+roi_height);

    Rect roi = Rect((roi_x), (roi_y), (roi_width), (roi_height));

//    cout << "ROI :" << roi_x << "," << roi_y << "," << roi_width << "," << roi_height << endl;
    // crop image
    Mat iCrop = Mat(iMat, roi);

    Mat result = iCrop;
    Scalar yellow(0,255,255);

    Mat DMat;
    cvtColor(bwMat,DMat,COLOR_GRAY2RGB);

    // cout << "Size LARGEST " << largestContour.size() << endl;
    // cout << "Size CONTOURS " << contours.size() << endl;

    vector<vector<Point> > draw = {largestContour};

    // drawContours(DMat,contours,-1,yellow,5);
    drawContours(DMat,draw,-1,yellow,5);

    if ((!DMat.empty()) && (DEBUG == 1)) {
        namedWindow("Drawn", WINDOW_GUI_NORMAL);
        imshow("Drawn",DMat);
    }


//    if ((!result.empty()) && (DEBUG == 1) && isComputing) {
//        namedWindow("ROI",WINDOW_GUI_NORMAL);
//        imshow("ROI",result);
//    }
    // cout << "SIZE RESULTS : " << result.size() << endl;

    if(isComputing){
        return result;
    }else{
        Mat empty;
        return empty;
    }
}

//Mat GetBoundingBoxByBgSub2(Mat img, int Colorspace, int DEBUG, Point2f & p1, Point2f & p2){

//    // Only deal with center part of each frame
////    int widthImg = img.size().width;
//    int heightImg = img.size().height;
////    int startX=widthImg/5,startY=0,width=(widthImg/5)*3,height=heightImg;
//    int startX=360, startY=0, width=1000-360, height=heightImg;

//    Mat ROI(img, Rect(startX,startY,width,height));
//    Mat croppedImage;
//    ROI.copyTo(croppedImage);
//    Mat iMat = croppedImage.clone();

//    Mat gsMat, bwMat;   // Grayscale image & binary image
////    cvtColor(iMat, gsMat, COLOR_RGB2GRAY);
//////    imshow("gsMat", gsMat);
////    threshold(gsMat, bwMat, 60, 255, THRESH_BINARY);        // Yinghan's threshold

//    Scalar lowBound(50,45,50); // (1,45,175)===== (0, 45, 70)
//    Scalar upperBound(180,255,255); // (180,255,255)
//    // Scalar lowBound(1,45,175); // (1,45,175)
//    // Scalar upperBound(180,255,255); // (180,255,255)
//    cvtColor(iMat, gsMat, COLOR_RGB2HSV);
//    inRange(gsMat, lowBound, upperBound, bwMat);
//    dilate(bwMat, bwMat, Mat());

//    imshow("bwMat" ,bwMat);

//    vector<vector<Point> > contours;
//    Mat hierarchy;
//    findContours(bwMat, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

//    vector<Point> largestContour;
//    double maxArea = 0;
//    for (vector<Point> contour : contours){
//        double area = contourArea(contour);
//        if (area > maxArea){
//            maxArea = area;
//            largestContour = contour;
//        }
//    }


//    // cout << "size contours : " << largestContour.size() << endl;

//    Moments M = moments(largestContour);

//    int cX = (int) (M.m10 / M.m00);
//    int cY = (int) (M.m01 / M.m00);

////    cout<<"center: "<<cX<<" "<<cY<<endl;

//    // Only when max area is larger than setting area & center of roi is in the center
//    // then we compute oxygen contents
//    if(maxArea>areaSpace && startX + cX>img.cols*para2.getLeftLine() && startX + cX<img.cols*para2.getRightLine()){
//        isComputing = true;
//    }else{
//        isComputing = false;
//    }

//    // old Scalars
//    // int scalar = 25; // for smaller resolutions
//    // int scalar = 100; // for larger resolutions

//    // Lucien's scalar
////    int scalar = largestContour.size()/5;
////    if (scalar > 25)
////        scalar = 25;
////    else if (scalar < 5)
////        scalar = 5;

//    // Rect roi = Rect((cX - scalar), (cY - scalar), (scalar*2), (scalar*2));
//    int roi_x = (int) (cX - scalar);
//    int roi_y = (int) (cY - scalar);
//    int roi_width = (int) (scalar*2);
//    int roi_height = (int) (scalar*2);

//    if (cY <= scalar) {
//        roi_y = 0;
//        roi_height = (int) scalar + cY;
//    }
//    else if (cY >= (iMat.rows - scalar)) {
//        roi_height = (int) scalar + (iMat.rows - (cY + 1));
//    }

//    if (cX <= scalar) {
//        roi_x = 0;
//        roi_width = (int) scalar + cX;
//    }
//    else if (cX >= (iMat.cols - scalar)) {
//        roi_width = (int) scalar + (iMat.cols - (cX + 1));
//    }

//    if (roi_width < 0)
//        roi_width = (int) scalar*2;
//    if (roi_height < 0)
//        roi_height = (int) scalar*2;

//    p1 = Point2f(startX+roi_x, roi_y);
//    p2 = Point2f(startX+roi_x+roi_width, roi_y+roi_height);

//    Rect roi = Rect((roi_x), (roi_y), (roi_width), (roi_height));

////    cout << "ROI :" << roi_x << "," << roi_y << "," << roi_width << "," << roi_height << endl;
//    // crop image
//    Mat iCrop = Mat(iMat, roi);

//    Mat result = iCrop;
//    Scalar yellow(0,255,255);

//    Mat DMat;
//    cvtColor(bwMat,DMat,COLOR_GRAY2RGB);

//    // cout << "Size LARGEST " << largestContour.size() << endl;
//    // cout << "Size CONTOURS " << contours.size() << endl;

//    vector<vector<Point> > draw = {largestContour};

//    // drawContours(DMat,contours,-1,yellow,5);
//    drawContours(DMat,draw,-1,yellow,5);

//    if ((!DMat.empty()) && (DEBUG == 1)) {
//        namedWindow("Drawn", WINDOW_GUI_NORMAL);
//        imshow("Drawn",DMat);
//    }


////    if ((!result.empty()) && (DEBUG == 1) && isComputing) {
////        namedWindow("ROI",WINDOW_GUI_NORMAL);
////        imshow("ROI",result);
////    }
//    // cout << "SIZE RESULTS : " << result.size() << endl;

//    if(isComputing){
//        return result;
//    }else{
//        Mat empty;
//        return empty;
//    }

//}

Mat getBoundingBox(Mat img, int ThresholdMethod, int Colorspace, int DEBUG) // String path
{
    // Mat iMat = imread(path, IMREAD_COLOR);


    // Only deal with center part of each frame
    int widthImg = img.size().width;
    int heightImg = img.size().height;
//    int startX=widthImg/5,startY=0,width=(widthImg/5)*3,height=heightImg;
    int startX=360, startY=0, width=1000-360, height=heightImg;

    Mat ROI(img, Rect(startX,startY,width,height));
    Mat croppedImage;
    ROI.copyTo(croppedImage);
    Mat iMat = croppedImage.clone();


//    Mat iMat = img.clone();




    // medianBlur(iMat,iMat,5); // Median filtering on the image, size 5
    Mat gsMat, bwMat;

    // resize(iMat,iMat,Size(),0.5,0.5,INTER_AREA);
    // resize(iMat,iMat,Size(),0.5,0.5,INTER_LINEAR);


    // int64 start, end;
    // double elapsed;
    // start = getTickCount(); // timer


    if (ThresholdMethod == 0) // 1st Threshold
    {
        cvtColor(iMat, gsMat, COLOR_RGB2GRAY);
//        threshold(gsMat, bwMat, 50, 255, THRESH_BINARY);      // Lucien's threshold
        threshold(gsMat, bwMat, 20, 255, THRESH_BINARY);        // Yinghan's threshold
    }
    else // 2nd Threshold
    {
        // new values for dataset3
        Scalar lowBound(0,45,70); // (1,45,175)
        Scalar upperBound(180,255,255); // (180,255,255)
        // Scalar lowBound(1,45,175); // (1,45,175)
        // Scalar upperBound(180,255,255); // (180,255,255)
        cvtColor(iMat, gsMat, COLOR_RGB2HSV);
        inRange(gsMat, lowBound, upperBound, bwMat);
        dilate(bwMat, bwMat, Mat());
    }

    // end = getTickCount();
    // elapsed = (end - start)/ (double) getTickFrequency();
    // cout << "THRESHOLD (s) : " << elapsed << endl;

    vector<vector<Point> > contours;
    Mat hierarchy;
    findContours(bwMat, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

    vector<Point> largestContour;
    double maxArea = 0;
    for (vector<Point> contour : contours){
        double area = contourArea(contour);
        if (area > maxArea){
            maxArea = area;
            largestContour = contour;
        }
    }

    // cout << "size contours : " << largestContour.size() << endl;

    Moments M = moments(largestContour);

    int cX = (int) (M.m10 / M.m00);
    int cY = (int) (M.m01 / M.m00);

    // old Scalars
    // int scalar = 25; // for smaller resolutions
    // int scalar = 100; // for larger resolutions

    // Lucien's scalar
//    int scalar = largestContour.size()/5;
//    if (scalar > 25)
//        scalar = 25;
//    else if (scalar < 5)
//        scalar = 5;

    // Yinghan's scalar
    int scalar = 75;

    // Rect roi = Rect((cX - scalar), (cY - scalar), (scalar*2), (scalar*2));
    int roi_x = (int) (cX - scalar);
    int roi_y = (int) (cY - scalar);
    int roi_width = (int) (scalar*2);
    int roi_height = (int) (scalar*2);

    if (cY <= scalar) {
        roi_y = 0;
        roi_height = (int) scalar + cY;
    }
    else if (cY >= (iMat.rows - scalar)) {
        roi_height = (int) scalar + (iMat.rows - (cY + 1));
    }

    if (cX <= scalar) {
        roi_x = 0;
        roi_width = (int) scalar + cX;
    }
    else if (cX >= (iMat.cols - scalar)) {
        roi_width = (int) scalar + (iMat.cols - (cX + 1));
    }

    if (roi_x < 0)
        roi_x = 0;
    if (roi_y < 0)
        roi_y = 0;
    if (roi_width < 0)
        roi_width = (int) scalar*2;
    if (roi_height < 0)
        roi_height = (int) scalar*2;

    Rect roi = Rect((roi_x), (roi_y), (roi_width), (roi_height));

//    cout << "ROI :" << roi_x << "," << roi_y << "," << roi_width << "," << roi_height << endl;
    // crop image
    Mat iCrop = Mat(iMat, roi);

    Mat result = iCrop;
    Scalar yellow(0,255,255);

    Mat DMat;
    cvtColor(bwMat,DMat,COLOR_GRAY2RGB);

    // cout << "Size LARGEST " << largestContour.size() << endl;
    // cout << "Size CONTOURS " << contours.size() << endl;

    vector<vector<Point> > draw = {largestContour};

    // drawContours(DMat,contours,-1,yellow,5);
    drawContours(DMat,draw,-1,yellow,5);

    if ((!DMat.empty()) && (DEBUG == 1)) {
        namedWindow("Drawn", WINDOW_GUI_NORMAL);
        imshow("Drawn",DMat);
    }
    if ((!result.empty()) && (DEBUG == 1)) {
        namedWindow("ROI",WINDOW_GUI_NORMAL);
        imshow("ROI",result);
    }
    // cout << "SIZE RESULTS : " << result.size() << endl;

    return result;
}


float avgHue(Mat img, int curveColorSpace)
{
    Mat roi = img.clone();

    // Convert BGR to RGB
    cvtColor(roi, roi, CV_BGR2RGB);

    if (roi.empty())
    {
        float average = 0.0f;
        return average;
    }

    float sumH = 0.0f;
    float maxH = 1.0f;
    float pixelHue;
    float offset;
    float h;
    int chan1 = 0;

    if (curveColorSpace == 0) {
        cvtColor(roi, roi, COLOR_RGB2HSV); // HSV conversion
        offset = 1.0;
    } else {
        cvtColor(roi, roi, COLOR_RGB2Lab); // L*a*b conversion
        offset = 100.0/255.0;
    }

    vector<Mat> hsv;
    split(roi,hsv);
    Mat hue;
    hsv[chan1].convertTo(hue, CV_32FC1); // 2nd channel (inverted)
    int size = hue.rows * hue.cols;

    vector<float> matArray; // 1D array for faster process
    Mat flatM;
    hue.reshape(1,1).convertTo(flatM, CV_32FC1);
    matArray.assign((float*)flatM.datastart, (float*)flatM.dataend); // matArray = flatM;

    if (curveColorSpace == 0) { // HSV
        for (int i = 0; i < (size); i++) {
            h = matArray[i];
            pixelHue = (h/180.0);
            pixelHue += (maxH/2);

            if (pixelHue > maxH) {
                pixelHue -= maxH;
            }
            sumH += pixelHue;
        }
    }
    else { // L*a*b
        sumH = (float) sum(hue)[chan1];
        // sumH = (double) mean(hue)[0];
    }

    float average = (float) ((sumH/(size)) * offset);
    // float average = sumH * offset;
    return average; // mean hue
}

float computeOxygen(float estimated, float parameters[]) {

    float y_given = estimated;
    float a = parameters[0];
    float b = parameters[1];
    float c = parameters[2];
    float d = parameters[3];

    const int N = 100;
    float x_estimated = 0; // computed oxygen concentration

    float y_experimental[N];
    for (int j = 1; j < (N + 1); j++) {
        y_experimental[j - 1] = (float) (a * exp(b * j) + c * exp(d * j));
    }

    float slope_experimental[N-1];
    for (int p = 0; p < (N-1); p++) {
        slope_experimental[p] = y_experimental[p + 1] - y_experimental[p];
    }

    // find edge values for curve
    float y_max_experimental = y_experimental[0];
    float y_min_experimental = y_experimental[0];
    for (int i = 1; i < N; i++) {
        if(y_experimental[i] > y_max_experimental) {
            y_max_experimental = y_experimental[i];
        } else if (y_experimental[i] < y_min_experimental) {
            y_min_experimental = y_experimental[i];
        }
    }

    if (y_given > y_max_experimental) {
        x_estimated = 0;
    } else if (y_given < y_min_experimental) {
        x_estimated = 100;
    } else {
        for (int i = 1; i < N; i++) {
            if (y_given < y_experimental[i-1] && y_given > y_experimental[i]) {
                x_estimated = i + ((y_given - y_experimental[i]) / slope_experimental[i-1]);
            }
        }
    }

    return x_estimated;
}



float findMedian(Mat img, int curveColorSpace)
{
    Mat roi = img.clone();

    if (roi.empty())
    {
        float medH = 0.0f;
        return medH;
    }

    double maxH = 1.0; // histogram maximum
    double medH; // image median
    double A,B; // values for f(x) = A*x + B to recover a more precise Median
    int indH; // index in array of median value
    double h;
    double pixelHue;
    int chan1 = 2;

    double offset;
    int nimages = 1;
    int dims = 1;
    int histSize; // histogram size
    const int channels = {0}; // values covered by the histogram
    float range[2]; // working with Lightness or Hue

    if (curveColorSpace == 0) {
        cvtColor(roi, roi, COLOR_RGB2HSV); // HSV conversion
        histSize = {256};
        offset = 1.0/255.0;
        range[0] = {0.0f};
        range[1] = {1.0f};
    } else {
        cvtColor(roi, roi, COLOR_RGB2Lab); // L*a*b conversion
        histSize = {256}; // 256 for Lightness
        offset = 100.0/255.0; // offset to shift back to standard L (8-bit images)
        range[0] = {0.0f};
        range[1] = {256.0f};
    }
    float ranges[] = { range[0], range[1] };
    const float* histRange = { ranges };

    Mat hsv[3];
    split(roi,hsv);
    Mat hue;
    // hsv[chan1].convertTo(hue, CV_32FC1);
    hsv[chan1].convertTo(hue, CV_32FC1);
    int rows = hue.rows;
    int cols = hue.cols;
    int size = rows * cols;

    vector<float> matArray; // 1D vector for faster Hue shifting
    Mat flatM;
    // hue.reshape(1,1).convertTo(flatM, CV_32FC1); // reshape hue to 1D array flatM
    hue.reshape(1,1).convertTo(flatM, CV_32FC1); // reshape hue to 1D array flatM
    matArray.assign((float*)flatM.datastart, (float*)flatM.dataend); // matArray = flatM;

    if (curveColorSpace == 0) { // HSV
        for (int i = 0; i < (size); i++) { // Hue Shifting
            h = matArray[i];
            pixelHue = (h/180.0);
            pixelHue += (maxH/2);

            if (pixelHue > maxH) {
                pixelHue -= maxH;
            }
            matArray[i] = pixelHue;
        }
    }
    memcpy(flatM.data,matArray.data(),matArray.size()*sizeof(float)); // export 1D vector to 1D Mat

    // flatM.convertTo(hue, CV_32FC1); // convert back into hue matrix
    // hue.reshape(1,cols).convertTo(hue, CV_32FC1); // reshape to 2D image

    flatM.convertTo(hue, CV_32FC1); // convert back into hue matrix
    hue.reshape(1,cols).convertTo(hue, CV_32FC1); // reshape to 2D image


    Mat hist;

    // cout << hue.size() << endl;

    // cout << hue << endl; // current test debug output

    calcHist(&hue, nimages, &channels, Mat(), hist, dims, &histSize, &histRange, true, false); // Histogram calculation

    // cout << hist << endl;

    vector<float> hist2;
    // hist.reshape(1,1).convertTo(flatM, CV_32FC1);
    // hist2 = hist; // import hist to vector<float> to sum the histogram
    // hist2.assign((float*)hist.datastart, (float*)hist.dataend);
    hist.col(0).copyTo(hist2);

    for (int i = 1; i <= histSize - 1; i++) {
        hist2[i] += hist2[i - 1]; // sum of histogram
    }
    maxH = hist2[histSize - 1]/2.0; // 1.5 to get 3rd Quartile, 2 is Median

    int top = histSize - 1;
    int bottom = 0;
    int mid = round((top + bottom) / 2);
    while (round(top - bottom) > 1) { // binary - dichotomic search
        if (hist2[mid] >= maxH) {
            top = mid; // lower area
        } else {
            bottom = mid; // higher area
        }
        mid = round((top + bottom) / 2);
    }
    indH = mid; // recover index value of Median in array

    A = ( hist2[indH + 1] - hist2[indH]);
    B = ( hist2[indH] - A*indH);
    medH = ((maxH - B) / A); // linear reconstruction

    return ((float) (medH * offset)); // final Median value
}

float* getParameters(int colorspace, int method, int sensor) {

    static float parameters[4];

    if (colorspace == 0) {
        if (method == 0) {
            if(sensor == 0){
                // Parameters for sensor MT2_L-3
                parameters[0] = {0.512f};
                parameters[1] = {-0.00709f};
                parameters[2] = {-0.06009f};
                parameters[3] = {-0.05214f};
            }else{
                // Old sensor parameters
                parameters[0] = {0.05411f};
                parameters[1] = {-0.3188f};
                parameters[2] = {0.4821f};
                parameters[3] = {-0.00257f};
            }

        }
        else {
            parameters[0] = {0.05299f};
            parameters[1] = {-0.349f};
            parameters[2] = {0.4278f};
            parameters[3] = {-0.002489f};
        }
    }
    else {
        if (method == 0) {
            parameters[0] = {55.72f};
            parameters[1] = {-0.005797f};
            parameters[2] = {0.004039f};
            parameters[3] = {0.06492f};
        }
        else {
            parameters[0] = {55.59f};
            parameters[1] = {-0.006038f};
            parameters[2] = {0.0009476f};
            parameters[3] = {0.07844f};
        }
    }

    return (float*) parameters;
}

Mat visuColorspace(Mat img, int Colorspace, int ThresholdMethod, int DEBUG)
{
    Mat iMat = img.clone();
    Mat gsMat, bwMat;

    if (ThresholdMethod == 0) // 1st Threshold
    {
        cvtColor(iMat, gsMat, COLOR_RGB2GRAY);
        threshold(gsMat, bwMat, 50, 255, THRESH_BINARY);
    }
    else // 2nd Threshold
    {
        Scalar lowBound(1,45,175); // (1,45,175)
        Scalar upperBound(180,255,255); // (180,255,255)
        cvtColor(iMat, gsMat, COLOR_RGB2HSV);
        inRange(gsMat, lowBound, upperBound, bwMat);
        dilate(bwMat, bwMat, Mat());
    }

    if ((!gsMat.empty()) && (DEBUG == 1)) {
        namedWindow("Converted Image",WINDOW_GUI_NORMAL);
        imshow("Converted Image", gsMat);
    }

    vector<Mat> LCh;
    if (Colorspace == 0)
        cvtColor(iMat, gsMat, COLOR_RGB2HSV);
    else if (Colorspace == 2) {
        cvtColor(iMat,gsMat, COLOR_RGB2Lab);

        vector<Mat> Lab;
        // vector<Mat> Lab, LCh;
        gsMat.convertTo(gsMat,CV_32FC3);
        split(gsMat,Lab);
        float offset = (100.0f/255.0f);
        LCh[0] = Lab[0].clone()*(offset);

        cartToPolar(Lab[1], Lab[2], LCh[1], LCh[2], false);

        float h;
        for (int i = 0; i < (gsMat.rows-1); i++) {
            for (int j = 0; j < (gsMat.cols-1); j++) {
                h = (LCh[2]).at<float>(i,j);
                if (h > 0)
                    h = (h/M_PI) * 180.0f;
                else
                    h = 360.0f - (abs(h) / M_PI) * 180.0f;

                if (h < 0)
                    h += 360.0f;
                else if (h >= 360)
                    h -= 360.0f;

                LCh[2].at<float>(i,j) = h;
            }
        }
        // cout << LCh[0].cols << "x" << LCh[0].rows << " ; " << LCh[1].cols << "x" << LCh[1].rows << " ; " << LCh[2].cols << "x" << LCh[2].rows << endl;

        // gsMat = LCh.data()->clone();
        // LCh.data()->copyTo(gsMat);

        // merge(LCh, gsMat);
        // merge(LCh.channels(), gsMat);
    }
    else if (Colorspace == 1)
    {
        cvtColor(iMat, gsMat, COLOR_RGB2Lab);
        // vector<Mat> Lab;
        // split(gsMat, Lab);
        // float offset = (100.0f/255.0f);
        // Lab[0] = Lab[0]*offset;
        // Lab[1] -= 128;
        // Lab[2] -= 128;
        // merge(Lab, gsMat);
    }

    vector<Mat> CMat;
    split(gsMat, CMat);

    if (Colorspace == 0)
    {
        if ((DEBUG == 1)) {
            namedWindow("Output H", WINDOW_GUI_NORMAL);
            imshow("Output H", CMat[0]);
            namedWindow("Output S", WINDOW_GUI_NORMAL);
            imshow("Output S", CMat[1]);
            namedWindow("Output V", WINDOW_GUI_NORMAL);
            imshow("Output V", CMat[2]);
        }
    }
    else if (Colorspace == 2)
    {
        if ((DEBUG == 1)) {
        namedWindow("Output L", WINDOW_GUI_NORMAL);
        imshow("Output L", CMat[0]);
        namedWindow("Output C", WINDOW_GUI_NORMAL);
        imshow("Output C", CMat[1]);
        namedWindow("Output h", WINDOW_GUI_NORMAL);
        imshow("Output h", CMat[2]);
        }
    }
    else
    {
        if ((DEBUG == 1)) {
        namedWindow("Output L", WINDOW_GUI_NORMAL);
        imshow("Output L", CMat[0]);
        namedWindow("Output a", WINDOW_GUI_NORMAL);
        imshow("Output a", CMat[1]);
        namedWindow("Output b", WINDOW_GUI_NORMAL);
        imshow("Output b", CMat[2]);
        }
    }

    return gsMat;
}

void gradientProcess(Mat img, int Colorspace, int DEBUG)
{
    Mat iMat = img.clone();
    Mat tmp, img2;
    vector<Mat> CMat;
    split(iMat,CMat);

    int low, high;
    if (Colorspace == 1)
    {
        // bilateralFilter(CMat[0], img2, 9, 300.0, 300.0, BORDER_DEFAULT); // 9, 300.0, 300.0 - 0
        low = 45;
        high = 2*low;
    }
    if (Colorspace == 0)
    {
        // bilateralFilter(CMat[2], img2, 9, 300.0, 300.0, BORDER_DEFAULT); // 9, 300.0, 300.0 - 2
        low = 25;
        high = 2*low;
    }

    img2 = iMat.clone();
    Scalar lowBound(0,45,70); // (1,45,175)
    Scalar upperBound(180,255,255); // (180,255,255)
    // cvtColor(img2, img2, COLOR_RGB2HSV);
    inRange(img2, lowBound, upperBound, img2);
    // dilate(img2, img2, Mat());

    int hmsize = 5;
    int hmmid = floor(hmsize/2);
    Mat_<int> hm_cross(hmsize, hmsize, CV_8U);
    hm_cross = hm_cross - 255;
    for (int i=0; i < hmsize; i++) {
        hm_cross(hmmid,i) = 255;
        hm_cross(i,hmmid) = 255;
    }
    Mat_<int> hm_hv(hmsize, hmsize, CV_8U); // horizontal/vertical structuring element
    hm_hv =  hm_hv - 255;
    for (int i=0; i < hmsize; i++) {
        hm_hv(i,hmmid) = 255;
    }
    Mat_<int> hm_diag(hmsize, hmsize, CV_8U); // diagonal structuring element
    hm_diag = hm_diag - 255;
    for (int i=0; i < hmsize; i++) {
        hm_diag(i,i) = 255;
    }
    bitwise_not(hm_hv,hm_hv);
    bitwise_not(hm_diag,hm_diag);
    bitwise_not(hm_cross,hm_cross);
    hm_hv += 1;
    hm_diag += 1;
    hm_cross += 1;

    // cout << " horizontal :" << endl << hm_hv << endl;
    // cout << " diagonal :" << endl << hm_diag << endl;
    // cout << " cross :" << endl << hm_cross << endl;

    Mat CanMatRes, CanMatRes2, CanMatRes3;
    Mat SobMat, SchMat, CanMat, CanMatHV, CanMatDIAG;

    if (Colorspace == 1) // L*a*b
        Canny(img2, CanMat, low, high, 3, true); // 25, 50, 3
    else // HSV
        Canny(img2, CanMat, low, high, 3, true); // 25, 50, 7 (used to be 3)

    Sobel(img2, SobMat, -1, 1, 1, 3, 1, 0, BORDER_REPLICATE);
    // Scharr(img2, SchMat, -1, 1, 1, 1, 0, BORDER_DEFAULT);

    morphologyEx(CanMat, CanMatHV, MORPH_HITMISS, hm_hv, Point(-1,-1), 1, BORDER_CONSTANT);
    morphologyEx(CanMat, CanMatDIAG, MORPH_HITMISS, hm_diag, Point(-1,-1), 1, BORDER_CONSTANT);

    /*
    Mat linH = getStructuringElement(MORPH_RECT,Size(3, 1),Point(-1,-1));
    Mat linV = getStructuringElement(MORPH_RECT,Size(1, 3),Point(-1,-1));
    linH = linH*255;
    linV = linV*255;
    cout << linH << endl;
    cout << linV << endl;
    morphologyEx(CanMat, CanMatRes2, MORPH_ERODE, linH, Point(-1,-1), 1, BORDER_CONSTANT);
    morphologyEx(CanMat, CanMatRes3, MORPH_ERODE, linV, Point(-1,-1), 1, BORDER_CONSTANT);
    bitwise_and(CanMat, CanMatRes2, CanMatRes2);
    bitwise_and(CanMat, CanMatRes3, CanMatRes3);
    bitwise_or(CanMatRes2, CanMatRes3, CanMatRes);
    */

    morphologyEx(CanMatHV, CanMatRes2, MORPH_DILATE, hm_hv, Point(-1,-1), 1, BORDER_CONSTANT);
    morphologyEx(CanMatDIAG, CanMatRes3, MORPH_DILATE, hm_diag, Point(-1,-1), 1, BORDER_CONSTANT);

    // bitwise_and(CanMatHV, CanMatV, CanMatRes2);
    // bitwise_and(CanMatHV, CanMatH, CanMatRes3);
    bitwise_or(CanMatRes2, CanMatRes3, CanMatRes);

    if ((!CanMatDIAG.empty()) && (DEBUG == 1)) {
        namedWindow("CanMatDIAG", WINDOW_GUI_NORMAL);
        imshow("CanMatDIAG", CanMatDIAG);
    }
    if ((!CanMatHV.empty()) && (DEBUG == 1)) {
        namedWindow("CanMatHV", WINDOW_GUI_NORMAL);
        imshow("CanMatHV", CanMatHV);
    }
    if ((!CanMatRes.empty()) && (DEBUG == 1)) {
        namedWindow("CanMatRes", WINDOW_GUI_NORMAL);
        imshow("CanMatRes", CanMatRes);
    }
    if ((!CanMatRes2.empty()) && (DEBUG == 1)) {
        namedWindow("CanMatRes2", WINDOW_GUI_NORMAL);
        imshow("CanMatRes2", CanMatRes2);
    }
    if ((!CanMatRes3.empty()) && (DEBUG == 1)) {
        namedWindow("CanMatRes3", WINDOW_GUI_NORMAL);
        imshow("CanMatRes3", CanMatRes3);
    }

    morphologyEx(img2, SchMat, MORPH_GRADIENT, hm_hv, Point(-1,-1), 1, BORDER_CONSTANT);
    Canny(SchMat, SchMat, 150, 255, 5, true); // 150 - 200 - 5

    if ((!img2.empty()) && (DEBUG == 1)) {
        namedWindow("bilateral", WINDOW_GUI_NORMAL);
        imshow("bilateral", img2);
    }

    // Mat LMat;
    // cvtColor(gsMat, LMat, CV_RGB2GRAY);

    Mat linesH, linesHP, linesC;

    // HoughLinesP(CanMat, linesHP, 1, M_PI/360, 100, 250, 30);

    // cout << "IMAGE SIZE : " << " COLS : " << iMat.cols << " ROWS : " << iMat.rows << endl; // 1920roi_x080

    /*
    IplImage *Himg = img2;
    CvMemStorage* linesH2 = cvCreateMemStorage(0);


    cvHoughLines2(*Himg, linesH2, HOUGH_STANDARD, 1, M_PI/360, 100);
    // cvHoughLines2(img2, linesH2, HOUGH_PROBABILISTIC, 1, M_PI/360, 100);
    // cvHoughLines2(img2, linesH2, HOUGH_MULTI_SCALE, 1, M_PI/360, 100, 1, 1);
    // cvHoughLines2(img2, linesH2, HOUGH_GRADIENT, 1, M_PI/360, 100);
    */
    /*
    Point seedpt = Point(floor(CanMatRes.cols/2), floor(CanMatRes.rows/2));

    Mat mask;
    // Canny(CanMatRes,mask,100,200);
    mask = CanMatRes.clone();

    copyMakeBorder(mask,mask,1,1,1,1,BORDER_REPLICATE);
    Mat cvdisk = getStructuringElement(MORPH_ELLIPSE,Size(2*hmsize, 2*hmsize), Point(-1,-1));
    // erode(mask, mask, cvdisk);
    // dilate(mask, mask, cvdisk);
    floodFill(CanMatRes, mask, seedpt, cv::Scalar(255), 0, cv::Scalar(), cv::Scalar(), FLOODFILL_FIXED_RANGE);

    cvdisk = getStructuringElement(MORPH_ELLIPSE,Size(5*hmsize, 5*hmsize), Point(-1,-1));
    morphologyEx(CanMatRes, CanMatRes,MORPH_OPEN,cvdisk,Point(-1,-1), 1,BORDER_CONSTANT);
    morphologyEx(CanMatRes, CanMatRes,MORPH_CLOSE,cvdisk,Point(-1,-1), 1,BORDER_CONSTANT);

    namedWindow("mask", WINDOW_GUI_NORMAL);
    imshow("mask", mask);
    */

    vector<vector<Point>> contours;
    Mat hierarchy;
    findContours(CanMatRes, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

    if ((!CanMatRes.empty()) && (DEBUG == 1)) {
    imshow("CantMatRes", CanMatRes);
    }
    // cout << mask << endl;

    HoughLinesP(CanMat, linesHP, 1, M_PI/360, 100, iMat.rows/5, iMat.rows/10); // 100, 250, 120
    // /8 /20


    //linesHP.reshape(linesHP.rows*2,2).copyTo(linesHP);
    //cout << linesHP << endl;
    // intersectConvexConvex()
    Mat hull1;

    // convexHull(linesHP, hull1, false, false);

    vector<Point> largestContour;
    double maxArea = 0;
    for (vector<Point> contour : contours){
        double area = contourArea(contour);
        if (area > maxArea){
            maxArea = area;
            largestContour = contour;
        }
    }

    Scalar yellow(0, 255, 255);

    // cout << "Size LARGEST " << largestContour.size() << endl;
    // cout << "Size CONTOURS " << contours.size() << endl;

    // HoughLines(CanMat, linesH, 1, M_PI/180, 100, 1, 1, 0, M_PI);

    // cout << linesH << endl;
    // cout << "rows H : " << linesH.rows << " cols H : " << linesH.cols << endl;
    // cout << linesHP << endl;
    // cout << "rows HP : " << linesHP.rows << " cols HP : " << linesHP.cols << endl;

    Mat linesMat;
    if (Colorspace == 1)
        cvtColor(iMat, linesMat, COLOR_Lab2RGB);
    else
        cvtColor(iMat, linesMat, COLOR_HSV2RGB);

    for ( int i = 0; i < linesHP.rows; i++ )
    {
        line(linesMat, Point(linesHP.at<int>(i,0), linesHP.at<int>(i,1)), Point(linesHP.at<int>(i,2), linesHP.at<int>(i,3)), yellow, 3, 8);
    }
    /*
    for ( int i = 0; i < linesH.rows; i++ )
    {
        line(linesMat, Point(linesH2.at<int>(i,0), linesH2.at<int>(i,1)), Point(linesH2.at<int>(i,2), linesH2.at<int>(i,3)), yellow, 3, 8);
    }
    */
    /*
    for ( int i = 0; i < linesH.rows; i++ )
    {
        line(linesMat, Point(linesH.at<int>(i,0), linesH.at<int>(i,1)), Point(linesH.at<int>(i,2), linesH.at<int>(i,3)), yellow, 3, 8);
    }
    */

    Mat DMat = linesMat.clone();
    vector<vector<Point> > draw = {largestContour};
    // drawContours(DMat,contours,-1,yellow,5);
    drawContours(DMat, draw, -1, yellow, 5);

    if ((!DMat.empty()) && (DEBUG == 1)) {
        namedWindow("Drawn2", WINDOW_GUI_NORMAL);
        imshow("Drawn2",DMat);
    }

    if ((!linesMat.empty()) && (DEBUG == 1)) {
        namedWindow("HoughLinesP", WINDOW_GUI_NORMAL);
        imshow("HoughLinesP", linesMat);
    }
    if ((!CanMat.empty()) && (DEBUG == 1)) {
        namedWindow("Canny", WINDOW_GUI_NORMAL);
        imshow("Canny", CanMat);
    }
    if ((!SobMat.empty()) && (DEBUG == 1)) {
        namedWindow("Sobel", WINDOW_GUI_NORMAL);
        imshow("Sobel", SobMat);
    }
    if ((!SchMat.empty()) && (DEBUG == 1)) {
        namedWindow("Scharr", WINDOW_GUI_NORMAL);
        imshow("Scharr", SchMat);
    }
}

string Formate(float O2){
    int x = O2*10;
    string result;
    if(x==1000){
        return "100";
    }else if(x<10){
        return "0."+to_string(x);
    }else{
        return to_string(x/10)+"."+to_string(x%10);
    }
}










