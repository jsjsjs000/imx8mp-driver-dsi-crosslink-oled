#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/i2c.h>

#define TAG "dsi_oled_i2c_driver: "
#define I2C_BUS 3                 // I2C4 (3 counting from 0)
#define OLED_ADDRESS_RIGHT 0x0e
#define OLED_ADDRESS_LEFT  0x0f

struct i2c_adapter *adapter;
struct i2c_client *client_l;
struct i2c_client *client_r;

struct i2c_board_info oled_info_l = {
	I2C_BOARD_INFO("oled", OLED_ADDRESS_LEFT)
};
struct i2c_board_info oled_info_r = {
	I2C_BOARD_INFO("oled", OLED_ADDRESS_RIGHT)
};

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

int i2c_driver_write_registers(struct i2c_client *client)
{
	i2c_driver_write_oled_registers(client_l);
	i2c_driver_write_oled_registers(client_r);
	return 0;
}

int i2c_driver_init(void)
{
	pr_err(TAG "initialize\n");

	adapter = i2c_get_adapter(I2C_BUS);
	if (!adapter)
	{
		pr_err(TAG "failed to get the i2c bus number\n");
		return -ENODEV;
	}

	client_l = i2c_new_client_device(adapter,  &oled_info_l);
	if (!client_l)
	{
		pr_err(TAG "Failed to create i2c client device\n");
		i2c_put_adapter(adapter);
		return -EINVAL;
	}

	client_r = i2c_new_client_device(adapter,  &oled_info_r);
	if (!client_r)
	{
		pr_err(TAG "Failed to create i2c client device\n");
		i2c_put_adapter(adapter);
		return -EINVAL;
	}

	return 0;
}

void i2c_driver_exit(void)
{
	if (client_l)
		i2c_unregister_device(client_l);

	if (client_r)
		i2c_unregister_device(client_r);

	i2c_put_adapter(adapter);

	pr_info(TAG "deinitialize\n");
}
