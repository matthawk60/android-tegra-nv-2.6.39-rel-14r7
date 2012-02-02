/*
 * arch/arm/mach-tegra/board-smba1002.c
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

#include <linux/console.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/serial_8250.h>
#include <linux/clk.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/dma-mapping.h>
#include <linux/fsl_devices.h>
#include <linux/platform_data/tegra_usb.h>
#include <linux/pda_power.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/reboot.h>
#include <linux/i2c-tegra.h>
#include <linux/memblock.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/time.h>
#include <asm/setup.h>

#include <mach/io.h>
#include <mach/w1.h>
#include <mach/iomap.h>
#include <mach/irqs.h>
#include <mach/nand.h>
#include <mach/iomap.h>
#include <mach/sdhci.h>
#include <mach/gpio.h>
#include <mach/clk.h>
#include <mach/usb_phy.h>
#include <mach/i2s.h>
#include <mach/system.h>
#include <mach/nvmap.h>

#include "board.h"
#include "board-smba1002.h"
#include "clock.h"
#include "gpio-names.h"
#include "devices.h"
#include "pm.h"
#include "wakeups-t2.h"
#include "wdt-recovery.h"


/* NVidia bootloader tags */
#define ATAG_NVIDIA		0x41000801

#define ATAG_NVIDIA_RM			0x1
#define ATAG_NVIDIA_DISPLAY		0x2
#define ATAG_NVIDIA_FRAMEBUFFER		0x3
#define ATAG_NVIDIA_CHIPSHMOO		0x4
#define ATAG_NVIDIA_CHIPSHMOOPHYS	0x5
#define ATAG_NVIDIA_CARVEOUT		0x6
#define ATAG_NVIDIA_WARMBOOT		0x7

#define ATAG_NVIDIA_PRESERVED_MEM_0	0x10000
#define ATAG_NVIDIA_PRESERVED_MEM_N	3
#define ATAG_NVIDIA_FORCE_32		0x7fffffff


struct tag_tegra {
	__u32 bootarg_key;
	__u32 bootarg_len;
	char bootarg[1];
};

/**
 * Resource Manager boot args.
 *
 * Nothing here yet.
 */
struct NVBOOTARGS_Rm
{
    u32 	reserved;
};

/**
 * Carveout boot args, which define the physical memory location of the GPU
 * carved-out memory region(s).
 */
struct NVBOOTARGS_Carveout
{
    void* 	base;
    u32 	size;
};

/**
 * Warmbootloader boot args. This structure only contains
 * a mem handle key to preserve the warm bootloader
 * across the bootloader->os transition
 */
struct NVBOOTARGS_Warmboot
{
    /* The key used for accessing the preserved memory handle */
    u32 	MemHandleKey;
};

/**
 * PreservedMemHandle boot args, indexed by ATAG_NVIDIA_PRESERVED_MEM_0 + n.
 * This allows physical memory allocations (e.g., for framebuffers) to persist
 * between the bootloader and operating system.  Only carveout and IRAM
 * allocations may be preserved with this interface.
 */
struct NVBOOTARGS_PreservedMemHandle
{
    u32 	Address;
    u32   	Size;
};

/**
 * Display boot args.
 *
 * The bootloader may have a splash screen. This will flag which controller
 * and device was used for the splash screen so the device will not be
 * reinitialized (which causes visual artifacts).
 */
struct NVBOOTARGS_Display
{
    /* which controller is initialized */
    u32 	Controller;

    /* index into the ODM device list of the boot display device */
    u32 	DisplayDeviceIndex;

    /* set to != 0 if the display has been initialized */
    u8 		bEnabled;
};

/**
 * Framebuffer boot args
 *
 * A framebuffer may be shared between the bootloader and the
 * operating system display driver.  When this key is present,
 * a preserved memory handle for the framebuffer must also
 * be present, to ensure that no display corruption occurs
 * during the transition.
 */
struct NVBOOTARGS_Framebuffer
{
    /*  The key used for accessing the preserved memory handle */
    u32 	MemHandleKey;
    /*  Total memory size of the framebuffer */
    u32 	Size;
    /*  Color format of the framebuffer, cast to a U32  */
    u32 	ColorFormat;
    /*  Width of the framebuffer, in pixels  */
    u16 	Width;
    /*  Height of each surface in the framebuffer, in pixels  */
    u16 	Height;
    /*  Pitch of a framebuffer scanline, in bytes  */
    u16 	Pitch;
    /*  Surface layout of the framebuffer, cast to a U8 */
    u8  	SurfaceLayout;
    /*  Number of contiguous surfaces of the same height in the
        framebuffer, if multi-buffering.  Each surface is
        assumed to begin at Pitch * Height bytes from the
        previous surface.  */
    u8  	NumSurfaces;
    /* Flags for future expandability.
       Current allowable flags are:
       zero - default
       NV_BOOT_ARGS_FB_FLAG_TEARING_EFFECT - use a tearing effect signal in
            combination with a trigger from the display software to generate
            a frame of pixels for the display device. */
    u32 	Flags;
#define NVBOOTARG_FB_FLAG_TEARING_EFFECT (0x1)

};

/**
 * Chip characterization shmoo data
 */
struct NVBOOTARGS_ChipShmoo
{
    /* The key used for accessing the preserved memory handle of packed
       characterization tables  */
    u32 	MemHandleKey;

    /* Offset and size of each unit in the packed buffer */
    u32 	CoreShmooVoltagesListOffset;
    u32 	CoreShmooVoltagesListSize;

    u32 	CoreScaledLimitsListOffset;
    u32 	CoreScaledLimitsListSize;

    u32 	OscDoublerListOffset;
    u32 	OscDoublerListSize;

    u32 	SKUedLimitsOffset;
    u32 	SKUedLimitsSize;

    u32 	CpuShmooVoltagesListOffset;
    u32 	CpuShmooVoltagesListSize;

    u32 	CpuScaledLimitsOffset;
    u32 	CpuScaledLimitsSize;

    /* Misc characterization settings */
    u16 	CoreCorner;
    u16 	CpuCorner;
    u32 	Dqsib;
    u32 	SvopLowVoltage;
    u32 	SvopLowSetting;
    u32 	SvopHighSetting;
};

/**
 * Chip characterization shmoo data indexed by NvBootArgKey_ChipShmooPhys
 */
struct NVBOOTARGS_ChipShmooPhys
{
    u32 	PhysShmooPtr;
    u32 	Size;
};


/**
 * OS-agnostic bootarg structure.
 */
struct NVBOOTARGS
{
    struct NVBOOTARGS_Rm 					RmArgs;
    struct NVBOOTARGS_Display 				DisplayArgs;
    struct NVBOOTARGS_Framebuffer 			FramebufferArgs;
    struct NVBOOTARGS_ChipShmoo 			ChipShmooArgs;
    struct NVBOOTARGS_ChipShmooPhys			ChipShmooPhysArgs;
    struct NVBOOTARGS_Warmboot 				WarmbootArgs;
    struct NVBOOTARGS_PreservedMemHandle 	MemHandleArgs[ATAG_NVIDIA_PRESERVED_MEM_N];
};
 
static struct NVBOOTARGS NvBootArgs = { {0}, {0}, {0}, {0}, {0}, {0}, {{0}} }; 

/*#define _DUMP_WBCODE 0*/
#ifdef _DUMP_WBCODE
u8 tohex(u8 b)
{
	return (b > 9) ? (b + ('A' - 10)) : (b + '0');
}

void dump_warmboot(u32 from,u32 size)
{
	u32 i,p;
	u8 buf[3*16+5];
	void __iomem *from_io = ioremap(from, size);
	
	if (!from_io) {
		pr_err("%s: Failed to map source framebuffer\n", __func__);
		return;
	}
	
	// Limit dump size
	if (size > 1024)
		size = 1024;
	
	for (i = 0,p = 0; i < size; i+= 4) {
		u32 val = readl(from_io + i);
		buf[p   ] = tohex((val  >> 4) & 0xF);
		buf[p+ 1] = tohex((val      ) & 0xF);
		buf[p+ 2] = ' ';
		buf[p+ 3] = tohex((val  >>12) & 0xF);
		buf[p+ 4] = tohex((val  >>8 ) & 0xF);
		buf[p+ 5] = ' ';
		buf[p+ 6] = tohex((val  >>20) & 0xF);
		buf[p+ 7] = tohex((val  >>16) & 0xF);
		buf[p+ 8] = ' ';
		buf[p+ 9] = tohex((val  >>28) & 0xF);
		buf[p+10] = tohex((val  >>24) & 0xF);
		buf[p+11] = ' ';
		p+=12;
		if (p >= 48) {
			p = 0;
			buf[48] = 0;
			pr_info("%08x: %s\n",i-12,buf);
		}
	}
	iounmap(from_io);
}
#endif

static int __init get_cfg_from_tags(void)
{
	/* If the bootloader framebuffer is found, use it */
	if (tegra_bootloader_fb_start == 0 && tegra_bootloader_fb_size == 0 &&
		NvBootArgs.FramebufferArgs.MemHandleKey >= ATAG_NVIDIA_PRESERVED_MEM_0 &&
        NvBootArgs.FramebufferArgs.MemHandleKey <  (ATAG_NVIDIA_PRESERVED_MEM_0+ATAG_NVIDIA_PRESERVED_MEM_N) &&
		NvBootArgs.FramebufferArgs.Size != 0 &&
		NvBootArgs.MemHandleArgs[NvBootArgs.FramebufferArgs.MemHandleKey - ATAG_NVIDIA_PRESERVED_MEM_0].Size != 0) 
	{
		/* Got the bootloader framebuffer address and size. Store it */
		tegra_bootloader_fb_start = NvBootArgs.MemHandleArgs[NvBootArgs.FramebufferArgs.MemHandleKey - ATAG_NVIDIA_PRESERVED_MEM_0].Address;
		tegra_bootloader_fb_size  = NvBootArgs.MemHandleArgs[NvBootArgs.FramebufferArgs.MemHandleKey - ATAG_NVIDIA_PRESERVED_MEM_0].Size;
		
		pr_info("Nvidia TAG: framebuffer: %lu @ 0x%08lx\n",tegra_bootloader_fb_size,tegra_bootloader_fb_start);
	}
	
	/* If the LP0 vector is found, use it */
	if (tegra_lp0_vec_start == 0 && tegra_lp0_vec_size == 0 &&
		NvBootArgs.WarmbootArgs.MemHandleKey >= ATAG_NVIDIA_PRESERVED_MEM_0 &&
        NvBootArgs.WarmbootArgs.MemHandleKey <  (ATAG_NVIDIA_PRESERVED_MEM_0+ATAG_NVIDIA_PRESERVED_MEM_N) &&
		NvBootArgs.MemHandleArgs[NvBootArgs.WarmbootArgs.MemHandleKey - ATAG_NVIDIA_PRESERVED_MEM_0].Size != 0) 
	{
		/* Got the Warmboot block address and size. Store it */
		tegra_lp0_vec_start = NvBootArgs.MemHandleArgs[NvBootArgs.WarmbootArgs.MemHandleKey - ATAG_NVIDIA_PRESERVED_MEM_0].Address;
		tegra_lp0_vec_size  = NvBootArgs.MemHandleArgs[NvBootArgs.WarmbootArgs.MemHandleKey - ATAG_NVIDIA_PRESERVED_MEM_0].Size;

		pr_info("Nvidia TAG: LP0: %lu @ 0x%08lx\n",tegra_lp0_vec_size,tegra_lp0_vec_start);		
		
		/* Until we find out if the bootloader supports the workaround required to implement
		   LP0, disable it */
		//tegra_lp0_vec_start = tegra_lp0_vec_size = 0;

	}
	
	return 0;
}

static int __init parse_tag_nvidia(const struct tag *tag)
{
    const char *addr = (const char *)&tag->hdr + sizeof(struct tag_header);
    const struct tag_tegra *nvtag = (const struct tag_tegra*)addr;

    if (nvtag->bootarg_key >= ATAG_NVIDIA_PRESERVED_MEM_0 &&
        nvtag->bootarg_key <  (ATAG_NVIDIA_PRESERVED_MEM_0+ATAG_NVIDIA_PRESERVED_MEM_N) )
    {
        int Index = nvtag->bootarg_key - ATAG_NVIDIA_PRESERVED_MEM_0;
		
        struct NVBOOTARGS_PreservedMemHandle *dst = 
			&NvBootArgs.MemHandleArgs[Index];
        const struct NVBOOTARGS_PreservedMemHandle *src = 
			(const struct NVBOOTARGS_PreservedMemHandle *) nvtag->bootarg;

        if (nvtag->bootarg_len != sizeof(*dst)) {
            pr_err("Unexpected preserved memory handle tag length (expected: %d, got: %d!\n",
				sizeof(*dst), nvtag->bootarg_len);
        } else {
		
			pr_debug("Preserved memhandle: 0x%08x, address: 0x%08x, size: %d\n",
				nvtag->bootarg_key, src->Address, src->Size);
				
			memcpy(dst,src,sizeof(*dst));
		}
        return get_cfg_from_tags();
    }

    switch (nvtag->bootarg_key) {
    case ATAG_NVIDIA_CHIPSHMOO:
    {
        struct NVBOOTARGS_ChipShmoo *dst = 
			&NvBootArgs.ChipShmooArgs;
        const struct NVBOOTARGS_ChipShmoo *src = 
			(const struct NVBOOTARGS_ChipShmoo *)nvtag->bootarg;

        if (nvtag->bootarg_len != sizeof(*dst)) {
            pr_err("Unexpected preserved memory handle tag length (expected: %d, got: %d!\n",
				sizeof(*dst), nvtag->bootarg_len);
        } else {
            pr_debug("Shmoo tag with 0x%08x handle\n", src->MemHandleKey);
			memcpy(dst,src,sizeof(*dst));
		}
        return get_cfg_from_tags();
    }
    case ATAG_NVIDIA_DISPLAY:
    {
        struct NVBOOTARGS_Display *dst = 
			&NvBootArgs.DisplayArgs;
        const struct NVBOOTARGS_Display *src = 
			(const struct NVBOOTARGS_Display *)nvtag->bootarg;

        if (nvtag->bootarg_len != sizeof(*dst)) {
            pr_err("Unexpected display tag length (expected: %d, got: %d!\n",
				sizeof(*dst), nvtag->bootarg_len);
        } else {
			memcpy(dst,src,sizeof(*dst));
		}
        return get_cfg_from_tags();
    }
    case ATAG_NVIDIA_FRAMEBUFFER:
    {
        struct NVBOOTARGS_Framebuffer *dst = 
			&NvBootArgs.FramebufferArgs;
        const struct NVBOOTARGS_Framebuffer *src = 
			(const struct NVBOOTARGS_Framebuffer *)nvtag->bootarg;

        if (nvtag->bootarg_len != sizeof(*dst)) {
            pr_err("Unexpected framebuffer tag length (expected: %d, got: %d!\n",
				sizeof(*dst), nvtag->bootarg_len);
        } else {
            pr_debug("Framebuffer tag with 0x%08x handle, size: %d\n",
                   src->MemHandleKey,src->Size);
			memcpy(dst,src,sizeof(*dst));
		}
        return get_cfg_from_tags();
    }
    case ATAG_NVIDIA_RM:
    {
        struct NVBOOTARGS_Rm *dst = 
			&NvBootArgs.RmArgs;
        const struct NVBOOTARGS_Rm *src = 
			(const struct NVBOOTARGS_Rm *)nvtag->bootarg;

        if (nvtag->bootarg_len != sizeof(*dst)) {
            pr_err("Unexpected RM tag length (expected: %d, got: %d!\n",
				sizeof(*dst), nvtag->bootarg_len);
        } else {
			memcpy(dst,src,sizeof(*dst));
		}

        return get_cfg_from_tags();
    }
    case ATAG_NVIDIA_CHIPSHMOOPHYS:
    {
        struct NVBOOTARGS_ChipShmooPhys *dst = 
			&NvBootArgs.ChipShmooPhysArgs;
        const struct NVBOOTARGS_ChipShmooPhys *src =
            (const struct NVBOOTARGS_ChipShmooPhys *)nvtag->bootarg;

        if (nvtag->bootarg_len != sizeof(*dst)) {
            pr_err("Unexpected phys shmoo tag length (expected: %d, got: %d!\n",
				sizeof(*dst), nvtag->bootarg_len);
        } else {
            pr_debug("Phys shmoo tag with pointer 0x%x and length %u\n",
                   src->PhysShmooPtr, src->Size);
			memcpy(dst,src,sizeof(*dst));
        }
        return get_cfg_from_tags();
    }
    case ATAG_NVIDIA_WARMBOOT:
    {
        struct NVBOOTARGS_Warmboot *dst = 
			&NvBootArgs.WarmbootArgs;
        const struct NVBOOTARGS_Warmboot *src =
            (const struct NVBOOTARGS_Warmboot *)nvtag->bootarg;

        if (nvtag->bootarg_len != sizeof(*dst)) {
            pr_err("Unexpected Warnboot tag length (expected: %d, got: %d!\n",
				sizeof(*dst), nvtag->bootarg_len);
        } else {
            pr_debug("Found a warmboot tag with handle 0x%08x!\n", src->MemHandleKey);
            memcpy(dst,src,sizeof(*dst));
        }
        return get_cfg_from_tags();
    }

    default:
        return get_cfg_from_tags();
    } 
	return get_cfg_from_tags();
}
__tagtable(ATAG_NVIDIA, parse_tag_nvidia);

/* #define _DUMP_BOOTCAUSE 0 */
#ifdef _DUMP_BOOTCAUSE

static void __iomem *pmc = IO_ADDRESS(TEGRA_PMC_BASE);
#define PMC_SCRATCH0		0x50
#define PMC_SCRATCH1		0x54
#define PMC_SCRATCH38		0x134
#define PMC_SCRATCH39		0x138
#define PMC_SCRATCH41		0x140 

void dump_bootflags(void)
{
	pr_info("PMC_SCRATCH0: 0x%08x | PMC_SCRATCH1: 0x%08x | PMC_SCRATCH41: 0x%08x\n",
		readl(pmc + PMC_SCRATCH0),
		readl(pmc + PMC_SCRATCH1),
		readl(pmc + PMC_SCRATCH41)
	);


}
#endif

static struct clk *wifi_32k_clk;
int smba1002_bt_wifi_gpio_init(void)
{
	static bool inited = 0;
	// Check to see if we've already been init'ed.
	if (inited) 
		return 0;
	wifi_32k_clk = clk_get_sys(NULL, "blink");
        if (IS_ERR(wifi_32k_clk)) {
                pr_err("%s: unable to get blink clock\n", __func__);
                return -1;
        }
	gpio_request(SMBA1002_WLAN_POWER, "bt_wifi_power");
        tegra_gpio_enable(SMBA1002_WLAN_POWER);
	gpio_direction_output(SMBA1002_WLAN_POWER, 0);
	inited = 1;
	return 0;	
}
EXPORT_SYMBOL_GPL(smba1002_bt_wifi_gpio_init);

int smba1002_bt_wifi_gpio_set(bool on)
{
       static int count = 0;
	if (IS_ERR(wifi_32k_clk)) {
		pr_err("%s: Clock wasn't obtained\n", __func__);
		return -1;
	}
				
	if (on) {
		if (count == 0) {
			gpio_set_value(SMBA1002_WLAN_POWER, 1);
        		mdelay(100);
			clk_enable(wifi_32k_clk);
		}
		count++;
	} else {
		if (count == 0) {
			pr_err("%s: Unbalanced wifi/bt power disable requests\n", __func__);
			return -1;
		} else if (count == 1) {
			        gpio_set_value(SMBA1002_WLAN_POWER, 0);
        			mdelay(100);
				clk_disable(wifi_32k_clk);
		} 
		--count;
	}
	return 0;		
}
EXPORT_SYMBOL_GPL(smba1002_bt_wifi_gpio_set);




static void smba1002_board_suspend(int lp_state, enum suspend_stage stg)
{
	if ((lp_state == TEGRA_SUSPEND_LP1) && (stg == TEGRA_SUSPEND_BEFORE_CPU))
		tegra_console_uart_suspend();
}

static void smba1002_board_resume(int lp_state, enum resume_stage stg)
{
	if ((lp_state == TEGRA_SUSPEND_LP1) && (stg == TEGRA_RESUME_AFTER_CPU))
		tegra_console_uart_resume();
}

static struct tegra_suspend_platform_data smba1002_suspend = {
	.cpu_timer 	  	= 2000,  	// 5000
	.cpu_off_timer 	= 100, 		// 5000
	.core_timer    	= 0x7e7e,	//
	.core_off_timer = 0xf,		// 0x7f
    .corereq_high 	= false,
	.sysclkreq_high = true,
	.suspend_mode 	= TEGRA_SUSPEND_LP1,
	.cpu_lp2_min_residency = 2000,	
	.board_suspend = smba1002_board_suspend,
	.board_resume = smba1002_board_resume, 	
};

#ifdef CONFIG_ANDROID_RAM_CONSOLE
static struct resource ram_console_resources[] = {
	{
		.flags = IORESOURCE_MEM,
	},
 };
 
 static struct platform_device ram_console_device = {
	.name           = "ram_console",
	.id             = -1,
	.num_resources  = ARRAY_SIZE(ram_console_resources),
	.resource       = ram_console_resources,
};

static void __init tegra_ramconsole_reserve(unsigned long size)
{
	struct resource *res;
	long ret;

	res = platform_get_resource(&ram_console_device, IORESOURCE_MEM, 0);
	if (!res) {
		pr_err("Failed to find memory resource for ram console\n");
		return;
	}
	res->start = memblock_end_of_DRAM() - size;
	res->end = res->start + size - 1;
	ret = memblock_remove(res->start, size);
	if (ret) {
		ram_console_device.resource = NULL;
		ram_console_device.num_resources = 0;
		pr_err("Failed to reserve memory block for ram console\n");
	}
}
#endif

static void __init tegra_smba1002_init(void)
{
	struct clk *clk;

	/* force consoles to stay enabled across suspend/resume */
	// console_suspend_enabled = 0;

	/* Init the suspend information */
	tegra_init_suspend(&smba1002_suspend);

	/* Set the SDMMC1 (wifi) tap delay to 6.  This value is determined
	 * based on propagation delay on the PCB traces. */
	clk = clk_get_sys("sdhci-tegra.0", NULL);
	if (!IS_ERR(clk)) {
		tegra_sdmmc_tap_delay(clk, 6);
		clk_put(clk);
	} else {
		pr_err("Failed to set wifi sdmmc tap delay\n");
	}

	/* Initialize the pinmux */
	smba1002_pinmux_init();

	/* Initialize the clocks - clocks require the pinmux to be initialized first */
	smba1002_clks_init();

	/* Register i2c devices - required for Power management and MUST be done before the power register */
	smba1002_i2c_register_devices();

	/* Register the power subsystem - Including the poweroff handler - Required by all the others */
	smba1002_power_register_devices();
	
	/* Register the USB device */
	smba1002_usb_register_devices();

	/* Register UART devices */
	smba1002_uart_register_devices();
	
	/* Register SPI devices */
	smba1002_spi_register_devices();

	/* Register GPU devices */
	smba1002_gpu_register_devices();

	/* Register Audio devices */
	smba1002_audio_register_devices();

	/* Register Jack devices */
//	smba1002_jack_register_devices();

	/* Register AES encryption devices */
	smba1002_aes_register_devices();

	/* Register Watchdog devices */
	smba1002_wdt_register_devices();

	/* Register all the keyboard devices */
	smba1002_keyboard_register_devices();
	
	/* Register touchscreen devices */
	smba1002_touch_register_devices();
	
	/* Register SDHCI devices */
	smba1002_sdhci_register_devices();

	/* Register accelerometer device */
	smba1002_sensors_register_devices();
	
	/* Register wlan powermanagement devices */
//	smba1002_wlan_pm_register_devices();
	
	/* Register gps powermanagement devices */
	//smba1002_gps_pm_register_devices();

	/* Register gsm powermanagement devices */
	//smba1002_gsm_pm_register_devices();
	
	/* Register Bluetooth powermanagement devices */
	smba1002_bt_rfkill();
	smba1002_setup_bluesleep();

	/* Register Camera powermanagement devices */
//	smba1002_camera_register_devices();

	/* Register NAND flash devices */
	smba1002_nand_register_devices();
	
	
	tegra_release_bootloader_fb();
#ifdef CONFIG_TEGRA_WDT_RECOVERY
	tegra_wdt_recovery_init();
#endif
#if 0
	/* Finally, init the external memory controller and memory frequency scaling
   	   NB: This is not working on SMBA1002. And seems there is no point in fixing it,
	   as the EMC clock is forced to the maximum speed as soon as the 2D/3D engine
	   starts.*/
	smba1002_init_emc();
#endif

#ifdef _DUMP_WBCODE
	dump_warmboot(tegra_lp0_vec_start,tegra_lp0_vec_size);
#endif

#ifdef _DUMP_BOOTCAUSE
	dump_bootflags();
#endif

#ifdef CONFIG_ANDROID_RAM_CONSOLE
	/* Register the RAM console device */
	platform_device_register(&ram_console_device);
#endif

	/* Release the tegra bootloader framebuffer */
	tegra_release_bootloader_fb();
}

static void __init tegra_smba1002_reserve(void)
{
	if (memblock_reserve(0x0, 4096) < 0)
		pr_warn("Cannot reserve first 4K of memory for safety\n");

	/* Reserve the graphics memory */		
#if defined(DYNAMIC_GPU_MEM)
#if defined(CONFIG_NVMAP_CONVERT_CARVEOUT_TO_IOVMM)	
	tegra_reserve(0, SMBA1002_FB1_MEM_SIZE, SMBA1002_FB2_MEM_SIZE);
#else
	tegra_reserve(SMBA1002_GPU_MEM_SIZE, SMBA1002_FB1_MEM_SIZE, SMBA1002_FB2_MEM_SIZE);
#endif
#endif

#ifdef CONFIG_ANDROID_RAM_CONSOLE
	/* Reserve 1M memory for the RAM console */
	tegra_ramconsole_reserve(SZ_1M);
#endif
}

static void __init tegra_smba1002_fixup(struct machine_desc *desc,
	struct tag *tags, char **cmdline, struct meminfo *mi)
{
	mi->nr_banks = SMBA1002_MEM_BANKS;
	mi->bank[0].start = PHYS_OFFSET;
#if defined(DYNAMIC_GPU_MEM)
	mi->bank[0].size  = SMBA1002_MEM_SIZE;
#else
	mi->bank[0].size  = SMBA1002_MEM_SIZE - SMBA1002_GPU_MEM_SIZE;
#endif
} 

/* the Shuttle bootloader identifies itself as MACH_TYPE_HARMONY [=2731]
   or as MACH_TYPE_LEGACY[=3333]. We MUST handle both cases in order
   to make the kernel bootable */
MACHINE_START(HARMONY, "harmony")
	.boot_params	= 0x00000100,
	.map_io         = tegra_map_common_io,
	.init_early     = tegra_init_early,
	.init_irq       = tegra_init_irq,
	.timer          = &tegra_timer,
	.init_machine	= tegra_smba1002_init,
	.reserve		= tegra_smba1002_reserve,
	.fixup			= tegra_smba1002_fixup,
MACHINE_END

#ifdef MACH_TYPE_TEGRA_LEGACY
MACHINE_START(TEGRA_LEGACY, "tegra_legacy")
#else
MACHINE_START(LEGACY, "legacy")
#endif
	.boot_params	= 0x00000100,
	.map_io         = tegra_map_common_io,
	.init_early     = tegra_init_early,
	.init_irq       = tegra_init_irq,
	.timer          = &tegra_timer, 	
	.init_machine	= tegra_smba1002_init,
	.reserve		= tegra_smba1002_reserve,
	.fixup			= tegra_smba1002_fixup,
MACHINE_END






