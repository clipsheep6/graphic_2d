hdc shell "mount -o remount,rw /"

hdc shell "cd /data/local/tmp/; tar cvf dm_output.tar dm_output/"
hdc file recv /data/local/tmp/dm_output.tar ./
pause