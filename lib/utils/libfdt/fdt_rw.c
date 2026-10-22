// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

ایستا صحیح fdt_blocks_misordered_(ثابت تهی *fdt,
				  صحیح mem_rsv_size, صحیح struct_size)
{
	بازگشت (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(اندازه(ساختار fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

ایستا صحیح fdt_rw_probe_(تهی *fdt)
{
	اگر (can_assume(VALID_DTB))
		بازگشت 0;
	FDT_RO_PROBE(fdt);

	اگر (!can_assume(LATEST) && fdt_version(fdt) < 17)
		بازگشت -FDT_ERR_BADVERSION;
	اگر (fdt_blocks_misordered_(fdt, اندازه(ساختار fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		بازگشت -FDT_ERR_BADLAYOUT;
	اگر (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	بازگشت 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		صحیح err_; \
		اگر ((err_ = fdt_rw_probe_(fdt)) != 0) \
			بازگشت err_; \
	}

ایستا درونخطی unsigned صحیح fdt_data_size_(تهی *fdt)
{
	بازگشت fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

ایستا صحیح fdt_splice_(تهی *fdt, تهی *splicepoint, صحیح oldlen, صحیح newlen)
{
	نویسه *p = splicepoint;
	unsigned صحیح dsize = fdt_data_size_(fdt);
	size_t soff = p - (نویسه *)fdt;

	اگر ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		بازگشت -FDT_ERR_BADOFFSET;
	اگر ((p < (نویسه *)fdt) || (dsize + newlen < (unsigned)oldlen))
		بازگشت -FDT_ERR_BADOFFSET;
	اگر (dsize - oldlen + newlen > fdt_totalsize(fdt))
		بازگشت -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((نویسه *)fdt + dsize) - (p + oldlen));
	بازگشت 0;
}

ایستا صحیح fdt_splice_mem_rsv_(تهی *fdt, ساختار fdt_reserve_entry *p,
			       صحیح oldn, صحیح newn)
{
	صحیح delta = (newn - oldn) * اندازه(*p);
	صحیح err;
	err = fdt_splice_(fdt, p, oldn * اندازه(*p), newn * اندازه(*p));
	اگر (err)
		بازگشت err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	بازگشت 0;
}

ایستا صحیح fdt_splice_struct_(تهی *fdt, تهی *p,
			      صحیح oldlen, صحیح newlen)
{
	صحیح delta = newlen - oldlen;
	صحیح err;

	اگر ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		بازگشت err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	بازگشت 0;
}

/* Must only be used to roll back in case of error */
ایستا تهی fdt_del_last_string_(تهی *fdt, ثابت نویسه *s)
{
	صحیح newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

ایستا صحیح fdt_splice_string_(تهی *fdt, صحیح newlen)
{
	تهی *p = (نویسه *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	صحیح err;

	اگر ((err = fdt_splice_(fdt, p, 0, newlen)))
		بازگشت err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	بازگشت 0;
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
ایستا صحیح fdt_find_add_string_(تهی *fdt, ثابت نویسه *s, صحیح *allocated)
{
	نویسه *strtab = (نویسه *)fdt + fdt_off_dt_strings(fdt);
	ثابت نویسه *p;
	نویسه *new;
	صحیح len = strlen(s) + 1;
	صحیح err;

	اگر (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	اگر (p)
		/* found it */
		بازگشت (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	اگر (err)
		بازگشت err;

	اگر (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	بازگشت (new - strtab);
}

صحیح fdt_add_mem_rsv(تهی *fdt, uint64_t address, uint64_t size)
{
	ساختار fdt_reserve_entry *re;
	صحیح err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_رقم_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	اگر (err)
		بازگشت err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	بازگشت 0;
}

صحیح fdt_del_mem_rsv(تهی *fdt, صحیح n)
{
	ساختار fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	اگر (n >= fdt_رقم_mem_rsv(fdt))
		بازگشت -FDT_ERR_NOTFOUND;

	بازگشت fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

ایستا صحیح fdt_resize_property_(تهی *fdt, صحیح nodeoffset, ثابت نویسه *name,
				صحیح len, ساختار fdt_property **prop)
{
	صحیح oldlen;
	صحیح err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	اگر (!*prop)
		بازگشت oldlen;

	اگر ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		بازگشت err;

	(*prop)->len = cpu_to_fdt32(len);
	بازگشت 0;
}

ایستا صحیح fdt_add_property_(تهی *fdt, صحیح nodeoffset, ثابت نویسه *name,
			     صحیح len, ساختار fdt_property **prop)
{
	صحیح proplen;
	صحیح nextoffset;
	صحیح namestroff;
	صحیح err;
	صحیح allocated;

	اگر ((nextoffset = fdt_check_گره_offset(fdt, nodeoffset)) < 0)
		بازگشت nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	اگر (namestroff < 0)
		بازگشت namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = اندازه(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	اگر (err) {
		/* Delete the string if we failed to add it */
		اگر (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		بازگشت err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	بازگشت 0;
}

صحیح fdt_set_name(تهی *fdt, صحیح nodeoffset, ثابت نویسه *name)
{
	نویسه *namep;
	صحیح oldlen, newlen;
	صحیح err;

	FDT_RW_PROBE(fdt);

	namep = (نویسه *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	اگر (!namep)
		بازگشت oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	اگر (err)
		بازگشت err;

	memcpy(namep, name, newlen+1);
	بازگشت 0;
}

صحیح fdt_setprop_placeholder(تهی *fdt, صحیح nodeoffset, ثابت نویسه *name,
			    صحیح len, تهی **prop_data)
{
	ساختار fdt_property *prop;
	صحیح err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	اگر (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	اگر (err)
		بازگشت err;

	*prop_data = prop->data;
	بازگشت 0;
}

صحیح fdt_setprop(تهی *fdt, صحیح nodeoffset, ثابت نویسه *name,
		ثابت تهی *val, صحیح len)
{
	تهی *prop_data;
	صحیح err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	اگر (err)
		بازگشت err;

	اگر (len)
		memcpy(prop_data, val, len);
	بازگشت 0;
}

صحیح fdt_appendprop(تهی *fdt, صحیح nodeoffset, ثابت نویسه *name,
		   ثابت تهی *val, صحیح len)
{
	ساختار fdt_property *prop;
	صحیح err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	اگر (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		اگر (err)
			بازگشت err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} وگرنه {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		اگر (err)
			بازگشت err;
		memcpy(prop->data, val, len);
	}
	بازگشت 0;
}

صحیح fdt_delprop(تهی *fdt, صحیح nodeoffset, ثابت نویسه *name)
{
	ساختار fdt_property *prop;
	صحیح len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	اگر (!prop)
		بازگشت len;

	proplen = اندازه(*prop) + FDT_TAGALIGN(len);
	بازگشت fdt_splice_struct_(fdt, prop, proplen, 0);
}

صحیح fdt_add_subnode_namelen(تهی *fdt, صحیح parentoffset,
			    ثابت نویسه *name, صحیح namelen)
{
	ساختار fdt_گره_header *nh;
	صحیح offset, nextoffset;
	صحیح nodelen;
	صحیح err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	اگر (offset >= 0)
		بازگشت -FDT_ERR_EXISTS;
	وگرنه اگر (offset != -FDT_ERR_NOTFOUND)
		بازگشت offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	اگر (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_گره))
		بازگشت -FDT_ERR_INTERNAL;
	انجامبده {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} وقتی ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = اندازه(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	اگر (err)
		بازگشت err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_گره);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((نویسه *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_گره);

	بازگشت offset;
}

صحیح fdt_add_subnode(تهی *fdt, صحیح parentoffset, ثابت نویسه *name)
{
	بازگشت fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

صحیح fdt_del_گره(تهی *fdt, صحیح nodeoffset)
{
	صحیح endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_گره_end_offset(fdt, nodeoffset);
	اگر (endoffset < 0)
		بازگشت endoffset;

	بازگشت fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

ایستا تهی fdt_packblocks_(ثابت نویسه *old, نویسه *new,
			    صحیح mem_rsv_size,
			    صحیح struct_size,
			    صحیح strings_size)
{
	صحیح mem_rsv_off, struct_off, strings_off;

	mem_rsv_off = FDT_ALIGN(اندازه(ساختار fdt_header), 8);
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

صحیح fdt_open_into(ثابت تهی *fdt, تهی *buf, صحیح bufsize)
{
	صحیح err;
	صحیح mem_rsv_size, struct_size;
	صحیح newsize;
	ثابت نویسه *fdtstart = fdt;
	ثابت نویسه *fdtend = fdtstart + fdt_totalsize(fdt);
	نویسه *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_رقم_mem_rsv(fdt)+1)
		* اندازه(ساختار fdt_reserve_entry);

	اگر (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} وگرنه اگر (fdt_version(fdt) == 16) {
		struct_size = 0;
		وقتی (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		اگر (struct_size < 0)
			بازگشت struct_size;
	} وگرنه {
		بازگشت -FDT_ERR_BADVERSION;
	}

	اگر (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		اگر (err)
			بازگشت err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		بازگشت 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(اندازه(ساختار fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	اگر (bufsize < newsize)
		بازگشت -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	اگر (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (نویسه *)(uintptr_t)fdtend;
		اگر ((tmp + newsize) > ((نویسه *)buf + bufsize))
			بازگشت -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	بازگشت 0;
}

صحیح fdt_pack(تهی *fdt)
{
	صحیح mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_رقم_mem_rsv(fdt)+1)
		* اندازه(ساختار fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	بازگشت 0;
}
