// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_blocks_misordered_(ಧ್ರುವ ಶೂನ್ಯ *fdt,
				  ಪೂರ್ಣಾಙ್ಕ mem_rsv_size, ಪೂರ್ಣಾಙ್ಕ struct_size)
{
	ಪ್ರತ್ಯಾಗಮ (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_rw_probe_(ಶೂನ್ಯ *fdt)
{
	ಯದಿ (can_assume(VALID_DTB))
		ಪ್ರತ್ಯಾಗಮ 0;
	FDT_RO_PROBE(fdt);

	ಯದಿ (!can_assume(LATEST) && fdt_version(fdt) < 17)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADVERSION;
	ಯದಿ (fdt_blocks_misordered_(fdt, ಪರಿಮಾಣ(ಸಂರಚನಾ fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADLAYOUT;
	ಯದಿ (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	ಪ್ರತ್ಯಾಗಮ 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		ಪೂರ್ಣಾಙ್ಕ err_; \
		ಯದಿ ((err_ = fdt_rw_probe_(fdt)) != 0) \
			ಪ್ರತ್ಯಾಗಮ err_; \
	}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ unsigned ಪೂರ್ಣಾಙ್ಕ fdt_data_size_(ಶೂನ್ಯ *fdt)
{
	ಪ್ರತ್ಯಾಗಮ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_splice_(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *splicepoint, ಪೂರ್ಣಾಙ್ಕ oldlen, ಪೂರ್ಣಾಙ್ಕ newlen)
{
	ವರ್ಣ *p = splicepoint;
	unsigned ಪೂರ್ಣಾಙ್ಕ dsize = fdt_data_size_(fdt);
	size_t soff = p - (ವರ್ಣ *)fdt;

	ಯದಿ ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOFFSET;
	ಯದಿ ((p < (ವರ್ಣ *)fdt) || (dsize + newlen < (unsigned)oldlen))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOFFSET;
	ಯದಿ (dsize - oldlen + newlen > fdt_totalsize(fdt))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((ವರ್ಣ *)fdt + dsize) - (p + oldlen));
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_splice_mem_rsv_(ಶೂನ್ಯ *fdt, ಸಂರಚನಾ fdt_reserve_entry *p,
			       ಪೂರ್ಣಾಙ್ಕ oldn, ಪೂರ್ಣಾಙ್ಕ newn)
{
	ಪೂರ್ಣಾಙ್ಕ delta = (newn - oldn) * ಪರಿಮಾಣ(*p);
	ಪೂರ್ಣಾಙ್ಕ err;
	err = fdt_splice_(fdt, p, oldn * ಪರಿಮಾಣ(*p), newn * ಪರಿಮಾಣ(*p));
	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_splice_struct_(ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *p,
			      ಪೂರ್ಣಾಙ್ಕ oldlen, ಪೂರ್ಣಾಙ್ಕ newlen)
{
	ಪೂರ್ಣಾಙ್ಕ delta = newlen - oldlen;
	ಪೂರ್ಣಾಙ್ಕ err;

	ಯದಿ ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		ಪ್ರತ್ಯಾಗಮ err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ಪ್ರತ್ಯಾಗಮ 0;
}

/* Must only be used to roll back in case of error */
ಸ್ಥಿರ ಶೂನ್ಯ fdt_del_last_string_(ಶೂನ್ಯ *fdt, ಧ್ರುವ ವರ್ಣ *s)
{
	ಪೂರ್ಣಾಙ್ಕ newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_splice_string_(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ newlen)
{
	ಶೂನ್ಯ *p = (ವರ್ಣ *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	ಪೂರ್ಣಾಙ್ಕ err;

	ಯದಿ ((err = fdt_splice_(fdt, p, 0, newlen)))
		ಪ್ರತ್ಯಾಗಮ err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	ಪ್ರತ್ಯಾಗಮ 0;
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_find_add_string_(ಶೂನ್ಯ *fdt, ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ *allocated)
{
	ವರ್ಣ *strtab = (ವರ್ಣ *)fdt + fdt_off_dt_strings(fdt);
	ಧ್ರುವ ವರ್ಣ *p;
	ವರ್ಣ *new;
	ಪೂರ್ಣಾಙ್ಕ len = strlen(s) + 1;
	ಪೂರ್ಣಾಙ್ಕ err;

	ಯದಿ (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	ಯದಿ (p)
		/* found it */
		ಪ್ರತ್ಯಾಗಮ (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ err;

	ಯದಿ (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	ಪ್ರತ್ಯಾಗಮ (new - strtab);
}

ಪೂರ್ಣಾಙ್ಕ fdt_add_mem_rsv(ಶೂನ್ಯ *fdt, uint64_t address, uint64_t size)
{
	ಸಂರಚನಾ fdt_reserve_entry *re;
	ಪೂರ್ಣಾಙ್ಕ err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_ಅಂಕೆ_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_del_mem_rsv(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ n)
{
	ಸಂರಚನಾ fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	ಯದಿ (n >= fdt_ಅಂಕೆ_mem_rsv(fdt))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOTFOUND;

	ಪ್ರತ್ಯಾಗಮ fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_resize_property_(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, ಧ್ರುವ ವರ್ಣ *name,
				ಪೂರ್ಣಾಙ್ಕ len, ಸಂರಚನಾ fdt_property **prop)
{
	ಪೂರ್ಣಾಙ್ಕ oldlen;
	ಪೂರ್ಣಾಙ್ಕ err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ಯದಿ (!*prop)
		ಪ್ರತ್ಯಾಗಮ oldlen;

	ಯದಿ ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		ಪ್ರತ್ಯಾಗಮ err;

	(*prop)->len = cpu_to_fdt32(len);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_add_property_(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, ಧ್ರುವ ವರ್ಣ *name,
			     ಪೂರ್ಣಾಙ್ಕ len, ಸಂರಚನಾ fdt_property **prop)
{
	ಪೂರ್ಣಾಙ್ಕ proplen;
	ಪೂರ್ಣಾಙ್ಕ nextoffset;
	ಪೂರ್ಣಾಙ್ಕ namestroff;
	ಪೂರ್ಣಾಙ್ಕ err;
	ಪೂರ್ಣಾಙ್ಕ allocated;

	ಯದಿ ((nextoffset = fdt_check_ಗಂಟು_offset(fdt, nodeoffset)) < 0)
		ಪ್ರತ್ಯಾಗಮ nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	ಯದಿ (namestroff < 0)
		ಪ್ರತ್ಯಾಗಮ namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = ಪರಿಮಾಣ(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	ಯದಿ (err) {
		/* Delete the string if we failed to add it */
		ಯದಿ (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		ಪ್ರತ್ಯಾಗಮ err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_set_name(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, ಧ್ರುವ ವರ್ಣ *name)
{
	ವರ್ಣ *namep;
	ಪೂರ್ಣಾಙ್ಕ oldlen, newlen;
	ಪೂರ್ಣಾಙ್ಕ err;

	FDT_RW_PROBE(fdt);

	namep = (ವರ್ಣ *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	ಯದಿ (!namep)
		ಪ್ರತ್ಯಾಗಮ oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ err;

	memcpy(namep, name, newlen+1);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_setprop_placeholder(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, ಧ್ರುವ ವರ್ಣ *name,
			    ಪೂರ್ಣಾಙ್ಕ len, ಶೂನ್ಯ **prop_data)
{
	ಸಂರಚನಾ fdt_property *prop;
	ಪೂರ್ಣಾಙ್ಕ err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	ಯದಿ (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ err;

	*prop_data = prop->data;
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_setprop(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, ಧ್ರುವ ವರ್ಣ *name,
		ಧ್ರುವ ಶೂನ್ಯ *val, ಪೂರ್ಣಾಙ್ಕ len)
{
	ಶೂನ್ಯ *prop_data;
	ಪೂರ್ಣಾಙ್ಕ err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ err;

	ಯದಿ (len)
		memcpy(prop_data, val, len);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_appendprop(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, ಧ್ರುವ ವರ್ಣ *name,
		   ಧ್ರುವ ಶೂನ್ಯ *val, ಪೂರ್ಣಾಙ್ಕ len)
{
	ಸಂರಚನಾ fdt_property *prop;
	ಪೂರ್ಣಾಙ್ಕ err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ಯದಿ (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		ಯದಿ (err)
			ಪ್ರತ್ಯಾಗಮ err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} ಅನ್ಯಥಾ {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		ಯದಿ (err)
			ಪ್ರತ್ಯಾಗಮ err;
		memcpy(prop->data, val, len);
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_delprop(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, ಧ್ರುವ ವರ್ಣ *name)
{
	ಸಂರಚನಾ fdt_property *prop;
	ಪೂರ್ಣಾಙ್ಕ len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ಯದಿ (!prop)
		ಪ್ರತ್ಯಾಗಮ len;

	proplen = ಪರಿಮಾಣ(*prop) + FDT_TAGALIGN(len);
	ಪ್ರತ್ಯಾಗಮ fdt_splice_struct_(fdt, prop, proplen, 0);
}

ಪೂರ್ಣಾಙ್ಕ fdt_add_subnode_namelen(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ parentoffset,
			    ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ namelen)
{
	ಸಂರಚನಾ fdt_ಗಂಟು_header *nh;
	ಪೂರ್ಣಾಙ್ಕ offset, nextoffset;
	ಪೂರ್ಣಾಙ್ಕ nodelen;
	ಪೂರ್ಣಾಙ್ಕ err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	ಯದಿ (offset >= 0)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_EXISTS;
	ಅನ್ಯಥಾ ಯದಿ (offset != -FDT_ERR_NOTFOUND)
		ಪ್ರತ್ಯಾಗಮ offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	ಯದಿ (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_ಗಂಟು))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_INTERNAL;
	ಕುರು {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} ಯಾವತ್ ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = ಪರಿಮಾಣ(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_ಗಂಟು);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((ವರ್ಣ *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_ಗಂಟು);

	ಪ್ರತ್ಯಾಗಮ offset;
}

ಪೂರ್ಣಾಙ್ಕ fdt_add_subnode(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ parentoffset, ಧ್ರುವ ವರ್ಣ *name)
{
	ಪ್ರತ್ಯಾಗಮ fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

ಪೂರ್ಣಾಙ್ಕ fdt_del_ಗಂಟು(ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset)
{
	ಪೂರ್ಣಾಙ್ಕ endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_ಗಂಟು_end_offset(fdt, nodeoffset);
	ಯದಿ (endoffset < 0)
		ಪ್ರತ್ಯಾಗಮ endoffset;

	ಪ್ರತ್ಯಾಗಮ fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

ಸ್ಥಿರ ಶೂನ್ಯ fdt_packblocks_(ಧ್ರುವ ವರ್ಣ *old, ವರ್ಣ *new,
			    ಪೂರ್ಣಾಙ್ಕ mem_rsv_size,
			    ಪೂರ್ಣಾಙ್ಕ struct_size,
			    ಪೂರ್ಣಾಙ್ಕ strings_size)
{
	ಪೂರ್ಣಾಙ್ಕ mem_rsv_off, struct_off, strings_off;

	mem_rsv_off = FDT_ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ fdt_header), 8);
	struct_off = mem_rsv_off + mem_rsv_size;
	strings_off = struct_off + struct_size;

	memmove(new + mem_rsv_off, old + fdt_off_mem_rsvmap(old), mem_rsv_size);
	fdt_set_off_mem_rsvmap(new, mem_rsv_off);

	memmove(new + struct_off, old + fdt_off_dt_struct(old), struct_size);
	fdt_set_off_dt_struct(new, struct_off);
	fdt_set_size_dt_struct(new, struct_size);

	memmove(new + strings_off, old + fdt_off_dt_strings(old), strings_size);
	fdt_set_off_dt_strings(new, strings_off);
	fdt_set_size_dt_strings(new, fdt_size_dt_strings(old));
}

ಪೂರ್ಣಾಙ್ಕ fdt_open_into(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಶೂನ್ಯ *buf, ಪೂರ್ಣಾಙ್ಕ bufsize)
{
	ಪೂರ್ಣಾಙ್ಕ err;
	ಪೂರ್ಣಾಙ್ಕ mem_rsv_size, struct_size;
	ಪೂರ್ಣಾಙ್ಕ newsize;
	ಧ್ರುವ ವರ್ಣ *fdtstart = fdt;
	ಧ್ರುವ ವರ್ಣ *fdtend = fdtstart + fdt_totalsize(fdt);
	ವರ್ಣ *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_ಅಂಕೆ_mem_rsv(fdt)+1)
		* ಪರಿಮಾಣ(ಸಂರಚನಾ fdt_reserve_entry);

	ಯದಿ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} ಅನ್ಯಥಾ ಯದಿ (fdt_version(fdt) == 16) {
		struct_size = 0;
		ಯಾವತ್ (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		ಯದಿ (struct_size < 0)
			ಪ್ರತ್ಯಾಗಮ struct_size;
	} ಅನ್ಯಥಾ {
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADVERSION;
	}

	ಯದಿ (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		ಯದಿ (err)
			ಪ್ರತ್ಯಾಗಮ err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	ಯದಿ (bufsize < newsize)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	ಯದಿ (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (ವರ್ಣ *)(uintptr_t)fdtend;
		ಯದಿ ((tmp + newsize) > ((ವರ್ಣ *)buf + bufsize))
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_pack(ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಙ್ಕ mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_ಅಂಕೆ_mem_rsv(fdt)+1)
		* ಪರಿಮಾಣ(ಸಂರಚನಾ fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	ಪ್ರತ್ಯಾಗಮ 0;
}
