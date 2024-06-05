// #include <linux/i2c.h>
// #include <linux/module.h>

// struct device_i2c_oled {
// 	char name[32];
// 	int i;
// };

// static struct device_i2c_oled device_i2c_oled_right = {
// 	"DSI I2C OLED1",
// 	0x0e
// };

// static struct device_i2c_oled device_i2c_oled_left = {
// 	"DSI I2C OLED1",
// 	0x0f
// };

// static const struct i2c_device_id i2c_driver_id_table[] = {
// 	{ "dsi_i2c_driver", 0 },
// 	// { "i2c-dsi-oled-r", (long unsigned int)&device_i2c_oled_right },
// 	// { "i2c-dsi-oled-l", (long unsigned int)&device_i2c_oled_left },
// 	{ }
// };

// MODULE_DEVICE_TABLE(i2c, i2c_driver_id_table);

// static struct i2c_client *i2c_client_instance;

// int dsi_crosslink_i2c(void)
// {
// 	int err;
// 	char data[1] = { 0x10 };
// 	pr_info("  DSI I2C writing...\n");
// 	//err = write8_reg(i2c_client_instance, 0x10, data, 1);
// 	err = i2c_master_send(i2c_client_instance, data, 1);
// 	if (err < 0)
// 	{
// 		pr_err("  DSI I2C write error\n");
// 		return err;
// 	}

// 	return 0;
// }

// static int i2c_driver_probe(struct i2c_client *client, const struct i2c_device_id *id)
// {
// 	pr_info("  DSI I2C probe\n");
// 	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
// 	{
// 		// dev_err(client->dev, "DSI I2C failed i2c_check_functionality\n");
// 		pr_err("DSI I2C failed i2c_check_functionality\n");
// 		return -ENODEV;
// 	}

// 	i2c_client_instance = client;

// 	dsi_crosslink_i2c();

// 	return 0;
// }

// static int i2c_driver_remove(struct i2c_client *client)
// {
// 	pr_info("  DSI I2C remove\n");
// 	i2c_client_instance = NULL;
// 	return 0;
// }

// static struct i2c_driver i2c_driver_instance = {
// 	.driver = {
// 		.name = "dsi_i2c_driver"
// 	},
// 	.id_table = i2c_driver_id_table,
// 	.probe = i2c_driver_probe,
// 	.remove = i2c_driver_remove
// };

// int i2c_driver_init(void)
// {
// pr_err("  DSI I2C init\n");
// 	return i2c_add_driver(&i2c_driver_instance);
// }

// void i2c_driver_exit(void)
// {
// pr_err("  DSI I2C deinit\n");
// 	i2c_del_driver(&i2c_driver_instance);
// }
