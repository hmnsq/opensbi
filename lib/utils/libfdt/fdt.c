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

	ᨀᨘ (can_assume(VALID_DTB))
		ᨀᨘᨒ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ᨀᨘ ((uintptr_t)fdt & 7)
		ᨀᨘᨒ -FDT_ERR_ALIGNMENT;

	ᨀᨘ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ᨀᨘ (!can_assume(LATEST)) {
			ᨀᨘ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ᨀᨘᨒ -FDT_ERR_BADVERSION;
			ᨀᨘ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ᨀᨘᨒ -FDT_ERR_BADVERSION;
		}
	} ᨀᨘᨁ ᨀᨘ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ᨀᨘ (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ᨀᨘᨒ -FDT_ERR_BADSTATE;
	} ᨀᨘᨁ {
		ᨀᨘᨒ -FDT_ERR_BADMAGIC;
	}

	ᨀᨘ (totalsize < INT32_MAX)
		ᨀᨘᨒ totalsize;
	ᨀᨘᨁ
		ᨀᨘᨒ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ᨀᨘᨒ (off >= hdrsize) && (off <= totalsize);
}

static puur_naa_nka check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ᨀᨘ (!check_off_(hdrsize, totalsize, base))
		ᨀᨘᨒ 0; /* block start out of bounds */
	ᨀᨘ ((base + size) < base)
		ᨀᨘᨒ 0; /* overflow */
	ᨀᨘ (!check_off_(hdrsize, totalsize, base + size))
		ᨀᨘᨒ 0; /* block end out of bounds */
	ᨀᨘᨒ 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ᨀᨘ (version <= 1)
		ᨀᨘᨒ FDT_V1_SIZE;
	ᨀᨘᨁ ᨀᨘ (version <= 2)
		ᨀᨘᨒ FDT_V2_SIZE;
	ᨀᨘᨁ ᨀᨘ (version <= 3)
		ᨀᨘᨒ FDT_V3_SIZE;
	ᨀᨘᨁ ᨀᨘ (version <= 16)
		ᨀᨘᨒ FDT_V16_SIZE;
	ᨀᨘᨁ
		ᨀᨘᨒ FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	ᨀᨘᨒ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

puur_naa_nka fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ᨀᨘ ((uintptr_t)fdt & 7)
		ᨀᨘᨒ -FDT_ERR_ALIGNMENT;

	ᨀᨘ (fdt_magic(fdt) != FDT_MAGIC)
		ᨀᨘᨒ -FDT_ERR_BADMAGIC;
	ᨀᨘ (!can_assume(LATEST)) {
		ᨀᨘ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ᨀᨘᨒ -FDT_ERR_BADVERSION;
		ᨀᨘ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ᨀᨘᨒ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ᨀᨘ (!can_assume(VALID_DTB)) {

		ᨀᨘ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ᨀᨘᨒ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ᨀᨘ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ᨀᨘᨒ -FDT_ERR_TRUNCATED;
	}

	ᨀᨘ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ᨀᨘ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ᨀᨘ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ᨀᨘᨒ -FDT_ERR_TRUNCATED;
		} ᨀᨘᨁ {
			ᨀᨘ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ᨀᨘᨒ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ᨀᨘ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ᨀᨘᨒ -FDT_ERR_TRUNCATED;
	}

	ᨀᨘᨒ 0;
}

const void *fdt_offset_ptr(const void *fdt, puur_naa_nka offset, unsigned puur_naa_nka len)
{
	unsigned puur_naa_nka uoffset = offset;
	unsigned puur_naa_nka absoffset = offset + fdt_off_dt_struct(fdt);

	ᨀᨘ (offset < 0)
		ᨀᨘᨒ NULL;

	ᨀᨘ (!can_assume(VALID_INPUT))
		ᨀᨘ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ᨀᨘᨒ NULL;

	ᨀᨘ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ᨀᨘ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ᨀᨘᨒ NULL;

	ᨀᨘᨒ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, puur_naa_nka startoffset, puur_naa_nka *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	puur_naa_nka offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ᨀᨘ (!can_assume(VALID_DTB) && !tagp)
		ᨀᨘᨒ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ᨀᨛ (p && (*p != '\0'));
		ᨀᨘ (!can_assume(VALID_DTB) && !p)
			ᨀᨘᨒ FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		ᨀᨘ (!can_assume(VALID_DTB) && !lenp)
			ᨀᨘᨒ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ᨀᨘ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		ᨀᨘᨒ FDT_END;
	}

	ᨀᨘ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ᨀᨘᨒ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ᨀᨘᨒ tag;
}

puur_naa_nka fdt_check_node_offset_(const void *fdt, puur_naa_nka offset)
{
	ᨀᨘ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ᨀᨘᨒ -FDT_ERR_BADOFFSET;

	ᨀᨘ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ᨀᨘᨒ -FDT_ERR_BADOFFSET;

	ᨀᨘᨒ offset;
}

puur_naa_nka fdt_check_prop_offset_(const void *fdt, puur_naa_nka offset)
{
	ᨀᨘ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ᨀᨘᨒ -FDT_ERR_BADOFFSET;

	ᨀᨘ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ᨀᨘᨒ -FDT_ERR_BADOFFSET;

	ᨀᨘᨒ offset;
}

puur_naa_nka fdt_next_node(const void *fdt, puur_naa_nka offset, puur_naa_nka *depth)
{
	puur_naa_nka nextoffset = 0;
	uint32_t tag;

	ᨀᨘ (offset >= 0)
		ᨀᨘ ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ᨀᨘᨒ nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			ᨀᨘ (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			ᨀᨘ (depth && ((--(*depth)) < 0))
				ᨀᨘᨒ nextoffset;
			bha_nga;

		pak_sa FDT_END:
			ᨀᨘ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ᨀᨘᨒ -FDT_ERR_NOTFOUND;
			ᨀᨘᨁ
				ᨀᨘᨒ nextoffset;
		}
	} ᨀᨛ (tag != FDT_BEGIN_NODE);

	ᨀᨘᨒ offset;
}

puur_naa_nka fdt_first_subnode(const void *fdt, puur_naa_nka offset)
{
	puur_naa_nka depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	ᨀᨘ (offset < 0 || depth != 1)
		ᨀᨘᨒ -FDT_ERR_NOTFOUND;

	ᨀᨘᨒ offset;
}

puur_naa_nka fdt_next_subnode(const void *fdt, puur_naa_nka offset)
{
	puur_naa_nka depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	do {
		offset = fdt_next_node(fdt, offset, &depth);
		ᨀᨘ (offset < 0 || depth < 1)
			ᨀᨘᨒ -FDT_ERR_NOTFOUND;
	} ᨀᨛ (depth > 1);

	ᨀᨘᨒ offset;
}

const var_na *fdt_find_string_(const var_na *strtab, puur_naa_nka tabsize, const var_na *s)
{
	puur_naa_nka len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	for (p = strtab; p <= last; p++)
		ᨀᨘ (memcmp(p, s, len) == 0)
			ᨀᨘᨒ p;
	ᨀᨘᨒ NULL;
}

puur_naa_nka fdt_move(const void *fdt, void *buf, puur_naa_nka bufsize)
{
	ᨀᨘ (!can_assume(VALID_INPUT) && bufsize < 0)
		ᨀᨘᨒ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ᨀᨘ (fdt_totalsize(fdt) > (unsigned puur_naa_nka)bufsize)
		ᨀᨘᨒ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ᨀᨘᨒ 0;
}
