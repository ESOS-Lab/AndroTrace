
cp ../../kernel_ing/arch/arm64/boot/Image ../kernel_androtrace

./mkbootimg  --kernel ../kernel_androtrace --ramdisk ../ramdisk/ramdisk_new.gz --cmdline "console=ttyHSL0,115200,n8 androidboot.console=ttyHSL0 user_debug=31 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 androidboot.hardware=p1 msm_rtb.filter=0x37" --base 0x00000000 --kernel_offset 0x00008000 --ramdisk_offset 0x02200000 --second_offset 0x00f00000 --tags_offset 0x00000100 --pagesize 4096 --dt boot_extracted.img-dtb  --output boot_new.img

adb reboot bootloader
fastboot flash boot boot_new.img && fastboot reboot

