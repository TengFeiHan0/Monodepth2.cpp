#include <iostream>
#include <typeinfo>
#include <cassert>
#include "defaults.h"
#include "modeling.h"
#include <torch/torch.h>

#include <torch/script.h>

#include "trainer.h"

#include "inference.h"


using namespace std;
using namespace monodepth;
using namespace coco;



int main(int argc, char *argv[], char *envp[]) {
  // torch::set_default_dtype(torch::kF32);
  //config path
  monodepth::config::SetCFGFromFile(argv[1]);
  cout << "load complete" << endl;
  // monodepth::utils::jit_to_cpp<monodepth::utils::VoVNetMapper>("../python_utils", argv[1], std::vector<std::string>{"backbone.pth", "bbox_pred.pth", "cls_score.pth", "extractor_fc6.pth", "extractor_fc7.pth", "rpn_bbox.pth", "rpn_conv.pth", "rpn_logits.pth"});

  if(string(argv[2]).compare("inference") == 0)
    engine::inference();
  else if(string(argv[2]).compare("train") == 0)
    engine::do_train();
  else{
    //only supports train and inference
    assert(false);
  }

  return 0;
}