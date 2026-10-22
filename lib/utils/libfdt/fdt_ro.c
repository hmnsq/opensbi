// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_nodename_eq_(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ,
			    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ len)
{
	ಪೂರ್ಣಾಂಕ olen;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = fdt_get_name(fdt, ಸ್ಥಾನಾಂತರ, &olen);

	ಆದರೆ (!p || olen < len)
		/* short match */
		ಹಿಂತಿರುಗಿ 0;

	ಆದರೆ (memcmp(p, s, len) != 0)
		ಹಿಂತಿರುಗಿ 0;

	ಆದರೆ (p[len] == '\0')
		ಹಿಂತಿರುಗಿ 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!memchr(s, '@', len) && (p[len] == '@'))
		ಹಿಂತಿರುಗಿ 1;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fdt_get_string(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ stroffset, ಪೂರ್ಣಾಂಕ *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	ಪೂರ್ಣಾಂಕ err;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, *n;

	ಆದರೆ (can_assume(VALID_ಒಳಹರಿವು)) {
		s = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)fdt + fdt_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt) + stroffset;

		ಆದರೆ (lenp)
			*lenp = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);
		ಹಿಂತಿರುಗಿ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ಆದರೆ (totalsize < 0)
		ಹೋಗು fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt);
	ಆದರೆ (absoffset >= (ಚಿಹ್ನೆರಹಿತ)totalsize)
		ಹೋಗು fail;
	len = totalsize - absoffset;

	ಆದರೆ (fdt_magic(fdt) == FDT_MAGIC) {
		ಆದರೆ (stroffset < 0)
			ಹೋಗು fail;
		ಆದರೆ (can_assume(LATEST) || fdt_ಆವೃತ್ತಿ(fdt) >= 17) {
			ಆದರೆ ((ಚಿಹ್ನೆರಹಿತ)stroffset >= fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt))
				ಹೋಗು fail;
			ಆದರೆ ((fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt) - stroffset) < len)
				len = fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt) - stroffset;
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ sw_stroffset = -stroffset;

		ಆದರೆ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_ಗಾತ್ರ_dt_ಅಕ್ಷರಸರಣಿಗಳು(fdt)))
			ಹೋಗು fail;
		ಆದರೆ (sw_stroffset < len)
			len = sw_stroffset;
	} ಇಲ್ಲದಿದ್ದರೆ {
		err = -FDT_ERR_ಆಂತರಿಕ;
		ಹೋಗು fail;
	}

	s = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ಆದರೆ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		ಹೋಗು fail;
	}

	ಆದರೆ (lenp)
		*lenp = n - s;
	ಹಿಂತಿರುಗಿ s;

fail:
	ಆದರೆ (lenp)
		*lenp = err;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fdt_string(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ stroffset)
{
	ಹಿಂತಿರುಗಿ fdt_get_string(fdt, stroffset, ಖಾಲಿ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fdt_string_eq_(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ stroffset,
			  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಪೂರ್ಣಾಂಕ len)
{
	ಪೂರ್ಣಾಂಕ slen;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = fdt_get_string(fdt, stroffset, &slen);

	ಹಿಂತಿರುಗಿ p && (slen == len) && (memcmp(p, s, len) == 0);
}

ಪೂರ್ಣಾಂಕ fdt_ಕಂಡುಹಿಡಿ_max_phandle(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = -1;

	ಆಗ (true) {
		uint32_t ಮೌಲ್ಯ;

		ಸ್ಥಾನಾಂತರ = fdt_ಮುಂದಿನ_ಗಂಟು(fdt, ಸ್ಥಾನಾಂತರ, ಖಾಲಿ);
		ಆದರೆ (ಸ್ಥಾನಾಂತರ < 0) {
			ಆದರೆ (ಸ್ಥಾನಾಂತರ == -FDT_ERR_NOTFOUND)
				ನಿಲ್ಲಿಸು;

			ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
		}

		ಮೌಲ್ಯ = fdt_get_phandle(fdt, ಸ್ಥಾನಾಂತರ);

		ಆದರೆ (ಮೌಲ್ಯ > max)
			max = ಮೌಲ್ಯ;
	}

	ಆದರೆ (phandle)
		*phandle = max;

	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_generate_phandle(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, uint32_t *phandle)
{
	uint32_t max;
	ಪೂರ್ಣಾಂಕ err;

	err = fdt_ಕಂಡುಹಿಡಿ_max_phandle(fdt, &max);
	ಆದರೆ (err < 0)
		ಹಿಂತಿರುಗಿ err;

	ಆದರೆ (max == FDT_MAX_PHANDLE)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOPHANDLES;

	ಆದರೆ (phandle)
		*phandle = max + 1;

	ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_reserve_ನಮೂದು *fdt_mem_rsv(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ n)
{
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = n * ಗಾತ್ರ(ರಚನೆ fdt_reserve_ನಮೂದು);
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ absoffset = fdt_off_mem_rsvmap(fdt) + ಸ್ಥಾನಾಂತರ;

	ಆದರೆ (!can_assume(VALID_ಒಳಹರಿವು)) {
		ಆದರೆ (absoffset < fdt_off_mem_rsvmap(fdt))
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		ಆದರೆ (absoffset > fdt_totalsize(fdt) -
		    ಗಾತ್ರ(ರಚನೆ fdt_reserve_ನಮೂದು))
			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಹಿಂತಿರುಗಿ fdt_mem_rsv_(fdt, n);
}

ಪೂರ್ಣಾಂಕ fdt_get_mem_rsv(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ n, uint64_t *ವಿಳಾಸ, uint64_t *ಗಾತ್ರ_2)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_reserve_ನಮೂದು *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ಆದರೆ (!can_assume(VALID_ಒಳಹರಿವು) && !re)
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADOFFSET;

	*ವಿಳಾಸ = fdt64_ld_(&re->ವಿಳಾಸ);
	*ಗಾತ್ರ_2 = fdt64_ld_(&re->ಗಾತ್ರ_2);
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_ಅಂಕೆ_mem_rsv(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಂಕ i;
	ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_reserve_ನಮೂದು *re;

	ಗಾಗಿ (i = 0; (re = fdt_mem_rsv(fdt, i)) != ಖಾಲಿ; i++) {
		ಆದರೆ (fdt64_ld_(&re->ಗಾತ್ರ_2) == 0)
			ಹಿಂತಿರುಗಿ i;
	}
	ಹಿಂತಿರುಗಿ -FDT_ERR_TRUNCATED;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ nextprop_(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
	uint32_t ಟ್ಯಾಗ್;
	ಪೂರ್ಣಾಂಕ nextoffset;

	ಮಾಡು {
		ಟ್ಯಾಗ್ = fdt_ಮುಂದಿನ_ಟ್ಯಾಗ್(fdt, ಸ್ಥಾನಾಂತರ, &nextoffset);

		ಆಯ್ಕೆ (ಟ್ಯಾಗ್) {
		ಸ್ಥಿತಿ FDT_ಕೊನೆ:
			ಆದರೆ (nextoffset >= 0)
				ಹಿಂತಿರುಗಿ -FDT_ERR_BADSTRUCTURE;
			ಇಲ್ಲದಿದ್ದರೆ
				ಹಿಂತಿರುಗಿ nextoffset;

		ಸ್ಥಿತಿ FDT_PROP:
			ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
		}
		ಸ್ಥಾನಾಂತರ = nextoffset;
	} ಆಗ (ಟ್ಯಾಗ್ == FDT_NOP);

	ಹಿಂತಿರುಗಿ -FDT_ERR_NOTFOUND;
}

ಪೂರ್ಣಾಂಕ fdt_subnode_ಸ್ಥಾನಾಂತರ_namelen(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ,
			       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ namelen)
{
	ಪೂರ್ಣಾಂಕ ಆಳ;

	FDT_RO_PROBE(fdt);

	ಗಾಗಿ (ಆಳ = 0;
	     (ಸ್ಥಾನಾಂತರ >= 0) && (ಆಳ >= 0);
	     ಸ್ಥಾನಾಂತರ = fdt_ಮುಂದಿನ_ಗಂಟು(fdt, ಸ್ಥಾನಾಂತರ, &ಆಳ))
		ಆದರೆ ((ಆಳ == 1)
		    && fdt_nodename_eq_(fdt, ಸ್ಥಾನಾಂತರ, name, namelen))
			ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;

	ಆದರೆ (ಆಳ < 0)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOTFOUND;
	ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ; /* error */
}

ಪೂರ್ಣಾಂಕ fdt_subnode_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ parentoffset,
		       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	ಹಿಂತಿರುಗಿ fdt_subnode_ಸ್ಥಾನಾಂತರ_namelen(fdt, parentoffset, name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name));
}

ಪೂರ್ಣಾಂಕ fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ_namelen(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ namelen)
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆ = ಮಾರ್ಗ + namelen;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಮಾರ್ಗ;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ಆದರೆ (*ಮಾರ್ಗ != '/') {
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *q = memchr(ಮಾರ್ಗ, '/', ಕೊನೆ - p);

		ಆದರೆ (!q)
			q = ಕೊನೆ;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ಆದರೆ (!p)
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADPATH;
		ಸ್ಥಾನಾಂತರ = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, p);

		p = q;
	}

	ಆಗ (p < ಕೊನೆ) {
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *q;

		ಆಗ (*p == '/') {
			p++;
			ಆದರೆ (p == ಕೊನೆ)
				ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
		}
		q = memchr(p, '/', ಕೊನೆ - p);
		ಆದರೆ (! q)
			q = ಕೊನೆ;

		ಸ್ಥಾನಾಂತರ = fdt_subnode_ಸ್ಥಾನಾಂತರ_namelen(fdt, ಸ್ಥಾನಾಂತರ, p, q-p);
		ಆದರೆ (ಸ್ಥಾನಾಂತರ < 0)
			ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;

		p = q;
	}

	ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
}

ಪೂರ್ಣಾಂಕ fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ)
{
	ಹಿಂತಿರುಗಿ fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ_namelen(fdt, ಮಾರ್ಗ, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮಾರ್ಗ));
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fdt_get_name(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಪೂರ್ಣಾಂಕ *len)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_ಗಂಟು_ಶೀರ್ಷಿಕೆ *nh = fdt_ಸ್ಥಾನಾಂತರ_ptr_2(fdt, nodeoffset);
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nameptr;
	ಪೂರ್ಣಾಂಕ err;

	ಆದರೆ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_ಗಂಟು_ಸ್ಥಾನಾಂತರ(fdt, nodeoffset)) < 0))
			ಹೋಗು fail;

	nameptr = nh->name;

	ಆದರೆ (!can_assume(LATEST) && fdt_ಆವೃತ್ತಿ(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಎಲೆ;
		ಎಲೆ = strrchr(nameptr, '/');
		ಆದರೆ (ಎಲೆ == ಖಾಲಿ) {
			err = -FDT_ERR_BADSTRUCTURE;
			ಹೋಗು fail;
		}
		nameptr = ಎಲೆ+1;
	}

	ಆದರೆ (len)
		*len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(nameptr);

	ಹಿಂತಿರುಗಿ nameptr;

 fail:
	ಆದರೆ (len)
		*len = err;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ fdt_ಮೊದಲ_property_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset)
{
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;

	ಆದರೆ ((ಸ್ಥಾನಾಂತರ = fdt_check_ಗಂಟು_ಸ್ಥಾನಾಂತರ(fdt, nodeoffset)) < 0)
		ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;

	ಹಿಂತಿರುಗಿ nextprop_(fdt, ಸ್ಥಾನಾಂತರ);
}

ಪೂರ್ಣಾಂಕ fdt_ಮುಂದಿನ_property_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
	ಆದರೆ ((ಸ್ಥಾನಾಂತರ = fdt_check_prop_ಸ್ಥಾನಾಂತರ(fdt, ಸ್ಥಾನಾಂತರ)) < 0)
		ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;

	ಹಿಂತಿರುಗಿ nextprop_(fdt, ಸ್ಥಾನಾಂತರ);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_property *fdt_get_property_by_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt,
						              ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ,
						              ಪೂರ್ಣಾಂಕ *lenp)
{
	ಪೂರ್ಣಾಂಕ err;
	ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_property *prop;

	ಆದರೆ (!can_assume(VALID_ಒಳಹರಿವು) &&
	    (err = fdt_check_prop_ಸ್ಥಾನಾಂತರ(fdt, ಸ್ಥಾನಾಂತರ)) < 0) {
		ಆದರೆ (lenp)
			*lenp = err;
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	prop = fdt_ಸ್ಥಾನಾಂತರ_ptr_2(fdt, ಸ್ಥಾನಾಂತರ);

	ಆದರೆ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ಹಿಂತಿರುಗಿ prop;
}

ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_property *fdt_get_property_by_ಸ್ಥಾನಾಂತರ_2(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt,
						      ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ,
						      ಪೂರ್ಣಾಂಕ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ಆದರೆ (!can_assume(LATEST) && fdt_ಆವೃತ್ತಿ(fdt) < 0x10) {
		ಆದರೆ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಹಿಂತಿರುಗಿ fdt_get_property_by_ಸ್ಥಾನಾಂತರ(fdt, ಸ್ಥಾನಾಂತರ, lenp);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_property *fdt_get_property_namelen_(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt,
						            ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ,
						            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
						            ಪೂರ್ಣಾಂಕ namelen,
							    ಪೂರ್ಣಾಂಕ *lenp,
							    ಪೂರ್ಣಾಂಕ *poffset)
{
	ಗಾಗಿ (ಸ್ಥಾನಾಂತರ = fdt_ಮೊದಲ_property_ಸ್ಥಾನಾಂತರ(fdt, ಸ್ಥಾನಾಂತರ);
	     (ಸ್ಥಾನಾಂತರ >= 0);
	     (ಸ್ಥಾನಾಂತರ = fdt_ಮುಂದಿನ_property_ಸ್ಥಾನಾಂತರ(fdt, ಸ್ಥಾನಾಂತರ))) {
		ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_property *prop;

		prop = fdt_get_property_by_ಸ್ಥಾನಾಂತರ(fdt, ಸ್ಥಾನಾಂತರ, lenp);
		ಆದರೆ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			ಸ್ಥಾನಾಂತರ = -FDT_ERR_ಆಂತರಿಕ;
			ನಿಲ್ಲಿಸು;
		}
		ಆದರೆ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ಆದರೆ (poffset)
				*poffset = ಸ್ಥಾನಾಂತರ;
			ಹಿಂತಿರುಗಿ prop;
		}
	}

	ಆದರೆ (lenp)
		*lenp = ಸ್ಥಾನಾಂತರ;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}


ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_property *fdt_get_property_namelen(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt,
						    ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ,
						    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
						    ಪೂರ್ಣಾಂಕ namelen, ಪೂರ್ಣಾಂಕ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ಆದರೆ (!can_assume(LATEST) && fdt_ಆವೃತ್ತಿ(fdt) < 0x10) {
		ಆದರೆ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಹಿಂತಿರುಗಿ fdt_get_property_namelen_(fdt, ಸ್ಥಾನಾಂತರ, name, namelen, lenp,
					 ಖಾಲಿ);
}


ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_property *fdt_get_property(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt,
					    ಪೂರ್ಣಾಂಕ nodeoffset,
					    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ *lenp)
{
	ಹಿಂತಿರುಗಿ fdt_get_property_namelen(fdt, nodeoffset, name,
					ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name), lenp);
}

ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt_getprop_namelen(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
				ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ namelen, ಪೂರ್ಣಾಂಕ *lenp)
{
	ಪೂರ್ಣಾಂಕ poffset;
	ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ಆದರೆ (!prop)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	/* Handle realignment */
	ಆದರೆ (!can_assume(LATEST) && fdt_ಆವೃತ್ತಿ(fdt) < 0x10 &&
	    (poffset + ಗಾತ್ರ(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ಹಿಂತಿರುಗಿ prop->ದತ್ತಾಂಶ + 4;
	ಹಿಂತಿರುಗಿ prop->ದತ್ತಾಂಶ;
}

ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt_getprop_by_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ,
				  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **namep, ಪೂರ್ಣಾಂಕ *lenp)
{
	ಸ್ಥಿರಾಂಕ ರಚನೆ fdt_property *prop;

	prop = fdt_get_property_by_ಸ್ಥಾನಾಂತರ(fdt, ಸ್ಥಾನಾಂತರ, lenp);
	ಆದರೆ (!prop)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (namep) {
		ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
		ಪೂರ್ಣಾಂಕ namelen;

		ಆದರೆ (!can_assume(VALID_ಒಳಹರಿವು)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ಆದರೆ (!name) {
				ಆದರೆ (lenp)
					*lenp = namelen;
				ಹಿಂತಿರುಗಿ ಖಾಲಿ;
			}
			*namep = name;
		} ಇಲ್ಲದಿದ್ದರೆ {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ಆದರೆ (!can_assume(LATEST) && fdt_ಆವೃತ್ತಿ(fdt) < 0x10 &&
	    (ಸ್ಥಾನಾಂತರ + ಗಾತ್ರ(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ಹಿಂತಿರುಗಿ prop->ದತ್ತಾಂಶ + 4;
	ಹಿಂತಿರುಗಿ prop->ದತ್ತಾಂಶ;
}

ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt_getprop(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
			ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ *lenp)
{
	ಹಿಂತಿರುಗಿ fdt_getprop_namelen(fdt, nodeoffset, name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name), lenp);
}

uint32_t fdt_get_phandle(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset)
{
	ಸ್ಥಿರಾಂಕ fdt32_t *php;
	ಪೂರ್ಣಾಂಕ len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ಆದರೆ (!php || (len != ಗಾತ್ರ(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ಆದರೆ (!php || (len != ಗಾತ್ರ(*php)))
			ಹಿಂತಿರುಗಿ 0;
	}

	ಹಿಂತಿರುಗಿ fdt32_ld_(php);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fdt_get_alias_namelen(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt,
				  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ namelen)
{
	ಪೂರ್ಣಾಂಕ aliasoffset;

	aliasoffset = fdt_ಮಾರ್ಗ_ಸ್ಥಾನಾಂತರ(fdt, "/aliases");
	ಆದರೆ (aliasoffset < 0)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ಹಿಂತಿರುಗಿ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, ಖಾಲಿ);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fdt_get_alias(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
	ಹಿಂತಿರುಗಿ fdt_get_alias_namelen(fdt, name, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name));
}

ಪೂರ್ಣಾಂಕ fdt_get_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಅಕ್ಷರ *buf, ಪೂರ್ಣಾಂಕ buflen)
{
	ಪೂರ್ಣಾಂಕ pdepth = 0, p = 0;
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಆಳ, namelen;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;

	FDT_RO_PROBE(fdt);

	ಆದರೆ (buflen < 2)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;

	ಗಾಗಿ (ಸ್ಥಾನಾಂತರ = 0, ಆಳ = 0;
	     (ಸ್ಥಾನಾಂತರ >= 0) && (ಸ್ಥಾನಾಂತರ <= nodeoffset);
	     ಸ್ಥಾನಾಂತರ = fdt_ಮುಂದಿನ_ಗಂಟು(fdt, ಸ್ಥಾನಾಂತರ, &ಆಳ)) {
		ಆಗ (pdepth > ಆಳ) {
			ಮಾಡು {
				p--;
			} ಆಗ (buf[p-1] != '/');
			pdepth--;
		}

		ಆದರೆ (pdepth >= ಆಳ) {
			name = fdt_get_name(fdt, ಸ್ಥಾನಾಂತರ, &namelen);
			ಆದರೆ (!name)
				ಹಿಂತಿರುಗಿ namelen;
			ಆದರೆ ((p + namelen + 1) <= buflen) {
				ಸ್ಮರಣೆ_ನಕಲು(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ಆದರೆ (ಸ್ಥಾನಾಂತರ == nodeoffset) {
			ಆದರೆ (pdepth < (ಆಳ + 1))
				ಹಿಂತಿರುಗಿ -FDT_ERR_NOSPACE;

			ಆದರೆ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ಹಿಂತಿರುಗಿ 0;
		}
	}

	ಆದರೆ ((ಸ್ಥಾನಾಂತರ == -FDT_ERR_NOTFOUND) || (ಸ್ಥಾನಾಂತರ >= 0))
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADOFFSET;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸ್ಥಾನಾಂತರ == -FDT_ERR_BADOFFSET)
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADSTRUCTURE;

	ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ; /* error from fdt_next_node() */
}

ಪೂರ್ಣಾಂಕ fdt_supernode_atdepth_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
				 ಪೂರ್ಣಾಂಕ supernodedepth, ಪೂರ್ಣಾಂಕ *nodedepth)
{
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಆಳ;
	ಪೂರ್ಣಾಂಕ supernodeoffset = -FDT_ERR_ಆಂತರಿಕ;

	FDT_RO_PROBE(fdt);

	ಆದರೆ (supernodedepth < 0)
		ಹಿಂತಿರುಗಿ -FDT_ERR_NOTFOUND;

	ಗಾಗಿ (ಸ್ಥಾನಾಂತರ = 0, ಆಳ = 0;
	     (ಸ್ಥಾನಾಂತರ >= 0) && (ಸ್ಥಾನಾಂತರ <= nodeoffset);
	     ಸ್ಥಾನಾಂತರ = fdt_ಮುಂದಿನ_ಗಂಟು(fdt, ಸ್ಥಾನಾಂತರ, &ಆಳ)) {
		ಆದರೆ (ಆಳ == supernodedepth)
			supernodeoffset = ಸ್ಥಾನಾಂತರ;

		ಆದರೆ (ಸ್ಥಾನಾಂತರ == nodeoffset) {
			ಆದರೆ (nodedepth)
				*nodedepth = ಆಳ;

			ಆದರೆ (supernodedepth > ಆಳ)
				ಹಿಂತಿರುಗಿ -FDT_ERR_NOTFOUND;
			ಇಲ್ಲದಿದ್ದರೆ
				ಹಿಂತಿರುಗಿ supernodeoffset;
		}
	}

	ಆದರೆ (!can_assume(VALID_ಒಳಹರಿವು)) {
		ಆದರೆ ((ಸ್ಥಾನಾಂತರ == -FDT_ERR_NOTFOUND) || (ಸ್ಥಾನಾಂತರ >= 0))
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADOFFSET;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸ್ಥಾನಾಂತರ == -FDT_ERR_BADOFFSET)
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADSTRUCTURE;
	}

	ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ; /* error from fdt_next_node() */
}

ಪೂರ್ಣಾಂಕ fdt_ಗಂಟು_ಆಳ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset)
{
	ಪೂರ್ಣಾಂಕ nodedepth;
	ಪೂರ್ಣಾಂಕ err;

	err = fdt_supernode_atdepth_ಸ್ಥಾನಾಂತರ(fdt, nodeoffset, 0, &nodedepth);
	ಆದರೆ (err)
		ಹಿಂತಿರುಗಿ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_ಆಂತರಿಕ;
	ಹಿಂತಿರುಗಿ nodedepth;
}

ಪೂರ್ಣಾಂಕ fdt_ಪೋಷಕ_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset)
{
	ಪೂರ್ಣಾಂಕ nodedepth = fdt_ಗಂಟು_ಆಳ(fdt, nodeoffset);

	ಆದರೆ (nodedepth < 0)
		ಹಿಂತಿರುಗಿ nodedepth;
	ಹಿಂತಿರುಗಿ fdt_supernode_atdepth_ಸ್ಥಾನಾಂತರ(fdt, nodeoffset,
					    nodedepth - 1, ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_prop_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ startoffset,
				  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *propname,
				  ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *propval, ಪೂರ್ಣಾಂಕ proplen)
{
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;
	ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *val;
	ಪೂರ್ಣಾಂಕ len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	ಗಾಗಿ (ಸ್ಥಾನಾಂತರ = fdt_ಮುಂದಿನ_ಗಂಟು(fdt, startoffset, ಖಾಲಿ);
	     ಸ್ಥಾನಾಂತರ >= 0;
	     ಸ್ಥಾನಾಂತರ = fdt_ಮುಂದಿನ_ಗಂಟು(fdt, ಸ್ಥಾನಾಂತರ, ಖಾಲಿ)) {
		val = fdt_getprop(fdt, ಸ್ಥಾನಾಂತರ, propname, &len);
		ಆದರೆ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
	}

	ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ; /* error from fdt_next_node() */
}

ಪೂರ್ಣಾಂಕ fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_phandle(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, uint32_t phandle)
{
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ;

	ಆದರೆ ((phandle == 0) || (phandle == ~0U))
		ಹಿಂತಿರುಗಿ -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	ಗಾಗಿ (ಸ್ಥಾನಾಂತರ = fdt_ಮುಂದಿನ_ಗಂಟು(fdt, -1, ಖಾಲಿ);
	     ಸ್ಥಾನಾಂತರ >= 0;
	     ಸ್ಥಾನಾಂತರ = fdt_ಮುಂದಿನ_ಗಂಟು(fdt, ಸ್ಥಾನಾಂತರ, ಖಾಲಿ)) {
		ಆದರೆ (fdt_get_phandle(fdt, ಸ್ಥಾನಾಂತರ) == phandle)
			ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
	}

	ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ; /* error from fdt_next_node() */
}

ಪೂರ್ಣಾಂಕ fdt_stringlist_ಒಳಗೊಂಡಿದೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *strlist, ಪೂರ್ಣಾಂಕ listlen, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
	ಪೂರ್ಣಾಂಕ len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str);
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

	ಆಗ (listlen >= len) {
		ಆದರೆ (memcmp(str, strlist, len+1) == 0)
			ಹಿಂತಿರುಗಿ 1;
		p = memchr(strlist, '\0', listlen);
		ಆದರೆ (!p)
			ಹಿಂತಿರುಗಿ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fdt_stringlist_ಎಣಿಕೆ(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property)
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *list, *ಕೊನೆ;
	ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ, ಎಣಿಕೆ_2 = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &ಉದ್ದಳತೆ);
	ಆದರೆ (!list)
		ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ;

	ಕೊನೆ = list + ಉದ್ದಳತೆ;

	ಆಗ (list < ಕೊನೆ) {
		ಉದ್ದಳತೆ = strnlen(list, ಕೊನೆ - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ಆದರೆ (list + ಉದ್ದಳತೆ > ಕೊನೆ)
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADVALUE;

		list += ಉದ್ದಳತೆ;
		ಎಣಿಕೆ_2++;
	}

	ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}

ಪೂರ್ಣಾಂಕ fdt_stringlist_ಹುಡುಕು(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property,
			  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string)
{
	ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ, len, idx = 0;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *list, *ಕೊನೆ;

	list = fdt_getprop(fdt, nodeoffset, property, &ಉದ್ದಳತೆ);
	ಆದರೆ (!list)
		ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ;

	len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(string) + 1;
	ಕೊನೆ = list + ಉದ್ದಳತೆ;

	ಆಗ (list < ಕೊನೆ) {
		ಉದ್ದಳತೆ = strnlen(list, ಕೊನೆ - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ಆದರೆ (list + ಉದ್ದಳತೆ > ಕೊನೆ)
			ಹಿಂತಿರುಗಿ -FDT_ERR_BADVALUE;

		ಆದರೆ (ಉದ್ದಳತೆ == len && memcmp(list, string, ಉದ್ದಳತೆ) == 0)
			ಹಿಂತಿರುಗಿ idx;

		list += ಉದ್ದಳತೆ;
		idx++;
	}

	ಹಿಂತಿರುಗಿ -FDT_ERR_NOTFOUND;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fdt_stringlist_get(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
			       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *property, ಪೂರ್ಣಾಂಕ idx,
			       ಪೂರ್ಣಾಂಕ *lenp)
{
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *list, *ಕೊನೆ;
	ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ;

	list = fdt_getprop(fdt, nodeoffset, property, &ಉದ್ದಳತೆ);
	ಆದರೆ (!list) {
		ಆದರೆ (lenp)
			*lenp = ಉದ್ದಳತೆ;

		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	ಕೊನೆ = list + ಉದ್ದಳತೆ;

	ಆಗ (list < ಕೊನೆ) {
		ಉದ್ದಳತೆ = strnlen(list, ಕೊನೆ - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ಆದರೆ (list + ಉದ್ದಳತೆ > ಕೊನೆ) {
			ಆದರೆ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}

		ಆದರೆ (idx == 0) {
			ಆದರೆ (lenp)
				*lenp = ಉದ್ದಳತೆ - 1;

			ಹಿಂತಿರುಗಿ list;
		}

		list += ಉದ್ದಳತೆ;
		idx--;
	}

	ಆದರೆ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ fdt_ಗಂಟು_check_compatible(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ nodeoffset,
			      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *compatible)
{
	ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *prop;
	ಪೂರ್ಣಾಂಕ len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ಆದರೆ (!prop)
		ಹಿಂತಿರುಗಿ len;

	ಹಿಂತಿರುಗಿ !fdt_stringlist_ಒಳಗೊಂಡಿದೆ(prop, len, compatible);
}

ಪೂರ್ಣಾಂಕ fdt_ಗಂಟು_ಸ್ಥಾನಾಂತರ_by_compatible(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಂಕ startoffset,
				  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *compatible)
{
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	ಗಾಗಿ (ಸ್ಥಾನಾಂತರ = fdt_ಮುಂದಿನ_ಗಂಟು(fdt, startoffset, ಖಾಲಿ);
	     ಸ್ಥಾನಾಂತರ >= 0;
	     ಸ್ಥಾನಾಂತರ = fdt_ಮುಂದಿನ_ಗಂಟು(fdt, ಸ್ಥಾನಾಂತರ, ಖಾಲಿ)) {
		err = fdt_ಗಂಟು_check_compatible(fdt, ಸ್ಥಾನಾಂತರ, compatible);
		ಆದರೆ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ಹಿಂತಿರುಗಿ err;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (err == 0)
			ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
	}

	ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ; /* error from fdt_next_node() */
}
