/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 *   Kautuk Consul <kconsul@ventanamicro.com>
 */

#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_string.h>
#অন্তর্ভাব <sbi/sbi_error.h>
#অন্তর্ভাব <sbi_utils/serial/semihosting.h>

#define SYSOPEN     0x01
#define SYSWRITEC   0x03
#define SYSWRITE    0x05
#define SYSREAD     0x06
#define SYSREADC    0x07
#define SYSERRNO	0x13

স্থির long semihosting_trap(পূর্ণসংখ্যা sysnum, শূন্য *addr)
{
	নিবন্ধ পূর্ণসংখ্যা ret asm ("a0") = sysnum;
	নিবন্ধ শূন্য *param0 asm ("a1") = addr;

	asm অস্থির (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	ফেরত ret;
}

স্থির bool _semihosting_enabled = true;
স্থির bool try_semihosting = true;

bool semihosting_enabled(শূন্য)
{
	নিবন্ধ পূর্ণসংখ্যা ret asm ("a0") = SYSERRNO;
	নিবন্ধ শূন্য *param0 asm ("a1") = ফাঁকা;
	unsigned long tmp = 0;

	যদি (!try_semihosting)
		ফেরত _semihosting_enabled;

	asm অস্থির (
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
	ফেরত _semihosting_enabled;
}

স্থির পূর্ণসংখ্যা semihosting_errno(শূন্য)
{
	long ret = semihosting_trap(SYSERRNO, ফাঁকা);

	যদি (ret > 0)
		ফেরত -ret;
	ফেরত SBI_EIO;
}

স্থির পূর্ণসংখ্যা semihosting_infd = SBI_ENODEV;
স্থির পূর্ণসংখ্যা semihosting_outfd = SBI_ENODEV;

স্থির long semihosting_open(ধ্রুবক অক্ষর *fname, গণনা semihosting_open_mode mode)
{
	long fd;
	কাঠামো semihosting_open_s {
		ধ্রুবক অক্ষর *fname;
		unsigned long mode;
		size_t len;
	} open;

	open.fname = fname;
	open.len = sbi_strlen(fname);
	open.mode = mode;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &open);
	যদি (fd == -1)
		ফেরত semihosting_errno();
	ফেরত fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
কাঠামো semihosting_rdwr_s {
	long fd;
	শূন্য *memp;
	size_t len;
};

স্থির long semihosting_read(long fd, শূন্য *memp, size_t len)
{
	long ret;
	কাঠামো semihosting_rdwr_s পঠ;

	পঠ.fd = fd;
	পঠ.memp = memp;
	পঠ.len = len;

	ret = semihosting_trap(SYSREAD, &পঠ);
	যদি (ret < 0)
		ফেরত semihosting_errno();
	ফেরত len - ret;
}

স্থির long semihosting_write(long fd, ধ্রুবক শূন্য *memp, size_t len)
{
	long ret;
	কাঠামো semihosting_rdwr_s লিখ;

	লিখ.fd = fd;
	লিখ.memp = (শূন্য *)memp;
	লিখ.len = len;

	ret = semihosting_trap(SYSWRITE, &লিখ);
	যদি (ret < 0)
		ফেরত semihosting_errno();
	ফেরত len - ret;
}

/* clang-format on */

স্থির unsigned long semihosting_puts(ধ্রুবক অক্ষর *str, unsigned long len)
{
	অক্ষর ch;
	long ret;
	unsigned long i;

	যদি (semihosting_outfd < 0) {
		জন্য (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} অন্যথা
		ret = semihosting_write(semihosting_outfd, str, len);

	ফেরত (ret < 0) ? 0 : ret;
}

স্থির পূর্ণসংখ্যা semihosting_getc(শূন্য)
{
	অক্ষর ch = 0;
	পূর্ণসংখ্যা ret;

	যদি (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, ফাঁকা);
		ret = ret < 0 ? -1 : ret;
	} অন্যথা
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	ফেরত ret;
}

স্থির কাঠামো sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

পূর্ণসংখ্যা semihosting_init(শূন্য)
{
	semihosting_infd = semihosting_open(":tt", MODE_READ);
	semihosting_outfd = semihosting_open(":tt", MODE_WRITE);

	sbi_console_set_device(&semihosting_console);

	ফেরত 0;
}
