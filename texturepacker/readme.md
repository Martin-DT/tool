##### 将多张图片合成一个大图


###### 摘要说明
- 目前仅支持RGB/RGBA格式,输出格式为RGBA
- 目前图片排版不支持旋转,不支持UI拖动
- 需要给定一个指定大小的画布,生成后会裁剪到适合大小
- 输出文件为xxx.png + xxx.json. 
  ```json
    {
        "0@2x.png": {
            "h": 36,
            "w": 36,
            "x": 5,
            "y": 401
        },
        "0@3x.png": {
            "h": 54,
            "w": 54,
            "x": 5,
            "y": 447
        }
    }
  ```
- 界面
  ![alt text](image.png)
  调整窗口大小可以自动调整合批结果.
  ![alt text](image-1.png)
  + 选择YES输出合批结果到指定的目录,并将合批图呈现出来,再次关闭退出程序
  + 选择NO直接退出

###### 示例环境准备(不需要UI非必须)
  - 安装QT5
    + mac
    ```
    brew install qt@5
    ```
###### 示例代码
```c++
//  需要开启 #define TEST_IMAGE_PACKER
ImagePackerTest img_packer_test;
// 从指定目录加载需要合批的文件
img_packer_test.loadImages("xxx/xxx/xxx/");
// 设置合批画布的原始SIZE, 碎图之间的间隔默认是5个像素
// 返回true 计算布局成功, 返回false 计算布局失败
bool bresult = img_packer_test.resize(1024, 1024, 5);
// 合批输出到xxx/xxx/test.png xxx/xxx/test.json
// 返回false 合批失败,要调整画布大小. 
// 返回true 合批成功 test.png 输出的size<=画布size
bresult = img_packer_test.packer.merge("xxx/xxx/", "test");
```

###### 开发依赖
- stb_image.h
- stb_image_write.h
- image_merge.hpp
- rector_packer.hpp
- json.hpp