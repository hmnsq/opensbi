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

	නම් (can_assume(VALID_DTB))
		ආපසු totalsize;

	/* The device tree must be at an 8-byte aligned address */
	නම් ((uintptr_t)fdt & 7)
		ආපසු -FDT_ERR_ALIGNMENT;

	නම් (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		නම් (!can_assume(LATEST)) {
			නම් (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ආපසු -FDT_ERR_BADVERSION;
			නම් (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ආපසු -FDT_ERR_BADVERSION;
		}
	} නැතහොත් නම් (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		නම් (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ආපසු -FDT_ERR_BADSTATE;
	} නැතහොත් {
		ආපසු -FDT_ERR_BADMAGIC;
	}

	නම් (totalsize < INT32_MAX)
		ආපසු totalsize;
	නැතහොත්
		ආපසු -FDT_ERR_TRUNCATED;
}

static පූර්ණ_සංඛ්‍යා check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ආපසු (off >= hdrsize) && (off <= totalsize);
}

static පූර්ණ_සංඛ්‍යා check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	නම් (!check_off_(hdrsize, totalsize, base))
		ආපසු 0; /* block start out of bounds */
	නම් ((base + size) < base)
		ආපසු 0; /* overflow */
	නම් (!check_off_(hdrsize, totalsize, base + size))
		ආපසු 0; /* block end out of bounds */
	ආපසු 1;
}

size_t fdt_header_size_(uint32_t version)
{
	නම් (version <= 1)
		ආපසු FDT_V1_SIZE;
	නැතහොත් නම් (version <= 2)
		ආපසු FDT_V2_SIZE;
	නැතහොත් නම් (version <= 3)
		ආපසු FDT_V3_SIZE;
	නැතහොත් නම් (version <= 16)
		ආපසු FDT_V16_SIZE;
	නැතහොත්
		ආපසු FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	ආපසු can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

පූර්ණ_සංඛ්‍යා fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	නම් ((uintptr_t)fdt & 7)
		ආපසු -FDT_ERR_ALIGNMENT;

	නම් (fdt_magic(fdt) != FDT_MAGIC)
		ආපසු -FDT_ERR_BADMAGIC;
	නම් (!can_assume(LATEST)) {
		නම් ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ආපසු -FDT_ERR_BADVERSION;
		නම් (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ආපසු -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	නම් (!can_assume(VALID_DTB)) {

		නම් ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ආපසු -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		නම් (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ආපසු -FDT_ERR_TRUNCATED;
	}

	නම් (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		නම් (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			නම් (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ආපසු -FDT_ERR_TRUNCATED;
		} නැතහොත් {
			නම් (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ආපසු -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		නම් (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ආපසු -FDT_ERR_TRUNCATED;
	}

	ආපසු 0;
}

const void *fdt_offset_ptr(const void *fdt, පූර්ණ_සංඛ්‍යා offset, unsigned පූර්ණ_සංඛ්‍යා len)
{
	unsigned පූර්ණ_සංඛ්‍යා uoffset = offset;
	unsigned පූර්ණ_සංඛ්‍යා absoffset = offset + fdt_off_dt_struct(fdt);

	නම් (offset < 0)
		ආපසු NULL;

	නම් (!can_assume(VALID_INPUT))
		නම් ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ආපසු NULL;

	නම් (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		නම් (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ආපසු NULL;

	ආපසු fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, පූර්ණ_සංඛ්‍යා startoffset, පූර්ණ_සංඛ්‍යා *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	පූර්ණ_සංඛ්‍යා offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	නම් (!can_assume(VALID_DTB) && !tagp)
		ආපසු FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} අතරතුර (p && (*p != '\0'));
		නම් (!can_assume(VALID_DTB) && !p)
			ආපසු FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		නම් (!can_assume(VALID_DTB) && !lenp)
			ආපසු FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		නම් (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		ආපසු FDT_END;
	}

	නම් (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ආපසු FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ආපසු tag;
}

පූර්ණ_සංඛ්‍යා fdt_check_node_offset_(const void *fdt, පූර්ණ_සංඛ්‍යා offset)
{
	නම් (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ආපසු -FDT_ERR_BADOFFSET;

	නම් (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ආපසු -FDT_ERR_BADOFFSET;

	ආපසු offset;
}

පූර්ණ_සංඛ්‍යා fdt_check_prop_offset_(const void *fdt, පූර්ණ_සංඛ්‍යා offset)
{
	නම් (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ආපසු -FDT_ERR_BADOFFSET;

	නම් (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ආපසු -FDT_ERR_BADOFFSET;

	ආපසු offset;
}

පූර්ණ_සංඛ්‍යා fdt_next_node(const void *fdt, පූර්ණ_සංඛ්‍යා offset, පූර්ණ_සංඛ්‍යා *depth)
{
	පූර්ණ_සංඛ්‍යා nextoffset = 0;
	uint32_t tag;

	නම් (offset >= 0)
		නම් ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ආපසු nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			නම් (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			නම් (depth && ((--(*depth)) < 0))
				ආපසු nextoffset;
			bha_nga;

		pak_sa FDT_END:
			නම් ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ආපසු -FDT_ERR_NOTFOUND;
			නැතහොත්
				ආපසු nextoffset;
		}
	} අතරතුර (tag != FDT_BEGIN_NODE);

	ආපසු offset;
}

පූර්ණ_සංඛ්‍යා fdt_first_subnode(const void *fdt, පූර්ණ_සංඛ්‍යා offset)
{
	පූර්ණ_සංඛ්‍යා depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	නම් (offset < 0 || depth != 1)
		ආපසු -FDT_ERR_NOTFOUND;

	ආපසු offset;
}

පූර්ණ_සංඛ්‍යා fdt_next_subnode(const void *fdt, පූර්ණ_සංඛ්‍යා offset)
{
	පූර්ණ_සංඛ්‍යා depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	do {
		offset = fdt_next_node(fdt, offset, &depth);
		නම් (offset < 0 || depth < 1)
			ආපසු -FDT_ERR_NOTFOUND;
	} අතරතුර (depth > 1);

	ආපසු offset;
}

const var_na *fdt_find_string_(const var_na *strtab, පූර්ණ_සංඛ්‍යා tabsize, const var_na *s)
{
	පූර්ණ_සංඛ්‍යා len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	සඳහා (p = strtab; p <= last; p++)
		නම් (memcmp(p, s, len) == 0)
			ආපසු p;
	ආපසු NULL;
}

පූර්ණ_සංඛ්‍යා fdt_move(const void *fdt, void *buf, පූර්ණ_සංඛ්‍යා bufsize)
{
	නම් (!can_assume(VALID_INPUT) && bufsize < 0)
		ආපසු -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	නම් (fdt_totalsize(fdt) > (unsigned පූර්ණ_සංඛ්‍යා)bufsize)
		ආපසු -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ආපසු 0;
}
