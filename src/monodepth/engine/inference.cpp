#include "inference.h"

#include <set>
#include <string>
#include <iostream>

#include <data.h>
#include <defaults.h>
#include <paths_catalog.h>
#include <checkpoint.h>


namespace monodepth{
namespace engine{

using namespace monodepth::utils;
using namespace monodepth::data;
using namespace monodepth::config;

void inference(){
  //Build Model
  GeneralizedRCNN model = BuildDetectionModel();
  
  //Build Dataset
  vector<string> dataset_list = GetCFG<std::vector<std::string>>({"DATASETS", "TEST"});
  Compose transforms = BuildTransforms(false);
  BatchCollator collate = BatchCollator(GetCFG<int>({"DATALOADER", "SIZE_DIVISIBILITY"}));
  COCODataset coco = BuildDataset(dataset_list, false);
  auto data = coco.map(transforms).map(collate);
  shared_ptr<torch::data::samplers::Sampler<>> sampler = make_batch_data_sampler(coco, false, 0);
  
  int images_per_batch = GetCFG<int64_t>({"TEST", "IMS_PER_BATCH"});
  torch::data::DataLoaderOptions options(images_per_batch);
  options.workers(GetCFG<int64_t>({"DATALOADER", "NUM_WORKERS"}));
  auto data_loader = torch::data::make_data_loader(std::move(data), *dynamic_cast<GroupedBatchSampler*>(sampler.get()), options);
  string output_dir = GetCFG<std::string>({"OUTPUT_DIR"});
  string weight_dir = GetCFG<std::string>({"MODEL", "WEIGHT"});
  Checkpoint::load(model, output_dir, weight_dir);
  //Check iou type
  set<string> iou_types{"bbox"};
  if(GetCFG<bool>({"MODEL", "MASK_ON"}))
    iou_types.insert("segm");

  string output_folder = GetCFG<string>({"OUTPUT_DIR"});
  
  torch::Device device(GetCFG<string>({"MODEL", "DEVICE"}));

  //todo
  cout << "Start evaluation on " << dataset_list[0] << " dataset[" << coco.size().value() << "]\n";
  Timer total_time = Timer();
  Timer inference_timer = Timer();

  total_time.tic();
  map<int64_t, BoxList> predictions = compute_on_dataset(model, data_loader, device, inference_timer, coco.size().value());

  auto total_time_ = total_time.toc();
  string total_time_str = total_time.avg_time_str();

  cout << "Total run time: " << total_time_str << " (" << total_time_ * /*device num*/1 / coco.size().value() << " s / img per device, on 1 devices)\n";

  cout << "Model inference time: " << inference_timer.total_time.count() << "s (" << inference_timer.total_time.count() / coco.size().value() << " s / img per device, on 1 devices)\n";
  monodepth::config::DatasetCatalog dataset_catalog = monodepth::config::DatasetCatalog();
  std::string dataset_name, img_dir, ann_file;
  std::tie(dataset_name, img_dir, ann_file) = dataset_catalog[dataset_list[0]];

  DoCOCOEvaluation(coco, predictions, output_folder, iou_types, ann_file);
}

}
}