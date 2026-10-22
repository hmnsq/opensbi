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

#ವ್ಯಾಖ್ಯಾನಿಸು SYSOPEN     0x01
#ವ್ಯಾಖ್ಯಾನಿಸು SYSWRITEC   0x03
#ವ್ಯಾಖ್ಯಾನಿಸು SYSWRITE    0x05
#ವ್ಯಾಖ್ಯಾನಿಸು SYSREAD     0x06
#ವ್ಯಾಖ್ಯಾನಿಸು SYSREADC    0x07
#ವ್ಯಾಖ್ಯಾನಿಸು SYSERRNO	0x13

ಸ್ಥಿರ ದೀರ್ಘ semihosting_trap(ಪೂರ್ಣಾಂಕ sysnum, ಶೂನ್ಯ *addr)
{
	ನೋಂದಣಿ ಪೂರ್ಣಾಂಕ ret asm ("a0") = sysnum;
	ನೋಂದಣಿ ಶೂನ್ಯ *param0 asm ("a1") = addr;

	asm ಅಸ್ಥಿರ (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ bool _semihosting_enabled = true;
ಸ್ಥಿರ bool try_semihosting = true;

bool semihosting_enabled(ಶೂನ್ಯ)
{
	ನೋಂದಣಿ ಪೂರ್ಣಾಂಕ ret asm ("a0") = SYSERRNO;
	ನೋಂದಣಿ ಶೂನ್ಯ *param0 asm ("a1") = ಖಾಲಿ;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tmp = 0;

	ಆದರೆ (!try_semihosting)
		ಹಿಂತಿರುಗಿ _semihosting_enabled;

	asm ಅಸ್ಥಿರ (
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
	ಹಿಂತಿರುಗಿ _semihosting_enabled;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ semihosting_errno(ಶೂನ್ಯ)
{
	ದೀರ್ಘ ret = semihosting_trap(SYSERRNO, ಖಾಲಿ);

	ಆದರೆ (ret > 0)
		ಹಿಂತಿರುಗಿ -ret;
	ಹಿಂತಿರುಗಿ SBI_EIO;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ semihosting_infd = SBI_ENODEV;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ semihosting_outfd = SBI_ENODEV;

ಸ್ಥಿರ ದೀರ್ಘ semihosting_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, ಎಣಿಕೆ semihosting_open_ಬಹುಲಕ ಬಹುಲಕ)
{
	ದೀರ್ಘ fd;
	ರಚನೆ semihosting_open_s {
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname;
		ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಬಹುಲಕ;
		size_t len;
	} open;

	open.fname = fname;
	open.len = sbi_strlen(fname);
	open.ಬಹುಲಕ = ಬಹುಲಕ;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &open);
	ಆದರೆ (fd == -1)
		ಹಿಂತಿರುಗಿ semihosting_errno();
	ಹಿಂತಿರುಗಿ fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
ರಚನೆ semihosting_rdwr_s {
	ದೀರ್ಘ fd;
	ಶೂನ್ಯ *memp;
	size_t len;
};

ಸ್ಥಿರ ದೀರ್ಘ semihosting_read(ದೀರ್ಘ fd, ಶೂನ್ಯ *memp, size_t len)
{
	ದೀರ್ಘ ret;
	ರಚನೆ semihosting_rdwr_s ಪಠ;

	ಪಠ.fd = fd;
	ಪಠ.memp = memp;
	ಪಠ.len = len;

	ret = semihosting_trap(SYSREAD, &ಪಠ);
	ಆದರೆ (ret < 0)
		ಹಿಂತಿರುಗಿ semihosting_errno();
	ಹಿಂತಿರುಗಿ len - ret;
}

ಸ್ಥಿರ ದೀರ್ಘ semihosting_write(ದೀರ್ಘ fd, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *memp, size_t len)
{
	ದೀರ್ಘ ret;
	ರಚನೆ semihosting_rdwr_s ಲಿಖ;

	ಲಿಖ.fd = fd;
	ಲಿಖ.memp = (ಶೂನ್ಯ *)memp;
	ಲಿಖ.len = len;

	ret = semihosting_trap(SYSWRITE, &ಲಿಖ);
	ಆದರೆ (ret < 0)
		ಹಿಂತಿರುಗಿ semihosting_errno();
	ಹಿಂತಿರುಗಿ len - ret;
}

/* clang-format on */

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ semihosting_puts(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ len)
{
	ಅಕ್ಷರ ch;
	ದೀರ್ಘ ret;
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i;

	ಆದರೆ (semihosting_outfd < 0) {
		ಗಾಗಿ (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} ಇಲ್ಲದಿದ್ದರೆ
		ret = semihosting_write(semihosting_outfd, str, len);

	ಹಿಂತಿರುಗಿ (ret < 0) ? 0 : ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ semihosting_getc(ಶೂನ್ಯ)
{
	ಅಕ್ಷರ ch = 0;
	ಪೂರ್ಣಾಂಕ ret;

	ಆದರೆ (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, ಖಾಲಿ);
		ret = ret < 0 ? -1 : ret;
	} ಇಲ್ಲದಿದ್ದರೆ
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ರಚನೆ sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

ಪೂರ್ಣಾಂಕ semihosting_init(ಶೂನ್ಯ)
{
	semihosting_infd = semihosting_open(":tt", ಬಹುಲಕ_READ);
	semihosting_outfd = semihosting_open(":tt", ಬಹುಲಕ_WRITE);

	sbi_console_ಗಣ_device(&semihosting_console);

	ಹಿಂತಿರುಗಿ 0;
}
