// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ଅନ୍ତର୍ଭାଵ "libfdt_env.h"

#ଅନ୍ତର୍ଭାଵ <fdt.h>
#ଅନ୍ତର୍ଭାଵ <libfdt.h>

#ଅନ୍ତର୍ଭାଵ "libfdt_internal.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_blocks_misordered_(ଅଚଳ ଶୂନ୍ୟ *fdt,
				  ପୂର୍ଣ୍ଣସଂଖ୍ୟା mem_rsv_size, ପୂର୍ଣ୍ଣସଂଖ୍ୟା struct_size)
{
	ଫେରନ୍ତୁ (fdt_off_mem_rsvmap(fdt) < FDT_ALIGN(ଆକାର(ସଂରଚନା fdt_header), 8))
		|| (fdt_off_dt_struct(fdt) <
		    (fdt_off_mem_rsvmap(fdt) + mem_rsv_size))
		|| (fdt_off_dt_strings(fdt) <
		    (fdt_off_dt_struct(fdt) + struct_size))
		|| (fdt_totalsize(fdt) <
		    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_rw_probe_(ଶୂନ୍ୟ *fdt)
{
	ଯଦି (can_assume(VALID_DTB))
		ଫେରନ୍ତୁ 0;
	FDT_RO_PROBE(fdt);

	ଯଦି (!can_assume(LATEST) && fdt_version(fdt) < 17)
		ଫେରନ୍ତୁ -FDT_ERR_BADVERSION;
	ଯଦି (fdt_blocks_misordered_(fdt, ଆକାର(ସଂରଚନା fdt_reserve_entry),
				   fdt_size_dt_struct(fdt)))
		ଫେରନ୍ତୁ -FDT_ERR_BADLAYOUT;
	ଯଦି (!can_assume(LATEST) && fdt_version(fdt) > 17)
		fdt_set_version(fdt, 17);

	ଫେରନ୍ତୁ 0;
}

#define FDT_RW_PROBE(fdt) \
	{ \
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା err_; \
		ଯଦି ((err_ = fdt_rw_probe_(fdt)) != 0) \
			ଫେରନ୍ତୁ err_; \
	}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_data_size_(ଶୂନ୍ୟ *fdt)
{
	ଫେରନ୍ତୁ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_splice_(ଶୂନ୍ୟ *fdt, ଶୂନ୍ୟ *splicepoint, ପୂର୍ଣ୍ଣସଂଖ୍ୟା oldlen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newlen)
{
	ଅକ୍ଷର *p = splicepoint;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା dsize = fdt_data_size_(fdt);
	size_t soff = p - (ଅକ୍ଷର *)fdt;

	ଯଦି ((oldlen < 0) || (soff + oldlen < soff) || (soff + oldlen > dsize))
		ଫେରନ୍ତୁ -FDT_ERR_BADOFFSET;
	ଯଦି ((p < (ଅକ୍ଷର *)fdt) || (dsize + newlen < (unsigned)oldlen))
		ଫେରନ୍ତୁ -FDT_ERR_BADOFFSET;
	ଯଦି (dsize - oldlen + newlen > fdt_totalsize(fdt))
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, ((ଅକ୍ଷର *)fdt + dsize) - (p + oldlen));
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_splice_mem_rsv_(ଶୂନ୍ୟ *fdt, ସଂରଚନା fdt_reserve_entry *p,
			       ପୂର୍ଣ୍ଣସଂଖ୍ୟା oldn, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newn)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା delta = (newn - oldn) * ଆକାର(*p);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
	err = fdt_splice_(fdt, p, oldn * ଆକାର(*p), newn * ଆକାର(*p));
	ଯଦି (err)
		ଫେରନ୍ତୁ err;
	fdt_set_off_dt_struct(fdt, fdt_off_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_splice_struct_(ଶୂନ୍ୟ *fdt, ଶୂନ୍ୟ *p,
			      ପୂର୍ଣ୍ଣସଂଖ୍ୟା oldlen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newlen)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା delta = newlen - oldlen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

	ଯଦି ((err = fdt_splice_(fdt, p, oldlen, newlen)))
		ଫେରନ୍ତୁ err;

	fdt_set_size_dt_struct(fdt, fdt_size_dt_struct(fdt) + delta);
	fdt_set_off_dt_strings(fdt, fdt_off_dt_strings(fdt) + delta);
	ଫେରନ୍ତୁ 0;
}

/* Must only be used to roll back in case of error */
ସ୍ଥିର ଶୂନ୍ୟ fdt_del_last_string_(ଶୂନ୍ୟ *fdt, ଅଚଳ ଅକ୍ଷର *s)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା newlen = strlen(s) + 1;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) - newlen);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_splice_string_(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newlen)
{
	ଶୂନ୍ୟ *p = (ଅକ୍ଷର *)fdt
		+ fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

	ଯଦି ((err = fdt_splice_(fdt, p, 0, newlen)))
		ଫେରନ୍ତୁ err;

	fdt_set_size_dt_strings(fdt, fdt_size_dt_strings(fdt) + newlen);
	ଫେରନ୍ତୁ 0;
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_find_add_string_(ଶୂନ୍ୟ *fdt, ଅଚଳ ଅକ୍ଷର *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *allocated)
{
	ଅକ୍ଷର *strtab = (ଅକ୍ଷର *)fdt + fdt_off_dt_strings(fdt);
	ଅଚଳ ଅକ୍ଷର *p;
	ଅକ୍ଷର *new;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = strlen(s) + 1;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

	ଯଦି (!can_assume(NO_ROLLBACK))
		*allocated = 0;

	p = fdt_find_string_(strtab, fdt_size_dt_strings(fdt), s);
	ଯଦି (p)
		/* found it */
		ଫେରନ୍ତୁ (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = fdt_splice_string_(fdt, len);
	ଯଦି (err)
		ଫେରନ୍ତୁ err;

	ଯଦି (!can_assume(NO_ROLLBACK))
		*allocated = 1;

	memcpy(new, s, len);
	ଫେରନ୍ତୁ (new - strtab);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_add_mem_rsv(ଶୂନ୍ୟ *fdt, uint64_t address, uint64_t size)
{
	ସଂରଚନା fdt_reserve_entry *re;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

	FDT_RW_PROBE(fdt);

	re = fdt_mem_rsv_w_(fdt, fdt_ଅଙ୍କ_mem_rsv(fdt));
	err = fdt_splice_mem_rsv_(fdt, re, 0, 1);
	ଯଦି (err)
		ଫେରନ୍ତୁ err;

	re->address = cpu_to_fdt64(address);
	re->size = cpu_to_fdt64(size);
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_del_mem_rsv(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
	ସଂରଚନା fdt_reserve_entry *re = fdt_mem_rsv_w_(fdt, n);

	FDT_RW_PROBE(fdt);

	ଯଦି (n >= fdt_ଅଙ୍କ_mem_rsv(fdt))
		ଫେରନ୍ତୁ -FDT_ERR_NOTFOUND;

	ଫେରନ୍ତୁ fdt_splice_mem_rsv_(fdt, re, 1, 0);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_resize_property_(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, ଅଚଳ ଅକ୍ଷର *name,
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ସଂରଚନା fdt_property **prop)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା oldlen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

	*prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ଯଦି (!*prop)
		ଫେରନ୍ତୁ oldlen;

	ଯଦି ((err = fdt_splice_struct_(fdt, (*prop)->data, FDT_TAGALIGN(oldlen),
				      FDT_TAGALIGN(len))))
		ଫେରନ୍ତୁ err;

	(*prop)->len = cpu_to_fdt32(len);
	ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_add_property_(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, ଅଚଳ ଅକ୍ଷର *name,
			     ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ସଂରଚନା fdt_property **prop)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା proplen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nextoffset;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା namestroff;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା allocated;

	ଯଦି ((nextoffset = fdt_check_ଗଣ୍ଠି_offset(fdt, nodeoffset)) < 0)
		ଫେରନ୍ତୁ nextoffset;

	namestroff = fdt_find_add_string_(fdt, name, &allocated);
	ଯଦି (namestroff < 0)
		ଫେରନ୍ତୁ namestroff;

	*prop = fdt_offset_ptr_w_(fdt, nextoffset);
	proplen = ଆକାର(**prop) + FDT_TAGALIGN(len);

	err = fdt_splice_struct_(fdt, *prop, 0, proplen);
	ଯଦି (err) {
		/* Delete the string if we failed to add it */
		ଯଦି (!can_assume(NO_ROLLBACK) && allocated)
			fdt_del_last_string_(fdt, name);
		ଫେରନ୍ତୁ err;
	}

	(*prop)->tag = cpu_to_fdt32(FDT_PROP);
	(*prop)->nameoff = cpu_to_fdt32(namestroff);
	(*prop)->len = cpu_to_fdt32(len);
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_set_name(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, ଅଚଳ ଅକ୍ଷର *name)
{
	ଅକ୍ଷର *namep;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା oldlen, newlen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

	FDT_RW_PROBE(fdt);

	namep = (ଅକ୍ଷର *)(uintptr_t)fdt_get_name(fdt, nodeoffset, &oldlen);
	ଯଦି (!namep)
		ଫେରନ୍ତୁ oldlen;

	newlen = strlen(name);

	err = fdt_splice_struct_(fdt, namep, FDT_TAGALIGN(oldlen+1),
				 FDT_TAGALIGN(newlen+1));
	ଯଦି (err)
		ଫେରନ୍ତୁ err;

	memcpy(namep, name, newlen+1);
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_setprop_placeholder(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, ଅଚଳ ଅକ୍ଷର *name,
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ଶୂନ୍ୟ **prop_data)
{
	ସଂରଚନା fdt_property *prop;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

	FDT_RW_PROBE(fdt);

	err = fdt_resize_property_(fdt, nodeoffset, name, len, &prop);
	ଯଦି (err == -FDT_ERR_NOTFOUND)
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
	ଯଦି (err)
		ଫେରନ୍ତୁ err;

	*prop_data = prop->data;
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_setprop(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, ଅଚଳ ଅକ୍ଷର *name,
		ଅଚଳ ଶୂନ୍ୟ *val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	ଶୂନ୍ୟ *prop_data;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

	err = fdt_setprop_placeholder(fdt, nodeoffset, name, len, &prop_data);
	ଯଦି (err)
		ଫେରନ୍ତୁ err;

	ଯଦି (len)
		memcpy(prop_data, val, len);
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_appendprop(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, ଅଚଳ ଅକ୍ଷର *name,
		   ଅଚଳ ଶୂନ୍ୟ *val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
	ସଂରଚନା fdt_property *prop;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err, oldlen, newlen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &oldlen);
	ଯଦି (prop) {
		newlen = len + oldlen;
		err = fdt_splice_struct_(fdt, prop->data,
					 FDT_TAGALIGN(oldlen),
					 FDT_TAGALIGN(newlen));
		ଯଦି (err)
			ଫେରନ୍ତୁ err;
		prop->len = cpu_to_fdt32(newlen);
		memcpy(prop->data + oldlen, val, len);
	} ଅନ୍ୟଥା {
		err = fdt_add_property_(fdt, nodeoffset, name, len, &prop);
		ଯଦି (err)
			ଫେରନ୍ତୁ err;
		memcpy(prop->data, val, len);
	}
	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_delprop(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset, ଅଚଳ ଅକ୍ଷର *name)
{
	ସଂରଚନା fdt_property *prop;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, proplen;

	FDT_RW_PROBE(fdt);

	prop = fdt_get_property_w(fdt, nodeoffset, name, &len);
	ଯଦି (!prop)
		ଫେରନ୍ତୁ len;

	proplen = ଆକାର(*prop) + FDT_TAGALIGN(len);
	ଫେରନ୍ତୁ fdt_splice_struct_(fdt, prop, proplen, 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_add_subnode_namelen(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା parentoffset,
			    ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା namelen)
{
	ସଂରଚନା fdt_ଗଣ୍ଠି_header *nh;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, nextoffset;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodelen;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
	uint32_t tag;
	fdt32_t *endtag;

	FDT_RW_PROBE(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	ଯଦି (offset >= 0)
		ଫେରନ୍ତୁ -FDT_ERR_EXISTS;
	ଅନ୍ୟଥା ଯଦି (offset != -FDT_ERR_NOTFOUND)
		ଫେରନ୍ତୁ offset;

	/* Try to place the new node after the parent's properties */
	tag = fdt_next_tag(fdt, parentoffset, &nextoffset);
	/* the fdt_subnode_offset_namelen() should ensure this never hits */
	ଯଦି (!can_assume(LIBFDT_FLAWLESS) && (tag != FDT_BEGIN_ଗଣ୍ଠି))
		ଫେରନ୍ତୁ -FDT_ERR_INTERNAL;
	କର {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);
	} ଯେତେବେଳେ ((tag == FDT_PROP) || (tag == FDT_NOP));

	nh = fdt_offset_ptr_w_(fdt, offset);
	nodelen = ଆକାର(*nh) + FDT_TAGALIGN(namelen+1) + FDT_TAGSIZE;

	err = fdt_splice_struct_(fdt, nh, 0, nodelen);
	ଯଦି (err)
		ଫେରନ୍ତୁ err;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_ଗଣ୍ଠି);
	memset(nh->name, 0, FDT_TAGALIGN(namelen+1));
	memcpy(nh->name, name, namelen);
	endtag = (fdt32_t *)((ଅକ୍ଷର *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_ଗଣ୍ଠି);

	ଫେରନ୍ତୁ offset;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_add_subnode(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା parentoffset, ଅଚଳ ଅକ୍ଷର *name)
{
	ଫେରନ୍ତୁ fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_del_ଗଣ୍ଠି(ଶୂନ୍ୟ *fdt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nodeoffset)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା endoffset;

	FDT_RW_PROBE(fdt);

	endoffset = fdt_ଗଣ୍ଠି_end_offset(fdt, nodeoffset);
	ଯଦି (endoffset < 0)
		ଫେରନ୍ତୁ endoffset;

	ଫେରନ୍ତୁ fdt_splice_struct_(fdt, fdt_offset_ptr_w_(fdt, nodeoffset),
				  endoffset - nodeoffset, 0);
}

ସ୍ଥିର ଶୂନ୍ୟ fdt_packblocks_(ଅଚଳ ଅକ୍ଷର *old, ଅକ୍ଷର *new,
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mem_rsv_size,
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା struct_size,
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା strings_size)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା mem_rsv_off, struct_off, strings_off;

	mem_rsv_off = FDT_ALIGN(ଆକାର(ସଂରଚନା fdt_header), 8);
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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_open_into(ଅଚଳ ଶୂନ୍ୟ *fdt, ଶୂନ୍ୟ *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufsize)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା mem_rsv_size, struct_size;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା newsize;
	ଅଚଳ ଅକ୍ଷର *fdtstart = fdt;
	ଅଚଳ ଅକ୍ଷର *fdtend = fdtstart + fdt_totalsize(fdt);
	ଅକ୍ଷର *tmp;

	FDT_RO_PROBE(fdt);

	mem_rsv_size = (fdt_ଅଙ୍କ_mem_rsv(fdt)+1)
		* ଆକାର(ସଂରଚନା fdt_reserve_entry);

	ଯଦି (can_assume(LATEST) || fdt_version(fdt) >= 17) {
		struct_size = fdt_size_dt_struct(fdt);
	} ଅନ୍ୟଥା ଯଦି (fdt_version(fdt) == 16) {
		struct_size = 0;
		ଯେତେବେଳେ (fdt_next_tag(fdt, struct_size, &struct_size) != FDT_END)
			;
		ଯଦି (struct_size < 0)
			ଫେରନ୍ତୁ struct_size;
	} ଅନ୍ୟଥା {
		ଫେରନ୍ତୁ -FDT_ERR_BADVERSION;
	}

	ଯଦି (can_assume(LIBFDT_ORDER) ||
	    !fdt_blocks_misordered_(fdt, mem_rsv_size, struct_size)) {
		/* no further work necessary */
		err = fdt_move(fdt, buf, bufsize);
		ଯଦି (err)
			ଫେରନ୍ତୁ err;
		fdt_set_version(buf, 17);
		fdt_set_size_dt_struct(buf, struct_size);
		fdt_set_totalsize(buf, bufsize);
		ଫେରନ୍ତୁ 0;
	}

	/* Need to reorder */
	newsize = FDT_ALIGN(ଆକାର(ସଂରଚନା fdt_header), 8) + mem_rsv_size
		+ struct_size + fdt_size_dt_strings(fdt);

	ଯଦି (bufsize < newsize)
		ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;

	/* First attempt to build converted tree at beginning of buffer */
	tmp = buf;
	/* But if that overlaps with the old tree... */
	ଯଦି (((tmp + newsize) > fdtstart) && (tmp < fdtend)) {
		/* Try right after the old tree instead */
		tmp = (ଅକ୍ଷର *)(uintptr_t)fdtend;
		ଯଦି ((tmp + newsize) > ((ଅକ୍ଷର *)buf + bufsize))
			ଫେରନ୍ତୁ -FDT_ERR_NOSPACE;
	}

	fdt_packblocks_(fdt, tmp, mem_rsv_size, struct_size,
			fdt_size_dt_strings(fdt));
	memmove(buf, tmp, newsize);

	fdt_set_magic(buf, FDT_MAGIC);
	fdt_set_totalsize(buf, bufsize);
	fdt_set_version(buf, 17);
	fdt_set_last_comp_version(buf, 16);
	fdt_set_boot_cpuid_phys(buf, fdt_boot_cpuid_phys(fdt));

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdt_pack(ଶୂନ୍ୟ *fdt)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା mem_rsv_size;

	FDT_RW_PROBE(fdt);

	mem_rsv_size = (fdt_ଅଙ୍କ_mem_rsv(fdt)+1)
		* ଆକାର(ସଂରଚନା fdt_reserve_entry);
	fdt_packblocks_(fdt, fdt, mem_rsv_size, fdt_size_dt_struct(fdt),
			fdt_size_dt_strings(fdt));
	fdt_set_totalsize(fdt, fdt_data_size_(fdt));

	ଫେରନ୍ତୁ 0;
}
