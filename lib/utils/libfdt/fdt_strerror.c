// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_env.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <fdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_internal.h"

ਬਣਤਰ fdt_errtabent {
	ਅਚਲ ਅੱਖਰ *str;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_ERRTABENT(val) \
	[(val)] = { .str = #val, }

ਸਥਿਰ ਬਣਤਰ fdt_errtabent fdt_errtable[] = {
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
	FDT_ERRTABENT(FDT_ERR_ਅੰਦਰੂਨੀ),
	FDT_ERRTABENT(FDT_ERR_BADNCELLS),
	FDT_ERRTABENT(FDT_ERR_BADVALUE),
	FDT_ERRTABENT(FDT_ERR_BADOVERLAY),
	FDT_ERRTABENT(FDT_ERR_NOPHANDLES),
	FDT_ERRTABENT(FDT_ERR_BADFLAGS),
};
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_ERRTABSIZE	((ਪੂਰਨ)(ਆਕਾਰ(fdt_errtable) / ਆਕਾਰ(fdt_errtable[0])))

ਅਚਲ ਅੱਖਰ *fdt_strerror(ਪੂਰਨ errval)
{
	ਜੇ (errval > 0)
		ਵਾਪਸ "<valid offset/length>";
	ਨਹੀਂਤਾਂ ਜੇ (errval == 0)
		ਵਾਪਸ "<no error>";
	ਨਹੀਂਤਾਂ ਜੇ (-errval < FDT_ERRTABSIZE) {
		ਅਚਲ ਅੱਖਰ *s = fdt_errtable[-errval].str;

		ਜੇ (s)
			ਵਾਪਸ s;
	}

	ਵਾਪਸ "<unknown error>";
}
