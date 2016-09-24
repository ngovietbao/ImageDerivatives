#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

void ImageHistogramGray(Mat img);
Mat ImageCorrelation(Mat img, Mat matH);
Mat CreateParodayMartrix(Mat input, int rows_h, int cols_h);
Mat CrossCorrelation(Mat input, Mat h, double &max_out, double &min_out);
Mat CrossConvolution(Mat input, Mat h, double &max_out, double &min_out);
Mat Zero_crossing(Mat input, Mat h, int threshold);

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }
    //------Read and Display Image-------
    Mat image;
    int rows, cols;
    image = imread( argv[1], CV_LOAD_IMAGE_COLOR );
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }
    namedWindow("Display Output Image", WINDOW_AUTOSIZE );
    imshow("Display Output Image", image);

    //----------------------------------
    cout << endl << "Please choose one of these options: " << endl;
    cout << "1. Display histogram" << endl;
    cout << "2. Box Filtering" << endl;
    //cout << "3. Sharpening"
    rows = image.rows;
    cols = image.cols;
    //-----Test CrossCorrelation-----
    Mat grayImg, grayImg_d, h, l, result,result_s, zero_cross;
    double data_bf[3][3] = {{1,1,1},{1,1,1},{1,1,1}};
    double data_h[3][3] = {{-1,2,-1},{-1,2,-1},{-1,2,-1}};
    double data_l[5][5] = {
                          {0,0,-1,0,0},
                          {0,-1,-2,-1,0},
                          {-1,-2,16,-2,-1},
                          {0,-1,-2,-1,0},
                          {0,0,-1,0,0}};
    cvtColor(image, grayImg, CV_BGR2GRAY);
    //cover grayImg to CV_64F (double) before filtering
    grayImg.convertTo(grayImg_d, CV_64F);
    h = Mat(3, 3, CV_64F, data_h);
    l = Mat(5, 5, CV_64F, data_l);
    //Result from CrossCorrelation 2 matrix grayImg and h (BoxFiltering)
    double max = 0, min = 0, threshold = 0;
    //result = CrossCorrelation(grayImg_d, l, max, min);
    zero_cross = Zero_crossing(grayImg_d, l, threshold);
    //cout << zero_cross;
    //result = CrossConvolution(grayImg_d, h, max);
    zero_cross.convertTo(result_s, CV_8UC1);
    //-----histogram-----
    //ImageHistogramGray(image);


    namedWindow("Display Input Image", WINDOW_AUTOSIZE );
    imshow("Display Input Image", result_s);

    waitKey(0);

    return 0;
}
Mat CreateParodayMartrix(Mat input, int rows_h, int cols_h)
{
  int rows_i = input.rows;
  int cols_i = input.cols;
  Mat parody = Mat::zeros(rows_i + 2*rows_h - 2, cols_i + 2*rows_h - 2 , CV_64F);
  for (int i = 0; i < rows_i; i++)
    for (int j = 0; j < cols_i; j++)
      parody.at<double>(i + rows_h - 1,j + cols_h - 1) = input.at<double>(i,j);

  //cout << "\nMatrix parody: \n" << parody << endl;
  return parody;
}

Mat CrossCorrelation(Mat input, Mat h, double &max_out, double &min_out)
{
  int rows_i = input.rows;
  int cols_i = input.cols;
  int rows_h = h.rows;
  int cols_h = h.cols;
  Mat result = Mat::zeros(rows_i + rows_h - 1, cols_i + cols_h - 1, CV_64F);
  Mat parody_I = CreateParodayMartrix(input, rows_h, cols_h);

  for (int i = 0; i < result.rows; i++)
    for (int j = 0; j < result.cols; j++)
    {
      int sum = 0;
      //Sum of multiplications from operands in matrix parody_I and h
      for (int x = 0; x < rows_h; x++)
        for (int y = 0; y < cols_h; y++)
          sum += h.at<double>(x,y)*parody_I.at<double>(i+x,j+y);
      result.at<double>(i,j) = sum;
      //Store max value for calculating pixel colors
      if (sum > max_out)
        max_out = sum;
      if (sum < min_out)
        min_out = sum;
    }
  //Covert matrix to pixel colors
  for (int i = 0; i < result.rows; i++)
    for (int j = 0; j < result.cols; j++)
      result.at<double>(i,j) = int((result.at<double>(i,j) - min_out)/ (max_out - min_out) * 255);
  return result;
}
Mat CrossConvolution(Mat input, Mat h, double &max_out, double &min_out)
{
  int rows_i = input.rows;
  int cols_i = input.cols;
  int rows_h = h.rows;
  int cols_h = h.cols;
  Mat result = Mat::zeros(rows_i + rows_h - 2, cols_i + cols_h - 2, CV_64F);
  Mat parody_I = CreateParodayMartrix(input, rows_h, cols_h);

  for (int i = 0; i < result.rows; i++)
    for (int j = 0; j < result.cols; j++)
    {
      int sum = 0;
      //Sum of multiplications from operands in matrix parody_I and h
      for (int x = 0; x < rows_h; x++)
        for (int y = 0; y < cols_h; y++)
          sum += h.at<double>(x,y)*parody_I.at<double>(i,j);
      result.at<double>(i,j) = sum;
      //Store max value for calculating pixel colors
      if (sum > max_out)
        max_out = sum;
      if (sum < min_out)
        min_out = sum;
    }
  //Covert matrix to pixel colors
  for (int i = 0; i < result.rows; i++)
    for (int j = 0; j < result.cols; j++)
      result.at<double>(i,j) = int((result.at<double>(i,j) - min_out) / max_out * 255);
  return result;

}
void ImageHistogramGray(Mat img)
{
  int histSize = 256;
  float range[] = {0, 256};
  const float* histRange = {range};
  Mat grayImg, histOut;

  cvtColor(img, grayImg, CV_BGR2GRAY);  //Convert image input into grayscale
  calcHist(&grayImg, 1, 0, Mat(), histOut, 1, &histSize, &histRange, true, false); // Confingure histogram for grayscale

  //Create an image to display histogram
  int hist_w = 512; int hist_h = 400;
  int bin_w = cvRound((double) hist_w/histSize);
  Mat histImage(hist_w, hist_h, CV_8U, Scalar(0,0,0));
  //Normalize the histogram
  normalize(histOut, histOut, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
  //Draw grayscale
  for(int i = 1; i < histSize; i++)
  {
    line(histImage, Point( bin_w*(i-1), hist_h - cvRound(histOut.at<float>(i-1)) ) ,
                     Point( bin_w*(i), hist_h - cvRound(histOut.at<float>(i)) ),
                     Scalar( 255, 0, 0), 2, 8, 0  );
  }
  //Display histogram through Window
  namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE );
  imshow("calcHist Demo", histImage );
}

Mat Zero_crossing(Mat input, Mat h, int threshold){
  int rows_i = input.rows;
  int cols_i = input.cols;
  int rows_h = h.rows;
  int cols_h = h.cols;
  Mat result = Mat::zeros(rows_i + rows_h - 1, cols_i + cols_h - 1, CV_64F);
  Mat result_r = Mat::zeros(rows_i + rows_h - 1, cols_i + cols_h - 1, CV_64F);
  int min_out = 0, max_out = 0;
  Mat parody_I = CreateParodayMartrix(input, rows_h, cols_h);

  for (int i = 0; i < result.rows; i++)
    for (int j = 0; j < result.cols; j++)
    {
      int sum = 0;
      //Sum of multiplications from operands in matrix parody_I and h
      for (int x = 0; x < rows_h; x++)
        for (int y = 0; y < cols_h; y++)
          sum += h.at<double>(x,y)*parody_I.at<double>(i+x,j+y);
      result.at<double>(i,j) = sum;

      if (sum > max_out)
        max_out = sum;
      if (sum < min_out)
        min_out = sum;
    }
  //Covert matrix
  for (int i = 0; i < result.rows; i++)
    for (int j = 0; j < result.cols; j++)
      if (result.at<double>(i,j) < 0 && abs(result.at<double>(i,j)) > 0.04*max_out)
        result.at<double>(i,j) = -1;
      else if (result.at<double>(i,j) > 0 && abs(result.at<double>(i,j)) > 0.04*max_out)
        result.at<double>(i,j) = 1;
      else
        result.at<double>(i,j) = 0;
  //Apply mask 3x3 to detect edge point
  for (int i = 1; i < result.rows - 1; i++)
    for (int j = 1; j < result.cols - 1; j++)
    {
      double nw = result.at<double>(i-1,j-1);
      double n = result.at<double>(i-1,j);
      double ne = result.at<double>(i-1,j+1);
      double w = result.at<double>(i,j-1);
      double e = result.at<double>(i,j+1);
      double sw = result.at<double>(i+1,j-1);
      double s = result.at<double>(i+1,j);
      double se = result.at<double>(i+1,j+1);
      if (w*e < 0 || n*s < 0 || nw*se < 0 || sw*ne < 0)
        result_r.at<double>(i,j) = 255;
      else
        result_r.at<double>(i,j) = 0;
    }
  return result_r;
}
