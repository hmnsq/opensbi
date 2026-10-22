// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#અન્તર્ભાવ "libfdt_env.h"

#અન્તર્ભાવ <fdt.h>
#અન્તર્ભાવ <libfdt.h>

#અન્તર્ભાવ "libfdt_internal.h"

સંરચના fdt_errtabent {
	અચલ અક્ષર *str;
};

#define FDT_ERRTABENT(val) \
	[(val)] = { .str = #val, }

સ્થિર સંરચના fdt_errtabent fdt_errtable[] = {
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
#define FDT_ERRTABSIZE	((પૂર્ણાંક)(કદ(fdt_errtable) / કદ(fdt_errtable[0])))

અચલ અક્ષર *fdt_strerror(પૂર્ણાંક errval)
{
	જો (errval > 0)
		પાછા "<valid offset/length>";
	નહીંતર જો (errval == 0)
		પાછા "<no error>";
	નહીંતર જો (-errval < FDT_ERRTABSIZE) {
		અચલ અક્ષર *s = fdt_errtable[-errval].str;

		જો (s)
			પાછા s;
	}

	પાછા "<unknown error>";
}
