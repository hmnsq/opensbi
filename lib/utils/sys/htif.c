/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2010-2020, The Regents of the University of California
 * (Regents).  All Rights Reserved.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/riscv_locks.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_system.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/sys/htif.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTIF_ਅੰਕੜੇ_ਬਿੱਟ		48
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTIF_ਅੰਕੜੇ_ਨਕਾਬ		((1ULL << HTIF_ਅੰਕੜੇ_ਬਿੱਟ) - 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTIF_ਅੰਕੜੇ_ਖਿਸਕਾਓ		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTIF_CMD_ਬਿੱਟ		8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTIF_CMD_ਨਕਾਬ		((1ULL << HTIF_CMD_ਬਿੱਟ) - 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTIF_CMD_ਖਿਸਕਾਓ		48
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTIF_DEV_ਬਿੱਟ		8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTIF_DEV_ਨਕਾਬ		((1ULL << HTIF_DEV_ਬਿੱਟ) - 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTIF_DEV_ਖਿਸਕਾਓ		56

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTIF_DEV_SYSTEM		0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTIF_DEV_CONSOLE	1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTIF_CONSOLE_CMD_GETC	0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HTIF_CONSOLE_CMD_PUTC	1

#ਜੇ __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_ਖਿਸਕਾਓ) | \
	 ((uint64_t)(cmd) << HTIF_CMD_ਖਿਸਕਾਓ) | \
	 (uint64_t)(payload))
#ਨਹੀਂਤਾਂ
# define TOHOST_CMD(dev, cmd, payload) ({ \
  ਜੇ ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FROMHOST_DEV(fromhost_ਮੁੱਲ) \
	((uint64_t)((fromhost_ਮੁੱਲ) >> HTIF_DEV_ਖਿਸਕਾਓ) & HTIF_DEV_ਨਕਾਬ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FROMHOST_CMD(fromhost_ਮੁੱਲ) \
	((uint64_t)((fromhost_ਮੁੱਲ) >> HTIF_CMD_ਖਿਸਕਾਓ) & HTIF_CMD_ਨਕਾਬ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FROMHOST_ਅੰਕੜੇ(fromhost_ਮੁੱਲ) \
	((uint64_t)((fromhost_ਮੁੱਲ) >> HTIF_ਅੰਕੜੇ_ਖਿਸਕਾਓ) & HTIF_ਅੰਕੜੇ_ਨਕਾਬ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PK_SYS_write 64

ਸਥਿਰ spinlock_t htif_ਤਾਲਾ = SPIN_ਤਾਲਾ_INITIALIZER;

ਅਸਥਿਰ uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_ਤਾਲਾ);
ਅਸਥਿਰ uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_ਤਾਲਾ);

ਸਥਿਰ uint64_t *htif_fromhost PT_GUARDED_BY(&htif_ਤਾਲਾ) = ਨਿਰਾ;
ਸਥਿਰ uint64_t *htif_tohost PT_GUARDED_BY(&htif_ਤਾਲਾ)	 = ਨਿਰਾ;
ਸਥਿਰ bool htif_custom = false;

ਸਥਿਰ ਪੂਰਨ htif_console_buf GUARDED_BY(&htif_ਤਾਲਾ);

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t __read_tohost(ਖਾਲੀ) MUST_HOLD(&htif_ਤਾਲਾ)
{
	ਵਾਪਸ (htif_custom) ? *htif_tohost : tohost;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ __write_tohost(uint64_t val) MUST_HOLD(&htif_ਤਾਲਾ)
{
	ਜੇ (htif_custom)
		*htif_tohost = val;
	ਨਹੀਂਤਾਂ
		tohost = val;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t __read_fromhost(ਖਾਲੀ) MUST_HOLD(&htif_ਤਾਲਾ)
{
	ਵਾਪਸ (htif_custom) ? *htif_fromhost : fromhost;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ __write_fromhost(uint64_t val) MUST_HOLD(&htif_ਤਾਲਾ)
{
	ਜੇ (htif_custom)
		*htif_fromhost = val;
	ਨਹੀਂਤਾਂ
		fromhost = val;
}

ਸਥਿਰ ਖਾਲੀ __check_fromhost() MUST_HOLD(&htif_ਤਾਲਾ)
{
	uint64_t fh = __read_fromhost();
	ਜੇ (!fh)
		ਵਾਪਸ;
	__write_fromhost(0);

	/* this should be from the console */
	ਜੇ (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	ਚੋਣ (FROMHOST_CMD(fh)) {
		ਹਾਲਤ HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_ਅੰਕੜੇ(fh);
			ਰੋਕੋ;
		ਹਾਲਤ HTIF_CONSOLE_CMD_PUTC:
			ਰੋਕੋ;
		ਮੂਲ:
			__builtin_trap();
	}
}

ਸਥਿਰ ਖਾਲੀ __set_tohost(uint64_t dev, uint64_t cmd, uint64_t ਅੰਕੜੇ)
	MUST_HOLD(&htif_ਤਾਲਾ)
{
	ਜਦੋਂ (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, ਅੰਕੜੇ));
}

ਸਥਿਰ ਪੂਰਨ ਸੈੱਟ_custom_addr(bool custom_addr,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ custom_fromhost_addr,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ custom_tohost_addr)
{
	ਜੇ (custom_addr) {
		ਜੇ (htif_custom &&
		    ((custom_fromhost_addr != (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)htif_fromhost) ||
		     (custom_tohost_addr != (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)htif_tohost)))
			ਵਾਪਸ SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	ਵਾਪਸ 0;
}

#ਜੇ __riscv_xlen == 32
ਸਥਿਰ ਖਾਲੀ do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t ਅੰਕੜੇ)
{
	spin_ਤਾਲਾ(&htif_ਤਾਲਾ);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, ਅੰਕੜੇ);

	ਜਦੋਂ (1) {
		uint64_t fh = __read_fromhost();
		ਜੇ (fh) {
			ਜੇ (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				ਰੋਕੋ;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_ਤਾਲਾ);
}

ਸਥਿਰ ਖਾਲੀ htif_putc(ਅੱਖਰ ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	ਅਸਥਿਰ uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#ਨਹੀਂਤਾਂ
ਸਥਿਰ ਖਾਲੀ htif_putc(ਅੱਖਰ ch)
{
	spin_ਤਾਲਾ(&htif_ਤਾਲਾ);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_ਤਾਲਾ);
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ htif_getc(ਖਾਲੀ)
{
	ਪੂਰਨ ch;

#ਜੇ __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	ਵਾਪਸ -1;
#ਜੇ_ਅੰਤ

	spin_ਤਾਲਾ(&htif_ਤਾਲਾ);

	__check_fromhost();
	ch = htif_console_buf;
	ਜੇ (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_ਤਾਲਾ);

	ਵਾਪਸ ch - 1;
}

ਸਥਿਰ ਬਣਤਰ sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

ਪੂਰਨ htif_serial_init(bool custom_addr,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ custom_fromhost_addr,
		     ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ custom_tohost_addr)
{
	ਪੂਰਨ rc;

	rc = ਸੈੱਟ_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	sbi_console_ਸੈੱਟ_device(&htif_console);
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ htif_system_reset_check(u32 type, u32 reason)
{
	ਵਾਪਸ 1;
}

ਸਥਿਰ ਖਾਲੀ htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	ਜਦੋਂ (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

ਸਥਿਰ ਬਣਤਰ sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

ਪੂਰਨ htif_system_reset_init(bool custom_addr,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ custom_fromhost_addr,
			   ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ custom_tohost_addr)
{
	ਪੂਰਨ rc;

	rc = ਸੈੱਟ_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	ਜੇ (rc)
		ਵਾਪਸ rc;

	sbi_system_reset_ਜੋੜੋ_device(&htif_reset);
	ਵਾਪਸ 0;
}
