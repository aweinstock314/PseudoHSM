#include <linux/module.h>
#include <linux/kernel.h>    
#include <linux/init.h>      

#include <linux/fs.h>        
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/string.h>    
#include <linux/random.h>    

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Avi Weinstock");
MODULE_DESCRIPTION("PseudoHSM");

#define PSEUDOHSM_IRQ 0x7f

static irqreturn_t interrupt_handler(int irq, void *dev_id) {
    return 0;
}

static ssize_t store_key(struct file* file, const char * buf, size_t count, loff_t *ppos) {
    // TODO: implement
    size_t i;
    printk(KERN_INFO "store_key: buf: %s\n", buf);
    for(i=0; i < count; i++) {
        printk("buf[%lu]: %hhx", i, buf[i]);
    }
    return count;
}

static const struct file_operations pseudohsm_ops = {
    .owner  = THIS_MODULE,
    .write  = store_key,
};

static struct miscdevice pseudohsm_dev = {
    MISC_DYNAMIC_MINOR, "pseudohsm" , &pseudohsm_ops
};

static int __init pseudohsm_init(void) {
    int rc;
    printk(KERN_INFO "PseudoHSM::pseudohsm_init\n");
    rc = misc_register(&pseudohsm_dev);
    printk(KERN_INFO "PseudoHSM: misc_register returned %d\n", rc);
    rc = request_irq(PSEUDOHSM_IRQ, interrupt_handler, 0, "pseudohsm_interrupt", 0);
    printk(KERN_INFO "PseudoHSM: request_irq returned %d\n", rc);
    return 0;
}

static void __exit pseudohsm_exit(void) {
    int rc;
    printk(KERN_INFO "PseudoHSM::pseudohsm_exit\n");
    rc = misc_deregister(&pseudohsm_dev);
    printk(KERN_INFO "PseudoHSM: misc_deregister returned %d\n", rc);
    free_irq(PSEUDOHSM_IRQ, 0);
}

module_init(pseudohsm_init);
module_exit(pseudohsm_exit);
