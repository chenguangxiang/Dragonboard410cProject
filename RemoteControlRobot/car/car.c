#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
//#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/sysfs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/input.h>
//#include <linux/jiffies.h>
#include <linux/mutex.h>
#include <linux/ctype.h>
#include <linux/pm_runtime.h>
#include <linux/device.h>
#include <linux/irq.h>
#include <linux/of_gpio.h>

#include <linux/wait.h>
#include <asm/uaccess.h> 

struct car_data{
	int pin[4];	 
	int d;
    struct mutex data_lock;
};

static int parse_dt(struct platform_device* pdev,struct car_data* data){
	int rc;
	struct device_node* node = pdev->dev.of_node;
	
	printk("parse begin-------\n");
	data->pin[0] = of_get_named_gpio(node,"thunder,gpio_in1",0);
	if(gpio_is_valid(data->pin[0])){
		rc = gpio_request(data->pin[0],"in1");
		if(rc < 0){
			pr_err("Unable to request in1 gpio\n");
			return -1;
		}
			gpio_direction_output(data->pin[0], 0);
		}else{
			pr_err("in1 gpio has been used\n");
			return -1;
		}

	data->pin[1] = of_get_named_gpio(node,"thunder,gpio_in2",0);
	if(gpio_is_valid(data->pin[1])){
		rc = gpio_request(data->pin[1],"in2");
		if(rc < 0){
			pr_err("Unable to request in2 gpio\n");
			return -1;
		}
			gpio_direction_output(data->pin[1], 0);
		}else{
			pr_err("in2 gpio has been used\n");
			return -1;
		}

	data->pin[2] = of_get_named_gpio(node,"thunder,gpio_in3",0);
	if(gpio_is_valid(data->pin[2])){
		rc = gpio_request(data->pin[2],"in3");
		if(rc < 0){
			pr_err("Unable to request in3 gpio\n");
			return -1;
		}
			gpio_direction_output(data->pin[2], 0);
		}else{
			pr_err("in3 gpio has been used\n");
			return -1;
		}

	data->pin[3] = of_get_named_gpio(node,"thunder,gpio_in4",0);
	if(gpio_is_valid(data->pin[3])){
		rc = gpio_request(data->pin[3],"in4");
		if(rc < 0){
			pr_err("Unable to request in4 gpio\n");
			return -1;
		}
			gpio_direction_output(data->pin[3], 0);
		}else{
			pr_err("in4 gpio has been used\n");
			return -1;
		}
		data->d = 0;

	printk("parse end-------\n");
	return 0;
}

static ssize_t car_store_value(struct device *dev,                                                                                                                                                  
                struct device_attribute *attr,
                const char *buf, size_t size){
	int i;
	int n = 0;
    struct car_data* data = dev_get_drvdata(dev);
	for(i = 0; buf[i] >= '0'&& buf[i] <= '9'; ++i)
	{
		n = 10*n + (buf[i] - '0');
	}
	printk("car_store_value begin-------%d\n", n);

	switch(n)
	{
		printk("buf--------%d\n", n);
		//go forward
		case 5: 
			gpio_set_value(data->pin[0], 0);
			gpio_set_value(data->pin[1], 1);
			gpio_set_value(data->pin[2], 0);
			gpio_set_value(data->pin[3], 1);
			break;

		//draw back	
		case 10: 
			gpio_set_value(data->pin[0], 1);
			gpio_set_value(data->pin[1], 0);
			gpio_set_value(data->pin[2], 1);
			gpio_set_value(data->pin[3], 0);
			break;

		//turn left
		case 9: 
			gpio_set_value(data->pin[0], 1);
			gpio_set_value(data->pin[1], 0);
			gpio_set_value(data->pin[2], 0);
			gpio_set_value(data->pin[3], 1);
			break;

		//turn right
		case 6: 
			gpio_set_value(data->pin[0], 0);
			gpio_set_value(data->pin[1], 1);
			gpio_set_value(data->pin[2], 1);
			gpio_set_value(data->pin[3], 0);
			break;

		//turn off
		default:
			gpio_set_value(data->pin[0], 0);
			gpio_set_value(data->pin[1], 0);
			gpio_set_value(data->pin[2], 0);
			gpio_set_value(data->pin[3], 0);
			break;

	}
	data->d = n;
	dump_stack();
    return size;
}

static ssize_t car_show_value(struct device *dev,
                                  struct device_attribute* attr,char* buf){
    struct car_data* data = dev_get_drvdata(dev);
    ssize_t da;
    mutex_lock(&data->data_lock);
	da = sprintf(buf, "%d", data->d);
    mutex_unlock(&data->data_lock);
	printk("show buf=%d\n", *buf);
	dump_stack();
    return da;
}

static DEVICE_ATTR(value,0666,car_show_value,car_store_value);

static int car_probe(struct platform_device *pdev){
	struct car_data* data;
	int result;
	data = kmalloc(sizeof(struct car_data),GFP_KERNEL);
	if(!data){
		pr_err("%s kmalloc error\n",__FUNCTION__);
		return -ENOMEM;  
	}
	printk("car_probe--------\n");
	dev_set_drvdata(&pdev->dev,data);
	result = parse_dt(pdev,data);
	if(result<0){
		pr_err("%s error when parse dt\n",__FUNCTION__);
		result = -EINVAL;
		goto err_parse_dt;
	}

    mutex_init(&data->data_lock);
	
	result=sysfs_create_file(&pdev->dev.kobj,&dev_attr_value.attr);
	printk("car probe success\n");
	return 0;
err_parse_dt:
	kfree(data);
	printk("car probe failed\n");
	return result;
}
static int car_remove(struct platform_device *pdev){
	return 0;
}


static struct of_device_id car_match_table[] = {
	{ .compatible = "thund,em",},
	{ },
};



static struct platform_driver car_driver = {
	.probe = car_probe,
	.remove = car_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "em",
		.of_match_table = car_match_table,
	},
};


module_platform_driver(car_driver);
MODULE_AUTHOR("EM");
MODULE_LICENSE("GPL v2");
