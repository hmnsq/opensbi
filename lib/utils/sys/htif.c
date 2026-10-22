/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2010-2020, The Regents of the University of California
 * (Regents).  All Rights Reserved.
 */

#অন্তর্ভাব <sbi/riscv_locks.h>
#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi/sbi_system.h>
#অন্তর্ভাব <sbi_utils/sys/htif.h>

#define HTIF_তথ্য_BITS		48
#define HTIF_তথ্য_MASK		((1ULL << HTIF_তথ্য_BITS) - 1)
#define HTIF_তথ্য_SHIFT		0
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

#যদি __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_SHIFT) | \
	 ((uint64_t)(cmd) << HTIF_CMD_SHIFT) | \
	 (uint64_t)(payload))
#অন্যথা
# define TOHOST_CMD(dev, cmd, payload) ({ \
  যদি ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#endif
#define FROMHOST_DEV(fromhost_মান) \
	((uint64_t)((fromhost_মান) >> HTIF_DEV_SHIFT) & HTIF_DEV_MASK)
#define FROMHOST_CMD(fromhost_মান) \
	((uint64_t)((fromhost_মান) >> HTIF_CMD_SHIFT) & HTIF_CMD_MASK)
#define FROMHOST_তথ্য(fromhost_মান) \
	((uint64_t)((fromhost_মান) >> HTIF_তথ্য_SHIFT) & HTIF_তথ্য_MASK)

#define PK_SYS_write 64

স্থির spinlock_t htif_lock = SPIN_LOCK_INITIALIZER;

অস্থির uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);
অস্থির uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);

স্থির uint64_t *htif_fromhost PT_GUARDED_BY(&htif_lock) = ফাঁকা;
স্থির uint64_t *htif_tohost PT_GUARDED_BY(&htif_lock)	 = ফাঁকা;
স্থির bool htif_custom = false;

স্থির পূর্ণসংখ্যা htif_console_buf GUARDED_BY(&htif_lock);

স্থির অন্তঃরেখ uint64_t __read_tohost(শূন্য) MUST_HOLD(&htif_lock)
{
	ফেরত (htif_custom) ? *htif_tohost : tohost;
}

স্থির অন্তঃরেখ শূন্য __write_tohost(uint64_t val) MUST_HOLD(&htif_lock)
{
	যদি (htif_custom)
		*htif_tohost = val;
	অন্যথা
		tohost = val;
}

স্থির অন্তঃরেখ uint64_t __read_fromhost(শূন্য) MUST_HOLD(&htif_lock)
{
	ফেরত (htif_custom) ? *htif_fromhost : fromhost;
}

স্থির অন্তঃরেখ শূন্য __write_fromhost(uint64_t val) MUST_HOLD(&htif_lock)
{
	যদি (htif_custom)
		*htif_fromhost = val;
	অন্যথা
		fromhost = val;
}

স্থির শূন্য __check_fromhost() MUST_HOLD(&htif_lock)
{
	uint64_t fh = __read_fromhost();
	যদি (!fh)
		ফেরত;
	__write_fromhost(0);

	/* this should be from the console */
	যদি (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	নির্বাচন (FROMHOST_CMD(fh)) {
		ক্ষেত্র HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_তথ্য(fh);
			বিরতি;
		ক্ষেত্র HTIF_CONSOLE_CMD_PUTC:
			বিরতি;
		পূর্বনির্ধারিত:
			__builtin_trap();
	}
}

স্থির শূন্য __set_tohost(uint64_t dev, uint64_t cmd, uint64_t তথ্য)
	MUST_HOLD(&htif_lock)
{
	যখন (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, তথ্য));
}

স্থির পূর্ণসংখ্যা set_custom_addr(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	যদি (custom_addr) {
		যদি (htif_custom &&
		    ((custom_fromhost_addr != (unsigned long)htif_fromhost) ||
		     (custom_tohost_addr != (unsigned long)htif_tohost)))
			ফেরত SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	ফেরত 0;
}

#যদি __riscv_xlen == 32
স্থির শূন্য do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t তথ্য)
{
	spin_lock(&htif_lock);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, তথ্য);

	যখন (1) {
		uint64_t fh = __read_fromhost();
		যদি (fh) {
			যদি (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				বিরতি;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_lock);
}

স্থির শূন্য htif_putc(অক্ষর ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	অস্থির uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#অন্যথা
স্থির শূন্য htif_putc(অক্ষর ch)
{
	spin_lock(&htif_lock);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_lock);
}
#endif

স্থির পূর্ণসংখ্যা htif_getc(শূন্য)
{
	পূর্ণসংখ্যা ch;

#যদি __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	ফেরত -1;
#endif

	spin_lock(&htif_lock);

	__check_fromhost();
	ch = htif_console_buf;
	যদি (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_lock);

	ফেরত ch - 1;
}

স্থির কাঠামো sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

পূর্ণসংখ্যা htif_serial_init(bool custom_addr,
		     unsigned long custom_fromhost_addr,
		     unsigned long custom_tohost_addr)
{
	পূর্ণসংখ্যা rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	যদি (rc)
		ফেরত rc;

	sbi_console_set_device(&htif_console);
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা htif_system_reset_check(u32 type, u32 reason)
{
	ফেরত 1;
}

স্থির শূন্য htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	যখন (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

স্থির কাঠামো sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

পূর্ণসংখ্যা htif_system_reset_init(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	পূর্ণসংখ্যা rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	যদি (rc)
		ফেরত rc;

	sbi_system_reset_add_device(&htif_reset);
	ফেরত 0;
}
