/*
 *
 * Himax HX7097 panel driver.
 *
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio/consumer.h>

static int hx7097_i2c_read(struct i2c_client *client, char *writebuf,
			   int writelen, char *readbuf, int readlen)
{
	int ret;

	if (writelen > 0) {
		struct i2c_msg msgs[] = {
			{
				 .addr = client->addr,
				 .flags = 0,
				 .len = writelen,
				 .buf = writebuf,
			 },
			{
				 .addr = client->addr,
				 .flags = I2C_M_RD,
				 .len = readlen,
				 .buf = readbuf,
			 },
		};
		ret = i2c_transfer(client->adapter, msgs, 2);
		if (ret < 0)
			dev_err(&client->dev, "%s: i2c read error.\n",
				__func__);
	} else {
		struct i2c_msg msgs[] = {
			{
				 .addr = client->addr,
				 .flags = I2C_M_RD,
				 .len = readlen,
				 .buf = readbuf,
			 },
		};
		ret = i2c_transfer(client->adapter, msgs, 1);
		if (ret < 0)
			dev_err(&client->dev, "%s:i2c read error.\n", __func__);
	}
	return ret;
}

static int hx7097_i2c_write(struct i2c_client *client, char *writebuf,
			    int writelen)
{
	int ret;

	struct i2c_msg msgs[] = {
		{
			 .addr = client->addr,
			 .flags = 0,
			 .len = writelen,
			 .buf = writebuf,
		 },
	};
	ret = i2c_transfer(client->adapter, msgs, 1);
	if (ret < 0)
		dev_err(&client->dev, "%s: i2c write error.\n", __func__);

	return ret;
}

static int hx7097_write_reg(struct i2c_client *client, u8 addr, const u8 val)
{
	u8 buf[2] = {0};

	buf[0] = addr;
	buf[1] = val;
	
	return hx7097_i2c_write(client, buf, sizeof(buf));
}

static int hx7097_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
	struct property *prop;
	int err;
	int i,size;
	struct device_node *np = client->dev.of_node;
	int addresses[100];
	int values[100];
	char address,value;

	prop = of_find_property(np, "hx7097,addresses", NULL);
	if (!prop)
		return -EINVAL;
	if (!prop->value)
		return -ENODATA;

	size = prop->length / sizeof(int);

	err = of_property_read_u32_array(np, "hx7097,addresses", addresses, size);
	if (err && (err != -EINVAL)) {
		dev_err(&client->dev, "Unable to read 'hx7097,addresses'\n");
		return err;
	}
	
	prop = of_find_property(np, "hx7097,values", NULL);
	if (!prop)
		return -EINVAL;
	if (!prop->value)
		return -ENODATA;

	i = prop->length / sizeof(u32);
	if (i != size) {
		dev_err(&client->dev, "invalid 'hx7097,values' length should be same as addresses\n");
		return -EINVAL;
	}

	err = of_property_read_u32_array(np, "hx7097,values", values, i);
	if (err && (err != -EINVAL)) {
		dev_err(&client->dev, "Unable to read 'hx7097,values'\n");
		return err;
	}
	
	for(i=0;i<size;i++)
	{
		hx7097_write_reg(client, addresses[i],values[i]);
		if (err < 0)
		{
			dev_err(&client->dev, "fail to write data to the chip\n");
			return err;
		}
	}
	return 0;
}

static int hx7097_remove(struct i2c_client *client)
{
	struct gpio_desc *enable_gpio;
	
	enable_gpio=devm_gpiod_get_optional(&client->dev, "enable",GPIOD_OUT_LOW);
	if (enable_gpio)
		gpiod_set_value_cansleep(enable_gpio, 0);
	return 0;
}

static const struct i2c_device_id hx7097_id[] = {
	{"hx7097", 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, hx7097_id);

#ifdef CONFIG_OF
static struct of_device_id hx7097_match_table[] = {
	{ .compatible = "himax,hx7097",},
	{ },
};
#else
#define hx7097_match_table NULL
#endif

static struct i2c_driver hx7097_driver = {
	.probe = hx7097_probe,
	.remove = hx7097_remove,
	.driver = {
		.name = "hx7097",
		.owner = THIS_MODULE,
		.of_match_table = hx7097_match_table,
	},
	.id_table = hx7097_id,
};

static int __init hx7097_init(void)
{
    printk(KERN_INFO "loading hx7097 i2c driver\n");
	return i2c_add_driver(&hx7097_driver);
}
module_init(hx7097_init);

static void __exit hx7097_exit(void)
{
	i2c_del_driver(&hx7097_driver);
}
module_exit(hx7097_exit);

MODULE_DESCRIPTION("Himax HX7097 panel i2c driver");
MODULE_LICENSE("GPL v2");