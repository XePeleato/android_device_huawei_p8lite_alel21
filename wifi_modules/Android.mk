ifeq (hikey, $(TARGET_PRODUCT))
export TOOLCHAIN_URL=https://releases.linaro.org/15.02/components/toolchain/binaries/aarch64-linux-gnu/gcc-linaro-4.9-2015.02-3-x86_64_aarch64-linux-gnu.tar.xz
export TOOLCHAIN_DIR=$(shell basename ${TOOLCHAIN_URL} .tar.xz )
export TOOLCHAIN_TARBALL=$(shell basename ${TOOLCHAIN_URL} )
wifi_modules: android_kernel
	mkdir -p $(ANDROID_PRODUCT_OUT)/obj/wifi-build &&\
	cd $(ANDROID_PRODUCT_OUT)/obj/wifi-build &&\
	export TOOLCHAIN_URL=https://releases.linaro.org/15.02/components/toolchain/binaries/aarch64-linux-gnu/gcc-linaro-4.9-2015.02-3-x86_64_aarch64-linux-gnu.tar.xz &&\
	if [ ! -d ${TOOLCHAIN_DIR} ] ; then wget ${TOOLCHAIN_URL}; tar -xJf ${TOOLCHAIN_TARBALL}; fi &&\
	export PATH=${PATH}:${PWD}/${TOOLCHAIN_DIR}/bin/ &&\
	cp -rf $(ANDROID_BUILD_TOP)/kernel/linaro/hisilicon/ kernel/ &&\
	cp -rf $(ANDROID_PRODUCT_OUT)/obj/kernel/. kernel/ &&\
	cd kernel/ &&\
	rm -rf build-utilites &&\
	git clone --depth 1 https://github.com/96boards/wilink8-wlan_build-utilites.git build-utilites &&\
	cp -a build-utilites/setup-env.sample build-utilites/setup-env &&\
	sed -e "s|^export TOOLCHAIN_PATH=.*|export TOOLCHAIN_PATH=$(ANDROID_PRODUCT_OUT)/obj/wifi-build/${TOOLCHAIN_DIR}/bin|" -e "s|^export KERNEL_PATH=.*|export KERNEL_PATH=$(ANDROID_PRODUCT_OUT)/obj/wifi-build/kernel|" -e "s|^export CROSS_COMPILE=.*|export CROSS_COMPILE=aarch64-linux-gnu-|" -e "s|^export ARCH=.*|export ARCH=arm64|" -i build-utilites/setup-env &&\
	git clone -b hikey --depth 1 https://github.com/96boards/wilink8-wlan_wl18xx.git    build-utilites/src/driver &&\
	git clone -b R8.5  --depth 1 https://github.com/96boards/wilink8-wlan_wl18xx_fw.git build-utilites/src/fw_download &&\
	git clone -b hikey --depth 1 https://github.com/96boards/wilink8-wlan_backports.git build-utilites/src/backports &&\
	cd build-utilites &&\
	./build_wl18xx.sh modules &&\
	mkdir -p $(ANDROID_PRODUCT_OUT)/system/modules/wifi
	cp `find $(ANDROID_PRODUCT_OUT)/obj/wifi-build/kernel/build-utilites/src -iname *.ko` $(ANDROID_PRODUCT_OUT)/system/modules/wifi/

systemimage: wifi_modules
endif
