// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#அந்தர்பாவ "libfdt_env.h"

#அந்தர்பாவ <fdt.h>
#அந்தர்பாவ <libfdt.h>

#அந்தர்பாவ "libfdt_internal.h"

நிலையான முழு fdt_blocks_misordered_(மாறிலி வெறுமை *fdt,
				  முழு mem_rsv_size, முழு struct_size)
{
	திரும்பு (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(அளவு(கட்டமைப்பு fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

நிலையான முழு fdt_rw_probe_(வெறுமை *fdt)
{
	எனில் (can_assume(VALID_DTB))
		திரும்பு 0;
	FDT_RO_PROBE(fdt);

	எனில் (!can_assume(LATEST) && fdt_version(fdt) < 17)
		திரும்பு -FDT_ERR_BADVERSION;
	எனில் (fdt_blocks_misordered_(fdt, அளவு(கட்டமைப்பு fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		திரும்பு -FDT_ERR_BADLAYOUT;
	எனில் (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	திரும்பு 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		முழு err_; \
		எனில் ((err_ = fdt_rw_probe_(fdt)) != 0) \
			திரும்பு err_; \
	}

நிலையான உள்வரி unsigned முழு fdt_data_size_(வெறுமை *fdt)
{
	திரும்பு fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

நிலையான முழு fdt_splice_(வெறுமை *fdt, வெறுமை *splicepoint, முழு oldlen, முழு newlen)
{
	எழுத்து *p = splicepoint;
	unsigned முழு dsize = fdt_data_size_(fdt);
	size_t soff = p - (எழுத்து *)fdt;

	எனில் ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		திரும்பு -FDT_ERR_BADOFFSET;
	எனில் ((p < (எழுத்து *)fdt) || (dsize + newlen < (unsigned)oldlen))
		திரும்பு -FDT_ERR_BADOFFSET;
	எனில் (dsize - oldlen + newlen > fdt_totalsize(fdt))
		திரும்பு -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((எழுத்து *)fdt + dsize) - (p + oldlen));
	திரும்பு 0;
}

நிலையான முழு fdt_splice_mem_rsv_(வெறுமை *fdt, கட்டமைப்பு fdt_reserve_entry *p,
			       முழு oldn, முழு newn)
{
	முழு delta = (newn - oldn) * அளவு(*p);
	முழு err;
	err = fdt_splice_(fdt, p, oldn * அளவு(*p), newn * அளவு(*p));
	எனில் (err)
		திரும்பு err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	திரும்பு 0;
}

நிலையான முழு fdt_splice_struct_(வெறுமை *fdt, வெறுமை *p,
			      முழு oldlen, முழு newlen)
{
	முழு delta = newlen - oldlen;
	முழு err;

	எனில் ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		திரும்பு err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	திரும்பு 0;
}

/* Must only be used to roll back in case of error */
நிலையான வெறுமை fdt_del_last_string_(வெறுமை *fdt, மாறிலி எழுத்து *s)
{
	முழு newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

நிலையான முழு fdt_splice_string_(வெறுமை *fdt, முழு newlen)
{
	வெறுமை *p = (எழுத்து *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	முழு err;

	எனில் ((err = fdt_splice_(fdt, p, 0, newlen)))
		திரும்பு err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	திரும்பு 0;
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
நிலையான முழு fdt_find_add_string_(வெறுமை *fdt, மாறிலி எழுத்து *s, முழு *allocated)
{
	எழுத்து *strtab = (எழுத்து *)fdt + fdt_off_dt_strings(fdt);
	மாறிலி எழுத்து *p;
	எழுத்து *new;
	முழு len = strlen(s) + 1;
	முழு err;

	எனில் (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	எனில் (p)
		/* found it */
		திரும்பு (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	எனில் (err)
		திரும்பு err;

	எனில் (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	திரும்பு (new - strtab);
}

முழு fdt_add_mem_rsv(வெறுமை *fdt, uint64_t address, uint64_t size)
{
	கட்டமைப்பு fdt_reserve_entry *re;
	முழு err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_இலக்கம்_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	எனில் (err)
		திரும்பு err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	திரும்பு 0;
}

முழு fdt_del_mem_rsv(வெறுமை *fdt, முழு n)
{
	கட்டமைப்பு fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	எனில் (n >= fdt_இலக்கம்_mem_rsv(fdt))
		திரும்பு -FDT_ERR_NOTFOUND;

	திரும்பு fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

நிலையான முழு fdt_resize_property_(வெறுமை *fdt, முழு nodeoffset, மாறிலி எழுத்து *name,
				முழு len, கட்டமைப்பு fdt_property **prop)
{
	முழு oldlen;
	முழு err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	எனில் (!*prop)
		திரும்பு oldlen;

	எனில் ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		திரும்பு err;

	(*prop)->len = cpu_to_fdt32(len);
	திரும்பு 0;
}

நிலையான முழு fdt_add_property_(வெறுமை *fdt, முழு nodeoffset, மாறிலி எழுத்து *name,
			     முழு len, கட்டமைப்பு fdt_property **prop)
{
	முழு proplen;
	முழு nextoffset;
	முழு namestroff;
	முழு err;
	முழு allocated;

	எனில் ((nextoffset = fdt_check_முனை_offset(fdt, nodeoffset)) < 0)
		திரும்பு nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	எனில் (namestroff < 0)
		திரும்பு namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = அளவு(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	எனில் (err) {
		/* Delete the string if we failed to add it */
		எனில் (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		திரும்பு err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	திரும்பு 0;
}

முழு fdt_set_name(வெறுமை *fdt, முழு nodeoffset, மாறிலி எழுத்து *name)
{
	எழுத்து *namep;
	முழு oldlen, newlen;
	முழு err;

	FDT_RW_PROBE(fdt);

	namep = (எழுத்து *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	எனில் (!namep)
		திரும்பு oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	எனில் (err)
		திரும்பு err;

	memcpy(namep, name, newlen+1);
	திரும்பு 0;
}

முழு fdt_setprop_placeholder(வெறுமை *fdt, முழு nodeoffset, மாறிலி எழுத்து *name,
			    முழு len, வெறுமை **prop_data)
{
	கட்டமைப்பு fdt_property *prop;
	முழு err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	எனில் (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	எனில் (err)
		திரும்பு err;

	*prop_data = prop->data;
	திரும்பு 0;
}

முழு fdt_setprop(வெறுமை *fdt, முழு nodeoffset, மாறிலி எழுத்து *name,
		மாறிலி வெறுமை *val, முழு len)
{
	வெறுமை *prop_data;
	முழு err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	எனில் (err)
		திரும்பு err;

	எனில் (len)
		memcpy(prop_data, val, len);
	திரும்பு 0;
}

முழு fdt_appendprop(வெறுமை *fdt, முழு nodeoffset, மாறிலி எழுத்து *name,
		   மாறிலி வெறுமை *val, முழு len)
{
	கட்டமைப்பு fdt_property *prop;
	முழு err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	எனில் (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		எனில் (err)
			திரும்பு err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} இல்லையெனில் {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		எனில் (err)
			திரும்பு err;
		memcpy(prop->data, val, len);
	}
	திரும்பு 0;
}

முழு fdt_delprop(வெறுமை *fdt, முழு nodeoffset, மாறிலி எழுத்து *name)
{
	கட்டமைப்பு fdt_property *prop;
	முழு len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	எனில் (!prop)
		திரும்பு len;

	proplen = அளவு(*prop) + FDT_TAGALIGN(len);
	திரும்பு fdt_splice_struct_(fdt, prop, proplen, 0);
}

முழு fdt_add_subnode_namelen(வெறுமை *fdt, முழு parentoffset,
			    மாறிலி எழுத்து *name, முழு namelen)
{
	கட்டமைப்பு fdt_முனை_header *nh;
	முழு offset, nextoffset;
	முழு nodelen;
	முழு err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	எனில் (offset >= 0)
		திரும்பு -FDT_ERR_EXISTS;
	இல்லையெனில் எனில் (offset != -FDT_ERR_NOTFOUND)
		திரும்பு offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	எனில் (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_முனை))
		திரும்பு -FDT_ERR_INTERNAL;
	செய் {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} போது ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = அளவு(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	எனில் (err)
		திரும்பு err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_முனை);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((எழுத்து *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_முனை);

	திரும்பு offset;
}

முழு fdt_add_subnode(வெறுமை *fdt, முழு parentoffset, மாறிலி எழுத்து *name)
{
	திரும்பு fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

முழு fdt_del_முனை(வெறுமை *fdt, முழு nodeoffset)
{
	முழு endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_முனை_end_offset(fdt, nodeoffset);
	எனில் (endoffset < 0)
		திரும்பு endoffset;

	திரும்பு fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

நிலையான வெறுமை fdt_packblocks_(மாறிலி எழுத்து *old, எழுத்து *new,
			    முழு mem_rsv_size,
			    முழு struct_size,
			    முழு strings_size)
{
	முழு mem_rsv_off, struct_off, strings_off;

	mem_rsv_off = FDT_ALIGN(அளவு(கட்டமைப்பு fdt_header), 8);
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

முழு fdt_open_into(மாறிலி வெறுமை *fdt, வெறுமை *buf, முழு bufsize)
{
	முழு err;
	முழு mem_rsv_size, struct_size;
	முழு newsize;
	மாறிலி எழுத்து *fdtstart = fdt;
	மாறிலி எழுத்து *fdtend = fdtstart + fdt_totalsize(fdt);
	எழுத்து *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_இலக்கம்_mem_rsv(fdt)+1)
		* அளவு(கட்டமைப்பு fdt_reserve_entry);

	எனில் (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} இல்லையெனில் எனில் (fdt_version(fdt) == 16) {
		struct_size = 0;
		போது (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		எனில் (struct_size < 0)
			திரும்பு struct_size;
	} இல்லையெனில் {
		திரும்பு -FDT_ERR_BADVERSION;
	}

	எனில் (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		எனில் (err)
			திரும்பு err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		திரும்பு 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(அளவு(கட்டமைப்பு fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	எனில் (bufsize < newsize)
		திரும்பு -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	எனில் (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (எழுத்து *)(uintptr_t)fdtend;
		எனில் ((tmp + newsize) > ((எழுத்து *)buf + bufsize))
			திரும்பு -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	திரும்பு 0;
}

முழு fdt_pack(வெறுமை *fdt)
{
	முழு mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_இலக்கம்_mem_rsv(fdt)+1)
		* அளவு(கட்டமைப்பு fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	திரும்பு 0;
}
