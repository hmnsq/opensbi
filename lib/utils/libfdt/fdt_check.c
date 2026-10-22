// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_env.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <fdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_internal.h"

ਪੂਰਨ fdt_check_ਭਰਿਆ(ਅਚਲ ਖਾਲੀ *fdt, size_t bufsize)
{
	ਪੂਰਨ err;
	ਪੂਰਨ ਅੰਕ_memrsv;
	ਪੂਰਨ ਵਿੱਥ, nextoffset = 0;
	uint32_t ਟੈਗ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਡੂੰਘਾਈ = 0;
	ਅਚਲ ਖਾਲੀ *prop;
	ਅਚਲ ਅੱਖਰ *propname;
	bool expect_ਸਿਰਾ = false;

	ਜੇ (bufsize < FDT_V1_ਮਾਪ)
		ਵਾਪਸ -FDT_ERR_TRUNCATED;
	ਜੇ (bufsize < fdt_ਸਿਰਲੇਖ_ਮਾਪ_2(fdt))
		ਵਾਪਸ -FDT_ERR_TRUNCATED;
	err = fdt_check_ਸਿਰਲੇਖ(fdt);
	ਜੇ (err != 0)
		ਵਾਪਸ err;
	ਜੇ (bufsize < fdt_totalsize(fdt))
		ਵਾਪਸ -FDT_ERR_TRUNCATED;

	ਅੰਕ_memrsv = fdt_ਅੰਕ_mem_rsv(fdt);
	ਜੇ (ਅੰਕ_memrsv < 0)
		ਵਾਪਸ ਅੰਕ_memrsv;

	ਜਦੋਂ (1) {
		ਵਿੱਥ = nextoffset;
		ਟੈਗ = fdt_ਅਗਲਾ_ਟੈਗ(fdt, ਵਿੱਥ, &nextoffset);

		ਜੇ (nextoffset < 0)
			ਵਾਪਸ nextoffset;

		/* If we see two root nodes, something is wrong */
		ਜੇ (expect_ਸਿਰਾ && ਟੈਗ != FDT_ਸਿਰਾ)
			ਵਾਪਸ -FDT_ERR_BADSTRUCTURE;

		ਚੋਣ (ਟੈਗ) {
		ਹਾਲਤ FDT_NOP:
			ਰੋਕੋ;

		ਹਾਲਤ FDT_ਸਿਰਾ:
			ਜੇ (ਡੂੰਘਾਈ != 0)
				ਵਾਪਸ -FDT_ERR_BADSTRUCTURE;
			ਵਾਪਸ 0;

		ਹਾਲਤ FDT_BEGIN_ਗੰਢ:
			ਡੂੰਘਾਈ++;
			ਜੇ (ਡੂੰਘਾਈ > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
				ਵਾਪਸ -FDT_ERR_BADSTRUCTURE;

			/* The root node must have an empty name */
			ਜੇ (ਡੂੰਘਾਈ == 1) {
				ਅਚਲ ਅੱਖਰ *name;
				ਪੂਰਨ len;

				name = fdt_get_name(fdt, ਵਿੱਥ, &len);
				ਜੇ (*name || len)
					ਵਾਪਸ -FDT_ERR_BADSTRUCTURE;
			}
			ਰੋਕੋ;

		ਹਾਲਤ FDT_ਸਿਰਾ_ਗੰਢ:
			ਜੇ (ਡੂੰਘਾਈ == 0)
				ਵਾਪਸ -FDT_ERR_BADSTRUCTURE;
			ਡੂੰਘਾਈ--;
			ਜੇ (ਡੂੰਘਾਈ == 0)
				expect_ਸਿਰਾ = true;
			ਰੋਕੋ;

		ਹਾਲਤ FDT_PROP:
			prop = fdt_getprop_by_ਵਿੱਥ(fdt, ਵਿੱਥ, &propname,
						     &err);
			ਜੇ (!prop)
				ਵਾਪਸ err;
			ਰੋਕੋ;

		ਮੂਲ:
			ਵਾਪਸ -FDT_ERR_ਅੰਦਰੂਨੀ;
		}
	}
}
