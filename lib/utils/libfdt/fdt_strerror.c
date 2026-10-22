// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ಸಂರಚನಾ fdt_errtabent {
	ಧ್ರುವ ವರ್ಣ *str;
};

#define FDT_ERRTABENT(val) \
	[(val)] = { .str = #val, }

ಸ್ಥಿರ ಸಂರಚನಾ fdt_errtabent fdt_errtable[] = {
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
#define FDT_ERRTABSIZE	((ಪೂರ್ಣಾಙ್ಕ)(ಪರಿಮಾಣ(fdt_errtable) / ಪರಿಮಾಣ(fdt_errtable[0])))

ಧ್ರುವ ವರ್ಣ *fdt_strerror(ಪೂರ್ಣಾಙ್ಕ errval)
{
	ಯದಿ (errval > 0)
		ಪ್ರತ್ಯಾಗಮ "<valid offset/length>";
	ಅನ್ಯಥಾ ಯದಿ (errval == 0)
		ಪ್ರತ್ಯಾಗಮ "<no error>";
	ಅನ್ಯಥಾ ಯದಿ (-errval < FDT_ERRTABSIZE) {
		ಧ್ರುವ ವರ್ಣ *s = fdt_errtable[-errval].str;

		ಯದಿ (s)
			ಪ್ರತ್ಯಾಗಮ s;
	}

	ಪ್ರತ್ಯಾಗಮ "<unknown error>";
}
