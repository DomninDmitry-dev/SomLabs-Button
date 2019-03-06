#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/of.h>
#include <linux/spinlock.h>

//static unsigned int GPIO_BTN = 42;
static unsigned int irq;
static struct gpio_desc *but;

/*----------------------------------------------------------------------------*/
static irqreturn_t btn_irq(int irq, void *data)
{
	int state = gpiod_get_value(but);
	//if(state == 0)
		pr_info("BTN: IRQ: ************************************************ %d\n", state);

	return (irqreturn_t)IRQ_HANDLED;
}
/*----------------------------------------------------------------------------*/
static int btn_probe(struct platform_device *pdev)
{
        int retval;
        struct device *dev = &pdev->dev;

        pr_info("BTN: Starting module\n");

        but = gpiod_get(dev, "but", GPIOD_IN);
        if (IS_ERR(but)) {
				dev_err(dev, "%s() unable to get led GPIO: %ld\n",
						__func__, PTR_ERR(but));
				return PTR_ERR(but);
		}

        retval = gpiod_set_debounce(but, 2000);
        	dev_err(dev, "BTN: gpiod_set_debounce - %d\n", retval);

        if(retval < 0)
        irq = gpiod_to_irq(but);
        pr_info("BTN: irq = %d\n", irq);
        retval = request_threaded_irq(irq, NULL,\
        								(irq_handler_t)btn_irq, \
										IRQF_TRIGGER_FALLING | IRQF_ONESHOT, \
                                        "my-button", NULL);

        return 0;
}
/*----------------------------------------------------------------------------*/
static int btn_remove(struct platform_device *pdev)
{
		gpiod_put(but);
		free_irq(irq, NULL);
        pr_info("BTN: Removing module\n");
        return 0;
}
/*----------------------------------------------------------------------------*/
static const struct of_device_id btn_dt_ids[] = {
    { .compatible = "somlabs-imx6ull,button" },
    { /* sentinel */ },
};

MODULE_DEVICE_TABLE(of, btn_dt_ids);

static struct platform_driver my_btn = {
        .driver = {
                .owner = THIS_MODULE,
                .name = "my_btn",
		        .of_match_table = of_match_ptr(btn_dt_ids),
        },
        .probe = btn_probe,
        .remove = btn_remove,
};

module_platform_driver(my_btn);
/*----------------------------------------------------------------------------*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry");
