/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2010-2020, The Regents of the University of California
 * (Regents).  All Rights Reserved.
 */

#समावेश <sbi/riscv_locks.h>
#समावेश <sbi/sbi_console.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_system.h>
#समावेश <sbi_utils/sys/htif.h>

#घोषणा HTIF_तथ्य_BITS		48
#घोषणा HTIF_तथ्य_MASK		((1ULL << HTIF_तथ्य_BITS) - 1)
#घोषणा HTIF_तथ्य_SHIFT		0
#घोषणा HTIF_CMD_BITS		8
#घोषणा HTIF_CMD_MASK		((1ULL << HTIF_CMD_BITS) - 1)
#घोषणा HTIF_CMD_SHIFT		48
#घोषणा HTIF_DEV_BITS		8
#घोषणा HTIF_DEV_MASK		((1ULL << HTIF_DEV_BITS) - 1)
#घोषणा HTIF_DEV_SHIFT		56

#घोषणा HTIF_DEV_SYSTEM		0
#घोषणा HTIF_DEV_CONSOLE	1

#घोषणा HTIF_CONSOLE_CMD_GETC	0
#घोषणा HTIF_CONSOLE_CMD_PUTC	1

#अगर __riscv_xlen == 64
# define TOHOST_CMD(dev, cmd, payload) \
	(((uint64_t)(dev) << HTIF_DEV_SHIFT) | \
	 ((uint64_t)(cmd) << HTIF_CMD_SHIFT) | \
	 (uint64_t)(payload))
#अन्यथा
# define TOHOST_CMD(dev, cmd, payload) ({ \
  अगर ((dev) || (cmd)) __builtin_trap(); \
  (payload); })
#पूर्ण_अगर
#घोषणा FROMHOST_DEV(fromhost_मूल्य) \
	((uint64_t)((fromhost_मूल्य) >> HTIF_DEV_SHIFT) & HTIF_DEV_MASK)
#घोषणा FROMHOST_CMD(fromhost_मूल्य) \
	((uint64_t)((fromhost_मूल्य) >> HTIF_CMD_SHIFT) & HTIF_CMD_MASK)
#घोषणा FROMHOST_तथ्य(fromhost_मूल्य) \
	((uint64_t)((fromhost_मूल्य) >> HTIF_तथ्य_SHIFT) & HTIF_तथ्य_MASK)

#घोषणा PK_SYS_write 64

अटल spinlock_t htif_lock = SPIN_LOCK_INITIALIZER;

अस्थिर uint64_t tohost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);
अस्थिर uint64_t fromhost
	__attribute__((section(".htif"))) GUARDED_BY(&htif_lock);

अटल uint64_t *htif_fromhost PT_GUARDED_BY(&htif_lock) = शून्य;
अटल uint64_t *htif_tohost PT_GUARDED_BY(&htif_lock)	 = शून्य;
अटल bool htif_custom = false;

अटल पूर्णांक htif_console_buf GUARDED_BY(&htif_lock);

अटल अंतरभूत uint64_t __read_tohost(व्योम) MUST_HOLD(&htif_lock)
{
	वापस (htif_custom) ? *htif_tohost : tohost;
}

अटल अंतरभूत व्योम __write_tohost(uint64_t val) MUST_HOLD(&htif_lock)
{
	अगर (htif_custom)
		*htif_tohost = val;
	अन्यथा
		tohost = val;
}

अटल अंतरभूत uint64_t __read_fromhost(व्योम) MUST_HOLD(&htif_lock)
{
	वापस (htif_custom) ? *htif_fromhost : fromhost;
}

अटल अंतरभूत व्योम __write_fromhost(uint64_t val) MUST_HOLD(&htif_lock)
{
	अगर (htif_custom)
		*htif_fromhost = val;
	अन्यथा
		fromhost = val;
}

अटल व्योम __check_fromhost() MUST_HOLD(&htif_lock)
{
	uint64_t fh = __read_fromhost();
	अगर (!fh)
		वापस;
	__write_fromhost(0);

	/* this should be from the console */
	अगर (FROMHOST_DEV(fh) != HTIF_DEV_CONSOLE)
		__builtin_trap();
	चयन (FROMHOST_CMD(fh)) {
		हाल HTIF_CONSOLE_CMD_GETC:
			htif_console_buf = 1 + (uint8_t)FROMHOST_तथ्य(fh);
			अवरोध;
		हाल HTIF_CONSOLE_CMD_PUTC:
			अवरोध;
		शेष:
			__builtin_trap();
	}
}

अटल व्योम __set_tohost(uint64_t dev, uint64_t cmd, uint64_t तथ्य)
	MUST_HOLD(&htif_lock)
{
	जबतक (__read_tohost())
		__check_fromhost();
	__write_tohost(TOHOST_CMD(dev, cmd, तथ्य));
}

अटल पूर्णांक set_custom_addr(bool custom_addr,
			   अचिन्हित दीर्घ custom_fromhost_addr,
			   अचिन्हित दीर्घ custom_tohost_addr)
{
	अगर (custom_addr) {
		अगर (htif_custom &&
		    ((custom_fromhost_addr != (अचिन्हित दीर्घ)htif_fromhost) ||
		     (custom_tohost_addr != (अचिन्हित दीर्घ)htif_tohost)))
			वापस SBI_EINVAL;
		htif_fromhost = (uint64_t *)custom_fromhost_addr;
		htif_tohost = (uint64_t *)custom_tohost_addr;
		htif_custom = true;
	}

	वापस 0;
}

#अगर __riscv_xlen == 32
अटल व्योम do_tohost_fromhost(uint64_t dev, uint64_t cmd, uint64_t तथ्य)
{
	spin_lock(&htif_lock);

	__set_tohost(HTIF_DEV_SYSTEM, cmd, तथ्य);

	जबतक (1) {
		uint64_t fh = __read_fromhost();
		अगर (fh) {
			अगर (FROMHOST_DEV(fh) == HTIF_DEV_SYSTEM &&
			    FROMHOST_CMD(fh) == cmd) {
				__write_fromhost(0);
				अवरोध;
			}
			__check_fromhost();
		}
	}

	spin_unlock(&htif_lock);
}

अटल व्योम htif_putc(अक्षर ch)
{
	/* HTIF devices are not supported on RV32, so do a proxy write call */
	अस्थिर uint64_t magic_mem[8];
	magic_mem[0] = PK_SYS_write;
	magic_mem[1] = HTIF_DEV_CONSOLE;
	magic_mem[2] = (uint64_t)(uintptr_t)&ch;
	magic_mem[3] = HTIF_CONSOLE_CMD_PUTC;
	do_tohost_fromhost(HTIF_DEV_SYSTEM, 0, (uint64_t)(uintptr_t)magic_mem);
}
#अन्यथा
अटल व्योम htif_putc(अक्षर ch)
{
	spin_lock(&htif_lock);
	__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	spin_unlock(&htif_lock);
}
#पूर्ण_अगर

अटल पूर्णांक htif_getc(व्योम)
{
	पूर्णांक ch;

#अगर __riscv_xlen == 32
	/* HTIF devices are not supported on RV32 */
	वापस -1;
#पूर्ण_अगर

	spin_lock(&htif_lock);

	__check_fromhost();
	ch = htif_console_buf;
	अगर (ch >= 0) {
		htif_console_buf = -1;
		__set_tohost(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	}

	spin_unlock(&htif_lock);

	वापस ch - 1;
}

अटल काष्ठा sbi_console_device htif_console = {
	.name = "htif",
	.console_putc = htif_putc,
	.console_getc = htif_getc
};

पूर्णांक htif_serial_init(bool custom_addr,
		     अचिन्हित दीर्घ custom_fromhost_addr,
		     अचिन्हित दीर्घ custom_tohost_addr)
{
	पूर्णांक rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	अगर (rc)
		वापस rc;

	sbi_console_set_device(&htif_console);
	वापस 0;
}

अटल पूर्णांक htif_system_reset_check(u32 type, u32 reason)
{
	वापस 1;
}

अटल व्योम htif_system_reset(u32 type, u32 reason) NO_THREAD_SAFETY_ANALYSIS
{
	जबतक (1) {
		__write_fromhost(0);
		__write_tohost(1);
	}
}

अटल काष्ठा sbi_system_reset_device htif_reset = {
	.name = "htif",
	.system_reset_check = htif_system_reset_check,
	.system_reset = htif_system_reset
};

पूर्णांक htif_system_reset_init(bool custom_addr,
			   अचिन्हित दीर्घ custom_fromhost_addr,
			   अचिन्हित दीर्घ custom_tohost_addr)
{
	पूर्णांक rc;

	rc = set_custom_addr(custom_addr, custom_fromhost_addr,
			     custom_tohost_addr);
	अगर (rc)
		वापस rc;

	sbi_system_reset_add_device(&htif_reset);
	वापस 0;
}
