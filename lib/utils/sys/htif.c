/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2010-2020, The Regents of the University of California
 * (Regents).  All Rights Reserved.
 */

#అన్తర్భావ <sbi/riscv_locks.h>
#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi/sbi_system.h>
#అన్తర్భావ <sbi_utils/sys/htif.h>

#నిర్వచించు HTIF_దత్తాంశం_బిట్లు		48
#నిర్వచించు HTIF_దత్తాంశం_ముసుగు		((1ULL << HTIF_దత్తాంశం_బిట్లు) - 1)
#నిర్వచించు HTIF_దత్తాంశం_జరుపు		0
#నిర్వచించు HTIF_CMD_బిట్లు		8
#నిర్వచించు HTIF_CMD_ముసుగు		((1ULL << HTIF_CMD_బిట్లు) - 1)
#నిర్వచించు HTIF_CMD_జరుపు		48
#నిర్వచించు HTIF_DEV_బిట్లు		8
#నిర్వచించు HTIF_DEV_ముసుగు		((1ULL << HTIF_DEV_బిట్లు) - 1)
#నిర్వచించు HTIF_DEV_జరుపు		56

#నిర్వచించు HTIF_DEV_SYSTEM		0
#నిర్వచించు HTIF_DEV_CONSOLE	1

#నిర్వచించు HTIF_CONSOLE_CMD_GETC	0
#నిర్వచించు HTIF_CONSOLE_CMD_PUTC	1

#ఒకవేళ __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_జరుపు) | \
	 ((uint64_t)(cmd) << HTIF_CMD_జరుపు) | \
	 (uint64_t)(payload))
#లేకపోతే
# define TOHOST_CMD(dev, cmd, payload) ({ \
  ఒకవేళ ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#అయితే_ముగింపు
#నిర్వచించు FROMHOST_DEV(fromhost_విలువ) \
	((uint64_t)((fromhost_విలువ) >> HTIF_DEV_జరుపు) & HTIF_DEV_ముసుగు)
#నిర్వచించు FROMHOST_CMD(fromhost_విలువ) \
	((uint64_t)((fromhost_విలువ) >> HTIF_CMD_జరుపు) & HTIF_CMD_ముసుగు)
#నిర్వచించు FROMHOST_దత్తాంశం(fromhost_విలువ) \
	((uint64_t)((fromhost_విలువ) >> HTIF_దత్తాంశం_జరుపు) & HTIF_దత్తాంశం_ముసుగు)

#నిర్వచించు PK_SYS_write 64

స్థిర spinlock_t htif_తాళం = SPIN_తాళం_INITIALIZER;

అస్థిర uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_తాళం);
అస్థిర uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_తాళం);

స్థిర uint64_t *htif_fromhost PT_GUARDED_BY(&htif_తాళం) = ఖాళీ;
స్థిర uint64_t *htif_tohost PT_GUARDED_BY(&htif_తాళం)	 = ఖాళీ;
స్థిర bool htif_custom = false;

స్థిర పూర్ణాంకం htif_console_buf GUARDED_BY(&htif_తాళం);

స్థిర అంతఃవరుస uint64_t __read_tohost(శూన్య) MUST_HOLD(&htif_తాళం)
{
	తిరిగి (htif_custom) ? *htif_tohost : tohost;
}

స్థిర అంతఃవరుస శూన్య __write_tohost(uint64_t val) MUST_HOLD(&htif_తాళం)
{
	ఒకవేళ (htif_custom)
		*htif_tohost = val;
	లేకపోతే
		tohost = val;
}

స్థిర అంతఃవరుస uint64_t __read_fromhost(శూన్య) MUST_HOLD(&htif_తాళం)
{
	తిరిగి (htif_custom) ? *htif_fromhost : fromhost;
}

స్థిర అంతఃవరుస శూన్య __write_fromhost(uint64_t val) MUST_HOLD(&htif_తాళం)
{
	ఒకవేళ (htif_custom)
		*htif_fromhost = val;
	లేకపోతే
		fromhost = val;
}

స్థిర శూన్య __check_fromhost() MUST_HOLD(&htif_తాళం)
{
	uint64_t fh = __read_fromhost();
	ఒకవేళ (!fh)
		తిరిగి;
	__write_fromhost(0);

	/* this should be from the console */
	ఒకవేళ (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	ఎంపిక (FROMHOST_CMD(fh)) {
		స్థితి HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_దత్తాంశం(fh);
			ఆపు;
		స్థితి HTIF_CONSOLE_CMD_PUTC:
			ఆపు;
		అప్రమేయ:
			__builtin_trap();
	}
}

స్థిర శూన్య __set_tohost(uint64_t dev, uint64_t cmd, uint64_t దత్తాంశం)
	MUST_HOLD(&htif_తాళం)
{
	అయితే (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, దత్తాంశం));
}

స్థిర పూర్ణాంకం సమితి_custom_addr(bool custom_addr,
			   గుర్తులేని దీర్ఘ custom_fromhost_addr,
			   గుర్తులేని దీర్ఘ custom_tohost_addr)
{
	ఒకవేళ (custom_addr) {
		ఒకవేళ (htif_custom &&
		    ((custom_fromhost_addr != (గుర్తులేని దీర్ఘ)htif_fromhost) ||
		     (custom_tohost_addr != (గుర్తులేని దీర్ఘ)htif_tohost)))
			తిరిగి SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	తిరిగి 0;
}

#ఒకవేళ __riscv_xlen == 32
స్థిర శూన్య do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t దత్తాంశం)
{
	spin_తాళం(&htif_తాళం);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, దత్తాంశం);

	అయితే (1) {
		uint64_t fh = __read_fromhost();
		ఒకవేళ (fh) {
			ఒకవేళ (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				ఆపు;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_తాళం);
}

స్థిర శూన్య htif_putc(అక్షరం ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	అస్థిర uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#లేకపోతే
స్థిర శూన్య htif_putc(అక్షరం ch)
{
	spin_తాళం(&htif_తాళం);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_తాళం);
}
#అయితే_ముగింపు

స్థిర పూర్ణాంకం htif_getc(శూన్య)
{
	పూర్ణాంకం ch;

#ఒకవేళ __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	తిరిగి -1;
#అయితే_ముగింపు

	spin_తాళం(&htif_తాళం);

	__check_fromhost();
	ch = htif_console_buf;
	ఒకవేళ (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_తాళం);

	తిరిగి ch - 1;
}

స్థిర నిర్మాణం sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

పూర్ణాంకం htif_serial_init(bool custom_addr,
		     గుర్తులేని దీర్ఘ custom_fromhost_addr,
		     గుర్తులేని దీర్ఘ custom_tohost_addr)
{
	పూర్ణాంకం rc;

	rc = సమితి_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	ఒకవేళ (rc)
		తిరిగి rc;

	sbi_console_సమితి_device(&htif_console);
	తిరిగి 0;
}

స్థిర పూర్ణాంకం htif_system_reset_check(u32 type, u32 reason)
{
	తిరిగి 1;
}

స్థిర శూన్య htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	అయితే (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

స్థిర నిర్మాణం sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

పూర్ణాంకం htif_system_reset_init(bool custom_addr,
			   గుర్తులేని దీర్ఘ custom_fromhost_addr,
			   గుర్తులేని దీర్ఘ custom_tohost_addr)
{
	పూర్ణాంకం rc;

	rc = సమితి_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	ఒకవేళ (rc)
		తిరిగి rc;

	sbi_system_reset_కూడు_device(&htif_reset);
	తిరిగి 0;
}
