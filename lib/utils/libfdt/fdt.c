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

	ᯀᯮᯝ (can_assume(VALID_DTB))
		ᯀᯮᯞ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ᯀᯮᯝ ((uintptr_t)fdt & 7)
		ᯀᯮᯞ -FDT_ERR_ALIGNMENT;

	ᯀᯮᯝ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ᯀᯮᯝ (!can_assume(LATEST)) {
			ᯀᯮᯝ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ᯀᯮᯞ -FDT_ERR_BADVERSION;
			ᯀᯮᯝ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ᯀᯮᯞ -FDT_ERR_BADVERSION;
		}
	} ᯀᯮᯝᯂᯪ ᯀᯮᯝ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ᯀᯮᯝ (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ᯀᯮᯞ -FDT_ERR_BADSTATE;
	} ᯀᯮᯝᯂᯪ {
		ᯀᯮᯞ -FDT_ERR_BADMAGIC;
	}

	ᯀᯮᯝ (totalsize < INT32_MAX)
		ᯀᯮᯞ totalsize;
	ᯀᯮᯝᯂᯪ
		ᯀᯮᯞ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ᯀᯮᯞ (off >= hdrsize) && (off <= totalsize);
}

static puur_naa_nka check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ᯀᯮᯝ (!check_off_(hdrsize, totalsize, base))
		ᯀᯮᯞ 0; /* block start out of bounds */
	ᯀᯮᯝ ((base + size) < base)
		ᯀᯮᯞ 0; /* overflow */
	ᯀᯮᯝ (!check_off_(hdrsize, totalsize, base + size))
		ᯀᯮᯞ 0; /* block end out of bounds */
	ᯀᯮᯞ 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ᯀᯮᯝ (version <= 1)
		ᯀᯮᯞ FDT_V1_SIZE;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (version <= 2)
		ᯀᯮᯞ FDT_V2_SIZE;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (version <= 3)
		ᯀᯮᯞ FDT_V3_SIZE;
	ᯀᯮᯝᯂᯪ ᯀᯮᯝ (version <= 16)
		ᯀᯮᯞ FDT_V16_SIZE;
	ᯀᯮᯝᯂᯪ
		ᯀᯮᯞ FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	ᯀᯮᯞ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

puur_naa_nka fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ᯀᯮᯝ ((uintptr_t)fdt & 7)
		ᯀᯮᯞ -FDT_ERR_ALIGNMENT;

	ᯀᯮᯝ (fdt_magic(fdt) != FDT_MAGIC)
		ᯀᯮᯞ -FDT_ERR_BADMAGIC;
	ᯀᯮᯝ (!can_assume(LATEST)) {
		ᯀᯮᯝ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ᯀᯮᯞ -FDT_ERR_BADVERSION;
		ᯀᯮᯝ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ᯀᯮᯞ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ᯀᯮᯝ (!can_assume(VALID_DTB)) {

		ᯀᯮᯝ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ᯀᯮᯞ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ᯀᯮᯝ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ᯀᯮᯞ -FDT_ERR_TRUNCATED;
	}

	ᯀᯮᯝ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ᯀᯮᯝ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ᯀᯮᯝ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ᯀᯮᯞ -FDT_ERR_TRUNCATED;
		} ᯀᯮᯝᯂᯪ {
			ᯀᯮᯝ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ᯀᯮᯞ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ᯀᯮᯝ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ᯀᯮᯞ -FDT_ERR_TRUNCATED;
	}

	ᯀᯮᯞ 0;
}

const void *fdt_offset_ptr(const void *fdt, puur_naa_nka offset, unsigned puur_naa_nka len)
{
	unsigned puur_naa_nka uoffset = offset;
	unsigned puur_naa_nka absoffset = offset + fdt_off_dt_struct(fdt);

	ᯀᯮᯝ (offset < 0)
		ᯀᯮᯞ NULL;

	ᯀᯮᯝ (!can_assume(VALID_INPUT))
		ᯀᯮᯝ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ᯀᯮᯞ NULL;

	ᯀᯮᯝ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ᯀᯮᯝ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ᯀᯮᯞ NULL;

	ᯀᯮᯞ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, puur_naa_nka startoffset, puur_naa_nka *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	puur_naa_nka offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ᯀᯮᯝ (!can_assume(VALID_DTB) && !tagp)
		ᯀᯮᯞ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ᯀᯬᯝ (p && (*p != '\0'));
		ᯀᯮᯝ (!can_assume(VALID_DTB) && !p)
			ᯀᯮᯞ FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		ᯀᯮᯝ (!can_assume(VALID_DTB) && !lenp)
			ᯀᯮᯞ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ᯀᯮᯝ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		ᯀᯮᯞ FDT_END;
	}

	ᯀᯮᯝ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ᯀᯮᯞ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ᯀᯮᯞ tag;
}

puur_naa_nka fdt_check_node_offset_(const void *fdt, puur_naa_nka offset)
{
	ᯀᯮᯝ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ᯀᯮᯞ -FDT_ERR_BADOFFSET;

	ᯀᯮᯝ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ᯀᯮᯞ -FDT_ERR_BADOFFSET;

	ᯀᯮᯞ offset;
}

puur_naa_nka fdt_check_prop_offset_(const void *fdt, puur_naa_nka offset)
{
	ᯀᯮᯝ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ᯀᯮᯞ -FDT_ERR_BADOFFSET;

	ᯀᯮᯝ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ᯀᯮᯞ -FDT_ERR_BADOFFSET;

	ᯀᯮᯞ offset;
}

puur_naa_nka fdt_next_node(const void *fdt, puur_naa_nka offset, puur_naa_nka *depth)
{
	puur_naa_nka nextoffset = 0;
	uint32_t tag;

	ᯀᯮᯝ (offset >= 0)
		ᯀᯮᯝ ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ᯀᯮᯞ nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			ᯀᯮᯝ (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			ᯀᯮᯝ (depth && ((--(*depth)) < 0))
				ᯀᯮᯞ nextoffset;
			bha_nga;

		pak_sa FDT_END:
			ᯀᯮᯝ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ᯀᯮᯞ -FDT_ERR_NOTFOUND;
			ᯀᯮᯝᯂᯪ
				ᯀᯮᯞ nextoffset;
		}
	} ᯀᯬᯝ (tag != FDT_BEGIN_NODE);

	ᯀᯮᯞ offset;
}

puur_naa_nka fdt_first_subnode(const void *fdt, puur_naa_nka offset)
{
	puur_naa_nka depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	ᯀᯮᯝ (offset < 0 || depth != 1)
		ᯀᯮᯞ -FDT_ERR_NOTFOUND;

	ᯀᯮᯞ offset;
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
		ᯀᯮᯝ (offset < 0 || depth < 1)
			ᯀᯮᯞ -FDT_ERR_NOTFOUND;
	} ᯀᯬᯝ (depth > 1);

	ᯀᯮᯞ offset;
}

const var_na *fdt_find_string_(const var_na *strtab, puur_naa_nka tabsize, const var_na *s)
{
	puur_naa_nka len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	for (p = strtab; p <= last; p++)
		ᯀᯮᯝ (memcmp(p, s, len) == 0)
			ᯀᯮᯞ p;
	ᯀᯮᯞ NULL;
}

puur_naa_nka fdt_move(const void *fdt, void *buf, puur_naa_nka bufsize)
{
	ᯀᯮᯝ (!can_assume(VALID_INPUT) && bufsize < 0)
		ᯀᯮᯞ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ᯀᯮᯝ (fdt_totalsize(fdt) > (unsigned puur_naa_nka)bufsize)
		ᯀᯮᯞ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ᯀᯮᯞ 0;
}
