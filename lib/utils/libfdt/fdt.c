// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_env.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <fdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(ਅਚਲ ਖਾਲੀ *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	ਜੇ (can_assume(VALID_DTB))
		ਵਾਪਸ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ਜੇ ((uintptr_t)fdt & 7)
		ਵਾਪਸ -FDT_ERR_ALIGNMENT;

	ਜੇ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ਜੇ (!can_assume(LATEST)) {
			ਜੇ (fdt_ਸੰਸਕਰਣ(fdt) < FDT_ਪਹਿਲਾ_SUPPORTED_ਸੰਸਕਰਣ)
				ਵਾਪਸ -FDT_ERR_BADVERSION;
			ਜੇ (fdt_ਆਖਰੀ_comp_ਸੰਸਕਰਣ(fdt) >
					FDT_ਆਖਰੀ_SUPPORTED_ਸੰਸਕਰਣ)
				ਵਾਪਸ -FDT_ERR_BADVERSION;
		}
	} ਨਹੀਂਤਾਂ ਜੇ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ਜੇ (!can_assume(VALID_ਨਿਵੇਸ਼) && fdt_ਮਾਪ_dt_struct(fdt) == 0)
			ਵਾਪਸ -FDT_ERR_BADSTATE;
	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ -FDT_ERR_BADMAGIC;
	}

	ਜੇ (totalsize < INT32_MAX)
		ਵਾਪਸ totalsize;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ -FDT_ERR_TRUNCATED;
}

ਸਥਿਰ ਪੂਰਨ check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ਵਾਪਸ (off >= hdrsize) && (off <= totalsize);
}

ਸਥਿਰ ਪੂਰਨ check_ਖੰਡ(uint32_t hdrsize, uint32_t totalsize,
			uint32_t ਅਧਾਰ_2, uint32_t ਮਾਪ)
{
	ਜੇ (!check_off_(hdrsize, totalsize, ਅਧਾਰ_2))
		ਵਾਪਸ 0; /* block start out of bounds */
	ਜੇ ((ਅਧਾਰ_2 + ਮਾਪ) < ਅਧਾਰ_2)
		ਵਾਪਸ 0; /*ਉਛਾਲ*/
	ਜੇ (!check_off_(hdrsize, totalsize, ਅਧਾਰ_2 + ਮਾਪ))
		ਵਾਪਸ 0; /* block end out of bounds */
	ਵਾਪਸ 1;
}

size_t fdt_ਸਿਰਲੇਖ_ਮਾਪ(uint32_t ਸੰਸਕਰਣ)
{
	ਜੇ (ਸੰਸਕਰਣ <= 1)
		ਵਾਪਸ FDT_V1_ਮਾਪ;
	ਨਹੀਂਤਾਂ ਜੇ (ਸੰਸਕਰਣ <= 2)
		ਵਾਪਸ FDT_V2_ਮਾਪ;
	ਨਹੀਂਤਾਂ ਜੇ (ਸੰਸਕਰਣ <= 3)
		ਵਾਪਸ FDT_V3_ਮਾਪ;
	ਨਹੀਂਤਾਂ ਜੇ (ਸੰਸਕਰਣ <= 16)
		ਵਾਪਸ FDT_V16_ਮਾਪ;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ FDT_V17_ਮਾਪ;
}

size_t fdt_ਸਿਰਲੇਖ_ਮਾਪ_2(ਅਚਲ ਖਾਲੀ *fdt)
{
	ਵਾਪਸ can_assume(LATEST) ? FDT_V17_ਮਾਪ :
		fdt_ਸਿਰਲੇਖ_ਮਾਪ(fdt_ਸੰਸਕਰਣ(fdt));
}

ਪੂਰਨ fdt_check_ਸਿਰਲੇਖ(ਅਚਲ ਖਾਲੀ *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ਜੇ ((uintptr_t)fdt & 7)
		ਵਾਪਸ -FDT_ERR_ALIGNMENT;

	ਜੇ (fdt_magic(fdt) != FDT_MAGIC)
		ਵਾਪਸ -FDT_ERR_BADMAGIC;
	ਜੇ (!can_assume(LATEST)) {
		ਜੇ ((fdt_ਸੰਸਕਰਣ(fdt) < FDT_ਪਹਿਲਾ_SUPPORTED_ਸੰਸਕਰਣ)
		    || (fdt_ਆਖਰੀ_comp_ਸੰਸਕਰਣ(fdt) >
			FDT_ਆਖਰੀ_SUPPORTED_ਸੰਸਕਰਣ))
			ਵਾਪਸ -FDT_ERR_BADVERSION;
		ਜੇ (fdt_ਸੰਸਕਰਣ(fdt) < fdt_ਆਖਰੀ_comp_ਸੰਸਕਰਣ(fdt))
			ਵਾਪਸ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_ਸਿਰਲੇਖ_ਮਾਪ_2(fdt);
	ਜੇ (!can_assume(VALID_DTB)) {

		ਜੇ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ))
			ਵਾਪਸ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ਜੇ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ਵਾਪਸ -FDT_ERR_TRUNCATED;
	}

	ਜੇ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ਜੇ (!can_assume(LATEST) && fdt_ਸੰਸਕਰਣ(fdt) < 17) {
			ਜੇ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ਵਾਪਸ -FDT_ERR_TRUNCATED;
		} ਨਹੀਂਤਾਂ {
			ਜੇ (!check_ਖੰਡ(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_ਮਾਪ_dt_struct(fdt)))
				ਵਾਪਸ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ਜੇ (!check_ਖੰਡ(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_ਲੜੀਆਂ(fdt),
				  fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt)))
			ਵਾਪਸ -FDT_ERR_TRUNCATED;
	}

	ਵਾਪਸ 0;
}

ਅਚਲ ਖਾਲੀ *fdt_ਵਿੱਥ_ptr(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ uoffset = ਵਿੱਥ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ absoffset = ਵਿੱਥ + fdt_off_dt_struct(fdt);

	ਜੇ (ਵਿੱਥ < 0)
		ਵਾਪਸ ਨਿਰਾ;

	ਜੇ (!can_assume(VALID_ਨਿਵੇਸ਼))
		ਜੇ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ਵਾਪਸ ਨਿਰਾ;

	ਜੇ (can_assume(LATEST) || fdt_ਸੰਸਕਰਣ(fdt) >= 0x11)
		ਜੇ (((uoffset + len) < uoffset)
		    || ((ਵਿੱਥ + len) > fdt_ਮਾਪ_dt_struct(fdt)))
			ਵਾਪਸ ਨਿਰਾ;

	ਵਾਪਸ fdt_ਵਿੱਥ_ptr_2(fdt, ਵਿੱਥ);
}

uint32_t fdt_ਅਗਲਾ_ਟੈਗ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ startoffset, ਪੂਰਨ *nextoffset)
{
	ਅਚਲ fdt32_t *tagp, *lenp;
	uint32_t ਟੈਗ;
	ਪੂਰਨ ਵਿੱਥ = startoffset;
	ਅਚਲ ਅੱਖਰ *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_ਵਿੱਥ_ptr(fdt, ਵਿੱਥ, FDT_TAGSIZE);
	ਜੇ (!can_assume(VALID_DTB) && !tagp)
		ਵਾਪਸ FDT_ਸਿਰਾ; /* premature end */
	ਟੈਗ = fdt32_ਮੰਜ਼ਿਲ_cpu(*tagp);
	ਵਿੱਥ += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	ਚੋਣ (ਟੈਗ) {
	ਹਾਲਤ FDT_BEGIN_ਗੰਢ:
		/* skip name */
		ਕਰੋ {
			p = fdt_ਵਿੱਥ_ptr(fdt, ਵਿੱਥ++, 1);
		} ਜਦੋਂ (p && (*p != '\0'));
		ਜੇ (!can_assume(VALID_DTB) && !p)
			ਵਾਪਸ FDT_ਸਿਰਾ; /* premature end */
		ਰੋਕੋ;

	ਹਾਲਤ FDT_PROP:
		lenp = fdt_ਵਿੱਥ_ptr(fdt, ਵਿੱਥ, ਆਕਾਰ(*lenp));
		ਜੇ (!can_assume(VALID_DTB) && !lenp)
			ਵਾਪਸ FDT_ਸਿਰਾ; /* premature end */
		/* skip-name offset, length and value */
		ਵਿੱਥ += ਆਕਾਰ(ਬਣਤਰ fdt_property) - FDT_TAGSIZE
			+ fdt32_ਮੰਜ਼ਿਲ_cpu(*lenp);
		ਜੇ (!can_assume(LATEST) &&
		    fdt_ਸੰਸਕਰਣ(fdt) < 0x10 && fdt32_ਮੰਜ਼ਿਲ_cpu(*lenp) >= 8 &&
		    ((ਵਿੱਥ - fdt32_ਮੰਜ਼ਿਲ_cpu(*lenp)) % 8) != 0)
			ਵਿੱਥ += 4;
		ਰੋਕੋ;

	ਹਾਲਤ FDT_ਸਿਰਾ:
	ਹਾਲਤ FDT_ਸਿਰਾ_ਗੰਢ:
	ਹਾਲਤ FDT_NOP:
		ਰੋਕੋ;

	ਮੂਲ:
		ਵਾਪਸ FDT_ਸਿਰਾ;
	}

	ਜੇ (!fdt_ਵਿੱਥ_ptr(fdt, startoffset, ਵਿੱਥ - startoffset))
		ਵਾਪਸ FDT_ਸਿਰਾ; /* premature end */

	*nextoffset = FDT_TAGALIGN(ਵਿੱਥ);
	ਵਾਪਸ ਟੈਗ;
}

ਪੂਰਨ fdt_check_ਗੰਢ_ਵਿੱਥ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ)
{
	ਜੇ (!can_assume(VALID_ਨਿਵੇਸ਼)
	    && ((ਵਿੱਥ < 0) || (ਵਿੱਥ % FDT_TAGSIZE)))
		ਵਾਪਸ -FDT_ERR_BADOFFSET;

	ਜੇ (fdt_ਅਗਲਾ_ਟੈਗ(fdt, ਵਿੱਥ, &ਵਿੱਥ) != FDT_BEGIN_ਗੰਢ)
		ਵਾਪਸ -FDT_ERR_BADOFFSET;

	ਵਾਪਸ ਵਿੱਥ;
}

ਪੂਰਨ fdt_check_prop_ਵਿੱਥ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ)
{
	ਜੇ (!can_assume(VALID_ਨਿਵੇਸ਼)
	    && ((ਵਿੱਥ < 0) || (ਵਿੱਥ % FDT_TAGSIZE)))
		ਵਾਪਸ -FDT_ERR_BADOFFSET;

	ਜੇ (fdt_ਅਗਲਾ_ਟੈਗ(fdt, ਵਿੱਥ, &ਵਿੱਥ) != FDT_PROP)
		ਵਾਪਸ -FDT_ERR_BADOFFSET;

	ਵਾਪਸ ਵਿੱਥ;
}

ਪੂਰਨ fdt_ਅਗਲਾ_ਗੰਢ(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ *ਡੂੰਘਾਈ)
{
	ਪੂਰਨ nextoffset = 0;
	uint32_t ਟੈਗ;

	ਜੇ (ਵਿੱਥ >= 0)
		ਜੇ ((nextoffset = fdt_check_ਗੰਢ_ਵਿੱਥ(fdt, ਵਿੱਥ)) < 0)
			ਵਾਪਸ nextoffset;

	ਕਰੋ {
		ਵਿੱਥ = nextoffset;
		ਟੈਗ = fdt_ਅਗਲਾ_ਟੈਗ(fdt, ਵਿੱਥ, &nextoffset);

		ਚੋਣ (ਟੈਗ) {
		ਹਾਲਤ FDT_PROP:
		ਹਾਲਤ FDT_NOP:
			ਰੋਕੋ;

		ਹਾਲਤ FDT_BEGIN_ਗੰਢ:
			ਜੇ (ਡੂੰਘਾਈ)
				(*ਡੂੰਘਾਈ)++;
			ਰੋਕੋ;

		ਹਾਲਤ FDT_ਸਿਰਾ_ਗੰਢ:
			ਜੇ (ਡੂੰਘਾਈ && ((--(*ਡੂੰਘਾਈ)) < 0))
				ਵਾਪਸ nextoffset;
			ਰੋਕੋ;

		ਹਾਲਤ FDT_ਸਿਰਾ:
			ਜੇ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !ਡੂੰਘਾਈ))
				ਵਾਪਸ -FDT_ERR_NOTFOUND;
			ਨਹੀਂਤਾਂ
				ਵਾਪਸ nextoffset;
		}
	} ਜਦੋਂ (ਟੈਗ != FDT_BEGIN_ਗੰਢ);

	ਵਾਪਸ ਵਿੱਥ;
}

ਪੂਰਨ fdt_ਪਹਿਲਾ_subnode(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ)
{
	ਪੂਰਨ ਡੂੰਘਾਈ = 0;

	ਵਿੱਥ = fdt_ਅਗਲਾ_ਗੰਢ(fdt, ਵਿੱਥ, &ਡੂੰਘਾਈ);
	ਜੇ (ਵਿੱਥ < 0 || ਡੂੰਘਾਈ != 1)
		ਵਾਪਸ -FDT_ERR_NOTFOUND;

	ਵਾਪਸ ਵਿੱਥ;
}

ਪੂਰਨ fdt_ਅਗਲਾ_subnode(ਅਚਲ ਖਾਲੀ *fdt, ਪੂਰਨ ਵਿੱਥ)
{
	ਪੂਰਨ ਡੂੰਘਾਈ = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	ਕਰੋ {
		ਵਿੱਥ = fdt_ਅਗਲਾ_ਗੰਢ(fdt, ਵਿੱਥ, &ਡੂੰਘਾਈ);
		ਜੇ (ਵਿੱਥ < 0 || ਡੂੰਘਾਈ < 1)
			ਵਾਪਸ -FDT_ERR_NOTFOUND;
	} ਜਦੋਂ (ਡੂੰਘਾਈ > 1);

	ਵਾਪਸ ਵਿੱਥ;
}

ਅਚਲ ਅੱਖਰ *fdt_ਲੱਭੋ_string(ਅਚਲ ਅੱਖਰ *strtab, ਪੂਰਨ tabsize, ਅਚਲ ਅੱਖਰ *s)
{
	ਪੂਰਨ len = ਲੜੀ_ਲੰਬਾਈ(s) + 1;
	ਅਚਲ ਅੱਖਰ *ਆਖਰੀ = strtab + tabsize - len;
	ਅਚਲ ਅੱਖਰ *p;

	ਲਈ (p = strtab; p <= ਆਖਰੀ; p++)
		ਜੇ (memcmp(p, s, len) == 0)
			ਵਾਪਸ p;
	ਵਾਪਸ ਨਿਰਾ;
}

ਪੂਰਨ fdt_move(ਅਚਲ ਖਾਲੀ *fdt, ਖਾਲੀ *buf, ਪੂਰਨ bufsize)
{
	ਜੇ (!can_assume(VALID_ਨਿਵੇਸ਼) && bufsize < 0)
		ਵਾਪਸ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ਜੇ (fdt_totalsize(fdt) > (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)bufsize)
		ਵਾਪਸ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ਵਾਪਸ 0;
}
