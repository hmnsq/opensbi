/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 *   Kautuk Consul <kconsul@ventanamicro.com>
 */

#அந்தர்பாவ <sbi/sbi_console.h>
#அந்தர்பாவ <sbi/sbi_string.h>
#அந்தர்பாவ <sbi/sbi_error.h>
#அந்தர்பாவ <sbi_utils/serial/semihosting.h>

#define SYSOPEN     0x01
#define SYSWRITEC   0x03
#define SYSWRITE    0x05
#define SYSREAD     0x06
#define SYSREADC    0x07
#define SYSERRNO	0x13

நிலையான long semihosting_trap(முழு sysnum, வெறுமை *addr)
{
	பதிவு முழு ret asm ("a0") = sysnum;
	பதிவு வெறுமை *param0 asm ("a1") = addr;

	asm நிலையற்ற (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	திரும்பு ret;
}

நிலையான bool _semihosting_enabled = true;
நிலையான bool try_semihosting = true;

bool semihosting_enabled(வெறுமை)
{
	பதிவு முழு ret asm ("a0") = SYSERRNO;
	பதிவு வெறுமை *param0 asm ("a1") = வெற்று;
	unsigned long tmp = 0;

	எனில் (!try_semihosting)
		திரும்பு _semihosting_enabled;

	asm நிலையற்ற (
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
	திரும்பு _semihosting_enabled;
}

நிலையான முழு semihosting_errno(வெறுமை)
{
	long ret = semihosting_trap(SYSERRNO, வெற்று);

	எனில் (ret > 0)
		திரும்பு -ret;
	திரும்பு SBI_EIO;
}

நிலையான முழு semihosting_infd = SBI_ENODEV;
நிலையான முழு semihosting_outfd = SBI_ENODEV;

நிலையான long semihosting_open(மாறிலி எழுத்து *fname, எண்ணிக்கை semihosting_open_mode mode)
{
	long fd;
	கட்டமைப்பு semihosting_open_s {
		மாறிலி எழுத்து *fname;
		unsigned long mode;
		size_t len;
	} open;

	open.fname = fname;
	open.len = sbi_strlen(fname);
	open.mode = mode;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &open);
	எனில் (fd == -1)
		திரும்பு semihosting_errno();
	திரும்பு fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
கட்டமைப்பு semihosting_rdwr_s {
	long fd;
	வெறுமை *memp;
	size_t len;
};

நிலையான long semihosting_read(long fd, வெறுமை *memp, size_t len)
{
	long ret;
	கட்டமைப்பு semihosting_rdwr_s பட;

	பட.fd = fd;
	பட.memp = memp;
	பட.len = len;

	ret = semihosting_trap(SYSREAD, &பட);
	எனில் (ret < 0)
		திரும்பு semihosting_errno();
	திரும்பு len - ret;
}

நிலையான long semihosting_write(long fd, மாறிலி வெறுமை *memp, size_t len)
{
	long ret;
	கட்டமைப்பு semihosting_rdwr_s லிக;

	லிக.fd = fd;
	லிக.memp = (வெறுமை *)memp;
	லிக.len = len;

	ret = semihosting_trap(SYSWRITE, &லிக);
	எனில் (ret < 0)
		திரும்பு semihosting_errno();
	திரும்பு len - ret;
}

/* clang-format on */

நிலையான unsigned long semihosting_puts(மாறிலி எழுத்து *str, unsigned long len)
{
	எழுத்து ch;
	long ret;
	unsigned long i;

	எனில் (semihosting_outfd < 0) {
		க்கு (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} இல்லையெனில்
		ret = semihosting_write(semihosting_outfd, str, len);

	திரும்பு (ret < 0) ? 0 : ret;
}

நிலையான முழு semihosting_getc(வெறுமை)
{
	எழுத்து ch = 0;
	முழு ret;

	எனில் (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, வெற்று);
		ret = ret < 0 ? -1 : ret;
	} இல்லையெனில்
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	திரும்பு ret;
}

நிலையான கட்டமைப்பு sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

முழு semihosting_init(வெறுமை)
{
	semihosting_infd = semihosting_open(":tt", MODE_READ);
	semihosting_outfd = semihosting_open(":tt", MODE_WRITE);

	sbi_console_set_device(&semihosting_console);

	திரும்பு 0;
}
