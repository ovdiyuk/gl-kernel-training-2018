// SPDX-License-Identifier: GPL-3.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/ctype.h>

#define MAXLEN PAGE_SIZE

static char strValue[MAXLEN];
static struct obj_statisic
{
	int last_absolute_time;
	int period_time;
}
time_class;

static ssize_t time_show(struct class *class, struct class_attribute *attr, char *buf)
{
	pr_info("mymodule_timer: absolute time of previous reading = %s\n", time_class.last_absolute_time);
	//sprintf(buf, "%s\n", strValue);
	time_class.last_absolute_time = current_kernel_time();
	return 0; //strlen(buf);
}

static ssize_t period_show(struct class *class, struct class_attribute *attr, char *buf)
{
	time_class.last_absolute_time = current_kernel_time();
	pr_info("mymodule_timer: period adjustable value = %s\n", time_class.period_time);
	pr_info("mymodule_timer: time (in seconds) passed since previous read = %s\n", time_class.last_absolute_time);
	//sprintf(buf, "%s\n", strValue);
	return 0; //strlen(buf);
}

static ssize_t period_store(struct class *class, struct class_attribute *attr, const char *buf, size_t count)
{
	size_t cpy_count;
	time_class.period_time = strValue;
	//cpy_count = (count < MAXLEN) ? count : MAXLEN;
	strncpy(strValue, buf, cpy_count);
	time_class.period_time = strValue;
	pr_info("mymodule_timer: period adjustable value = %s\n", strValue);

	return count;
}


// CLASS_ATTR_RW(rw);
// CLASS_ATTR_RO(statistic);

struct class_attribute class_attr_rw = {
	.attr = {.name = "period", .mode = 0666},
	.show = period_show,
	.store = period_store
};

struct class_attribute class_attr_statistic = {
	.attr = {.name = "time", .mode = 0444},
	.show = time_show,
};

static struct class *attr_class = 0;

static int mymodule_init(void)
{
	int ret;

	attr_class = class_create(THIS_MODULE, "mymodule_timer");
	if (attr_class == NULL)	{
		pr_err("mymodule_timer: error creating sysfs class\n");
		return -ENOMEM;
	}
	ret = class_create_file(attr_class, &class_attr_period);
	if (ret) {
		pr_err("mymodule_timer: error creating class_attr_period class attribute\n");
		class_destroy(attr_class);
		return ret;
	}
	ret = class_create_file(attr_class, &class_attr_time);
	if (ret) {
		pr_err("mymodule_timer: error creating class_attr_time class attribute\n");
		class_destroy(attr_class);
		return ret;
	}

	pr_info("mymodule_timer: module loaded\n");
	return 0;
}

static void mymodule_exit(void)
{
	class_remove_file(attr_class, &class_attr_period);
	class_remove_file(attr_class, &class_attr_time);
	class_destroy(attr_class);

	pr_info("mymodule_timer: module exited\n");
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_AUTHOR("Eugene.Ovdiyuk <ovdiyuk@gmail.com>");
MODULE_DESCRIPTION("sysfs_time  Kernel module");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
