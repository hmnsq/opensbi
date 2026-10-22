// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static පූර්ණ_සංඛ්‍යා fdt_blocks_misordered_(const void *fdt,
				  පූර්ණ_සංඛ්‍යා mem_rsv_size, පූර්ණ_සංඛ්‍යා struct_size)
{
	ආපසු (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

static පූර්ණ_සංඛ්‍යා fdt_rw_probe_(void *fdt)
{
	නම් (can_assume(VALID_DTB))
		ආපසු 0;
	FDT_RO_PROBE(fdt);

	නම් (!can_assume(LATEST) && fdt_version(fdt) < 17)
		ආපසු -FDT_ERR_BADVERSION;
	නම් (fdt_blocks_misordered_(fdt, parimaa_na(sa_nracanaa fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		ආපසු -FDT_ERR_BADLAYOUT;
	නම් (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	ආපසු 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		පූර්ණ_සංඛ්‍යා err_; \
		නම් ((err_ = fdt_rw_probe_(fdt)) != 0) \
			ආපසු err_; \
	}

static anta_hstha unsigned පූර්ණ_සංඛ්‍යා fdt_data_size_(void *fdt)
{
	ආපසු fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

static පූර්ණ_සංඛ්‍යා fdt_splice_(void *fdt, void *splicepoint, පූර්ණ_සංඛ්‍යා oldlen, පූර්ණ_සංඛ්‍යා newlen)
{
	var_na *p = splicepoint;
	unsigned පූර්ණ_සංඛ්‍යා dsize = fdt_data_size_(fdt);
	size_t soff = p - (var_na *)fdt;

	නම් ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		ආපසු -FDT_ERR_BADOFFSET;
	නම් ((p < (var_na *)fdt) || (dsize + newlen < (unsigned)oldlen))
		ආපසු -FDT_ERR_BADOFFSET;
	නම් (dsize - oldlen + newlen > fdt_totalsize(fdt))
		ආපසු -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((var_na *)fdt + dsize) - (p + oldlen));
	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා fdt_splice_mem_rsv_(void *fdt, sa_nracanaa fdt_reserve_entry *p,
			       පූර්ණ_සංඛ්‍යා oldn, පූර්ණ_සංඛ්‍යා newn)
{
	පූර්ණ_සංඛ්‍යා delta = (newn - oldn) * parimaa_na(*p);
	පූර්ණ_සංඛ්‍යා err;
	err = fdt_splice_(fdt, p, oldn * parimaa_na(*p), newn * parimaa_na(*p));
	නම් (err)
		ආපසු err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා fdt_splice_struct_(void *fdt, void *p,
			      පූර්ණ_සංඛ්‍යා oldlen, පූර්ණ_සංඛ්‍යා newlen)
{
	පූර්ණ_සංඛ්‍යා delta = newlen - oldlen;
	පූර්ණ_සංඛ්‍යා err;

	නම් ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		ආපසු err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ආපසු 0;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const var_na *s)
{
	පූර්ණ_සංඛ්‍යා newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

static පූර්ණ_සංඛ්‍යා fdt_splice_string_(void *fdt, පූර්ණ_සංඛ්‍යා newlen)
{
	void *p = (var_na *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	පූර්ණ_සංඛ්‍යා err;

	නම් ((err = fdt_splice_(fdt, p, 0, newlen)))
		ආපසු err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	ආපසු 0;
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
static පූර්ණ_සංඛ්‍යා fdt_find_add_string_(void *fdt, const var_na *s, පූර්ණ_සංඛ්‍යා *allocated)
{
	var_na *strtab = (var_na *)fdt + fdt_off_dt_strings(fdt);
	const var_na *p;
	var_na *new;
	පූර්ණ_සංඛ්‍යා len = strlen(s) + 1;
	පූර්ණ_සංඛ්‍යා err;

	නම් (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	නම් (p)
		/* found it */
		ආපසු (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	නම් (err)
		ආපසු err;

	නම් (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	ආපසු (new - strtab);
}

පූර්ණ_සංඛ්‍යා fdt_add_mem_rsv(void *fdt, uint64_t address, uint64_t size)
{
	sa_nracanaa fdt_reserve_entry *re;
	පූර්ණ_සංඛ්‍යා err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_num_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	නම් (err)
		ආපසු err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_del_mem_rsv(void *fdt, පූර්ණ_සංඛ්‍යා n)
{
	sa_nracanaa fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	නම් (n >= fdt_num_mem_rsv(fdt))
		ආපසු -FDT_ERR_NOTFOUND;

	ආපසු fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

static පූර්ණ_සංඛ්‍යා fdt_resize_property_(void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset, const var_na *name,
				පූර්ණ_සංඛ්‍යා len, sa_nracanaa fdt_property **prop)
{
	පූර්ණ_සංඛ්‍යා oldlen;
	පූර්ණ_සංඛ්‍යා err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	නම් (!*prop)
		ආපසු oldlen;

	නම් ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		ආපසු err;

	(*prop)->len = cpu_to_fdt32(len);
	ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා fdt_add_property_(void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset, const var_na *name,
			     පූර්ණ_සංඛ්‍යා len, sa_nracanaa fdt_property **prop)
{
	පූර්ණ_සංඛ්‍යා proplen;
	පූර්ණ_සංඛ්‍යා nextoffset;
	පූර්ණ_සංඛ්‍යා namestroff;
	පූර්ණ_සංඛ්‍යා err;
	පූර්ණ_සංඛ්‍යා allocated;

	නම් ((nextoffset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ආපසු nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	නම් (namestroff < 0)
		ආපසු namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = parimaa_na(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	නම් (err) {
		/* Delete the string if we failed to add it */
		නම් (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		ආපසු err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_set_name(void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset, const var_na *name)
{
	var_na *namep;
	පූර්ණ_සංඛ්‍යා oldlen, newlen;
	පූර්ණ_සංඛ්‍යා err;

	FDT_RW_PROBE(fdt);

	namep = (var_na *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	නම් (!namep)
		ආපසු oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	නම් (err)
		ආපසු err;

	memcpy(namep, name, newlen+1);
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_setprop_placeholder(void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset, const var_na *name,
			    පූර්ණ_සංඛ්‍යා len, void **prop_data)
{
	sa_nracanaa fdt_property *prop;
	පූර්ණ_සංඛ්‍යා err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	නම් (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	නම් (err)
		ආපසු err;

	*prop_data = prop->data;
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_setprop(void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset, const var_na *name,
		const void *val, පූර්ණ_සංඛ්‍යා len)
{
	void *prop_data;
	පූර්ණ_සංඛ්‍යා err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	නම් (err)
		ආපසු err;

	නම් (len)
		memcpy(prop_data, val, len);
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_appendprop(void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset, const var_na *name,
		   const void *val, පූර්ණ_සංඛ්‍යා len)
{
	sa_nracanaa fdt_property *prop;
	පූර්ණ_සංඛ්‍යා err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	නම් (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		නම් (err)
			ආපසු err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} නැතහොත් {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		නම් (err)
			ආපසු err;
		memcpy(prop->data, val, len);
	}
	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_delprop(void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset, const var_na *name)
{
	sa_nracanaa fdt_property *prop;
	පූර්ණ_සංඛ්‍යා len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	නම් (!prop)
		ආපසු len;

	proplen = parimaa_na(*prop) + FDT_TAGALIGN(len);
	ආපසු fdt_splice_struct_(fdt, prop, proplen, 0);
}

පූර්ණ_සංඛ්‍යා fdt_add_subnode_namelen(void *fdt, පූර්ණ_සංඛ්‍යා parentoffset,
			    const var_na *name, පූර්ණ_සංඛ්‍යා namelen)
{
	sa_nracanaa fdt_node_header *nh;
	පූර්ණ_සංඛ්‍යා offset, nextoffset;
	පූර්ණ_සංඛ්‍යා nodelen;
	පූර්ණ_සංඛ්‍යා err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	නම් (offset >= 0)
		ආපසු -FDT_ERR_EXISTS;
	නැතහොත් නම් (offset != -FDT_ERR_NOTFOUND)
		ආපසු offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	නම් (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_NODE))
		ආපසු -FDT_ERR_INTERNAL;
	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} අතරතුර ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = parimaa_na(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	නම් (err)
		ආපසු err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((var_na *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_NODE);

	ආපසු offset;
}

පූර්ණ_සංඛ්‍යා fdt_add_subnode(void *fdt, පූර්ණ_සංඛ්‍යා parentoffset, const var_na *name)
{
	ආපසු fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

පූර්ණ_සංඛ්‍යා fdt_del_node(void *fdt, පූර්ණ_සංඛ්‍යා nodeoffset)
{
	පූර්ණ_සංඛ්‍යා endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	නම් (endoffset < 0)
		ආපසු endoffset;

	ආපසු fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

static void fdt_packblocks_(const var_na *old, var_na *new,
			    පූර්ණ_සංඛ්‍යා mem_rsv_size,
			    පූර්ණ_සංඛ්‍යා struct_size,
			    පූර්ණ_සංඛ්‍යා strings_size)
{
	පූර්ණ_සංඛ්‍යා mem_rsv_off, struct_off, strings_off;

	mem_rsv_off = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8);
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

පූර්ණ_සංඛ්‍යා fdt_open_into(const void *fdt, void *buf, පූර්ණ_සංඛ්‍යා bufsize)
{
	පූර්ණ_සංඛ්‍යා err;
	පූර්ණ_සංඛ්‍යා mem_rsv_size, struct_size;
	පූර්ණ_සංඛ්‍යා newsize;
	const var_na *fdtstart = fdt;
	const var_na *fdtend = fdtstart + fdt_totalsize(fdt);
	var_na *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* parimaa_na(sa_nracanaa fdt_reserve_entry);

	නම් (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} නැතහොත් නම් (fdt_version(fdt) == 16) {
		struct_size = 0;
		අතරතුර (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		නම් (struct_size < 0)
			ආපසු struct_size;
	} නැතහොත් {
		ආපසු -FDT_ERR_BADVERSION;
	}

	නම් (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		නම් (err)
			ආපසු err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		ආපසු 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	නම් (bufsize < newsize)
		ආපසු -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	නම් (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (var_na *)(uintptr_t)fdtend;
		නම් ((tmp + newsize) > ((var_na *)buf + bufsize))
			ආපසු -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	ආපසු 0;
}

පූර්ණ_සංඛ්‍යා fdt_pack(void *fdt)
{
	පූර්ණ_සංඛ්‍යා mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* parimaa_na(sa_nracanaa fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	ආපසු 0;
}
