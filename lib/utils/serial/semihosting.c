/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 *   Kautuk Consul <kconsul@ventanamicro.com>
 */

#ଅନ୍ତର୍ଭାଵ <sbi/sbi_console.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_string.h>
#ଅନ୍ତର୍ଭାଵ <sbi/sbi_error.h>
#ଅନ୍ତର୍ଭାଵ <sbi_utils/serial/semihosting.h>

#define SYSOPEN     0x01
#define SYSWRITEC   0x03
#define SYSWRITE    0x05
#define SYSREAD     0x06
#define SYSREADC    0x07
#define SYSERRNO	0x13

ସ୍ଥିର long semihosting_trap(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysnum, ଶୂନ୍ୟ *addr)
{
	ପଞ୍ଜିକରଣ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret asm ("a0") = sysnum;
	ପଞ୍ଜିକରଣ ଶୂନ୍ୟ *param0 asm ("a1") = addr;

	asm ଅସ୍ଥିର (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର bool _semihosting_enabled = true;
ସ୍ଥିର bool try_semihosting = true;

bool semihosting_enabled(ଶୂନ୍ୟ)
{
	ପଞ୍ଜିକରଣ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret asm ("a0") = SYSERRNO;
	ପଞ୍ଜିକରଣ ଶୂନ୍ୟ *param0 asm ("a1") = ଖାଲି;
	unsigned long tmp = 0;

	ଯଦି (!try_semihosting)
		ଫେରନ୍ତୁ _semihosting_enabled;

	asm ଅସ୍ଥିର (
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
	ଫେରନ୍ତୁ _semihosting_enabled;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା semihosting_errno(ଶୂନ୍ୟ)
{
	long ret = semihosting_trap(SYSERRNO, ଖାଲି);

	ଯଦି (ret > 0)
		ଫେରନ୍ତୁ -ret;
	ଫେରନ୍ତୁ SBI_EIO;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା semihosting_infd = SBI_ENODEV;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା semihosting_outfd = SBI_ENODEV;

ସ୍ଥିର long semihosting_open(ଅଚଳ ଅକ୍ଷର *fname, ଗଣନା semihosting_open_mode mode)
{
	long fd;
	ସଂରଚନା semihosting_open_s {
		ଅଚଳ ଅକ୍ଷର *fname;
		unsigned long mode;
		size_t len;
	} open;

	open.fname = fname;
	open.len = sbi_strlen(fname);
	open.mode = mode;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &open);
	ଯଦି (fd == -1)
		ଫେରନ୍ତୁ semihosting_errno();
	ଫେରନ୍ତୁ fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
ସଂରଚନା semihosting_rdwr_s {
	long fd;
	ଶୂନ୍ୟ *memp;
	size_t len;
};

ସ୍ଥିର long semihosting_read(long fd, ଶୂନ୍ୟ *memp, size_t len)
{
	long ret;
	ସଂରଚନା semihosting_rdwr_s ପଠ;

	ପଠ.fd = fd;
	ପଠ.memp = memp;
	ପଠ.len = len;

	ret = semihosting_trap(SYSREAD, &ପଠ);
	ଯଦି (ret < 0)
		ଫେରନ୍ତୁ semihosting_errno();
	ଫେରନ୍ତୁ len - ret;
}

ସ୍ଥିର long semihosting_write(long fd, ଅଚଳ ଶୂନ୍ୟ *memp, size_t len)
{
	long ret;
	ସଂରଚନା semihosting_rdwr_s ଲିଖ;

	ଲିଖ.fd = fd;
	ଲିଖ.memp = (ଶୂନ୍ୟ *)memp;
	ଲିଖ.len = len;

	ret = semihosting_trap(SYSWRITE, &ଲିଖ);
	ଯଦି (ret < 0)
		ଫେରନ୍ତୁ semihosting_errno();
	ଫେରନ୍ତୁ len - ret;
}

/* clang-format on */

ସ୍ଥିର unsigned long semihosting_puts(ଅଚଳ ଅକ୍ଷର *str, unsigned long len)
{
	ଅକ୍ଷର ch;
	long ret;
	unsigned long i;

	ଯଦି (semihosting_outfd < 0) {
		ପାଇଁ (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} ଅନ୍ୟଥା
		ret = semihosting_write(semihosting_outfd, str, len);

	ଫେରନ୍ତୁ (ret < 0) ? 0 : ret;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା semihosting_getc(ଶୂନ୍ୟ)
{
	ଅକ୍ଷର ch = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

	ଯଦି (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, ଖାଲି);
		ret = ret < 0 ? -1 : ret;
	} ଅନ୍ୟଥା
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ସଂରଚନା sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା semihosting_init(ଶୂନ୍ୟ)
{
	semihosting_infd = semihosting_open(":tt", MODE_READ);
	semihosting_outfd = semihosting_open(":tt", MODE_WRITE);

	sbi_console_set_device(&semihosting_console);

	ଫେରନ୍ତୁ 0;
}
