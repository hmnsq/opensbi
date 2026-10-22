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

#تعریف_کرو HTIF_ڈیٹا_بٹ		48
#تعریف_کرو HTIF_ڈیٹا_نقاب		((1ULL << HTIF_ڈیٹا_بٹ) - 1)
#تعریف_کرو HTIF_ڈیٹا_سرکاؤ		0
#تعریف_کرو HTIF_CMD_بٹ		8
#تعریف_کرو HTIF_CMD_نقاب		((1ULL << HTIF_CMD_بٹ) - 1)
#تعریف_کرو HTIF_CMD_سرکاؤ		48
#تعریف_کرو HTIF_DEV_بٹ		8
#تعریف_کرو HTIF_DEV_نقاب		((1ULL << HTIF_DEV_بٹ) - 1)
#تعریف_کرو HTIF_DEV_سرکاؤ		56

#تعریف_کرو HTIF_DEV_SYSTEM		0
#تعریف_کرو HTIF_DEV_CONSOLE	1

#تعریف_کرو HTIF_CONSOLE_CMD_GETC	0
#تعریف_کرو HTIF_CONSOLE_CMD_PUTC	1

#اگر __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_سرکاؤ) | \
	 ((uint64_t)(cmd) << HTIF_CMD_سرکاؤ) | \
	 (uint64_t)(payload))
#ورنہ
# define TOHOST_CMD(dev, cmd, payload) ({ \
  اگر ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#اگر_ختم
#تعریف_کرو FROMHOST_DEV(fromhost_قدر) \
	((uint64_t)((fromhost_قدر) >> HTIF_DEV_سرکاؤ) & HTIF_DEV_نقاب)
#تعریف_کرو FROMHOST_CMD(fromhost_قدر) \
	((uint64_t)((fromhost_قدر) >> HTIF_CMD_سرکاؤ) & HTIF_CMD_نقاب)
#تعریف_کرو FROMHOST_ڈیٹا(fromhost_قدر) \
	((uint64_t)((fromhost_قدر) >> HTIF_ڈیٹا_سرکاؤ) & HTIF_ڈیٹا_نقاب)

#تعریف_کرو PK_SYS_write 64

ساکن spinlock_t htif_تالا = SPIN_تالا_INITIALIZER;

متلون uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_تالا);
متلون uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_تالا);

ساکن uint64_t *htif_fromhost PT_GUARDED_BY(&htif_تالا) = تہی;
ساکن uint64_t *htif_tohost PT_GUARDED_BY(&htif_تالا)	 = تہی;
ساکن bool htif_custom = false;

ساکن صحیح htif_console_buf GUARDED_BY(&htif_تالا);

ساکن اندرونی uint64_t __read_tohost(خالی) MUST_HOLD(&htif_تالا)
{
	واپس (htif_custom) ? *htif_tohost : tohost;
}

ساکن اندرونی خالی __write_tohost(uint64_t val) MUST_HOLD(&htif_تالا)
{
	اگر (htif_custom)
		*htif_tohost = val;
	ورنہ
		tohost = val;
}

ساکن اندرونی uint64_t __read_fromhost(خالی) MUST_HOLD(&htif_تالا)
{
	واپس (htif_custom) ? *htif_fromhost : fromhost;
}

ساکن اندرونی خالی __write_fromhost(uint64_t val) MUST_HOLD(&htif_تالا)
{
	اگر (htif_custom)
		*htif_fromhost = val;
	ورنہ
		fromhost = val;
}

ساکن خالی __check_fromhost() MUST_HOLD(&htif_تالا)
{
	uint64_t fh = __read_fromhost();
	اگر (!fh)
		واپس;
	__write_fromhost(0);

	/* this should be from the console */
	اگر (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	انتخاب (FROMHOST_CMD(fh)) {
		حالت HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_ڈیٹا(fh);
			توڑو;
		حالت HTIF_CONSOLE_CMD_PUTC:
			توڑو;
		طےشدہ:
			__builtin_trap();
	}
}

ساکن خالی __set_tohost(uint64_t dev, uint64_t cmd, uint64_t ڈیٹا)
	MUST_HOLD(&htif_تالا)
{
	جب (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, ڈیٹا));
}

ساکن صحیح سیٹ_custom_addr(bool custom_addr,
			   غیرعلامتی طویل custom_fromhost_addr,
			   غیرعلامتی طویل custom_tohost_addr)
{
	اگر (custom_addr) {
		اگر (htif_custom &&
		    ((custom_fromhost_addr != (غیرعلامتی طویل)htif_fromhost) ||
		     (custom_tohost_addr != (غیرعلامتی طویل)htif_tohost)))
			واپس SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	واپس 0;
}

#اگر __riscv_xlen == 32
ساکن خالی do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t ڈیٹا)
{
	spin_تالا(&htif_تالا);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, ڈیٹا);

	جب (1) {
		uint64_t fh = __read_fromhost();
		اگر (fh) {
			اگر (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				توڑو;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_تالا);
}

ساکن خالی htif_putc(حرف ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	متلون uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#ورنہ
ساکن خالی htif_putc(حرف ch)
{
	spin_تالا(&htif_تالا);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_تالا);
}
#اگر_ختم

ساکن صحیح htif_getc(خالی)
{
	صحیح ch;

#اگر __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	واپس -1;
#اگر_ختم

	spin_تالا(&htif_تالا);

	__check_fromhost();
	ch = htif_console_buf;
	اگر (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_تالا);

	واپس ch - 1;
}

ساکن ساخت sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

صحیح htif_serial_init(bool custom_addr,
		     غیرعلامتی طویل custom_fromhost_addr,
		     غیرعلامتی طویل custom_tohost_addr)
{
	صحیح rc;

	rc = سیٹ_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	اگر (rc)
		واپس rc;

	sbi_console_سیٹ_device(&htif_console);
	واپس 0;
}

ساکن صحیح htif_system_reset_check(u32 type, u32 reason)
{
	واپس 1;
}

ساکن خالی htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	جب (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

ساکن ساخت sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

صحیح htif_system_reset_init(bool custom_addr,
			   غیرعلامتی طویل custom_fromhost_addr,
			   غیرعلامتی طویل custom_tohost_addr)
{
	صحیح rc;

	rc = سیٹ_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	اگر (rc)
		واپس rc;

	sbi_system_reset_جوڑو_device(&htif_reset);
	واپس 0;
}
