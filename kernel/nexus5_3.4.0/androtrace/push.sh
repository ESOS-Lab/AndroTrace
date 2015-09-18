./daemon.sh
adb push androtrace_daemon /data/androtrace/
cd module
make 
cd ../
adb push androtrace_module.ko /data/androtrace/
adb push script.sh /data/androtrace/androtrace.sh
