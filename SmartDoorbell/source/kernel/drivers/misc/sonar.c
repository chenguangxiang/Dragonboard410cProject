#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/sysfs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/input.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>
#include <linux/ctype.h>
#include <linux/pm_runtime.h>
#include <linux/device.h>
#include <linux/irq.h>
#include <linux/of_gpio.h>

#include <linux/wait.h>
#include <asm/uaccess.h> 

#define POLLIN		0x0001
#define MAX_LENTH   2000

struct us100_data{
	int enable;
	int poll_time; 	  //50

	int cmd_gpio;	 	 //24
	int echo_gpio;	 //13
	int irq;

	ktime_t last_ktime;
  	ktime_t now_ktime;
	int distance;		//mm
	
    	struct mutex data_lock;
    	bool data_ready;
	
    	wait_queue_head_t data_queue;

	struct work_struct report_work;
	struct delayed_work cmd_work;
	
/*=====================v2 ================*/
	/*
	int dev_major;
	struct device *dev;
	struct resource *res;
	struct class *cls;
	struct fasync_struct *faysnc;
	*/
};

/*=====================v2 ================*/ 
/*
int sonar_fasync( int fd, struct file* filp, int on ){
//	struct device *dev;
	struct us100_data * data = (struct us100_data*)filp;
	return fasync_helper(fd, filp, on, &data->faysnc);
}
*/
static irqreturn_t hs100_interrupt_handler(int irq, void *ptr){
    struct us100_data* data = (struct us100_data*)ptr;
    if(!gpio_get_value(data->echo_gpio)){
        data->now_ktime = ktime_get_boottime();
	    schedule_work(&data->report_work);
    }else{
        data->last_ktime = ktime_get_boottime();
    }
	return IRQ_HANDLED;
}

static void report_work_func(struct work_struct* work){
	struct timespec now,last;
	s64 diff_time;
	struct us100_data* data = container_of(work,struct us100_data,report_work);
	now = ktime_to_timespec(data->now_ktime);
	last= ktime_to_timespec(data->last_ktime);
	diff_time = now.tv_sec*1000000000UL+now.tv_nsec-last.tv_sec*1000000000UL-last.tv_nsec;
    mutex_lock(&data->data_lock);
    data->distance = diff_time*170*100*10/1000000000UL;
    mutex_unlock(&data->data_lock);
    data-> data_ready = true;
    wake_up_interruptible(&data->data_queue);

}

static void cmd_work_func(struct work_struct* work){
	struct us100_data* data = container_of(work,struct us100_data,cmd_work.work);
	gpio_set_value(data->cmd_gpio,1);
	udelay(12);
	gpio_set_value(data->cmd_gpio,0);	//genarate a 12 us pluse
	schedule_work(&data->report_work);
	schedule_delayed_work(&data->cmd_work,msecs_to_jiffies(data->poll_time));
}

static int parse_dt(struct platform_device* pdev,struct us100_data* data){
	int rc;
	struct device_node* node = pdev->dev.of_node;
	rc = of_property_read_u32(node,"thunder,poll_time",&data->poll_time);
	if(rc){
		pr_warning("%s you should point time",__FUNCTION__);
		data->poll_time = 20;
	}
	data->cmd_gpio = of_get_named_gpio(node,"thunder,gpio_cmd",0);
	if(gpio_is_valid(data->cmd_gpio)){
		rc = gpio_request(data->cmd_gpio,"hs100_cmd_gpio");
		if(rc < 0){
			pr_err("Unable to request cmd gpio\n");
		}
		gpio_direction_output(data->cmd_gpio,1);
	}
	data->echo_gpio = of_get_named_gpio(node,"thunder,gpio_irq",0);
	if(gpio_is_valid(data->echo_gpio)){
		rc = gpio_request(data->echo_gpio,"hs100_irq_gpio");
		if(rc < 0){
			pr_err("unable to request irq gpio\n");
		}
		rc = gpio_direction_input(data->echo_gpio);
	}
	if(data->cmd_gpio<0 || data->echo_gpio<0){
		pr_err("%s,error gpio\n",__FUNCTION__);
		return -EINVAL;
	}
	return 0;
}


static ssize_t hs100_show_enable(struct device *dev,
				struct device_attribute *attr, char *buf){
	struct us100_data* data = dev_get_drvdata(dev);
	ssize_t lenth = sprintf(buf,"%d",data->enable);
	return lenth;
}

static ssize_t hs100_store_enable(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t size){
	struct us100_data* data = dev_get_drvdata(dev);
	int enable = simple_strtoul(buf,NULL,10);
	if(enable){                               
		schedule_delayed_work(&data->cmd_work,0);
		data->enable =1;
	}
	else{		
		cancel_delayed_work_sync(&data->cmd_work);
		data->enable=0;
	}
	return size;

}

static ssize_t hs100_show_value(struct device *dev,
                                  struct device_attribute* attr,char* buf){
    struct us100_data* data = dev_get_drvdata(dev);
    ssize_t lenth;
    wait_event_interruptible(data->data_queue,data->data_ready);
    data->data_ready = false;
    mutex_lock(&data->data_lock);
	if(data->distance > MAX_LENTH){
		lenth = sprintf(buf, "%d", 0);
	}else{
		lenth = sprintf(buf,"%d",data->distance);
	}
    printk("%d\n",data->distance);       //v2
    mutex_unlock(&data->data_lock);
/*=====================v2 ================*/ 
	/*
	if(data->distance < 500){
		printk("send signal to user \n");
		kill_fasync(&data->faysnc, SIGIO, POLLIN);
	}
	*/
    return lenth;
}

static DEVICE_ATTR(enable,0644,hs100_show_enable,hs100_store_enable);
static DEVICE_ATTR(value,0644,hs100_show_value,NULL);

static int us100_probe(struct platform_device *pdev){
	struct us100_data* data;
	int result;
	data = kmalloc(sizeof(struct us100_data),GFP_KERNEL);
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
		data->irq = gpio_to_irq(data->echo_gpio);
		result = request_irq(data->irq,hs100_interrupt_handler,IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING,"hs100_time_count",data);
		if(result<0){
			pr_err("Unable to request irq\n");
			goto err_parse_dt;
		}
	INIT_WORK(&data->report_work,report_work_func);
	INIT_DELAYED_WORK(&data->cmd_work,cmd_work_func);

    mutex_init(&data->data_lock);
    init_waitqueue_head(&data->data_queue);
	schedule_delayed_work(&data->cmd_work,msecs_to_jiffies(data->poll_time));
	
	result=sysfs_create_file(&pdev->dev.kobj,&dev_attr_enable.attr);
	result=sysfs_create_file(&pdev->dev.kobj,&dev_attr_value.attr);
	printk("sonar probe sucess\n");
	return 0;
err_parse_dt:
	kfree(data);
	printk("sonar probe failed\n");
	return result;
}
static int us100_remove(struct platform_device *pdev){
	return 0;
}


static struct of_device_id u100_match_table[] = {
	{ .compatible = "thundersoft,sonar",},
	{ },
};



static struct platform_driver us100_driver = {
	.probe = us100_probe,
	.remove = us100_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "sensor_sonar",
		.of_match_table = u100_match_table,
	},
};


module_platform_driver(us100_driver);
MODULE_AUTHOR("SONAR");
MODULE_LICENSE("GPL v2");
