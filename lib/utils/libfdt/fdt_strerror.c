// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

بنية fdt_errtabent {
	ثابت حرف *str;
};

#تعریف_کړه FDT_ERRTABENT(val) \
	[(val)] = { .str = #val, }

ساكن بنية fdt_errtabent fdt_errtable[] = {
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
	FDT_ERRTABENT(FDT_ERR_دننی),
	FDT_ERRTABENT(FDT_ERR_BADNCELLS),
	FDT_ERRTABENT(FDT_ERR_BADVALUE),
	FDT_ERRTABENT(FDT_ERR_BADOVERLAY),
	FDT_ERRTABENT(FDT_ERR_NOPHANDLES),
	FDT_ERRTABENT(FDT_ERR_BADFLAGS),
};
#تعریف_کړه FDT_ERRTABSIZE	((صحيح)(حجم(fdt_errtable) / حجم(fdt_errtable[0])))

ثابت حرف *fdt_strerror(صحيح errval)
{
	إذا (errval > 0)
		أرجع "<valid offset/length>";
	وإلا إذا (errval == 0)
		أرجع "<no error>";
	وإلا إذا (-errval < FDT_ERRTABSIZE) {
		ثابت حرف *s = fdt_errtable[-errval].str;

		إذا (s)
			أرجع s;
	}

	أرجع "<unknown error>";
}
