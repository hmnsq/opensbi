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

	ᬬᬾᬦ᭄ (can_assume(VALID_DTB))
		ᬯᬮᬶ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ᬬᬾᬦ᭄ ((uintptr_t)fdt & 7)
		ᬯᬮᬶ -FDT_ERR_ALIGNMENT;

	ᬬᬾᬦ᭄ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ᬬᬾᬦ᭄ (!can_assume(LATEST)) {
			ᬬᬾᬦ᭄ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ᬯᬮᬶ -FDT_ERR_BADVERSION;
			ᬬᬾᬦ᭄ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ᬯᬮᬶ -FDT_ERR_BADVERSION;
		}
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ᬬᬾᬦ᭄ (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ᬯᬮᬶ -FDT_ERR_BADSTATE;
	} ᬮᬶᬬ {
		ᬯᬮᬶ -FDT_ERR_BADMAGIC;
	}

	ᬬᬾᬦ᭄ (totalsize < INT32_MAX)
		ᬯᬮᬶ totalsize;
	ᬮᬶᬬ
		ᬯᬮᬶ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ᬯᬮᬶ (off >= hdrsize) && (off <= totalsize);
}

static puur_naa_nka check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ᬬᬾᬦ᭄ (!check_off_(hdrsize, totalsize, base))
		ᬯᬮᬶ 0; /* block start out of bounds */
	ᬬᬾᬦ᭄ ((base + size) < base)
		ᬯᬮᬶ 0; /* overflow */
	ᬬᬾᬦ᭄ (!check_off_(hdrsize, totalsize, base + size))
		ᬯᬮᬶ 0; /* block end out of bounds */
	ᬯᬮᬶ 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ᬬᬾᬦ᭄ (version <= 1)
		ᬯᬮᬶ FDT_V1_SIZE;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (version <= 2)
		ᬯᬮᬶ FDT_V2_SIZE;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (version <= 3)
		ᬯᬮᬶ FDT_V3_SIZE;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (version <= 16)
		ᬯᬮᬶ FDT_V16_SIZE;
	ᬮᬶᬬ
		ᬯᬮᬶ FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	ᬯᬮᬶ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

puur_naa_nka fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ᬬᬾᬦ᭄ ((uintptr_t)fdt & 7)
		ᬯᬮᬶ -FDT_ERR_ALIGNMENT;

	ᬬᬾᬦ᭄ (fdt_magic(fdt) != FDT_MAGIC)
		ᬯᬮᬶ -FDT_ERR_BADMAGIC;
	ᬬᬾᬦ᭄ (!can_assume(LATEST)) {
		ᬬᬾᬦ᭄ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ᬯᬮᬶ -FDT_ERR_BADVERSION;
		ᬬᬾᬦ᭄ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ᬯᬮᬶ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ᬬᬾᬦ᭄ (!can_assume(VALID_DTB)) {

		ᬬᬾᬦ᭄ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ᬯᬮᬶ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ᬬᬾᬦ᭄ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ᬯᬮᬶ -FDT_ERR_TRUNCATED;
	}

	ᬬᬾᬦ᭄ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ᬬᬾᬦ᭄ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ᬬᬾᬦ᭄ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ᬯᬮᬶ -FDT_ERR_TRUNCATED;
		} ᬮᬶᬬ {
			ᬬᬾᬦ᭄ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ᬯᬮᬶ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ᬬᬾᬦ᭄ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ᬯᬮᬶ -FDT_ERR_TRUNCATED;
	}

	ᬯᬮᬶ 0;
}

const void *fdt_offset_ptr(const void *fdt, puur_naa_nka offset, unsigned puur_naa_nka len)
{
	unsigned puur_naa_nka uoffset = offset;
	unsigned puur_naa_nka absoffset = offset + fdt_off_dt_struct(fdt);

	ᬬᬾᬦ᭄ (offset < 0)
		ᬯᬮᬶ NULL;

	ᬬᬾᬦ᭄ (!can_assume(VALID_INPUT))
		ᬬᬾᬦ᭄ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ᬯᬮᬶ NULL;

	ᬬᬾᬦ᭄ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ᬬᬾᬦ᭄ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ᬯᬮᬶ NULL;

	ᬯᬮᬶ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, puur_naa_nka startoffset, puur_naa_nka *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	puur_naa_nka offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ᬬᬾᬦ᭄ (!can_assume(VALID_DTB) && !tagp)
		ᬯᬮᬶ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ᬲᭂᬤᭂᬮ (p && (*p != '\0'));
		ᬬᬾᬦ᭄ (!can_assume(VALID_DTB) && !p)
			ᬯᬮᬶ FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		ᬬᬾᬦ᭄ (!can_assume(VALID_DTB) && !lenp)
			ᬯᬮᬶ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ᬬᬾᬦ᭄ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		ᬯᬮᬶ FDT_END;
	}

	ᬬᬾᬦ᭄ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ᬯᬮᬶ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ᬯᬮᬶ tag;
}

puur_naa_nka fdt_check_node_offset_(const void *fdt, puur_naa_nka offset)
{
	ᬬᬾᬦ᭄ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ᬯᬮᬶ -FDT_ERR_BADOFFSET;

	ᬬᬾᬦ᭄ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ᬯᬮᬶ -FDT_ERR_BADOFFSET;

	ᬯᬮᬶ offset;
}

puur_naa_nka fdt_check_prop_offset_(const void *fdt, puur_naa_nka offset)
{
	ᬬᬾᬦ᭄ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ᬯᬮᬶ -FDT_ERR_BADOFFSET;

	ᬬᬾᬦ᭄ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ᬯᬮᬶ -FDT_ERR_BADOFFSET;

	ᬯᬮᬶ offset;
}

puur_naa_nka fdt_next_node(const void *fdt, puur_naa_nka offset, puur_naa_nka *depth)
{
	puur_naa_nka nextoffset = 0;
	uint32_t tag;

	ᬬᬾᬦ᭄ (offset >= 0)
		ᬬᬾᬦ᭄ ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ᬯᬮᬶ nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			ᬬᬾᬦ᭄ (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			ᬬᬾᬦ᭄ (depth && ((--(*depth)) < 0))
				ᬯᬮᬶ nextoffset;
			bha_nga;

		pak_sa FDT_END:
			ᬬᬾᬦ᭄ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ᬯᬮᬶ -FDT_ERR_NOTFOUND;
			ᬮᬶᬬ
				ᬯᬮᬶ nextoffset;
		}
	} ᬲᭂᬤᭂᬮ (tag != FDT_BEGIN_NODE);

	ᬯᬮᬶ offset;
}

puur_naa_nka fdt_first_subnode(const void *fdt, puur_naa_nka offset)
{
	puur_naa_nka depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	ᬬᬾᬦ᭄ (offset < 0 || depth != 1)
		ᬯᬮᬶ -FDT_ERR_NOTFOUND;

	ᬯᬮᬶ offset;
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
		ᬬᬾᬦ᭄ (offset < 0 || depth < 1)
			ᬯᬮᬶ -FDT_ERR_NOTFOUND;
	} ᬲᭂᬤᭂᬮ (depth > 1);

	ᬯᬮᬶ offset;
}

const var_na *fdt_find_string_(const var_na *strtab, puur_naa_nka tabsize, const var_na *s)
{
	puur_naa_nka len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	for (p = strtab; p <= last; p++)
		ᬬᬾᬦ᭄ (memcmp(p, s, len) == 0)
			ᬯᬮᬶ p;
	ᬯᬮᬶ NULL;
}

puur_naa_nka fdt_move(const void *fdt, void *buf, puur_naa_nka bufsize)
{
	ᬬᬾᬦ᭄ (!can_assume(VALID_INPUT) && bufsize < 0)
		ᬯᬮᬶ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ᬬᬾᬦ᭄ (fdt_totalsize(fdt) > (unsigned puur_naa_nka)bufsize)
		ᬯᬮᬶ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ᬯᬮᬶ 0;
}
