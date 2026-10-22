// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#અન્તર્ભાવ "libfdt_env.h"

#અન્તર્ભાવ <fdt.h>
#અન્તર્ભાવ <libfdt.h>

#અન્તર્ભાવ "libfdt_internal.h"

સ્થિર પૂર્ણાંક fdt_blocks_misordered_(અચલ શૂન્ય *fdt,
				  પૂર્ણાંક mem_rsv_size, પૂર્ણાંક struct_size)
{
	પાછા (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(કદ(સંરચના fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

સ્થિર પૂર્ણાંક fdt_rw_probe_(શૂન્ય *fdt)
{
	જો (can_assume(VALID_DTB))
		પાછા 0;
	FDT_RO_PROBE(fdt);

	જો (!can_assume(LATEST) && fdt_version(fdt) < 17)
		પાછા -FDT_ERR_BADVERSION;
	જો (fdt_blocks_misordered_(fdt, કદ(સંરચના fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		પાછા -FDT_ERR_BADLAYOUT;
	જો (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	પાછા 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		પૂર્ણાંક err_; \
		જો ((err_ = fdt_rw_probe_(fdt)) != 0) \
			પાછા err_; \
	}

સ્થિર અંતઃરેખ unsigned પૂર્ણાંક fdt_data_size_(શૂન્ય *fdt)
{
	પાછા fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

સ્થિર પૂર્ણાંક fdt_splice_(શૂન્ય *fdt, શૂન્ય *splicepoint, પૂર્ણાંક oldlen, પૂર્ણાંક newlen)
{
	અક્ષર *p = splicepoint;
	unsigned પૂર્ણાંક dsize = fdt_data_size_(fdt);
	size_t soff = p - (અક્ષર *)fdt;

	જો ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		પાછા -FDT_ERR_BADOFFSET;
	જો ((p < (અક્ષર *)fdt) || (dsize + newlen < (unsigned)oldlen))
		પાછા -FDT_ERR_BADOFFSET;
	જો (dsize - oldlen + newlen > fdt_totalsize(fdt))
		પાછા -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((અક્ષર *)fdt + dsize) - (p + oldlen));
	પાછા 0;
}

સ્થિર પૂર્ણાંક fdt_splice_mem_rsv_(શૂન્ય *fdt, સંરચના fdt_reserve_entry *p,
			       પૂર્ણાંક oldn, પૂર્ણાંક newn)
{
	પૂર્ણાંક delta = (newn - oldn) * કદ(*p);
	પૂર્ણાંક err;
	err = fdt_splice_(fdt, p, oldn * કદ(*p), newn * કદ(*p));
	જો (err)
		પાછા err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	પાછા 0;
}

સ્થિર પૂર્ણાંક fdt_splice_struct_(શૂન્ય *fdt, શૂન્ય *p,
			      પૂર્ણાંક oldlen, પૂર્ણાંક newlen)
{
	પૂર્ણાંક delta = newlen - oldlen;
	પૂર્ણાંક err;

	જો ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		પાછા err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	પાછા 0;
}

/* Must only be used to roll back in case of error */
સ્થિર શૂન્ય fdt_del_last_string_(શૂન્ય *fdt, અચલ અક્ષર *s)
{
	પૂર્ણાંક newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

સ્થિર પૂર્ણાંક fdt_splice_string_(શૂન્ય *fdt, પૂર્ણાંક newlen)
{
	શૂન્ય *p = (અક્ષર *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	પૂર્ણાંક err;

	જો ((err = fdt_splice_(fdt, p, 0, newlen)))
		પાછા err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	પાછા 0;
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
સ્થિર પૂર્ણાંક fdt_find_add_string_(શૂન્ય *fdt, અચલ અક્ષર *s, પૂર્ણાંક *allocated)
{
	અક્ષર *strtab = (અક્ષર *)fdt + fdt_off_dt_strings(fdt);
	અચલ અક્ષર *p;
	અક્ષર *new;
	પૂર્ણાંક len = strlen(s) + 1;
	પૂર્ણાંક err;

	જો (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	જો (p)
		/* found it */
		પાછા (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	જો (err)
		પાછા err;

	જો (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	પાછા (new - strtab);
}

પૂર્ણાંક fdt_add_mem_rsv(શૂન્ય *fdt, uint64_t address, uint64_t size)
{
	સંરચના fdt_reserve_entry *re;
	પૂર્ણાંક err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_અંક_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	જો (err)
		પાછા err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	પાછા 0;
}

પૂર્ણાંક fdt_del_mem_rsv(શૂન્ય *fdt, પૂર્ણાંક n)
{
	સંરચના fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	જો (n >= fdt_અંક_mem_rsv(fdt))
		પાછા -FDT_ERR_NOTFOUND;

	પાછા fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

સ્થિર પૂર્ણાંક fdt_resize_property_(શૂન્ય *fdt, પૂર્ણાંક nodeoffset, અચલ અક્ષર *name,
				પૂર્ણાંક len, સંરચના fdt_property **prop)
{
	પૂર્ણાંક oldlen;
	પૂર્ણાંક err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	જો (!*prop)
		પાછા oldlen;

	જો ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		પાછા err;

	(*prop)->len = cpu_to_fdt32(len);
	પાછા 0;
}

સ્થિર પૂર્ણાંક fdt_add_property_(શૂન્ય *fdt, પૂર્ણાંક nodeoffset, અચલ અક્ષર *name,
			     પૂર્ણાંક len, સંરચના fdt_property **prop)
{
	પૂર્ણાંક proplen;
	પૂર્ણાંક nextoffset;
	પૂર્ણાંક namestroff;
	પૂર્ણાંક err;
	પૂર્ણાંક allocated;

	જો ((nextoffset = fdt_check_ગાંઠ_offset(fdt, nodeoffset)) < 0)
		પાછા nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	જો (namestroff < 0)
		પાછા namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = કદ(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	જો (err) {
		/* Delete the string if we failed to add it */
		જો (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		પાછા err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	પાછા 0;
}

પૂર્ણાંક fdt_set_name(શૂન્ય *fdt, પૂર્ણાંક nodeoffset, અચલ અક્ષર *name)
{
	અક્ષર *namep;
	પૂર્ણાંક oldlen, newlen;
	પૂર્ણાંક err;

	FDT_RW_PROBE(fdt);

	namep = (અક્ષર *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	જો (!namep)
		પાછા oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	જો (err)
		પાછા err;

	memcpy(namep, name, newlen+1);
	પાછા 0;
}

પૂર્ણાંક fdt_setprop_placeholder(શૂન્ય *fdt, પૂર્ણાંક nodeoffset, અચલ અક્ષર *name,
			    પૂર્ણાંક len, શૂન્ય **prop_data)
{
	સંરચના fdt_property *prop;
	પૂર્ણાંક err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	જો (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	જો (err)
		પાછા err;

	*prop_data = prop->data;
	પાછા 0;
}

પૂર્ણાંક fdt_setprop(શૂન્ય *fdt, પૂર્ણાંક nodeoffset, અચલ અક્ષર *name,
		અચલ શૂન્ય *val, પૂર્ણાંક len)
{
	શૂન્ય *prop_data;
	પૂર્ણાંક err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	જો (err)
		પાછા err;

	જો (len)
		memcpy(prop_data, val, len);
	પાછા 0;
}

પૂર્ણાંક fdt_appendprop(શૂન્ય *fdt, પૂર્ણાંક nodeoffset, અચલ અક્ષર *name,
		   અચલ શૂન્ય *val, પૂર્ણાંક len)
{
	સંરચના fdt_property *prop;
	પૂર્ણાંક err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	જો (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		જો (err)
			પાછા err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} નહીંતર {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		જો (err)
			પાછા err;
		memcpy(prop->data, val, len);
	}
	પાછા 0;
}

પૂર્ણાંક fdt_delprop(શૂન્ય *fdt, પૂર્ણાંક nodeoffset, અચલ અક્ષર *name)
{
	સંરચના fdt_property *prop;
	પૂર્ણાંક len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	જો (!prop)
		પાછા len;

	proplen = કદ(*prop) + FDT_TAGALIGN(len);
	પાછા fdt_splice_struct_(fdt, prop, proplen, 0);
}

પૂર્ણાંક fdt_add_subnode_namelen(શૂન્ય *fdt, પૂર્ણાંક parentoffset,
			    અચલ અક્ષર *name, પૂર્ણાંક namelen)
{
	સંરચના fdt_ગાંઠ_header *nh;
	પૂર્ણાંક offset, nextoffset;
	પૂર્ણાંક nodelen;
	પૂર્ણાંક err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	જો (offset >= 0)
		પાછા -FDT_ERR_EXISTS;
	નહીંતર જો (offset != -FDT_ERR_NOTFOUND)
		પાછા offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	જો (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_ગાંઠ))
		પાછા -FDT_ERR_INTERNAL;
	કરો {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} જ્યારે ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = કદ(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	જો (err)
		પાછા err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_ગાંઠ);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((અક્ષર *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_ગાંઠ);

	પાછા offset;
}

પૂર્ણાંક fdt_add_subnode(શૂન્ય *fdt, પૂર્ણાંક parentoffset, અચલ અક્ષર *name)
{
	પાછા fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

પૂર્ણાંક fdt_del_ગાંઠ(શૂન્ય *fdt, પૂર્ણાંક nodeoffset)
{
	પૂર્ણાંક endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_ગાંઠ_end_offset(fdt, nodeoffset);
	જો (endoffset < 0)
		પાછા endoffset;

	પાછા fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

સ્થિર શૂન્ય fdt_packblocks_(અચલ અક્ષર *old, અક્ષર *new,
			    પૂર્ણાંક mem_rsv_size,
			    પૂર્ણાંક struct_size,
			    પૂર્ણાંક strings_size)
{
	પૂર્ણાંક mem_rsv_off, struct_off, strings_off;

	mem_rsv_off = FDT_ALIGN(કદ(સંરચના fdt_header), 8);
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

પૂર્ણાંક fdt_open_into(અચલ શૂન્ય *fdt, શૂન્ય *buf, પૂર્ણાંક bufsize)
{
	પૂર્ણાંક err;
	પૂર્ણાંક mem_rsv_size, struct_size;
	પૂર્ણાંક newsize;
	અચલ અક્ષર *fdtstart = fdt;
	અચલ અક્ષર *fdtend = fdtstart + fdt_totalsize(fdt);
	અક્ષર *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_અંક_mem_rsv(fdt)+1)
		* કદ(સંરચના fdt_reserve_entry);

	જો (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} નહીંતર જો (fdt_version(fdt) == 16) {
		struct_size = 0;
		જ્યારે (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		જો (struct_size < 0)
			પાછા struct_size;
	} નહીંતર {
		પાછા -FDT_ERR_BADVERSION;
	}

	જો (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		જો (err)
			પાછા err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		પાછા 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(કદ(સંરચના fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	જો (bufsize < newsize)
		પાછા -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	જો (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (અક્ષર *)(uintptr_t)fdtend;
		જો ((tmp + newsize) > ((અક્ષર *)buf + bufsize))
			પાછા -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	પાછા 0;
}

પૂર્ણાંક fdt_pack(શૂન્ય *fdt)
{
	પૂર્ણાંક mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_અંક_mem_rsv(fdt)+1)
		* કદ(સંરચના fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	પાછા 0;
}
