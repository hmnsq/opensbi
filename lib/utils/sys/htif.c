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

#تعریف_کن HTIF_داده_بیتها		48
#تعریف_کن HTIF_داده_ماسک		((1ULL << HTIF_داده_بیتها) - 1)
#تعریف_کن HTIF_داده_لغزش		0
#تعریف_کن HTIF_CMD_بیتها		8
#تعریف_کن HTIF_CMD_ماسک		((1ULL << HTIF_CMD_بیتها) - 1)
#تعریف_کن HTIF_CMD_لغزش		48
#تعریف_کن HTIF_DEV_بیتها		8
#تعریف_کن HTIF_DEV_ماسک		((1ULL << HTIF_DEV_بیتها) - 1)
#تعریف_کن HTIF_DEV_لغزش		56

#تعریف_کن HTIF_DEV_SYSTEM		0
#تعریف_کن HTIF_DEV_CONSOLE	1

#تعریف_کن HTIF_CONSOLE_CMD_GETC	0
#تعریف_کن HTIF_CONSOLE_CMD_PUTC	1

#إذا __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_لغزش) | \
	 ((uint64_t)(cmd) << HTIF_CMD_لغزش) | \
	 (uint64_t)(payload))
#وإلا
# define TOHOST_CMD(dev, cmd, payload) ({ \
  إذا ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#پایان_اگر
#تعریف_کن FROMHOST_DEV(fromhost_مقدار) \
	((uint64_t)((fromhost_مقدار) >> HTIF_DEV_لغزش) & HTIF_DEV_ماسک)
#تعریف_کن FROMHOST_CMD(fromhost_مقدار) \
	((uint64_t)((fromhost_مقدار) >> HTIF_CMD_لغزش) & HTIF_CMD_ماسک)
#تعریف_کن FROMHOST_داده(fromhost_مقدار) \
	((uint64_t)((fromhost_مقدار) >> HTIF_داده_لغزش) & HTIF_داده_ماسک)

#تعریف_کن PK_SYS_write 64

ساكن spinlock_t htif_قفل = SPIN_قفل_INITIALIZER;

متطاير uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_قفل);
متطاير uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_قفل);

ساكن uint64_t *htif_fromhost PT_GUARDED_BY(&htif_قفل) = عدم;
ساكن uint64_t *htif_tohost PT_GUARDED_BY(&htif_قفل)	 = عدم;
ساكن bool htif_custom = false;

ساكن صحيح htif_console_buf GUARDED_BY(&htif_قفل);

ساكن مضمن uint64_t __read_tohost(فراغ) MUST_HOLD(&htif_قفل)
{
	أرجع (htif_custom) ? *htif_tohost : tohost;
}

ساكن مضمن فراغ __write_tohost(uint64_t val) MUST_HOLD(&htif_قفل)
{
	إذا (htif_custom)
		*htif_tohost = val;
	وإلا
		tohost = val;
}

ساكن مضمن uint64_t __read_fromhost(فراغ) MUST_HOLD(&htif_قفل)
{
	أرجع (htif_custom) ? *htif_fromhost : fromhost;
}

ساكن مضمن فراغ __write_fromhost(uint64_t val) MUST_HOLD(&htif_قفل)
{
	إذا (htif_custom)
		*htif_fromhost = val;
	وإلا
		fromhost = val;
}

ساكن فراغ __check_fromhost() MUST_HOLD(&htif_قفل)
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
			htif_console_buf = 1 + (uint8_t)FROMHOST_داده(fh);
			اكسر;
		حالة HTIF_CONSOLE_CMD_PUTC:
			اكسر;
		افتراضي:
			__builtin_trap();
	}
}

ساكن فراغ __set_tohost(uint64_t dev, uint64_t cmd, uint64_t داده)
	MUST_HOLD(&htif_قفل)
{
	طالما (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, داده));
}

ساكن صحيح مجموعه_custom_addr(bool custom_addr,
			   بیعلامت دراز custom_fromhost_addr,
			   بیعلامت دراز custom_tohost_addr)
{
	إذا (custom_addr) {
		إذا (htif_custom &&
		    ((custom_fromhost_addr != (بیعلامت دراز)htif_fromhost) ||
		     (custom_tohost_addr != (بیعلامت دراز)htif_tohost)))
			أرجع SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	أرجع 0;
}

#إذا __riscv_xlen == 32
ساكن فراغ do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t داده)
{
	spin_قفل(&htif_قفل);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, داده);

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

	spin_unlock(&htif_قفل);
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
	spin_قفل(&htif_قفل);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_قفل);
}
#پایان_اگر

ساكن صحيح htif_getc(فراغ)
{
	صحيح ch;

#إذا __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	أرجع -1;
#پایان_اگر

	spin_قفل(&htif_قفل);

	__check_fromhost();
	ch = htif_console_buf;
	إذا (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_قفل);

	أرجع ch - 1;
}

ساكن بنية sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

صحيح htif_serial_init(bool custom_addr,
		     بیعلامت دراز custom_fromhost_addr,
		     بیعلامت دراز custom_tohost_addr)
{
	صحيح rc;

	rc = مجموعه_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	إذا (rc)
		أرجع rc;

	sbi_console_مجموعه_device(&htif_console);
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
			   بیعلامت دراز custom_fromhost_addr,
			   بیعلامت دراز custom_tohost_addr)
{
	صحيح rc;

	rc = مجموعه_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	إذا (rc)
		أرجع rc;

	sbi_system_reset_جمع_کن_device(&htif_reset);
	أرجع 0;
}
