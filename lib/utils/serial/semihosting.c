/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 *   Kautuk Consul <kconsul@ventanamicro.com>
 */

#അന്തര്ഭാവ <sbi/sbi_console.h>
#അന്തര്ഭാവ <sbi/sbi_string.h>
#അന്തര്ഭാവ <sbi/sbi_error.h>
#അന്തര്ഭാവ <sbi_utils/serial/semihosting.h>

#define SYSOPEN     0x01
#define SYSWRITEC   0x03
#define SYSWRITE    0x05
#define SYSREAD     0x06
#define SYSREADC    0x07
#define SYSERRNO	0x13

സ്ഥിതം long semihosting_trap(പൂർണ്ണസംഖ്യ sysnum, ശൂന്യം *addr)
{
	രജിസ്റ്റർ പൂർണ്ണസംഖ്യ ret asm ("a0") = sysnum;
	രജിസ്റ്റർ ശൂന്യം *param0 asm ("a1") = addr;

	asm അസ്ഥിര (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	തിരികെ ret;
}

സ്ഥിതം bool _semihosting_enabled = true;
സ്ഥിതം bool try_semihosting = true;

bool semihosting_enabled(ശൂന്യം)
{
	രജിസ്റ്റർ പൂർണ്ണസംഖ്യ ret asm ("a0") = SYSERRNO;
	രജിസ്റ്റർ ശൂന്യം *param0 asm ("a1") = ശൂന്യ;
	unsigned long tmp = 0;

	എങ്കിൽ (!try_semihosting)
		തിരികെ _semihosting_enabled;

	asm അസ്ഥിര (
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
	തിരികെ _semihosting_enabled;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ semihosting_errno(ശൂന്യം)
{
	long ret = semihosting_trap(SYSERRNO, ശൂന്യ);

	എങ്കിൽ (ret > 0)
		തിരികെ -ret;
	തിരികെ SBI_EIO;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ semihosting_infd = SBI_ENODEV;
സ്ഥിതം പൂർണ്ണസംഖ്യ semihosting_outfd = SBI_ENODEV;

സ്ഥിതം long semihosting_open(സ്ഥിരം അക്ഷരം *fname, എണ്ണൽ semihosting_open_mode mode)
{
	long fd;
	ഘടന semihosting_open_s {
		സ്ഥിരം അക്ഷരം *fname;
		unsigned long mode;
		size_t len;
	} open;

	open.fname = fname;
	open.len = sbi_strlen(fname);
	open.mode = mode;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &open);
	എങ്കിൽ (fd == -1)
		തിരികെ semihosting_errno();
	തിരികെ fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
ഘടന semihosting_rdwr_s {
	long fd;
	ശൂന്യം *memp;
	size_t len;
};

സ്ഥിതം long semihosting_read(long fd, ശൂന്യം *memp, size_t len)
{
	long ret;
	ഘടന semihosting_rdwr_s പഠ;

	പഠ.fd = fd;
	പഠ.memp = memp;
	പഠ.len = len;

	ret = semihosting_trap(SYSREAD, &പഠ);
	എങ്കിൽ (ret < 0)
		തിരികെ semihosting_errno();
	തിരികെ len - ret;
}

സ്ഥിതം long semihosting_write(long fd, സ്ഥിരം ശൂന്യം *memp, size_t len)
{
	long ret;
	ഘടന semihosting_rdwr_s ലിഖ;

	ലിഖ.fd = fd;
	ലിഖ.memp = (ശൂന്യം *)memp;
	ലിഖ.len = len;

	ret = semihosting_trap(SYSWRITE, &ലിഖ);
	എങ്കിൽ (ret < 0)
		തിരികെ semihosting_errno();
	തിരികെ len - ret;
}

/* clang-format on */

സ്ഥിതം unsigned long semihosting_puts(സ്ഥിരം അക്ഷരം *str, unsigned long len)
{
	അക്ഷരം ch;
	long ret;
	unsigned long i;

	എങ്കിൽ (semihosting_outfd < 0) {
		വേണ്ടി (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} അല്ലെങ്കിൽ
		ret = semihosting_write(semihosting_outfd, str, len);

	തിരികെ (ret < 0) ? 0 : ret;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ semihosting_getc(ശൂന്യം)
{
	അക്ഷരം ch = 0;
	പൂർണ്ണസംഖ്യ ret;

	എങ്കിൽ (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, ശൂന്യ);
		ret = ret < 0 ? -1 : ret;
	} അല്ലെങ്കിൽ
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	തിരികെ ret;
}

സ്ഥിതം ഘടന sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

പൂർണ്ണസംഖ്യ semihosting_init(ശൂന്യം)
{
	semihosting_infd = semihosting_open(":tt", MODE_READ);
	semihosting_outfd = semihosting_open(":tt", MODE_WRITE);

	sbi_console_set_device(&semihosting_console);

	തിരികെ 0;
}
