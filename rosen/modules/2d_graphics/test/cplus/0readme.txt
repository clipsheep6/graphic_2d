1、功能测试流程
    代码见：https://gitee.com/xieyijun3/graphic_graphic_2d/pulls/13
    1）核心代码，见cplus/test_brench/rs_demo_draw_rect_test.cpp
    4）编译产物(build target = graphic_2d_test)：drawing_demo,见out\generic_generic_arm_64only\hisi_all_phone_standard\graphic\graphic_2d
    5）将drawing推送到设备上，创建目录/data/test。执行：./drawing_demo drawrect（用例名称，不填则全部遍历） 10（显示时间）.即可生成绘制图片，见hdc file recv /data/test/drawrect.jpg ./

2、补充功能用例说明
    1）rosen/modules/2d_graphics/test/drawing 目录下新增子类，继承自TestBase
    2)子类构造函数中可以初始化TestBase的配置属性，可按需更改。 
        width_/height_:生成图片的宽高，默认500*500；
        background_：生成图片的背景色，默认白色；
        fileName_：生成图片的文件名，默认为用例名.jpg.文件保存于/data/test/目录下
    3）重写OnRecording接口，即为用例的绘制流程，可参考void DrawRectTest::OnRecording()
