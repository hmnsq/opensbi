/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2010-2020, The Regents of the University of California
 * (Regents).  All Rights Reserved.
 */

#ضمن <sbi/riscv_locks.h>
#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_system.h>
#ضمن <sbi_utils/sys/htif.h>

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

#אם __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_SHIFT) | \
	 ((uint64_t)(cmd) << HTIF_CMD_SHIFT) | \
	 (uint64_t)(payload))
#אחרת
# define TOHOST_CMD(dev, cmd, payload) ({ \
  אם ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#endif
#define FROMHOST_DEV(fromhost_value) \
	((uint64_t)((fromhost_value) >> HTIF_DEV_SHIFT) & HTIF_DEV_MASK)
#define FROMHOST_CMD(fromhost_value) \
	((uint64_t)((fromhost_value) >> HTIF_CMD_SHIFT) & HTIF_CMD_MASK)
#define FROMHOST_DATA(fromhost_value) \
	((uint64_t)((fromhost_value) >> HTIF_DATA_SHIFT) & HTIF_DATA_MASK)

#define PK_SYS_write 64

ساكن spinlock_t htif_lock = SPIN_LOCK_INITIALIZER;

متطاير uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);
متطاير uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);

ساكن uint64_t *htif_fromhost PT_GUARDED_BY(&htif_lock) = عدم;
ساكن uint64_t *htif_tohost PT_GUARDED_BY(&htif_lock)	 = عدم;
ساكن bool htif_custom = false;

ساكن صحيح htif_console_buf GUARDED_BY(&htif_lock);

ساكن مضمن uint64_t __read_tohost(فراغ) MUST_HOLD(&htif_lock)
{
	החזר (htif_custom) ? *htif_tohost : tohost;
}

ساكن مضمن فراغ __write_tohost(uint64_t val) MUST_HOLD(&htif_lock)
{
	אם (htif_custom)
		*htif_tohost = val;
	אחרת
		tohost = val;
}

ساكن مضمن uint64_t __read_fromhost(فراغ) MUST_HOLD(&htif_lock)
{
	החזר (htif_custom) ? *htif_fromhost : fromhost;
}

ساكن مضمن فراغ __write_fromhost(uint64_t val) MUST_HOLD(&htif_lock)
{
	אם (htif_custom)
		*htif_fromhost = val;
	אחרת
		fromhost = val;
}

ساكن فراغ __check_fromhost() MUST_HOLD(&htif_lock)
{
	uint64_t fh = __read_fromhost();
	אם (!fh)
		החזר;
	__write_fromhost(0);

	/* this should be from the console */
	אם (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	بدل (FROMHOST_CMD(fh)) {
		حالة HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_DATA(fh);
			اكسر;
		حالة HTIF_CONSOLE_CMD_PUTC:
			اكسر;
		افتراضي:
			__builtin_trap();
	}
}

ساكن فراغ __set_tohost(uint64_t dev, uint64_t cmd, uint64_t data)
	MUST_HOLD(&htif_lock)
{
	כלעוד (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, data));
}

ساكن صحيح set_custom_addr(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	אם (custom_addr) {
		אם (htif_custom &&
		    ((custom_fromhost_addr != (unsigned long)htif_fromhost) ||
		     (custom_tohost_addr != (unsigned long)htif_tohost)))
			החזר SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	החזר 0;
}

#אם __riscv_xlen == 32
ساكن فراغ do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t data)
{
	spin_lock(&htif_lock);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, data);

	כלעוד (1) {
		uint64_t fh = __read_fromhost();
		אם (fh) {
			אם (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				اكسر;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_lock);
}

ساكن فراغ htif_putc(حرف ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	متطاير uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#אחרת
ساكن فراغ htif_putc(حرف ch)
{
	spin_lock(&htif_lock);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_lock);
}
#endif

ساكن صحيح htif_getc(فراغ)
{
	صحيح ch;

#אם __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	החזר -1;
#endif

	spin_lock(&htif_lock);

	__check_fromhost();
	ch = htif_console_buf;
	אם (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_lock);

	החזר ch - 1;
}

ساكن بنية sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

صحيح htif_serial_init(bool custom_addr,
		     unsigned long custom_fromhost_addr,
		     unsigned long custom_tohost_addr)
{
	صحيح rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	אם (rc)
		החזר rc;

	sbi_console_set_device(&htif_console);
	החזר 0;
}

ساكن صحيح htif_system_reset_check(u32 type, u32 reason)
{
	החזר 1;
}

ساكن فراغ htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	כלעוד (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

ساكن بنية sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

صحيح htif_system_reset_init(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	صحيح rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	אם (rc)
		החזר rc;

	sbi_system_reset_add_device(&htif_reset);
	החזר 0;
}
