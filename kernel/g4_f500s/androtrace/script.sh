#!/system/bin/sh

chmod 755 /data/androtrace/androtrace_module.ko
insmod /data/androtrace/androtrace_module.ko
chmod 755 /data/androtrace/androtrace_daemon
/data/androtrace/androtrace_daemon


