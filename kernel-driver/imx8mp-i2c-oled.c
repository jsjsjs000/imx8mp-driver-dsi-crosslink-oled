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

// #define I2C_BUS 3                 // I2C4 (3 counting from 0)
// #define OLED_ADDRESS_RIGHT 0x0e
// #define OLED_ADDRESS_LEFT  0x0f

// struct i2c_adapter *adapter;
// struct i2c_client *client_l;
// struct i2c_client *client_r;

// struct i2c_board_info oled_info_l = {
// 	I2C_BOARD_INFO("oled", OLED_ADDRESS_LEFT)
// };
// struct i2c_board_info oled_info_r = {
// 	I2C_BOARD_INFO("oled", OLED_ADDRESS_RIGHT)
// };

static void send_i2c(struct i2c_client *client, u8 reg, u8 value)
{
	int result = 0;
	char buffer[2];
	buffer[0] = reg;
	buffer[1] = value;

	if (!client)
	{
		pr_err(TAG "client = null");
		return;
	}

	result = i2c_master_send(client, buffer, sizeof(buffer));
	if (result < 0)
	{
		pr_err(TAG "Failed to send I2C the data\n");
		return;
	}

	pr_info(TAG "I2C data sent successfully: 0x%.2x 0x%.2x\n", buffer[0], buffer[1]);
}

static int i2c_driver_write_oled_registers(struct i2c_client *client)
{
	pr_info(TAG "write registers\n");

	send_i2c(client, 0x10, 0x00);  /// turn on screen
	send_i2c(client, 0x01, 0x34);  /// external sync with DE
	send_i2c(client, 0x02, 0x00);  /// sync active high
	send_i2c(client, 0x19, 0xb0);  /// Vcom level for temperature
	send_i2c(client, 0x20, 0x00);  /// start of gamma correction
	send_i2c(client, 0x21, 0x00);
	send_i2c(client, 0x22, 0x40);
	send_i2c(client, 0x23, 0x01);
	send_i2c(client, 0x24, 0x48);
	send_i2c(client, 0x25, 0x01);
	send_i2c(client, 0x26, 0x57);
	send_i2c(client, 0x27, 0x01);
	send_i2c(client, 0x28, 0x67);
	send_i2c(client, 0x29, 0x01);
	send_i2c(client, 0x2a, 0x74);
	send_i2c(client, 0x2b, 0x01);
	send_i2c(client, 0x2c, 0x81);
	send_i2c(client, 0x2d, 0x01);
	send_i2c(client, 0x2e, 0x8f);
	send_i2c(client, 0x2f, 0x01);
	send_i2c(client, 0x30, 0x9a);
	send_i2c(client, 0x31, 0x01);
	send_i2c(client, 0x32, 0xa7);
	send_i2c(client, 0x33, 0x01);
	send_i2c(client, 0x34, 0xb3);
	send_i2c(client, 0x35, 0x01);
	send_i2c(client, 0x36, 0xbf);
	send_i2c(client, 0x37, 0x01);
	send_i2c(client, 0x38, 0xcb);
	send_i2c(client, 0x39, 0x01);
	send_i2c(client, 0x3a, 0xd6);
	send_i2c(client, 0x3b, 0x01);
	send_i2c(client, 0x3c, 0xe0);
	send_i2c(client, 0x3d, 0x01);
	send_i2c(client, 0x3e, 0xea);
	send_i2c(client, 0x3f, 0x01);
	send_i2c(client, 0x40, 0x00);
	send_i2c(client, 0x41, 0x02);  /// end of gamma correction

	return 0;
}

static const struct of_device_id i2c_of_match[] = {
	{
		.compatible = "pco,i2c_oled_svga060"
	},
	{}
};
MODULE_DEVICE_TABLE(i2c, i2c_of_match);

static struct i2c_device_id i2c_table[] = {
  { "i2c_oled_driver", 0},
  {}
};

static int i2c_oled_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	pr_info(TAG "probe\n");
	// i2c_driver_write_oled_registers(client);
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
	.remove = i2c_oled_remove
};

static int __init i2c_oled_init(void)
{
	// int err;

	pr_info(TAG "initinalize\n");

	// if (i2c_driver_init() < 0)
	// {
	// 	pr_err("i2c_add_driver() failed\n");
	// 	return err;
	// }

	return 0;
}
module_init(i2c_oled_init);

static void __exit i2c_oled_exit(void)
{
	pr_info(TAG "deinitialize\n");
	// i2c_driver_exit();
}
module_exit(i2c_oled_exit);

MODULE_AUTHOR("jarsulk, PCO");
MODULE_DESCRIPTION("I.MX 8M Plus DSI - Crosslink - OLED driver");
MODULE_LICENSE("GPL");
