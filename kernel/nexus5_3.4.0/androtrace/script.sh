#!/system/bin/sh

chmod 0755 /data/androtrace/androtrace_module.ko
insmod /data/androtrace/androtrace_module.ko
chmod 0755 /data/androtrace/androtrace_daemon
/data/androtrace/androtrace_daemon


