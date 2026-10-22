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

#परिभाषय HTIF_दत्त_द्वयङ्काः		48
#परिभाषय HTIF_दत्त_आवरणम्		((1ULL << HTIF_दत्त_द्वयङ्काः) - 1)
#परिभाषय HTIF_दत्त_सरणम्		0
#परिभाषय HTIF_CMD_द्वयङ्काः		8
#परिभाषय HTIF_CMD_आवरणम्		((1ULL << HTIF_CMD_द्वयङ्काः) - 1)
#परिभाषय HTIF_CMD_सरणम्		48
#परिभाषय HTIF_DEV_द्वयङ्काः		8
#परिभाषय HTIF_DEV_आवरणम्		((1ULL << HTIF_DEV_द्वयङ्काः) - 1)
#परिभाषय HTIF_DEV_सरणम्		56

#परिभाषय HTIF_DEV_SYSTEM		0
#परिभाषय HTIF_DEV_CONSOLE	1

#परिभाषय HTIF_CONSOLE_CMD_GETC	0
#परिभाषय HTIF_CONSOLE_CMD_PUTC	1

#यदि __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_सरणम्) | \
	 ((uint64_t)(cmd) << HTIF_CMD_सरणम्) | \
	 (uint64_t)(payload))
#अन्यथा
# define TOHOST_CMD(dev, cmd, payload) ({ \
  यदि ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#यदि_अन्तः
#परिभाषय FROMHOST_DEV(fromhost_मानम्) \
	((uint64_t)((fromhost_मानम्) >> HTIF_DEV_सरणम्) & HTIF_DEV_आवरणम्)
#परिभाषय FROMHOST_CMD(fromhost_मानम्) \
	((uint64_t)((fromhost_मानम्) >> HTIF_CMD_सरणम्) & HTIF_CMD_आवरणम्)
#परिभाषय FROMHOST_दत्त(fromhost_मानम्) \
	((uint64_t)((fromhost_मानम्) >> HTIF_दत्त_सरणम्) & HTIF_दत्त_आवरणम्)

#परिभाषय PK_SYS_write 64

स्थिर spinlock_t htif_तालकम् = SPIN_तालकम्_INITIALIZER;

चञ्चल uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_तालकम्);
चञ्चल uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_तालकम्);

स्थिर uint64_t *htif_fromhost PT_GUARDED_BY(&htif_तालकम्) = रिक्त;
स्थिर uint64_t *htif_tohost PT_GUARDED_BY(&htif_तालकम्)	 = रिक्त;
स्थिर bool htif_custom = false;

स्थिर पूर्णाङ्क htif_console_buf GUARDED_BY(&htif_तालकम्);

स्थिर अन्तःस्थ uint64_t __read_tohost(शून्य) MUST_HOLD(&htif_तालकम्)
{
	प्रत्यागम (htif_custom) ? *htif_tohost : tohost;
}

स्थिर अन्तःस्थ शून्य __write_tohost(uint64_t val) MUST_HOLD(&htif_तालकम्)
{
	यदि (htif_custom)
		*htif_tohost = val;
	अन्यथा
		tohost = val;
}

स्थिर अन्तःस्थ uint64_t __read_fromhost(शून्य) MUST_HOLD(&htif_तालकम्)
{
	प्रत्यागम (htif_custom) ? *htif_fromhost : fromhost;
}

स्थिर अन्तःस्थ शून्य __write_fromhost(uint64_t val) MUST_HOLD(&htif_तालकम्)
{
	यदि (htif_custom)
		*htif_fromhost = val;
	अन्यथा
		fromhost = val;
}

स्थिर शून्य __check_fromhost() MUST_HOLD(&htif_तालकम्)
{
	uint64_t fh = __read_fromhost();
	यदि (!fh)
		प्रत्यागम;
	__write_fromhost(0);

	/* this should be from the console */
	यदि (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	विकल्प (FROMHOST_CMD(fh)) {
		पक्ष HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_दत्त(fh);
			भङ्ग;
		पक्ष HTIF_CONSOLE_CMD_PUTC:
			भङ्ग;
		स्वभाव:
			__builtin_trap();
	}
}

स्थिर शून्य __set_tohost(uint64_t dev, uint64_t cmd, uint64_t दत्त)
	MUST_HOLD(&htif_तालकम्)
{
	यावत् (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, दत्त));
}

स्थिर पूर्णाङ्क समुच्चयः_custom_addr(bool custom_addr,
			   अचिह्नित दीर्घ custom_fromhost_addr,
			   अचिह्नित दीर्घ custom_tohost_addr)
{
	यदि (custom_addr) {
		यदि (htif_custom &&
		    ((custom_fromhost_addr != (अचिह्नित दीर्घ)htif_fromhost) ||
		     (custom_tohost_addr != (अचिह्नित दीर्घ)htif_tohost)))
			प्रत्यागम SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	प्रत्यागम 0;
}

#यदि __riscv_xlen == 32
स्थिर शून्य do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t दत्त)
{
	spin_तालकम्(&htif_तालकम्);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, दत्त);

	यावत् (1) {
		uint64_t fh = __read_fromhost();
		यदि (fh) {
			यदि (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				भङ्ग;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_तालकम्);
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
#अन्यथा
स्थिर शून्य htif_putc(वर्ण ch)
{
	spin_तालकम्(&htif_तालकम्);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_तालकम्);
}
#यदि_अन्तः

स्थिर पूर्णाङ्क htif_getc(शून्य)
{
	पूर्णाङ्क ch;

#यदि __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	प्रत्यागम -1;
#यदि_अन्तः

	spin_तालकम्(&htif_तालकम्);

	__check_fromhost();
	ch = htif_console_buf;
	यदि (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_तालकम्);

	प्रत्यागम ch - 1;
}

स्थिर संरचना sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

पूर्णाङ्क htif_serial_init(bool custom_addr,
		     अचिह्नित दीर्घ custom_fromhost_addr,
		     अचिह्नित दीर्घ custom_tohost_addr)
{
	पूर्णाङ्क rc;

	rc = समुच्चयः_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	यदि (rc)
		प्रत्यागम rc;

	sbi_console_समुच्चयः_device(&htif_console);
	प्रत्यागम 0;
}

स्थिर पूर्णाङ्क htif_system_reset_check(u32 type, u32 reason)
{
	प्रत्यागम 1;
}

स्थिर शून्य htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	यावत् (1) {
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
			   अचिह्नित दीर्घ custom_fromhost_addr,
			   अचिह्नित दीर्घ custom_tohost_addr)
{
	पूर्णाङ्क rc;

	rc = समुच्चयः_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	यदि (rc)
		प्रत्यागम rc;

	sbi_system_reset_सङ्कलय_device(&htif_reset);
	प्रत्यागम 0;
}
