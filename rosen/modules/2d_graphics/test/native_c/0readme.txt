1、环境准备
    1）devceo下载安装：https://developer.harmonyos.com/cn/develop/deveco-studio
    2）更新sdk
        最新sdk：https://gitee.com/openharmony/interface_sdk-js/pulls从门禁中下载
        目前deveco最新版本是api9，将下载的skd替换deveco的sdk。默认路径见deveco->tools->skdmanager->sdk->harmonyos/openharmony(最好两个都替换)->location.
        替换完成后需要修改skd的版本号。将Sdk\9\ets\oh-uni-package.json文件中的"apiVersion": "11"修改为"apiVersion": "9"（一共5个目录，都需要替换）
    3）创建默认native工程，安装到设备中会显示hello world。将本目录下工程的文件拷贝到默认工程中，文件：
        MyApplication_c_perf_demo\entry\src\main\cpp
        MyApplication_c_perf_demo\entry\src\main\ets
    4）核心代码，见MyApplication_c_perf_demo\entry\src\main\cpp\samples\drawing_demo.cpp：void DrawingDemo::Draw(napi_env env, void* data)

2、功能测试
    a)cpu离屏绘制+xcomponent:离屏绘制，绘制完成后将绘制结果通过xcomponent组件显示在屏幕上
        1）function/performance选择function
        2）选择cpu-xcomponent
        3）TestCase输入用例名称，如drawrect
        4）点击draw按钮。可以显示一个白色背景的红色矩形。同时会把图片保存在该目录下：
        hdc file recv /data/app/el2/100/base/com.example.MyApplication_c_perf_demo/files/native_rect.jpg ./
        5）点击TestAll，可以执行所有用例并保存图片
    a)cpu离屏绘制+Xnode:直接在xnode的显示区域上进行离屏绘制
        1）function/performance选择function
        2）选择cpu-xnode
        3）TestCase输入用例名称，如drawrect
        4）点击draw按钮。可以显示一个白色背景的红色矩形。同时会把图片保存在该目录下：
        hdc file recv /data/app/el2/100/base/com.example.MyApplication_c_perf_demo/files/native_rect.jpg ./
        5）点击snapshort，会将当前的应用界面截图保存

3、补充功能用例说明
    1）entry\src\main\cpp\testcase\ 目录下新增子类，继承自TestBase
    2)子类构造函数中可以初始化TestBase的配置属性，可按需更改。 
        width_/height_:生成图片的宽高，默认500*500；
        background_：生成图片的背景色，默认白色；
        fileName_：生成图片的文件名，默认为用例名.jpg.文件保存于/data/app/el2/100/base/com.example.MyApplication_c_perf_demo/files/目录下
    3）重写OnTestFunctionCpu接口，即为用例的绘制流程，可参考void DrawRectTest::OnTestFunctionCpu()
    4）补充用例映射表：entry\src\main\cpp\testcasefactory.cpp：FunctionalCpuMap/PerformanceCpuMap

4、性能测试
    a)cpu离屏绘制+xcomponent:离屏绘制，绘制完成后将绘制结果通过xcomponent组件显示在屏幕上
        1）function/performance选择performance
        2）选择cpu-xcomponent
        3）TestCase输入用例名称，如drawrect
        4）testcount输入执行次数，如10000
        5）点击draw按钮。可以显示一个白色背景的红色矩形。同时会在页面底部显示用时
    a)cpu离屏绘制+Xnode:直接在xnode的显示区域上进行离屏绘制
        1）function/performance选择performance
        2）选择cpu-xnode
        3）TestCase输入用例名称，如drawrect
        4）testcount输入执行次数，如10000
        5）点击draw按钮。可以显示一个白色背景的红色矩形。同时会在页面底部显示用时
        6）使用xnode框架执行性能测试时候，如果绘制100000次，应用的界面渲染负载会非常重，会导致响应不及时。可以点击clear按钮清除xnode中的绘制指令，即可恢复流畅

5、补充性能用例说明(当前只适配了cpu绘制)
    1）entry\src\main\cpp\testcase\ 目录下新增子类，继承自TestBase
    2)子类构造函数中可以初始化TestBase的配置属性，可按需更改。 
        width_/height_:生成图片的宽高，默认500*500；
        background_：生成图片的背景色，默认白色；
    3）重写OnTestPerformanceCpu接口，即为用例的绘制流程，可参考void DrawRectTest::OnTestPerformanceCpu()
    4）补充用例映射表：entry\src\main\cpp\testcasefactory.cpp：FunctionalCpuMap/PerformanceCpuMap

    
