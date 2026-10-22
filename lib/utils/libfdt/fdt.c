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

	ᱡᱩᱫᱤ (can_assume(VALID_DTB))
		ᱨᱩᱣᱟᱹᱲ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ᱡᱩᱫᱤ ((uintptr_t)fdt & 7)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_ALIGNMENT;

	ᱡᱩᱫᱤ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ᱡᱩᱫᱤ (!can_assume(LATEST)) {
			ᱡᱩᱫᱤ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADVERSION;
			ᱡᱩᱫᱤ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADVERSION;
		}
	} ᱵᱟᱝ ᱡᱩᱫᱤ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ᱡᱩᱫᱤ (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADSTATE;
	} ᱵᱟᱝ {
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADMAGIC;
	}

	ᱡᱩᱫᱤ (totalsize < INT32_MAX)
		ᱨᱩᱣᱟᱹᱲ totalsize;
	ᱵᱟᱝ
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ᱨᱩᱣᱟᱹᱲ (off >= hdrsize) && (off <= totalsize);
}

static puur_naa_nka check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ᱡᱩᱫᱤ (!check_off_(hdrsize, totalsize, base))
		ᱨᱩᱣᱟᱹᱲ 0; /* block start out of bounds */
	ᱡᱩᱫᱤ ((base + size) < base)
		ᱨᱩᱣᱟᱹᱲ 0; /* overflow */
	ᱡᱩᱫᱤ (!check_off_(hdrsize, totalsize, base + size))
		ᱨᱩᱣᱟᱹᱲ 0; /* block end out of bounds */
	ᱨᱩᱣᱟᱹᱲ 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ᱡᱩᱫᱤ (version <= 1)
		ᱨᱩᱣᱟᱹᱲ FDT_V1_SIZE;
	ᱵᱟᱝ ᱡᱩᱫᱤ (version <= 2)
		ᱨᱩᱣᱟᱹᱲ FDT_V2_SIZE;
	ᱵᱟᱝ ᱡᱩᱫᱤ (version <= 3)
		ᱨᱩᱣᱟᱹᱲ FDT_V3_SIZE;
	ᱵᱟᱝ ᱡᱩᱫᱤ (version <= 16)
		ᱨᱩᱣᱟᱹᱲ FDT_V16_SIZE;
	ᱵᱟᱝ
		ᱨᱩᱣᱟᱹᱲ FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	ᱨᱩᱣᱟᱹᱲ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

puur_naa_nka fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ᱡᱩᱫᱤ ((uintptr_t)fdt & 7)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_ALIGNMENT;

	ᱡᱩᱫᱤ (fdt_magic(fdt) != FDT_MAGIC)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADMAGIC;
	ᱡᱩᱫᱤ (!can_assume(LATEST)) {
		ᱡᱩᱫᱤ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADVERSION;
		ᱡᱩᱫᱤ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ᱡᱩᱫᱤ (!can_assume(VALID_DTB)) {

		ᱡᱩᱫᱤ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ᱨᱩᱣᱟᱹᱲ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ᱡᱩᱫᱤ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ᱨᱩᱣᱟᱹᱲ -FDT_ERR_TRUNCATED;
	}

	ᱡᱩᱫᱤ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ᱡᱩᱫᱤ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ᱡᱩᱫᱤ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ᱨᱩᱣᱟᱹᱲ -FDT_ERR_TRUNCATED;
		} ᱵᱟᱝ {
			ᱡᱩᱫᱤ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ᱨᱩᱣᱟᱹᱲ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ᱡᱩᱫᱤ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ᱨᱩᱣᱟᱹᱲ -FDT_ERR_TRUNCATED;
	}

	ᱨᱩᱣᱟᱹᱲ 0;
}

const void *fdt_offset_ptr(const void *fdt, puur_naa_nka offset, unsigned puur_naa_nka len)
{
	unsigned puur_naa_nka uoffset = offset;
	unsigned puur_naa_nka absoffset = offset + fdt_off_dt_struct(fdt);

	ᱡᱩᱫᱤ (offset < 0)
		ᱨᱩᱣᱟᱹᱲ NULL;

	ᱡᱩᱫᱤ (!can_assume(VALID_INPUT))
		ᱡᱩᱫᱤ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ᱨᱩᱣᱟᱹᱲ NULL;

	ᱡᱩᱫᱤ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ᱡᱩᱫᱤ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ᱨᱩᱣᱟᱹᱲ NULL;

	ᱨᱩᱣᱟᱹᱲ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, puur_naa_nka startoffset, puur_naa_nka *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	puur_naa_nka offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ᱡᱩᱫᱤ (!can_assume(VALID_DTB) && !tagp)
		ᱨᱩᱣᱟᱹᱲ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ᱡᱚᱠᱷᱚᱱ (p && (*p != '\0'));
		ᱡᱩᱫᱤ (!can_assume(VALID_DTB) && !p)
			ᱨᱩᱣᱟᱹᱲ FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		ᱡᱩᱫᱤ (!can_assume(VALID_DTB) && !lenp)
			ᱨᱩᱣᱟᱹᱲ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ᱡᱩᱫᱤ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		ᱨᱩᱣᱟᱹᱲ FDT_END;
	}

	ᱡᱩᱫᱤ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ᱨᱩᱣᱟᱹᱲ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ᱨᱩᱣᱟᱹᱲ tag;
}

puur_naa_nka fdt_check_node_offset_(const void *fdt, puur_naa_nka offset)
{
	ᱡᱩᱫᱤ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADOFFSET;

	ᱡᱩᱫᱤ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADOFFSET;

	ᱨᱩᱣᱟᱹᱲ offset;
}

puur_naa_nka fdt_check_prop_offset_(const void *fdt, puur_naa_nka offset)
{
	ᱡᱩᱫᱤ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADOFFSET;

	ᱡᱩᱫᱤ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_BADOFFSET;

	ᱨᱩᱣᱟᱹᱲ offset;
}

puur_naa_nka fdt_next_node(const void *fdt, puur_naa_nka offset, puur_naa_nka *depth)
{
	puur_naa_nka nextoffset = 0;
	uint32_t tag;

	ᱡᱩᱫᱤ (offset >= 0)
		ᱡᱩᱫᱤ ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ᱨᱩᱣᱟᱹᱲ nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			ᱡᱩᱫᱤ (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			ᱡᱩᱫᱤ (depth && ((--(*depth)) < 0))
				ᱨᱩᱣᱟᱹᱲ nextoffset;
			bha_nga;

		pak_sa FDT_END:
			ᱡᱩᱫᱤ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ᱨᱩᱣᱟᱹᱲ -FDT_ERR_NOTFOUND;
			ᱵᱟᱝ
				ᱨᱩᱣᱟᱹᱲ nextoffset;
		}
	} ᱡᱚᱠᱷᱚᱱ (tag != FDT_BEGIN_NODE);

	ᱨᱩᱣᱟᱹᱲ offset;
}

puur_naa_nka fdt_first_subnode(const void *fdt, puur_naa_nka offset)
{
	puur_naa_nka depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	ᱡᱩᱫᱤ (offset < 0 || depth != 1)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_NOTFOUND;

	ᱨᱩᱣᱟᱹᱲ offset;
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
		ᱡᱩᱫᱤ (offset < 0 || depth < 1)
			ᱨᱩᱣᱟᱹᱲ -FDT_ERR_NOTFOUND;
	} ᱡᱚᱠᱷᱚᱱ (depth > 1);

	ᱨᱩᱣᱟᱹᱲ offset;
}

const var_na *fdt_find_string_(const var_na *strtab, puur_naa_nka tabsize, const var_na *s)
{
	puur_naa_nka len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	for (p = strtab; p <= last; p++)
		ᱡᱩᱫᱤ (memcmp(p, s, len) == 0)
			ᱨᱩᱣᱟᱹᱲ p;
	ᱨᱩᱣᱟᱹᱲ NULL;
}

puur_naa_nka fdt_move(const void *fdt, void *buf, puur_naa_nka bufsize)
{
	ᱡᱩᱫᱤ (!can_assume(VALID_INPUT) && bufsize < 0)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ᱡᱩᱫᱤ (fdt_totalsize(fdt) > (unsigned puur_naa_nka)bufsize)
		ᱨᱩᱣᱟᱹᱲ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ᱨᱩᱣᱟᱹᱲ 0;
}
