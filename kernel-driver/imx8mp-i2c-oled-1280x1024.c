// SPDX-License-Identifier: GPL-2.0

#include <linux/delay.h>
#include <linux/media-bus-format.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/regulator/consumer.h>

#define TAG "i2c_oled_driver: "

static bool send_i2c(struct i2c_client *client, u8 reg, u8 value)
{
	int result = 0;
	char buffer[2];
	buffer[0] = reg;
	buffer[1] = value;

	if (!client)
	{
		pr_err(TAG "Client = null");
		return false;
	}

	result = i2c_master_send(client, buffer, sizeof(buffer));
	if (result < 0)
	{
		pr_err(TAG "Failed to send I2C the data: 0x%.2x 0x%.2x\n", buffer[0], buffer[1]);
		return false;
	}

	pr_info(TAG "I2C data sent successfully: 0x%.2x 0x%.2x\n", buffer[0], buffer[1]);
	return true;
}

static int i2c_driver_write_oled_registers(struct i2c_client *client)
{
	pr_info(TAG "Write registers\n");

	send_i2c(client, 0x21, 0x93);  /// turn on screen, mirror H, mirror V
	send_i2c(client, 0x01, 0x0f);  /// external sync with DE, sync active high
	send_i2c(client, 0x32, 0xb0);  /// DAC control - temperature
	send_i2c(client, 0x34, 0x00);  /// Vcom 0x0070
	send_i2c(client, 0x35, 0x70);  /// Vcom 0x0070
	send_i2c(client, 0x36, 0x18);  /// reserved - use the temperature sensor
	send_i2c(client, 0x37, 0x32);  /// reserved - use the temperature sensor

	return 0;
}

static const struct of_device_id i2c_of_match[] = {
	{
		.compatible = "pco,i2c_oled_sxga060"
	},
	{}
};
MODULE_DEVICE_TABLE(of, i2c_of_match);

static struct i2c_device_id i2c_table[] = {
  { "i2c_oled_driver", 0 },
  {}
};
MODULE_DEVICE_TABLE(i2c, i2c_table);

static int i2c_oled_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	pr_info(TAG "probe %x\n", client->addr);
	i2c_driver_write_oled_registers(client);
	return 0;
}

static int i2c_oled_remove(struct i2c_client *client)
{
	pr_info(TAG "remove\n");
	return 0;
}

static struct i2c_driver i2c_oled_driver = {
	.driver = {
		.name = "i2c_oled_driver",
		.of_match_table = i2c_of_match,
	},
  .id_table = i2c_table,
	.probe = i2c_oled_probe,
	.remove = i2c_oled_remove,
};

module_i2c_driver(i2c_oled_driver);

MODULE_AUTHOR("jarsulk, p2119, PCO");
MODULE_DESCRIPTION("I.MX 8M Plus DSI - Crosslink - OLED SXGA060 1280x1024 driver");
MODULE_LICENSE("GPL");

/*

SXGA060 - 1280x1024
# I2C: 0x0e - left
# I2C: 0x0f - right

i2cdetect -y 3  #I2C4

# initialize left OLED
i2cset -y 3 0x0e 0x21 0x93  # turn on screen, mirror H, mirror V
i2cset -y 3 0x0e 0x01 0x0f  # external sync with DE, sync active high
i2cset -y 3 0x0e 0x32 0xb0  # DAC control - temperature
i2cset -y 3 0x0e 0x34 0x00  # Vcom 0x0070
i2cset -y 3 0x0e 0x35 0x70  # Vcom 0x0070
i2cset -y 3 0x0e 0x36 0x18  # reserved - use the temperature sensor
i2cset -y 3 0x0e 0x37 0x32  # reserved - use the temperature sensor

# initialize right OLED
i2cset -y 3 0x0f 0x21 0x93  # turn on screen, mirror H, mirror V
i2cset -y 3 0x0f 0x01 0x0f  # external sync with DE, sync active high
i2cset -y 3 0x0f 0x32 0xb0  # DAC control - temperature
i2cset -y 3 0x0f 0x34 0x00  # Vcom 0x0070
i2cset -y 3 0x0f 0x35 0x70  # Vcom 0x0070
i2cset -y 3 0x0f 0x36 0x18  # reserved - use the temperature sensor
i2cset -y 3 0x0f 0x37 0x32  # reserved - use the temperature sensor

# i2cset -y 3 0x0e 0x22 0x00  # margin left H
# i2cset -y 3 0x0e 0x23 0x02  # margin left L    (1284 - 1280) / 2 = 2
# i2cset -y 3 0x0e 0x24 0x00  # margin right H
# i2cset -y 3 0x0e 0x25 0x02  # margin right L   (1284 - 1280) / 2 = 2
# i2cset -y 3 0x0e 0x26 0x00  # margin top H
# i2cset -y 3 0x0e 0x27 0x02  # margin top L     (1028 - 1024) / 2 = 2
# i2cset -y 3 0x0e 0x28 0x00  # margin bottom H
# i2cset -y 3 0x0e 0x29 0x02  # margin bottom L  (1028 - 1024) / 2 = 2

# enable test screen - vertical color bar
i2cset -y 3 0x0e 0x06 0x01
i2cset -y 3 0x0f 0x06 0x01

# disable test
i2cset -y 3 0x0e 0x06 0x00
i2cset -y 3 0x0f 0x06 0x00

weston-terminal &
weston-flower &

*/