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

	ပေႃး (can_assume(VALID_DTB))
		ပွၵ်ႈ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ပေႃး ((uintptr_t)fdt & 7)
		ပွၵ်ႈ -FDT_ERR_ALIGNMENT;

	ပေႃး (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ပေႃး (!can_assume(LATEST)) {
			ပေႃး (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ပွၵ်ႈ -FDT_ERR_BADVERSION;
			ပေႃး (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ပွၵ်ႈ -FDT_ERR_BADVERSION;
		}
	} ပေႃးဢမ်ႇ ပေႃး (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ပေႃး (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ပွၵ်ႈ -FDT_ERR_BADSTATE;
	} ပေႃးဢမ်ႇ {
		ပွၵ်ႈ -FDT_ERR_BADMAGIC;
	}

	ပေႃး (totalsize < INT32_MAX)
		ပွၵ်ႈ totalsize;
	ပေႃးဢမ်ႇ
		ပွၵ်ႈ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ပွၵ်ႈ (off >= hdrsize) && (off <= totalsize);
}

static puur_naa_nka check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ပေႃး (!check_off_(hdrsize, totalsize, base))
		ပွၵ်ႈ 0; /* block start out of bounds */
	ပေႃး ((base + size) < base)
		ပွၵ်ႈ 0; /* overflow */
	ပေႃး (!check_off_(hdrsize, totalsize, base + size))
		ပွၵ်ႈ 0; /* block end out of bounds */
	ပွၵ်ႈ 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ပေႃး (version <= 1)
		ပွၵ်ႈ FDT_V1_SIZE;
	ပေႃးဢမ်ႇ ပေႃး (version <= 2)
		ပွၵ်ႈ FDT_V2_SIZE;
	ပေႃးဢမ်ႇ ပေႃး (version <= 3)
		ပွၵ်ႈ FDT_V3_SIZE;
	ပေႃးဢမ်ႇ ပေႃး (version <= 16)
		ပွၵ်ႈ FDT_V16_SIZE;
	ပေႃးဢမ်ႇ
		ပွၵ်ႈ FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	ပွၵ်ႈ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

puur_naa_nka fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ပေႃး ((uintptr_t)fdt & 7)
		ပွၵ်ႈ -FDT_ERR_ALIGNMENT;

	ပေႃး (fdt_magic(fdt) != FDT_MAGIC)
		ပွၵ်ႈ -FDT_ERR_BADMAGIC;
	ပေႃး (!can_assume(LATEST)) {
		ပေႃး ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ပွၵ်ႈ -FDT_ERR_BADVERSION;
		ပေႃး (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ပွၵ်ႈ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ပေႃး (!can_assume(VALID_DTB)) {

		ပေႃး ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ပွၵ်ႈ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ပေႃး (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ပွၵ်ႈ -FDT_ERR_TRUNCATED;
	}

	ပေႃး (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ပေႃး (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ပေႃး (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ပွၵ်ႈ -FDT_ERR_TRUNCATED;
		} ပေႃးဢမ်ႇ {
			ပေႃး (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ပွၵ်ႈ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ပေႃး (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ပွၵ်ႈ -FDT_ERR_TRUNCATED;
	}

	ပွၵ်ႈ 0;
}

const void *fdt_offset_ptr(const void *fdt, puur_naa_nka offset, unsigned puur_naa_nka len)
{
	unsigned puur_naa_nka uoffset = offset;
	unsigned puur_naa_nka absoffset = offset + fdt_off_dt_struct(fdt);

	ပေႃး (offset < 0)
		ပွၵ်ႈ NULL;

	ပေႃး (!can_assume(VALID_INPUT))
		ပေႃး ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ပွၵ်ႈ NULL;

	ပေႃး (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ပေႃး (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ပွၵ်ႈ NULL;

	ပွၵ်ႈ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, puur_naa_nka startoffset, puur_naa_nka *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	puur_naa_nka offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ပေႃး (!can_assume(VALID_DTB) && !tagp)
		ပွၵ်ႈ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ၶၢဝ်း (p && (*p != '\0'));
		ပေႃး (!can_assume(VALID_DTB) && !p)
			ပွၵ်ႈ FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		ပေႃး (!can_assume(VALID_DTB) && !lenp)
			ပွၵ်ႈ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ပေႃး (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		ပွၵ်ႈ FDT_END;
	}

	ပေႃး (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ပွၵ်ႈ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ပွၵ်ႈ tag;
}

puur_naa_nka fdt_check_node_offset_(const void *fdt, puur_naa_nka offset)
{
	ပေႃး (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ပွၵ်ႈ -FDT_ERR_BADOFFSET;

	ပေႃး (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ပွၵ်ႈ -FDT_ERR_BADOFFSET;

	ပွၵ်ႈ offset;
}

puur_naa_nka fdt_check_prop_offset_(const void *fdt, puur_naa_nka offset)
{
	ပေႃး (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ပွၵ်ႈ -FDT_ERR_BADOFFSET;

	ပေႃး (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ပွၵ်ႈ -FDT_ERR_BADOFFSET;

	ပွၵ်ႈ offset;
}

puur_naa_nka fdt_next_node(const void *fdt, puur_naa_nka offset, puur_naa_nka *depth)
{
	puur_naa_nka nextoffset = 0;
	uint32_t tag;

	ပေႃး (offset >= 0)
		ပေႃး ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ပွၵ်ႈ nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			ပေႃး (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			ပေႃး (depth && ((--(*depth)) < 0))
				ပွၵ်ႈ nextoffset;
			bha_nga;

		pak_sa FDT_END:
			ပေႃး ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ပွၵ်ႈ -FDT_ERR_NOTFOUND;
			ပေႃးဢမ်ႇ
				ပွၵ်ႈ nextoffset;
		}
	} ၶၢဝ်း (tag != FDT_BEGIN_NODE);

	ပွၵ်ႈ offset;
}

puur_naa_nka fdt_first_subnode(const void *fdt, puur_naa_nka offset)
{
	puur_naa_nka depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	ပေႃး (offset < 0 || depth != 1)
		ပွၵ်ႈ -FDT_ERR_NOTFOUND;

	ပွၵ်ႈ offset;
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
		ပေႃး (offset < 0 || depth < 1)
			ပွၵ်ႈ -FDT_ERR_NOTFOUND;
	} ၶၢဝ်း (depth > 1);

	ပွၵ်ႈ offset;
}

const var_na *fdt_find_string_(const var_na *strtab, puur_naa_nka tabsize, const var_na *s)
{
	puur_naa_nka len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	for (p = strtab; p <= last; p++)
		ပေႃး (memcmp(p, s, len) == 0)
			ပွၵ်ႈ p;
	ပွၵ်ႈ NULL;
}

puur_naa_nka fdt_move(const void *fdt, void *buf, puur_naa_nka bufsize)
{
	ပေႃး (!can_assume(VALID_INPUT) && bufsize < 0)
		ပွၵ်ႈ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ပေႃး (fdt_totalsize(fdt) > (unsigned puur_naa_nka)bufsize)
		ပွၵ်ႈ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ပွၵ်ႈ 0;
}
