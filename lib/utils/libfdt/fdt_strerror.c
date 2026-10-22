// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#அந்தர்பாவ "libfdt_env.h"

#அந்தர்பாவ <fdt.h>
#அந்தர்பாவ <libfdt.h>

#அந்தர்பாவ "libfdt_internal.h"

கட்டமைப்பு fdt_errtabent {
	மாறிலி எழுத்து *str;
};

#define FDT_ERRTABENT(val) \
	[(val)] = { .str = #val, }

நிலையான கட்டமைப்பு fdt_errtabent fdt_errtable[] = {
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
#define FDT_ERRTABSIZE	((முழு)(அளவு(fdt_errtable) / அளவு(fdt_errtable[0])))

மாறிலி எழுத்து *fdt_strerror(முழு errval)
{
	எனில் (errval > 0)
		திரும்பு "<valid offset/length>";
	இல்லையெனில் எனில் (errval == 0)
		திரும்பு "<no error>";
	இல்லையெனில் எனில் (-errval < FDT_ERRTABSIZE) {
		மாறிலி எழுத்து *s = fdt_errtable[-errval].str;

		எனில் (s)
			திரும்பு s;
	}

	திரும்பு "<unknown error>";
}
