

20240329记录：
蓝区rk编译执行skia dm测试套：

1、刚才根目录下执行：./third_party/skia/auto_build/add_skia_dm_test_RK3568.sh
	参考：https://gitee.com/openharmony/third_party_skia/wikis/01.%20skia%E5%A6%82%E4%BD%95%E6%9E%84%E5%BB%BA/skia%E9%83%A8%E4%BB%B6%E5%8C%96%E5%8F%8A%E8%87%AA%E6%B5%8B%E8%AF%95%E6%9E%84%E5%BB%BA%E6%8C%87%E5%AF%BC%E4%B9%A6
2、编译dm测试用例：./build.sh --product-name rk3568 --build-target dm --gn-args is_component_build=false --target-cpu arm
	a）由于编译不过，需要打补丁dmbuild_qosmanager_frameworks.diff。路径：foundation/resourceschedule/qos_manager/frameworks/native/BUILD.gn
	b）由于编译不过，需要打补丁dmbuild_skia_20240329ok.diff。路径：third_party/skia/dmbuild0329.diff
3、拷贝编译产物到本文件目录：
	cp ./rk3568/lib.unstripped/startup/init/libbegetutil.so   xyj
	cp ./rk3568/lib.unstripped/thirdparty/skia/libskia_canvaskit.so xyj
	cp ./rk3568/thirdparty/bounds_checking_function/libsec_shared.so xyj
	cp ./rk3568/lib.unstripped/commonlibrary/c_utils/libutils.so xyj
	cp ./rk3568/dm xyj
	
4、将resource文件打包拷贝到本文件所在目录：
	cd 系统组件/third_party/skia
	tar -zcvf resources.tar.gz resources
	得到resources.tar.gz文件，将resources.tar.gz获取到放在本目录

5、执行dm_1.bat 发送资源/so
6、执行dm_2.bat 发送dm文件，并全量执行
7、执行dm_3.bat 收集结果并发送到本目录下

8、如果要单用例执行，执行dm_2_exe_one.bat即可。需要将脚本中的”blurcircles“字段替换成目标用例的onShortName()