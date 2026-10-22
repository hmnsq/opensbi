/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 *   Kautuk Consul <kconsul@ventanamicro.com>
 */

#అన్తర్భావ <sbi/sbi_console.h>
#అన్తర్భావ <sbi/sbi_string.h>
#అన్తర్భావ <sbi/sbi_error.h>
#అన్తర్భావ <sbi_utils/serial/semihosting.h>

#నిర్వచించు SYSOPEN     0x01
#నిర్వచించు SYSWRITEC   0x03
#నిర్వచించు SYSWRITE    0x05
#నిర్వచించు SYSREAD     0x06
#నిర్వచించు SYSREADC    0x07
#నిర్వచించు SYSERRNO	0x13

స్థిర దీర్ఘ semihosting_trap(పూర్ణాంకం sysnum, శూన్య *addr)
{
	నమోదు పూర్ణాంకం ret asm ("a0") = sysnum;
	నమోదు శూన్య *param0 asm ("a1") = addr;

	asm అస్థిర (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	తిరిగి ret;
}

స్థిర bool _semihosting_enabled = true;
స్థిర bool try_semihosting = true;

bool semihosting_enabled(శూన్య)
{
	నమోదు పూర్ణాంకం ret asm ("a0") = SYSERRNO;
	నమోదు శూన్య *param0 asm ("a1") = ఖాళీ;
	గుర్తులేని దీర్ఘ tmp = 0;

	ఒకవేళ (!try_semihosting)
		తిరిగి _semihosting_enabled;

	asm అస్థిర (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	j _semihost_test_vector_next\n"
		"	.align 4\n"
		"_semihost_test_vector:\n"
		"	csrr %[en], mepc\n"
		"	addi %[en], %[en], 4\n"
		"	csrw mepc, %[en]\n"
		"	add %[en], zero, zero\n"
		"	mret\n"
		"_semihost_test_vector_next:\n"

		"	lla %[tmp], _semihost_test_vector\n"
		"	csrrw %[tmp], mtvec, %[tmp]\n"
		"	.align 4\n"
		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"
		"	csrw mtvec, %[tmp]\n"

		"	.option pop\n"
		: [tmp] "+r" (tmp), [en] "+r" (_semihosting_enabled),
		  [ret] "+r" (ret)
		: "r" (param0) : "memory");

	try_semihosting = false;
	తిరిగి _semihosting_enabled;
}

స్థిర పూర్ణాంకం semihosting_errno(శూన్య)
{
	దీర్ఘ ret = semihosting_trap(SYSERRNO, ఖాళీ);

	ఒకవేళ (ret > 0)
		తిరిగి -ret;
	తిరిగి SBI_EIO;
}

స్థిర పూర్ణాంకం semihosting_infd = SBI_ENODEV;
స్థిర పూర్ణాంకం semihosting_outfd = SBI_ENODEV;

స్థిర దీర్ఘ semihosting_open(స్థిరరాశి అక్షరం *fname, గణన semihosting_open_బహుళకం బహుళకం)
{
	దీర్ఘ fd;
	నిర్మాణం semihosting_open_s {
		స్థిరరాశి అక్షరం *fname;
		గుర్తులేని దీర్ఘ బహుళకం;
		size_t len;
	} open;

	open.fname = fname;
	open.len = sbi_strlen(fname);
	open.బహుళకం = బహుళకం;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &open);
	ఒకవేళ (fd == -1)
		తిరిగి semihosting_errno();
	తిరిగి fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
నిర్మాణం semihosting_rdwr_s {
	దీర్ఘ fd;
	శూన్య *memp;
	size_t len;
};

స్థిర దీర్ఘ semihosting_read(దీర్ఘ fd, శూన్య *memp, size_t len)
{
	దీర్ఘ ret;
	నిర్మాణం semihosting_rdwr_s పఠ;

	పఠ.fd = fd;
	పఠ.memp = memp;
	పఠ.len = len;

	ret = semihosting_trap(SYSREAD, &పఠ);
	ఒకవేళ (ret < 0)
		తిరిగి semihosting_errno();
	తిరిగి len - ret;
}

స్థిర దీర్ఘ semihosting_write(దీర్ఘ fd, స్థిరరాశి శూన్య *memp, size_t len)
{
	దీర్ఘ ret;
	నిర్మాణం semihosting_rdwr_s లిఖ;

	లిఖ.fd = fd;
	లిఖ.memp = (శూన్య *)memp;
	లిఖ.len = len;

	ret = semihosting_trap(SYSWRITE, &లిఖ);
	ఒకవేళ (ret < 0)
		తిరిగి semihosting_errno();
	తిరిగి len - ret;
}

/* clang-format on */

స్థిర గుర్తులేని దీర్ఘ semihosting_puts(స్థిరరాశి అక్షరం *str, గుర్తులేని దీర్ఘ len)
{
	అక్షరం ch;
	దీర్ఘ ret;
	గుర్తులేని దీర్ఘ i;

	ఒకవేళ (semihosting_outfd < 0) {
		కొరకు (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} లేకపోతే
		ret = semihosting_write(semihosting_outfd, str, len);

	తిరిగి (ret < 0) ? 0 : ret;
}

స్థిర పూర్ణాంకం semihosting_getc(శూన్య)
{
	అక్షరం ch = 0;
	పూర్ణాంకం ret;

	ఒకవేళ (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, ఖాళీ);
		ret = ret < 0 ? -1 : ret;
	} లేకపోతే
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	తిరిగి ret;
}

స్థిర నిర్మాణం sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

పూర్ణాంకం semihosting_init(శూన్య)
{
	semihosting_infd = semihosting_open(":tt", బహుళకం_READ);
	semihosting_outfd = semihosting_open(":tt", బహుళకం_WRITE);

	sbi_console_సమితి_device(&semihosting_console);

	తిరిగి 0;
}
