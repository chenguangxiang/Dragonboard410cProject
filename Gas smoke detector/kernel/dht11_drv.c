#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/serio.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>
#include <linux/printk.h>
#include <linux/time.h>
#include <linux/irq.h>
#include <asm-generic/uaccess.h>

#define DEVICE_NAME "dht11"
#define DEVICE_MAJOR 0

struct dht11_sensor_dev{
	int pin;
	unsigned char value[5];
	int signal;
	int time;
	int count;
	dev_t devno;
	struct class *dht11_class;
	struct cdev cdev;
	struct mutex read_data;
	struct timeval lasttv;
	struct timeval tv;
	struct work_struct dht11_work;
};

static struct dht11_sensor_dev *dht11_dev;

static int read_byte(void){
		unsigned char data;
		int i = 0;
	for(i = 0; i < 8; i++){
		while(!gpio_get_value(dht11_dev->pin));
		udelay(30);
		data <<= 1;
		if(gpio_get_value(dht11_dev->pin) == 1){
			data |=0x01;
			while(gpio_get_value(dht11_dev->pin));
		}else{
			data |=0x00;
		}		
	}
	return data;
}

static void dht11_read(void){

	int i = 0;

	dht11_dev->signal = gpio_get_value(dht11_dev->pin);
	if(dht11_dev->signal == 0){
		while(!gpio_get_value(dht11_dev->pin));
		while(gpio_get_value(dht11_dev->pin));
		for(i = 0; i < 5; i++){
			dht11_dev->value[i] = read_byte();
		}
	}
}

static int dht11_start(void)
{
	gpio_direction_output(dht11_dev->pin, 0);	
	msleep(20);
	gpio_set_value(dht11_dev->pin,1);
	udelay(20);
	gpio_direction_input(dht11_dev->pin);
	do_gettimeofday(&dht11_dev->lasttv);
	dht11_dev->count=0;
	return 0;
}


static int dht11_checksum(struct dht11_sensor_dev *dev)
{
	int tmp = 0;
	tmp = dev->value[0] + dev->value[1] + dev->value[2] + dev->value[3];
	

	if((tmp != dev->value[4]) || (dev->value[4] == 0) || (tmp == 0) ){
		printk(KERN_INFO "[%s] %d %d\n", __func__, dev->value[4], tmp);
		return -1;
	}
	return 1;
}

static int dht11_sensor_open(struct inode *inode, struct file *filp)
{
	printk("dht11_sensor_open\n");	
	return 0;
}

static ssize_t dht11_sensor_read(struct file *filp,char __user *buf,size_t 
size,loff_t *f_pos)
{
	int result = 0;
	dht11_dev->value[0] = 0;
	dht11_dev->value[1] = 0;
	dht11_dev->value[2] = 0;
	dht11_dev->value[3] = 0;
	dht11_dev->value[4] = 0;
 
	printk("dht11_sensor_read\n");
	dht11_start();
	dht11_read();
	msleep(10);
	result=dht11_checksum(dht11_dev);
	if(result<0)
			return -EAGAIN; 

	printk("---Humidity=%d.%d%%RH\n---Temperature=%d.%dC\n",\
			dht11_dev->value[0], dht11_dev->value[1], \
			dht11_dev->value[2], dht11_dev->value[3]);
    result=copy_to_user(buf,&dht11_dev->value,4);
    if(result<0)
	{
         printk("copy to user err\n");
         return -EAGAIN;
    } 
    return  result;           
}

static int dht11_sensor_release(struct inode *inode,struct file *filp)
{
	module_put(THIS_MODULE);
	return 0;
}

static struct file_operations dht11_sensor_fops={
	.owner   = THIS_MODULE,
	.open    = dht11_sensor_open,
	.read    = dht11_sensor_read,
	.release = dht11_sensor_release,
};

//static int dht11_gpio_init(void)
static int parse_dt (struct platform_device *pdev,struct dht11_sensor_dev *data){
	int result;
	struct device_node *node = pdev->dev.of_node;
	data->pin = of_get_named_gpio(node,"thunder,gpio_data",0);
	if(gpio_is_valid(data->pin)){
    result=gpio_request(data->pin, "dht11_gpio");
	if(result)
	{
		printk(KERN_INFO "[%s] gpio_request \n", __func__);
		return -1;
	}
	gpio_direction_output(data->pin,1);
	}
	return 0;
}


static int dht11_sensor_setup_cdev(void)
{
	int ret;
	cdev_init(&(dht11_dev->cdev), &dht11_sensor_fops);
	dht11_dev->cdev.owner = THIS_MODULE;
	ret=cdev_add(&(dht11_dev->cdev),dht11_dev->devno, 1);
	if(ret)
	{
		printk(KERN_NOTICE"erro %d adding %s\n",ret,DEVICE_NAME);
	}
	return ret;
}

static int dht11_probe(struct platform_device *pdev){
	int result;
	dht11_dev=kmalloc(sizeof(struct dht11_sensor_dev),GFP_KERNEL);
	if(!dht11_dev)
	{
		result=-ENOMEM;
		goto allocate_memory_fail;
	}
    if(DEVICE_MAJOR)
	{
		result = register_chrdev_region(dht11_dev->devno, 1, DEVICE_NAME);
	}
	else
	{
		result = alloc_chrdev_region(&dht11_dev->devno, 0, 1, DEVICE_NAME);
	}
	if(result < 0)
	{
	    printk("register_chrdev_region err!\n");
		goto chardev_region_fail;
	}
	dht11_dev->dht11_class = class_create(THIS_MODULE, DEVICE_NAME);
	if(IS_ERR(dht11_dev->dht11_class))
    {
             printk("Err: failed in creating class.\n");
             goto class_create_fail;
    } 
	device_create(dht11_dev->dht11_class, NULL,dht11_dev->devno, NULL, 
DEVICE_NAME);
	result = parse_dt(pdev,dht11_dev);
	if(result<0)
		goto gpio_request_fail;
	result=dht11_sensor_setup_cdev();
	if(result<0)
		goto cdev_fail;
	mutex_init(&dht11_dev->read_data);
	printk("dht11 init ok!\n");
	return 0;
	
gpio_request_fail:
	cdev_del(&dht11_dev->cdev);
cdev_fail:
	device_destroy(dht11_dev->dht11_class,dht11_dev->devno);
	class_destroy(dht11_dev->dht11_class);
class_create_fail:
	unregister_chrdev_region(dht11_dev->devno,1);
chardev_region_fail:
allocate_memory_fail:
	kfree(dht11_dev);
	return result;
}

static int dht11_remove(struct platform_device *pdev)
{
	cdev_del(&dht11_dev->cdev);
	device_destroy(dht11_dev->dht11_class,dht11_dev->devno);
	class_destroy(dht11_dev->dht11_class);	
	unregister_chrdev_region(dht11_dev->devno, 1);
	kfree(dht11_dev);

	return 0;
}

static struct of_device_id dht11_match_table[] = {
	{ .compatible = "thundersoft,dht11",},
	{ },
};

static struct platform_driver dht11_driver = {

	.probe = dht11_probe,
	.remove = dht11_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = DEVICE_NAME,
		.of_match_table = dht11_match_table,
	},
};

module_platform_driver(dht11_driver);

MODULE_AUTHOR("winston");
MODULE_DESCRIPTION("DHT11 Driver");
MODULE_LICENSE("GPL");
