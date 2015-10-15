#!/system/bin/sh

chmod 0777 /data/androtrace/androtrace_module.ko
insmod /data/androtrace/androtrace_module.ko
chmod 0777 /data/androtrace/androtrace_daemon
/data/androtrace/androtrace_daemon
chmod 777 /system/bin/androtrace_daemon
/system/bin/androtrace_daemon


