// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#അന്തര്ഭാവ "libfdt_env.h"

#അന്തര്ഭാവ <fdt.h>
#അന്തര്ഭാവ <libfdt.h>

#അന്തര്ഭാവ "libfdt_internal.h"

ഘടന fdt_errtabent {
	സ്ഥിരം അക്ഷരം *str;
};

#define FDT_ERRTABENT(val) \
	[(val)] = { .str = #val, }

സ്ഥിതം ഘടന fdt_errtabent fdt_errtable[] = {
	FDT_ERRTABENT(FDT_ERR_NOTFOUND),
	FDT_ERRTABENT(FDT_ERR_EXISTS),
	FDT_ERRTABENT(FDT_ERR_NOSPACE),

	FDT_ERRTABENT(FDT_ERR_BADOFFSET),
	FDT_ERRTABENT(FDT_ERR_BADPATH),
	FDT_ERRTABENT(FDT_ERR_BADPHANDLE),
	FDT_ERRTABENT(FDT_ERR_BADSTATE),

	FDT_ERRTABENT(FDT_ERR_TRUNCATED),
	FDT_ERRTABENT(FDT_ERR_BADMAGIC),
	FDT_ERRTABENT(FDT_ERR_BADVERSION),
	FDT_ERRTABENT(FDT_ERR_BADSTRUCTURE),
	FDT_ERRTABENT(FDT_ERR_BADLAYOUT),
	FDT_ERRTABENT(FDT_ERR_INTERNAL),
	FDT_ERRTABENT(FDT_ERR_BADNCELLS),
	FDT_ERRTABENT(FDT_ERR_BADVALUE),
	FDT_ERRTABENT(FDT_ERR_BADOVERLAY),
	FDT_ERRTABENT(FDT_ERR_NOPHANDLES),
	FDT_ERRTABENT(FDT_ERR_BADFLAGS),
};
#define FDT_ERRTABSIZE	((പൂർണ്ണസംഖ്യ)(വലുപ്പം(fdt_errtable) / വലുപ്പം(fdt_errtable[0])))

സ്ഥിരം അക്ഷരം *fdt_strerror(പൂർണ്ണസംഖ്യ errval)
{
	എങ്കിൽ (errval > 0)
		തിരികെ "<valid offset/length>";
	അല്ലെങ്കിൽ എങ്കിൽ (errval == 0)
		തിരികെ "<no error>";
	അല്ലെങ്കിൽ എങ്കിൽ (-errval < FDT_ERRTABSIZE) {
		സ്ഥിരം അക്ഷരം *s = fdt_errtable[-errval].str;

		എങ്കിൽ (s)
			തിരികെ s;
	}

	തിരികെ "<unknown error>";
}
