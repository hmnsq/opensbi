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

#만약 __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_SHIFT) | \
	 ((uint64_t)(cmd) << HTIF_CMD_SHIFT) | \
	 (uint64_t)(payload))
#아니면
# define TOHOST_CMD(dev, cmd, payload) ({ \
  만약 ((dev) || (cmd)) __builtin_trap(); \
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

volatile uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);
volatile uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);

static uint64_t *htif_fromhost PT_GUARDED_BY(&htif_lock) = NULL;
static uint64_t *htif_tohost PT_GUARDED_BY(&htif_lock)	 = NULL;
static bool htif_custom = false;

static 정수 htif_console_buf GUARDED_BY(&htif_lock);

static inline uint64_t __read_tohost(void) MUST_HOLD(&htif_lock)
{
	반환 (htif_custom) ? *htif_tohost : tohost;
}

static inline void __write_tohost(uint64_t val) MUST_HOLD(&htif_lock)
{
	만약 (htif_custom)
		*htif_tohost = val;
	아니면
		tohost = val;
}

static inline uint64_t __read_fromhost(void) MUST_HOLD(&htif_lock)
{
	반환 (htif_custom) ? *htif_fromhost : fromhost;
}

static inline void __write_fromhost(uint64_t val) MUST_HOLD(&htif_lock)
{
	만약 (htif_custom)
		*htif_fromhost = val;
	아니면
		fromhost = val;
}

static void __check_fromhost() MUST_HOLD(&htif_lock)
{
	uint64_t fh = __read_fromhost();
	만약 (!fh)
		반환;
	__write_fromhost(0);

	/* this should be from the console */
	만약 (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	switch (FROMHOST_CMD(fh)) {
		case HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_DATA(fh);
			break;
		case HTIF_CONSOLE_CMD_PUTC:
			break;
		default:
			__builtin_trap();
	}
}

static void __set_tohost(uint64_t dev, uint64_t cmd, uint64_t data)
	MUST_HOLD(&htif_lock)
{
	동안 (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, data));
}

static 정수 set_custom_addr(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	만약 (custom_addr) {
		만약 (htif_custom &&
		    ((custom_fromhost_addr != (unsigned long)htif_fromhost) ||
		     (custom_tohost_addr != (unsigned long)htif_tohost)))
			반환 SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	반환 0;
}

#만약 __riscv_xlen == 32
static void do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t data)
{
	spin_lock(&htif_lock);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, data);

	동안 (1) {
		uint64_t fh = __read_fromhost();
		만약 (fh) {
			만약 (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				break;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_lock);
}

static void htif_putc(char ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	volatile uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#아니면
static void htif_putc(char ch)
{
	spin_lock(&htif_lock);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_lock);
}
#endif

static 정수 htif_getc(void)
{
	정수 ch;

#만약 __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	반환 -1;
#endif

	spin_lock(&htif_lock);

	__check_fromhost();
	ch = htif_console_buf;
	만약 (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_lock);

	반환 ch - 1;
}

static struct sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

정수 htif_serial_init(bool custom_addr,
		     unsigned long custom_fromhost_addr,
		     unsigned long custom_tohost_addr)
{
	정수 rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	만약 (rc)
		반환 rc;

	sbi_console_set_device(&htif_console);
	반환 0;
}

static 정수 htif_system_reset_check(u32 type, u32 reason)
{
	반환 1;
}

static void htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	동안 (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

static struct sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

정수 htif_system_reset_init(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	정수 rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	만약 (rc)
		반환 rc;

	sbi_system_reset_add_device(&htif_reset);
	반환 0;
}
