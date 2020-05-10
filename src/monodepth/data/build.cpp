#include "build.h"
#include "paths_catalog.h"
#include "tovec.h"

#include <cassert>
#include <type_traits>
#include <iostream>

#include "transforms/build.h"
#include "collate_batch.h"
#include "defaults.h"
#include "samplers/samplers.h"
#include "samplers/build.h"

#include <torch/data/dataloader/stateful.h>
#include <torch/data/dataloader/stateless.h>

#include <torch/csrc/utils/memory.h>
#include <torch/csrc/utils/variadic.h>



namespace monodepth{
namespace data{

//supports only coco dataset
//TODO Concat dataset
COCODataset BuildDataset(std::vector<std::string> dataset_list, bool is_train){
  assert(dataset_list.size() == 1);
  monodepth::config::DatasetCatalog dataset_catalog = monodepth::config::DatasetCatalog();
  std::string dataset_name, img_dir;
  std::tie(dataset_name, img_dir) = dataset_catalog[dataset_list[0]];

  if(dataset_name.compare("COCODataset") == 0)
    return COCODataset(ann_file, img_dir, is_train);
  else if(dataset_name.compare("CityScapesDataset")==0)
    return CityScapesDataset(img_dir);
  else
    assert(false);
}

// template<>
// torch::data::StatelessDataLoader<COCODataset, IterationBasedBatchSampler> MakeDataLoader(bool is_train /*, is_distributed=false */, int start_iter){

//   std::vector<std::string> dataset_list;
//   if(is_train){
//     auto dataset = monodepth::config::GetCFG<monodepth::config::CFGS>({"DATASETS", "TRAIN"});
//     dataset_list = monodepth::config::tovec(dataset.get());
//   }
//   else{
//     auto dataset = monodepth::config::GetCFG<monodepth::config::CFGS>({"DATASETS", "TEST"});
//     dataset_list = monodepth::config::tovec(dataset.get());
//   }
    

//   Compose transforms = BuildTransforms(is_train);
//   BatchCollator collate = BatchCollator(monodepth::config::GetCFG<int>({"DATALOADER", "SIZE_DIVISIBILITY"}));
  
//   int64_t images_per_batch;
//   if(is_train){
//     images_per_batch = monodepth::config::GetCFG<int64_t>({"SOLVER", "IMS_PER_BATCH"});
//   }
//   else{
//     images_per_batch = monodepth::config::GetCFG<int64_t>({"TEST", "IMS_PER_BATCH"});
//   }
//   COCODataset coco = BuildDataset(dataset_list, is_train);
//   auto dataset = coco.map(transforms).map(collate);
//   std::shared_ptr<torch::data::samplers::Sampler<>> sampler = make_batch_data_sampler(coco, is_train, start_iter);


//   torch::data::DataLoaderOptions options(images_per_batch);
//   options.workers(monodepth::config::GetCFG<int64_t>({"DATALOADER", "NUM_WORKERS"}));
//   int num_iter = monodepth::config::GetCFG<int64_t>({"SOLVER", "MAX_ITER"});
//   return torch::data::make_data_loader(std::move(dataset), *dynamic_cast<IterationBasedBatchSampler*>(sampler.get()), options);
//   //TODO make it template
// }

// template<>
// torch::data::StatelessDataLoader<COCODataset, GroupedBatchSampler> MakeDataLoader(bool is_train /*, is_distributed=false */, int start_iter){

//   std::vector<std::string> dataset_list;
//   if(is_train){
//     auto dataset = monodepth::config::GetCFG<monodepth::config::CFGS>({"DATASETS", "TRAIN"});
//     dataset_list = monodepth::config::tovec(dataset.get());
//   }
//   else{
//     auto dataset = monodepth::config::GetCFG<monodepth::config::CFGS>({"DATASETS", "TEST"});
//     dataset_list = monodepth::config::tovec(dataset.get());
//   }
    

//   Compose transforms = BuildTransforms(is_train);
//   BatchCollator collate = BatchCollator(monodepth::config::GetCFG<int>({"DATALOADER", "SIZE_DIVISIBILITY"}));
  
//   int64_t images_per_batch;
//   if(is_train){
//     images_per_batch = monodepth::config::GetCFG<int64_t>({"SOLVER", "IMS_PER_BATCH"});
//   }
//   else{
//     images_per_batch = monodepth::config::GetCFG<int64_t>({"TEST", "IMS_PER_BATCH"});
//   }
//   COCODataset coco = BuildDataset(dataset_list, is_train);
//   auto dataset = coco.map(transforms).map(collate);
//   std::shared_ptr<torch::data::samplers::Sampler<>> sampler = make_batch_data_sampler(coco, is_train, start_iter);


//   torch::data::DataLoaderOptions options(images_per_batch);
//   options.workers(monodepth::config::GetCFG<int64_t>({"DATALOADER", "NUM_WORKERS"}));
//   int num_iter = monodepth::config::GetCFG<int64_t>({"SOLVER", "MAX_ITER"});
//   //return torch::data::make_data_loader(std::move(dataset), *dynamic_cast<IterationBasedBatchSampler*>(sampler.get()), options);
//   //TODO make it template
//   return torch::data::make_data_loader(std::move(dataset), *dynamic_cast<GroupedBatchSampler*>(sampler.get()), options);
// }

// template<>
// torch::data::StatelessDataLoader<COCODataset, torch::data::samplers::RandomSampler> MakeDataLoader(bool is_train /*, is_distributed=false */, int start_iter){

//   std::vector<std::string> dataset_list;
//   if(is_train){
//     auto dataset = monodepth::config::GetCFG<monodepth::config::CFGS>({"DATASETS", "TRAIN"});
//     dataset_list = monodepth::config::tovec(dataset.get());
//   }
//   else{
//     auto dataset = monodepth::config::GetCFG<monodepth::config::CFGS>({"DATASETS", "TEST"});
//     dataset_list = monodepth::config::tovec(dataset.get());
//   }
    

//   Compose transforms = BuildTransforms(is_train);
//   BatchCollator collate = BatchCollator(monodepth::config::GetCFG<int>({"DATALOADER", "SIZE_DIVISIBILITY"}));
  
//   int64_t images_per_batch;
//   if(is_train){
//     images_per_batch = monodepth::config::GetCFG<int64_t>({"SOLVER", "IMS_PER_BATCH"});
//   }
//   else{
//     images_per_batch = monodepth::config::GetCFG<int64_t>({"TEST", "IMS_PER_BATCH"});
//   }
//   COCODataset coco = BuildDataset(dataset_list, is_train);
//   auto dataset = coco.map(transforms).map(collate);
//   std::shared_ptr<torch::data::samplers::Sampler<>> sampler = make_batch_data_sampler(coco, is_train, start_iter);


//   torch::data::DataLoaderOptions options(images_per_batch);
//   options.workers(monodepth::config::GetCFG<int64_t>({"DATALOADER", "NUM_WORKERS"}));
//   int num_iter = monodepth::config::GetCFG<int64_t>({"SOLVER", "MAX_ITER"});
//   return torch::data::make_data_loader(std::move(dataset), *dynamic_cast<torch::data::samplers::RandomSampler*>(sampler.get()), options);
  
// }

}
}