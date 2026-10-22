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

	ຖ້າ (can_assume(VALID_DTB))
		ສົ່ງຄືນ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ຖ້າ ((uintptr_t)fdt & 7)
		ສົ່ງຄືນ -FDT_ERR_ALIGNMENT;

	ຖ້າ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ຖ້າ (!can_assume(LATEST)) {
			ຖ້າ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ສົ່ງຄືນ -FDT_ERR_BADVERSION;
			ຖ້າ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ສົ່ງຄືນ -FDT_ERR_BADVERSION;
		}
	} ບໍ່ດັ່ງນັ້ນ ຖ້າ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ຖ້າ (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ສົ່ງຄືນ -FDT_ERR_BADSTATE;
	} ບໍ່ດັ່ງນັ້ນ {
		ສົ່ງຄືນ -FDT_ERR_BADMAGIC;
	}

	ຖ້າ (totalsize < INT32_MAX)
		ສົ່ງຄືນ totalsize;
	ບໍ່ດັ່ງນັ້ນ
		ສົ່ງຄືນ -FDT_ERR_TRUNCATED;
}

static ຈຳນວນເຕັມ check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ສົ່ງຄືນ (off >= hdrsize) && (off <= totalsize);
}

static ຈຳນວນເຕັມ check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ຖ້າ (!check_off_(hdrsize, totalsize, base))
		ສົ່ງຄືນ 0; /* block start out of bounds */
	ຖ້າ ((base + size) < base)
		ສົ່ງຄືນ 0; /* overflow */
	ຖ້າ (!check_off_(hdrsize, totalsize, base + size))
		ສົ່ງຄືນ 0; /* block end out of bounds */
	ສົ່ງຄືນ 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ຖ້າ (version <= 1)
		ສົ່ງຄືນ FDT_V1_SIZE;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (version <= 2)
		ສົ່ງຄືນ FDT_V2_SIZE;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (version <= 3)
		ສົ່ງຄືນ FDT_V3_SIZE;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (version <= 16)
		ສົ່ງຄືນ FDT_V16_SIZE;
	ບໍ່ດັ່ງນັ້ນ
		ສົ່ງຄືນ FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	ສົ່ງຄືນ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

ຈຳນວນເຕັມ fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ຖ້າ ((uintptr_t)fdt & 7)
		ສົ່ງຄືນ -FDT_ERR_ALIGNMENT;

	ຖ້າ (fdt_magic(fdt) != FDT_MAGIC)
		ສົ່ງຄືນ -FDT_ERR_BADMAGIC;
	ຖ້າ (!can_assume(LATEST)) {
		ຖ້າ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ສົ່ງຄືນ -FDT_ERR_BADVERSION;
		ຖ້າ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ສົ່ງຄືນ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ຖ້າ (!can_assume(VALID_DTB)) {

		ຖ້າ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ສົ່ງຄືນ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ຖ້າ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ສົ່ງຄືນ -FDT_ERR_TRUNCATED;
	}

	ຖ້າ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ຖ້າ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ຖ້າ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ສົ່ງຄືນ -FDT_ERR_TRUNCATED;
		} ບໍ່ດັ່ງນັ້ນ {
			ຖ້າ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ສົ່ງຄືນ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ຖ້າ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ສົ່ງຄືນ -FDT_ERR_TRUNCATED;
	}

	ສົ່ງຄືນ 0;
}

const void *fdt_offset_ptr(const void *fdt, ຈຳນວນເຕັມ offset, unsigned ຈຳນວນເຕັມ len)
{
	unsigned ຈຳນວນເຕັມ uoffset = offset;
	unsigned ຈຳນວນເຕັມ absoffset = offset + fdt_off_dt_struct(fdt);

	ຖ້າ (offset < 0)
		ສົ່ງຄືນ NULL;

	ຖ້າ (!can_assume(VALID_INPUT))
		ຖ້າ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ສົ່ງຄືນ NULL;

	ຖ້າ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ຖ້າ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ສົ່ງຄືນ NULL;

	ສົ່ງຄືນ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, ຈຳນວນເຕັມ startoffset, ຈຳນວນເຕັມ *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	ຈຳນວນເຕັມ offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ຖ້າ (!can_assume(VALID_DTB) && !tagp)
		ສົ່ງຄືນ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ໃນຂະນະທີ່ (p && (*p != '\0'));
		ຖ້າ (!can_assume(VALID_DTB) && !p)
			ສົ່ງຄືນ FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		ຖ້າ (!can_assume(VALID_DTB) && !lenp)
			ສົ່ງຄືນ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ຖ້າ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		ສົ່ງຄືນ FDT_END;
	}

	ຖ້າ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ສົ່ງຄືນ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ສົ່ງຄືນ tag;
}

ຈຳນວນເຕັມ fdt_check_node_offset_(const void *fdt, ຈຳນວນເຕັມ offset)
{
	ຖ້າ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ສົ່ງຄືນ -FDT_ERR_BADOFFSET;

	ຖ້າ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ສົ່ງຄືນ -FDT_ERR_BADOFFSET;

	ສົ່ງຄືນ offset;
}

ຈຳນວນເຕັມ fdt_check_prop_offset_(const void *fdt, ຈຳນວນເຕັມ offset)
{
	ຖ້າ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ສົ່ງຄືນ -FDT_ERR_BADOFFSET;

	ຖ້າ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ສົ່ງຄືນ -FDT_ERR_BADOFFSET;

	ສົ່ງຄືນ offset;
}

ຈຳນວນເຕັມ fdt_next_node(const void *fdt, ຈຳນວນເຕັມ offset, ຈຳນວນເຕັມ *depth)
{
	ຈຳນວນເຕັມ nextoffset = 0;
	uint32_t tag;

	ຖ້າ (offset >= 0)
		ຖ້າ ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ສົ່ງຄືນ nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			ຖ້າ (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			ຖ້າ (depth && ((--(*depth)) < 0))
				ສົ່ງຄືນ nextoffset;
			bha_nga;

		pak_sa FDT_END:
			ຖ້າ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ສົ່ງຄືນ -FDT_ERR_NOTFOUND;
			ບໍ່ດັ່ງນັ້ນ
				ສົ່ງຄືນ nextoffset;
		}
	} ໃນຂະນະທີ່ (tag != FDT_BEGIN_NODE);

	ສົ່ງຄືນ offset;
}

ຈຳນວນເຕັມ fdt_first_subnode(const void *fdt, ຈຳນວນເຕັມ offset)
{
	ຈຳນວນເຕັມ depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	ຖ້າ (offset < 0 || depth != 1)
		ສົ່ງຄືນ -FDT_ERR_NOTFOUND;

	ສົ່ງຄືນ offset;
}

ຈຳນວນເຕັມ fdt_next_subnode(const void *fdt, ຈຳນວນເຕັມ offset)
{
	ຈຳນວນເຕັມ depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	do {
		offset = fdt_next_node(fdt, offset, &depth);
		ຖ້າ (offset < 0 || depth < 1)
			ສົ່ງຄືນ -FDT_ERR_NOTFOUND;
	} ໃນຂະນະທີ່ (depth > 1);

	ສົ່ງຄືນ offset;
}

const var_na *fdt_find_string_(const var_na *strtab, ຈຳນວນເຕັມ tabsize, const var_na *s)
{
	ຈຳນວນເຕັມ len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	ສຳລັບ (p = strtab; p <= last; p++)
		ຖ້າ (memcmp(p, s, len) == 0)
			ສົ່ງຄືນ p;
	ສົ່ງຄືນ NULL;
}

ຈຳນວນເຕັມ fdt_move(const void *fdt, void *buf, ຈຳນວນເຕັມ bufsize)
{
	ຖ້າ (!can_assume(VALID_INPUT) && bufsize < 0)
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ຖ້າ (fdt_totalsize(fdt) > (unsigned ຈຳນວນເຕັມ)bufsize)
		ສົ່ງຄືນ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ສົ່ງຄືນ 0;
}
