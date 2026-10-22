/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 *   Kautuk Consul <kconsul@ventanamicro.com>
 */

#ಅನ್ತರ್ಭಾವ <sbi/sbi_console.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_string.h>
#ಅನ್ತರ್ಭಾವ <sbi/sbi_error.h>
#ಅನ್ತರ್ಭಾವ <sbi_utils/serial/semihosting.h>

#define SYSOPEN     0x01
#define SYSWRITEC   0x03
#define SYSWRITE    0x05
#define SYSREAD     0x06
#define SYSREADC    0x07
#define SYSERRNO	0x13

ಸ್ಥಿರ long semihosting_trap(ಪೂರ್ಣಾಙ್ಕ sysnum, ಶೂನ್ಯ *addr)
{
	ಪಞ್ಜಿಕಾ ಪೂರ್ಣಾಙ್ಕ ret asm ("a0") = sysnum;
	ಪಞ್ಜಿಕಾ ಶೂನ್ಯ *param0 asm ("a1") = addr;

	asm ಚಞ್ಚಲ (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ bool _semihosting_enabled = true;
ಸ್ಥಿರ bool try_semihosting = true;

bool semihosting_enabled(ಶೂನ್ಯ)
{
	ಪಞ್ಜಿಕಾ ಪೂರ್ಣಾಙ್ಕ ret asm ("a0") = SYSERRNO;
	ಪಞ್ಜಿಕಾ ಶೂನ್ಯ *param0 asm ("a1") = ರಿಕ್ತ;
	unsigned long tmp = 0;

	ಯದಿ (!try_semihosting)
		ಪ್ರತ್ಯಾಗಮ _semihosting_enabled;

	asm ಚಞ್ಚಲ (
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
	ಪ್ರತ್ಯಾಗಮ _semihosting_enabled;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ semihosting_errno(ಶೂನ್ಯ)
{
	long ret = semihosting_trap(SYSERRNO, ರಿಕ್ತ);

	ಯದಿ (ret > 0)
		ಪ್ರತ್ಯಾಗಮ -ret;
	ಪ್ರತ್ಯಾಗಮ SBI_EIO;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ semihosting_infd = SBI_ENODEV;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ semihosting_outfd = SBI_ENODEV;

ಸ್ಥಿರ long semihosting_open(ಧ್ರುವ ವರ್ಣ *fname, ಗಣನಾ semihosting_open_mode mode)
{
	long fd;
	ಸಂರಚನಾ semihosting_open_s {
		ಧ್ರುವ ವರ್ಣ *fname;
		unsigned long mode;
		size_t len;
	} open;

	open.fname = fname;
	open.len = sbi_strlen(fname);
	open.mode = mode;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &open);
	ಯದಿ (fd == -1)
		ಪ್ರತ್ಯಾಗಮ semihosting_errno();
	ಪ್ರತ್ಯಾಗಮ fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
ಸಂರಚನಾ semihosting_rdwr_s {
	long fd;
	ಶೂನ್ಯ *memp;
	size_t len;
};

ಸ್ಥಿರ long semihosting_read(long fd, ಶೂನ್ಯ *memp, size_t len)
{
	long ret;
	ಸಂರಚನಾ semihosting_rdwr_s ಪಠ;

	ಪಠ.fd = fd;
	ಪಠ.memp = memp;
	ಪಠ.len = len;

	ret = semihosting_trap(SYSREAD, &ಪಠ);
	ಯದಿ (ret < 0)
		ಪ್ರತ್ಯಾಗಮ semihosting_errno();
	ಪ್ರತ್ಯಾಗಮ len - ret;
}

ಸ್ಥಿರ long semihosting_write(long fd, ಧ್ರುವ ಶೂನ್ಯ *memp, size_t len)
{
	long ret;
	ಸಂರಚನಾ semihosting_rdwr_s ಲಿಖ;

	ಲಿಖ.fd = fd;
	ಲಿಖ.memp = (ಶೂನ್ಯ *)memp;
	ಲಿಖ.len = len;

	ret = semihosting_trap(SYSWRITE, &ಲಿಖ);
	ಯದಿ (ret < 0)
		ಪ್ರತ್ಯಾಗಮ semihosting_errno();
	ಪ್ರತ್ಯಾಗಮ len - ret;
}

/* clang-format on */

ಸ್ಥಿರ unsigned long semihosting_puts(ಧ್ರುವ ವರ್ಣ *str, unsigned long len)
{
	ವರ್ಣ ch;
	long ret;
	unsigned long i;

	ಯದಿ (semihosting_outfd < 0) {
		ಪ್ರತಿ (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} ಅನ್ಯಥಾ
		ret = semihosting_write(semihosting_outfd, str, len);

	ಪ್ರತ್ಯಾಗಮ (ret < 0) ? 0 : ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ semihosting_getc(ಶೂನ್ಯ)
{
	ವರ್ಣ ch = 0;
	ಪೂರ್ಣಾಙ್ಕ ret;

	ಯದಿ (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, ರಿಕ್ತ);
		ret = ret < 0 ? -1 : ret;
	} ಅನ್ಯಥಾ
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಸಂರಚನಾ sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

ಪೂರ್ಣಾಙ್ಕ semihosting_init(ಶೂನ್ಯ)
{
	semihosting_infd = semihosting_open(":tt", MODE_READ);
	semihosting_outfd = semihosting_open(":tt", MODE_WRITE);

	sbi_console_set_device(&semihosting_console);

	ಪ್ರತ್ಯಾಗಮ 0;
}
