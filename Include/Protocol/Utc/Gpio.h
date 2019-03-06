/*
 * Copyright (c) 2011, Google Inc. All rights reserved.
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _TEGRA_GPIO_H_
#define _TEGRA_GPIO_H_

#include <PiDxe.h>
#include <Uefi.h>
#include <Foundation/Types.h>
#include <Protocol/Utc/Tegra.h>

#define TEGRA_GPIO_PORT_A 0
#define TEGRA_GPIO_PORT_B 1
#define TEGRA_GPIO_PORT_C 2
#define TEGRA_GPIO_PORT_D 3
#define TEGRA_GPIO_PORT_E 4
#define TEGRA_GPIO_PORT_F 5
#define TEGRA_GPIO_PORT_G 6
#define TEGRA_GPIO_PORT_H 7
#define TEGRA_GPIO_PORT_I 8
#define TEGRA_GPIO_PORT_J 9
#define TEGRA_GPIO_PORT_K 10
#define TEGRA_GPIO_PORT_L 11
#define TEGRA_GPIO_PORT_M 12
#define TEGRA_GPIO_PORT_N 13
#define TEGRA_GPIO_PORT_O 14
#define TEGRA_GPIO_PORT_P 15
#define TEGRA_GPIO_PORT_Q 16
#define TEGRA_GPIO_PORT_R 17
#define TEGRA_GPIO_PORT_S 18
#define TEGRA_GPIO_PORT_T 19
#define TEGRA_GPIO_PORT_U 20
#define TEGRA_GPIO_PORT_V 21
#define TEGRA_GPIO_PORT_W 22
#define TEGRA_GPIO_PORT_X 23
#define TEGRA_GPIO_PORT_Y 24
#define TEGRA_GPIO_PORT_Z 25
#define TEGRA_GPIO_PORT_AA 26
#define TEGRA_GPIO_PORT_BB 27
#define TEGRA_GPIO_PORT_CC 28
#define TEGRA_GPIO_PORT_DD 29
#define TEGRA_GPIO_PORT_EE 30
#define TEGRA_GPIO_PORT_FF 31

#define TEGRA_GPIO(port, offset) \
	((TEGRA_GPIO_PORT_##port * 8) + offset)

#define TEGRA_GPIOS_PER_PORT	8
#define TEGRA_PORTS_PER_BANK	4
#define MAX_NUM_GPIOS           (TEGRA_GPIO_PORTS * TEGRA_GPIO_BANKS * 8)
#define GPIO_NAME_SIZE		20	/* gpio_request max label len */

#define GPIO_BANK(x)		((x) >> 5)
#define GPIO_PORT(x)		(((x) >> 3) & 0x3)
#define GPIO_FULLPORT(x)	((x) >> 3)
#define GPIO_BIT(x)		((x) & 0x7)

enum tegra_gpio_init {
	TEGRA_GPIO_INIT_IN,
	TEGRA_GPIO_INIT_OUT0,
	TEGRA_GPIO_INIT_OUT1,
};

struct tegra_gpio_config {
	u32 gpio:16;
	u32 init:2;
};

/**
 * Configure a list of GPIOs
 *
 * @param config	List of GPIO configurations
 * @param len		Number of config items in list
 */
void gpio_config_table(const struct tegra_gpio_config *config, int len);

#endif	/* TEGRA_GPIO_H_ */
