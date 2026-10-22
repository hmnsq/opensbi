/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2010-2020, The Regents of the University of California
 * (Regents).  All Rights Reserved.
 */

#ଅନ୍ତର୍ଭାଵ <sbi/riscv_locks.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_system.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/sys/htif.h>

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

#ଯଦି __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_SHIFT) | \
	 ((uint64_t)(cmd) << HTIF_CMD_SHIFT) | \
	 (uint64_t)(payload))
#ଅନ୍ୟଥା
# define TOHOST_CMD(dev, cmd, payload) ({ \
  ଯଦି ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#endif
#define FROMHOST_DEV(fromhost_ମୂଲ୍ୟ) \
	((uint64_t)((fromhost_ମୂଲ୍ୟ) >> HTIF_DEV_SHIFT) & HTIF_DEV_MASK)
#define FROMHOST_CMD(fromhost_ମୂଲ୍ୟ) \
	((uint64_t)((fromhost_ମୂଲ୍ୟ) >> HTIF_CMD_SHIFT) & HTIF_CMD_MASK)
#define FROMHOST_DATA(fromhost_ମୂଲ୍ୟ) \
	((uint64_t)((fromhost_ମୂଲ୍ୟ) >> HTIF_DATA_SHIFT) & HTIF_DATA_MASK)

#define PK_SYS_write 64

ସ୍ଥିର spinlock_t htif_lock = SPIN_LOCK_INITIALIZER;

ଅସ୍ଥିର uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);
ଅସ୍ଥିର uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);

ସ୍ଥିର uint64_t *htif_fromhost PT_GUARDED_BY(&htif_lock) = ଖାଲି;
ସ୍ଥିର uint64_t *htif_tohost PT_GUARDED_BY(&htif_lock)	 = ଖାଲି;
ସ୍ଥିର bool htif_custom = false;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା htif_console_buf GUARDED_BY(&htif_lock);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint64_t __read_tohost(ଶୂନ୍ୟ) MUST_HOLD(&htif_lock)
{
	ଫେରନ୍ତୁ (htif_custom) ? *htif_tohost : tohost;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ __write_tohost(uint64_t val) MUST_HOLD(&htif_lock)
{
	ଯଦି (htif_custom)
		*htif_tohost = val;
	ଅନ୍ୟଥା
		tohost = val;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint64_t __read_fromhost(ଶୂନ୍ୟ) MUST_HOLD(&htif_lock)
{
	ଫେରନ୍ତୁ (htif_custom) ? *htif_fromhost : fromhost;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ __write_fromhost(uint64_t val) MUST_HOLD(&htif_lock)
{
	ଯଦି (htif_custom)
		*htif_fromhost = val;
	ଅନ୍ୟଥା
		fromhost = val;
}

ସ୍ଥିର ଶୂନ୍ୟ __check_fromhost() MUST_HOLD(&htif_lock)
{
	uint64_t fh = __read_fromhost();
	ଯଦି (!fh)
		ଫେରନ୍ତୁ;
	__write_fromhost(0);

	/* this should be from the console */
	ଯଦି (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	ବାଛ (FROMHOST_CMD(fh)) {
		ସ୍ଥିତି HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_DATA(fh);
			ଭାଙ୍ଗ;
		ସ୍ଥିତି HTIF_CONSOLE_CMD_PUTC:
			ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			__builtin_trap();
	}
}

ସ୍ଥିର ଶୂନ୍ୟ __set_tohost(uint64_t dev, uint64_t cmd, uint64_t data)
	MUST_HOLD(&htif_lock)
{
	ଯେତେବେଳେ (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, data));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_custom_addr(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	ଯଦି (custom_addr) {
		ଯଦି (htif_custom &&
		    ((custom_fromhost_addr != (unsigned long)htif_fromhost) ||
		     (custom_tohost_addr != (unsigned long)htif_tohost)))
			ଫେରନ୍ତୁ SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	ଫେରନ୍ତୁ 0;
}

#ଯଦି __riscv_xlen == 32
ସ୍ଥିର ଶୂନ୍ୟ do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t data)
{
	spin_lock(&htif_lock);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, data);

	ଯେତେବେଳେ (1) {
		uint64_t fh = __read_fromhost();
		ଯଦି (fh) {
			ଯଦି (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				ଭାଙ୍ଗ;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_lock);
}

ସ୍ଥିର ଶୂନ୍ୟ htif_putc(ଅକ୍ଷର ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	ଅସ୍ଥିର uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#ଅନ୍ୟଥା
ସ୍ଥିର ଶୂନ୍ୟ htif_putc(ଅକ୍ଷର ch)
{
	spin_lock(&htif_lock);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_lock);
}
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା htif_getc(ଶୂନ୍ୟ)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ch;

#ଯଦି __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	ଫେରନ୍ତୁ -1;
#endif

	spin_lock(&htif_lock);

	__check_fromhost();
	ch = htif_console_buf;
	ଯଦି (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_lock);

	ଫେରନ୍ତୁ ch - 1;
}

ସ୍ଥିର ସଂରଚନା sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା htif_serial_init(bool custom_addr,
		     unsigned long custom_fromhost_addr,
		     unsigned long custom_tohost_addr)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	sbi_console_set_device(&htif_console);
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା htif_system_reset_check(u32 type, u32 reason)
{
	ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଶୂନ୍ୟ htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	ଯେତେବେଳେ (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

ସ୍ଥିର ସଂରଚନା sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା htif_system_reset_init(bool custom_addr,
			   unsigned long custom_fromhost_addr,
			   unsigned long custom_tohost_addr)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	ଯଦି (rc)
		ଫେରନ୍ତୁ rc;

	sbi_system_reset_add_device(&htif_reset);
	ଫେରନ୍ତୁ 0;
}
