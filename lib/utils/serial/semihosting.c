/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 *   Kautuk Consul <kconsul@ventanamicro.com>
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_console.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi/sbi_error.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sbi_utils/serial/semihosting.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYSOPEN     0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYSWRITEC   0x03
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYSWRITE    0x05
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYSREAD     0x06
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYSREADC    0x07
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYSERRNO	0x13

ਸਥਿਰ ਲੰਮਾ semihosting_trap(ਪੂਰਨ sysnum, ਖਾਲੀ *addr)
{
	ਰਜਿਸਟਰ ਪੂਰਨ ret asm ("a0") = sysnum;
	ਰਜਿਸਟਰ ਖਾਲੀ *param0 asm ("a1") = addr;

	asm ਅਸਥਿਰ (
		"	.align 4\n"
		"	.option push\n"
		"	.option norvc\n"

		"	slli zero, zero, 0x1f\n"
		"	ebreak\n"
		"	srai zero, zero, 7\n"

		"	.option pop\n"
		: "+r" (ret) : "r" (param0) : "memory");

	ਵਾਪਸ ret;
}

ਸਥਿਰ bool _semihosting_enabled = true;
ਸਥਿਰ bool try_semihosting = true;

bool semihosting_enabled(ਖਾਲੀ)
{
	ਰਜਿਸਟਰ ਪੂਰਨ ret asm ("a0") = SYSERRNO;
	ਰਜਿਸਟਰ ਖਾਲੀ *param0 asm ("a1") = ਨਿਰਾ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tmp = 0;

	ਜੇ (!try_semihosting)
		ਵਾਪਸ _semihosting_enabled;

	asm ਅਸਥਿਰ (
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
	ਵਾਪਸ _semihosting_enabled;
}

ਸਥਿਰ ਪੂਰਨ semihosting_errno(ਖਾਲੀ)
{
	ਲੰਮਾ ret = semihosting_trap(SYSERRNO, ਨਿਰਾ);

	ਜੇ (ret > 0)
		ਵਾਪਸ -ret;
	ਵਾਪਸ SBI_EIO;
}

ਸਥਿਰ ਪੂਰਨ semihosting_infd = SBI_ENODEV;
ਸਥਿਰ ਪੂਰਨ semihosting_outfd = SBI_ENODEV;

ਸਥਿਰ ਲੰਮਾ semihosting_open(ਅਚਲ ਅੱਖਰ *fname, ਗਿਣਤੀ semihosting_open_ਬਹੁਲਕ ਬਹੁਲਕ)
{
	ਲੰਮਾ fd;
	ਬਣਤਰ semihosting_open_s {
		ਅਚਲ ਅੱਖਰ *fname;
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਬਹੁਲਕ;
		size_t len;
	} open;

	open.fname = fname;
	open.len = sbi_strlen(fname);
	open.ਬਹੁਲਕ = ਬਹੁਲਕ;

	/* Open the file on the host */
	fd = semihosting_trap(SYSOPEN, &open);
	ਜੇ (fd == -1)
		ਵਾਪਸ semihosting_errno();
	ਵਾਪਸ fd;
}

/**
 * struct semihosting_rdwr_s - Arguments for read and write
 * @fd: A file descriptor returned from semihosting_open()
 * @memp: Pointer to a buffer of memory of at least @len bytes
 * @len: The number of bytes to read or write
 */
ਬਣਤਰ semihosting_rdwr_s {
	ਲੰਮਾ fd;
	ਖਾਲੀ *memp;
	size_t len;
};

ਸਥਿਰ ਲੰਮਾ semihosting_read(ਲੰਮਾ fd, ਖਾਲੀ *memp, size_t len)
{
	ਲੰਮਾ ret;
	ਬਣਤਰ semihosting_rdwr_s ਪਠ;

	ਪਠ.fd = fd;
	ਪਠ.memp = memp;
	ਪਠ.len = len;

	ret = semihosting_trap(SYSREAD, &ਪਠ);
	ਜੇ (ret < 0)
		ਵਾਪਸ semihosting_errno();
	ਵਾਪਸ len - ret;
}

ਸਥਿਰ ਲੰਮਾ semihosting_write(ਲੰਮਾ fd, ਅਚਲ ਖਾਲੀ *memp, size_t len)
{
	ਲੰਮਾ ret;
	ਬਣਤਰ semihosting_rdwr_s ਲਿਖ;

	ਲਿਖ.fd = fd;
	ਲਿਖ.memp = (ਖਾਲੀ *)memp;
	ਲਿਖ.len = len;

	ret = semihosting_trap(SYSWRITE, &ਲਿਖ);
	ਜੇ (ret < 0)
		ਵਾਪਸ semihosting_errno();
	ਵਾਪਸ len - ret;
}

/* clang-format on */

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ semihosting_puts(ਅਚਲ ਅੱਖਰ *str, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ len)
{
	ਅੱਖਰ ch;
	ਲੰਮਾ ret;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i;

	ਜੇ (semihosting_outfd < 0) {
		ਲਈ (i = 0; i < len; i++) {
			ch = str[i];
			semihosting_trap(SYSWRITEC, &ch);
		}
		ret = len;
	} ਨਹੀਂਤਾਂ
		ret = semihosting_write(semihosting_outfd, str, len);

	ਵਾਪਸ (ret < 0) ? 0 : ret;
}

ਸਥਿਰ ਪੂਰਨ semihosting_getc(ਖਾਲੀ)
{
	ਅੱਖਰ ch = 0;
	ਪੂਰਨ ret;

	ਜੇ (semihosting_infd < 0)  {
		ret = semihosting_trap(SYSREADC, ਨਿਰਾ);
		ret = ret < 0 ? -1 : ret;
	} ਨਹੀਂਤਾਂ
		ret = semihosting_read(semihosting_infd, &ch, 1) > 0 ? ch : -1;

	ਵਾਪਸ ret;
}

ਸਥਿਰ ਬਣਤਰ sbi_console_device semihosting_console = {
	.name = "semihosting",
	.console_puts = semihosting_puts,
	.console_getc = semihosting_getc
};

ਪੂਰਨ semihosting_init(ਖਾਲੀ)
{
	semihosting_infd = semihosting_open(":tt", ਬਹੁਲਕ_READ);
	semihosting_outfd = semihosting_open(":tt", ਬਹੁਲਕ_WRITE);

	sbi_console_ਸੈੱਟ_device(&semihosting_console);

	ਵਾਪਸ 0;
}
