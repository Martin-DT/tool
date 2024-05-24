#include <filesystem>
#include <iostream>
#include <sstream>
#define TEST_IMAGE_PACKER
#include "image_merge.hpp"
int main(int argc, char const *argv[])
{
    if(argc<4){
        printf("请按顺序传入参数 资源路径 输出资源名字:dir/xxx 画布大小:500x500 大小不够会显示不下\n");
        return 0;
    }
    printf("------%s-%s-%s\n", argv[1], argv[2], argv[3]);
    
    std::string res_dir = argv[1];
    std::filesystem::path path(argv[2]);
    std::stringstream ss(argv[3]);
    std::string token;
    // 分割字符串并转换为整数
    int width, height;
    std::getline(ss, token, 'x');
    width = std::stoi(token);
    std::getline(ss, token, 'x');
    height = std::stoi(token);

    std::string merge_image_name = path.filename();
    std::string merge_image_dir = path.parent_path();
    printf("资源路径为 %s, 输出到 %s 文件 %s, 画布大小为 %d, %d\n", 
        res_dir.c_str(), merge_image_dir.c_str(), merge_image_name.c_str(), width, height);

    // return 0;
    ImagePackerTest img_packer_test;
    if(!img_packer_test.loadImages(res_dir.c_str())){
        printf("加载资源出错!\n");
        return 0;
    }
    
    img_packer_test.resize(width, height);
    if(img_packer_test.packer.imageList.size() != img_packer_test.packer.rcList.size()){
        printf("画布太小!\n");
        return 0;
    }

    img_packer_test.packer.merge(merge_image_dir.c_str(), merge_image_name.c_str());
    
    return 0;
}
