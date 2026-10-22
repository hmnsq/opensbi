/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 *   Kautuk Consul <kconsul@ventanamicro.com>
 */

#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi_utils/serial/semihosting.h>

#define SYSOPEN     0x01
#define SYSWRITEC   0x03
#define SYSWRITE    0x05
#define SYSREAD     0x06
#define SYSREADC    0x07
#define SYSERRNO	0x13

ایستا long semihosting_trap(صحیح sysnum, تهی *addr)
{
	ثبات صحیح ret asm ("a0") = sysnum;
	ثبات تهی *param0 asm ("a1") = addr;

	asm فرار (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	بازگشت ret;
}

ایستا bool _semihosting_enabled = true;
ایستا bool try_semihosting = true;

bool semihosting_enabled(تهی)
{
	ثبات صحیح ret asm ("a0") = SYSERRNO;
	ثبات تهی *param0 asm ("a1") = پوچ;
	unsigned long tmp = 0;

	اگر (!try_semihosting)
		بازگشت _semihosting_enabled;

	asm فرار (
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
	بازگشت _semihosting_enabled;
}

ایستا صحیح semihosting_errno(تهی)
{
	long ret = semihosting_trap(SYSERRNO, پوچ);

	اگر (ret > 0)
		بازگشت -ret;
	بازگشت SBI_EIO;
}

ایستا صحیح semihosting_infd = SBI_ENODEV;
ایستا صحیح semihosting_outfd = SBI_ENODEV;

ایستا long semihosting_open(ثابت نویسه *fname, شمارش semihosting_open_mode mode)
{
	long fd;
	ساختار semihosting_open_s {
		ثابت نویسه *fname;
		unsigned long mode;
		size_t len;
	} open;

	open.fname = fname;
	open.len = sbi_strlen(fname);
	open.mode = mode;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &open);
	اگر (fd == -1)
		بازگشت semihosting_errno();
	بازگشت fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
ساختار semihosting_rdwr_s {
	long fd;
	تهی *memp;
	size_t len;
};

ایستا long semihosting_read(long fd, تهی *memp, size_t len)
{
	long ret;
	ساختار semihosting_rdwr_s اقرأ;

	اقرأ.fd = fd;
	اقرأ.memp = memp;
	اقرأ.len = len;

	ret = semihosting_trap(SYSREAD, &اقرأ);
	اگر (ret < 0)
		بازگشت semihosting_errno();
	بازگشت len - ret;
}

ایستا long semihosting_write(long fd, ثابت تهی *memp, size_t len)
{
	long ret;
	ساختار semihosting_rdwr_s اكتب;

	اكتب.fd = fd;
	اكتب.memp = (تهی *)memp;
	اكتب.len = len;

	ret = semihosting_trap(SYSWRITE, &اكتب);
	اگر (ret < 0)
		بازگشت semihosting_errno();
	بازگشت len - ret;
}

/* clang-format on */

ایستا unsigned long semihosting_puts(ثابت نویسه *str, unsigned long len)
{
	نویسه ch;
	long ret;
	unsigned long i;

	اگر (semihosting_outfd < 0) {
		برای (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} وگرنه
		ret = semihosting_write(semihosting_outfd, str, len);

	بازگشت (ret < 0) ? 0 : ret;
}

ایستا صحیح semihosting_getc(تهی)
{
	نویسه ch = 0;
	صحیح ret;

	اگر (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, پوچ);
		ret = ret < 0 ? -1 : ret;
	} وگرنه
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	بازگشت ret;
}

ایستا ساختار sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

صحیح semihosting_init(تهی)
{
	semihosting_infd = semihosting_open(":tt", MODE_READ);
	semihosting_outfd = semihosting_open(":tt", MODE_WRITE);

	sbi_console_set_device(&semihosting_console);

	بازگشت 0;
}
