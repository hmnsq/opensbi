// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_env.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <fdt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libfdt.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "libfdt_internal.h"

ਸਥਿਰ ਪੂਰਨ fdt_ਖੰਡਾਂ_misordered(ਅਚਲ ਖਾਲੀ *fdt,
				  ਪੂਰਨ mem_rsv_ਮਾਪ, ਪੂਰਨ struct_ਮਾਪ)
{
	ਵਾਪਸ (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(ਆਕਾਰ(ਬਣਤਰ fdt_ਸਿਰਲੇਖ), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_ਮਾਪ))
		|| (fdt_off_dt_ਲੜੀਆਂ(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_ਮਾਪ))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_ਲੜੀਆਂ(fdt) + fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt)));
}

ਸਥਿਰ ਪੂਰਨ fdt_rw_probe_(ਖਾਲੀ *fdt)
{
	ਜੇ (can_assume(VALID_DTB))
		ਵਾਪਸ 0;
	FDT_RO_PROBE(fdt);

	ਜੇ (!can_assume(LATEST) && fdt_ਸੰਸਕਰਣ(fdt) < 17)
		ਵਾਪਸ -FDT_ERR_BADVERSION;
	ਜੇ (fdt_ਖੰਡਾਂ_misordered(fdt, ਆਕਾਰ(ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ),
				   fdt_ਮਾਪ_dt_struct(fdt)))
		ਵਾਪਸ -FDT_ERR_BADLAYOUT;
	ਜੇ (!can_assume(LATEST) && fdt_ਸੰਸਕਰਣ(fdt) > 17)
		fdt_ਸੈੱਟ_ਸੰਸਕਰਣ(fdt, 17);

	ਵਾਪਸ 0;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FDT_RW_PROBE(fdt) \
	{ \
		ਪੂਰਨ err_; \
		ਜੇ ((err_ = fdt_rw_probe_(fdt)) != 0) \
			ਵਾਪਸ err_; \
	}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ fdt_ਅੰਕੜੇ_ਮਾਪ(ਖਾਲੀ *fdt)
{
	ਵਾਪਸ fdt_off_dt_ਲੜੀਆਂ(fdt) + fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt);
}

ਸਥਿਰ ਪੂਰਨ fdt_splice_(ਖਾਲੀ *fdt, ਖਾਲੀ *splicepoint, ਪੂਰਨ oldlen, ਪੂਰਨ newlen)
{
	ਅੱਖਰ *p = splicepoint;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ dsize = fdt_ਅੰਕੜੇ_ਮਾਪ(fdt);
	size_t soff = p - (ਅੱਖਰ *)fdt;

	ਜੇ ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		ਵਾਪਸ -FDT_ERR_BADOFFSET;
	ਜੇ ((p < (ਅੱਖਰ *)fdt) || (dsize + newlen < (ਬਿਨਾਨਿਸ਼ਾਨੀ)oldlen))
		ਵਾਪਸ -FDT_ERR_BADOFFSET;
	ਜੇ (dsize - oldlen + newlen > fdt_totalsize(fdt))
		ਵਾਪਸ -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((ਅੱਖਰ *)fdt + dsize) - (p + oldlen));
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ fdt_splice_mem_rsv_(ਖਾਲੀ *fdt, ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ *p,
			       ਪੂਰਨ oldn, ਪੂਰਨ newn)
{
	ਪੂਰਨ delta = (newn - oldn) * ਆਕਾਰ(*p);
	ਪੂਰਨ err;
	err = fdt_splice_(fdt, p, oldn * ਆਕਾਰ(*p), newn * ਆਕਾਰ(*p));
	ਜੇ (err)
		ਵਾਪਸ err;
	fdt_ਸੈੱਟ_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_ਸੈੱਟ_off_dt_ਲੜੀਆਂ(fdt, fdt_off_dt_ਲੜੀਆਂ(fdt) + delta);
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ fdt_splice_struct_(ਖਾਲੀ *fdt, ਖਾਲੀ *p,
			      ਪੂਰਨ oldlen, ਪੂਰਨ newlen)
{
	ਪੂਰਨ delta = newlen - oldlen;
	ਪੂਰਨ err;

	ਜੇ ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		ਵਾਪਸ err;

	fdt_ਸੈੱਟ_ਮਾਪ_dt_struct(fdt, fdt_ਮਾਪ_dt_struct(fdt) + delta);
	fdt_ਸੈੱਟ_off_dt_ਲੜੀਆਂ(fdt, fdt_off_dt_ਲੜੀਆਂ(fdt) + delta);
	ਵਾਪਸ 0;
}

/* Must only be used to roll back in case of error */
ਸਥਿਰ ਖਾਲੀ fdt_del_ਆਖਰੀ_string(ਖਾਲੀ *fdt, ਅਚਲ ਅੱਖਰ *s)
{
	ਪੂਰਨ newlen = ਲੜੀ_ਲੰਬਾਈ(s) + 1;

	fdt_ਸੈੱਟ_ਮਾਪ_dt_ਲੜੀਆਂ(fdt, fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt) - newlen);
}

ਸਥਿਰ ਪੂਰਨ fdt_splice_string_(ਖਾਲੀ *fdt, ਪੂਰਨ newlen)
{
	ਖਾਲੀ *p = (ਅੱਖਰ *)fdt
		+ fdt_off_dt_ਲੜੀਆਂ(fdt) + fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt);
	ਪੂਰਨ err;

	ਜੇ ((err = fdt_splice_(fdt, p, 0, newlen)))
		ਵਾਪਸ err;

	fdt_ਸੈੱਟ_ਮਾਪ_dt_ਲੜੀਆਂ(fdt, fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt) + newlen);
	ਵਾਪਸ 0;
}

/**
 * fdt_find_add_string_() - Find or allocate a string
 *
 * @fdt: pointer to the device tree to check/adjust
 * @s: string to find/add
 * @allocated: Set to 0 if the string was found, 1 if not found and so
 *	allocated. Ignored if can_assume(NO_ROLLBACK)
 * @return offset of string in the string table (whether found or added)
 */
ਸਥਿਰ ਪੂਰਨ fdt_ਲੱਭੋ_ਜੋੜੋ_string(ਖਾਲੀ *fdt, ਅਚਲ ਅੱਖਰ *s, ਪੂਰਨ *allocated)
{
	ਅੱਖਰ *strtab = (ਅੱਖਰ *)fdt + fdt_off_dt_ਲੜੀਆਂ(fdt);
	ਅਚਲ ਅੱਖਰ *p;
	ਅੱਖਰ *new;
	ਪੂਰਨ len = ਲੜੀ_ਲੰਬਾਈ(s) + 1;
	ਪੂਰਨ err;

	ਜੇ (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_ਲੱਭੋ_string(strtab, fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt), s);
	ਜੇ (p)
		/* found it */
		ਵਾਪਸ (p - strtab);

	new = strtab + fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt);
	err = fdt_splice_string_(fdt, len);
	ਜੇ (err)
		ਵਾਪਸ err;

	ਜੇ (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	ਸਮ੍ਰਿਤੀ_ਨਕਲ(new, s, len);
	ਵਾਪਸ (new - strtab);
}

ਪੂਰਨ fdt_ਜੋੜੋ_mem_rsv(ਖਾਲੀ *fdt, uint64_t ਪਤਾ, uint64_t ਮਾਪ)
{
	ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ *re;
	ਪੂਰਨ err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_ਅੰਕ_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	ਜੇ (err)
		ਵਾਪਸ err;

	re->ਪਤਾ = cpu_ਮੰਜ਼ਿਲ_fdt64(ਪਤਾ);
	re->ਮਾਪ = cpu_ਮੰਜ਼ਿਲ_fdt64(ਮਾਪ);
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_del_mem_rsv(ਖਾਲੀ *fdt, ਪੂਰਨ n)
{
	ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	ਜੇ (n >= fdt_ਅੰਕ_mem_rsv(fdt))
		ਵਾਪਸ -FDT_ERR_NOTFOUND;

	ਵਾਪਸ fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

ਸਥਿਰ ਪੂਰਨ fdt_resize_property_(ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਅਚਲ ਅੱਖਰ *name,
				ਪੂਰਨ len, ਬਣਤਰ fdt_property **prop)
{
	ਪੂਰਨ oldlen;
	ਪੂਰਨ err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ਜੇ (!*prop)
		ਵਾਪਸ oldlen;

	ਜੇ ((err = fdt_splice_struct_(fdt, (*prop)->ਅੰਕੜੇ, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		ਵਾਪਸ err;

	(*prop)->len = cpu_ਮੰਜ਼ਿਲ_fdt32(len);
	ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ fdt_ਜੋੜੋ_property(ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਅਚਲ ਅੱਖਰ *name,
			     ਪੂਰਨ len, ਬਣਤਰ fdt_property **prop)
{
	ਪੂਰਨ proplen;
	ਪੂਰਨ nextoffset;
	ਪੂਰਨ namestroff;
	ਪੂਰਨ err;
	ਪੂਰਨ allocated;

	ਜੇ ((nextoffset = fdt_check_ਗੰਢ_ਵਿੱਥ(fdt, nodeoffset)) < 0)
		ਵਾਪਸ nextoffset;

	namestroff = fdt_ਲੱਭੋ_ਜੋੜੋ_string(fdt, name, &allocated);
	ਜੇ (namestroff < 0)
		ਵਾਪਸ namestroff;

	*prop = fdt_ਵਿੱਥ_ptr_w(fdt, nextoffset);
	proplen = ਆਕਾਰ(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	ਜੇ (err) {
		/* Delete the string if we failed to add it */
		ਜੇ (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_ਆਖਰੀ_string(fdt, name);
		ਵਾਪਸ err;
	}

	(*prop)->ਟੈਗ = cpu_ਮੰਜ਼ਿਲ_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_ਮੰਜ਼ਿਲ_fdt32(namestroff);
	(*prop)->len = cpu_ਮੰਜ਼ਿਲ_fdt32(len);
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_ਸੈੱਟ_name(ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਅਚਲ ਅੱਖਰ *name)
{
	ਅੱਖਰ *namep;
	ਪੂਰਨ oldlen, newlen;
	ਪੂਰਨ err;

	FDT_RW_PROBE(fdt);

	namep = (ਅੱਖਰ *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	ਜੇ (!namep)
		ਵਾਪਸ oldlen;

	newlen = ਲੜੀ_ਲੰਬਾਈ(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	ਜੇ (err)
		ਵਾਪਸ err;

	ਸਮ੍ਰਿਤੀ_ਨਕਲ(namep, name, newlen+1);
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_setprop_placeholder(ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਅਚਲ ਅੱਖਰ *name,
			    ਪੂਰਨ len, ਖਾਲੀ **prop_ਅੰਕੜੇ)
{
	ਬਣਤਰ fdt_property *prop;
	ਪੂਰਨ err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	ਜੇ (err == -FDT_ERR_NOTFOUND)
		err = fdt_ਜੋੜੋ_property(fdt, nodeoffset, name, len, &prop);
	ਜੇ (err)
		ਵਾਪਸ err;

	*prop_ਅੰਕੜੇ = prop->ਅੰਕੜੇ;
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_setprop(ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਅਚਲ ਅੱਖਰ *name,
		ਅਚਲ ਖਾਲੀ *val, ਪੂਰਨ len)
{
	ਖਾਲੀ *prop_ਅੰਕੜੇ;
	ਪੂਰਨ err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_ਅੰਕੜੇ);
	ਜੇ (err)
		ਵਾਪਸ err;

	ਜੇ (len)
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(prop_ਅੰਕੜੇ, val, len);
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_appendprop(ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਅਚਲ ਅੱਖਰ *name,
		   ਅਚਲ ਖਾਲੀ *val, ਪੂਰਨ len)
{
	ਬਣਤਰ fdt_property *prop;
	ਪੂਰਨ err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ਜੇ (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->ਅੰਕੜੇ,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		ਜੇ (err)
			ਵਾਪਸ err;
		prop->len = cpu_ਮੰਜ਼ਿਲ_fdt32(newlen);
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(prop->ਅੰਕੜੇ + oldlen, val, len);
	} ਨਹੀਂਤਾਂ {
		err = fdt_ਜੋੜੋ_property(fdt, nodeoffset, name, len, &prop);
		ਜੇ (err)
			ਵਾਪਸ err;
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(prop->ਅੰਕੜੇ, val, len);
	}
	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_delprop(ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset, ਅਚਲ ਅੱਖਰ *name)
{
	ਬਣਤਰ fdt_property *prop;
	ਪੂਰਨ len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ਜੇ (!prop)
		ਵਾਪਸ len;

	proplen = ਆਕਾਰ(*prop) + FDT_TAGALIGN(len);
	ਵਾਪਸ fdt_splice_struct_(fdt, prop, proplen, 0);
}

ਪੂਰਨ fdt_ਜੋੜੋ_subnode_namelen(ਖਾਲੀ *fdt, ਪੂਰਨ parentoffset,
			    ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ namelen)
{
	ਬਣਤਰ fdt_ਗੰਢ_ਸਿਰਲੇਖ *nh;
	ਪੂਰਨ ਵਿੱਥ, nextoffset;
	ਪੂਰਨ nodelen;
	ਪੂਰਨ err;
	uint32_t ਟੈਗ;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	ਵਿੱਥ = fdt_subnode_ਵਿੱਥ_namelen(fdt, parentoffset, name, namelen);
	ਜੇ (ਵਿੱਥ >= 0)
		ਵਾਪਸ -FDT_ERR_EXISTS;
	ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਥ != -FDT_ERR_NOTFOUND)
		ਵਾਪਸ ਵਿੱਥ;

	/* Try to place the new node after the parent's properties */
	ਟੈਗ = fdt_ਅਗਲਾ_ਟੈਗ(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	ਜੇ (!can_assume(LIBFDT_FLAWLESS) && (ਟੈਗ != FDT_BEGIN_ਗੰਢ))
		ਵਾਪਸ -FDT_ERR_ਅੰਦਰੂਨੀ;
	ਕਰੋ {
		ਵਿੱਥ = nextoffset;
		ਟੈਗ = fdt_ਅਗਲਾ_ਟੈਗ(fdt, ਵਿੱਥ, &nextoffset);
	} ਜਦੋਂ ((ਟੈਗ == FDT_PROP) || (ਟੈਗ == FDT_NOP));

	nh = fdt_ਵਿੱਥ_ptr_w(fdt, ਵਿੱਥ);
	nodelen = ਆਕਾਰ(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	ਜੇ (err)
		ਵਾਪਸ err;

	nh->ਟੈਗ = cpu_ਮੰਜ਼ਿਲ_fdt32(FDT_BEGIN_ਗੰਢ);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(nh->name, name, namelen);
	endtag = (fdt32_t *)((ਅੱਖਰ *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_ਮੰਜ਼ਿਲ_fdt32(FDT_ਸਿਰਾ_ਗੰਢ);

	ਵਾਪਸ ਵਿੱਥ;
}

ਪੂਰਨ fdt_ਜੋੜੋ_subnode(ਖਾਲੀ *fdt, ਪੂਰਨ parentoffset, ਅਚਲ ਅੱਖਰ *name)
{
	ਵਾਪਸ fdt_ਜੋੜੋ_subnode_namelen(fdt, parentoffset, name, ਲੜੀ_ਲੰਬਾਈ(name));
}

ਪੂਰਨ fdt_del_ਗੰਢ(ਖਾਲੀ *fdt, ਪੂਰਨ nodeoffset)
{
	ਪੂਰਨ endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_ਗੰਢ_ਸਿਰਾ_ਵਿੱਥ(fdt, nodeoffset);
	ਜੇ (endoffset < 0)
		ਵਾਪਸ endoffset;

	ਵਾਪਸ fdt_splice_struct_(fdt, fdt_ਵਿੱਥ_ptr_w(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

ਸਥਿਰ ਖਾਲੀ fdt_packblocks_(ਅਚਲ ਅੱਖਰ *old, ਅੱਖਰ *new,
			    ਪੂਰਨ mem_rsv_ਮਾਪ,
			    ਪੂਰਨ struct_ਮਾਪ,
			    ਪੂਰਨ ਲੜੀਆਂ_ਮਾਪ)
{
	ਪੂਰਨ mem_rsv_off, struct_off, ਲੜੀਆਂ_off;

	mem_rsv_off = FDT_ALIGN(ਆਕਾਰ(ਬਣਤਰ fdt_ਸਿਰਲੇਖ), 8);
	struct_off = mem_rsv_off + mem_rsv_ਮਾਪ;
	ਲੜੀਆਂ_off = struct_off + struct_ਮਾਪ;

	memmove(new + mem_rsv_off, old + fdt_off_mem_rsvmap(old), mem_rsv_ਮਾਪ);
	fdt_ਸੈੱਟ_off_mem_rsvmap(new, mem_rsv_off);

	memmove(new + struct_off, old + fdt_off_dt_struct(old), struct_ਮਾਪ);
	fdt_ਸੈੱਟ_off_dt_struct(new, struct_off);
	fdt_ਸੈੱਟ_ਮਾਪ_dt_struct(new, struct_ਮਾਪ);

	memmove(new + ਲੜੀਆਂ_off, old + fdt_off_dt_ਲੜੀਆਂ(old), ਲੜੀਆਂ_ਮਾਪ);
	fdt_ਸੈੱਟ_off_dt_ਲੜੀਆਂ(new, ਲੜੀਆਂ_off);
	fdt_ਸੈੱਟ_ਮਾਪ_dt_ਲੜੀਆਂ(new, fdt_ਮਾਪ_dt_ਲੜੀਆਂ(old));
}

ਪੂਰਨ fdt_open_into(ਅਚਲ ਖਾਲੀ *fdt, ਖਾਲੀ *buf, ਪੂਰਨ bufsize)
{
	ਪੂਰਨ err;
	ਪੂਰਨ mem_rsv_ਮਾਪ, struct_ਮਾਪ;
	ਪੂਰਨ newsize;
	ਅਚਲ ਅੱਖਰ *fdtstart = fdt;
	ਅਚਲ ਅੱਖਰ *fdtend = fdtstart + fdt_totalsize(fdt);
	ਅੱਖਰ *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_ਮਾਪ = (fdt_ਅੰਕ_mem_rsv(fdt)+1)
		* ਆਕਾਰ(ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ);

	ਜੇ (can_assume(LATEST) || fdt_ਸੰਸਕਰਣ(fdt) >= 17) {
		struct_ਮਾਪ = fdt_ਮਾਪ_dt_struct(fdt);
	} ਨਹੀਂਤਾਂ ਜੇ (fdt_ਸੰਸਕਰਣ(fdt) == 16) {
		struct_ਮਾਪ = 0;
		ਜਦੋਂ (fdt_ਅਗਲਾ_ਟੈਗ(fdt, struct_ਮਾਪ, &struct_ਮਾਪ) != FDT_ਸਿਰਾ)
			;
		ਜੇ (struct_ਮਾਪ < 0)
			ਵਾਪਸ struct_ਮਾਪ;
	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ -FDT_ERR_BADVERSION;
	}

	ਜੇ (can_assume(LIBFDT_ਕ੍ਰਮ) ||
	    !fdt_ਖੰਡਾਂ_misordered(fdt, mem_rsv_ਮਾਪ, struct_ਮਾਪ)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		ਜੇ (err)
			ਵਾਪਸ err;
		fdt_ਸੈੱਟ_ਸੰਸਕਰਣ(buf, 17);
		fdt_ਸੈੱਟ_ਮਾਪ_dt_struct(buf, struct_ਮਾਪ);
		fdt_ਸੈੱਟ_totalsize(buf, bufsize);
		ਵਾਪਸ 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(ਆਕਾਰ(ਬਣਤਰ fdt_ਸਿਰਲੇਖ), 8) + mem_rsv_ਮਾਪ
		+ struct_ਮਾਪ + fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt);

	ਜੇ (bufsize < newsize)
		ਵਾਪਸ -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	ਜੇ (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (ਅੱਖਰ *)(uintptr_t)fdtend;
		ਜੇ ((tmp + newsize) > ((ਅੱਖਰ *)buf + bufsize))
			ਵਾਪਸ -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_ਮਾਪ, struct_ਮਾਪ,
			fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt));
	memmove(buf, tmp, newsize);

	fdt_ਸੈੱਟ_magic(buf, FDT_MAGIC);
	fdt_ਸੈੱਟ_totalsize(buf, bufsize);
	fdt_ਸੈੱਟ_ਸੰਸਕਰਣ(buf, 17);
	fdt_ਸੈੱਟ_ਆਖਰੀ_comp_ਸੰਸਕਰਣ(buf, 16);
	fdt_ਸੈੱਟ_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	ਵਾਪਸ 0;
}

ਪੂਰਨ fdt_pack(ਖਾਲੀ *fdt)
{
	ਪੂਰਨ mem_rsv_ਮਾਪ;

	FDT_RW_PROBE(fdt);

	mem_rsv_ਮਾਪ = (fdt_ਅੰਕ_mem_rsv(fdt)+1)
		* ਆਕਾਰ(ਬਣਤਰ fdt_reserve_ਇੰਦਰਾਜ);
	fdt_packblocks_(fdt, fdt, mem_rsv_ਮਾਪ, fdt_ਮਾਪ_dt_struct(fdt),
			fdt_ਮਾਪ_dt_ਲੜੀਆਂ(fdt));
	fdt_ਸੈੱਟ_totalsize(fdt, fdt_ਅੰਕੜੇ_ਮਾਪ(fdt));

	ਵਾਪਸ 0;
}
