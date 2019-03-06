#include <PiDxe.h>
#include <Uefi.h>
#include <Protocol/Utc/Gpio.h>
#include <Protocol/Utc/Tegra210/PinMux.h>
#include <Protocol/Utc/Tegra210/Sysctl.h>
#include <Protocol/Utc/ErrNo.h>
#include <Library/IoLib.h>
#include <Foundation/Types.h>
#include <Shim/DebugLib.h>
#include <Shim/TimerLib.h>
#include <Shim/UBootIo.h>
#include <Library/TimerLib.h>
#include <Device/PinMux.h>
#include <Protocol/PinMux.h>

#include "Include/Gpio.h"
#include "Tegra210/Gpio.h"

static const int CONFIG_SFIO = 0;
static const int CONFIG_GPIO = 1;
static const int DIRECTION_INPUT = 0;
static const int DIRECTION_OUTPUT = 1;

/* Config GPIO pin 'gpio' as input or output (OE) as per 'output' */
static void set_direction(unsigned gpio, int output)
{
	struct gpio_ctlr *ctlr = (struct gpio_ctlr *) NV_PA_GPIO_BASE;
	struct gpio_ctlr_bank *bank = &ctlr->gpio_bank[GPIO_BANK(gpio)];
	u32 u;

	debug("set_direction: port = %d, bit = %d, %s\n",
		GPIO_FULLPORT(gpio), GPIO_BIT(gpio), output ? "OUT" : "IN");

	u = readl(&bank->gpio_dir_out[GPIO_PORT(gpio)]);
	if (output != DIRECTION_INPUT)
		u |= 1 << GPIO_BIT(gpio);
	else
		u &= ~(1 << GPIO_BIT(gpio));
	writel(u, &bank->gpio_dir_out[GPIO_PORT(gpio)]);
}

/* set GPIO pin 'gpio' output bit as 0 or 1 as per 'high' */
static void set_level(unsigned gpio, int high)
{
	struct gpio_ctlr *ctlr = (struct gpio_ctlr *)NV_PA_GPIO_BASE;
	struct gpio_ctlr_bank *bank = &ctlr->gpio_bank[GPIO_BANK(gpio)];
	u32 u;

	debug("set_level: port = %d, bit %d == %d\n",
		GPIO_FULLPORT(gpio), GPIO_BIT(gpio), high);

	u = readl(&bank->gpio_out[GPIO_PORT(gpio)]);
	if (high)
		u |= 1 << GPIO_BIT(gpio);
	else
		u &= ~(1 << GPIO_BIT(gpio));
	writel(u, &bank->gpio_out[GPIO_PORT(gpio)]);
}

/* Return config of pin 'gpio' as GPIO (1) or SFIO (0) */
static int get_config(unsigned gpio)
{
	struct gpio_ctlr *ctlr = (struct gpio_ctlr *)NV_PA_GPIO_BASE;
	struct gpio_ctlr_bank *bank = &ctlr->gpio_bank[GPIO_BANK(gpio)];
	u32 u;
	int type;

	u = readl(&bank->gpio_config[GPIO_PORT(gpio)]);
	type = (u >> GPIO_BIT(gpio)) & 1;

	debug("get_config: port = %d, bit = %d is %s\n",
		GPIO_FULLPORT(gpio), GPIO_BIT(gpio), type ? "GPIO" : "SFPIO");

	return type ? CONFIG_GPIO : CONFIG_SFIO;
}

/* Config pin 'gpio' as GPIO or SFIO, based on 'type' */
static void set_config(unsigned gpio, int type)
{
	struct gpio_ctlr *ctlr = (struct gpio_ctlr *)NV_PA_GPIO_BASE;
	struct gpio_ctlr_bank *bank = &ctlr->gpio_bank[GPIO_BANK(gpio)];
	u32 u;

	debug("set_config: port = %d, bit = %d, %s\n",
		GPIO_FULLPORT(gpio), GPIO_BIT(gpio), type ? "GPIO" : "SFPIO");

	u = readl(&bank->gpio_config[GPIO_PORT(gpio)]);
	if (type != CONFIG_SFIO)
		u |= 1 << GPIO_BIT(gpio);
	else
		u &= ~(1 << GPIO_BIT(gpio));
	writel(u, &bank->gpio_config[GPIO_PORT(gpio)]);
}

void gpio_config_table(const struct tegra_gpio_config *config, int len)
{
	int i;

	for (i = 0; i < len; i++) 
    {
		switch (config[i].init) 
        {
		case TEGRA_GPIO_INIT_IN:
			set_direction(config[i].gpio, DIRECTION_INPUT);
			break;
		case TEGRA_GPIO_INIT_OUT0:
			set_level(config[i].gpio, 0);
			set_direction(config[i].gpio, DIRECTION_OUTPUT);
			break;
		case TEGRA_GPIO_INIT_OUT1:
			set_level(config[i].gpio, 1);
			set_direction(config[i].gpio, DIRECTION_OUTPUT);
			break;
		}
		set_config(config[i].gpio, CONFIG_GPIO);
	}
}