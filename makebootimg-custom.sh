rm boot-sd410-lvds.img 
export CROSS_COMPILE=/home/andrew/dev/dragonboard/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
export ARCH=arm64

cd glasses-kernel
./build.sh $1
cd ..

skales/mkbootimg --kernel glasses-kernel/arch/arm64/boot/Image.gz_lvds_dtb\
              --ramdisk ramdisk/initrd.img \
              --output boot-sd410-lvds.img \
              --pagesize 2048 \
              --base 0x80000000 \
              --cmdline "root=/dev/disk/by-partlabel/rootfs rw rootwait console=ttyMSM0,115200n8"
rm glasses-kernel/arch/arm64/boot/Image.gz_lvds_dtb

echo Finished
