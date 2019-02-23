export CROSS_COMPILE=/home/andrew/dev/dragonboard/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
export ARCH=arm64
if [[ $1 = "full" ]]; then
	make mrproper
	make defconfig distro_custom.config custom.config
fi
make KERNELRELEASE=4.14.0-qcomlt-arm64 Image.gz dtbs modules -j12
rm -rf ./build-arm64-modules/
make KERNELRELEASE=4.14.0-qcomlt-arm64 INSTALL_MOD_PATH=build-${ARCH}-modules modules_install -j12 2&>/dev/zero
depmod -b build-${ARCH}-modules -F System.map 4.14.0-qcomlt-arm64
find ./build-arm64-modules/ -type l -delete 
cd ./build-arm64-modules/
pwd
cd ..
#cat arch/arm64/boot/Image.gz arch/arm64/boot/dts/qcom/apq8016-var-dart-hdmi.dtb > arch/arm64/boot/Image.gz_hdmi_dtb
cat arch/arm64/boot/Image.gz arch/arm64/boot/dts/qcom/apq8016-var-dart-lvds.dtb > arch/arm64/boot/Image.gz_lvds_dtb

