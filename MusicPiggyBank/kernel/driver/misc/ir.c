#include <linux/module.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/sysfs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/jiffies.h>
#include <linux/irq.h>
#include <linux/of_gpio.h>

struct ir_data {
	int gpio;
	int irq;
    struct input_dev *input;
	struct delayed_work report_work;
};

static irqreturn_t ir_interrupt_handler(int irq, void *ptr) {
    struct ir_data* data = (struct ir_data*)ptr;

    if (!data) {
        printk("%s: data is NULL!\n", __func__);
        return IRQ_HANDLED;
    }

    schedule_delayed_work(&data->report_work, 0);

	return IRQ_HANDLED;
}

static void report_work_func(struct work_struct *work) {
	struct ir_data *obj = (struct ir_data *)container_of(work, struct ir_data, report_work.work);
    static unsigned long last_time = 0;
    unsigned long after_time;
    unsigned long tmp;

    after_time = jiffies;
    tmp = (after_time - last_time) * 1000 / HZ;
  
    if (tmp > 50) {
        if (!gpio_get_value(obj->gpio)) {
            input_report_key(obj->input, KEY_NEXTSONG, 1);
            input_sync(obj->input);
            input_report_key(obj->input, KEY_NEXTSONG, 0);
            input_sync(obj->input);
        }
    }
    last_time = after_time;
}

static int parse_dt(struct platform_device* pdev, struct ir_data* data) {
	int rc;
	struct device_node* node = pdev->dev.of_node;

	data->gpio = of_get_named_gpio(node,"thunder,gpio", 0);
	if (gpio_is_valid(data->gpio)) {
		rc = gpio_request(data->gpio, "ir_gpio");
		if (rc < 0) {
			printk("%s: unable to request gpio\n", __func__);
            return -EINVAL;
		}
		rc = gpio_direction_input(data->gpio);
	}

	if (data->gpio < 0) {
		printk("%s: error gpio\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static ssize_t ir_show_value(struct device *dev, struct device_attribute* attr, char* buf) {
    struct ir_data *data = dev_get_drvdata(dev);

    printk("%s: gpio value is %d\n", __func__, gpio_get_value(data->gpio));
    return 0;
}

static DEVICE_ATTR(value, 0644, ir_show_value, NULL);

static int ir_probe(struct platform_device *pdev) {
	struct ir_data* data;
	int result;

    printk("%s\n", __func__);
	data = kmalloc(sizeof(struct ir_data), GFP_KERNEL);
	if (!data) {
		printk("%s kmalloc error\n", __func__);
		return -ENOMEM;  
	}
	dev_set_drvdata(&pdev->dev, data);

    data->input = input_allocate_device();
    if (!data->input) {
        printk("%s: input_allocate_device failed!\n", __func__);
        goto err_input_allocate;
    }
    data->input->name = "ir";

    set_bit(EV_SYN, data->input->evbit);
    set_bit(EV_KEY, data->input->evbit);
    set_bit(KEY_NEXTSONG, data->input->keybit);

    result = input_register_device(data->input);
    if (result < 0) {
        printk("%s: input_register_device() failed!\n", __func__);
        goto err_input_register;
    }

	result = parse_dt(pdev, data);
	if (result < 0) {
		printk("%s:error when parse dt\n", __func__);
		result = -EINVAL;
		goto err_parse_dt;
	}
	
	data->irq = gpio_to_irq(data->gpio);

	result = request_irq(data->irq, ir_interrupt_handler, IRQF_TRIGGER_FALLING, "ir_int", data);
	if (result < 0) {
		printk("%s: Unable to request irq\n", __func__);
		goto err_parse_dt;
	}
    printk("%s: gpio=%d, irq=%d\n", __func__, data->gpio, data->irq);

	INIT_DELAYED_WORK(&data->report_work, report_work_func);

	result = sysfs_create_file(&pdev->dev.kobj, &dev_attr_value.attr);
	printk("%s: ir probe success\n", __func__);
	return 0;

err_parse_dt:
err_input_register:
    input_free_device(data->input);
err_input_allocate:
	kfree(data);
	printk("%s: ir probe failed\n", __func__);
	return result;
}

static int ir_remove(struct platform_device *pdev){
	return 0;
}

static struct of_device_id ir_match_table[] = {
	{ .compatible = "thundersoft,ir",},
	{ },
};

static struct platform_driver ir_driver = {
	.probe  = ir_probe,
	.remove = ir_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "ir_module",
		.of_match_table = ir_match_table,
	},
};

module_platform_driver(ir_driver);
MODULE_AUTHOR("Chen Guangxiang(chengx0327@thundersoft.com)");
MODULE_LICENSE("GPL v2");
