cd linux-imx-v5.15.71_2.2.2-phy/
mkdir -p drivers/gpu/drm/bridge/imx8mp-dsi-crosslink-oled/

# set git repositorium location
git_imx8mp_driver_dsi_crosslink_oled=~/git-driver-dsi-crosslink-oled
#git_imx8mp_driver_dsi_crosslink_oled=~/jarsulk-pco/projects/LT-22/Programs/A53/kernel_space/imx8mp-driver-dsi-crosslink-oled/

cp $git_imx8mp_driver_dsi_crosslink_oled/imx8mp-dsi-crosslink-oled-* drivers/gpu/drm/bridge/imx8mp-dsi-crosslink-oled/
cp $git_imx8mp_driver_dsi_crosslink_oled/Makefile drivers/gpu/drm/bridge/imx8mp-dsi-crosslink-oled/

echo "obj-y += imx8mp-dsi-crosslink-oled/" >> drivers/gpu/drm/bridge/Makefile

nano arch/arm64/boot/dts/freescale/imx8mp-phyboard-pollux-rdk.dts
# ---------------------------------------------------------
# ...
# ---------------------------------------------------------

nano drivers/gpu/drm/bridge/Kconfig
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


&mipi_dsi {
	status = "okay";
	compatible = "fsl,imx8mp-mipi-dsim", "pco,dsi_crosslink_oled_svga060";
	dsi-lanes = <4>;
	video-mode = <2>;

//	port@0 {
//		dsim_from_lcdif: endpoint {
//			remote-endpoint = <&lcdif_to_dsim>;
//		};

//	panel {
//		compatible = "pco,dsi_crosslink_oled_svga060";
//		//reg = <0>;
//		dsi-lanes = <4>;
//		video-mode = <2>;
//		status = "okay";
//	};

/*
	panel@0 {
		compatible = "pco,dsi_crosslink_oled_svga060";
		reg = <0>;
		dsi-lanes = <4>;
		video-mode = <2>;
		status = "okay";

		display-timings {
			lcd {
				clock-frequency = <40000000>;
				hactive = <800>;
				vactive = <600>;
				hback-porch = <88>;
				hfront-porch = <40>;
				vback-porch = <23>;
				vfront-porch = <1>;
				hsync-len = <128>;
				vsync-len = <4>;
				hsync-active = <0>;
				vsync-active = <0>;
				de-active = <1>;
				pixelclk-active = <0>;
			};
		};

		port {
		//port@0 {
			// reg = <0>;

		port {
			dsi_crosslink_oled_in: endpoint {
				remote-endpoint = <&dsi_crosslink_oled_out>;
			};
		};
	};

	port@1 {
		reg = <1>;

		dsi_crosslink_oled_out: endpoint {
			remote-endpoint = <&dsi_crosslink_oled_in>;
		};
	};
*/
};
