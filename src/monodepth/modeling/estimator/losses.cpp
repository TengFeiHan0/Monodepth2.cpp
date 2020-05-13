#include <torch/torch.h>
#include "estimator/losses.h"
#include "defaults.h"
#include "misc.h"
#include <tuple>

namespace monodepth{
    namespace modeling{
        
        // torch::Tensor get_smooth_loss(torch::Tensor disp, torch::Tensor img){

        //     torch::Tensor grad_disp_x = torch::abs(disp[:, :, :, :-1] - disp[:, :, :, 1:]);
        //     torch::Tensor grad_disp_y = torch::abs(disp[:, :, :-1, :] - disp[:, :, 1:, :]);

        //     torch::Tensor grad_img_x = torch.mean(torch::abs(img[:, :, :, :-1] - img[:, :, :, 1:]), 1, keepdim=True);
        //     torch::Tensor grad_img_y = torch.mean(torch::abs(img[:, :, :-1, :] - img[:, :, 1:, :]), 1, keepdim=True);

        //     grad_disp_x *= at::exp(-grad_img_x);
        //     grad_disp_y *= at::exp(-grad_img_y);

        //     return grad_disp_x.mean() + grad_disp_y.mean();
        // }



        std::map<std::string, torch::Tensor> compute_losses( std::vector<std::vector<torch::Tensor>> &inputs,
                std::vector<torch::Tensor> &disps, std::vector<std::vector<torch::Tensor>> &recontructed_img){

            std::map<std::string, torch::Tensor> losses;
            torch::Tensor total_loss;
            for(int i=0; i < 4; i++){
                torch::Tensor loss;
                std::vector<torch::Tensor> reprojection_losses;
                int source_scale =i;
                torch::Tensor disp = disps[i];
                torch::Tensor color = inputs[i][0];
                torch::Tensor target = inputs[source_scale][0];
                for(int j = 1; j <2; ++j){
                    torch::Tensor pred = recontructed_img[i][j];
                    reprojection_losses.push_back(compute_reprojection_loss(pred, target));
                }
                torch::Tensor reprojection_loss = at::cat(at::TensorList(reprojection_losses),1);

                std::vector<torch::Tensor> identity_reprojection_losses;
                for(int j=1; j<=2; ++j){
                    torch::Tensor pred = inputs[source_scale][j];
                    identity_reprojection_losses.push_back(compute_reprojection_loss(pred, target));
                }
                torch::Tensor identity_reprojection_loss = at::cat(at::TensorList(identity_reprojection_losses),1);
                
                identity_reprojection_loss += torch::randn(identity_reprojection_loss.sizes()).cuda()*0.00001;

                torch::Tensor combined = at::cat((identity_reprojection_loss, reprojection_loss), 1);
                torch::Tensor to_optimize;
                if(combined.size(1)==1){
                    to_optimize = combined;
                }else{
                    std::tuple<torch::Tensor, torch::Tensor> optimize_and_idxs = at::min(combined, 1);
                    to_optimize = std::get<0>(optimize_and_idxs);
                    torch::Tensor idxs = std::get<1>(optimize_and_idxs);
                }
                loss += to_optimize.mean();

                torch::Tensor mean_disp = disp.mean(2,true).mean(3, true);
                torch::Tensor norm_disp = disp / (mean_disp +1e-7);
                // torch::Tensor smooth_loss = get_smooth_loss(norm_disp, color);
                // int64_t smooth_weight =  monodepth::config::GetCFG<int64_t>({"MODEL","DEPTH", "SMOOTH_WEIGHT"});

                // loss += smooth_weight * smooth_loss / (2**i);
                total_loss +=loss;
            }
            total_loss /=4;
            losses["loss"] = total_loss;

            return losses;
        };
        
        std::vector<std::vector<torch::Tensor>>  generate_image_pred( 
                     std::vector<std::vector<torch::Tensor>> &inputs,std::vector<torch::Tensor> &disps, 
                     std::map<int, torch::Tensor> &cam_T_cam){

                std::vector<std::vector<torch::Tensor>> sample_img;
                std::vector<std::vector<torch::Tensor>> recontructed_img;
            
                for(int i= 0; i<4; i++){
                    torch::Tensor disp = disps[i];
                    int source_scale = i; 
                    int64_t min_depth =  monodepth::config::GetCFG<int64_t>({"MODEL","DEPTH", "MIN_DEPTH"});
                    int64_t max_depth = monodepth::config::GetCFG<int64_t>({"MODEL","DEPTH", "MAX_DEPTH"});

                    std::tuple<torch::Tensor, torch::Tensor> tuple_depth = monodepth::layers::disp_to_depth(disp, min_depth, max_depth);
                    torch::Tensor depth = std::get<1>(tuple_depth);

                    for(int j=1; j<=2; j++){
                        torch::Tensor T = cam_T_cam[j];
                        torch::Tensor cam_points = backproject_depth[source_scale]->forward(depth, inv_K[source_scale]);
                        torch::Tensor pix_coords = project_3d[source_scale]->forward(cam_points, K[source_scale], T);

                        sample_img[i][j]= pix_coords;
                        recontructed_img[i][j] = torch::nn::functional::grid_sample(inputs[i][j], sample_img[i][j],
                            torch::nn::functional::GridSampleFuncOptions().mode(torch::kBilinear).padding_mode(torch::kBorder));
                    }


                }
                return recontructed_img;

        }
        
        torch::Tensor compute_reprojection_loss(torch::Tensor pred, torch::Tensor target){
            torch::Tensor abs_diff = torch::abs(target - pred);
            auto l1_loss = abs_diff.mean(1, true);

            torch::Tensor ssim_loss = SSIM->forward(pred, target);

            ssim_loss = ssim_loss.mean(1, true);

            torch::Tensor reprojection_loss = 0.85*ssim_loss +0.15*l1_loss;

            return reprojection_loss;

        }

            
    }
}