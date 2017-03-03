#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/irqdomain.h>
#include <linux/miscdevice.h>
#include <linux/random.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Avi Weinstock");
MODULE_DESCRIPTION("PseudoHSM");

#define PSEUDOHSM_IRQ 0x22

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

static int disable_cache(void) {
    printk(KERN_INFO "PseudoHSM: Disabling L1 and L2 caches.\n");
    __asm__("mov    %%cr0, %%rax\n\t"
            "or     $(1 << 30), %%rax\n\t"
            "mov    %%rax, %%cr0\n\t"
            "wbinvd\n\t"
    : : : "rax" );
    return 0;
}
static void enable_cache(void) {
    printk(KERN_INFO "PseudoHSM: Enabling L1 and L2 caches.\n");
    __asm__("mov    %%cr0, %%rax\n\t"
            "and    $~(1 << 30), %%rax\n\t"
            "mov    %%rax, %%cr0\n\t"
            "wbinvd\n\t"
    : : : "rax" );
}

static ssize_t toggle_cache(struct file* file, const char * buf, size_t count, loff_t *ppos) {
    if(count != 1) {
        printk(KERN_ALERT "PseudoHSM::toggle_cache: count != 1\n");
        return -EMSGSIZE;
    }
    switch(*buf) {
        default:
            printk(KERN_ALERT "PseudoHSM::toggle_cache: unexpected command %c\n", *buf);
            return -EBADRQC;
        case '0':
            disable_cache();
            return 1;
        case '1':
            enable_cache();
            return 1;
    }
}

static unsigned long int get_controlflags(void) {
    register unsigned long int ret asm("rax");
    __asm__("mov    %%cr0, %%rax\n\t" : : : "rax" );
    return ret;
}

static char is_cache_enabled(void) {
    return !(get_controlflags() & (1<<30));
}

static ssize_t read_cache(struct file* file, char * buf, size_t count, loff_t *ppos) {
    if(*ppos == 0) {
        ++*ppos;
        if(count >= 1) {
            buf[0] = is_cache_enabled();
            printk("PseudoHSM::read_cache: reading %d\n", buf[0]);
            return 1;
        } else {
            printk("PseudoHSM::read_cache: count was 0\n");
            return 0;
        }
    } else {
        printk("PseudoHSM::read_cache: EOF\n");
        return 0;
    }
}

static const struct file_operations cache_ops = {
    .owner  = THIS_MODULE,
    .read   = read_cache,
    .write  = toggle_cache,
};

static struct miscdevice cache_dev = {
    MISC_DYNAMIC_MINOR, "cachectl" , &cache_ops
};

static int __init pseudohsm_init(void) {
    int rc/*, virq*/;
    printk(KERN_INFO "PseudoHSM::pseudohsm_init\n");
    rc = misc_register(&pseudohsm_dev);
    printk(KERN_INFO "PseudoHSM: misc_register(&pseudohsm_dev) returned %d\n", rc);
    rc = misc_register(&cache_dev);
    printk(KERN_INFO "PseudoHSM: misc_register(&cache_dev) returned %d\n", rc);
    //rc = irq_alloc_desc_at(PSEUDOHSM_IRQ, 0);
    //printk(KERN_INFO "PseudoHSM: irq_alloc_desc_at returned %d\n", rc);
    //virq = irq_create_mapping(0, PSEUDOHSM_IRQ);
    //printk(KERN_INFO "PseudoHSM: irq_create_mapping(0, %d) returned %d\n", PSEUDOHSM_IRQ, virq);
    rc = request_irq(PSEUDOHSM_IRQ, interrupt_handler, IRQF_TRIGGER_RISING, "pseudohsm_interrupt", 0);
    printk(KERN_INFO "PseudoHSM: request_irq returned %d\n", rc);
    return 0;
}

static void __exit pseudohsm_exit(void) {
    int rc;
    printk(KERN_INFO "PseudoHSM::pseudohsm_exit\n");
    rc = misc_deregister(&pseudohsm_dev);
    printk(KERN_INFO "PseudoHSM: misc_deregister(&pseudohsm_dev) returned %d\n", rc);
    rc = misc_deregister(&cache_dev);
    printk(KERN_INFO "PseudoHSM: misc_deregister(&cache_dev) returned %d\n", rc);
    free_irq(PSEUDOHSM_IRQ, 0);
    //irq_free_desc(PSEUDOHSM_IRQ);
}

module_init(pseudohsm_init);
module_exit(pseudohsm_exit);
