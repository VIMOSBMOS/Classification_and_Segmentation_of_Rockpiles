//----------------------------------------------------------------------
/*!\file    projects/rock_classification/Algorithm2/mWatershedAlgorithmImplementationA2.cpp
 *
 * \author  Vimstan Moses
 *
 * \date    2019-07-08
 *
 */
//----------------------------------------------------------------------
#include "projects/rock_classification/Algorithm2/mWatershedAlgorithmImplementationA2.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace rock_classification
{
namespace Algorithm2
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------
runtime_construction::tStandardCreateModuleAction<mWatershedAlgorithmImplementationA2> cCREATE_ACTION_FOR_M_WATERSHEDALGORITHMIMPLEMENTATIONA2("WatershedAlgorithmImplementationA2");

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// mWatershedAlgorithmImplementationA2 constructor
//----------------------------------------------------------------------
mWatershedAlgorithmImplementationA2::mWatershedAlgorithmImplementationA2(core::tFrameworkElement *parent, const std::string &name) :
  tModule(parent, name, false) // change to 'true' to make module's ports shared (so that ports in other processes can connect to its output and/or input ports)
{}

//----------------------------------------------------------------------
// mWatershedAlgorithmImplementationA2 destructor
//----------------------------------------------------------------------
mWatershedAlgorithmImplementationA2::~mWatershedAlgorithmImplementationA2()
{}

//----------------------------------------------------------------------
// mWatershedAlgorithmImplementationA2 OnStaticParameterChange
//----------------------------------------------------------------------
void mWatershedAlgorithmImplementationA2::OnStaticParameterChange()
{
}

//----------------------------------------------------------------------
// mWatershedAlgorithmImplementationA2 OnParameterChange
//----------------------------------------------------------------------
void mWatershedAlgorithmImplementationA2::OnParameterChange()
{
}

//----------------------------------------------------------------------
// mWatershedAlgorithmImplementationA2 Update
//----------------------------------------------------------------------
void mWatershedAlgorithmImplementationA2::Update()
{
  if (this->in_distance_transform_threshold_image.IsConnected() && this->in_distance_transform_threshold_image.HasChanged())
  {
    data_ports::tPortDataPointer<const rrlib::coviroa::tImage> in_dis_transform_threshold_img_ptr = this->in_distance_transform_threshold_image.GetPointer();

    data_ports::tPortDataPointer<const rrlib::coviroa::tImage> in_sharpened_img_ptr = this->in_sharpened_image.GetPointer();

    data_ports::tPortDataPointer<rrlib::coviroa::tImage> out_markers_ptr = this->out_markers_image.GetUnusedBuffer();

    data_ports::tPortDataPointer<rrlib::coviroa::tImage> out_watershed_ptr = this->out_watershed_image.GetUnusedBuffer();

    if (out_markers_ptr->GetImageSize() != in_dis_transform_threshold_img_ptr->GetImageSize())
    {
      out_markers_ptr->Clear();
      for (size_t i = 0; i < in_dis_transform_threshold_img_ptr ->GetImageSize(); ++i)
      {
        out_markers_ptr->Resize(in_dis_transform_threshold_img_ptr ->GetWidth(), in_dis_transform_threshold_img_ptr ->GetHeight(), rrlib::coviroa::eIMAGE_FORMAT_MONO8, 0);
      }
    }


    if (out_watershed_ptr->GetImageSize() != in_dis_transform_threshold_img_ptr ->GetImageSize())
    {
      out_watershed_ptr->Clear();
      for (size_t i = 0; i < in_dis_transform_threshold_img_ptr ->GetImageSize(); ++i)
      {
        out_watershed_ptr->Resize(in_dis_transform_threshold_img_ptr ->GetWidth(), in_dis_transform_threshold_img_ptr ->GetHeight(), rrlib::coviroa::eIMAGE_FORMAT_MONO8, 0);
      }
    }

    this->ProcessImage(*in_dis_transform_threshold_img_ptr , *in_sharpened_img_ptr, *out_markers_ptr, *out_watershed_ptr);
  }
}

bool mWatershedAlgorithmImplementationA2::ProcessImage(const rrlib::coviroa::tImage& in_dis_transform_threshold_img_ptr, const rrlib::coviroa::tImage& in_sharpened_img_ptr, rrlib::coviroa::tImage& out_markers_ptr, rrlib::coviroa::tImage& out_watershed_ptr)
{
  cv::Mat out_markers_img = rrlib::coviroa::AccessImageAsMat(out_markers_ptr);
  cv::Mat out_watershed_img = rrlib::coviroa::AccessImageAsMat(out_watershed_ptr);
  cv::Mat in_dis_transform_threshold_img  = rrlib::coviroa::AccessImageAsMat(in_dis_transform_threshold_img_ptr);
  cv::Mat in_sharpened_img  = rrlib::coviroa::AccessImageAsMat(in_sharpened_img_ptr);
  ProcessImage(in_dis_transform_threshold_img, in_sharpened_img, out_markers_img, out_watershed_img);
  this->out_markers_image.Publish(out_markers_ptr);
  this->out_watershed_image.Publish(out_watershed_ptr);

  return true;

}

bool mWatershedAlgorithmImplementationA2::ProcessImage(const cv::Mat& in_distance_transform_threshold_image, const cv::Mat& in_sharpened_image, cv::Mat& out_markers_image, cv::Mat& out_watershed_image)
{
  cv::Mat input_image = in_distance_transform_threshold_image;
  if (input_image.type() == CV_8UC3)
  {
    cv::cvtColor(in_distance_transform_threshold_image, input_image, CV_RGB2GRAY);
  }

  cv::Mat peaks_8u;
  input_image.convertTo(peaks_8u, CV_8U);

  // Find total markers
  //Each contour is stored as a vector of points (e.g.std::vector<std::vector<cv::Point> >).
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(peaks_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

  // Create the marker image for the watershed algorithm
  // CV_32S - 32-bit signed integers ( -2147483648..2147483647 )
  cv::Mat markers = cv::Mat::zeros(input_image.size(), CV_32S);

  // Draw the foreground markers
  for (size_t i = 0; i < contours.size(); i++)
  {
    cv::drawContours(markers, contours, static_cast<int>(i), cv::Scalar(static_cast<int>(i) + 1), -1);
  }

  // Draw the background marker
  cv::circle(markers, cv::Point(5, 5), 3, cv::Scalar(255), -1);
  markers.convertTo(out_markers_image, CV_8U);


  cv::watershed(in_sharpened_image, markers);

  // Generate random colors
  std::vector<cv::Vec3b> colors;
  for (size_t i = 0; i < contours.size(); i++)
  {
    int b = cv::theRNG().uniform(0, 256); //0,256
    int g = cv::theRNG().uniform(0, 256);
    int r = cv::theRNG().uniform(0, 256);
    colors.push_back(cv::Vec3b((uchar)b, (uchar)g, (uchar)r));
  }

  // Create the result image
  cv::Mat dst = cv::Mat::zeros(markers.size(), CV_8UC3);
  
  // Fill labeled objects with random colors
  for (int i = 0; i < markers.rows; i++)
  {
    for (int j = 0; j < markers.cols; j++)
    {
      int index = markers.at<int>(i, j);
      if (index > 0 && index <= static_cast<int>(contours.size()))
      {
        dst.at<cv::Vec3b>(i, j) = colors[index - 1];
      }
    }
  }
  cv::cvtColor(dst, out_watershed_image, CV_RGB2GRAY);
  //FINROC_LOG_PRINT(DEBUG, "type : ", out_watershed_image.type());


  return true;
}


//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}
