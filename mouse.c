#include <linux/module.h>
#include <linux/usb.h>
#include <linux/cdev.h>
#include <linux/input.h>
#include <linux/uaccess.h>
#include <linux/keyboard.h>

MODULE_LICENSE("GPL");

// Shit that i talk to
static struct input_dev *mouse_dev 	= NULL;

// Function that catch keyboard button pressed
static int key_cb(struct notifier_block *nblock, unsigned long code, void *_param);

static struct notifier_block key_shit = {
	.notifier_call = key_cb,
};

static int __init mouse_init(void)
{
	int res;

	// Allocate input struct 
	mouse_dev = input_allocate_device();
        if (!mouse_dev) {
                printk(KERN_ERR "Not enough memory\n");
                return -ENOMEM;
        }
	mouse_dev->name = "mouse_shit";
	mouse_dev->relbit[0] = BIT_MASK(REL_X) | BIT_MASK(REL_Y) | BIT_MASK(REL_WHEEL);
	mouse_dev->evbit[0] =  BIT_MASK(EV_REL);

	res = input_register_device(mouse_dev);
	if(res) {
		printk(KERN_ERR "Failed to register device\n");
		goto mouse_err;
	}

	register_keyboard_notifier(&key_shit);

	return 0;

mouse_err:
	input_unregister_device(mouse_dev);
	return -EAGAIN;
}

static void __exit mouse_exit(void)
{
	input_unregister_device(mouse_dev);
	unregister_keyboard_notifier(&key_shit);
}

static int key_cb(struct notifier_block *nblock, unsigned long code, void *_param)
{
	struct keyboard_notifier_param *param = _param;

	pr_info("111code: 0x%x\n", param->value);

	switch(param->value){
	case 0x11:
		input_report_rel(mouse_dev, REL_Y, -10);
		break;
	case 0x1f:
		input_report_rel(mouse_dev, REL_Y, 10);
		break;
	case 0x1e:
		input_report_rel(mouse_dev, REL_X, -10);
		break;
	case 0x20:
		input_report_rel(mouse_dev, REL_X, 10);

	default:
		break;
	}

	input_sync(mouse_dev);

	return NOTIFY_OK;
}

module_init(mouse_init);
module_exit(mouse_exit);
