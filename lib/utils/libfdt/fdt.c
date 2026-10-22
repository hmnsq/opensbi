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

	ꦪꦼꦤ꧀ (can_assume(VALID_DTB))
		ꦮꦭꦶ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ꦪꦼꦤ꧀ ((uintptr_t)fdt & 7)
		ꦮꦭꦶ -FDT_ERR_ALIGNMENT;

	ꦪꦼꦤ꧀ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ꦪꦼꦤ꧀ (!can_assume(LATEST)) {
			ꦪꦼꦤ꧀ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ꦮꦭꦶ -FDT_ERR_BADVERSION;
			ꦪꦼꦤ꧀ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ꦮꦭꦶ -FDT_ERR_BADVERSION;
		}
	} ꦭꦶꦪ ꦪꦼꦤ꧀ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ꦪꦼꦤ꧀ (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ꦮꦭꦶ -FDT_ERR_BADSTATE;
	} ꦭꦶꦪ {
		ꦮꦭꦶ -FDT_ERR_BADMAGIC;
	}

	ꦪꦼꦤ꧀ (totalsize < INT32_MAX)
		ꦮꦭꦶ totalsize;
	ꦭꦶꦪ
		ꦮꦭꦶ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ꦮꦭꦶ (off >= hdrsize) && (off <= totalsize);
}

static puur_naa_nka check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ꦪꦼꦤ꧀ (!check_off_(hdrsize, totalsize, base))
		ꦮꦭꦶ 0; /* block start out of bounds */
	ꦪꦼꦤ꧀ ((base + size) < base)
		ꦮꦭꦶ 0; /* overflow */
	ꦪꦼꦤ꧀ (!check_off_(hdrsize, totalsize, base + size))
		ꦮꦭꦶ 0; /* block end out of bounds */
	ꦮꦭꦶ 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ꦪꦼꦤ꧀ (version <= 1)
		ꦮꦭꦶ FDT_V1_SIZE;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (version <= 2)
		ꦮꦭꦶ FDT_V2_SIZE;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (version <= 3)
		ꦮꦭꦶ FDT_V3_SIZE;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (version <= 16)
		ꦮꦭꦶ FDT_V16_SIZE;
	ꦭꦶꦪ
		ꦮꦭꦶ FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	ꦮꦭꦶ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

puur_naa_nka fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ꦪꦼꦤ꧀ ((uintptr_t)fdt & 7)
		ꦮꦭꦶ -FDT_ERR_ALIGNMENT;

	ꦪꦼꦤ꧀ (fdt_magic(fdt) != FDT_MAGIC)
		ꦮꦭꦶ -FDT_ERR_BADMAGIC;
	ꦪꦼꦤ꧀ (!can_assume(LATEST)) {
		ꦪꦼꦤ꧀ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ꦮꦭꦶ -FDT_ERR_BADVERSION;
		ꦪꦼꦤ꧀ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ꦮꦭꦶ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ꦪꦼꦤ꧀ (!can_assume(VALID_DTB)) {

		ꦪꦼꦤ꧀ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ꦮꦭꦶ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ꦪꦼꦤ꧀ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ꦮꦭꦶ -FDT_ERR_TRUNCATED;
	}

	ꦪꦼꦤ꧀ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ꦪꦼꦤ꧀ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ꦪꦼꦤ꧀ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ꦮꦭꦶ -FDT_ERR_TRUNCATED;
		} ꦭꦶꦪ {
			ꦪꦼꦤ꧀ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ꦮꦭꦶ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ꦪꦼꦤ꧀ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ꦮꦭꦶ -FDT_ERR_TRUNCATED;
	}

	ꦮꦭꦶ 0;
}

const void *fdt_offset_ptr(const void *fdt, puur_naa_nka offset, unsigned puur_naa_nka len)
{
	unsigned puur_naa_nka uoffset = offset;
	unsigned puur_naa_nka absoffset = offset + fdt_off_dt_struct(fdt);

	ꦪꦼꦤ꧀ (offset < 0)
		ꦮꦭꦶ NULL;

	ꦪꦼꦤ꧀ (!can_assume(VALID_INPUT))
		ꦪꦼꦤ꧀ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ꦮꦭꦶ NULL;

	ꦪꦼꦤ꧀ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ꦪꦼꦤ꧀ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ꦮꦭꦶ NULL;

	ꦮꦭꦶ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, puur_naa_nka startoffset, puur_naa_nka *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	puur_naa_nka offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ꦪꦼꦤ꧀ (!can_assume(VALID_DTB) && !tagp)
		ꦮꦭꦶ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ꦱꦼꦢꦼꦭ (p && (*p != '\0'));
		ꦪꦼꦤ꧀ (!can_assume(VALID_DTB) && !p)
			ꦮꦭꦶ FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		ꦪꦼꦤ꧀ (!can_assume(VALID_DTB) && !lenp)
			ꦮꦭꦶ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ꦪꦼꦤ꧀ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		ꦮꦭꦶ FDT_END;
	}

	ꦪꦼꦤ꧀ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ꦮꦭꦶ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ꦮꦭꦶ tag;
}

puur_naa_nka fdt_check_node_offset_(const void *fdt, puur_naa_nka offset)
{
	ꦪꦼꦤ꧀ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ꦮꦭꦶ -FDT_ERR_BADOFFSET;

	ꦪꦼꦤ꧀ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ꦮꦭꦶ -FDT_ERR_BADOFFSET;

	ꦮꦭꦶ offset;
}

puur_naa_nka fdt_check_prop_offset_(const void *fdt, puur_naa_nka offset)
{
	ꦪꦼꦤ꧀ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ꦮꦭꦶ -FDT_ERR_BADOFFSET;

	ꦪꦼꦤ꧀ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ꦮꦭꦶ -FDT_ERR_BADOFFSET;

	ꦮꦭꦶ offset;
}

puur_naa_nka fdt_next_node(const void *fdt, puur_naa_nka offset, puur_naa_nka *depth)
{
	puur_naa_nka nextoffset = 0;
	uint32_t tag;

	ꦪꦼꦤ꧀ (offset >= 0)
		ꦪꦼꦤ꧀ ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ꦮꦭꦶ nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			ꦪꦼꦤ꧀ (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			ꦪꦼꦤ꧀ (depth && ((--(*depth)) < 0))
				ꦮꦭꦶ nextoffset;
			bha_nga;

		pak_sa FDT_END:
			ꦪꦼꦤ꧀ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ꦮꦭꦶ -FDT_ERR_NOTFOUND;
			ꦭꦶꦪ
				ꦮꦭꦶ nextoffset;
		}
	} ꦱꦼꦢꦼꦭ (tag != FDT_BEGIN_NODE);

	ꦮꦭꦶ offset;
}

puur_naa_nka fdt_first_subnode(const void *fdt, puur_naa_nka offset)
{
	puur_naa_nka depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	ꦪꦼꦤ꧀ (offset < 0 || depth != 1)
		ꦮꦭꦶ -FDT_ERR_NOTFOUND;

	ꦮꦭꦶ offset;
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
		ꦪꦼꦤ꧀ (offset < 0 || depth < 1)
			ꦮꦭꦶ -FDT_ERR_NOTFOUND;
	} ꦱꦼꦢꦼꦭ (depth > 1);

	ꦮꦭꦶ offset;
}

const var_na *fdt_find_string_(const var_na *strtab, puur_naa_nka tabsize, const var_na *s)
{
	puur_naa_nka len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	ꦏꦁꦒꦺꦴ (p = strtab; p <= last; p++)
		ꦪꦼꦤ꧀ (memcmp(p, s, len) == 0)
			ꦮꦭꦶ p;
	ꦮꦭꦶ NULL;
}

puur_naa_nka fdt_move(const void *fdt, void *buf, puur_naa_nka bufsize)
{
	ꦪꦼꦤ꧀ (!can_assume(VALID_INPUT) && bufsize < 0)
		ꦮꦭꦶ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ꦪꦼꦤ꧀ (fdt_totalsize(fdt) > (unsigned puur_naa_nka)bufsize)
		ꦮꦭꦶ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ꦮꦭꦶ 0;
}
