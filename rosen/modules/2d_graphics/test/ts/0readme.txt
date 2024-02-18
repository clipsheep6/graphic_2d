1、功能测试
    1）devceo下载安装：https://developer.harmonyos.com/cn/develop/deveco-studio
    2）更新sdk
        最新sdk：https://gitee.com/openharmony/interface_sdk-js/pulls从门禁中下载
        目前deveco最新版本是api9，将下载的skd替换deveco的sdk。默认路径见deveco->tools->skdmanager->sdk->harmonyos/openharmony(最好两个都替换)->location.
        替换完成后需要修改skd的版本号。将Sdk\9\ets\oh-uni-package.json文件中的"apiVersion": "11"修改为"apiVersion": "9"（一共5个目录，都需要替换）
    3）创建默认native工程，安装到设备中会显示hello world。将本目录下工程的文件拷贝到默认工程中，文件：
        MyApplication\entry\src\main\ets\pages
    4）核心代码，见entry/src/main/ets/pages/Index.ets
    5）hap编译安装后，点击桌面按钮。可以显示一个白色背景的红色矩形。同时会把图片保存在该目录下：
       hdc file recv /data/app/el2/100/base/com.example.myapplication_bench/haps/entry/cache/xyjrect.jpg ./
       上述com.example.myapplication_benc需要替换为自己的hap名

2、补充功能用例说明
    1）MyApplication_js_drawing_noxnode\entry\src\main\ets\pages\drawrect.ts 目录下新增子类，继承自TestBase
    2)子类构造函数中可以初始化TestBase的配置属性，可按需更改。 
        width_/height_:生成图片的宽高，默认500*500；
        background_：生成图片的背景色，默认白色；
        fileName_：生成图片的文件名，默认为用例名.jpg.文件保存于/data/app/el2/100/base/com.example.myapplication_0202sdk/files/目录下
    3）重写OnDraw接口，即为用例的绘制流程，可参考DrawRect::OnDraw()
    4）注册实例构造接口：entry\src\main\ets\pages\Index.ets：aboutToAppear()
    5）界面增加buttom调用用例，如：entry\src\main\ets\pages\Index.ets：Button(DRAWRECT)

