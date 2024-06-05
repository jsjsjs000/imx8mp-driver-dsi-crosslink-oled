	# set variables
kernel_dir=~/linux-imx-phytec-v5.10.72_2.2.0-phy9/drivers/gpu/drm/imx/imx8mp-dsi-crosslink-oled/
jarsulk_dir=~/jarsulk-pco/projects/LT-22/Programs/A53/kernel_driver/imx8mp-dsi-crosslink-oled/

	# copy kernel module to ~/jarsulk-pco repository
cp ${kernel_dir}README.txt ${kernel_dir}modules.order ${kernel_dir}Makefile ${kernel_dir}imx8mp-dsi-crosslink-oled-i2c.{c,h} ${kernel_dir}imx8mp-dsi-crosslink-oled-main.c $jarsulk_dir

	# copy ~/jarsulk-pco repository to kernel module
cp ${jarsulk_dir}README.txt ${jarsulk_dir}modules.order ${jarsulk_dir}Makefile ${jarsulk_dir}imx8mp-dsi-crosslink-oled-i2c.{c,h} ${jarsulk_dir}imx8mp-dsi-crosslink-oled-main.c $kernel_dir
