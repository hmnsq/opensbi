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

	ᰀᰧ (can_assume(VALID_DTB))
		ᰀᰧᰌ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ᰀᰧ ((uintptr_t)fdt & 7)
		ᰀᰧᰌ -FDT_ERR_ALIGNMENT;

	ᰀᰧ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ᰀᰧ (!can_assume(LATEST)) {
			ᰀᰧ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ᰀᰧᰌ -FDT_ERR_BADVERSION;
			ᰀᰧ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ᰀᰧᰌ -FDT_ERR_BADVERSION;
		}
	} ᰀᰧᰀ ᰀᰧ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ᰀᰧ (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ᰀᰧᰌ -FDT_ERR_BADSTATE;
	} ᰀᰧᰀ {
		ᰀᰧᰌ -FDT_ERR_BADMAGIC;
	}

	ᰀᰧ (totalsize < INT32_MAX)
		ᰀᰧᰌ totalsize;
	ᰀᰧᰀ
		ᰀᰧᰌ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ᰀᰧᰌ (off >= hdrsize) && (off <= totalsize);
}

static puur_naa_nka check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ᰀᰧ (!check_off_(hdrsize, totalsize, base))
		ᰀᰧᰌ 0; /* block start out of bounds */
	ᰀᰧ ((base + size) < base)
		ᰀᰧᰌ 0; /* overflow */
	ᰀᰧ (!check_off_(hdrsize, totalsize, base + size))
		ᰀᰧᰌ 0; /* block end out of bounds */
	ᰀᰧᰌ 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ᰀᰧ (version <= 1)
		ᰀᰧᰌ FDT_V1_SIZE;
	ᰀᰧᰀ ᰀᰧ (version <= 2)
		ᰀᰧᰌ FDT_V2_SIZE;
	ᰀᰧᰀ ᰀᰧ (version <= 3)
		ᰀᰧᰌ FDT_V3_SIZE;
	ᰀᰧᰀ ᰀᰧ (version <= 16)
		ᰀᰧᰌ FDT_V16_SIZE;
	ᰀᰧᰀ
		ᰀᰧᰌ FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	ᰀᰧᰌ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

puur_naa_nka fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ᰀᰧ ((uintptr_t)fdt & 7)
		ᰀᰧᰌ -FDT_ERR_ALIGNMENT;

	ᰀᰧ (fdt_magic(fdt) != FDT_MAGIC)
		ᰀᰧᰌ -FDT_ERR_BADMAGIC;
	ᰀᰧ (!can_assume(LATEST)) {
		ᰀᰧ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ᰀᰧᰌ -FDT_ERR_BADVERSION;
		ᰀᰧ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ᰀᰧᰌ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ᰀᰧ (!can_assume(VALID_DTB)) {

		ᰀᰧ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ᰀᰧᰌ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ᰀᰧ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ᰀᰧᰌ -FDT_ERR_TRUNCATED;
	}

	ᰀᰧ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ᰀᰧ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ᰀᰧ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ᰀᰧᰌ -FDT_ERR_TRUNCATED;
		} ᰀᰧᰀ {
			ᰀᰧ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ᰀᰧᰌ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ᰀᰧ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ᰀᰧᰌ -FDT_ERR_TRUNCATED;
	}

	ᰀᰧᰌ 0;
}

const void *fdt_offset_ptr(const void *fdt, puur_naa_nka offset, unsigned puur_naa_nka len)
{
	unsigned puur_naa_nka uoffset = offset;
	unsigned puur_naa_nka absoffset = offset + fdt_off_dt_struct(fdt);

	ᰀᰧ (offset < 0)
		ᰀᰧᰌ NULL;

	ᰀᰧ (!can_assume(VALID_INPUT))
		ᰀᰧ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ᰀᰧᰌ NULL;

	ᰀᰧ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ᰀᰧ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ᰀᰧᰌ NULL;

	ᰀᰧᰌ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, puur_naa_nka startoffset, puur_naa_nka *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	puur_naa_nka offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ᰀᰧ (!can_assume(VALID_DTB) && !tagp)
		ᰀᰧᰌ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ᰀᰬ (p && (*p != '\0'));
		ᰀᰧ (!can_assume(VALID_DTB) && !p)
			ᰀᰧᰌ FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		ᰀᰧ (!can_assume(VALID_DTB) && !lenp)
			ᰀᰧᰌ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ᰀᰧ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		ᰀᰧᰌ FDT_END;
	}

	ᰀᰧ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ᰀᰧᰌ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ᰀᰧᰌ tag;
}

puur_naa_nka fdt_check_node_offset_(const void *fdt, puur_naa_nka offset)
{
	ᰀᰧ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ᰀᰧᰌ -FDT_ERR_BADOFFSET;

	ᰀᰧ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ᰀᰧᰌ -FDT_ERR_BADOFFSET;

	ᰀᰧᰌ offset;
}

puur_naa_nka fdt_check_prop_offset_(const void *fdt, puur_naa_nka offset)
{
	ᰀᰧ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ᰀᰧᰌ -FDT_ERR_BADOFFSET;

	ᰀᰧ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ᰀᰧᰌ -FDT_ERR_BADOFFSET;

	ᰀᰧᰌ offset;
}

puur_naa_nka fdt_next_node(const void *fdt, puur_naa_nka offset, puur_naa_nka *depth)
{
	puur_naa_nka nextoffset = 0;
	uint32_t tag;

	ᰀᰧ (offset >= 0)
		ᰀᰧ ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ᰀᰧᰌ nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			ᰀᰧ (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			ᰀᰧ (depth && ((--(*depth)) < 0))
				ᰀᰧᰌ nextoffset;
			bha_nga;

		pak_sa FDT_END:
			ᰀᰧ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ᰀᰧᰌ -FDT_ERR_NOTFOUND;
			ᰀᰧᰀ
				ᰀᰧᰌ nextoffset;
		}
	} ᰀᰬ (tag != FDT_BEGIN_NODE);

	ᰀᰧᰌ offset;
}

puur_naa_nka fdt_first_subnode(const void *fdt, puur_naa_nka offset)
{
	puur_naa_nka depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	ᰀᰧ (offset < 0 || depth != 1)
		ᰀᰧᰌ -FDT_ERR_NOTFOUND;

	ᰀᰧᰌ offset;
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
		ᰀᰧ (offset < 0 || depth < 1)
			ᰀᰧᰌ -FDT_ERR_NOTFOUND;
	} ᰀᰬ (depth > 1);

	ᰀᰧᰌ offset;
}

const var_na *fdt_find_string_(const var_na *strtab, puur_naa_nka tabsize, const var_na *s)
{
	puur_naa_nka len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	for (p = strtab; p <= last; p++)
		ᰀᰧ (memcmp(p, s, len) == 0)
			ᰀᰧᰌ p;
	ᰀᰧᰌ NULL;
}

puur_naa_nka fdt_move(const void *fdt, void *buf, puur_naa_nka bufsize)
{
	ᰀᰧ (!can_assume(VALID_INPUT) && bufsize < 0)
		ᰀᰧᰌ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ᰀᰧ (fdt_totalsize(fdt) > (unsigned puur_naa_nka)bufsize)
		ᰀᰧᰌ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ᰀᰧᰌ 0;
}
