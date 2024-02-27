1、demo编译准备
    代码见：https://gitee.com/xieyijun3/graphic_graphic_2d/pulls/13
    1）核心代码，见rosen/test/2d_graphics/drawing/drawing_demo_draw_rect.cpp
    4）编译产物(build target = graphic_2d_test)：drawing_demo,见out\rk3568\graphic\graphic_2d
    5）将drawing推送到设备上，创建目录/data/test。执行：

2、功能测试说明（cpu绘制）
    cmd: drawing_demo function {cpu | gpu} {caseName?} {displayTime?}
    cpu：走cpu绘制通路
    gpu：走gpu绘制通路，当前还未适配
    caseName:不填写则遍历所有测试，指定用例名则单用例测试
    displayTime：单用例测试情况下才生效，指定绘制结果显示在屏幕上的时间
    eg：drawing_demo function cpu  执行所有cpu绘制功能用例
    eg：drawing_demo function cpu drawrect 10 执行drawrect cpu绘制功能用例，用例结果在桌面显示10s
    测试用例会将绘制结果保存在图片中，见hdc file recv /data/test/drawrect.jpg ./

3、补充功能用例说明（cpu绘制）
    1）rosen/modules/2d_graphics/test/drawing 目录下新增子类，继承自TestBase
    2)子类构造函数中可以初始化TestBase的配置属性，可按需更改。 
        width_/height_:生成图片的宽高，默认500*500；
        background_：生成图片的背景色，默认白色；
        fileName_：生成图片的文件名，默认为用例名.jpg.文件保存于/data/test/目录下
    3）重写OnTestFunctionCpu接口，即为用例的绘制流程，可参考void DrawRectTest::OnTestFunctionCpu()

4、性能测试说明（cpu绘制）
    cmd: drawing_demo performance {cpu | gpu} caseName count
    cpu：走cpu绘制通路
    gpu：走gpu绘制通路，当前还未适配
    caseName:指定测试的用例名称
    count：目标接口的执行次数
    eg：drawing_demo performance cpu drawrect 1000000 执行drawrect cpu绘制性能用例，重复次数1000000次
    执行结果将打印在终端和log日志中

5、补充性能用例说明（cpu绘制）
    1）rosen/modules/2d_graphics/test/drawing 目录下新增子类，继承自TestBase
    2)子类构造函数中可以初始化TestBase的配置属性，可按需更改。 
        width_/height_:生成图片的宽高，默认500*500；
        background_：生成图片的背景色，默认白色；
        fileName_：生成图片的文件名，默认为用例名.jpg.文件保存于/data/test/目录下
    3）重写OnTestPerformanceCpu接口，即为用例的绘制流程，可参考void DrawRectTest::OnTestPerformanceCpu()

6、自动化执行性能测试用例（cpu绘制）
   1）将drawing_demo放在drawing_api_test/demo/drawing目录下
   2）将drawing_api_test文件夹拷贝到本地目录下，用pycharm打开
   3）将要测试的用例名称填写入drawing_api_test/oh_drawing_api.xlsx，并填入yes
   4）drawing_api_test/main.py文件最下面run_all_test函数可以指定性能测试的次数等信息
   5）启动main.py，在drawing_api_test/report中查看结果

   功耗的衡量主要依赖hiperf获取到进程使用的指令数，hiperf文件夹获取以及使用：
        1、工程根目录下执行
        ./developtools/hiperf/script/package.sh  rk3568
        2、获取到hiperf文件夹
        out\host\developtools\hiperf
        3、抓取hiperf
        设备上执行：
        hiperf record -e hw-instructions,hw-cpu-cycles -f 1000 -s dwarf -p {pid} -d {test_time} -o /data/local/tmp/{data_name}_report.data
        4、获取perf文件
        hdc file recv /data/local/tmp/drawrect_report.data ./
        5、Perf文件解析为html
        Hiperf目录下打开cmd命令行执行：python.exe ./make_report.py -i drawrect_report.data
        结果查看：hiperf_report.html