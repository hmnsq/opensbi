/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2010-2020, The Regents of the University of California
 * (Regents).  All Rights Reserved.
 */

#ಅನ್ತರ್ಭಾವ <sbi/riscv_locks.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_system.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/sys/htif.h>

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

#ಯದಿ __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_SHIFT) | \
	 ((uint64_t)(cmd) << HTIF_CMD_SHIFT) | \
	 (uint64_t)(payload))
#ಅನ್ಯಥಾ
# define TOHOST_CMD(dev, cmd, payload) ({ \
  ಯದಿ ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#endif
#define FROMHOST_DEV(fromhost_ಮೌಲ್ಯ) \
	((uint64_t)((fromhost_ಮೌಲ್ಯ) >> HTIF_DEV_SHIFT) & HTIF_DEV_MASK)
#define FROMHOST_CMD(fromhost_ಮೌಲ್ಯ) \
	((uint64_t)((fromhost_ಮೌಲ್ಯ) >> HTIF_CMD_SHIFT) & HTIF_CMD_MASK)
#define FROMHOST_DATA(fromhost_ಮೌಲ್ಯ) \
	((uint64_t)((fromhost_ಮೌಲ್ಯ) >> HTIF_DATA_SHIFT) & HTIF_DATA_MASK)

#define PK_SYS_write 64

ಸ್ಥಿರ spinlock_t htif_lock = SPIN_LOCK_INITIALIZER;

ಚಞ್ಚಲ uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);
ಚಞ್ಚಲ uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);

ಸ್ಥಿರ uint64_t *htif_fromhost PT_GUARDED_BY(&htif_lock) = ರಿಕ್ತ;
ಸ್ಥಿರ uint64_t *htif_tohost PT_GUARDED_BY(&htif_lock)	 = ರಿಕ್ತ;
ಸ್ಥಿರ bool htif_custom = false;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ htif_console_buf GUARDED_BY(&htif_lock);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ uint64_t __read_tohost(ಶೂನ್ಯ) MUST_HOLD(&htif_lock)
{
	ಪ್ರತ್ಯಾಗಮ (htif_custom) ? *htif_tohost : tohost;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ __write_tohost(uint64_t val) MUST_HOLD(&htif_lock)
{
	ಯದಿ (htif_custom)
		*htif_tohost = val;
	ಅನ್ಯಥಾ
		tohost = val;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ uint64_t __read_fromhost(ಶೂನ್ಯ) MUST_HOLD(&htif_lock)
{
	ಪ್ರತ್ಯಾಗಮ (htif_custom) ? *htif_fromhost : fromhost;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ __write_fromhost(uint64_t val) MUST_HOLD(&htif_lock)
{
	ಯದಿ (htif_custom)
		*htif_fromhost = val;
	ಅನ್ಯಥಾ
		fromhost = val;
}

ಸ್ಥಿರ ಶೂನ್ಯ __check_fromhost() MUST_HOLD(&htif_lock)
{
	uint64_t fh = __read_fromhost();
	ಯದಿ (!fh)
		ಪ್ರತ್ಯಾಗಮ;
	__write_fromhost(0);

	/* this should be from the console */
	ಯದಿ (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	ವಿಕಲ್ಪ (FROMHOST_CMD(fh)) {
		ಪಕ್ಷ HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_DATA(fh);
			ಭಙ್ಗ;
		ಪಕ್ಷ HTIF_CONSOLE_CMD_PUTC:
			ಭಙ್ಗ;
		ಸ್ವಭಾವ:
			__builtin_trap();
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ __set_tohost(uint64_t dev, uint64_t cmd, uint64_t data)
	MUST_HOLD(&htif_lock)
{
	ಯಾವತ್ (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, data));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ set_custom_addr(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	ಯದಿ (custom_addr) {
		ಯದಿ (htif_custom &&
		    ((custom_fromhost_addr != (unsigned long)htif_fromhost) ||
		     (custom_tohost_addr != (unsigned long)htif_tohost)))
			ಪ್ರತ್ಯಾಗಮ SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

#ಯದಿ __riscv_xlen == 32
ಸ್ಥಿರ ಶೂನ್ಯ do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t data)
{
	spin_lock(&htif_lock);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, data);

	ಯಾವತ್ (1) {
		uint64_t fh = __read_fromhost();
		ಯದಿ (fh) {
			ಯದಿ (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				ಭಙ್ಗ;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_lock);
}

ಸ್ಥಿರ ಶೂನ್ಯ htif_putc(ವರ್ಣ ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	ಚಞ್ಚಲ uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#ಅನ್ಯಥಾ
ಸ್ಥಿರ ಶೂನ್ಯ htif_putc(ವರ್ಣ ch)
{
	spin_lock(&htif_lock);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_lock);
}
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ htif_getc(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಙ್ಕ ch;

#ಯದಿ __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	ಪ್ರತ್ಯಾಗಮ -1;
#endif

	spin_lock(&htif_lock);

	__check_fromhost();
	ch = htif_console_buf;
	ಯದಿ (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_lock);

	ಪ್ರತ್ಯಾಗಮ ch - 1;
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

ಪೂರ್ಣಾಙ್ಕ htif_serial_init(bool custom_addr,
		     unsigned long custom_fromhost_addr,
		     unsigned long custom_tohost_addr)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	sbi_console_set_device(&htif_console);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ htif_system_reset_check(u32 type, u32 reason)
{
	ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	ಯಾವತ್ (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

ಪೂರ್ಣಾಙ್ಕ htif_system_reset_init(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	ಪೂರ್ಣಾಙ್ಕ rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	ಯದಿ (rc)
		ಪ್ರತ್ಯಾಗಮ rc;

	sbi_system_reset_add_device(&htif_reset);
	ಪ್ರತ್ಯಾಗಮ 0;
}
