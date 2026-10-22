/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2010-2020, The Regents of the University of California
 * (Regents).  All Rights Reserved.
 */

#अन्तर्भाव <sbi/riscv_locks.h>
#अन्तर्भाव <sbi/sbi_console.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_system.h>
#अन्तर्भाव <sbi_utils/sys/htif.h>

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

#सचे __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_SHIFT) | \
	 ((uint64_t)(cmd) << HTIF_CMD_SHIFT) | \
	 (uint64_t)(payload))
#अञ्ञथा
# define TOHOST_CMD(dev, cmd, payload) ({ \
  सचे ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#endif
#define FROMHOST_DEV(fromhost_मानम्) \
	((uint64_t)((fromhost_मानम्) >> HTIF_DEV_SHIFT) & HTIF_DEV_MASK)
#define FROMHOST_CMD(fromhost_मानम्) \
	((uint64_t)((fromhost_मानम्) >> HTIF_CMD_SHIFT) & HTIF_CMD_MASK)
#define FROMHOST_DATA(fromhost_मानम्) \
	((uint64_t)((fromhost_मानम्) >> HTIF_DATA_SHIFT) & HTIF_DATA_MASK)

#define PK_SYS_write 64

स्थिर spinlock_t htif_lock = SPIN_LOCK_INITIALIZER;

चञ्चल uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);
चञ्चल uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);

स्थिर uint64_t *htif_fromhost PT_GUARDED_BY(&htif_lock) = रिक्त;
स्थिर uint64_t *htif_tohost PT_GUARDED_BY(&htif_lock)	 = रिक्त;
स्थिर bool htif_custom = false;

स्थिर पूर्णाङ्क htif_console_buf GUARDED_BY(&htif_lock);

स्थिर अन्तःस्थ uint64_t __read_tohost(शून्य) MUST_HOLD(&htif_lock)
{
	पटिनिवत्त (htif_custom) ? *htif_tohost : tohost;
}

स्थिर अन्तःस्थ शून्य __write_tohost(uint64_t val) MUST_HOLD(&htif_lock)
{
	सचे (htif_custom)
		*htif_tohost = val;
	अञ्ञथा
		tohost = val;
}

स्थिर अन्तःस्थ uint64_t __read_fromhost(शून्य) MUST_HOLD(&htif_lock)
{
	पटिनिवत्त (htif_custom) ? *htif_fromhost : fromhost;
}

स्थिर अन्तःस्थ शून्य __write_fromhost(uint64_t val) MUST_HOLD(&htif_lock)
{
	सचे (htif_custom)
		*htif_fromhost = val;
	अञ्ञथा
		fromhost = val;
}

स्थिर शून्य __check_fromhost() MUST_HOLD(&htif_lock)
{
	uint64_t fh = __read_fromhost();
	सचे (!fh)
		पटिनिवत्त;
	__write_fromhost(0);

	/* this should be from the console */
	सचे (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	विकल्प (FROMHOST_CMD(fh)) {
		पक्ष HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_DATA(fh);
			भङ्ग;
		पक्ष HTIF_CONSOLE_CMD_PUTC:
			भङ्ग;
		स्वभाव:
			__builtin_trap();
	}
}

स्थिर शून्य __set_tohost(uint64_t dev, uint64_t cmd, uint64_t data)
	MUST_HOLD(&htif_lock)
{
	याव (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, data));
}

स्थिर पूर्णाङ्क set_custom_addr(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	सचे (custom_addr) {
		सचे (htif_custom &&
		    ((custom_fromhost_addr != (unsigned long)htif_fromhost) ||
		     (custom_tohost_addr != (unsigned long)htif_tohost)))
			पटिनिवत्त SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	पटिनिवत्त 0;
}

#सचे __riscv_xlen == 32
स्थिर शून्य do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t data)
{
	spin_lock(&htif_lock);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, data);

	याव (1) {
		uint64_t fh = __read_fromhost();
		सचे (fh) {
			सचे (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				भङ्ग;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_lock);
}

स्थिर शून्य htif_putc(वर्ण ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	चञ्चल uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#अञ्ञथा
स्थिर शून्य htif_putc(वर्ण ch)
{
	spin_lock(&htif_lock);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_lock);
}
#endif

स्थिर पूर्णाङ्क htif_getc(शून्य)
{
	पूर्णाङ्क ch;

#सचे __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	पटिनिवत्त -1;
#endif

	spin_lock(&htif_lock);

	__check_fromhost();
	ch = htif_console_buf;
	सचे (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_lock);

	पटिनिवत्त ch - 1;
}

स्थिर संरचना sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

पूर्णाङ्क htif_serial_init(bool custom_addr,
		     unsigned long custom_fromhost_addr,
		     unsigned long custom_tohost_addr)
{
	पूर्णाङ्क rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	सचे (rc)
		पटिनिवत्त rc;

	sbi_console_set_device(&htif_console);
	पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क htif_system_reset_check(u32 type, u32 reason)
{
	पटिनिवत्त 1;
}

स्थिर शून्य htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	याव (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

स्थिर संरचना sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

पूर्णाङ्क htif_system_reset_init(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	पूर्णाङ्क rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	सचे (rc)
		पटिनिवत्त rc;

	sbi_system_reset_add_device(&htif_reset);
	पटिनिवत्त 0;
}
