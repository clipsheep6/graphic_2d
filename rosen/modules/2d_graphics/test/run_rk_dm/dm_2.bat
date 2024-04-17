hdc shell "mount -o remount,rw /"
hdc shell rm /data/locat/tmp/dm
hdc file send dm /data/local/tmp/

hdc shell rm /data/local/tmp/dm_output/*

hdc shell "cd /data/local/tmp/; chmod 777 dm; ./dm -v -w dm_output --config gles --src gm --ignoreSigInt"
pause
