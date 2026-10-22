// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(const void *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	အကယ်၍ (can_assume(VALID_DTB))
		ပြန်ပေး totalsize;

	/* The device tree must be at an 8-byte aligned address */
	အကယ်၍ ((uintptr_t)fdt & 7)
		ပြန်ပေး -FDT_ERR_ALIGNMENT;

	အကယ်၍ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		အကယ်၍ (!can_assume(LATEST)) {
			အကယ်၍ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ပြန်ပေး -FDT_ERR_BADVERSION;
			အကယ်၍ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ပြန်ပေး -FDT_ERR_BADVERSION;
		}
	} မဟုတ်ပါက အကယ်၍ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		အကယ်၍ (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ပြန်ပေး -FDT_ERR_BADSTATE;
	} မဟုတ်ပါက {
		ပြန်ပေး -FDT_ERR_BADMAGIC;
	}

	အကယ်၍ (totalsize < INT32_MAX)
		ပြန်ပေး totalsize;
	မဟုတ်ပါက
		ပြန်ပေး -FDT_ERR_TRUNCATED;
}

static ကိန်းပြည့် check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ပြန်ပေး (off >= hdrsize) && (off <= totalsize);
}

static ကိန်းပြည့် check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	အကယ်၍ (!check_off_(hdrsize, totalsize, base))
		ပြန်ပေး 0; /* block start out of bounds */
	အကယ်၍ ((base + size) < base)
		ပြန်ပေး 0; /* overflow */
	အကယ်၍ (!check_off_(hdrsize, totalsize, base + size))
		ပြန်ပေး 0; /* block end out of bounds */
	ပြန်ပေး 1;
}

size_t fdt_header_size_(uint32_t version)
{
	အကယ်၍ (version <= 1)
		ပြန်ပေး FDT_V1_SIZE;
	မဟုတ်ပါက အကယ်၍ (version <= 2)
		ပြန်ပေး FDT_V2_SIZE;
	မဟုတ်ပါက အကယ်၍ (version <= 3)
		ပြန်ပေး FDT_V3_SIZE;
	မဟုတ်ပါက အကယ်၍ (version <= 16)
		ပြန်ပေး FDT_V16_SIZE;
	မဟုတ်ပါက
		ပြန်ပေး FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	ပြန်ပေး can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

ကိန်းပြည့် fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	အကယ်၍ ((uintptr_t)fdt & 7)
		ပြန်ပေး -FDT_ERR_ALIGNMENT;

	အကယ်၍ (fdt_magic(fdt) != FDT_MAGIC)
		ပြန်ပေး -FDT_ERR_BADMAGIC;
	အကယ်၍ (!can_assume(LATEST)) {
		အကယ်၍ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ပြန်ပေး -FDT_ERR_BADVERSION;
		အကယ်၍ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ပြန်ပေး -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	အကယ်၍ (!can_assume(VALID_DTB)) {

		အကယ်၍ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ပြန်ပေး -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		အကယ်၍ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ပြန်ပေး -FDT_ERR_TRUNCATED;
	}

	အကယ်၍ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		အကယ်၍ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			အကယ်၍ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ပြန်ပေး -FDT_ERR_TRUNCATED;
		} မဟုတ်ပါက {
			အကယ်၍ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ပြန်ပေး -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		အကယ်၍ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ပြန်ပေး -FDT_ERR_TRUNCATED;
	}

	ပြန်ပေး 0;
}

const void *fdt_offset_ptr(const void *fdt, ကိန်းပြည့် offset, unsigned ကိန်းပြည့် len)
{
	unsigned ကိန်းပြည့် uoffset = offset;
	unsigned ကိန်းပြည့် absoffset = offset + fdt_off_dt_struct(fdt);

	အကယ်၍ (offset < 0)
		ပြန်ပေး NULL;

	အကယ်၍ (!can_assume(VALID_INPUT))
		အကယ်၍ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ပြန်ပေး NULL;

	အကယ်၍ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		အကယ်၍ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ပြန်ပေး NULL;

	ပြန်ပေး fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, ကိန်းပြည့် startoffset, ကိန်းပြည့် *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	ကိန်းပြည့် offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	အကယ်၍ (!can_assume(VALID_DTB) && !tagp)
		ပြန်ပေး FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} နေစဉ် (p && (*p != '\0'));
		အကယ်၍ (!can_assume(VALID_DTB) && !p)
			ပြန်ပေး FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		အကယ်၍ (!can_assume(VALID_DTB) && !lenp)
			ပြန်ပေး FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		အကယ်၍ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		ပြန်ပေး FDT_END;
	}

	အကယ်၍ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ပြန်ပေး FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ပြန်ပေး tag;
}

ကိန်းပြည့် fdt_check_node_offset_(const void *fdt, ကိန်းပြည့် offset)
{
	အကယ်၍ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ပြန်ပေး -FDT_ERR_BADOFFSET;

	အကယ်၍ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ပြန်ပေး -FDT_ERR_BADOFFSET;

	ပြန်ပေး offset;
}

ကိန်းပြည့် fdt_check_prop_offset_(const void *fdt, ကိန်းပြည့် offset)
{
	အကယ်၍ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ပြန်ပေး -FDT_ERR_BADOFFSET;

	အကယ်၍ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ပြန်ပေး -FDT_ERR_BADOFFSET;

	ပြန်ပေး offset;
}

ကိန်းပြည့် fdt_next_node(const void *fdt, ကိန်းပြည့် offset, ကိန်းပြည့် *depth)
{
	ကိန်းပြည့် nextoffset = 0;
	uint32_t tag;

	အကယ်၍ (offset >= 0)
		အကယ်၍ ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ပြန်ပေး nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			အကယ်၍ (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			အကယ်၍ (depth && ((--(*depth)) < 0))
				ပြန်ပေး nextoffset;
			bha_nga;

		pak_sa FDT_END:
			အကယ်၍ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ပြန်ပေး -FDT_ERR_NOTFOUND;
			မဟုတ်ပါက
				ပြန်ပေး nextoffset;
		}
	} နေစဉ် (tag != FDT_BEGIN_NODE);

	ပြန်ပေး offset;
}

ကိန်းပြည့် fdt_first_subnode(const void *fdt, ကိန်းပြည့် offset)
{
	ကိန်းပြည့် depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	အကယ်၍ (offset < 0 || depth != 1)
		ပြန်ပေး -FDT_ERR_NOTFOUND;

	ပြန်ပေး offset;
}

ကိန်းပြည့် fdt_next_subnode(const void *fdt, ကိန်းပြည့် offset)
{
	ကိန်းပြည့် depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	do {
		offset = fdt_next_node(fdt, offset, &depth);
		အကယ်၍ (offset < 0 || depth < 1)
			ပြန်ပေး -FDT_ERR_NOTFOUND;
	} နေစဉ် (depth > 1);

	ပြန်ပေး offset;
}

const var_na *fdt_find_string_(const var_na *strtab, ကိန်းပြည့် tabsize, const var_na *s)
{
	ကိန်းပြည့် len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	အတွက် (p = strtab; p <= last; p++)
		အကယ်၍ (memcmp(p, s, len) == 0)
			ပြန်ပေး p;
	ပြန်ပေး NULL;
}

ကိန်းပြည့် fdt_move(const void *fdt, void *buf, ကိန်းပြည့် bufsize)
{
	အကယ်၍ (!can_assume(VALID_INPUT) && bufsize < 0)
		ပြန်ပေး -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	အကယ်၍ (fdt_totalsize(fdt) > (unsigned ကိန်းပြည့်)bufsize)
		ပြန်ပေး -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ပြန်ပေး 0;
}
