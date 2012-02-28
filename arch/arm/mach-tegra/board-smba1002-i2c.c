/*
 * arch/arm/mach-tegra/board-smba1002-i2c.c
 *
 * Copyright (C) 2011 Eduardo Jos� Tagle <ejtagle@tutopia.com>
 * Copyright (C) 2011 Jens Andersen <jens.andersen@gmail.com>
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

#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>
#include <linux/resource.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/i2c-tegra.h>

#include <asm/mach-types.h>
//#include <mach/nvhost.h>
#include <mach/nvmap.h>
#include <mach/irqs.h>
#include <mach/iomap.h>
#include <mach/dc.h>
#include <mach/fb.h>

#include "board.h"
#include "devices.h"
#include "gpio-names.h"
#include "board-smba1002.h"

static struct tegra_i2c_platform_data smba1002_i2c1_platform_data = {
      .adapter_nr = 0,
      .bus_count = 1,
      .bus_clk_rate = { 100000, 0 },
      .is_clkon_always = true,
      .retries = 10,
      .timeout = 300,
      .scl_gpio = {TEGRA_GPIO_PC4, 0},
      .sda_gpio = {TEGRA_GPIO_PC5, 0},
      .arb_recovery = arb_lost_recovery,
};
static const struct tegra_pingroup_config i2c2_ddc = {
	.pingroup	= TEGRA_PINGROUP_DDC,
	.func		= TEGRA_MUX_I2C2,
};

static const struct tegra_pingroup_config i2c2_gen2 = {
	.pingroup	= TEGRA_PINGROUP_PTA,
	.func		= TEGRA_MUX_I2C2,
};

static struct tegra_i2c_platform_data smba1002_i2c2_platform_data = {
	.adapter_nr	= 1,
	.bus_count	= 2,
	.bus_clk_rate	= { 100000, 100000},
	.bus_mux	= { &i2c2_ddc, &i2c2_gen2 },
	.bus_mux_len	= { 1, 1 },
	.scl_gpio = {0, TEGRA_GPIO_PT5},
        .sda_gpio = {0, TEGRA_GPIO_PT6},
        .arb_recovery = arb_lost_recovery,
};

static struct tegra_i2c_platform_data smba1002_i2c3_platform_data = {
      .adapter_nr = 3,
      .bus_count = 1,
      .bus_clk_rate = { 100000, 0 },
      .slave_addr = 0x8a,
      .scl_gpio = {TEGRA_GPIO_PBB2, 0},
      .sda_gpio = {TEGRA_GPIO_PBB3, 0},
      .arb_recovery = arb_lost_recovery,
};

static struct tegra_i2c_platform_data smba1002_dvc_platform_data = {
	.adapter_nr	= 4,
	.bus_count	= 1,
	.bus_clk_rate	= { 100000, 0 },
	.is_dvc		= true,
	.scl_gpio = {TEGRA_GPIO_PZ6, 0},
	.sda_gpio = {TEGRA_GPIO_PZ7, 0},
	.arb_recovery = arb_lost_recovery,
};

int __init smba1002_i2c_register_devices(void)
{
	tegra_i2c_device1.dev.platform_data = &smba1002_i2c1_platform_data;
	tegra_i2c_device2.dev.platform_data = &smba1002_i2c2_platform_data;
	tegra_i2c_device3.dev.platform_data = &smba1002_i2c3_platform_data;
	tegra_i2c_device4.dev.platform_data = &smba1002_dvc_platform_data;

	platform_device_register(&tegra_i2c_device1);
	platform_device_register(&tegra_i2c_device2);
	platform_device_register(&tegra_i2c_device3);
	platform_device_register(&tegra_i2c_device4);
	
	return 0;
}

