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
	ᬯᬮᬶ (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

static puur_naa_nka fdt_rw_probe_(void *fdt)
{
	ᬬᬾᬦ᭄ (can_assume(VALID_DTB))
		ᬯᬮᬶ 0;
	FDT_RO_PROBE(fdt);

	ᬬᬾᬦ᭄ (!can_assume(LATEST) && fdt_version(fdt) < 17)
		ᬯᬮᬶ -FDT_ERR_BADVERSION;
	ᬬᬾᬦ᭄ (fdt_blocks_misordered_(fdt, parimaa_na(sa_nracanaa fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		ᬯᬮᬶ -FDT_ERR_BADLAYOUT;
	ᬬᬾᬦ᭄ (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	ᬯᬮᬶ 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		puur_naa_nka err_; \
		ᬬᬾᬦ᭄ ((err_ = fdt_rw_probe_(fdt)) != 0) \
			ᬯᬮᬶ err_; \
	}

static anta_hstha unsigned puur_naa_nka fdt_data_size_(void *fdt)
{
	ᬯᬮᬶ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

static puur_naa_nka fdt_splice_(void *fdt, void *splicepoint, puur_naa_nka oldlen, puur_naa_nka newlen)
{
	var_na *p = splicepoint;
	unsigned puur_naa_nka dsize = fdt_data_size_(fdt);
	size_t soff = p - (var_na *)fdt;

	ᬬᬾᬦ᭄ ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		ᬯᬮᬶ -FDT_ERR_BADOFFSET;
	ᬬᬾᬦ᭄ ((p < (var_na *)fdt) || (dsize + newlen < (unsigned)oldlen))
		ᬯᬮᬶ -FDT_ERR_BADOFFSET;
	ᬬᬾᬦ᭄ (dsize - oldlen + newlen > fdt_totalsize(fdt))
		ᬯᬮᬶ -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((var_na *)fdt + dsize) - (p + oldlen));
	ᬯᬮᬶ 0;
}

static puur_naa_nka fdt_splice_mem_rsv_(void *fdt, sa_nracanaa fdt_reserve_entry *p,
			       puur_naa_nka oldn, puur_naa_nka newn)
{
	puur_naa_nka delta = (newn - oldn) * parimaa_na(*p);
	puur_naa_nka err;
	err = fdt_splice_(fdt, p, oldn * parimaa_na(*p), newn * parimaa_na(*p));
	ᬬᬾᬦ᭄ (err)
		ᬯᬮᬶ err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ᬯᬮᬶ 0;
}

static puur_naa_nka fdt_splice_struct_(void *fdt, void *p,
			      puur_naa_nka oldlen, puur_naa_nka newlen)
{
	puur_naa_nka delta = newlen - oldlen;
	puur_naa_nka err;

	ᬬᬾᬦ᭄ ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		ᬯᬮᬶ err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ᬯᬮᬶ 0;
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

	ᬬᬾᬦ᭄ ((err = fdt_splice_(fdt, p, 0, newlen)))
		ᬯᬮᬶ err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	ᬯᬮᬶ 0;
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

	ᬬᬾᬦ᭄ (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	ᬬᬾᬦ᭄ (p)
		/* found it */
		ᬯᬮᬶ (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	ᬬᬾᬦ᭄ (err)
		ᬯᬮᬶ err;

	ᬬᬾᬦ᭄ (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	ᬯᬮᬶ (new - strtab);
}

puur_naa_nka fdt_add_mem_rsv(void *fdt, uint64_t address, uint64_t size)
{
	sa_nracanaa fdt_reserve_entry *re;
	puur_naa_nka err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_num_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	ᬬᬾᬦ᭄ (err)
		ᬯᬮᬶ err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_del_mem_rsv(void *fdt, puur_naa_nka n)
{
	sa_nracanaa fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	ᬬᬾᬦ᭄ (n >= fdt_num_mem_rsv(fdt))
		ᬯᬮᬶ -FDT_ERR_NOTFOUND;

	ᬯᬮᬶ fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

static puur_naa_nka fdt_resize_property_(void *fdt, puur_naa_nka nodeoffset, const var_na *name,
				puur_naa_nka len, sa_nracanaa fdt_property **prop)
{
	puur_naa_nka oldlen;
	puur_naa_nka err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ᬬᬾᬦ᭄ (!*prop)
		ᬯᬮᬶ oldlen;

	ᬬᬾᬦ᭄ ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		ᬯᬮᬶ err;

	(*prop)->len = cpu_to_fdt32(len);
	ᬯᬮᬶ 0;
}

static puur_naa_nka fdt_add_property_(void *fdt, puur_naa_nka nodeoffset, const var_na *name,
			     puur_naa_nka len, sa_nracanaa fdt_property **prop)
{
	puur_naa_nka proplen;
	puur_naa_nka nextoffset;
	puur_naa_nka namestroff;
	puur_naa_nka err;
	puur_naa_nka allocated;

	ᬬᬾᬦ᭄ ((nextoffset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ᬯᬮᬶ nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	ᬬᬾᬦ᭄ (namestroff < 0)
		ᬯᬮᬶ namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = parimaa_na(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	ᬬᬾᬦ᭄ (err) {
		/* Delete the string if we failed to add it */
		ᬬᬾᬦ᭄ (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		ᬯᬮᬶ err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_set_name(void *fdt, puur_naa_nka nodeoffset, const var_na *name)
{
	var_na *namep;
	puur_naa_nka oldlen, newlen;
	puur_naa_nka err;

	FDT_RW_PROBE(fdt);

	namep = (var_na *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	ᬬᬾᬦ᭄ (!namep)
		ᬯᬮᬶ oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	ᬬᬾᬦ᭄ (err)
		ᬯᬮᬶ err;

	memcpy(namep, name, newlen+1);
	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_setprop_placeholder(void *fdt, puur_naa_nka nodeoffset, const var_na *name,
			    puur_naa_nka len, void **prop_data)
{
	sa_nracanaa fdt_property *prop;
	puur_naa_nka err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	ᬬᬾᬦ᭄ (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	ᬬᬾᬦ᭄ (err)
		ᬯᬮᬶ err;

	*prop_data = prop->data;
	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_setprop(void *fdt, puur_naa_nka nodeoffset, const var_na *name,
		const void *val, puur_naa_nka len)
{
	void *prop_data;
	puur_naa_nka err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	ᬬᬾᬦ᭄ (err)
		ᬯᬮᬶ err;

	ᬬᬾᬦ᭄ (len)
		memcpy(prop_data, val, len);
	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_appendprop(void *fdt, puur_naa_nka nodeoffset, const var_na *name,
		   const void *val, puur_naa_nka len)
{
	sa_nracanaa fdt_property *prop;
	puur_naa_nka err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ᬬᬾᬦ᭄ (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		ᬬᬾᬦ᭄ (err)
			ᬯᬮᬶ err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} ᬮᬶᬬ {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		ᬬᬾᬦ᭄ (err)
			ᬯᬮᬶ err;
		memcpy(prop->data, val, len);
	}
	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_delprop(void *fdt, puur_naa_nka nodeoffset, const var_na *name)
{
	sa_nracanaa fdt_property *prop;
	puur_naa_nka len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ᬬᬾᬦ᭄ (!prop)
		ᬯᬮᬶ len;

	proplen = parimaa_na(*prop) + FDT_TAGALIGN(len);
	ᬯᬮᬶ fdt_splice_struct_(fdt, prop, proplen, 0);
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
	ᬬᬾᬦ᭄ (offset >= 0)
		ᬯᬮᬶ -FDT_ERR_EXISTS;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (offset != -FDT_ERR_NOTFOUND)
		ᬯᬮᬶ offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	ᬬᬾᬦ᭄ (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_NODE))
		ᬯᬮᬶ -FDT_ERR_INTERNAL;
	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} ᬲᭂᬤᭂᬮ ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = parimaa_na(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	ᬬᬾᬦ᭄ (err)
		ᬯᬮᬶ err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((var_na *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_NODE);

	ᬯᬮᬶ offset;
}

puur_naa_nka fdt_add_subnode(void *fdt, puur_naa_nka parentoffset, const var_na *name)
{
	ᬯᬮᬶ fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

puur_naa_nka fdt_del_node(void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_node_end_offset_(fdt, nodeoffset);
	ᬬᬾᬦ᭄ (endoffset < 0)
		ᬯᬮᬶ endoffset;

	ᬯᬮᬶ fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
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

	ᬬᬾᬦ᭄ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (fdt_version(fdt) == 16) {
		struct_size = 0;
		ᬲᭂᬤᭂᬮ (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		ᬬᬾᬦ᭄ (struct_size < 0)
			ᬯᬮᬶ struct_size;
	} ᬮᬶᬬ {
		ᬯᬮᬶ -FDT_ERR_BADVERSION;
	}

	ᬬᬾᬦ᭄ (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		ᬬᬾᬦ᭄ (err)
			ᬯᬮᬶ err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		ᬯᬮᬶ 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(parimaa_na(sa_nracanaa fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	ᬬᬾᬦ᭄ (bufsize < newsize)
		ᬯᬮᬶ -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	ᬬᬾᬦ᭄ (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (var_na *)(uintptr_t)fdtend;
		ᬬᬾᬦ᭄ ((tmp + newsize) > ((var_na *)buf + bufsize))
			ᬯᬮᬶ -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	ᬯᬮᬶ 0;
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

	ᬯᬮᬶ 0;
}
