#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/sysfs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/irq.h>
#include <linux/of_gpio.h>
#include <asm/uaccess.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mutex.h>

#include <linux/input.h>
#include <linux/workqueue.h>

struct lamp_led_data {
	struct platform_device *pdev;
	int lamp_gpio;	//gpio13
	int led_gpio;		//gpio69
	int irq;
	int flag;

//	struct mutex data_lock;
};

struct lamp_led_data* data;
static struct input_dev *hongwai_dev;

void func(struct work_struct *work)
{
	printk("lamp irq bottom half enter\n");
	msleep(10);
	data->flag = gpio_get_value(data->lamp_gpio);

	input_report_key(hongwai_dev, KEY_1, !(data->flag));
	input_sync(hongwai_dev);
}

DECLARE_WORK(mywork, func);

static irqreturn_t lamp_interrupt_handler(int irq, void *ptr)
{
	printk("lamp irq top half enter\n"); 

	schedule_work(&mywork);

	return IRQ_HANDLED;
}
/*
static ssize_t lamp_show_value(struct device *dev,
                                  struct device_attribute* attr,char* buf)
{
	ssize_t lenth;
	data = dev_get_drvdata(dev);

	if (data->flag) {
		lenth = sprintf(buf, "%d\n", 0);
	} else {
		lenth = sprintf(buf, "%d\n", 1);
	}

    return lenth;
}
*/
static ssize_t lamp_store_value(struct device *dev,struct device_attribute* attr,
									const char *buf, size_t len)
{
	int ret;
	unsigned long v;
//    struct lamp_led_data* data = dev_get_drvdata(dev);

	ret = kstrtoul(buf, 10, &v);

    switch (v) {   
		case 0:
			gpio_set_value(data->led_gpio, 0); 
			break;
		case 1:
			gpio_set_value(data->led_gpio, 1); 
			break;
		default:
			return -EINVAL;
		}   

    return len;
}

//static DEVICE_ATTR(value, 0664, lamp_show_value, NULL);
static DEVICE_ATTR(value, 0664, NULL, lamp_store_value);

static int lamp_probe(struct platform_device *pdev)
{
	int result;
	struct device_node* node = pdev->dev.of_node;

	printk("lamp probe enter\n");

	data = devm_kzalloc(&pdev->dev, sizeof(data),GFP_KERNEL);
	if(!data){
		pr_err("%s kzalloc error\n",__FUNCTION__);
		return -ENOMEM;  
	}

	dev_set_drvdata(&pdev->dev,data);
	data->lamp_gpio = of_get_named_gpio(node,"thundersoft,lamp_gpio",0);
	if (!gpio_is_valid(data->lamp_gpio)) {
		pr_err("gpio not specified\n");
		goto err1;
	}else {
		result = gpio_request(data->lamp_gpio, "lamp_gpio");
		if(result<0){
			pr_err("Unable to request lamp gpio\n");
			goto err1;
		}else {
			printk("request gpio 13 success\n");
			gpio_direction_input(data->lamp_gpio);
		}
	}

	data->led_gpio = of_get_named_gpio(node,"thundersoft,led_gpio",0);
	if (!gpio_is_valid(data->led_gpio)) {
		pr_err("gpio not specified\n");
		goto err2;
	}else{
		result = gpio_request(data->led_gpio, "lamp_gpio");
		if(result<0){
				pr_err("Unable to request led gpio\n");
				goto err2;
		}else{
			gpio_direction_output(data->led_gpio, 1);
		}
	}

	data->irq = gpio_to_irq(data->lamp_gpio);
	result = request_irq(data->irq,lamp_interrupt_handler,
					IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING,"lamp_led_intr",data);
	if(result<0){
		pr_err("Unable to request irq\n");
		goto err2;
	}

	//input event
	hongwai_dev = input_allocate_device();
	if (!hongwai_dev) {
		printk(KERN_ERR "lamp_led.c: Not enough memory\n");
		goto err3;
	}
	hongwai_dev->evbit[0] = BIT_MASK(EV_KEY);
	hongwai_dev->keybit[BIT_WORD(KEY_1)] = BIT_MASK(KEY_1);
	if (input_register_device(hongwai_dev)) {
		printk(KERN_ERR "lamp_led.c: Failed to register device\n");
		goto err3;
	}

	result=sysfs_create_file(&pdev->dev.kobj,&dev_attr_value.attr);
	if (result < 0) {
		printk("sysfs create file failed\n");
		goto err3;
	}
	printk("lamp probe success\n");
	return 0;

err3:
	gpio_free(data->led_gpio);
err2:
	gpio_free(data->lamp_gpio);
err1:
	kfree(data);
	printk("lamp probe failed\n");
	return -EINVAL;
}

static int lamp_remove(struct platform_device *pdev)
{
	struct lamp_led_data* data = dev_get_drvdata(&pdev->dev);
	input_unregister_device(hongwai_dev);
	gpio_free(data->led_gpio);
	gpio_free(data->lamp_gpio);
	kfree(data);

	return 0;
}


static struct of_device_id lamp_match_table[] = {
	{ .compatible = "thundersoft,sound",},
	{ },
};



static struct platform_driver lamp_driver = {
	.probe = lamp_probe,
	.remove = lamp_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "lamp",
		.of_match_table = lamp_match_table,
	},
};


module_platform_driver(lamp_driver);
MODULE_AUTHOR("heql0703@thundersoft.com");
MODULE_LICENSE("GPL v2");
