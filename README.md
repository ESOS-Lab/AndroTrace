AndroTrace 
=======================
* Maintainer: Eunryoung Lim (eunryoung.lim@gmail.com)
* Reference: Eunryoung Lim, Seongjin Lee, Youjip Won, ¡°Androtrace: Framework for Tracing and Analyzing IOs on Android¡±,  In Proceedings of 3rd workshop on Interactions of NVM/Flash with Operating-Systems and Workloads (INFLOW 2015), California, USA, Oct. 4-7, 2015
* http://www.esos.hanyang.ac.kr/files/publication/conferences/international/androtrace.pdf

------------
AndroTrace is a framework to collect ***'REAL'*** IO traces and file life-span from Android platform devices. There is no limitation of partition and period in collecting traces. 
We modified Android kernel to capture all IO traces that blktrace collects in the block device layer. 
AndroTrace also collects more essential attributes for understanding Android IO characteristics, such as data block, file type, flags for *fsync()*, *fsync()* from SQLite. 
IO operation performed by SQLite, the data management system most android applications use, is one of the most important factors because SQLite uses journaling for recovery with generation of write request.
And we also collect file life-span with file size, file type and others when file is deleted. File delete requests metadata update to storage. All of collected traces are held in memory buffer temporally until user daemon moves it to storage.

All information what we collects are as following:
* IO completion date, time (nsec)
* Flags for IO operation (read, write)
* Flags for fsync(), fdatasync(), fsync() from SQLite
* Device number (major:minor)
* Block type (meta, journal, data)
* Sector address and IO size
* File type and file name which file request IO to storage
* Process name 
* File life-span with file size, file type, others when file is deleted 

Behavior
----------------
![Image of AndroTrace]( http://dmclab.hanyang.ac.kr/wikidata/img/androtrace_behavior.jpg)

Kernel Modification
----------------
* Kernel source is based on Linux kernel 3.4.0 (Defined by CONFIG_UDROID)
* Block layer to collects IO traces 
    * block\blk-core.c
* VFS layer to collects file life-span 
    * fs\inode.c
    * fs\namei.c
    * fs\open.c
* JBD2 layer to log flags for fsync()
    * fs\sync.c
    * fs\ext4\fsync.c
    * fs\jbd2\commic.c
    * fs\jbd2\journal.c
* Added new filed to existing structure (flags for fsync(), file creation time,...)
    * include\fs.h
    * include\jbd2.h
* [New] Kernel module & User daemon
    * androtrace\androtrace_module.c
    * androtrace\androtrace_daemon.c

Setup
--------
    1. Download kernel source code
        # Nexus 5:
	  # git clone https://android.googlesource.com/kernel/msm.git kernel
          # cd kernel
          # git checkout 3.4.0-gadb2201
        # LG G4
          # download from http://opensource.lge.com/osSch/list?types=ALL&search=F500S
    2. Patch kernel
        # patch source code what AnaroTrace modified (search CONFIG_ANDROTRACE) 
    3. Set folder where traces files will be saved (refer to androtrace\androtrace_daemon.c)
	# search #define ANDROTRACE_IO_DIR
    4. Build Kernel (configuration setting)
    	# nexus5 use arm and g4 use arm64, check Makefile and README.txt for each device model)
        # nexus5
          # make ARCH = arm hammmerhead_defconfig
          # make menuconfig (check all Enable loadable module support)
          # make ARCH = arm CROSS_COMPILE=[toolchain path]
        # g4
          # make ARCH = arm64 p1_skt_kr-perf_defconfig
          # make ARCH = arm64 CROSS_COMPILE=/opt/toolchains/aarch64-linux-android-4.9/bin/aarch64-linux-android- 
    5. Flash Custom boot img (custrom ramdisk, custom kernel) (refer to ./custom_img/porting.sh)
    6. Build androtrace kernel module & user daemon (refer to ./androtrace/push.sh) and push them to target device.
    7. Reboot device
    8. insmod kernel module & execute user daemon automatically
    8. androtrace start collects IO traces
    9. [Appendix] Convert IO trace files to .txt (./kernel/model soruce/androtrace/parser/pull.sh, parse.sh, incase you want to check trace data.



2014-10-14, Eunryoung Lim <eunryoung.lim@gmail.com>

