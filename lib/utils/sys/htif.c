/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2010-2020, The Regents of the University of California
 * (Regents).  All Rights Reserved.
 */

#અન્તર્ભાવ <sbi/riscv_locks.h>
#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_error.h>
#અન્તર્ભાવ <sbi/sbi_system.h>
#અન્તર્ભાવ <sbi_utils/sys/htif.h>

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

#જો __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_SHIFT) | \
	 ((uint64_t)(cmd) << HTIF_CMD_SHIFT) | \
	 (uint64_t)(payload))
#નહીંતર
# define TOHOST_CMD(dev, cmd, payload) ({ \
  જો ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#endif
#define FROMHOST_DEV(fromhost_કિંમત) \
	((uint64_t)((fromhost_કિંમત) >> HTIF_DEV_SHIFT) & HTIF_DEV_MASK)
#define FROMHOST_CMD(fromhost_કિંમત) \
	((uint64_t)((fromhost_કિંમત) >> HTIF_CMD_SHIFT) & HTIF_CMD_MASK)
#define FROMHOST_DATA(fromhost_કિંમત) \
	((uint64_t)((fromhost_કિંમત) >> HTIF_DATA_SHIFT) & HTIF_DATA_MASK)

#define PK_SYS_write 64

સ્થિર spinlock_t htif_lock = SPIN_LOCK_INITIALIZER;

અસ્થિર uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);
અસ્થિર uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);

સ્થિર uint64_t *htif_fromhost PT_GUARDED_BY(&htif_lock) = ખાલી;
સ્થિર uint64_t *htif_tohost PT_GUARDED_BY(&htif_lock)	 = ખાલી;
સ્થિર bool htif_custom = false;

સ્થિર પૂર્ણાંક htif_console_buf GUARDED_BY(&htif_lock);

સ્થિર અંતઃરેખ uint64_t __read_tohost(શૂન્ય) MUST_HOLD(&htif_lock)
{
	પાછા (htif_custom) ? *htif_tohost : tohost;
}

સ્થિર અંતઃરેખ શૂન્ય __write_tohost(uint64_t val) MUST_HOLD(&htif_lock)
{
	જો (htif_custom)
		*htif_tohost = val;
	નહીંતર
		tohost = val;
}

સ્થિર અંતઃરેખ uint64_t __read_fromhost(શૂન્ય) MUST_HOLD(&htif_lock)
{
	પાછા (htif_custom) ? *htif_fromhost : fromhost;
}

સ્થિર અંતઃરેખ શૂન્ય __write_fromhost(uint64_t val) MUST_HOLD(&htif_lock)
{
	જો (htif_custom)
		*htif_fromhost = val;
	નહીંતર
		fromhost = val;
}

સ્થિર શૂન્ય __check_fromhost() MUST_HOLD(&htif_lock)
{
	uint64_t fh = __read_fromhost();
	જો (!fh)
		પાછા;
	__write_fromhost(0);

	/* this should be from the console */
	જો (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	પસંદગી (FROMHOST_CMD(fh)) {
		સ્થિતિ HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_DATA(fh);
			તોડો;
		સ્થિતિ HTIF_CONSOLE_CMD_PUTC:
			તોડો;
		મૂળભૂત:
			__builtin_trap();
	}
}

સ્થિર શૂન્ય __set_tohost(uint64_t dev, uint64_t cmd, uint64_t data)
	MUST_HOLD(&htif_lock)
{
	જ્યારે (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, data));
}

સ્થિર પૂર્ણાંક set_custom_addr(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	જો (custom_addr) {
		જો (htif_custom &&
		    ((custom_fromhost_addr != (unsigned long)htif_fromhost) ||
		     (custom_tohost_addr != (unsigned long)htif_tohost)))
			પાછા SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	પાછા 0;
}

#જો __riscv_xlen == 32
સ્થિર શૂન્ય do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t data)
{
	spin_lock(&htif_lock);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, data);

	જ્યારે (1) {
		uint64_t fh = __read_fromhost();
		જો (fh) {
			જો (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				તોડો;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_lock);
}

સ્થિર શૂન્ય htif_putc(અક્ષર ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	અસ્થિર uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#નહીંતર
સ્થિર શૂન્ય htif_putc(અક્ષર ch)
{
	spin_lock(&htif_lock);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_lock);
}
#endif

સ્થિર પૂર્ણાંક htif_getc(શૂન્ય)
{
	પૂર્ણાંક ch;

#જો __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	પાછા -1;
#endif

	spin_lock(&htif_lock);

	__check_fromhost();
	ch = htif_console_buf;
	જો (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_lock);

	પાછા ch - 1;
}

સ્થિર સંરચના sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

પૂર્ણાંક htif_serial_init(bool custom_addr,
		     unsigned long custom_fromhost_addr,
		     unsigned long custom_tohost_addr)
{
	પૂર્ણાંક rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	જો (rc)
		પાછા rc;

	sbi_console_set_device(&htif_console);
	પાછા 0;
}

સ્થિર પૂર્ણાંક htif_system_reset_check(u32 type, u32 reason)
{
	પાછા 1;
}

સ્થિર શૂન્ય htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	જ્યારે (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

સ્થિર સંરચના sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

પૂર્ણાંક htif_system_reset_init(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	પૂર્ણાંક rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	જો (rc)
		પાછા rc;

	sbi_system_reset_add_device(&htif_reset);
	પાછા 0;
}
