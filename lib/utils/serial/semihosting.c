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

#تعریف_کرو SYSOPEN     0x01
#تعریف_کرو SYSWRITEC   0x03
#تعریف_کرو SYSWRITE    0x05
#تعریف_کرو SYSREAD     0x06
#تعریف_کرو SYSREADC    0x07
#تعریف_کرو SYSERRNO	0x13

ساکن طویل semihosting_trap(صحیح sysnum, خالی *addr)
{
	اندراج صحیح ret asm ("a0") = sysnum;
	اندراج خالی *param0 asm ("a1") = addr;

	asm متلون (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	واپس ret;
}

ساکن bool _semihosting_enabled = true;
ساکن bool try_semihosting = true;

bool semihosting_enabled(خالی)
{
	اندراج صحیح ret asm ("a0") = SYSERRNO;
	اندراج خالی *param0 asm ("a1") = تہی;
	غیرعلامتی طویل tmp = 0;

	اگر (!try_semihosting)
		واپس _semihosting_enabled;

	asm متلون (
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
	واپس _semihosting_enabled;
}

ساکن صحیح semihosting_errno(خالی)
{
	طویل ret = semihosting_trap(SYSERRNO, تہی);

	اگر (ret > 0)
		واپس -ret;
	واپس SBI_EIO;
}

ساکن صحیح semihosting_infd = SBI_ENODEV;
ساکن صحیح semihosting_outfd = SBI_ENODEV;

ساکن طویل semihosting_open(ثابت حرف *fname, شمار semihosting_open_منوال منوال)
{
	طویل fd;
	ساخت semihosting_open_s {
		ثابت حرف *fname;
		غیرعلامتی طویل منوال;
		size_t len;
	} open;

	open.fname = fname;
	open.len = sbi_strlen(fname);
	open.منوال = منوال;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &open);
	اگر (fd == -1)
		واپس semihosting_errno();
	واپس fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
ساخت semihosting_rdwr_s {
	طویل fd;
	خالی *memp;
	size_t len;
};

ساکن طویل semihosting_read(طویل fd, خالی *memp, size_t len)
{
	طویل ret;
	ساخت semihosting_rdwr_s اقرأ;

	اقرأ.fd = fd;
	اقرأ.memp = memp;
	اقرأ.len = len;

	ret = semihosting_trap(SYSREAD, &اقرأ);
	اگر (ret < 0)
		واپس semihosting_errno();
	واپس len - ret;
}

ساکن طویل semihosting_write(طویل fd, ثابت خالی *memp, size_t len)
{
	طویل ret;
	ساخت semihosting_rdwr_s اكتب;

	اكتب.fd = fd;
	اكتب.memp = (خالی *)memp;
	اكتب.len = len;

	ret = semihosting_trap(SYSWRITE, &اكتب);
	اگر (ret < 0)
		واپس semihosting_errno();
	واپس len - ret;
}

/* clang-format on */

ساکن غیرعلامتی طویل semihosting_puts(ثابت حرف *str, غیرعلامتی طویل len)
{
	حرف ch;
	طویل ret;
	غیرعلامتی طویل i;

	اگر (semihosting_outfd < 0) {
		برائے (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} ورنہ
		ret = semihosting_write(semihosting_outfd, str, len);

	واپس (ret < 0) ? 0 : ret;
}

ساکن صحیح semihosting_getc(خالی)
{
	حرف ch = 0;
	صحیح ret;

	اگر (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, تہی);
		ret = ret < 0 ? -1 : ret;
	} ورنہ
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	واپس ret;
}

ساکن ساخت sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

صحیح semihosting_init(خالی)
{
	semihosting_infd = semihosting_open(":tt", منوال_READ);
	semihosting_outfd = semihosting_open(":tt", منوال_WRITE);

	sbi_console_سیٹ_device(&semihosting_console);

	واپس 0;
}
