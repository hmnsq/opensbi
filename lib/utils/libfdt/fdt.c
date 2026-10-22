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

	ᮚᮦᮔ᮪ (can_assume(VALID_DTB))
		ᮝᮜᮤ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ᮚᮦᮔ᮪ ((uintptr_t)fdt & 7)
		ᮝᮜᮤ -FDT_ERR_ALIGNMENT;

	ᮚᮦᮔ᮪ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ᮚᮦᮔ᮪ (!can_assume(LATEST)) {
			ᮚᮦᮔ᮪ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ᮝᮜᮤ -FDT_ERR_BADVERSION;
			ᮚᮦᮔ᮪ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ᮝᮜᮤ -FDT_ERR_BADVERSION;
		}
	} ᮜᮤᮚ ᮚᮦᮔ᮪ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ᮚᮦᮔ᮪ (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ᮝᮜᮤ -FDT_ERR_BADSTATE;
	} ᮜᮤᮚ {
		ᮝᮜᮤ -FDT_ERR_BADMAGIC;
	}

	ᮚᮦᮔ᮪ (totalsize < INT32_MAX)
		ᮝᮜᮤ totalsize;
	ᮜᮤᮚ
		ᮝᮜᮤ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ᮝᮜᮤ (off >= hdrsize) && (off <= totalsize);
}

static puur_naa_nka check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ᮚᮦᮔ᮪ (!check_off_(hdrsize, totalsize, base))
		ᮝᮜᮤ 0; /* block start out of bounds */
	ᮚᮦᮔ᮪ ((base + size) < base)
		ᮝᮜᮤ 0; /* overflow */
	ᮚᮦᮔ᮪ (!check_off_(hdrsize, totalsize, base + size))
		ᮝᮜᮤ 0; /* block end out of bounds */
	ᮝᮜᮤ 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ᮚᮦᮔ᮪ (version <= 1)
		ᮝᮜᮤ FDT_V1_SIZE;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (version <= 2)
		ᮝᮜᮤ FDT_V2_SIZE;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (version <= 3)
		ᮝᮜᮤ FDT_V3_SIZE;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (version <= 16)
		ᮝᮜᮤ FDT_V16_SIZE;
	ᮜᮤᮚ
		ᮝᮜᮤ FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	ᮝᮜᮤ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

puur_naa_nka fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ᮚᮦᮔ᮪ ((uintptr_t)fdt & 7)
		ᮝᮜᮤ -FDT_ERR_ALIGNMENT;

	ᮚᮦᮔ᮪ (fdt_magic(fdt) != FDT_MAGIC)
		ᮝᮜᮤ -FDT_ERR_BADMAGIC;
	ᮚᮦᮔ᮪ (!can_assume(LATEST)) {
		ᮚᮦᮔ᮪ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ᮝᮜᮤ -FDT_ERR_BADVERSION;
		ᮚᮦᮔ᮪ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ᮝᮜᮤ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ᮚᮦᮔ᮪ (!can_assume(VALID_DTB)) {

		ᮚᮦᮔ᮪ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ᮝᮜᮤ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ᮚᮦᮔ᮪ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ᮝᮜᮤ -FDT_ERR_TRUNCATED;
	}

	ᮚᮦᮔ᮪ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ᮚᮦᮔ᮪ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ᮚᮦᮔ᮪ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ᮝᮜᮤ -FDT_ERR_TRUNCATED;
		} ᮜᮤᮚ {
			ᮚᮦᮔ᮪ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ᮝᮜᮤ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ᮚᮦᮔ᮪ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ᮝᮜᮤ -FDT_ERR_TRUNCATED;
	}

	ᮝᮜᮤ 0;
}

const void *fdt_offset_ptr(const void *fdt, puur_naa_nka offset, unsigned puur_naa_nka len)
{
	unsigned puur_naa_nka uoffset = offset;
	unsigned puur_naa_nka absoffset = offset + fdt_off_dt_struct(fdt);

	ᮚᮦᮔ᮪ (offset < 0)
		ᮝᮜᮤ NULL;

	ᮚᮦᮔ᮪ (!can_assume(VALID_INPUT))
		ᮚᮦᮔ᮪ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ᮝᮜᮤ NULL;

	ᮚᮦᮔ᮪ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ᮚᮦᮔ᮪ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ᮝᮜᮤ NULL;

	ᮝᮜᮤ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, puur_naa_nka startoffset, puur_naa_nka *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	puur_naa_nka offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ᮚᮦᮔ᮪ (!can_assume(VALID_DTB) && !tagp)
		ᮝᮜᮤ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ᮞᮨᮓᮨᮜ (p && (*p != '\0'));
		ᮚᮦᮔ᮪ (!can_assume(VALID_DTB) && !p)
			ᮝᮜᮤ FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		ᮚᮦᮔ᮪ (!can_assume(VALID_DTB) && !lenp)
			ᮝᮜᮤ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ᮚᮦᮔ᮪ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		ᮝᮜᮤ FDT_END;
	}

	ᮚᮦᮔ᮪ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ᮝᮜᮤ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ᮝᮜᮤ tag;
}

puur_naa_nka fdt_check_node_offset_(const void *fdt, puur_naa_nka offset)
{
	ᮚᮦᮔ᮪ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ᮝᮜᮤ -FDT_ERR_BADOFFSET;

	ᮚᮦᮔ᮪ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ᮝᮜᮤ -FDT_ERR_BADOFFSET;

	ᮝᮜᮤ offset;
}

puur_naa_nka fdt_check_prop_offset_(const void *fdt, puur_naa_nka offset)
{
	ᮚᮦᮔ᮪ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ᮝᮜᮤ -FDT_ERR_BADOFFSET;

	ᮚᮦᮔ᮪ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ᮝᮜᮤ -FDT_ERR_BADOFFSET;

	ᮝᮜᮤ offset;
}

puur_naa_nka fdt_next_node(const void *fdt, puur_naa_nka offset, puur_naa_nka *depth)
{
	puur_naa_nka nextoffset = 0;
	uint32_t tag;

	ᮚᮦᮔ᮪ (offset >= 0)
		ᮚᮦᮔ᮪ ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ᮝᮜᮤ nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			ᮚᮦᮔ᮪ (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			ᮚᮦᮔ᮪ (depth && ((--(*depth)) < 0))
				ᮝᮜᮤ nextoffset;
			bha_nga;

		pak_sa FDT_END:
			ᮚᮦᮔ᮪ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ᮝᮜᮤ -FDT_ERR_NOTFOUND;
			ᮜᮤᮚ
				ᮝᮜᮤ nextoffset;
		}
	} ᮞᮨᮓᮨᮜ (tag != FDT_BEGIN_NODE);

	ᮝᮜᮤ offset;
}

puur_naa_nka fdt_first_subnode(const void *fdt, puur_naa_nka offset)
{
	puur_naa_nka depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	ᮚᮦᮔ᮪ (offset < 0 || depth != 1)
		ᮝᮜᮤ -FDT_ERR_NOTFOUND;

	ᮝᮜᮤ offset;
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
		ᮚᮦᮔ᮪ (offset < 0 || depth < 1)
			ᮝᮜᮤ -FDT_ERR_NOTFOUND;
	} ᮞᮨᮓᮨᮜ (depth > 1);

	ᮝᮜᮤ offset;
}

const var_na *fdt_find_string_(const var_na *strtab, puur_naa_nka tabsize, const var_na *s)
{
	puur_naa_nka len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	for (p = strtab; p <= last; p++)
		ᮚᮦᮔ᮪ (memcmp(p, s, len) == 0)
			ᮝᮜᮤ p;
	ᮝᮜᮤ NULL;
}

puur_naa_nka fdt_move(const void *fdt, void *buf, puur_naa_nka bufsize)
{
	ᮚᮦᮔ᮪ (!can_assume(VALID_INPUT) && bufsize < 0)
		ᮝᮜᮤ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ᮚᮦᮔ᮪ (fdt_totalsize(fdt) > (unsigned puur_naa_nka)bufsize)
		ᮝᮜᮤ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ᮝᮜᮤ 0;
}
