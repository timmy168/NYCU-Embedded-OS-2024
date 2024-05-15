#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

static char alphabet_to_signal[27][16] = 
{
    "1111001100010001", // A
    "0000011100000101", // b
    "1100111100000000", // C
    "0000011001000101", // d
    "1000011100000001", // E
    "1000001100000001", // F
    "1001111100010000", // G
    "0011001100010001", // H
    "1100110001000100", // I
    "1100010001000100", // J
    "0000000001101100", // K
    "0000111100000000", // L
    "0011001110100000", // M
    "0011001110001000", // N
    "1111111100000000", // O
    "1000001101000001", // P
    "0111000001010000", // q
    "1110001100011001", // R
    "1101110100010001", // S
    "1100000001000100", // T
    "0011111100000000", // U
    "0000001100100010", // V
    "0011001100001010", // W
    "0000000010101010", // X
    "0000000010100100", // Y
    "1100110000100010", // Z
    "0000000000000000"  // default
};

static char NAME[10];
static int letter;
static char alphabet;

//**********driver function**********//
static ssize_t my_read(struct file *fp, char *buf, size_t count, loff_t *fpos)
{
    count = 16;
    letter = 0;
    alphabet = NAME[0];
    printk("call read: Alphabet is %c\n", alphabet);
    switch (alphabet)
    {
    case 'A':
        letter = 0;
        break;
    case 'B':
        letter = 1;
        break;
    case 'C':
        letter = 2;
        break;
    case 'D':
        letter = 3;
        break;
    case 'E':
        letter = 4;
        break;
    case 'F':
        letter = 5;
        break;
    case 'G':
        letter = 6;
        break;
    case 'H':
        letter = 7;
        break;
    case 'I':
        letter = 8;
        break;
    case 'J':
        letter = 9;
        break;
    case 'K':
        letter = 10;
        break;
    case 'L':
        letter = 11;
        break;
    case 'M':
        letter = 12;
        break;
    case 'N':
        letter = 13;
        break;
    case 'O':
        letter = 14;
        break;
    case 'P':
        letter = 15;
        break;
    case 'Q':
        letter = 16;
        break;
    case 'R':
        letter = 17;
        break;
    case 'S':
        letter = 18;
        break;
    case 'T':
        letter = 19;
        break;
    case 'U':
        letter = 20;
        break;
    case 'V':
        letter = 21;
        break;
    case 'W':
        letter = 22;
        break;
    case 'X':
        letter = 23;
        break;
    case 'Y':
        letter = 24;
        break;
    case 'Z':
        letter = 25;
        break;
    default:
        letter = 26;
        break;
    }
    if( copy_to_user(buf, alphabet_to_signal[letter], count) > 0) {
        pr_err("ERROR: Not all the bytes have been copied to user\n");
    }
    return count;
}

static ssize_t my_write(struct file *fp, const char *buf, size_t count, loff_t *fpos)
{
    printk("call write\n");
    if (copy_from_user(NAME, buf, count) > 0)
    {
        pr_err("ERROR: Not all the bytes have been copied from user\n");
    }
    pr_info("Write Function : Alphabet = %c\n", NAME);
    return count;
}

static int my_open(struct inode *inode, struct file *fp)
{
    printk("call open\n");
    return 0;
}

//**********file op***********//
struct file_operations my_fops = {
    read : my_read,
    write : my_write,
    open : my_open
};

//**********module function***********//
//#define MAJOR_NUM 244
#define MAJOR_NUM 200
#define DEVICE_NAME "mydev"
static int my_init(void)
{
    printk("call init\n");
    if (register_chrdev(MAJOR_NUM, DEVICE_NAME, &my_fops) < 0)
    {
        printk("Can not get major %d\n", MAJOR_NUM);
        return (-EBUSY);
    }
    printk("My device is started and the major is %d\n", MAJOR_NUM);
    return 0;
}
static void my_exit(void)
{
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    printk("call exit\n");
}
module_init(my_init);
module_exit(my_exit);
