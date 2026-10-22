// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

ساكن صحيح fdt_blocks_misordered_(ثابت فراغ *fdt,
				  صحيح mem_rsv_size, صحيح struct_size)
{
	أرجع (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(حجم(بنية fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

ساكن صحيح fdt_rw_probe_(فراغ *fdt)
{
	𐤀𐤌 (can_assume(VALID_DTB))
		أرجع 0;
	FDT_RO_PROBE(fdt);

	𐤀𐤌 (!can_assume(LATEST) && fdt_version(fdt) < 17)
		أرجع -FDT_ERR_BADVERSION;
	𐤀𐤌 (fdt_blocks_misordered_(fdt, حجم(بنية fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		أرجع -FDT_ERR_BADLAYOUT;
	𐤀𐤌 (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	أرجع 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		صحيح err_; \
		𐤀𐤌 ((err_ = fdt_rw_probe_(fdt)) != 0) \
			أرجع err_; \
	}

ساكن مضمن unsigned صحيح fdt_data_size_(فراغ *fdt)
{
	أرجع fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

ساكن صحيح fdt_splice_(فراغ *fdt, فراغ *splicepoint, صحيح oldlen, صحيح newlen)
{
	حرف *p = splicepoint;
	unsigned صحيح dsize = fdt_data_size_(fdt);
	size_t soff = p - (حرف *)fdt;

	𐤀𐤌 ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		أرجع -FDT_ERR_BADOFFSET;
	𐤀𐤌 ((p < (حرف *)fdt) || (dsize + newlen < (unsigned)oldlen))
		أرجع -FDT_ERR_BADOFFSET;
	𐤀𐤌 (dsize - oldlen + newlen > fdt_totalsize(fdt))
		أرجع -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((حرف *)fdt + dsize) - (p + oldlen));
	أرجع 0;
}

ساكن صحيح fdt_splice_mem_rsv_(فراغ *fdt, بنية fdt_reserve_entry *p,
			       صحيح oldn, صحيح newn)
{
	صحيح delta = (newn - oldn) * حجم(*p);
	صحيح err;
	err = fdt_splice_(fdt, p, oldn * حجم(*p), newn * حجم(*p));
	𐤀𐤌 (err)
		أرجع err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	أرجع 0;
}

ساكن صحيح fdt_splice_struct_(فراغ *fdt, فراغ *p,
			      صحيح oldlen, صحيح newlen)
{
	صحيح delta = newlen - oldlen;
	صحيح err;

	𐤀𐤌 ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		أرجع err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	أرجع 0;
}

/* Must only be used to roll back in case of error */
ساكن فراغ fdt_del_last_string_(فراغ *fdt, ثابت حرف *s)
{
	صحيح newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

ساكن صحيح fdt_splice_string_(فراغ *fdt, صحيح newlen)
{
	فراغ *p = (حرف *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	صحيح err;

	𐤀𐤌 ((err = fdt_splice_(fdt, p, 0, newlen)))
		أرجع err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	أرجع 0;
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
ساكن صحيح fdt_find_add_string_(فراغ *fdt, ثابت حرف *s, صحيح *allocated)
{
	حرف *strtab = (حرف *)fdt + fdt_off_dt_strings(fdt);
	ثابت حرف *p;
	حرف *new;
	صحيح len = strlen(s) + 1;
	صحيح err;

	𐤀𐤌 (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	𐤀𐤌 (p)
		/* found it */
		أرجع (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	𐤀𐤌 (err)
		أرجع err;

	𐤀𐤌 (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	أرجع (new - strtab);
}

صحيح fdt_add_mem_rsv(فراغ *fdt, uint64_t address, uint64_t size)
{
	بنية fdt_reserve_entry *re;
	صحيح err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_num_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	𐤀𐤌 (err)
		أرجع err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	أرجع 0;
}

صحيح fdt_del_mem_rsv(فراغ *fdt, صحيح n)
{
	بنية fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	𐤀𐤌 (n >= fdt_num_mem_rsv(fdt))
		أرجع -FDT_ERR_NOTFOUND;

	أرجع fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

ساكن صحيح fdt_resize_property_(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
				صحيح len, بنية fdt_property **prop)
{
	صحيح oldlen;
	صحيح err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	𐤀𐤌 (!*prop)
		أرجع oldlen;

	𐤀𐤌 ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		أرجع err;

	(*prop)->len = cpu_to_fdt32(len);
	أرجع 0;
}

ساكن صحيح fdt_add_property_(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
			     صحيح len, بنية fdt_property **prop)
{
	صحيح proplen;
	صحيح nextoffset;
	صحيح namestroff;
	صحيح err;
	صحيح allocated;

	𐤀𐤌 ((nextoffset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		أرجع nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	𐤀𐤌 (namestroff < 0)
		أرجع namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = حجم(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	𐤀𐤌 (err) {
		/* Delete the string if we failed to add it */
		𐤀𐤌 (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		أرجع err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	أرجع 0;
}

صحيح fdt_set_name(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name)
{
	حرف *namep;
	صحيح oldlen, newlen;
	صحيح err;

	FDT_RW_PROBE(fdt);

	namep = (حرف *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	𐤀𐤌 (!namep)
		أرجع oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	𐤀𐤌 (err)
		أرجع err;

	memcpy(namep, name, newlen+1);
	أرجع 0;
}

صحيح fdt_setprop_placeholder(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
			    صحيح len, فراغ **prop_data)
{
	بنية fdt_property *prop;
	صحيح err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	𐤀𐤌 (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	𐤀𐤌 (err)
		أرجع err;

	*prop_data = prop->data;
	أرجع 0;
}

صحيح fdt_setprop(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
		ثابت فراغ *val, صحيح len)
{
	فراغ *prop_data;
	صحيح err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	𐤀𐤌 (err)
		أرجع err;

	𐤀𐤌 (len)
		memcpy(prop_data, val, len);
	أرجع 0;
}

صحيح fdt_appendprop(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name,
		   ثابت فراغ *val, صحيح len)
{
	بنية fdt_property *prop;
	صحيح err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	𐤀𐤌 (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		𐤀𐤌 (err)
			أرجع err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} وإلا {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		𐤀𐤌 (err)
			أرجع err;
		memcpy(prop->data, val, len);
	}
	أرجع 0;
}

صحيح fdt_delprop(فراغ *fdt, صحيح nodeoffset, ثابت حرف *name)
{
	بنية fdt_property *prop;
	صحيح len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	𐤀𐤌 (!prop)
		أرجع len;

	proplen = حجم(*prop) + FDT_TAGALIGN(len);
	أرجع fdt_splice_struct_(fdt, prop, proplen, 0);
}

صحيح fdt_add_subnode_namelen(فراغ *fdt, صحيح parentoffset,
			    ثابت حرف *name, صحيح namelen)
{
	بنية fdt_node_header *nh;
	صحيح offset, nextoffset;
	صحيح nodelen;
	صحيح err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	𐤀𐤌 (offset >= 0)
		أرجع -FDT_ERR_EXISTS;
	وإلا 𐤀𐤌 (offset != -FDT_ERR_NOTFOUND)
		أرجع offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	𐤀𐤌 (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_NODE))
		أرجع -FDT_ERR_INTERNAL;
	𐤐𐤏𐤋 {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} طالما ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = حجم(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	𐤀𐤌 (err)
		أرجع err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((حرف *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_NODE);

	أرجع offset;
}

صحيح fdt_add_subnode(فراغ *fdt, صحيح parentoffset, ثابت حرف *name)
{
	أرجع fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

صحيح fdt_del_node(فراغ *fdt, صحيح nodeoffset)
{
	صحيح endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	𐤀𐤌 (endoffset < 0)
		أرجع endoffset;

	أرجع fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

ساكن فراغ fdt_packblocks_(ثابت حرف *old, حرف *new,
			    صحيح mem_rsv_size,
			    صحيح struct_size,
			    صحيح strings_size)
{
	صحيح mem_rsv_off, struct_off, strings_off;

	mem_rsv_off = FDT_ALIGN(حجم(بنية fdt_header), 8);
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

صحيح fdt_open_into(ثابت فراغ *fdt, فراغ *buf, صحيح bufsize)
{
	صحيح err;
	صحيح mem_rsv_size, struct_size;
	صحيح newsize;
	ثابت حرف *fdtstart = fdt;
	ثابت حرف *fdtend = fdtstart + fdt_totalsize(fdt);
	حرف *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* حجم(بنية fdt_reserve_entry);

	𐤀𐤌 (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} وإلا 𐤀𐤌 (fdt_version(fdt) == 16) {
		struct_size = 0;
		طالما (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		𐤀𐤌 (struct_size < 0)
			أرجع struct_size;
	} وإلا {
		أرجع -FDT_ERR_BADVERSION;
	}

	𐤀𐤌 (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		𐤀𐤌 (err)
			أرجع err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		أرجع 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(حجم(بنية fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	𐤀𐤌 (bufsize < newsize)
		أرجع -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	𐤀𐤌 (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (حرف *)(uintptr_t)fdtend;
		𐤀𐤌 ((tmp + newsize) > ((حرف *)buf + bufsize))
			أرجع -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	أرجع 0;
}

صحيح fdt_pack(فراغ *fdt)
{
	صحيح mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* حجم(بنية fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	أرجع 0;
}
