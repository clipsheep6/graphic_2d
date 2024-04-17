hdc shell "mount -o remount,rw /"
hdc file send resources.tar.gz /data/local/tmp/
hdc shell tar zxvf /data/local/tmp/resources.tar.gz -C /data/local/tmp/
hdc file send libbegetutil.so  /system/lib/platformsdk/
hdc file send libsec_shared.so  /system/lib/platformsdk/
hdc file send libskia_canvaskit.so  /system/lib/platformsdk/
hdc file send libutils.so  /system/lib/platformsdk/
pause