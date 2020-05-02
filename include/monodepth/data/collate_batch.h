#pragma once

#include <torch/data/example.h>
#include <torch/data/transforms/collate.h>
#include <bounding_box.h>
#include <image_list.h>
#include "datasets/coco_datasets.h"


namespace monodepth{
namespace data{

using batch = std::tuple<monodepth::structures::ImageList, std::vector<monodepth::structures::BoxList>, std::vector<int64_t>>;
//<output, input>
struct BatchCollator : public torch::data::transforms::Collation<batch, std::vector<torch::data::Example<torch::Tensor, RCNNData>>>{

  BatchCollator(int size_divisible);
  batch apply_batch(std::vector<torch::data::Example<torch::Tensor, RCNNData>> examples) override;

  int size_divisible_;

};

}
}