./daemon.sh
adb push androtrace_daemon /sdcard/
cd module
make 
cd ../
adb push androtrace_module.ko /sdcard/
adb push script.sh /sdcard/androtrace.sh
