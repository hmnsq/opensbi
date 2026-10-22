/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2010-2020, The Regents of the University of California
 * (Regents).  All Rights Reserved.
 */

#شامل <sbi/riscv_locks.h>
#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_system.h>
#شامل <sbi_utils/sys/htif.h>

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

#اگر __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_SHIFT) | \
	 ((uint64_t)(cmd) << HTIF_CMD_SHIFT) | \
	 (uint64_t)(payload))
#وگرنه
# define TOHOST_CMD(dev, cmd, payload) ({ \
  اگر ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#endif
#define FROMHOST_DEV(fromhost_مقدار) \
	((uint64_t)((fromhost_مقدار) >> HTIF_DEV_SHIFT) & HTIF_DEV_MASK)
#define FROMHOST_CMD(fromhost_مقدار) \
	((uint64_t)((fromhost_مقدار) >> HTIF_CMD_SHIFT) & HTIF_CMD_MASK)
#define FROMHOST_DATA(fromhost_مقدار) \
	((uint64_t)((fromhost_مقدار) >> HTIF_DATA_SHIFT) & HTIF_DATA_MASK)

#define PK_SYS_write 64

ایستا spinlock_t htif_lock = SPIN_LOCK_INITIALIZER;

فرار uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);
فرار uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);

ایستا uint64_t *htif_fromhost PT_GUARDED_BY(&htif_lock) = پوچ;
ایستا uint64_t *htif_tohost PT_GUARDED_BY(&htif_lock)	 = پوچ;
ایستا bool htif_custom = false;

ایستا صحیح htif_console_buf GUARDED_BY(&htif_lock);

ایستا درونخطی uint64_t __read_tohost(تهی) MUST_HOLD(&htif_lock)
{
	بازگشت (htif_custom) ? *htif_tohost : tohost;
}

ایستا درونخطی تهی __write_tohost(uint64_t val) MUST_HOLD(&htif_lock)
{
	اگر (htif_custom)
		*htif_tohost = val;
	وگرنه
		tohost = val;
}

ایستا درونخطی uint64_t __read_fromhost(تهی) MUST_HOLD(&htif_lock)
{
	بازگشت (htif_custom) ? *htif_fromhost : fromhost;
}

ایستا درونخطی تهی __write_fromhost(uint64_t val) MUST_HOLD(&htif_lock)
{
	اگر (htif_custom)
		*htif_fromhost = val;
	وگرنه
		fromhost = val;
}

ایستا تهی __check_fromhost() MUST_HOLD(&htif_lock)
{
	uint64_t fh = __read_fromhost();
	اگر (!fh)
		بازگشت;
	__write_fromhost(0);

	/* this should be from the console */
	اگر (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	انتخاب (FROMHOST_CMD(fh)) {
		حالت HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_DATA(fh);
			بشکن;
		حالت HTIF_CONSOLE_CMD_PUTC:
			بشکن;
		پیشفرض:
			__builtin_trap();
	}
}

ایستا تهی __set_tohost(uint64_t dev, uint64_t cmd, uint64_t data)
	MUST_HOLD(&htif_lock)
{
	وقتی (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, data));
}

ایستا صحیح set_custom_addr(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	اگر (custom_addr) {
		اگر (htif_custom &&
		    ((custom_fromhost_addr != (unsigned long)htif_fromhost) ||
		     (custom_tohost_addr != (unsigned long)htif_tohost)))
			بازگشت SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	بازگشت 0;
}

#اگر __riscv_xlen == 32
ایستا تهی do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t data)
{
	spin_lock(&htif_lock);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, data);

	وقتی (1) {
		uint64_t fh = __read_fromhost();
		اگر (fh) {
			اگر (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				بشکن;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_lock);
}

ایستا تهی htif_putc(نویسه ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	فرار uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#وگرنه
ایستا تهی htif_putc(نویسه ch)
{
	spin_lock(&htif_lock);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_lock);
}
#endif

ایستا صحیح htif_getc(تهی)
{
	صحیح ch;

#اگر __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	بازگشت -1;
#endif

	spin_lock(&htif_lock);

	__check_fromhost();
	ch = htif_console_buf;
	اگر (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_lock);

	بازگشت ch - 1;
}

ایستا ساختار sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

صحیح htif_serial_init(bool custom_addr,
		     unsigned long custom_fromhost_addr,
		     unsigned long custom_tohost_addr)
{
	صحیح rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	اگر (rc)
		بازگشت rc;

	sbi_console_set_device(&htif_console);
	بازگشت 0;
}

ایستا صحیح htif_system_reset_check(u32 type, u32 reason)
{
	بازگشت 1;
}

ایستا تهی htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	وقتی (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

ایستا ساختار sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

صحیح htif_system_reset_init(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	صحیح rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	اگر (rc)
		بازگشت rc;

	sbi_system_reset_add_device(&htif_reset);
	بازگشت 0;
}
