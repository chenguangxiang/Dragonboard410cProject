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

struct mq_data{
	int dottl;	 //13
	int irq;
	int d;
	int flag;

	ktime_t last_ktime;
  	ktime_t now_ktime;
	
    struct mutex data_lock;
    bool data_ready;
	wait_queue_head_t data_queue;

	struct work_struct report_work;
	struct delayed_work cmd_work;
	
};

static irqreturn_t mq_interrupt_handler(int irq, void *ptr){
    struct mq_data* data = (struct mq_data*)ptr;
    if(!gpio_get_value(data->dottl)){
		data->d = 0;
   	}else{
		data->d = 1;
	}
	data->flag = 1;
    data-> data_ready = true;
    wake_up_interruptible(&data->data_queue);
	return IRQ_HANDLED;
}

static void cmd_work_func(struct work_struct* work){
	struct mq_data* data = container_of(work,struct mq_data,cmd_work.work);
    data-> data_ready = true;
    wake_up_interruptible(&data->data_queue);
	schedule_delayed_work(&data->cmd_work,msecs_to_jiffies(50));
}

static int parse_dt(struct platform_device* pdev,struct mq_data* data){
	int rc;
	struct device_node* node = pdev->dev.of_node;
	data->dottl = of_get_named_gpio(node,"thunder,dottl",0);
	if(gpio_is_valid(data->dottl)){
		rc = gpio_request(data->dottl,"mq_dottl");
		if(rc < 0){
			pr_err("Unable to request dottl gpio\n");
		}
		gpio_direction_input(data->dottl);
	}
	return 0;
}


static ssize_t mq_show_value(struct device *dev,
                                  struct device_attribute* attr,char* buf){
    struct mq_data* data = dev_get_drvdata(dev);
    ssize_t lenth;
    wait_event_interruptible(data->data_queue,data->data_ready);
    data->data_ready = false;
    mutex_lock(&data->data_lock);
	if(data->flag == 1){
		lenth = sprintf(buf,"%d\n",data->d);
	}else{
		lenth = sprintf(buf, "%d\n", 1);
	}
    mutex_unlock(&data->data_lock);
    return lenth;
}

static DEVICE_ATTR(value,0644,mq_show_value,NULL);

static int mq_probe(struct platform_device *pdev){
	struct mq_data* data;
	int result;
	data = kmalloc(sizeof(struct mq_data),GFP_KERNEL);
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
		data->irq = gpio_to_irq(data->dottl);
		result = request_irq(data->irq,mq_interrupt_handler,IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING,"mq_time_count",data);
		if(result<0){
			pr_err("Unable to request irq\n");
			goto err_parse_dt;
		}
	INIT_DELAYED_WORK(&data->cmd_work,cmd_work_func);

    mutex_init(&data->data_lock);
    init_waitqueue_head(&data->data_queue);
	schedule_delayed_work(&data->cmd_work,msecs_to_jiffies(50));
	
	result=sysfs_create_file(&pdev->dev.kobj,&dev_attr_value.attr);
	printk("mq-2 probe success\n");
	return 0;
err_parse_dt:
	kfree(data);
	printk("mq-2 probe failed\n");
	return result;
}
static int mq_remove(struct platform_device *pdev){
	return 0;
}


static struct of_device_id mq_match_table[] = {
	{ .compatible = "thundersoft,mq",},
	{ },
};



static struct platform_driver mq_driver = {
	.probe = mq_probe,
	.remove = mq_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "MQ-2",
		.of_match_table = mq_match_table,
	},
};


module_platform_driver(mq_driver);
MODULE_AUTHOR("MQ-2");
MODULE_LICENSE("GPL v2");
