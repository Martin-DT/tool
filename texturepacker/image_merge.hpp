#pragma once

#include <vector>
#define STB_IMAGE_IMPLEMENTATION  
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "rect_packer.hpp"
#include <string>
#include <fstream>
#include "json.hpp"

struct ImagePacker {
    static const int DEFAULT_CHANNEL = STBI_rgb_alpha;
    struct Image{
        std::string name = "";
        int channel = -1;
        int w = 0, h = 0;
        std::vector<unsigned char> data = {};
        ~Image(){
            data = {};
            channel = -1;
            w = 0;
            h = 0;
        }
        bool load(const char * path){
            stbi_uc * stbi_data = stbi_load(path, &w, &h, &channel, 0);
            // if(channel != DEFAULT_CHANNEL){
            //     printf("Err image %s is not %d!\n", path, DEFAULT_CHANNEL);
            //     // return false;
            // }
            size_t sz = w * h * channel;
            data.resize(sz);
            memcpy(data.data(), stbi_data, sz);
            return sz;
        }
        Image() = default;
    };
    
    int packer_w = 0;
    int packer_h = 0;
    std::vector<Image> imageList;
    std::vector<Rect> rcList;

    void clear(){
        imageList = {};
        rcList = {};
    }
    bool addImage(const char * path){
        Image img;
        if(!img.load(path)){
            return false;
        }
        auto file = std::filesystem::path(path);
        img.name = file.filename();
        imageList.push_back(img);
        return true;
    }
    
    int max_margin(int m, int tm){
        return m < tm ? tm : m;
    }
    int makePowerOfTwo(int num) {
        if (num <= 0) {
            return 1;
        }
        
        int power = 1;
        
        while (power < num) {
            power <<= 1;
        }
        
        return power;
    }
    bool checkPacker(int w, int h, int interval){
        RectPacker packer(w, h);
        packer.setInterval(interval);
        int interval_x = 0, interval_y = 0;
        int max_x = 0, max_y = 0;
        std::vector<Rect> temp_rects;
        for(int i=0;i<imageList.size();i++){
            const auto & img = imageList[i];
            bool bok = false;
            Rect temp_rc = packer.insert(img.w, img.h, &bok);
            if (!bok) {
                printf("Unable to fit size %dx%d\n", img.w, img.h);
                return false;
            }
            assert(temp_rc.width == img.w && temp_rc.height == img.h);
            temp_rects.push_back(temp_rc);
            
            interval_x = interval_x > 0 ? interval_x : temp_rc.x;
            interval_y = interval_y > 0 ? interval_y : temp_rc.y;
            max_x = max_margin(max_x, temp_rc.x + temp_rc.width);
            max_y = max_margin(max_y, temp_rc.y + temp_rc.height);
        }
        this->rcList = temp_rects;
        packer_w = max_x + interval_x;
        packer_h = max_y + interval_y;
        packer_w = makePowerOfTwo(packer_w);
        packer_h = makePowerOfTwo(packer_h);
        return true;
    }
    bool merge( const char* output_path, const char * name_without_suffix){
        // 创建合并后的图像画布  
        std::vector<unsigned char> canvas(packer_w * packer_h * DEFAULT_CHANNEL, 0); // 假设所有图像都是RGBA格式  
        int idx = 0;
        // 合并图像到画布  
        for (const auto & img : imageList){
            auto rc = rcList[idx++];
            for (int y = 0; y < rc.height; y++){
                for (int x = 0; x < rc.width; x++){
                    int img_data_index = (y * rc.width + x) * img.channel;
                    int canvas_index = ((y+rc.y) * packer_w + x + rc.x) * DEFAULT_CHANNEL;
                    
                    // 复制像素数据到画布  
                    canvas[canvas_index + 0] += img.data[img_data_index + 0]; // R  
                    canvas[canvas_index + 1] += img.data[img_data_index + 1]; // G  
                    canvas[canvas_index + 2] += img.data[img_data_index + 2]; // B  
                    if(img.channel == STBI_rgb){
                        canvas[canvas_index + 3] = 255;
                    }
                    else
                        canvas[canvas_index + 3] = std::max(canvas[canvas_index + 3], img.data[img_data_index + 3]); // A 
                }
            }
        }
        std::string file = output_path;
        file += "/";
        file += name_without_suffix;
        file += ".png";
    
        // 写入输出文件  
        int success = stbi_write_png(file.c_str(), packer_w, packer_h, DEFAULT_CHANNEL, canvas.data(), packer_w * DEFAULT_CHANNEL);  
        if (!success) {  
            printf("Error saving merged image to %s\n", file.c_str());
            return false;  
        }

        file = output_path;
        file += "/";
        file += name_without_suffix;
        file += ".json";
        toJson(file.c_str());

        return true;
    }

    std::string toJson(const char * file){
        using json = nlohmann::json;
        json j;
        int idx = 0;
        j["size"] = {
            {"w", packer_w},
            {"h", packer_h}
        };
        // 合并图像到画布  
        for (const auto & img : imageList){
            auto rc = rcList[idx++];
            j["subitem"][img.name] = {
                { "x", rc.x },
                { "y", rc.y },
                { "w", rc.width },
                { "h", rc.height}
            };
        }
        if(file)
            std::ofstream(file) << std::setw(4) << j << std::endl;
        return j.dump();
    }
};

#ifdef TEST_IMAGE_PACKER
#include <filesystem>
struct ImagePackerTest {
    std::vector<Rect> rects;
    ImagePacker packer;
    
    bool loadImages(const char * path){
        namespace fs = std::filesystem;
        auto testdir = fs::path(path);
        if (!fs::exists(testdir)) {
            printf("file or directory is not exists!\n");
            return false;
        }
        fs::directory_options opt(fs::directory_options::none);
        fs::directory_entry dir(testdir);
        int idx = 0;
        for (fs::directory_entry const& entry : fs::directory_iterator(testdir, opt))  {
            if (entry.is_regular_file()) {
                if(!packer.addImage(entry.path().c_str())){
                    printf("Error loading image : %s\n", entry.path().c_str());
                    continue;
                }
                printf("Sus loading image%s\n", entry.path().c_str());
            }
            idx ++;
        }
        return true;
    }
    bool resize(int w, int h, int interval = 5){
        return packer.checkPacker(w, h, 5);
    }
};
#endif