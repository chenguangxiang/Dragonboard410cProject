/*
 *
 * xfm10411 driver.
 *
 * Copyright (c) 2010  Focal tech Ltd.
 * Copyright (c) 2012-2015, The Linux Foundation. All rights reserved.
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
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/firmware.h>
#include <linux/debugfs.h>
#include <linux/sensors.h>
#include <linux/wakelock.h>

#define KEY_PRESSED 1
#define KEY_RELEASE 0
#define XFM_MAX_RETRY_TIMES 5

struct xfm10411_platform_data {
	const char *name;
	u32 irq_gpio;
	u32 ldo_en;
	u32 lcd_avdd_en;
};

struct xfm10411_data {
	struct i2c_client *client;
	struct xfm10411_platform_data *pdata;
	/* pinctrl data*/
	struct pinctrl *pinctrl;
	struct pinctrl_state *pin_default;
	struct pinctrl_state *pin_sleep;
	struct input_dev *input_dev;
	struct delayed_work wake_work;
};

static struct xfm10411_data *xfmdata;

static int xfm10411_i2c_read(struct i2c_client *client, char *writebuf,
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

static int xfm10411_i2c_write(struct i2c_client *client, char *writebuf,
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

static void wake_work_func(struct work_struct *work)
{
    struct xfm10411_data *data = container_of(work, struct xfm10411_data, wake_work.work);
    int state = -1;

  state = gpio_get_value(data->pdata->irq_gpio);
  if(!state){
    pr_info("%s : state error\n", __func__);
    return;
  }
  pr_info("%s : enter\n", __func__);
	input_report_key(data->input_dev, KEY_POWER, KEY_PRESSED);
	input_sync(data->input_dev);
	msleep(5);
	input_report_key(data->input_dev, KEY_POWER, KEY_RELEASE);
	input_sync(data->input_dev);
}

static irqreturn_t power_wake_irq(int irq, void *_data)
{
  struct xfm10411_data *data= (struct xfm10411_data *)_data;

	if (!data) {
		pr_err("%s: Invalid data\n", __func__);
		return IRQ_HANDLED;
	}

	schedule_delayed_work(&data->wake_work, msecs_to_jiffies(10));

    return IRQ_HANDLED;
}

static int xfm10411_pinctrl_init(struct xfm10411_data *data)
{
	struct i2c_client *client = data->client;

	data->pinctrl = devm_pinctrl_get(&client->dev);
	if (IS_ERR_OR_NULL(data->pinctrl)) {
		dev_err(&client->dev, "Failed to get pinctrl\n");
		return PTR_ERR(data->pinctrl);
	}

	data->pin_default =
		pinctrl_lookup_state(data->pinctrl, "default");
	if (IS_ERR_OR_NULL(data->pin_default)) {
		dev_err(&client->dev, "Failed to look up default state\n");
		return PTR_ERR(data->pin_default);
	}

	data->pin_sleep =
		pinctrl_lookup_state(data->pinctrl, "sleep");
	if (IS_ERR_OR_NULL(data->pin_sleep)) {
		dev_err(&client->dev, "Failed to look up sleep state\n");
		return PTR_ERR(data->pin_sleep);
	}

	return 0;
}

static int xfm10411_parse_dt(struct device *dev,
			struct xfm10411_platform_data *pdata)
{
	struct device_node *np = dev->of_node;

    pdata->name = "xfm10411";
	pdata->irq_gpio = of_get_named_gpio(np, "xfm,gpio-int", 0);
	if (pdata->irq_gpio < 0)
		return pdata->irq_gpio;
	pdata->ldo_en = of_get_named_gpio(np, "xfm,gpio-ldo-en", 0);
	if (pdata->ldo_en < 0)
		return pdata->ldo_en;
	pdata->lcd_avdd_en = of_get_named_gpio(np, "xfm,gpio-lcd-avdd", 0);
	if (pdata->lcd_avdd_en < 0)
		return pdata->lcd_avdd_en;

	return 0;
}

static ssize_t xfm10411_angle_show(struct class *class, struct class_attribute *attr,
			char *buf)
{
	struct xfm10411_data *data = xfmdata;
	u8 w_buf[5] = {0x00, 0x00, 0x10, 0x00, 0x00};
	u8 r_buf[4] = {0x00, 0x00, 0x00, 0x00};
	int ret = 0;
	int n_try = XFM_MAX_RETRY_TIMES;

    ret = xfm10411_i2c_write(data->client, w_buf, sizeof(buf));
	if(ret < 0){
        pr_err("%s : i2c write failed, ret = %d\n", __func__, ret);
		return -EINVAL;
	}

	do{
		msleep(20);
		ret = xfm10411_i2c_read(data->client, &w_buf[0], 1, r_buf, sizeof(r_buf));
		if(ret < 0){
			pr_err("%s : i2c read failed, ret = %d\n", __func__, ret);
			return -EINVAL;
		}
		else if(r_buf[0] == 0x01 && r_buf[1] == 0x0 && r_buf[2] == 0x01 && r_buf[3] == 0x0){
			w_buf[0] = 0x01;
			ret = xfm10411_i2c_read(data->client, &w_buf[0], 1, r_buf, sizeof(r_buf));
			if(ret < 0){
				pr_err("%s : i2c read failed, ret = %d\n", __func__, ret);
				return -EINVAL;
			}

			return scnprintf(buf, PAGE_SIZE, "%d\n", r_buf[0]);
		}else{
		    pr_info("%s : not readly, n_try = %d\n", __func__, n_try);
		}
	}while(n_try--);
	pr_err("%s : failed\n", __func__);

	return -EINVAL;
}

static ssize_t xfm10411_version_show(struct class *class, struct class_attribute *attr,
			char *buf)
{
	struct xfm10411_data *data = xfmdata;
	u8 w_buf[5] = {0x00, 0x00, 0x0f, 0x00, 0x00};
	u8 r_buf[4] = {0x00, 0x00, 0x00, 0x00};
	int ret = 0;
	int n_try = XFM_MAX_RETRY_TIMES;

    ret = xfm10411_i2c_write(data->client, w_buf, sizeof(w_buf));
	if(ret < 0){
        pr_err("%s : i2c write failed, ret = %d\n", __func__, ret);
	    return -EINVAL;
	}

	do{
		msleep(20);
		ret = xfm10411_i2c_read(data->client, &w_buf[0], 1, r_buf, sizeof(r_buf));
		if(ret < 0){
			pr_err("%s : i2c read failed, ret = %d\n", __func__, ret);
			return -EINVAL;
		}
		else if(r_buf[0] == 0x01 && r_buf[1] == 0x0 && r_buf[2] == 0x02 && r_buf[3] == 0x0){
			w_buf[0] = 0x01;
			ret = xfm10411_i2c_read(data->client, &w_buf[0], 1, r_buf, sizeof(r_buf));
			if(ret < 0){
				pr_err("%s : i2c read failed, ret = %d\n", __func__, ret);
				return -EINVAL;
			}

			return scnprintf(buf, PAGE_SIZE, "%d %d %d %d\n",
			    r_buf[0], r_buf[1], r_buf[2], r_buf[3]);
		}else{
		    pr_info("%s : not readly, n_try = %d\n", __func__, n_try);
		}
	}while(n_try--);

	return -EINVAL;

}

static ssize_t xfm10411_wake_store(struct class *class,
				struct class_attribute *attr,
				const char *buf, size_t len)
{
	struct xfm10411_data *data = xfmdata;
	u8 w_buf[5] = {0x00, 0x00, 0x11, 0x00, 0x00};
	u8 r_buf[4] = {0x00, 0x00, 0x00, 0x00};
	unsigned long value = 0;
	int ret;
	int n_try = XFM_MAX_RETRY_TIMES;

	ret = kstrtoul(buf, 16, &value);
	if (ret < 0) {
		pr_err("%s:kstrtoul failed, ret=0x%x\n",
			__func__, ret);
		return ret;
	}

	if(value == 1){
		ret = xfm10411_i2c_write(data->client, w_buf, sizeof(w_buf));
		if(ret < 0){
            pr_err("%s : i2c write failed, ret = %d\n", __func__, ret);
			return -EINVAL;
		}

		do{
			msleep(20);
			ret = xfm10411_i2c_read(data->client, &w_buf[0], 1, r_buf, sizeof(r_buf));
			if(ret < 0){
			    pr_err("%s : i2c write failed, ret = %d\n", __func__, ret);
			    return -EINVAL;
			}else if(r_buf[0] == 0x01 && r_buf[1] == 0x0 && r_buf[2] == 0x0 && r_buf[3] == 0x0){
			    return len;
			}else{
			    pr_info("%s : not readly, n_try = %d\n", __func__, n_try);
			}
		}while(n_try--);

		pr_err("%s : enable wake failed\n", __func__);
	}
	else
		pr_err("%s : cmd not support, value = %ld\n", __func__, value);

	return -EINVAL;
}

static ssize_t xfm10411_enable_store(struct class *class,
				struct class_attribute *attr,
				const char *buf, size_t len)
{
	struct xfm10411_data *data = xfmdata;
	u8 w_buf[5] = {0x00, 0x00, 0x12, 0x00, 0x00};
	u8 r_buf[4] = {0x00, 0x00, 0x00, 0x00};
	unsigned long value = 0;
	int n_try = XFM_MAX_RETRY_TIMES;
	int ret;

	ret = kstrtoul(buf, 16, &value);
	if (ret < 0) {
		pr_err("%s:kstrtoul failed, ret=0x%x\n",
			__func__, ret);
		return ret;
	}

	if(value == 1){
		ret = xfm10411_i2c_write(data->client, w_buf, sizeof(w_buf));
		if(ret < 0){
			pr_err("%s : i2c write failed, ret = %d\n", __func__, ret);
			return -EINVAL;
		}

		do{
			msleep(20);
			ret = xfm10411_i2c_read(data->client, &w_buf[0], 1, r_buf, sizeof(r_buf));
			if(ret < 0){
			    pr_err("%s : i2c write failed, ret = %d\n", __func__, ret);
			    return -EINVAL;
			}else if(r_buf[0] == 0x01 && r_buf[1] == 0x0 && r_buf[2] == 0x0 && r_buf[3] == 0x0){
			    return len;
			}else{
			    pr_info("%s : not readly, n_try = %d\n", __func__, n_try);
			}
		}while(n_try--);

		pr_err("%s : enable output failed\n", __func__);
	}
	else
		pr_err("%s : cmd not support, value = %ld\n", __func__, value);

	return -EINVAL;

}


static struct class_attribute xfm10411_class_attrs[] = {
	__ATTR(angle, 0444, xfm10411_angle_show, NULL),
	__ATTR(version, 0444, xfm10411_version_show, NULL),
	__ATTR(wake, 0222, NULL, xfm10411_wake_store),
	__ATTR(enable, 0222, NULL, xfm10411_enable_store),
	__ATTR_NULL,
};

static struct class xfm10411_class = {
	.name =		"xfm10411",
	.owner =	THIS_MODULE,
	.class_attrs =	xfm10411_class_attrs,
};

static void xfm10411_gpio_init(struct xfm10411_data *data)
{
    int err = 0;

	err = gpio_request(data->pdata->ldo_en, "xfm ldo en");
	if(err){
        pr_err("%s : gpio ldo request failed\n", __func__);
		return;
	}
	gpio_direction_output(data->pdata->ldo_en, 1);

	err = gpio_request(data->pdata->irq_gpio, "xfm irq");
	if(err){
        pr_err("%s : gpio ldo request failed\n", __func__);
		return;
	}
	if(!IS_ERR_OR_NULL(data->pinctrl)){
		err = pinctrl_select_state(data->pinctrl, data->pin_default);
		if (err){
			dev_err(&data->client->dev, "Can't select pinctrl state default\n");
		}
	}
	gpio_direction_input(data->pdata->irq_gpio);

	/*err = gpio_request(data->pdata->lcd_avdd_en, "xfm lcd avdd en");
	if(err){
        pr_err("%s : gpio lcd_avdd_en request failed\n", __func__);
		return;
	}
	gpio_direction_output(data->pdata->lcd_avdd_en, 1);*/
}

static int xfm10411_probe(struct i2c_client *client,
			   const struct i2c_device_id *id)
{
	struct xfm10411_platform_data *pdata;
	struct xfm10411_data *data;
	struct input_dev *input_dev;
	int err;
    int wake_irq;

    printk("%s : enter\n", __func__);
	if (client->dev.of_node) {
		pdata = devm_kzalloc(&client->dev,
			sizeof(struct xfm10411_platform_data), GFP_KERNEL);
		if (!pdata) {
			dev_err(&client->dev, "Failed to allocate memory\n");
			return -ENOMEM;
		}

		err = xfm10411_parse_dt(&client->dev, pdata);
		if (err) {
			dev_err(&client->dev, "DT parsing failed\n");
			return err;
		}
	} else
		pdata = client->dev.platform_data;

	if (!pdata) {
		dev_err(&client->dev, "Invalid pdata\n");
		return -EINVAL;
	}

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev, "I2C not supported\n");
		return -ENODEV;
	}

	data = devm_kzalloc(&client->dev,
			sizeof(struct xfm10411_data), GFP_KERNEL);
	if (!data) {
		dev_err(&client->dev, "Not enough memory\n");
		return -ENOMEM;
	}

	input_dev = input_allocate_device();
	if (!input_dev) {
		dev_err(&client->dev, "failed to allocate input device\n");
		return -ENOMEM;
	}

	data->input_dev = input_dev;
	data->client = client;
	data->pdata = pdata;
	input_dev->name = "xfm10411";
	input_dev->id.bustype = BUS_I2C;
	input_dev->dev.parent = &client->dev;
	xfmdata = data;

	input_set_drvdata(input_dev, data);
	i2c_set_clientdata(client, data);
	xfm10411_pinctrl_init(data);
	xfm10411_gpio_init(data);

	input_set_capability(input_dev, EV_KEY, KEY_POWER);

	err = input_register_device(input_dev);
	if (err) {
		dev_err(&client->dev, "Input device registration failed\n");
		input_free_device(input_dev);
		return err;
	}

    INIT_DELAYED_WORK(&data->wake_work, wake_work_func);
    wake_irq = gpio_to_irq(pdata->irq_gpio);
    if (wake_irq) {
          err = request_irq(wake_irq,
                   power_wake_irq,
                   IRQF_TRIGGER_RISING | IRQF_ONESHOT,
                   "wake", data);
        if (err) {
           dev_err(&client->dev, "request irq failed for WAKE\n");
        }
    } else {
        dev_err(&client->dev, "WAKE IRQ doesn't exist\n");
    }
    enable_irq_wake(wake_irq);

    err = class_register(&xfm10411_class);
	if (err < 0){
		dev_err(&client->dev, "%s : register xfm10411_class failed\n", __func__);
		return err;
	}
	printk("%s : exit\n", __func__);

	return 0;
}

static int xfm10411_remove(struct i2c_client *client)
{
	struct xfm10411_data *data = i2c_get_clientdata(client);

	pr_info("%s : enter\n", __func__);
	if(gpio_is_valid(data->pdata->ldo_en))
		gpio_free(data->pdata->ldo_en);

	class_unregister(&xfm10411_class);
	free_irq(gpio_to_irq(data->pdata->irq_gpio), data);
	if(gpio_is_valid(data->pdata->irq_gpio))
		gpio_free(data->pdata->irq_gpio);
	input_unregister_device(data->input_dev);
	kfree(data->pdata);
	kfree(data);

	return 0;
}

static const struct i2c_device_id xfm10411_id[] = {
	{"xfm10411", 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, xfm10411_id);

#ifdef CONFIG_OF
static struct of_device_id xfm10411_match_table[] = {
	{ .compatible = "xfm,10411",},
	{ },
};
#else
#define xfm10411_match_table NULL
#endif

static struct i2c_driver xfm10411_driver = {
	.probe = xfm10411_probe,
	.remove = xfm10411_remove,
	.driver = {
		   .name = "xfm10411",
		   .owner = THIS_MODULE,
		   .of_match_table = xfm10411_match_table,
		   },
	.id_table = xfm10411_id,
};

static int __init xfm10411_init(void)
{
    printk("%s\n", __func__);
	return i2c_add_driver(&xfm10411_driver);
}
module_init(xfm10411_init);

static void __exit xfm10411_exit(void)
{
	i2c_del_driver(&xfm10411_driver);
}
module_exit(xfm10411_exit);

MODULE_DESCRIPTION("xfm10411 driver");
MODULE_LICENSE("GPL v2");
