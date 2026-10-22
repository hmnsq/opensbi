// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static puur_naa_nka fdt_blocks_misordered_(const void *fdt,
				  puur_naa_nka mem_rsv_size, puur_naa_nka struct_size)
{
	ပွၵ်ႈ (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

static puur_naa_nka fdt_rw_probe_(void *fdt)
{
	ပေႃး (can_assume(VALID_DTB))
		ပွၵ်ႈ 0;
	FDT_RO_PROBE(fdt);

	ပေႃး (!can_assume(LATEST) && fdt_version(fdt) < 17)
		ပွၵ်ႈ -FDT_ERR_BADVERSION;
	ပေႃး (fdt_blocks_misordered_(fdt, parimaa_na(sa_nracanaa fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		ပွၵ်ႈ -FDT_ERR_BADLAYOUT;
	ပေႃး (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	ပွၵ်ႈ 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		puur_naa_nka err_; \
		ပေႃး ((err_ = fdt_rw_probe_(fdt)) != 0) \
			ပွၵ်ႈ err_; \
	}

static anta_hstha unsigned puur_naa_nka fdt_data_size_(void *fdt)
{
	ပွၵ်ႈ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

static puur_naa_nka fdt_splice_(void *fdt, void *splicepoint, puur_naa_nka oldlen, puur_naa_nka newlen)
{
	var_na *p = splicepoint;
	unsigned puur_naa_nka dsize = fdt_data_size_(fdt);
	size_t soff = p - (var_na *)fdt;

	ပေႃး ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		ပွၵ်ႈ -FDT_ERR_BADOFFSET;
	ပေႃး ((p < (var_na *)fdt) || (dsize + newlen < (unsigned)oldlen))
		ပွၵ်ႈ -FDT_ERR_BADOFFSET;
	ပေႃး (dsize - oldlen + newlen > fdt_totalsize(fdt))
		ပွၵ်ႈ -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((var_na *)fdt + dsize) - (p + oldlen));
	ပွၵ်ႈ 0;
}

static puur_naa_nka fdt_splice_mem_rsv_(void *fdt, sa_nracanaa fdt_reserve_entry *p,
			       puur_naa_nka oldn, puur_naa_nka newn)
{
	puur_naa_nka delta = (newn - oldn) * parimaa_na(*p);
	puur_naa_nka err;
	err = fdt_splice_(fdt, p, oldn * parimaa_na(*p), newn * parimaa_na(*p));
	ပေႃး (err)
		ပွၵ်ႈ err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ပွၵ်ႈ 0;
}

static puur_naa_nka fdt_splice_struct_(void *fdt, void *p,
			      puur_naa_nka oldlen, puur_naa_nka newlen)
{
	puur_naa_nka delta = newlen - oldlen;
	puur_naa_nka err;

	ပေႃး ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		ပွၵ်ႈ err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ပွၵ်ႈ 0;
}

/* Must only be used to roll back in case of error */
static void fdt_del_last_string_(void *fdt, const var_na *s)
{
	puur_naa_nka newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

static puur_naa_nka fdt_splice_string_(void *fdt, puur_naa_nka newlen)
{
	void *p = (var_na *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	puur_naa_nka err;

	ပေႃး ((err = fdt_splice_(fdt, p, 0, newlen)))
		ပွၵ်ႈ err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	ပွၵ်ႈ 0;
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
static puur_naa_nka fdt_find_add_string_(void *fdt, const var_na *s, puur_naa_nka *allocated)
{
	var_na *strtab = (var_na *)fdt + fdt_off_dt_strings(fdt);
	const var_na *p;
	var_na *new;
	puur_naa_nka len = strlen(s) + 1;
	puur_naa_nka err;

	ပေႃး (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	ပေႃး (p)
		/* found it */
		ပွၵ်ႈ (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	ပေႃး (err)
		ပွၵ်ႈ err;

	ပေႃး (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	ပွၵ်ႈ (new - strtab);
}

puur_naa_nka fdt_add_mem_rsv(void *fdt, uint64_t address, uint64_t size)
{
	sa_nracanaa fdt_reserve_entry *re;
	puur_naa_nka err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_num_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	ပေႃး (err)
		ပွၵ်ႈ err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_del_mem_rsv(void *fdt, puur_naa_nka n)
{
	sa_nracanaa fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	ပေႃး (n >= fdt_num_mem_rsv(fdt))
		ပွၵ်ႈ -FDT_ERR_NOTFOUND;

	ပွၵ်ႈ fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

static puur_naa_nka fdt_resize_property_(void *fdt, puur_naa_nka nodeoffset, const var_na *name,
				puur_naa_nka len, sa_nracanaa fdt_property **prop)
{
	puur_naa_nka oldlen;
	puur_naa_nka err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ပေႃး (!*prop)
		ပွၵ်ႈ oldlen;

	ပေႃး ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		ပွၵ်ႈ err;

	(*prop)->len = cpu_to_fdt32(len);
	ပွၵ်ႈ 0;
}

static puur_naa_nka fdt_add_property_(void *fdt, puur_naa_nka nodeoffset, const var_na *name,
			     puur_naa_nka len, sa_nracanaa fdt_property **prop)
{
	puur_naa_nka proplen;
	puur_naa_nka nextoffset;
	puur_naa_nka namestroff;
	puur_naa_nka err;
	puur_naa_nka allocated;

	ပေႃး ((nextoffset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ပွၵ်ႈ nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	ပေႃး (namestroff < 0)
		ပွၵ်ႈ namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = parimaa_na(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	ပေႃး (err) {
		/* Delete the string if we failed to add it */
		ပေႃး (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		ပွၵ်ႈ err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_set_name(void *fdt, puur_naa_nka nodeoffset, const var_na *name)
{
	var_na *namep;
	puur_naa_nka oldlen, newlen;
	puur_naa_nka err;

	FDT_RW_PROBE(fdt);

	namep = (var_na *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	ပေႃး (!namep)
		ပွၵ်ႈ oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	ပေႃး (err)
		ပွၵ်ႈ err;

	memcpy(namep, name, newlen+1);
	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_setprop_placeholder(void *fdt, puur_naa_nka nodeoffset, const var_na *name,
			    puur_naa_nka len, void **prop_data)
{
	sa_nracanaa fdt_property *prop;
	puur_naa_nka err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	ပေႃး (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	ပေႃး (err)
		ပွၵ်ႈ err;

	*prop_data = prop->data;
	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_setprop(void *fdt, puur_naa_nka nodeoffset, const var_na *name,
		const void *val, puur_naa_nka len)
{
	void *prop_data;
	puur_naa_nka err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	ပေႃး (err)
		ပွၵ်ႈ err;

	ပေႃး (len)
		memcpy(prop_data, val, len);
	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_appendprop(void *fdt, puur_naa_nka nodeoffset, const var_na *name,
		   const void *val, puur_naa_nka len)
{
	sa_nracanaa fdt_property *prop;
	puur_naa_nka err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ပေႃး (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		ပေႃး (err)
			ပွၵ်ႈ err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} ပေႃးဢမ်ႇ {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		ပေႃး (err)
			ပွၵ်ႈ err;
		memcpy(prop->data, val, len);
	}
	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_delprop(void *fdt, puur_naa_nka nodeoffset, const var_na *name)
{
	sa_nracanaa fdt_property *prop;
	puur_naa_nka len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ပေႃး (!prop)
		ပွၵ်ႈ len;

	proplen = parimaa_na(*prop) + FDT_TAGALIGN(len);
	ပွၵ်ႈ fdt_splice_struct_(fdt, prop, proplen, 0);
}

puur_naa_nka fdt_add_subnode_namelen(void *fdt, puur_naa_nka parentoffset,
			    const var_na *name, puur_naa_nka namelen)
{
	sa_nracanaa fdt_node_header *nh;
	puur_naa_nka offset, nextoffset;
	puur_naa_nka nodelen;
	puur_naa_nka err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	ပေႃး (offset >= 0)
		ပွၵ်ႈ -FDT_ERR_EXISTS;
	ပေႃးဢမ်ႇ ပေႃး (offset != -FDT_ERR_NOTFOUND)
		ပွၵ်ႈ offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	ပေႃး (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_NODE))
		ပွၵ်ႈ -FDT_ERR_INTERNAL;
	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} ၶၢဝ်း ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = parimaa_na(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	ပေႃး (err)
		ပွၵ်ႈ err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((var_na *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_NODE);

	ပွၵ်ႈ offset;
}

puur_naa_nka fdt_add_subnode(void *fdt, puur_naa_nka parentoffset, const var_na *name)
{
	ပွၵ်ႈ fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

puur_naa_nka fdt_del_node(void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	ပေႃး (endoffset < 0)
		ပွၵ်ႈ endoffset;

	ပွၵ်ႈ fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

static void fdt_packblocks_(const var_na *old, var_na *new,
			    puur_naa_nka mem_rsv_size,
			    puur_naa_nka struct_size,
			    puur_naa_nka strings_size)
{
	puur_naa_nka mem_rsv_off, struct_off, strings_off;

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

puur_naa_nka fdt_open_into(const void *fdt, void *buf, puur_naa_nka bufsize)
{
	puur_naa_nka err;
	puur_naa_nka mem_rsv_size, struct_size;
	puur_naa_nka newsize;
	const var_na *fdtstart = fdt;
	const var_na *fdtend = fdtstart + fdt_totalsize(fdt);
	var_na *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* parimaa_na(sa_nracanaa fdt_reserve_entry);

	ပေႃး (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} ပေႃးဢမ်ႇ ပေႃး (fdt_version(fdt) == 16) {
		struct_size = 0;
		ၶၢဝ်း (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		ပေႃး (struct_size < 0)
			ပွၵ်ႈ struct_size;
	} ပေႃးဢမ်ႇ {
		ပွၵ်ႈ -FDT_ERR_BADVERSION;
	}

	ပေႃး (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		ပေႃး (err)
			ပွၵ်ႈ err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		ပွၵ်ႈ 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	ပေႃး (bufsize < newsize)
		ပွၵ်ႈ -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	ပေႃး (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (var_na *)(uintptr_t)fdtend;
		ပေႃး ((tmp + newsize) > ((var_na *)buf + bufsize))
			ပွၵ်ႈ -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	ပွၵ်ႈ 0;
}

puur_naa_nka fdt_pack(void *fdt)
{
	puur_naa_nka mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_num_mem_rsv(fdt)+1)
		* parimaa_na(sa_nracanaa fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	ပွၵ်ႈ 0;
}
