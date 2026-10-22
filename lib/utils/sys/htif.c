/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2010-2020, The Regents of the University of California
 * (Regents).  All Rights Reserved.
 */

#അന്തര്ഭാവ <sbi/riscv_locks.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi/sbi_system.h>
#അന്തര്ഭാവ <sbi_utils/sys/htif.h>

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

#എങ്കിൽ __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_SHIFT) | \
	 ((uint64_t)(cmd) << HTIF_CMD_SHIFT) | \
	 (uint64_t)(payload))
#അല്ലെങ്കിൽ
# define TOHOST_CMD(dev, cmd, payload) ({ \
  എങ്കിൽ ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#endif
#define FROMHOST_DEV(fromhost_മൂല്യം) \
	((uint64_t)((fromhost_മൂല്യം) >> HTIF_DEV_SHIFT) & HTIF_DEV_MASK)
#define FROMHOST_CMD(fromhost_മൂല്യം) \
	((uint64_t)((fromhost_മൂല്യം) >> HTIF_CMD_SHIFT) & HTIF_CMD_MASK)
#define FROMHOST_DATA(fromhost_മൂല്യം) \
	((uint64_t)((fromhost_മൂല്യം) >> HTIF_DATA_SHIFT) & HTIF_DATA_MASK)

#define PK_SYS_write 64

സ്ഥിതം spinlock_t htif_lock = SPIN_LOCK_INITIALIZER;

അസ്ഥിര uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);
അസ്ഥിര uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);

സ്ഥിതം uint64_t *htif_fromhost PT_GUARDED_BY(&htif_lock) = ശൂന്യ;
സ്ഥിതം uint64_t *htif_tohost PT_GUARDED_BY(&htif_lock)	 = ശൂന്യ;
സ്ഥിതം bool htif_custom = false;

സ്ഥിതം പൂർണ്ണസംഖ്യ htif_console_buf GUARDED_BY(&htif_lock);

സ്ഥിതം വരിയിൽ uint64_t __read_tohost(ശൂന്യം) MUST_HOLD(&htif_lock)
{
	തിരികെ (htif_custom) ? *htif_tohost : tohost;
}

സ്ഥിതം വരിയിൽ ശൂന്യം __write_tohost(uint64_t val) MUST_HOLD(&htif_lock)
{
	എങ്കിൽ (htif_custom)
		*htif_tohost = val;
	അല്ലെങ്കിൽ
		tohost = val;
}

സ്ഥിതം വരിയിൽ uint64_t __read_fromhost(ശൂന്യം) MUST_HOLD(&htif_lock)
{
	തിരികെ (htif_custom) ? *htif_fromhost : fromhost;
}

സ്ഥിതം വരിയിൽ ശൂന്യം __write_fromhost(uint64_t val) MUST_HOLD(&htif_lock)
{
	എങ്കിൽ (htif_custom)
		*htif_fromhost = val;
	അല്ലെങ്കിൽ
		fromhost = val;
}

സ്ഥിതം ശൂന്യം __check_fromhost() MUST_HOLD(&htif_lock)
{
	uint64_t fh = __read_fromhost();
	എങ്കിൽ (!fh)
		തിരികെ;
	__write_fromhost(0);

	/* this should be from the console */
	എങ്കിൽ (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	തിരഞ്ഞെടുപ്പ് (FROMHOST_CMD(fh)) {
		അവസ്ഥ HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_DATA(fh);
			നിർത്തുക;
		അവസ്ഥ HTIF_CONSOLE_CMD_PUTC:
			നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
			__builtin_trap();
	}
}

സ്ഥിതം ശൂന്യം __set_tohost(uint64_t dev, uint64_t cmd, uint64_t data)
	MUST_HOLD(&htif_lock)
{
	ആകുമ്പോൾ (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, data));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ set_custom_addr(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	എങ്കിൽ (custom_addr) {
		എങ്കിൽ (htif_custom &&
		    ((custom_fromhost_addr != (unsigned long)htif_fromhost) ||
		     (custom_tohost_addr != (unsigned long)htif_tohost)))
			തിരികെ SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	തിരികെ 0;
}

#എങ്കിൽ __riscv_xlen == 32
സ്ഥിതം ശൂന്യം do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t data)
{
	spin_lock(&htif_lock);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, data);

	ആകുമ്പോൾ (1) {
		uint64_t fh = __read_fromhost();
		എങ്കിൽ (fh) {
			എങ്കിൽ (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				നിർത്തുക;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_lock);
}

സ്ഥിതം ശൂന്യം htif_putc(അക്ഷരം ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	അസ്ഥിര uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#അല്ലെങ്കിൽ
സ്ഥിതം ശൂന്യം htif_putc(അക്ഷരം ch)
{
	spin_lock(&htif_lock);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_lock);
}
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ htif_getc(ശൂന്യം)
{
	പൂർണ്ണസംഖ്യ ch;

#എങ്കിൽ __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	തിരികെ -1;
#endif

	spin_lock(&htif_lock);

	__check_fromhost();
	ch = htif_console_buf;
	എങ്കിൽ (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_lock);

	തിരികെ ch - 1;
}

സ്ഥിതം ഘടന sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

പൂർണ്ണസംഖ്യ htif_serial_init(bool custom_addr,
		     unsigned long custom_fromhost_addr,
		     unsigned long custom_tohost_addr)
{
	പൂർണ്ണസംഖ്യ rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	എങ്കിൽ (rc)
		തിരികെ rc;

	sbi_console_set_device(&htif_console);
	തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ htif_system_reset_check(u32 type, u32 reason)
{
	തിരികെ 1;
}

സ്ഥിതം ശൂന്യം htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	ആകുമ്പോൾ (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

സ്ഥിതം ഘടന sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

പൂർണ്ണസംഖ്യ htif_system_reset_init(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	പൂർണ്ണസംഖ്യ rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	എങ്കിൽ (rc)
		തിരികെ rc;

	sbi_system_reset_add_device(&htif_reset);
	തിരികെ 0;
}
