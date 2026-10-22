// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#অন্তর্ভাব "libfdt_env.h"

#অন্তর্ভাব <fdt.h>
#অন্তর্ভাব <libfdt.h>

#অন্তর্ভাব "libfdt_internal.h"

স্থির পূর্ণাঙ্ক fdt_blocks_misordered_(ধ্রুব শূন্য *fdt,
				  পূর্ণাঙ্ক mem_rsv_size, পূর্ণাঙ্ক struct_size)
{
	প্রত্যাগম (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(পরিমাণ(সংরচনা fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

স্থির পূর্ণাঙ্ক fdt_rw_probe_(শূন্য *fdt)
{
	যদি (can_assume(VALID_DTB))
		প্রত্যাগম 0;
	FDT_RO_PROBE(fdt);

	যদি (!can_assume(LATEST) && fdt_version(fdt) < 17)
		প্রত্যাগম -FDT_ERR_BADVERSION;
	যদি (fdt_blocks_misordered_(fdt, পরিমাণ(সংরচনা fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		প্রত্যাগম -FDT_ERR_BADLAYOUT;
	যদি (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	প্রত্যাগম 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		পূর্ণাঙ্ক err_; \
		যদি ((err_ = fdt_rw_probe_(fdt)) != 0) \
			প্রত্যাগম err_; \
	}

স্থির অন্তঃস্থ unsigned পূর্ণাঙ্ক fdt_data_size_(শূন্য *fdt)
{
	প্রত্যাগম fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

স্থির পূর্ণাঙ্ক fdt_splice_(শূন্য *fdt, শূন্য *splicepoint, পূর্ণাঙ্ক oldlen, পূর্ণাঙ্ক newlen)
{
	বর্ণ *p = splicepoint;
	unsigned পূর্ণাঙ্ক dsize = fdt_data_size_(fdt);
	size_t soff = p - (বর্ণ *)fdt;

	যদি ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		প্রত্যাগম -FDT_ERR_BADOFFSET;
	যদি ((p < (বর্ণ *)fdt) || (dsize + newlen < (unsigned)oldlen))
		প্রত্যাগম -FDT_ERR_BADOFFSET;
	যদি (dsize - oldlen + newlen > fdt_totalsize(fdt))
		প্রত্যাগম -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((বর্ণ *)fdt + dsize) - (p + oldlen));
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক fdt_splice_mem_rsv_(শূন্য *fdt, সংরচনা fdt_reserve_entry *p,
			       পূর্ণাঙ্ক oldn, পূর্ণাঙ্ক newn)
{
	পূর্ণাঙ্ক delta = (newn - oldn) * পরিমাণ(*p);
	পূর্ণাঙ্ক err;
	err = fdt_splice_(fdt, p, oldn * পরিমাণ(*p), newn * পরিমাণ(*p));
	যদি (err)
		প্রত্যাগম err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক fdt_splice_struct_(শূন্য *fdt, শূন্য *p,
			      পূর্ণাঙ্ক oldlen, পূর্ণাঙ্ক newlen)
{
	পূর্ণাঙ্ক delta = newlen - oldlen;
	পূর্ণাঙ্ক err;

	যদি ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		প্রত্যাগম err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	প্রত্যাগম 0;
}

/* Must only be used to roll back in case of error */
স্থির শূন্য fdt_del_last_string_(শূন্য *fdt, ধ্রুব বর্ণ *s)
{
	পূর্ণাঙ্ক newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

স্থির পূর্ণাঙ্ক fdt_splice_string_(শূন্য *fdt, পূর্ণাঙ্ক newlen)
{
	শূন্য *p = (বর্ণ *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	পূর্ণাঙ্ক err;

	যদি ((err = fdt_splice_(fdt, p, 0, newlen)))
		প্রত্যাগম err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	প্রত্যাগম 0;
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
স্থির পূর্ণাঙ্ক fdt_find_add_string_(শূন্য *fdt, ধ্রুব বর্ণ *s, পূর্ণাঙ্ক *allocated)
{
	বর্ণ *strtab = (বর্ণ *)fdt + fdt_off_dt_strings(fdt);
	ধ্রুব বর্ণ *p;
	বর্ণ *new;
	পূর্ণাঙ্ক len = strlen(s) + 1;
	পূর্ণাঙ্ক err;

	যদি (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	যদি (p)
		/* found it */
		প্রত্যাগম (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	যদি (err)
		প্রত্যাগম err;

	যদি (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	প্রত্যাগম (new - strtab);
}

পূর্ণাঙ্ক fdt_add_mem_rsv(শূন্য *fdt, uint64_t address, uint64_t size)
{
	সংরচনা fdt_reserve_entry *re;
	পূর্ণাঙ্ক err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_অঙ্ক_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	যদি (err)
		প্রত্যাগম err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_del_mem_rsv(শূন্য *fdt, পূর্ণাঙ্ক n)
{
	সংরচনা fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	যদি (n >= fdt_অঙ্ক_mem_rsv(fdt))
		প্রত্যাগম -FDT_ERR_NOTFOUND;

	প্রত্যাগম fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

স্থির পূর্ণাঙ্ক fdt_resize_property_(শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, ধ্রুব বর্ণ *name,
				পূর্ণাঙ্ক len, সংরচনা fdt_property **prop)
{
	পূর্ণাঙ্ক oldlen;
	পূর্ণাঙ্ক err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	যদি (!*prop)
		প্রত্যাগম oldlen;

	যদি ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		প্রত্যাগম err;

	(*prop)->len = cpu_to_fdt32(len);
	প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক fdt_add_property_(শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, ধ্রুব বর্ণ *name,
			     পূর্ণাঙ্ক len, সংরচনা fdt_property **prop)
{
	পূর্ণাঙ্ক proplen;
	পূর্ণাঙ্ক nextoffset;
	পূর্ণাঙ্ক namestroff;
	পূর্ণাঙ্ক err;
	পূর্ণাঙ্ক allocated;

	যদি ((nextoffset = fdt_check_গ্রন্থি_offset(fdt, nodeoffset)) < 0)
		প্রত্যাগম nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	যদি (namestroff < 0)
		প্রত্যাগম namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = পরিমাণ(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	যদি (err) {
		/* Delete the string if we failed to add it */
		যদি (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		প্রত্যাগম err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_set_name(শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, ধ্রুব বর্ণ *name)
{
	বর্ণ *namep;
	পূর্ণাঙ্ক oldlen, newlen;
	পূর্ণাঙ্ক err;

	FDT_RW_PROBE(fdt);

	namep = (বর্ণ *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	যদি (!namep)
		প্রত্যাগম oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	যদি (err)
		প্রত্যাগম err;

	memcpy(namep, name, newlen+1);
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_setprop_placeholder(শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, ধ্রুব বর্ণ *name,
			    পূর্ণাঙ্ক len, শূন্য **prop_data)
{
	সংরচনা fdt_property *prop;
	পূর্ণাঙ্ক err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	যদি (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	যদি (err)
		প্রত্যাগম err;

	*prop_data = prop->data;
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_setprop(শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, ধ্রুব বর্ণ *name,
		ধ্রুব শূন্য *val, পূর্ণাঙ্ক len)
{
	শূন্য *prop_data;
	পূর্ণাঙ্ক err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	যদি (err)
		প্রত্যাগম err;

	যদি (len)
		memcpy(prop_data, val, len);
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_appendprop(শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, ধ্রুব বর্ণ *name,
		   ধ্রুব শূন্য *val, পূর্ণাঙ্ক len)
{
	সংরচনা fdt_property *prop;
	পূর্ণাঙ্ক err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	যদি (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		যদি (err)
			প্রত্যাগম err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} অন্যথা {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		যদি (err)
			প্রত্যাগম err;
		memcpy(prop->data, val, len);
	}
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_delprop(শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, ধ্রুব বর্ণ *name)
{
	সংরচনা fdt_property *prop;
	পূর্ণাঙ্ক len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	যদি (!prop)
		প্রত্যাগম len;

	proplen = পরিমাণ(*prop) + FDT_TAGALIGN(len);
	প্রত্যাগম fdt_splice_struct_(fdt, prop, proplen, 0);
}

পূর্ণাঙ্ক fdt_add_subnode_namelen(শূন্য *fdt, পূর্ণাঙ্ক parentoffset,
			    ধ্রুব বর্ণ *name, পূর্ণাঙ্ক namelen)
{
	সংরচনা fdt_গ্রন্থি_header *nh;
	পূর্ণাঙ্ক offset, nextoffset;
	পূর্ণাঙ্ক nodelen;
	পূর্ণাঙ্ক err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	যদি (offset >= 0)
		প্রত্যাগম -FDT_ERR_EXISTS;
	অন্যথা যদি (offset != -FDT_ERR_NOTFOUND)
		প্রত্যাগম offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	যদি (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_গ্রন্থি))
		প্রত্যাগম -FDT_ERR_INTERNAL;
	কুরু {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} যাবত্ ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = পরিমাণ(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	যদি (err)
		প্রত্যাগম err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_গ্রন্থি);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((বর্ণ *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_গ্রন্থি);

	প্রত্যাগম offset;
}

পূর্ণাঙ্ক fdt_add_subnode(শূন্য *fdt, পূর্ণাঙ্ক parentoffset, ধ্রুব বর্ণ *name)
{
	প্রত্যাগম fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

পূর্ণাঙ্ক fdt_del_গ্রন্থি(শূন্য *fdt, পূর্ণাঙ্ক nodeoffset)
{
	পূর্ণাঙ্ক endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_গ্রন্থি_end_offset(fdt, nodeoffset);
	যদি (endoffset < 0)
		প্রত্যাগম endoffset;

	প্রত্যাগম fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

স্থির শূন্য fdt_packblocks_(ধ্রুব বর্ণ *old, বর্ণ *new,
			    পূর্ণাঙ্ক mem_rsv_size,
			    পূর্ণাঙ্ক struct_size,
			    পূর্ণাঙ্ক strings_size)
{
	পূর্ণাঙ্ক mem_rsv_off, struct_off, strings_off;

	mem_rsv_off = FDT_ALIGN(পরিমাণ(সংরচনা fdt_header), 8);
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

পূর্ণাঙ্ক fdt_open_into(ধ্রুব শূন্য *fdt, শূন্য *buf, পূর্ণাঙ্ক bufsize)
{
	পূর্ণাঙ্ক err;
	পূর্ণাঙ্ক mem_rsv_size, struct_size;
	পূর্ণাঙ্ক newsize;
	ধ্রুব বর্ণ *fdtstart = fdt;
	ধ্রুব বর্ণ *fdtend = fdtstart + fdt_totalsize(fdt);
	বর্ণ *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_অঙ্ক_mem_rsv(fdt)+1)
		* পরিমাণ(সংরচনা fdt_reserve_entry);

	যদি (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} অন্যথা যদি (fdt_version(fdt) == 16) {
		struct_size = 0;
		যাবত্ (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		যদি (struct_size < 0)
			প্রত্যাগম struct_size;
	} অন্যথা {
		প্রত্যাগম -FDT_ERR_BADVERSION;
	}

	যদি (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		যদি (err)
			প্রত্যাগম err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		প্রত্যাগম 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(পরিমাণ(সংরচনা fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	যদি (bufsize < newsize)
		প্রত্যাগম -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	যদি (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (বর্ণ *)(uintptr_t)fdtend;
		যদি ((tmp + newsize) > ((বর্ণ *)buf + bufsize))
			প্রত্যাগম -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_pack(শূন্য *fdt)
{
	পূর্ণাঙ্ক mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_অঙ্ক_mem_rsv(fdt)+1)
		* পরিমাণ(সংরচনা fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	প্রত্যাগম 0;
}
