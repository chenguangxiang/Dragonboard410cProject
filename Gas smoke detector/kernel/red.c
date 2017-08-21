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

struct red_data{
	int pin;	 //115
	int d;
    struct mutex data_lock;
};

static int parse_dt(struct platform_device* pdev,struct red_data* data){
	int rc;
	struct device_node* node = pdev->dev.of_node;
	data->pin = of_get_named_gpio(node,"thunder,pin",0);
	if(gpio_is_valid(data->pin)){
		rc = gpio_request(data->pin,"red_pin");
		if(rc < 0){
			pr_err("Unable to request pin gpio\n");
		}
		gpio_direction_output(data->pin, 1);
		data->d = 1;
	}
	return 0;
}

static ssize_t red_store_value(struct device *dev,                                                                                                                                                  
                struct device_attribute *attr,
                const char *buf, size_t size){
    struct red_data* data = dev_get_drvdata(dev);
//	int da = simple_strtoul(buf,NULL,10);
//	if(da == 1){
	if(buf[0] == 1){
		gpio_set_value(data->pin, 1);
	}else{
		gpio_set_value(data->pin, 0);
	}
//	data->d = da;
	data->d = buf[0];
	dump_stack();
    return size;
}

static ssize_t red_show_value(struct device *dev,
                                  struct device_attribute* attr,char* buf){
    struct red_data* data = dev_get_drvdata(dev);
    ssize_t da;
    mutex_lock(&data->data_lock);
	da = sprintf(buf, "%d", data->d);
    mutex_unlock(&data->data_lock);
	printk("show buf=%c\n", *buf);
	dump_stack();
    return da;
}

static DEVICE_ATTR(value,0666,red_show_value,red_store_value);

static int red_probe(struct platform_device *pdev){
	struct red_data* data;
	int result;
	data = kmalloc(sizeof(struct red_data),GFP_KERNEL);
	if(!data){
		pr_err("%s kmalloc error\n",__FUNCTION__);
		return -ENOMEM;  
	}
	dev_set_drvdata(&pdev->dev,data);
	result = parse_dt(pdev,data);
	if(result<0){
		pr_err("%s error when parse dt\n",__FUNCTION__);
		result = -EINVAL;
		goto err_parse_dt;
	}

    mutex_init(&data->data_lock);
	
	result=sysfs_create_file(&pdev->dev.kobj,&dev_attr_value.attr);
	printk("red probe success\n");
	return 0;
err_parse_dt:
	kfree(data);
	printk("red probe failed\n");
	return result;
}
static int red_remove(struct platform_device *pdev){
	return 0;
}


static struct of_device_id red_match_table[] = {
	{ .compatible = "thundersoft,red",},
	{ },
};



static struct platform_driver red_driver = {
	.probe = red_probe,
	.remove = red_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "red",
		.of_match_table = red_match_table,
	},
};


module_platform_driver(red_driver);
MODULE_AUTHOR("RED");
MODULE_LICENSE("GPL v2");
