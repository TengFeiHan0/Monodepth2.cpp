#pragma once
#include "coco_detection.h"

#include <torch/data/example.h>
#include <torch/data/datasets/base.h>

#include <coco.h>


namespace monodepth{
namespace data{

bool has_valid_annotation(std::vector<coco::Annotation> anno);
bool _has_only_empty_bbox(std::vector<coco::Annotation> anno);

struct RCNNData{
  int64_t idx;
  monodepth::structures::BoxList target;
};

// template<typename Self>
// class RCNNDataset : public torch::data::datasets::Dataset<Self, torch::data::Example<torch::Tensor, RCNNData>>{
  
// public:

//   RCNNDataset() = default;
//   torch::data::Example<torch::Tensor, RCNNData> get(size_t index) override;
//   torch::optional<size_t> size() const override;
//   virtual coco::Image get_img_info(int64_t index) = 0;
// };

class COCODataset : public torch::data::datasets::Dataset<COCODataset, torch::data::Example<cv::Mat, RCNNData>>{

public:
  COCODataset(std::string annFile, std::string root, bool remove_images_without_annotations);
  torch::data::Example<cv::Mat, RCNNData> get(size_t index) override;
  torch::optional<size_t> size() const override;
  coco::Image get_img_info(int64_t index);

  std::map<int64_t, std::string> categories;
  std::map<int64_t, int64_t> json_category_id_to_contiguous_id;
  std::map<int64_t, int64_t> contiguous_category_id_to_json_id;
  std::map<int64_t, int64_t> id_to_img_map;
  COCODetection coco_detection;
  //transforms
};

}
}
