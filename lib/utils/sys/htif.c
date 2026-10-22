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

#وصف_ڪريو HTIF_معلومات_بٽ		48
#وصف_ڪريو HTIF_معلومات_نقاب		((1ULL << HTIF_معلومات_بٽ) - 1)
#وصف_ڪريو HTIF_معلومات_سرڪايو		0
#وصف_ڪريو HTIF_CMD_بٽ		8
#وصف_ڪريو HTIF_CMD_نقاب		((1ULL << HTIF_CMD_بٽ) - 1)
#وصف_ڪريو HTIF_CMD_سرڪايو		48
#وصف_ڪريو HTIF_DEV_بٽ		8
#وصف_ڪريو HTIF_DEV_نقاب		((1ULL << HTIF_DEV_بٽ) - 1)
#وصف_ڪريو HTIF_DEV_سرڪايو		56

#وصف_ڪريو HTIF_DEV_SYSTEM		0
#وصف_ڪريو HTIF_DEV_CONSOLE	1

#وصف_ڪريو HTIF_CONSOLE_CMD_GETC	0
#وصف_ڪريو HTIF_CONSOLE_CMD_PUTC	1

#إذا __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_سرڪايو) | \
	 ((uint64_t)(cmd) << HTIF_CMD_سرڪايو) | \
	 (uint64_t)(payload))
#وإلا
# define TOHOST_CMD(dev, cmd, payload) ({ \
  إذا ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#جيڪڏهن_پورو
#وصف_ڪريو FROMHOST_DEV(fromhost_قدر) \
	((uint64_t)((fromhost_قدر) >> HTIF_DEV_سرڪايو) & HTIF_DEV_نقاب)
#وصف_ڪريو FROMHOST_CMD(fromhost_قدر) \
	((uint64_t)((fromhost_قدر) >> HTIF_CMD_سرڪايو) & HTIF_CMD_نقاب)
#وصف_ڪريو FROMHOST_معلومات(fromhost_قدر) \
	((uint64_t)((fromhost_قدر) >> HTIF_معلومات_سرڪايو) & HTIF_معلومات_نقاب)

#وصف_ڪريو PK_SYS_write 64

ساكن spinlock_t htif_تالو = SPIN_تالو_INITIALIZER;

متطاير uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_تالو);
متطاير uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_تالو);

ساكن uint64_t *htif_fromhost PT_GUARDED_BY(&htif_تالو) = عدم;
ساكن uint64_t *htif_tohost PT_GUARDED_BY(&htif_تالو)	 = عدم;
ساكن bool htif_custom = false;

ساكن صحيح htif_console_buf GUARDED_BY(&htif_تالو);

ساكن مضمن uint64_t __read_tohost(فراغ) MUST_HOLD(&htif_تالو)
{
	أرجع (htif_custom) ? *htif_tohost : tohost;
}

ساكن مضمن فراغ __write_tohost(uint64_t val) MUST_HOLD(&htif_تالو)
{
	إذا (htif_custom)
		*htif_tohost = val;
	وإلا
		tohost = val;
}

ساكن مضمن uint64_t __read_fromhost(فراغ) MUST_HOLD(&htif_تالو)
{
	أرجع (htif_custom) ? *htif_fromhost : fromhost;
}

ساكن مضمن فراغ __write_fromhost(uint64_t val) MUST_HOLD(&htif_تالو)
{
	إذا (htif_custom)
		*htif_fromhost = val;
	وإلا
		fromhost = val;
}

ساكن فراغ __check_fromhost() MUST_HOLD(&htif_تالو)
{
	uint64_t fh = __read_fromhost();
	إذا (!fh)
		أرجع;
	__write_fromhost(0);

	/* this should be from the console */
	إذا (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	بدل (FROMHOST_CMD(fh)) {
		حالة HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_معلومات(fh);
			اكسر;
		حالة HTIF_CONSOLE_CMD_PUTC:
			اكسر;
		افتراضي:
			__builtin_trap();
	}
}

ساكن فراغ __set_tohost(uint64_t dev, uint64_t cmd, uint64_t معلومات)
	MUST_HOLD(&htif_تالو)
{
	طالما (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, معلومات));
}

ساكن صحيح سيٽ_custom_addr(bool custom_addr,
			   بنانشان ڊگهو custom_fromhost_addr,
			   بنانشان ڊگهو custom_tohost_addr)
{
	إذا (custom_addr) {
		إذا (htif_custom &&
		    ((custom_fromhost_addr != (بنانشان ڊگهو)htif_fromhost) ||
		     (custom_tohost_addr != (بنانشان ڊگهو)htif_tohost)))
			أرجع SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	أرجع 0;
}

#إذا __riscv_xlen == 32
ساكن فراغ do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t معلومات)
{
	spin_تالو(&htif_تالو);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, معلومات);

	طالما (1) {
		uint64_t fh = __read_fromhost();
		إذا (fh) {
			إذا (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				اكسر;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_تالو);
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
#وإلا
ساكن فراغ htif_putc(حرف ch)
{
	spin_تالو(&htif_تالو);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_تالو);
}
#جيڪڏهن_پورو

ساكن صحيح htif_getc(فراغ)
{
	صحيح ch;

#إذا __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	أرجع -1;
#جيڪڏهن_پورو

	spin_تالو(&htif_تالو);

	__check_fromhost();
	ch = htif_console_buf;
	إذا (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_تالو);

	أرجع ch - 1;
}

ساكن بنية sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

صحيح htif_serial_init(bool custom_addr,
		     بنانشان ڊگهو custom_fromhost_addr,
		     بنانشان ڊگهو custom_tohost_addr)
{
	صحيح rc;

	rc = سيٽ_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	إذا (rc)
		أرجع rc;

	sbi_console_سيٽ_device(&htif_console);
	أرجع 0;
}

ساكن صحيح htif_system_reset_check(u32 type, u32 reason)
{
	أرجع 1;
}

ساكن فراغ htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	طالما (1) {
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
			   بنانشان ڊگهو custom_fromhost_addr,
			   بنانشان ڊگهو custom_tohost_addr)
{
	صحيح rc;

	rc = سيٽ_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	إذا (rc)
		أرجع rc;

	sbi_system_reset_جوڙيو_device(&htif_reset);
	أرجع 0;
}
