/*
 * arch/arm/mach-tegra/board-smba1002.h
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

#ifndef _MACH_TEGRA_BOARD_SMBA1002_H
#define _MACH_TEGRA_BOARD_SMBA1002_H

#define TPS6586X_INT_BASE	TEGRA_NR_IRQS

#define SMBA1002_BT_RESET 		TEGRA_GPIO_PU0 	/* 0= reset asserted */

/* GPS and Magnetic sensor share the same enabling IO line */
#define SMBA1002_GPSMAG_DISABLE  	TEGRA_GPIO_PV3 	/* 0= disabled */
#define SMBA1002_3G_DISABLE		TEGRA_GPIO_PJ2 /* 0 = disabled */
#define SMBA1002_KEY_VOLUMEUP 		TEGRA_GPIO_PV4 	/* 0=pressed */
#define SMBA1002_KEY_VOLUMEDOWN 	TEGRA_GPIO_PD4 	/* 0=pressed */
#define SMBA1002_KEY_POWER 		TEGRA_GPIO_PV2 	/* 0=pressed */
#define SMBA1002_KEY_BACK		TEGRA_GPIO_PH0	/* 0=pressed */

/* #define SMBA1002_EMC_SAMSUNG		*/
/* #define SMBA1002_EMC_ELPIDA50NM	*/
/* #define SMBA1002_EMC_ELPIDA40NM	*/


#define SMBA1002_CAMERA_POWER 	TEGRA_GPIO_PBB5 /* 1=powered on */
#define SMBA1002_CAMERA_ROTATION	TEGRA_GPIO_PX7

#define SMBA1002_NAND_WPN		TEGRA_GPIO_PC7	/* NAND flash write protect: 0=writeprotected */

#define SMBA1002_BL_ENB			TEGRA_GPIO_PD3
#define SMBA1002_LVDS_SHUTDOWN	TEGRA_GPIO_PB2 // Is this right?
#define SMBA1002_EN_VDD_PANEL	TEGRA_GPIO_PC6 
#define SMBA1002_BL_VDD			TEGRA_GPIO_PW0
#define SMBA1002_BL_PWM			TEGRA_GPIO_PU3 /* PWM */
#define SMBA1002_HDMI_ENB		TEGRA_GPIO_PV5 /* unconfirmed */ // Does smba1002 have HDMI enbl?
#define SMBA1002_HDMI_HPD		TEGRA_GPIO_PN7 /* 1=HDMI plug detected */

#define SMBA1002_BL_PWM_ID		0				/* PWM0 controls backlight */

#define SMBA1002_FB_PAGES		2				/* At least, 2 video pages */
#define SMBA1002_FB_HDMI_PAGES	2				/* At least, 2 video pages for HDMI */

// smba1002 memory is 2xSZ_512M
#define SMBA1002_MEM_SIZE 		SZ_512M			/* Total memory */
#define SMBA1002_MEM_BANKS		1

/*#define SMBA1002_GPU_MEM_SIZE 	SZ_128M*/		/* Memory reserved for GPU */
/*#define SMBA1002_GPU_MEM_SIZE 	SZ_64M*/		/* Memory reserved for GPU */
#define SMBA1002_GPU_MEM_SIZE 	(3*SZ_32M)		/* Memory reserved for GPU */

#define SMBA1002_FB1_MEM_SIZE 	SZ_4M			/* Memory reserved for Framebuffer 1: LCD */
#define SMBA1002_FB2_MEM_SIZE 	SZ_2M			/* Memory reserved for Framebuffer 2: HDMI out */

#define DYNAMIC_GPU_MEM 1						/* use dynamic memory for GPU */

#define SMBA1002_1024x600PANEL1 /* The smba1002 default panel */

/* maximum allowed HDMI resolution */

#define SMBA1002_1280x720HDMI


/*#define SMBA1002_48KHZ_AUDIO*/ /* <- define this if you want 48khz audio sampling rate instead of 44100Hz */


// TPS6586x GPIOs as registered 
#define PMU_GPIO_BASE		(TEGRA_NR_GPIOS) 
#define PMU_GPIO0 		(PMU_GPIO_BASE)
#define PMU_GPIO1 		(PMU_GPIO_BASE + 1) 
#define PMU_GPIO2 		(PMU_GPIO_BASE + 2)
#define PMU_GPIO3 		(PMU_GPIO_BASE + 3)

#define ALC5623_GPIO_BASE	(TEGRA_NR_GPIOS + 16)
#define ALC5623_GP0		(ALC5623_GPIO_BASE)

#define PMU_IRQ_BASE		(TEGRA_NR_IRQS)
#define PMU_IRQ_RTC_ALM1 	(TPS6586X_INT_BASE + TPS6586X_INT_RTC_ALM1)

#define	SMBA1002_ENABLE_VDD_VID	TEGRA_GPIO_PD1	/* 1=enabled.  Powers HDMI. Wait 500uS to let it stabilize before returning */

// TODO: Find whether there are any definitions for these?
/*#define SMBA1002_SDIO0_CD		TEGRA_GPIO_PI5
#define SMBA1002_SDIO0_POWER	TEGRA_GPIO_PD0*/	/* SDIO0 and SDIO2 power */

#define SMBA1002_SDHC_CD		TEGRA_GPIO_PI5
#define SMBA1002_SDHC_WP		-1	/*1=Write Protected */
#define SMBA1002_SDHC_POWER	TEGRA_GPIO_PD0

#define SMBA1002_TS_IRQ		TEGRA_GPIO_PJ7
#define SMBA1002_TS_RESET	TEGRA_GPIO_PH1
#define SMBA1002_TS_POWER	TEGRA_GPIO_PK2
//#define SMBA1002_TS_DISABLE	TEGRA_GPIO_PAA6 /* 0=enabled */

//#define SMBA1002_FB_NONROTATE TEGRA_GPIO_PH1 /*1 = screen rotation locked */

#define SMBA1002_WLAN_POWER 	TEGRA_GPIO_PK5
#define SMBA1002_WLAN_RESET 	TEGRA_GPIO_PK6

#define SMBA1002_BT_RST		TEGRA_GPIO_PU0

#define SMBA1002_LOW_BATT	TEGRA_GPIO_PW3 /*(0=low battery)*/
#define SMBA1002_IN_S3		TEGRA_GPIO_PAA7 /*1 = in S3 */

#define SMBA1002_USB0_VBUS		TEGRA_GPIO_PB0		/* 1= VBUS usb0 */
#define SMBA1002_USB1_RESET		TEGRA_GPIO_PV1	/* 0= reset */

#define SMBA1002_HP_DETECT	TEGRA_GPIO_PW2 	/* HeadPhone detect for audio codec: 1=Hedphone plugged */

#define SMBA1002_NVEC_REQ	TEGRA_GPIO_PD0	/* Set to 0 to send a command to the NVidia Embedded controller */
#define SMBA1002_NVEC_I2C_ADDR 0x8a 			/* I2C address of Tegra, when acting as I2C slave */

#define SMBA1002_WAKE_KEY_POWER  TEGRA_WAKE_GPIO_PV2
#define SMBA1002_WAKE_KEY_RESUME TEGRA_WAKE_GPIO_PV2

#define SMBA1002_TEMP_ALERT	TEGRA_GPIO_PN6

/* The switch used to indicate rotation lock */
//#define SW_ROTATION_LOCK 	(SW_MAX-1)

extern void smba1002_gps_mag_poweron(void);
extern void smba1002_gps_mag_poweroff(void);
extern void smba1002_gps_mag_init(void);

extern void smba1002_wifi_set_cd(int val);

extern void smba1002_init_emc(void);
extern void smba1002_pinmux_init(void);
extern void smba1002_clks_init(void);

extern int smba1002_usb_register_devices(void);
extern int smba1002_audio_register_devices(void);
extern int smba1002_jack_register_devices(void);
extern int smba1002_gpu_register_devices(void);
extern int smba1002_uart_register_devices(void);
extern int smba1002_spi_register_devices(void);
extern int smba1002_aes_register_devices(void);
extern int smba1002_wdt_register_devices(void);
extern int smba1002_i2c_register_devices(void);
extern int smba1002_power_register_devices(void);
extern int smba1002_keyboard_register_devices(void);
extern int smba1002_touch_register_devices(void);
extern int smba1002_sdhci_register_devices(void);
extern int smba1002_sensors_register_devices(void);
extern int smba1002_wlan_pm_register_devices(void);
extern int smba1002_gps_pm_register_devices(void);
extern int smba1002_gsm_pm_register_devices(void);
//extern int smba1002_bt_pm_register_devices(void);
extern void smba1002_setup_bluesleep(void);
extern void smba1002_bt_rfkill(void);
extern int smba1002_nand_register_devices(void);
extern int smba1002_camera_register_devices(void);

/* Autocalculate framebuffer sizes */

#define TEGRA_ROUND_ALLOC(x) (((x) + 4095) & ((unsigned)(-4096)))
/*Framebuffer Size for default Gtablet Panel*/
#define SMBA1002_FB_SIZE TEGRA_ROUND_ALLOC(1024*600*(16/8)*SMBA1002_FB_PAGES)
/*Frambuffer size for 720p HDMI Framebuffer Output*/
#define SMBA1002_FB_HDMI_SIZE TEGRA_ROUND_ALLOC(1280*720*(32/8)*2)


#endif

/*Limiting Frambuffers to Default Panel and 720p memory output.  Check board-shuttle.h for other options*/
