// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#অন্তর্ভাব "libfdt_env.h"

#অন্তর্ভাব <fdt.h>
#অন্তর্ভাব <libfdt.h>

#অন্তর্ভাব "libfdt_internal.h"

স্থির পূর্ণসংখ্যা fdt_blocks_misordered_(ধ্রুবক শূন্য *fdt,
				  পূর্ণসংখ্যা mem_rsv_আকার, পূর্ণসংখ্যা struct_আকার)
{
	ফেরত (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(আকার(কাঠামো fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_আকার))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_আকার))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_আকার_dt_strings(fdt)));
}

স্থির পূর্ণসংখ্যা fdt_rw_probe_(শূন্য *fdt)
{
	যদি (can_assume(VALID_DTB))
		ফেরত 0;
	FDT_RO_PROBE(fdt);

	যদি (!can_assume(LATEST) && fdt_version(fdt) < 17)
		ফেরত -FDT_ERR_BADVERSION;
	যদি (fdt_blocks_misordered_(fdt, আকার(কাঠামো fdt_reserve_entry),
				   fdt_আকার_dt_struct(fdt)))
		ফেরত -FDT_ERR_BADLAYOUT;
	যদি (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	ফেরত 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		পূর্ণসংখ্যা err_; \
		যদি ((err_ = fdt_rw_probe_(fdt)) != 0) \
			ফেরত err_; \
	}

স্থির অন্তঃরেখ unsigned পূর্ণসংখ্যা fdt_তথ্য_আকার(শূন্য *fdt)
{
	ফেরত fdt_off_dt_strings(fdt) + fdt_আকার_dt_strings(fdt);
}

স্থির পূর্ণসংখ্যা fdt_splice_(শূন্য *fdt, শূন্য *splicepoint, পূর্ণসংখ্যা oldlen, পূর্ণসংখ্যা newlen)
{
	অক্ষর *p = splicepoint;
	unsigned পূর্ণসংখ্যা dsize = fdt_তথ্য_আকার(fdt);
	size_t soff = p - (অক্ষর *)fdt;

	যদি ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		ফেরত -FDT_ERR_BADOFFSET;
	যদি ((p < (অক্ষর *)fdt) || (dsize + newlen < (unsigned)oldlen))
		ফেরত -FDT_ERR_BADOFFSET;
	যদি (dsize - oldlen + newlen > fdt_totalsize(fdt))
		ফেরত -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((অক্ষর *)fdt + dsize) - (p + oldlen));
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা fdt_splice_mem_rsv_(শূন্য *fdt, কাঠামো fdt_reserve_entry *p,
			       পূর্ণসংখ্যা oldn, পূর্ণসংখ্যা newn)
{
	পূর্ণসংখ্যা delta = (newn - oldn) * আকার(*p);
	পূর্ণসংখ্যা err;
	err = fdt_splice_(fdt, p, oldn * আকার(*p), newn * আকার(*p));
	যদি (err)
		ফেরত err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা fdt_splice_struct_(শূন্য *fdt, শূন্য *p,
			      পূর্ণসংখ্যা oldlen, পূর্ণসংখ্যা newlen)
{
	পূর্ণসংখ্যা delta = newlen - oldlen;
	পূর্ণসংখ্যা err;

	যদি ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		ফেরত err;

	fdt_set_আকার_dt_struct(fdt, fdt_আকার_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ফেরত 0;
}

/* Must only be used to roll back in case of error */
স্থির শূন্য fdt_del_last_string_(শূন্য *fdt, ধ্রুবক অক্ষর *s)
{
	পূর্ণসংখ্যা newlen = strlen(s) + 1;

	fdt_set_আকার_dt_strings(fdt, fdt_আকার_dt_strings(fdt) - newlen);
}

স্থির পূর্ণসংখ্যা fdt_splice_string_(শূন্য *fdt, পূর্ণসংখ্যা newlen)
{
	শূন্য *p = (অক্ষর *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_আকার_dt_strings(fdt);
	পূর্ণসংখ্যা err;

	যদি ((err = fdt_splice_(fdt, p, 0, newlen)))
		ফেরত err;

	fdt_set_আকার_dt_strings(fdt, fdt_আকার_dt_strings(fdt) + newlen);
	ফেরত 0;
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
স্থির পূর্ণসংখ্যা fdt_find_add_string_(শূন্য *fdt, ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা *allocated)
{
	অক্ষর *strtab = (অক্ষর *)fdt + fdt_off_dt_strings(fdt);
	ধ্রুবক অক্ষর *p;
	অক্ষর *new;
	পূর্ণসংখ্যা len = strlen(s) + 1;
	পূর্ণসংখ্যা err;

	যদি (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_আকার_dt_strings(fdt), s);
	যদি (p)
		/* found it */
		ফেরত (p - strtab);

	new = strtab + fdt_আকার_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	যদি (err)
		ফেরত err;

	যদি (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	ফেরত (new - strtab);
}

পূর্ণসংখ্যা fdt_add_mem_rsv(শূন্য *fdt, uint64_t address, uint64_t আকার_2)
{
	কাঠামো fdt_reserve_entry *re;
	পূর্ণসংখ্যা err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_অঙ্ক_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	যদি (err)
		ফেরত err;

	re->address = cpu_to_fdt64(address);
	re->আকার_2 = cpu_to_fdt64(আকার_2);
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_del_mem_rsv(শূন্য *fdt, পূর্ণসংখ্যা n)
{
	কাঠামো fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	যদি (n >= fdt_অঙ্ক_mem_rsv(fdt))
		ফেরত -FDT_ERR_NOTFOUND;

	ফেরত fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

স্থির পূর্ণসংখ্যা fdt_resize_property_(শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, ধ্রুবক অক্ষর *name,
				পূর্ণসংখ্যা len, কাঠামো fdt_property **prop)
{
	পূর্ণসংখ্যা oldlen;
	পূর্ণসংখ্যা err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	যদি (!*prop)
		ফেরত oldlen;

	যদি ((err = fdt_splice_struct_(fdt, (*prop)->তথ্য, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		ফেরত err;

	(*prop)->len = cpu_to_fdt32(len);
	ফেরত 0;
}

স্থির পূর্ণসংখ্যা fdt_add_property_(শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, ধ্রুবক অক্ষর *name,
			     পূর্ণসংখ্যা len, কাঠামো fdt_property **prop)
{
	পূর্ণসংখ্যা proplen;
	পূর্ণসংখ্যা nextoffset;
	পূর্ণসংখ্যা namestroff;
	পূর্ণসংখ্যা err;
	পূর্ণসংখ্যা allocated;

	যদি ((nextoffset = fdt_check_গ্রন্থি_offset(fdt, nodeoffset)) < 0)
		ফেরত nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	যদি (namestroff < 0)
		ফেরত namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = আকার(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	যদি (err) {
		/* Delete the string if we failed to add it */
		যদি (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		ফেরত err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_set_name(শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, ধ্রুবক অক্ষর *name)
{
	অক্ষর *namep;
	পূর্ণসংখ্যা oldlen, newlen;
	পূর্ণসংখ্যা err;

	FDT_RW_PROBE(fdt);

	namep = (অক্ষর *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	যদি (!namep)
		ফেরত oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	যদি (err)
		ফেরত err;

	memcpy(namep, name, newlen+1);
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_setprop_placeholder(শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, ধ্রুবক অক্ষর *name,
			    পূর্ণসংখ্যা len, শূন্য **prop_তথ্য)
{
	কাঠামো fdt_property *prop;
	পূর্ণসংখ্যা err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	যদি (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	যদি (err)
		ফেরত err;

	*prop_তথ্য = prop->তথ্য;
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_setprop(শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, ধ্রুবক অক্ষর *name,
		ধ্রুবক শূন্য *val, পূর্ণসংখ্যা len)
{
	শূন্য *prop_তথ্য;
	পূর্ণসংখ্যা err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_তথ্য);
	যদি (err)
		ফেরত err;

	যদি (len)
		memcpy(prop_তথ্য, val, len);
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_appendprop(শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, ধ্রুবক অক্ষর *name,
		   ধ্রুবক শূন্য *val, পূর্ণসংখ্যা len)
{
	কাঠামো fdt_property *prop;
	পূর্ণসংখ্যা err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	যদি (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->তথ্য,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		যদি (err)
			ফেরত err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->তথ্য + oldlen, val, len);
	} অন্যথা {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		যদি (err)
			ফেরত err;
		memcpy(prop->তথ্য, val, len);
	}
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_delprop(শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, ধ্রুবক অক্ষর *name)
{
	কাঠামো fdt_property *prop;
	পূর্ণসংখ্যা len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	যদি (!prop)
		ফেরত len;

	proplen = আকার(*prop) + FDT_TAGALIGN(len);
	ফেরত fdt_splice_struct_(fdt, prop, proplen, 0);
}

পূর্ণসংখ্যা fdt_add_subnode_namelen(শূন্য *fdt, পূর্ণসংখ্যা parentoffset,
			    ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা namelen)
{
	কাঠামো fdt_গ্রন্থি_header *nh;
	পূর্ণসংখ্যা offset, nextoffset;
	পূর্ণসংখ্যা nodelen;
	পূর্ণসংখ্যা err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	যদি (offset >= 0)
		ফেরত -FDT_ERR_EXISTS;
	অন্যথা যদি (offset != -FDT_ERR_NOTFOUND)
		ফেরত offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_পরবর্তী_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	যদি (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_গ্রন্থি))
		ফেরত -FDT_ERR_INTERNAL;
	করো {
		offset = nextoffset;
		tag = fdt_পরবর্তী_tag(fdt, offset, &nextoffset);
	} যখন ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = আকার(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	যদি (err)
		ফেরত err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_গ্রন্থি);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((অক্ষর *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_গ্রন্থি);

	ফেরত offset;
}

পূর্ণসংখ্যা fdt_add_subnode(শূন্য *fdt, পূর্ণসংখ্যা parentoffset, ধ্রুবক অক্ষর *name)
{
	ফেরত fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

পূর্ণসংখ্যা fdt_del_গ্রন্থি(শূন্য *fdt, পূর্ণসংখ্যা nodeoffset)
{
	পূর্ণসংখ্যা endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_গ্রন্থি_end_offset(fdt, nodeoffset);
	যদি (endoffset < 0)
		ফেরত endoffset;

	ফেরত fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

স্থির শূন্য fdt_packblocks_(ধ্রুবক অক্ষর *old, অক্ষর *new,
			    পূর্ণসংখ্যা mem_rsv_আকার,
			    পূর্ণসংখ্যা struct_আকার,
			    পূর্ণসংখ্যা strings_আকার)
{
	পূর্ণসংখ্যা mem_rsv_off, struct_off, strings_off;

	mem_rsv_off = FDT_ALIGN(আকার(কাঠামো fdt_header), 8);
	struct_off = mem_rsv_off + mem_rsv_আকার;
	strings_off = struct_off + struct_আকার;

	memmove(new + mem_rsv_off, old + fdt_off_mem_rsvmap(old), mem_rsv_আকার);
	fdt_set_off_mem_rsvmap(new, mem_rsv_off);

	memmove(new + struct_off, old + fdt_off_dt_struct(old), struct_আকার);
	fdt_set_off_dt_struct(new, struct_off);
	fdt_set_আকার_dt_struct(new, struct_আকার);

	memmove(new + strings_off, old + fdt_off_dt_strings(old), strings_আকার);
	fdt_set_off_dt_strings(new, strings_off);
	fdt_set_আকার_dt_strings(new, fdt_আকার_dt_strings(old));
}

পূর্ণসংখ্যা fdt_open_into(ধ্রুবক শূন্য *fdt, শূন্য *buf, পূর্ণসংখ্যা bufsize)
{
	পূর্ণসংখ্যা err;
	পূর্ণসংখ্যা mem_rsv_আকার, struct_আকার;
	পূর্ণসংখ্যা newsize;
	ধ্রুবক অক্ষর *fdtstart = fdt;
	ধ্রুবক অক্ষর *fdtend = fdtstart + fdt_totalsize(fdt);
	অক্ষর *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_আকার = (fdt_অঙ্ক_mem_rsv(fdt)+1)
		* আকার(কাঠামো fdt_reserve_entry);

	যদি (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_আকার = fdt_আকার_dt_struct(fdt);
	} অন্যথা যদি (fdt_version(fdt) == 16) {
		struct_আকার = 0;
		যখন (fdt_পরবর্তী_tag(fdt, struct_আকার, &struct_আকার) != FDT_END)
			;
		যদি (struct_আকার < 0)
			ফেরত struct_আকার;
	} অন্যথা {
		ফেরত -FDT_ERR_BADVERSION;
	}

	যদি (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_আকার, struct_আকার)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		যদি (err)
			ফেরত err;
		fdt_set_version(buf, 17);
		fdt_set_আকার_dt_struct(buf, struct_আকার);
		fdt_set_totalsize(buf, bufsize);
		ফেরত 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(আকার(কাঠামো fdt_header), 8) + mem_rsv_আকার
		+ struct_আকার + fdt_আকার_dt_strings(fdt);

	যদি (bufsize < newsize)
		ফেরত -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	যদি (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (অক্ষর *)(uintptr_t)fdtend;
		যদি ((tmp + newsize) > ((অক্ষর *)buf + bufsize))
			ফেরত -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_আকার, struct_আকার,
			fdt_আকার_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_pack(শূন্য *fdt)
{
	পূর্ণসংখ্যা mem_rsv_আকার;

	FDT_RW_PROBE(fdt);

	mem_rsv_আকার = (fdt_অঙ্ক_mem_rsv(fdt)+1)
		* আকার(কাঠামো fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_আকার, fdt_আকার_dt_struct(fdt),
			fdt_আকার_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_তথ্য_আকার(fdt));

	ফেরত 0;
}
