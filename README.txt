	# set variables
kernel_dir=~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/
jarsulk_dir=~/jarsulk-pco/projects/LT-22/Programs/A53/kernel_space/imx8mp-dsi-crosslink-oled/

	# copy kernel module to ~/jarsulk-pco repository
cp ${kernel_dir}README.txt ${kernel_dir}modules.order ${kernel_dir}Makefile ${kernel_dir}imx8mp-dsi-crosslink-oled-i2c.{c,h} ${kernel_dir}imx8mp-dsi-crosslink-oled-main.c $jarsulk_dir

	# copy ~/jarsulk-pco repository to kernel module
cp ${jarsulk_dir}README.txt ${jarsulk_dir}modules.order ${jarsulk_dir}Makefile ${jarsulk_dir}imx8mp-dsi-crosslink-oled-i2c.{c,h} ${jarsulk_dir}imx8mp-dsi-crosslink-oled-main.c $kernel_dir


	# add new driver to kernel configuration
nano ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/.config
# ---------------------------------------------------------
# Graphics support
# ...
CONFIG_IMX8MP_DSI_CROSSLINK_OLED=y
# ---------------------------------------------------------

nano ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/arch/arm64/boot/dts/freescale/Makefile
# ---------------------------------------------------------
dtb-$(CONFIG_ARCH_MXC) += imx8mp-phyboard-pollux-rdk-rpmsg-dsi.dtb
# ---------------------------------------------------------

nano ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/arch/arm64/boot/dts/freescale/imx8mp-phyboard-pollux-rdk-rpmsg-dsi.dts
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

nano ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/gpu/drm/imx/Makefile
# ---------------------------------------------------------
obj-$(CONFIG_IMX8MP_DSI_CROSSLINK_OLED) += imx8mp-dsi-crosslink-oled/
# ---------------------------------------------------------

nano ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/Makefile
# ---------------------------------------------------------
imx8mp-dsi-crosslink-oled-objs := imx8mp-dsi-crosslink-oled-main.o imx8mp-dsi-crosslink-oled-i2c.o
obj-$(CONFIG_IMX8MP_DSI_CROSSLINK_OLED) += imx8mp-dsi-crosslink-oled.o
# ---------------------------------------------------------

nano ~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/gpu/drm/imx/Kconfig
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
make -j16

	# copy Kernel to SD card
cp arch/arm64/boot/Image /media/$USER/boot/
cp arch/arm64/boot/dts/freescale/imx8mp-phyboard-pollux-rdk-rpmsg-dsi.dtb /media/$USER/boot/
sudo mkdir -p /media/$USER/root/lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/gpu/drm/imx/
sudo cp drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/imx8mp-dsi-crosslink-oled.ko /media/$USER/root/lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/
sync; umount /media/$USER/boot; umount /media/$USER/root

	# create directory on destination device
mkdir /lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled

	# upload module
scp drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/imx8mp-dsi-crosslink-oled.ko root@192.168.30.11:/lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/

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
# insmod /lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/imx8mp-dsi-crosslink-oled.ko
# rmmod imx8mp-dsi-crosslink-oled.ko
# modprobe imx8mp-dsi-crosslink-oled
# rmmod imx8mp-dsi-crosslink-oled.ko; insmod /lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/imx8mp-dsi-crosslink-oled.ko

# autorun module at boot
echo "imx_rpmsg_connector" >> /etc/modules-load.d/imx_rpmsg_connector.conf
cat /etc/modules-load.d/imx_rpmsg_connector.conf
depmod   # recreate modules dependency list
# rm /etc/modules-load.d/imx_rpmsg_connector.conf

# compile kernel module, upload to i.MX8 and reboot
make -j16 && scp drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/imx8mp-dsi-crosslink-oled.ko root@192.168.30.11:/lib/modules/5.10.72-bsp-yocto-nxp-i.mx8mp-pd22.1.1/kernel/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/
# && ssh root@192.168.30.11 'reboot'

cat /proc/device-tree/soc\@0/bus\@32c00000/mipi_dsi\@32e60000/status
