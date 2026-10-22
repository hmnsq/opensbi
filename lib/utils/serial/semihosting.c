/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 *   Kautuk Consul <kconsul@ventanamicro.com>
 */

#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi_utils/serial/semihosting.h>

#وصف_ڪريو SYSOPEN     0x01
#وصف_ڪريو SYSWRITEC   0x03
#وصف_ڪريو SYSWRITE    0x05
#وصف_ڪريو SYSREAD     0x06
#وصف_ڪريو SYSREADC    0x07
#وصف_ڪريو SYSERRNO	0x13

ساكن ڊگهو semihosting_trap(صحيح sysnum, فراغ *addr)
{
	سجل صحيح ret asm ("a0") = sysnum;
	سجل فراغ *param0 asm ("a1") = addr;

	asm متطاير (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	أرجع ret;
}

ساكن bool _semihosting_enabled = true;
ساكن bool try_semihosting = true;

bool semihosting_enabled(فراغ)
{
	سجل صحيح ret asm ("a0") = SYSERRNO;
	سجل فراغ *param0 asm ("a1") = عدم;
	بنانشان ڊگهو tmp = 0;

	إذا (!try_semihosting)
		أرجع _semihosting_enabled;

	asm متطاير (
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
	أرجع _semihosting_enabled;
}

ساكن صحيح semihosting_errno(فراغ)
{
	ڊگهو ret = semihosting_trap(SYSERRNO, عدم);

	إذا (ret > 0)
		أرجع -ret;
	أرجع SBI_EIO;
}

ساكن صحيح semihosting_infd = SBI_ENODEV;
ساكن صحيح semihosting_outfd = SBI_ENODEV;

ساكن ڊگهو semihosting_open(ثابت حرف *fname, تعداد semihosting_open_گهڻائي گهڻائي)
{
	ڊگهو fd;
	بنية semihosting_open_s {
		ثابت حرف *fname;
		بنانشان ڊگهو گهڻائي;
		size_t len;
	} open;

	open.fname = fname;
	open.len = sbi_strlen(fname);
	open.گهڻائي = گهڻائي;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &open);
	إذا (fd == -1)
		أرجع semihosting_errno();
	أرجع fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
بنية semihosting_rdwr_s {
	ڊگهو fd;
	فراغ *memp;
	size_t len;
};

ساكن ڊگهو semihosting_read(ڊگهو fd, فراغ *memp, size_t len)
{
	ڊگهو ret;
	بنية semihosting_rdwr_s اقرأ;

	اقرأ.fd = fd;
	اقرأ.memp = memp;
	اقرأ.len = len;

	ret = semihosting_trap(SYSREAD, &اقرأ);
	إذا (ret < 0)
		أرجع semihosting_errno();
	أرجع len - ret;
}

ساكن ڊگهو semihosting_write(ڊگهو fd, ثابت فراغ *memp, size_t len)
{
	ڊگهو ret;
	بنية semihosting_rdwr_s اكتب;

	اكتب.fd = fd;
	اكتب.memp = (فراغ *)memp;
	اكتب.len = len;

	ret = semihosting_trap(SYSWRITE, &اكتب);
	إذا (ret < 0)
		أرجع semihosting_errno();
	أرجع len - ret;
}

/* clang-format on */

ساكن بنانشان ڊگهو semihosting_puts(ثابت حرف *str, بنانشان ڊگهو len)
{
	حرف ch;
	ڊگهو ret;
	بنانشان ڊگهو i;

	إذا (semihosting_outfd < 0) {
		لكل (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} وإلا
		ret = semihosting_write(semihosting_outfd, str, len);

	أرجع (ret < 0) ? 0 : ret;
}

ساكن صحيح semihosting_getc(فراغ)
{
	حرف ch = 0;
	صحيح ret;

	إذا (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, عدم);
		ret = ret < 0 ? -1 : ret;
	} وإلا
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	أرجع ret;
}

ساكن بنية sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

صحيح semihosting_init(فراغ)
{
	semihosting_infd = semihosting_open(":tt", گهڻائي_READ);
	semihosting_outfd = semihosting_open(":tt", گهڻائي_WRITE);

	sbi_console_سيٽ_device(&semihosting_console);

	أرجع 0;
}
