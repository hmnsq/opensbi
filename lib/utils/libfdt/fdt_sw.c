// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_env.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <fdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_internal.h"

ਸਥਿਰ ਪੂਰਨ fdt_sw_probe_(ਖਾਲੀ *fdt)
{
	ਜੇ (!can_assume(VALID_ਨਿਵੇਸ਼)) {
		ਜੇ (fdt_magic(fdt) == FDT_MAGIC)
			ਵਾਪਸ -FDT_ERR_BADSTATE;
		ਨਹੀਂਤਾਂ ਜੇ (fdt_magic(fdt) != FDT_SW_MAGIC)
			ਵਾਪਸ -FDT_ERR_BADMAGIC;
	}

	ਵਾਪਸ 0;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_SW_PROBE(fdt) \
	{ \
		ਪੂਰਨ err; \
		ਜੇ ((err = fdt_sw_probe_(fdt)) != 0) \
			ਵਾਪਸ err; \
	}

/* 'memrsv' state:	Initial state after fdt_create()
 *
 * Allowed functions:
 *	fdt_add_reservemap_entry()
 *	fdt_finish_reservemap()		[moves to 'struct' state]
 */
ਸਥਿਰ ਪੂਰਨ fdt_sw_probe_memrsv_(ਖਾਲੀ *fdt)
{
	ਪੂਰਨ err = fdt_sw_probe_(fdt);
	ਜੇ (err)
		ਵਾਪਸ err;

	ਜੇ (!can_assume(VALID_ਨਿਵੇਸ਼) && fdt_off_dt_ਲੜੀਆਂ(fdt) != 0)
		ਵਾਪਸ -FDT_ERR_BADSTATE;
	ਵਾਪਸ 0;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_SW_PROBE_MEMRSV(fdt) \
	{ \
		ਪੂਰਨ err; \
		ਜੇ ((err = fdt_sw_probe_memrsv_(fdt)) != 0) \
			ਵਾਪਸ err; \
	}

/* 'struct' state:	Enter this state after fdt_finish_reservemap()
 *
 * Allowed functions:
 *	fdt_begin_node()
 *	fdt_end_node()
 *	fdt_property*()
 *	fdt_finish()			[moves to 'complete' state]
 */
ਸਥਿਰ ਪੂਰਨ fdt_sw_probe_struct_(ਖਾਲੀ *fdt)
{
	ਪੂਰਨ err = fdt_sw_probe_(fdt);
	ਜੇ (err)
		ਵਾਪਸ err;

	ਜੇ (!can_assume(VALID_ਨਿਵੇਸ਼) &&
	    fdt_off_dt_ਲੜੀਆਂ(fdt) != fdt_totalsize(fdt))
		ਵਾਪਸ -FDT_ERR_BADSTATE;
	ਵਾਪਸ 0;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_SW_PROBE_STRUCT(fdt) \
	{ \
		ਪੂਰਨ err; \
		ਜੇ ((err = fdt_sw_probe_struct_(fdt)) != 0) \
			ਵਾਪਸ err; \
	}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t sw_flags(ਖਾਲੀ *fdt)
{
	/* assert: (fdt_magic(fdt) == FDT_SW_MAGIC) */
	ਵਾਪਸ fdt_ਆਖਰੀ_comp_ਸੰਸਕਰਣ(fdt);
}

/* 'complete' state:	Enter this state after fdt_finish()
 *
 * Allowed functions: none
 */

ਸਥਿਰ ਖਾਲੀ *fdt_grab_space_(ਖਾਲੀ *fdt, size_t len)
{
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਵਿੱਥ = fdt_ਮਾਪ_dt_struct(fdt);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ spaceleft;

	spaceleft = fdt_totalsize(fdt) - fdt_off_dt_struct(fdt)
		- fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt);

	ਜੇ ((ਵਿੱਥ + len < ਵਿੱਥ) || (ਵਿੱਥ + len > spaceleft))
		ਵਾਪਸ ਨਿਰਾ;

	fdt_ਸੈੱਟ_ਮਾਪ_dt_struct(fdt, ਵਿੱਥ + len);
	ਵਾਪਸ fdt_ਵਿੱਥ_ptr_w(fdt, ਵਿੱਥ);
}

ਪੂਰਨ fdt_create_with_flags(ਖਾਲੀ *buf, ਪੂਰਨ bufsize, uint32_t flags)
{
	ਅਚਲ ਪੂਰਨ hdrsize = FDT_ALIGN(ਆਕਾਰ(ਬਣਤਰ fdt_ਸਿਰਲੇਖ),
				      ਆਕਾਰ(ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ));
	ਖਾਲੀ *fdt = buf;

	ਜੇ (bufsize < hdrsize)
		ਵਾਪਸ -FDT_ERR_NOSPACE;

	ਜੇ (flags & ~FDT_CREATE_FLAGS_ALL)
		ਵਾਪਸ -FDT_ERR_BADFLAGS;

	memset(buf, 0, bufsize);

	/*
	 * magic and last_comp_version keep intermediate state during the fdt
	 * creation process, which is replaced with the proper FDT format by
	 * fdt_finish().
	 *
	 * flags should be accessed with sw_flags().
	 */
	fdt_ਸੈੱਟ_magic(fdt, FDT_SW_MAGIC);
	fdt_ਸੈੱਟ_ਸੰਸਕਰਣ(fdt, FDT_ਆਖਰੀ_SUPPORTED_ਸੰਸਕਰਣ);
	fdt_ਸੈੱਟ_ਆਖਰੀ_comp_ਸੰਸਕਰਣ(fdt, flags);

	fdt_ਸੈੱਟ_totalsize(fdt,  bufsize);

	fdt_ਸੈੱਟ_off_mem_rsvmap(fdt, hdrsize);
	fdt_ਸੈੱਟ_off_dt_struct(fdt, fdt_off_mem_rsvmap(fdt));
	fdt_ਸੈੱਟ_off_dt_ਲੜੀਆਂ(fdt, 0);

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_create(ਖਾਲੀ *buf, ਪੂਰਨ bufsize)
{
	ਵਾਪਸ fdt_create_with_flags(buf, bufsize, 0);
}

ਪੂਰਨ fdt_resize(ਖਾਲੀ *fdt, ਖਾਲੀ *buf, ਪੂਰਨ bufsize)
{
	size_t headsize, tailsize;
	ਅੱਖਰ *oldtail, *newtail;

	FDT_SW_PROBE(fdt);

	ਜੇ (bufsize < 0)
		ਵਾਪਸ -FDT_ERR_NOSPACE;

	headsize = fdt_off_dt_struct(fdt) + fdt_ਮਾਪ_dt_struct(fdt);
	tailsize = fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt);

	ਜੇ (!can_assume(VALID_DTB) &&
	    headsize + tailsize > fdt_totalsize(fdt))
		ਵਾਪਸ -FDT_ERR_ਅੰਦਰੂਨੀ;

	ਜੇ ((headsize + tailsize) > (ਬਿਨਾਨਿਸ਼ਾਨੀ)bufsize)
		ਵਾਪਸ -FDT_ERR_NOSPACE;

	oldtail = (ਅੱਖਰ *)fdt + fdt_totalsize(fdt) - tailsize;
	newtail = (ਅੱਖਰ *)buf + bufsize - tailsize;

	/* Two cases to avoid clobbering data if the old and new
	 * buffers partially overlap */
	ਜੇ (buf <= fdt) {
		memmove(buf, fdt, headsize);
		memmove(newtail, oldtail, tailsize);
	} ਨਹੀਂਤਾਂ {
		memmove(newtail, oldtail, tailsize);
		memmove(buf, fdt, headsize);
	}

	fdt_ਸੈੱਟ_totalsize(buf, bufsize);
	ਜੇ (fdt_off_dt_ਲੜੀਆਂ(buf))
		fdt_ਸੈੱਟ_off_dt_ਲੜੀਆਂ(buf, bufsize);

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_ਜੋੜੋ_reservemap_ਇੰਦਰਾਜ(ਖਾਲੀ *fdt, uint64_t addr, uint64_t ਮਾਪ)
{
	ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ *re;
	ਪੂਰਨ ਵਿੱਥ;

	FDT_SW_PROBE_MEMRSV(fdt);

	ਵਿੱਥ = fdt_off_dt_struct(fdt);
	ਜੇ ((ਵਿੱਥ + ਆਕਾਰ(*re)) > fdt_totalsize(fdt))
		ਵਾਪਸ -FDT_ERR_NOSPACE;

	re = (ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ *)((ਅੱਖਰ *)fdt + ਵਿੱਥ);
	re->ਪਤਾ = cpu_ਮੰਜ਼ਿਲ_fdt64(addr);
	re->ਮਾਪ = cpu_ਮੰਜ਼ਿਲ_fdt64(ਮਾਪ);

	fdt_ਸੈੱਟ_off_dt_struct(fdt, ਵਿੱਥ + ਆਕਾਰ(*re));

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_ਅੰਤ_reservemap(ਖਾਲੀ *fdt)
{
	ਪੂਰਨ err = fdt_ਜੋੜੋ_reservemap_ਇੰਦਰਾਜ(fdt, 0, 0);

	ਜੇ (err)
		ਵਾਪਸ err;

	fdt_ਸੈੱਟ_off_dt_ਲੜੀਆਂ(fdt, fdt_totalsize(fdt));
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_begin_ਗੰਢ(ਖਾਲੀ *fdt, ਅਚਲ ਅੱਖਰ *name)
{
	ਬਣਤਰ fdt_ਗੰਢ_ਸਿਰਲੇਖ *nh;
	ਪੂਰਨ namelen;

	FDT_SW_PROBE_STRUCT(fdt);

	namelen = ਲੜੀ_ਲੰਬਾਈ(name) + 1;
	nh = fdt_grab_space_(fdt, ਆਕਾਰ(*nh) + FDT_TAGALIGN(namelen));
	ਜੇ (! nh)
		ਵਾਪਸ -FDT_ERR_NOSPACE;

	nh->ਟੈਗ = cpu_ਮੰਜ਼ਿਲ_fdt32(FDT_BEGIN_ਗੰਢ);
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(nh->name, name, namelen);
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_ਸਿਰਾ_ਗੰਢ(ਖਾਲੀ *fdt)
{
	fdt32_t *en;

	FDT_SW_PROBE_STRUCT(fdt);

	en = fdt_grab_space_(fdt, FDT_TAGSIZE);
	ਜੇ (! en)
		ਵਾਪਸ -FDT_ERR_NOSPACE;

	*en = cpu_ਮੰਜ਼ਿਲ_fdt32(FDT_ਸਿਰਾ_ਗੰਢ);
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ fdt_ਜੋੜੋ_string(ਖਾਲੀ *fdt, ਅਚਲ ਅੱਖਰ *s)
{
	ਅੱਖਰ *strtab = (ਅੱਖਰ *)fdt + fdt_totalsize(fdt);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ strtabsize = fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt);
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len = ਲੜੀ_ਲੰਬਾਈ(s) + 1;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ struct_ਸਿਖਰ, ਵਿੱਥ;

	ਵਿੱਥ = strtabsize + len;
	struct_ਸਿਖਰ = fdt_off_dt_struct(fdt) + fdt_ਮਾਪ_dt_struct(fdt);
	ਜੇ (fdt_totalsize(fdt) - ਵਿੱਥ < struct_ਸਿਖਰ)
		ਵਾਪਸ 0; /* no more room :( */

	ਸਮ੍ਰਿਤੀ_ਨਕਲ(strtab - ਵਿੱਥ, s, len);
	fdt_ਸੈੱਟ_ਮਾਪ_dt_ਲੜੀਆਂ(fdt, strtabsize + len);
	ਵਾਪਸ -ਵਿੱਥ;
}

/* Must only be used to roll back in case of error */
ਸਥਿਰ ਖਾਲੀ fdt_del_ਆਖਰੀ_string(ਖਾਲੀ *fdt, ਅਚਲ ਅੱਖਰ *s)
{
	ਪੂਰਨ strtabsize = fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt);
	ਪੂਰਨ len = ਲੜੀ_ਲੰਬਾਈ(s) + 1;

	fdt_ਸੈੱਟ_ਮਾਪ_dt_ਲੜੀਆਂ(fdt, strtabsize - len);
}

ਸਥਿਰ ਪੂਰਨ fdt_ਲੱਭੋ_ਜੋੜੋ_string(ਖਾਲੀ *fdt, ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ *allocated)
{
	ਅੱਖਰ *strtab = (ਅੱਖਰ *)fdt + fdt_totalsize(fdt);
	ਪੂਰਨ strtabsize = fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt);
	ਅਚਲ ਅੱਖਰ *p;

	*allocated = 0;

	p = fdt_ਲੱਭੋ_string(strtab - strtabsize, strtabsize, s);
	ਜੇ (p)
		ਵਾਪਸ p - strtab;

	*allocated = 1;

	ਵਾਪਸ fdt_ਜੋੜੋ_string(fdt, s);
}

ਪੂਰਨ fdt_property_placeholder(ਖਾਲੀ *fdt, ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ len, ਖਾਲੀ **valp)
{
	ਬਣਤਰ fdt_property *prop;
	ਪੂਰਨ nameoff;
	ਪੂਰਨ allocated;

	FDT_SW_PROBE_STRUCT(fdt);

	/* String de-duplication can be slow, _NO_NAME_DEDUP skips it */
	ਜੇ (sw_flags(fdt) & FDT_CREATE_FLAG_NO_NAME_DEDUP) {
		allocated = 1;
		nameoff = fdt_ਜੋੜੋ_string(fdt, name);
	} ਨਹੀਂਤਾਂ {
		nameoff = fdt_ਲੱਭੋ_ਜੋੜੋ_string(fdt, name, &allocated);
	}
	ਜੇ (nameoff == 0)
		ਵਾਪਸ -FDT_ERR_NOSPACE;

	prop = fdt_grab_space_(fdt, ਆਕਾਰ(*prop) + FDT_TAGALIGN(len));
	ਜੇ (! prop) {
		ਜੇ (allocated)
			fdt_del_ਆਖਰੀ_string(fdt, name);
		ਵਾਪਸ -FDT_ERR_NOSPACE;
	}

	prop->ਟੈਗ = cpu_ਮੰਜ਼ਿਲ_fdt32(FDT_PROP);
	prop->nameoff = cpu_ਮੰਜ਼ਿਲ_fdt32(nameoff);
	prop->len = cpu_ਮੰਜ਼ਿਲ_fdt32(len);
	*valp = prop->ਅੰਕੜੇ;
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_property(ਖਾਲੀ *fdt, ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਖਾਲੀ *val, ਪੂਰਨ len)
{
	ਖਾਲੀ *ptr;
	ਪੂਰਨ ret;

	ret = fdt_property_placeholder(fdt, name, len, &ptr);
	ਜੇ (ret)
		ਵਾਪਸ ret;
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(ptr, val, len);
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_ਅੰਤ(ਖਾਲੀ *fdt)
{
	ਅੱਖਰ *p = (ਅੱਖਰ *)fdt;
	fdt32_t *ਸਿਰਾ;
	ਪੂਰਨ oldstroffset, newstroffset;
	uint32_t ਟੈਗ;
	ਪੂਰਨ ਵਿੱਥ, nextoffset;

	FDT_SW_PROBE_STRUCT(fdt);

	/* Add terminator */
	ਸਿਰਾ = fdt_grab_space_(fdt, ਆਕਾਰ(*ਸਿਰਾ));
	ਜੇ (! ਸਿਰਾ)
		ਵਾਪਸ -FDT_ERR_NOSPACE;
	*ਸਿਰਾ = cpu_ਮੰਜ਼ਿਲ_fdt32(FDT_ਸਿਰਾ);

	/* Relocate the string table */
	oldstroffset = fdt_totalsize(fdt) - fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt);
	newstroffset = fdt_off_dt_struct(fdt) + fdt_ਮਾਪ_dt_struct(fdt);
	memmove(p + newstroffset, p + oldstroffset, fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt));
	fdt_ਸੈੱਟ_off_dt_ਲੜੀਆਂ(fdt, newstroffset);

	/* Walk the structure, correcting string offsets */
	ਵਿੱਥ = 0;
	ਜਦੋਂ ((ਟੈਗ = fdt_ਅਗਲਾ_ਟੈਗ(fdt, ਵਿੱਥ, &nextoffset)) != FDT_ਸਿਰਾ) {
		ਜੇ (ਟੈਗ == FDT_PROP) {
			ਬਣਤਰ fdt_property *prop =
				fdt_ਵਿੱਥ_ptr_w(fdt, ਵਿੱਥ);
			ਪੂਰਨ nameoff;

			nameoff = fdt32_ਮੰਜ਼ਿਲ_cpu(prop->nameoff);
			nameoff += fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt);
			prop->nameoff = cpu_ਮੰਜ਼ਿਲ_fdt32(nameoff);
		}
		ਵਿੱਥ = nextoffset;
	}
	ਜੇ (nextoffset < 0)
		ਵਾਪਸ nextoffset;

	/* Finally, adjust the header */
	fdt_ਸੈੱਟ_totalsize(fdt, newstroffset + fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt));

	/* And fix up fields that were keeping intermediate state. */
	fdt_ਸੈੱਟ_ਆਖਰੀ_comp_ਸੰਸਕਰਣ(fdt, FDT_ਆਖਰੀ_COMPATIBLE_ਸੰਸਕਰਣ);
	fdt_ਸੈੱਟ_magic(fdt, FDT_MAGIC);

	ਵਾਪਸ 0;
}
