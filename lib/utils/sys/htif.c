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

#ವ್ಯಾಖ್ಯಾನಿಸು HTIF_ದತ್ತಾಂಶ_ಬಿಟ್ಟುಗಳು		48
#ವ್ಯಾಖ್ಯಾನಿಸು HTIF_ದತ್ತಾಂಶ_ಮುಖವಾಡ		((1ULL << HTIF_ದತ್ತಾಂಶ_ಬಿಟ್ಟುಗಳು) - 1)
#ವ್ಯಾಖ್ಯಾನಿಸು HTIF_ದತ್ತಾಂಶ_ಸರಿಸು		0
#ವ್ಯಾಖ್ಯಾನಿಸು HTIF_CMD_ಬಿಟ್ಟುಗಳು		8
#ವ್ಯಾಖ್ಯಾನಿಸು HTIF_CMD_ಮುಖವಾಡ		((1ULL << HTIF_CMD_ಬಿಟ್ಟುಗಳು) - 1)
#ವ್ಯಾಖ್ಯಾನಿಸು HTIF_CMD_ಸರಿಸು		48
#ವ್ಯಾಖ್ಯಾನಿಸು HTIF_DEV_ಬಿಟ್ಟುಗಳು		8
#ವ್ಯಾಖ್ಯಾನಿಸು HTIF_DEV_ಮುಖವಾಡ		((1ULL << HTIF_DEV_ಬಿಟ್ಟುಗಳು) - 1)
#ವ್ಯಾಖ್ಯಾನಿಸು HTIF_DEV_ಸರಿಸು		56

#ವ್ಯಾಖ್ಯಾನಿಸು HTIF_DEV_SYSTEM		0
#ವ್ಯಾಖ್ಯಾನಿಸು HTIF_DEV_CONSOLE	1

#ವ್ಯಾಖ್ಯಾನಿಸು HTIF_CONSOLE_CMD_GETC	0
#ವ್ಯಾಖ್ಯಾನಿಸು HTIF_CONSOLE_CMD_PUTC	1

#ಆದರೆ __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_ಸರಿಸು) | \
	 ((uint64_t)(cmd) << HTIF_CMD_ಸರಿಸು) | \
	 (uint64_t)(payload))
#ಇಲ್ಲದಿದ್ದರೆ
# define TOHOST_CMD(dev, cmd, payload) ({ \
  ಆದರೆ ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು FROMHOST_DEV(fromhost_ಮೌಲ್ಯ) \
	((uint64_t)((fromhost_ಮೌಲ್ಯ) >> HTIF_DEV_ಸರಿಸು) & HTIF_DEV_ಮುಖವಾಡ)
#ವ್ಯಾಖ್ಯಾನಿಸು FROMHOST_CMD(fromhost_ಮೌಲ್ಯ) \
	((uint64_t)((fromhost_ಮೌಲ್ಯ) >> HTIF_CMD_ಸರಿಸು) & HTIF_CMD_ಮುಖವಾಡ)
#ವ್ಯಾಖ್ಯಾನಿಸು FROMHOST_ದತ್ತಾಂಶ(fromhost_ಮೌಲ್ಯ) \
	((uint64_t)((fromhost_ಮೌಲ್ಯ) >> HTIF_ದತ್ತಾಂಶ_ಸರಿಸು) & HTIF_ದತ್ತಾಂಶ_ಮುಖವಾಡ)

#ವ್ಯಾಖ್ಯಾನಿಸು PK_SYS_write 64

ಸ್ಥಿರ spinlock_t htif_ಬೀಗ = SPIN_ಬೀಗ_INITIALIZER;

ಅಸ್ಥಿರ uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_ಬೀಗ);
ಅಸ್ಥಿರ uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_ಬೀಗ);

ಸ್ಥಿರ uint64_t *htif_fromhost PT_GUARDED_BY(&htif_ಬೀಗ) = ಖಾಲಿ;
ಸ್ಥಿರ uint64_t *htif_tohost PT_GUARDED_BY(&htif_ಬೀಗ)	 = ಖಾಲಿ;
ಸ್ಥಿರ bool htif_custom = false;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ htif_console_buf GUARDED_BY(&htif_ಬೀಗ);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t __read_tohost(ಶೂನ್ಯ) MUST_HOLD(&htif_ಬೀಗ)
{
	ಹಿಂತಿರುಗಿ (htif_custom) ? *htif_tohost : tohost;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ __write_tohost(uint64_t val) MUST_HOLD(&htif_ಬೀಗ)
{
	ಆದರೆ (htif_custom)
		*htif_tohost = val;
	ಇಲ್ಲದಿದ್ದರೆ
		tohost = val;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t __read_fromhost(ಶೂನ್ಯ) MUST_HOLD(&htif_ಬೀಗ)
{
	ಹಿಂತಿರುಗಿ (htif_custom) ? *htif_fromhost : fromhost;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ __write_fromhost(uint64_t val) MUST_HOLD(&htif_ಬೀಗ)
{
	ಆದರೆ (htif_custom)
		*htif_fromhost = val;
	ಇಲ್ಲದಿದ್ದರೆ
		fromhost = val;
}

ಸ್ಥಿರ ಶೂನ್ಯ __check_fromhost() MUST_HOLD(&htif_ಬೀಗ)
{
	uint64_t fh = __read_fromhost();
	ಆದರೆ (!fh)
		ಹಿಂತಿರುಗಿ;
	__write_fromhost(0);

	/* this should be from the console */
	ಆದರೆ (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	ಆಯ್ಕೆ (FROMHOST_CMD(fh)) {
		ಸ್ಥಿತಿ HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_ದತ್ತಾಂಶ(fh);
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ HTIF_CONSOLE_CMD_PUTC:
			ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
			__builtin_trap();
	}
}

ಸ್ಥಿರ ಶೂನ್ಯ __set_tohost(uint64_t dev, uint64_t cmd, uint64_t ದತ್ತಾಂಶ)
	MUST_HOLD(&htif_ಬೀಗ)
{
	ಆಗ (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, ದತ್ತಾಂಶ));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಗಣ_custom_addr(bool custom_addr,
			   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ custom_fromhost_addr,
			   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ custom_tohost_addr)
{
	ಆದರೆ (custom_addr) {
		ಆದರೆ (htif_custom &&
		    ((custom_fromhost_addr != (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)htif_fromhost) ||
		     (custom_tohost_addr != (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)htif_tohost)))
			ಹಿಂತಿರುಗಿ SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	ಹಿಂತಿರುಗಿ 0;
}

#ಆದರೆ __riscv_xlen == 32
ಸ್ಥಿರ ಶೂನ್ಯ do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t ದತ್ತಾಂಶ)
{
	spin_ಬೀಗ(&htif_ಬೀಗ);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, ದತ್ತಾಂಶ);

	ಆಗ (1) {
		uint64_t fh = __read_fromhost();
		ಆದರೆ (fh) {
			ಆದರೆ (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				ನಿಲ್ಲಿಸು;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_ಬೀಗ);
}

ಸ್ಥಿರ ಶೂನ್ಯ htif_putc(ಅಕ್ಷರ ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	ಅಸ್ಥಿರ uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಶೂನ್ಯ htif_putc(ಅಕ್ಷರ ch)
{
	spin_ಬೀಗ(&htif_ಬೀಗ);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_ಬೀಗ);
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ htif_getc(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ ch;

#ಆದರೆ __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	ಹಿಂತಿರುಗಿ -1;
#ಆದರೆ_ಅಂತ್ಯ

	spin_ಬೀಗ(&htif_ಬೀಗ);

	__check_fromhost();
	ch = htif_console_buf;
	ಆದರೆ (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_ಬೀಗ);

	ಹಿಂತಿರುಗಿ ch - 1;
}

ಸ್ಥಿರ ರಚನೆ sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

ಪೂರ್ಣಾಂಕ htif_serial_init(bool custom_addr,
		     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ custom_fromhost_addr,
		     ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ custom_tohost_addr)
{
	ಪೂರ್ಣಾಂಕ rc;

	rc = ಗಣ_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	sbi_console_ಗಣ_device(&htif_console);
	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ htif_system_reset_check(u32 type, u32 reason)
{
	ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	ಆಗ (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

ಸ್ಥಿರ ರಚನೆ sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

ಪೂರ್ಣಾಂಕ htif_system_reset_init(bool custom_addr,
			   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ custom_fromhost_addr,
			   ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ custom_tohost_addr)
{
	ಪೂರ್ಣಾಂಕ rc;

	rc = ಗಣ_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	ಆದರೆ (rc)
		ಹಿಂತಿರುಗಿ rc;

	sbi_system_reset_ಕೂಡಿಸು_device(&htif_reset);
	ಹಿಂತಿರುಗಿ 0;
}
