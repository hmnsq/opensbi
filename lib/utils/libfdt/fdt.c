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

	ប្រសិនបើ (can_assume(VALID_DTB))
		ត្រឡប់ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ប្រសិនបើ ((uintptr_t)fdt & 7)
		ត្រឡប់ -FDT_ERR_ALIGNMENT;

	ប្រសិនបើ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ប្រសិនបើ (!can_assume(LATEST)) {
			ប្រសិនបើ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				ត្រឡប់ -FDT_ERR_BADVERSION;
			ប្រសិនបើ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				ត្រឡប់ -FDT_ERR_BADVERSION;
		}
	} បើមិនដូច្នេះ ប្រសិនបើ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ប្រសិនបើ (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			ត្រឡប់ -FDT_ERR_BADSTATE;
	} បើមិនដូច្នេះ {
		ត្រឡប់ -FDT_ERR_BADMAGIC;
	}

	ប្រសិនបើ (totalsize < INT32_MAX)
		ត្រឡប់ totalsize;
	បើមិនដូច្នេះ
		ត្រឡប់ -FDT_ERR_TRUNCATED;
}

static ចំនួនគត់ check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	ត្រឡប់ (off >= hdrsize) && (off <= totalsize);
}

static ចំនួនគត់ check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ប្រសិនបើ (!check_off_(hdrsize, totalsize, base))
		ត្រឡប់ 0; /* block start out of bounds */
	ប្រសិនបើ ((base + size) < base)
		ត្រឡប់ 0; /* overflow */
	ប្រសិនបើ (!check_off_(hdrsize, totalsize, base + size))
		ត្រឡប់ 0; /* block end out of bounds */
	ត្រឡប់ 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ប្រសិនបើ (version <= 1)
		ត្រឡប់ FDT_V1_SIZE;
	បើមិនដូច្នេះ ប្រសិនបើ (version <= 2)
		ត្រឡប់ FDT_V2_SIZE;
	បើមិនដូច្នេះ ប្រសិនបើ (version <= 3)
		ត្រឡប់ FDT_V3_SIZE;
	បើមិនដូច្នេះ ប្រសិនបើ (version <= 16)
		ត្រឡប់ FDT_V16_SIZE;
	បើមិនដូច្នេះ
		ត្រឡប់ FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	ត្រឡប់ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

ចំនួនគត់ fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ប្រសិនបើ ((uintptr_t)fdt & 7)
		ត្រឡប់ -FDT_ERR_ALIGNMENT;

	ប្រសិនបើ (fdt_magic(fdt) != FDT_MAGIC)
		ត្រឡប់ -FDT_ERR_BADMAGIC;
	ប្រសិនបើ (!can_assume(LATEST)) {
		ប្រសិនបើ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			ត្រឡប់ -FDT_ERR_BADVERSION;
		ប្រសិនបើ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			ត្រឡប់ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ប្រសិនបើ (!can_assume(VALID_DTB)) {

		ប្រសិនបើ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			ត្រឡប់ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ប្រសិនបើ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			ត្រឡប់ -FDT_ERR_TRUNCATED;
	}

	ប្រសិនបើ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ប្រសិនបើ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ប្រសិនបើ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				ត្រឡប់ -FDT_ERR_TRUNCATED;
		} បើមិនដូច្នេះ {
			ប្រសិនបើ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				ត្រឡប់ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ប្រសិនបើ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			ត្រឡប់ -FDT_ERR_TRUNCATED;
	}

	ត្រឡប់ 0;
}

const void *fdt_offset_ptr(const void *fdt, ចំនួនគត់ offset, unsigned ចំនួនគត់ len)
{
	unsigned ចំនួនគត់ uoffset = offset;
	unsigned ចំនួនគត់ absoffset = offset + fdt_off_dt_struct(fdt);

	ប្រសិនបើ (offset < 0)
		ត្រឡប់ NULL;

	ប្រសិនបើ (!can_assume(VALID_INPUT))
		ប្រសិនបើ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			ត្រឡប់ NULL;

	ប្រសិនបើ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ប្រសិនបើ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			ត្រឡប់ NULL;

	ត្រឡប់ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, ចំនួនគត់ startoffset, ចំនួនគត់ *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	ចំនួនគត់ offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ប្រសិនបើ (!can_assume(VALID_DTB) && !tagp)
		ត្រឡប់ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ខណៈពេល (p && (*p != '\0'));
		ប្រសិនបើ (!can_assume(VALID_DTB) && !p)
			ត្រឡប់ FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		ប្រសិនបើ (!can_assume(VALID_DTB) && !lenp)
			ត្រឡប់ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ប្រសិនបើ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		ត្រឡប់ FDT_END;
	}

	ប្រសិនបើ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		ត្រឡប់ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	ត្រឡប់ tag;
}

ចំនួនគត់ fdt_check_node_offset_(const void *fdt, ចំនួនគត់ offset)
{
	ប្រសិនបើ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ត្រឡប់ -FDT_ERR_BADOFFSET;

	ប្រសិនបើ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		ត្រឡប់ -FDT_ERR_BADOFFSET;

	ត្រឡប់ offset;
}

ចំនួនគត់ fdt_check_prop_offset_(const void *fdt, ចំនួនគត់ offset)
{
	ប្រសិនបើ (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		ត្រឡប់ -FDT_ERR_BADOFFSET;

	ប្រសិនបើ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		ត្រឡប់ -FDT_ERR_BADOFFSET;

	ត្រឡប់ offset;
}

ចំនួនគត់ fdt_next_node(const void *fdt, ចំនួនគត់ offset, ចំនួនគត់ *depth)
{
	ចំនួនគត់ nextoffset = 0;
	uint32_t tag;

	ប្រសិនបើ (offset >= 0)
		ប្រសិនបើ ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			ត្រឡប់ nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			ប្រសិនបើ (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			ប្រសិនបើ (depth && ((--(*depth)) < 0))
				ត្រឡប់ nextoffset;
			bha_nga;

		pak_sa FDT_END:
			ប្រសិនបើ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				ត្រឡប់ -FDT_ERR_NOTFOUND;
			បើមិនដូច្នេះ
				ត្រឡប់ nextoffset;
		}
	} ខណៈពេល (tag != FDT_BEGIN_NODE);

	ត្រឡប់ offset;
}

ចំនួនគត់ fdt_first_subnode(const void *fdt, ចំនួនគត់ offset)
{
	ចំនួនគត់ depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	ប្រសិនបើ (offset < 0 || depth != 1)
		ត្រឡប់ -FDT_ERR_NOTFOUND;

	ត្រឡប់ offset;
}

ចំនួនគត់ fdt_next_subnode(const void *fdt, ចំនួនគត់ offset)
{
	ចំនួនគត់ depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	do {
		offset = fdt_next_node(fdt, offset, &depth);
		ប្រសិនបើ (offset < 0 || depth < 1)
			ត្រឡប់ -FDT_ERR_NOTFOUND;
	} ខណៈពេល (depth > 1);

	ត្រឡប់ offset;
}

const var_na *fdt_find_string_(const var_na *strtab, ចំនួនគត់ tabsize, const var_na *s)
{
	ចំនួនគត់ len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	សម្រាប់ (p = strtab; p <= last; p++)
		ប្រសិនបើ (memcmp(p, s, len) == 0)
			ត្រឡប់ p;
	ត្រឡប់ NULL;
}

ចំនួនគត់ fdt_move(const void *fdt, void *buf, ចំនួនគត់ bufsize)
{
	ប្រសិនបើ (!can_assume(VALID_INPUT) && bufsize < 0)
		ត្រឡប់ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ប្រសិនបើ (fdt_totalsize(fdt) > (unsigned ចំនួនគត់)bufsize)
		ត្រឡប់ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	ត្រឡប់ 0;
}
