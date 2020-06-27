
//----------------------------------------------------------------------
/*!\file    projects/rock_classification/Algorithm2/mWatershedAlgorithmImplementationA2.h
 *
 * \author  Vimstan Moses
 *
 * \date    2019-07-08
 *
 * \brief Contains mWatershedAlgorithmImplementationA2
 *
 * \b mWatershedAlgorithmImplementationA2
 *
 * Implemenatation of the watershed algorithm
 * Implementation of the final process which is the watershed algorithm
 *
 */
//----------------------------------------------------------------------
#ifndef __projects__rock_classification__Algorithm2__mWatershedAlgorithmImplementationA2_h__
#define __projects__rock_classification__Algorithm2__mWatershedAlgorithmImplementationA2_h__

#include "plugins/structure/tModule.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/coviroa/tImage.h"
#include "rrlib/coviroa/opencv_utils.h"
#include "libraries/image_processing/utils/image_utils.h"
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
// Class declaration
//----------------------------------------------------------------------
//! SHORT_DESCRIPTION
/*!
 * Implemenatation of the watershed algorithm
 */
class mWatershedAlgorithmImplementationA2 : public structure::tModule
{

//----------------------------------------------------------------------
// Ports (These are the only variables that may be declared public)
//----------------------------------------------------------------------
public:

  tInput<rrlib::coviroa::tImage> in_distance_transform_threshold_image;
  tInput<rrlib::coviroa::tImage> in_sharpened_image;
  tOutput<rrlib::coviroa::tImage> out_markers_image;
  tOutput<rrlib::coviroa::tImage> out_watershed_image;

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  mWatershedAlgorithmImplementationA2(core::tFrameworkElement *parent, const std::string &name = "WatershedAlgorithmImplementationA2");

//----------------------------------------------------------------------
// Protected methods
//----------------------------------------------------------------------
protected:

  /*! Destructor
   *
   * The destructor of modules is declared protected to avoid accidental deletion. Deleting
   * modules is already handled by the framework.
   */
  ~mWatershedAlgorithmImplementationA2();

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  virtual void OnStaticParameterChange() override;
  virtual void OnParameterChange() override;
  virtual void Update() override;

  bool ProcessImage(const rrlib::coviroa::tImage& in_distance_transform_threshold_image, const rrlib::coviroa::tImage& in_sharpened_image, rrlib::coviroa::tImage& out_markers_image, rrlib::coviroa::tImage& out_watershed_image);
  bool ProcessImage(const cv::Mat& in_distance_transform_threshold_image, const cv::Mat& in_sharpened_image, cv::Mat& out_markers_image, cv::Mat& out_watershed_image);

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}



#endif
