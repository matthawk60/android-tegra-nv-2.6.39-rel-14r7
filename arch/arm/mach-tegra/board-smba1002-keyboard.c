/* OK */
/*
 * arch/arm/mach-tegra/board-smba1002-keyboard.c
 *
 * Copyright (C) 2011 Eduardo José Tagle <ejtagle@tutopia.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/platform_device.h>
#include <linux/input.h>

#include <linux/gpio_keys.h>
//#include <linux/gpio_shortlong_key.h>
#include <linux/leds.h>
#include <linux/leds_pwm.h>
#include <mach/iomap.h>
#include <mach/io.h>
#include <linux/io.h>
#include <linux/pm.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

#include <linux/gpio.h>
#include <asm/mach-types.h>

#include "board-smba1002.h"
#include "gpio-names.h"
#include "wakeups-t2.h"
#include "fuse.h"

static struct gpio_keys_button smba1002_keys[] = {
	[0] = {
		.gpio = SMBA1002_KEY_VOLUMEUP,
		.active_low = true,
		.debounce_interval = 10,
		.wakeup = false,		
		.code = KEY_VOLUMEUP,
		.type = EV_KEY,		
		.desc = "volume up",
	},
	[1] = {
		.gpio = SMBA1002_KEY_VOLUMEDOWN,
		.active_low = true,
		.debounce_interval = 10,
		.wakeup = false,		
		.code = KEY_VOLUMEDOWN,
		.type = EV_KEY,		
		.desc = "volume down",
	},
	[2] = {
		.gpio = SMBA1002_KEY_POWER,
		.active_low = true,
		.debounce_interval = 50,
		.wakeup = true,		
		.code = KEY_POWER,
		.type = EV_KEY,		
		.desc = "power",
	},
	[3] = {
		.gpio = SMBA1002_KEY_BACK,
		.active_low = true,
		.debounce_interval = 10,
		.wakeup = false,		
		.code = KEY_BACK,
		.type = EV_KEY,		
		.desc = "back",
	},
};
#define PMC_WAKE_STATUS 0x14

static int smba1002_wakeup_key(void)
{
	unsigned long status = 
		readl(IO_ADDRESS(TEGRA_PMC_BASE) + PMC_WAKE_STATUS);
	return status & TEGRA_WAKE_GPIO_PV2 ? KEY_POWER : KEY_RESERVED;
}

static struct gpio_keys_platform_data smba1002_keys_platform_data = {
	.buttons 	= smba1002_keys,
	.nbuttons 	= ARRAY_SIZE(smba1002_keys),
	.wakeup_key     = smba1002_wakeup_key,
	.rep		= false, /* auto repeat enabled */
};

static struct platform_device smba1002_keys_device = {
	.name 		= "gpio-keys",
	.id 		= 0,
	.dev		= {
		.platform_data = &smba1002_keys_platform_data,
	},
};


static struct platform_device *smba1002_pmu_devices[] __initdata = {
	&smba1002_keys_device,
};

/* Register all keyboard devices */
int __init smba1002_keyboard_register_devices(void)
{
  	//enable_irq_wake(gpio_to_irq(TEGRA_WAKE_GPIO_PV2));
	return platform_add_devices(smba1002_pmu_devices, ARRAY_SIZE(smba1002_pmu_devices));
}

