::关闭显示命令
@echo off

::UTF-8代码页
chcp 65001

::GBK代码页
::chcp 936

::美国英语代码页
::chcp 437

echo UTF-8 code page
echo If exist unknown command, open the script and set encoding format
echo.



::获取当前连接的设备
hdc shell mount -o remount,rw /
::hdc file send F:\OHOS\graphic\file\1123\libskia_rs_framework.z.so /system/lib/libskia_rs_framework.z.so
::hdc file send F:\OHOS\graphic\file\1123\texgine_sample_weight_style /data/texgine_sample_weight_style

::hdc shell chmod 777 /data/texgine_sample_weight_style

hdc file recv /data/exlog exe.log
hdc shell rm -rf /data/snap/*
hdc shell mkdir /data/snap/
hdc shell chmod 777 /data/snap/
hdc shell snapshot_display
pause

hdc shell mv /data/local/tmp/*.jpeg /data/snap
hdc shell mv /data/*.jpeg /data/snap
hdc file recv /data/snap D:\xieyijun\07_OH_Drawing\benchmark
::hdc shell 

pause