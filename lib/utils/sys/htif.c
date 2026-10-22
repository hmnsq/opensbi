/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2010-2020, The Regents of the University of California
 * (Regents).  All Rights Reserved.
 */

#அந்தர்பாவ <sbi/riscv_locks.h>
#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi/sbi_system.h>
#அந்தர்பாவ <sbi_utils/sys/htif.h>

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

#எனில் __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_SHIFT) | \
	 ((uint64_t)(cmd) << HTIF_CMD_SHIFT) | \
	 (uint64_t)(payload))
#இல்லையெனில்
# define TOHOST_CMD(dev, cmd, payload) ({ \
  எனில் ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#endif
#define FROMHOST_DEV(fromhost_மதிப்பு) \
	((uint64_t)((fromhost_மதிப்பு) >> HTIF_DEV_SHIFT) & HTIF_DEV_MASK)
#define FROMHOST_CMD(fromhost_மதிப்பு) \
	((uint64_t)((fromhost_மதிப்பு) >> HTIF_CMD_SHIFT) & HTIF_CMD_MASK)
#define FROMHOST_DATA(fromhost_மதிப்பு) \
	((uint64_t)((fromhost_மதிப்பு) >> HTIF_DATA_SHIFT) & HTIF_DATA_MASK)

#define PK_SYS_write 64

நிலையான spinlock_t htif_lock = SPIN_LOCK_INITIALIZER;

நிலையற்ற uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);
நிலையற்ற uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);

நிலையான uint64_t *htif_fromhost PT_GUARDED_BY(&htif_lock) = வெற்று;
நிலையான uint64_t *htif_tohost PT_GUARDED_BY(&htif_lock)	 = வெற்று;
நிலையான bool htif_custom = false;

நிலையான முழு htif_console_buf GUARDED_BY(&htif_lock);

நிலையான உள்வரி uint64_t __read_tohost(வெறுமை) MUST_HOLD(&htif_lock)
{
	திரும்பு (htif_custom) ? *htif_tohost : tohost;
}

நிலையான உள்வரி வெறுமை __write_tohost(uint64_t val) MUST_HOLD(&htif_lock)
{
	எனில் (htif_custom)
		*htif_tohost = val;
	இல்லையெனில்
		tohost = val;
}

நிலையான உள்வரி uint64_t __read_fromhost(வெறுமை) MUST_HOLD(&htif_lock)
{
	திரும்பு (htif_custom) ? *htif_fromhost : fromhost;
}

நிலையான உள்வரி வெறுமை __write_fromhost(uint64_t val) MUST_HOLD(&htif_lock)
{
	எனில் (htif_custom)
		*htif_fromhost = val;
	இல்லையெனில்
		fromhost = val;
}

நிலையான வெறுமை __check_fromhost() MUST_HOLD(&htif_lock)
{
	uint64_t fh = __read_fromhost();
	எனில் (!fh)
		திரும்பு;
	__write_fromhost(0);

	/* this should be from the console */
	எனில் (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	தேர்வு (FROMHOST_CMD(fh)) {
		நிலை HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_DATA(fh);
			நிறுத்து;
		நிலை HTIF_CONSOLE_CMD_PUTC:
			நிறுத்து;
		இயல்பு:
			__builtin_trap();
	}
}

நிலையான வெறுமை __set_tohost(uint64_t dev, uint64_t cmd, uint64_t data)
	MUST_HOLD(&htif_lock)
{
	போது (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, data));
}

நிலையான முழு set_custom_addr(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	எனில் (custom_addr) {
		எனில் (htif_custom &&
		    ((custom_fromhost_addr != (unsigned long)htif_fromhost) ||
		     (custom_tohost_addr != (unsigned long)htif_tohost)))
			திரும்பு SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	திரும்பு 0;
}

#எனில் __riscv_xlen == 32
நிலையான வெறுமை do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t data)
{
	spin_lock(&htif_lock);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, data);

	போது (1) {
		uint64_t fh = __read_fromhost();
		எனில் (fh) {
			எனில் (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				நிறுத்து;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_lock);
}

நிலையான வெறுமை htif_putc(எழுத்து ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	நிலையற்ற uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#இல்லையெனில்
நிலையான வெறுமை htif_putc(எழுத்து ch)
{
	spin_lock(&htif_lock);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_lock);
}
#endif

நிலையான முழு htif_getc(வெறுமை)
{
	முழு ch;

#எனில் __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	திரும்பு -1;
#endif

	spin_lock(&htif_lock);

	__check_fromhost();
	ch = htif_console_buf;
	எனில் (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_lock);

	திரும்பு ch - 1;
}

நிலையான கட்டமைப்பு sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

முழு htif_serial_init(bool custom_addr,
		     unsigned long custom_fromhost_addr,
		     unsigned long custom_tohost_addr)
{
	முழு rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	எனில் (rc)
		திரும்பு rc;

	sbi_console_set_device(&htif_console);
	திரும்பு 0;
}

நிலையான முழு htif_system_reset_check(u32 type, u32 reason)
{
	திரும்பு 1;
}

நிலையான வெறுமை htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	போது (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

நிலையான கட்டமைப்பு sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

முழு htif_system_reset_init(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	முழு rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	எனில் (rc)
		திரும்பு rc;

	sbi_system_reset_add_device(&htif_reset);
	திரும்பு 0;
}
