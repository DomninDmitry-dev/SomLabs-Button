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

struct button {
	unsigned int irq;
	struct gpio_desc *but;
	struct platform_device *pdev;
};

/*----------------------------------------------------------------------------*/
//static inline struct button *to_btn(struct platform_device *pd)
//{
//	return container_of(pd, struct button, pdev);
//}
/*----------------------------------------------------------------------------*/
static irqreturn_t btn_irq(int irq, void *data)
{
	struct button *btn = data;
	int state = gpiod_get_value(btn->but);
	//if(state == 0)
	dev_info(&btn->pdev->dev, "BTN: IRQ: ************************************************ %d\n", state);

	return (irqreturn_t)IRQ_HANDLED;
}
/*----------------------------------------------------------------------------*/
static int btn_probe(struct platform_device *pdev)
{
        int retval;
        struct button *btn;

        dev_info(&pdev->dev, "Starting module\n");

        btn = devm_kzalloc(&pdev->dev, sizeof(*btn), GFP_KERNEL);
		if (!btn) {
			dev_err(&pdev->dev, "Error mem <devm_kzalloc> \n");
			return -ENOMEM;
		}

        btn->but = gpiod_get(&pdev->dev, "but", GPIOD_IN);
        if (IS_ERR(btn->but)) {
				dev_err(&pdev->dev, "%s() unable to get led GPIO: %ld\n",
						__func__, PTR_ERR(btn->but));
				return PTR_ERR(btn->but);
		}

        retval = gpiod_set_debounce(btn->but, 2000);
        if(!retval)
        	dev_err(&pdev->dev, "gpiod_set_debounce - %d\n", retval);

        btn->irq = gpiod_to_irq(btn->but);

        //btn->irq = platform_get_irq(pdev, 0);
        dev_info(&pdev->dev, "irq = %d\n", btn->irq);
        retval = devm_request_threaded_irq(&pdev->dev, btn->irq, NULL,\
        								btn_irq, \
										IRQF_TRIGGER_FALLING | IRQF_ONESHOT, \
                                        "my-button", btn);
        btn->pdev = pdev;
        platform_set_drvdata(pdev, btn);
        return 0;
}
/*----------------------------------------------------------------------------*/
static int btn_remove(struct platform_device *pdev)
{
		struct button *btn = platform_get_drvdata(pdev);

		devm_free_irq(&pdev->dev, btn->irq, btn);
		devm_kfree(&pdev->dev, btn);
		gpiod_put(btn->but);
        dev_info(&pdev->dev, "Removing module\n");
        return 0;
}
/*----------------------------------------------------------------------------*/
static const struct of_device_id btn_dt_ids[] = {
    { .compatible = "somlabs-imx6ull,button" },
    { /* sentinel */ },
};

MODULE_DEVICE_TABLE(of, btn_dt_ids);

static struct platform_driver my_btn = {
		.probe = btn_probe,
		.remove = btn_remove,
        .driver = {
                .owner = THIS_MODULE,
                .name = "my_btn",
		        .of_match_table = of_match_ptr(btn_dt_ids),
        },
};

module_platform_driver(my_btn);
/*----------------------------------------------------------------------------*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dmitry");
