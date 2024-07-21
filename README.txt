# 1. Kernel standalone

	# set variables
kernel_dir=~/linux-imx-phytec-v5.15.71_2.2.2/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/
jarsulk_dir=~/jarsulk-pco/projects/LT-22/Programs/A53/kernel_space/imx8mp-dsi-crosslink-oled/

	# copy kernel module to ~/jarsulk-pco repository
cp ${kernel_dir}README.txt ${kernel_dir}modules.order ${kernel_dir}Makefile ${kernel_dir}imx8mp-dsi-crosslink-oled-i2c.{c,h} ${kernel_dir}imx8mp-dsi-crosslink-oled-main.c $jarsulk_dir

	# copy ~/jarsulk-pco repository to kernel module
cp ${jarsulk_dir}README.txt ${jarsulk_dir}modules.order ${jarsulk_dir}Makefile ${jarsulk_dir}imx8mp-dsi-crosslink-oled-i2c.{c,h} ${jarsulk_dir}imx8mp-dsi-crosslink-oled-main.c $kernel_dir


cd ~/linux-imx-phytec-v5.15.71_2.2.2/

	# add new driver to kernel configuration
nano .config
# ---------------------------------------------------------
# Graphics support
# ...
CONFIG_IMX8MP_DSI_CROSSLINK_OLED=y
# ---------------------------------------------------------

nano arch/arm64/boot/dts/freescale/Makefile
# ---------------------------------------------------------
dtb-$(CONFIG_ARCH_MXC) += imx8mp-phyboard-pollux-rdk-rpmsg-dsi.dtb
# ---------------------------------------------------------

nano arch/arm64/boot/dts/freescale/imx8mp-phyboard-pollux-rdk-rpmsg-dsi.dts
# ---------------------------------------------------------
# ...

&lcdif1 {
	status = "okay";
};

&mipi_dsi {
	status = "okay";

	panel-bridge@0 {
		compatible = "toshiba,tc358867xbg";
		reg = <0>;

		port {
			tc358867_in: endpoint {
				remote-endpoint = <&dsi_out>;
			};
		};

		display-timings {
			lcd {
				// all EDT displays - Leave clock at 25175000!!!
				clock-frequency = <25175000>;
				hactive = <320>;
				vactive = <240>;
				hback-porch = <38>;
				hfront-porch = <20>;
				vback-porch = <14>;
				vfront-porch = <4>;
				hsync-len = <30>;
				vsync-len = <4>;
				hsync-active = <0>;
				vsync-active = <0>;
				de-active = <1>;
				pixelclk-active = <0>;
			};
		};

	};

	port@1 {
		reg = <1>;

		dsi_out: endpoint {
			remote-endpoint = <&tc358867_in>;
		};
	};
};
# ---------------------------------------------------------

nano drivers/gpu/drm/imx/Makefile
# ---------------------------------------------------------
obj-$(CONFIG_IMX8MP_DSI_CROSSLINK_OLED) += imx8mp-dsi-crosslink-oled/
# ---------------------------------------------------------

nano drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/Makefile
# ---------------------------------------------------------
imx8mp-dsi-crosslink-oled-objs := imx8mp-dsi-crosslink-oled-main.o imx8mp-dsi-crosslink-oled-i2c.o
obj-$(CONFIG_IMX8MP_DSI_CROSSLINK_OLED) += imx8mp-dsi-crosslink-oled.o
# ---------------------------------------------------------

nano drivers/gpu/drm/imx/Kconfig
# ---------------------------------------------------------
config IMX8MP_DSI_CROSSLINK_OLED
	tristate "I.MX 8M Plus DSI - Crosslink - OLED driver"
	default m
	depends on OF
	select DRM_MIPI_DSI
	select DRM_KMS_HELPER
	select DRM_PANEL
	help
	  I.MX 8M Plus DSI - Crosslink - OLED driver
# ---------------------------------------------------------

	# compile and deploy kernel
source /opt/ampliphy-vendor-xwayland/BSP-Yocto-NXP-i.MX8MP-PD23.1.0/environment-setup-cortexa53-crypto-phytec-linux
make -j16

	# copy Kernel to SD card
cp arch/arm64/boot/Image /media/$USER/boot/
cp arch/arm64/boot/dts/freescale/imx8mp-phyboard-pollux-rdk-rpmsg-dsi.dtb /media/$USER/boot/
sudo mkdir -p /media/$USER/root/lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd23.1.0/kernel/drivers/gpu/drm/imx/
sudo cp drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/imx8mp-dsi-crosslink-oled.ko /media/$USER/root/lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd23.1.0/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/
sync; umount /media/$USER/boot; umount /media/$USER/root

	# create directory on destination device
mkdir /lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd23.1.0/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled

	# upload module
scp drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/imx8mp-dsi-crosslink-oled.ko root@192.168.30.11:/lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd23.1.0/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/

	# upload Device Tree
scp arch/arm64/boot/dts/freescale/imx8mp-phyboard-pollux-rdk-rpmsg-dsi.dtb root@192.168.30.11:/boot

	# set Device Tree in U-boot
setenv fdt_file imx8mp-phyboard-pollux-rdk-rpmsg-dsi.dtb
saveenv
reset

	# Linux
depmod   # recreate modules dependency list
reboot
# kernel wczytuje driver automatycznie na starcie jeśli jest w Device Tree

# sysctl kernel.printk=7
# insmod /lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd23.1.0/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/imx8mp-dsi-crosslink-oled.ko
# rmmod imx8mp-dsi-crosslink-oled.ko
# modprobe imx8mp-dsi-crosslink-oled
# rmmod imx8mp-dsi-crosslink-oled.ko; insmod /lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd23.1.0/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/imx8mp-dsi-crosslink-oled.ko

# autorun module at boot
echo "imx_rpmsg_connector" >> /etc/modules-load.d/imx_rpmsg_connector.conf
cat /etc/modules-load.d/imx_rpmsg_connector.conf
depmod   # recreate modules dependency list
# rm /etc/modules-load.d/imx_rpmsg_connector.conf

# compile kernel module, upload to i.MX8 and reboot
make -j16 && scp drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/imx8mp-dsi-crosslink-oled.ko root@192.168.30.11:/lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd23.1.0/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/
# && ssh root@192.168.30.11 'reboot'

cat /proc/device-tree/soc\@0/bus\@32c00000/mipi_dsi\@32e60000/status
cat /proc/device-tree/soc\@0/bus\@32c00000/lcd-controller\@32e80000/status

# -----------------------------------------------------------------------------

# 2. Yocto

# extract Kernel files if folder ~/phyLinux/build/linux-imx/ not exists
devtool modify -x linux-imx linux-imx
bitbake phytec-qt6demo-image

mkdir -p ~/phyLinux/build/linux-imx/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/
cd <imx8mp-driver-dsi-crosslink-oled>
cp imx8mp-dsi-crosslink-oled-i2c.{c,h} ~/phyLinux/build/linux-imx/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/
cp imx8mp-dsi-crosslink-oled-main.c ~/phyLinux/build/linux-imx/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/
cd -

	# copy kernel module to ~/jarsulk-pco repository
#source_dir=~/phyLinux/build/linux-imx/drivers/gpu/drm/imx/
#??? backup_dir=~/jarsulk-pco/projects/SOD-5/Programs/a53/kernel_space/sod5_m7_kernel_driver/sod5_m7_kernel_driver/
#cp ${source_dir}_readme.txt ${source_dir}modules.order ${source_dir}Makefile ${source_dir}{main,parse_procfs_commands,procfs,rpmsg,sod_registers}.{c,h} $backup_dir

nano ~/phyLinux/build/linux-imx/arch/arm64/boot/dts/freescale/imx8mp-phyboard-pollux-rdk.dts
# ---------------------------------------------------------
# ...

&lcdif1 {
	status = "okay";
};

&mipi_dsi {
	status = "okay";

	panel-bridge@0 {
		compatible = "toshiba,tc358867xbg";
		reg = <0>;

		port {
			tc358867_in: endpoint {
				remote-endpoint = <&dsi_out>;
			};
		};

		display-timings {
			lcd {
				// all EDT displays - Leave clock at 25175000!!!
				clock-frequency = <25175000>;
				hactive = <320>;
				vactive = <240>;
				hback-porch = <38>;
				hfront-porch = <20>;
				vback-porch = <14>;
				vfront-porch = <4>;
				hsync-len = <30>;
				vsync-len = <4>;
				hsync-active = <0>;
				vsync-active = <0>;
				de-active = <1>;
				pixelclk-active = <0>;
			};
		};

	};

	port@1 {
		reg = <1>;

		dsi_out: endpoint {
			remote-endpoint = <&tc358867_in>;
		};
	};
};
# ---------------------------------------------------------

nano ~/phyLinux/build/linux-imx/drivers/gpu/drm/imx/Makefile
# ---------------------------------------------------------
obj-$(CONFIG_IMX8MP_DSI_CROSSLINK_OLED) += imx8mp-dsi-crosslink-oled/
# ---------------------------------------------------------

nano ~/phyLinux/build/linux-imx/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/Makefile
# ---------------------------------------------------------
imx8mp-dsi-crosslink-oled-objs := imx8mp-dsi-crosslink-oled-main.o imx8mp-dsi-crosslink-oled-i2c.o
obj-$(CONFIG_IMX8MP_DSI_CROSSLINK_OLED) += imx8mp-dsi-crosslink-oled.o
# ---------------------------------------------------------

nano ~/phyLinux/build/linux-imx/drivers/gpu/drm/imx/Kconfig
# ---------------------------------------------------------
config IMX8MP_DSI_CROSSLINK_OLED
	tristate "I.MX 8M Plus DSI - Crosslink - OLED driver"
	default m
	depends on OF
	select DRM_MIPI_DSI
	select DRM_KMS_HELPER
	select DRM_PANEL
	help
	  I.MX 8M Plus DSI - Crosslink - OLED driver
# ---------------------------------------------------------

	# add new driver to kernel configuration
bitbake linux-imx -c menuconfig
#> Device Drivers > Graphics support (?)

	# or
nano ~/phyLinux/build/tmp/work/phyboard_pollux_imx8mp_3-phytec-linux/linux-imx/5.15.71-r0.0/linux-imx-5.15.71/.config
# nano ~/phyLinux/build/linux-imx/.config.baseline
# nano ~/phyLinux/build/linux-imx/.config.new
# ---------------------------------------------------------
# Graphics support
# ...
CONFIG_IMX8MP_DSI_CROSSLINK_OLED=y
# ---------------------------------------------------------

	# compile and deploy kernel
bitbake linux-imx -c compile -f && bitbake linux-imx -c deploy

	# copy Kernel to SD card
#cp deploy/images/phyboard-pollux-imx8mp-3/Image /media/p2119/boot/
#sync; umount /media/$USER/boot; umount /media/$USER/root

ll tmp/work/phyboard_pollux_imx8mp_3-phytec-linux/linux-imx/5.15.71-r0.0/linux-imx-5.15.71/arch/arm64/boot/Image
ll tmp/work/phyboard_pollux_imx8mp_3-phytec-linux/linux-imx/5.15.71-r0.0/linux-imx-5.15.71/arch/arm64/boot/dts/freescale/imx8mp-phyboard-pollux-rdk.dtb
ll tmp/work/phyboard_pollux_imx8mp_3-phytec-linux/linux-imx/5.15.71-r0.0/image/lib/modules/5.15.71-bsp-yocto-nxp-i.mx8mp-pd23.1.0/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/imx8mp-dsi-crosslink-oled.ko

	# send output files to i.MX via SSH
# send to i.MX: Image
scp tmp/work/phyboard_pollux_imx8mp_3-phytec-linux/linux-imx/5.15.71-r0.0/linux-imx-5.15.71/arch/arm64/boot/Image root@192.168.3.11:/boot/

# send to i.MX: imx8mp-phyboard-pollux-rdk.dtb
scp tmp/work/phyboard_pollux_imx8mp_3-phytec-linux/linux-imx/5.15.71-r0.0/linux-imx-5.15.71/arch/arm64/boot/dts/freescale/imx8mp-phyboard-pollux-rdk.dtb root@192.168.3.11:/boot/

# send to i.MX: imx8mp-dsi-crosslink-oled.ko
scp tmp/work/phyboard_pollux_imx8mp_3-phytec-linux/linux-imx/5.15.71-r0.0/image/lib/modules/5.15.71-bsp-yocto-nxp-i.mx8mp-pd23.1.0/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/imx8mp-dsi-crosslink-oled.ko root@192.168.3.11:/lib/modules/5.15.71-bsp-yocto-nxp-i.mx8mp-pd23.1.0/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/

	# or write to SD card:
# write to SD card: Image
cp tmp/work/phyboard_pollux_imx8mp_3-phytec-linux/linux-imx/5.15.71-r0.0/linux-imx-5.15.71/arch/arm64/boot/Image /media/$USER/boot/

# write to SD card: imx8mp-phyboard-pollux-rdk.dtb
cp tmp/work/phyboard_pollux_imx8mp_3-phytec-linux/linux-imx/5.15.71-r0.0/linux-imx-5.15.71/arch/arm64/boot/dts/freescale/imx8mp-phyboard-pollux-rdk.dtb /media/$USER/boot/

# write to SD card: imx8mp-dsi-crosslink-oled.ko
sudo mkdir -p /media/$USER/root/lib/modules/5.15.71-bsp-yocto-nxp-i.mx8mp-pd23.1.0/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/
sudo cp tmp/work/phyboard_pollux_imx8mp_3-phytec-linux/linux-imx/5.15.71-r0.0/image/lib/modules/5.15.71-bsp-yocto-nxp-i.mx8mp-pd23.1.0/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/imx8mp-dsi-crosslink-oled.ko /media/$USER/root/lib/modules/5.15.71-bsp-yocto-nxp-i.mx8mp-pd23.1.0/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/

sync; umount /media/$USER/boot; umount /media/$USER/root

	# optional
depmod
modprobe imx8mp-dsi-crosslink-oled
modprobe -r imx8mp-dsi-crosslink-oled

	# U-boot
setenv fdt_file imx8mp-phyboard-pollux-rdk.dtb
saveenv
reset

dmesg | grep dsi_oled_driver
dmesg | grep dsi
