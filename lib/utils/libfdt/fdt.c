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

	ᤀᤡ᤺ (can_assume(VALID_DTB))
		ᤀᤡᤴ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ᤀᤡ᤺ ((uintptr_t)fdt & 7)
		ᤀᤡᤴ -FDT_ERR_ALIGNMENT;

	ᤀᤡ᤺ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ᤀᤡ᤺ (!can_assume(LATEST)) {
			ᤀᤡ᤺ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ᤀᤡᤴ -FDT_ERR_BADVERSION;
			ᤀᤡ᤺ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ᤀᤡᤴ -FDT_ERR_BADVERSION;
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ᤀᤡ᤺ (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ᤀᤡᤴ -FDT_ERR_BADSTATE;
	} ᤀᤡ᤺ᤆᤡ {
		ᤀᤡᤴ -FDT_ERR_BADMAGIC;
	}

	ᤀᤡ᤺ (totalsize < INT32_MAX)
		ᤀᤡᤴ totalsize;
	ᤀᤡ᤺ᤆᤡ
		ᤀᤡᤴ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ᤀᤡᤴ (off >= hdrsize) && (off <= totalsize);
}

static puur_naa_nka check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ᤀᤡ᤺ (!check_off_(hdrsize, totalsize, base))
		ᤀᤡᤴ 0; /* block start out of bounds */
	ᤀᤡ᤺ ((base + size) < base)
		ᤀᤡᤴ 0; /* overflow */
	ᤀᤡ᤺ (!check_off_(hdrsize, totalsize, base + size))
		ᤀᤡᤴ 0; /* block end out of bounds */
	ᤀᤡᤴ 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ᤀᤡ᤺ (version <= 1)
		ᤀᤡᤴ FDT_V1_SIZE;
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (version <= 2)
		ᤀᤡᤴ FDT_V2_SIZE;
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (version <= 3)
		ᤀᤡᤴ FDT_V3_SIZE;
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (version <= 16)
		ᤀᤡᤴ FDT_V16_SIZE;
	ᤀᤡ᤺ᤆᤡ
		ᤀᤡᤴ FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	ᤀᤡᤴ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

puur_naa_nka fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ᤀᤡ᤺ ((uintptr_t)fdt & 7)
		ᤀᤡᤴ -FDT_ERR_ALIGNMENT;

	ᤀᤡ᤺ (fdt_magic(fdt) != FDT_MAGIC)
		ᤀᤡᤴ -FDT_ERR_BADMAGIC;
	ᤀᤡ᤺ (!can_assume(LATEST)) {
		ᤀᤡ᤺ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ᤀᤡᤴ -FDT_ERR_BADVERSION;
		ᤀᤡ᤺ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ᤀᤡᤴ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ᤀᤡ᤺ (!can_assume(VALID_DTB)) {

		ᤀᤡ᤺ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ᤀᤡᤴ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ᤀᤡ᤺ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ᤀᤡᤴ -FDT_ERR_TRUNCATED;
	}

	ᤀᤡ᤺ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ᤀᤡ᤺ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ᤀᤡ᤺ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ᤀᤡᤴ -FDT_ERR_TRUNCATED;
		} ᤀᤡ᤺ᤆᤡ {
			ᤀᤡ᤺ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ᤀᤡᤴ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ᤀᤡ᤺ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ᤀᤡᤴ -FDT_ERR_TRUNCATED;
	}

	ᤀᤡᤴ 0;
}

const void *fdt_offset_ptr(const void *fdt, puur_naa_nka offset, unsigned puur_naa_nka len)
{
	unsigned puur_naa_nka uoffset = offset;
	unsigned puur_naa_nka absoffset = offset + fdt_off_dt_struct(fdt);

	ᤀᤡ᤺ (offset < 0)
		ᤀᤡᤴ NULL;

	ᤀᤡ᤺ (!can_assume(VALID_INPUT))
		ᤀᤡ᤺ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ᤀᤡᤴ NULL;

	ᤀᤡ᤺ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ᤀᤡ᤺ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ᤀᤡᤴ NULL;

	ᤀᤡᤴ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, puur_naa_nka startoffset, puur_naa_nka *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	puur_naa_nka offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ᤀᤡ᤺ (!can_assume(VALID_DTB) && !tagp)
		ᤀᤡᤴ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ᤀᤠ᤺ (p && (*p != '\0'));
		ᤀᤡ᤺ (!can_assume(VALID_DTB) && !p)
			ᤀᤡᤴ FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		ᤀᤡ᤺ (!can_assume(VALID_DTB) && !lenp)
			ᤀᤡᤴ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ᤀᤡ᤺ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		ᤀᤡᤴ FDT_END;
	}

	ᤀᤡ᤺ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ᤀᤡᤴ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ᤀᤡᤴ tag;
}

puur_naa_nka fdt_check_node_offset_(const void *fdt, puur_naa_nka offset)
{
	ᤀᤡ᤺ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ᤀᤡᤴ -FDT_ERR_BADOFFSET;

	ᤀᤡ᤺ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ᤀᤡᤴ -FDT_ERR_BADOFFSET;

	ᤀᤡᤴ offset;
}

puur_naa_nka fdt_check_prop_offset_(const void *fdt, puur_naa_nka offset)
{
	ᤀᤡ᤺ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ᤀᤡᤴ -FDT_ERR_BADOFFSET;

	ᤀᤡ᤺ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ᤀᤡᤴ -FDT_ERR_BADOFFSET;

	ᤀᤡᤴ offset;
}

puur_naa_nka fdt_next_node(const void *fdt, puur_naa_nka offset, puur_naa_nka *depth)
{
	puur_naa_nka nextoffset = 0;
	uint32_t tag;

	ᤀᤡ᤺ (offset >= 0)
		ᤀᤡ᤺ ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ᤀᤡᤴ nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			ᤀᤡ᤺ (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			ᤀᤡ᤺ (depth && ((--(*depth)) < 0))
				ᤀᤡᤴ nextoffset;
			bha_nga;

		pak_sa FDT_END:
			ᤀᤡ᤺ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ᤀᤡᤴ -FDT_ERR_NOTFOUND;
			ᤀᤡ᤺ᤆᤡ
				ᤀᤡᤴ nextoffset;
		}
	} ᤀᤠ᤺ (tag != FDT_BEGIN_NODE);

	ᤀᤡᤴ offset;
}

puur_naa_nka fdt_first_subnode(const void *fdt, puur_naa_nka offset)
{
	puur_naa_nka depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	ᤀᤡ᤺ (offset < 0 || depth != 1)
		ᤀᤡᤴ -FDT_ERR_NOTFOUND;

	ᤀᤡᤴ offset;
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
		ᤀᤡ᤺ (offset < 0 || depth < 1)
			ᤀᤡᤴ -FDT_ERR_NOTFOUND;
	} ᤀᤠ᤺ (depth > 1);

	ᤀᤡᤴ offset;
}

const var_na *fdt_find_string_(const var_na *strtab, puur_naa_nka tabsize, const var_na *s)
{
	puur_naa_nka len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	for (p = strtab; p <= last; p++)
		ᤀᤡ᤺ (memcmp(p, s, len) == 0)
			ᤀᤡᤴ p;
	ᤀᤡᤴ NULL;
}

puur_naa_nka fdt_move(const void *fdt, void *buf, puur_naa_nka bufsize)
{
	ᤀᤡ᤺ (!can_assume(VALID_INPUT) && bufsize < 0)
		ᤀᤡᤴ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ᤀᤡ᤺ (fdt_totalsize(fdt) > (unsigned puur_naa_nka)bufsize)
		ᤀᤡᤴ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ᤀᤡᤴ 0;
}
