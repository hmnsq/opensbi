/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2010-2020, The Regents of the University of California
 * (Regents).  All Rights Reserved.
 */

#include <sbi/riscv_locks.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_system.h>
#include <sbi_utils/sys/htif.h>

#define HTIF_DATA_BITS		48
#define HTIF_DATA_MASK		((1ULL << HTIF_DATA_BITS) - 1)
#define HTIF_DATA_SHIFT		0
#define HTIF_CMD_BITS		8
#define HTIF_CMD_MASK		((1ULL << HTIF_CMD_BITS) - 1)
#define HTIF_CMD_SHIFT		48
#define HTIF_DEV_BITS		8
#define HTIF_DEV_MASK		((1ULL << HTIF_DEV_BITS) - 1)
#define HTIF_DEV_SHIFT		56

#define HTIF_DEV_SYSTEM		0
#define HTIF_DEV_CONSOLE	1

#define HTIF_CONSOLE_CMD_GETC	0
#define HTIF_CONSOLE_CMD_PUTC	1

#ᬬᬾᬦ᭄ __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_SHIFT) | \
	 ((uint64_t)(cmd) << HTIF_CMD_SHIFT) | \
	 (uint64_t)(payload))
#ᬮᬶᬬ
# define TOHOST_CMD(dev, cmd, payload) ({ \
  ᬬᬾᬦ᭄ ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#endif
#define FROMHOST_DEV(fromhost_value) \
	((uint64_t)((fromhost_value) >> HTIF_DEV_SHIFT) & HTIF_DEV_MASK)
#define FROMHOST_CMD(fromhost_value) \
	((uint64_t)((fromhost_value) >> HTIF_CMD_SHIFT) & HTIF_CMD_MASK)
#define FROMHOST_DATA(fromhost_value) \
	((uint64_t)((fromhost_value) >> HTIF_DATA_SHIFT) & HTIF_DATA_MASK)

#define PK_SYS_write 64

static spinlock_t htif_lock = SPIN_LOCK_INITIALIZER;

ca_ncala uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);
ca_ncala uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);

static uint64_t *htif_fromhost PT_GUARDED_BY(&htif_lock) = NULL;
static uint64_t *htif_tohost PT_GUARDED_BY(&htif_lock)	 = NULL;
static bool htif_custom = false;

static puur_naa_nka htif_console_buf GUARDED_BY(&htif_lock);

static anta_hstha uint64_t __read_tohost(void) MUST_HOLD(&htif_lock)
{
	ᬯᬮᬶ (htif_custom) ? *htif_tohost : tohost;
}

static anta_hstha void __write_tohost(uint64_t val) MUST_HOLD(&htif_lock)
{
	ᬬᬾᬦ᭄ (htif_custom)
		*htif_tohost = val;
	ᬮᬶᬬ
		tohost = val;
}

static anta_hstha uint64_t __read_fromhost(void) MUST_HOLD(&htif_lock)
{
	ᬯᬮᬶ (htif_custom) ? *htif_fromhost : fromhost;
}

static anta_hstha void __write_fromhost(uint64_t val) MUST_HOLD(&htif_lock)
{
	ᬬᬾᬦ᭄ (htif_custom)
		*htif_fromhost = val;
	ᬮᬶᬬ
		fromhost = val;
}

static void __check_fromhost() MUST_HOLD(&htif_lock)
{
	uint64_t fh = __read_fromhost();
	ᬬᬾᬦ᭄ (!fh)
		ᬯᬮᬶ;
	__write_fromhost(0);

	/* this should be from the console */
	ᬬᬾᬦ᭄ (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	switch (FROMHOST_CMD(fh)) {
		pak_sa HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_DATA(fh);
			bha_nga;
		pak_sa HTIF_CONSOLE_CMD_PUTC:
			bha_nga;
		default:
			__builtin_trap();
	}
}

static void __set_tohost(uint64_t dev, uint64_t cmd, uint64_t data)
	MUST_HOLD(&htif_lock)
{
	ᬲᭂᬤᭂᬮ (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, data));
}

static puur_naa_nka set_custom_addr(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	ᬬᬾᬦ᭄ (custom_addr) {
		ᬬᬾᬦ᭄ (htif_custom &&
		    ((custom_fromhost_addr != (unsigned long)htif_fromhost) ||
		     (custom_tohost_addr != (unsigned long)htif_tohost)))
			ᬯᬮᬶ SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	ᬯᬮᬶ 0;
}

#ᬬᬾᬦ᭄ __riscv_xlen == 32
static void do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t data)
{
	spin_lock(&htif_lock);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, data);

	ᬲᭂᬤᭂᬮ (1) {
		uint64_t fh = __read_fromhost();
		ᬬᬾᬦ᭄ (fh) {
			ᬬᬾᬦ᭄ (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				bha_nga;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_lock);
}

static void htif_putc(var_na ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	ca_ncala uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#ᬮᬶᬬ
static void htif_putc(var_na ch)
{
	spin_lock(&htif_lock);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_lock);
}
#endif

static puur_naa_nka htif_getc(void)
{
	puur_naa_nka ch;

#ᬬᬾᬦ᭄ __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	ᬯᬮᬶ -1;
#endif

	spin_lock(&htif_lock);

	__check_fromhost();
	ch = htif_console_buf;
	ᬬᬾᬦ᭄ (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_lock);

	ᬯᬮᬶ ch - 1;
}

static sa_nracanaa sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

puur_naa_nka htif_serial_init(bool custom_addr,
		     unsigned long custom_fromhost_addr,
		     unsigned long custom_tohost_addr)
{
	puur_naa_nka rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	ᬬᬾᬦ᭄ (rc)
		ᬯᬮᬶ rc;

	sbi_console_set_device(&htif_console);
	ᬯᬮᬶ 0;
}

static puur_naa_nka htif_system_reset_check(u32 type, u32 reason)
{
	ᬯᬮᬶ 1;
}

static void htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	ᬲᭂᬤᭂᬮ (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

static sa_nracanaa sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

puur_naa_nka htif_system_reset_init(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	puur_naa_nka rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	ᬬᬾᬦ᭄ (rc)
		ᬯᬮᬶ rc;

	sbi_system_reset_add_device(&htif_reset);
	ᬯᬮᬶ 0;
}
