/***************************************************************************//**
* \file led_driver.c
* \details Simple GPIO driver explanation
* \author EmbeTronicX
* \Tested with Linux raspberrypi 6.1.77-v8+
*******************************************************************************/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h>  //copy_to/from_user()
#include <linux/gpio.h>     //GPIO

//Seven Segment GPIO
#define GPIO_12 (12) //a
#define GPIO_13 (13) //b
#define GPIO_14 (14) //c
#define GPIO_15 (15) //d
#define GPIO_16 (16) //e
#define GPIO_17 (17) //f
#define GPIO_18 (18) //g

//LED GPIO
#define GPIO_19 (19)
#define GPIO_20 (20)
#define GPIO_21 (21)
#define GPIO_22 (22)
#define GPIO_23 (23)
#define GPIO_24 (24)
#define GPIO_25 (25)
#define GPIO_26 (26)

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;

static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);

/*************** Driver functions Declare **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t * off);
/**************************************************************/

/*************** Driver functions **********************/
//File operation structure
static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .read = etx_read,
    .write = etx_write,
    .open = etx_open,
    .release = etx_release,
};

/* This function will be called when we open the Device file*/
static int etx_open(struct inode *inode, struct file *file)
{
    pr_info("Device File Opened...!!!\n");
    return 0;
}

/* This function will be called when we close the Device file*/
static int etx_release(struct inode *inode, struct file *file)
{
    pr_info("Device File Closed...!!!\n");
    return 0;
}

/* This function will be called when we read the Device file*/
static ssize_t etx_read(struct file *filp,char __user *buf, size_t len, loff_t *off)
{
    uint8_t gpio_states[15] = {0}; 
    int i;

    //reading GPIO value
    for (i = 12; i <= 26; i++) {
        gpio_states[i - 12] = gpio_get_value(GPIO_12+(i-12));
    }

    //write to user
    if (copy_to_user(buf, gpio_states, sizeof(gpio_states)) > 0) {
        pr_err("ERROR: Not all the bytes have been copied to user\n");
    }

    //output read gpio status
    for (i = 0; i < 15; i++) {
        pr_info("Read function : GPIO_%d = %d \n", i + 12, gpio_states[i]);
    }
    
    return 0;
}

/*This function will be called when we write the Device file*/
static ssize_t etx_write(struct file *filp,const char __user *buf, size_t len, loff_t *off)
{
    uint8_t rec_buf[15] = {0};

    if( copy_from_user( rec_buf, buf, len ) > 0) {
        pr_err("ERROR: Not all the bytes have been copied from user\n");
    }

    for(size_t i=0 ; i < len; i++)
    {
        pr_info("Write Function : GPIO %c Set = %c\n",(i+12),rec_buf[i]);
        if(rec_buf[i]=='1') gpio_set_value(GPIO_12+i, 1);
        else if(rec_buf[i]=='0') gpio_set_value(GPIO_12+i, 0);
        else pr_err("Unknown command : Please provide either 1 or 0 \n");
    }
    return len;
}
/**************************************************************/
/*Module Init function*/
static int __init etx_driver_init(void)
{
    int gpios[] = {GPIO_12, GPIO_13, GPIO_14, GPIO_15, GPIO_16, GPIO_17, GPIO_18, GPIO_19,
                   GPIO_20, GPIO_21, GPIO_22, GPIO_23, GPIO_24, GPIO_25, GPIO_26};
    int num_gpios = sizeof(gpios) / sizeof(gpios[0]);


    /* Allocating Major number */
    if ((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) < 0) {
        pr_err("Cannot allocate major number\n");
        goto r_unreg;
    }

    pr_info("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

    /* Creating cdev structure */
    cdev_init(&etx_cdev, &fops);

    /* Adding character device to the system */
    if ((cdev_add(&etx_cdev, dev, 1)) < 0) {
        pr_err("Cannot add the device to the system\n");
        goto r_del;
    }

    /* Creating struct class */
    if ((dev_class = class_create(THIS_MODULE, "etx_class")) == NULL) {
        pr_err("Cannot create the struct class\n");
        goto r_class;
    }

    /* Creating device */
    if ((device_create(dev_class, NULL, dev, NULL, "etx_device")) == NULL) {
        pr_err("Cannot create the Device\n");
        goto r_device;
    }

    // Checking the GPIOs are valid or not
    for (int i = 0; i < num_gpios; i++) {
        if (!gpio_is_valid(gpios[i])) {
            pr_err("GPIO %d is not valid\n", gpios[i]);
            goto r_device;
        }
    }

    // Requesting the GPIOs
    for (int i = 0; i < num_gpios; i++) {
        if (gpio_request(gpios[i], "etx_gpio") < 0) {
            pr_err("ERROR: GPIO %d request\n", gpios[i]);
            goto r_gpio;
        }
    }

    // Configure the GPIOs as output
    for (int i = 0; i < num_gpios; i++) {
        gpio_direction_output(gpios[i], 0);
    }

    for (int i = 0; i < num_gpios; i++) {
        gpio_export(gpios[i], false);
    }

    pr_info("Device Driver Insert...Done!!!\n");
    return 0;

r_gpio:
    for (int j = 0; j < num_gpios; j++) {
        gpio_free(gpios[j]);
    }
r_device:
    device_destroy(dev_class, dev);
r_class:
    class_destroy(dev_class);
r_del:
    cdev_del(&etx_cdev);
r_unreg:
    unregister_chrdev_region(dev, 1);
    return -1;
}

/* Module exit function*/
static void __exit etx_driver_exit(void)
{
    int gpios[] = {GPIO_12, GPIO_13, GPIO_14, GPIO_15, GPIO_16, GPIO_17, GPIO_18, GPIO_19,
                   GPIO_20, GPIO_21, GPIO_22, GPIO_23, GPIO_24, GPIO_25, GPIO_26};
    int num_gpios = sizeof(gpios) / sizeof(gpios[0]);

    // Unexport and free GPIOs
    for (int i = 0; i < num_gpios; i++) {
        gpio_unexport(gpios[i]);
        gpio_free(gpios[i]);
    }

    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Device Driver Remove...Done!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - GPIO Driver");
MODULE_VERSION("1.32");