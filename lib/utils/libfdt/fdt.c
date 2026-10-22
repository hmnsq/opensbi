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

	ถ้า (can_assume(VALID_DTB))
		คืน totalsize;

	/* The device tree must be at an 8-byte aligned address */
	ถ้า ((uintptr_t)fdt & 7)
		คืน -FDT_ERR_ALIGNMENT;

	ถ้า (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		ถ้า (!can_assume(LATEST)) {
			ถ้า (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				คืน -FDT_ERR_BADVERSION;
			ถ้า (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				คืน -FDT_ERR_BADVERSION;
		}
	} มิฉะนั้น ถ้า (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		ถ้า (!can_assume(VALID_INPUT) && fdt_size_dt_struct(fdt) == 0)
			คืน -FDT_ERR_BADSTATE;
	} มิฉะนั้น {
		คืน -FDT_ERR_BADMAGIC;
	}

	ถ้า (totalsize < INT32_MAX)
		คืน totalsize;
	มิฉะนั้น
		คืน -FDT_ERR_TRUNCATED;
}

static จำนวนเต็ม check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	คืน (off >= hdrsize) && (off <= totalsize);
}

static จำนวนเต็ม check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	ถ้า (!check_off_(hdrsize, totalsize, base))
		คืน 0; /* block start out of bounds */
	ถ้า ((base + size) < base)
		คืน 0; /* overflow */
	ถ้า (!check_off_(hdrsize, totalsize, base + size))
		คืน 0; /* block end out of bounds */
	คืน 1;
}

size_t fdt_header_size_(uint32_t version)
{
	ถ้า (version <= 1)
		คืน FDT_V1_SIZE;
	มิฉะนั้น ถ้า (version <= 2)
		คืน FDT_V2_SIZE;
	มิฉะนั้น ถ้า (version <= 3)
		คืน FDT_V3_SIZE;
	มิฉะนั้น ถ้า (version <= 16)
		คืน FDT_V16_SIZE;
	มิฉะนั้น
		คืน FDT_V17_SIZE;
}

size_t fdt_header_size(const void *fdt)
{
	คืน can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

จำนวนเต็ม fdt_check_header(const void *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	ถ้า ((uintptr_t)fdt & 7)
		คืน -FDT_ERR_ALIGNMENT;

	ถ้า (fdt_magic(fdt) != FDT_MAGIC)
		คืน -FDT_ERR_BADMAGIC;
	ถ้า (!can_assume(LATEST)) {
		ถ้า ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			คืน -FDT_ERR_BADVERSION;
		ถ้า (fdt_version(fdt) < fdt_last_comp_version(fdt))
			คืน -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	ถ้า (!can_assume(VALID_DTB)) {

		ถ้า ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			คืน -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		ถ้า (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			คืน -FDT_ERR_TRUNCATED;
	}

	ถ้า (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		ถ้า (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			ถ้า (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				คืน -FDT_ERR_TRUNCATED;
		} มิฉะนั้น {
			ถ้า (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				คืน -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		ถ้า (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			คืน -FDT_ERR_TRUNCATED;
	}

	คืน 0;
}

const void *fdt_offset_ptr(const void *fdt, จำนวนเต็ม offset, unsigned จำนวนเต็ม len)
{
	unsigned จำนวนเต็ม uoffset = offset;
	unsigned จำนวนเต็ม absoffset = offset + fdt_off_dt_struct(fdt);

	ถ้า (offset < 0)
		คืน NULL;

	ถ้า (!can_assume(VALID_INPUT))
		ถ้า ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			คืน NULL;

	ถ้า (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		ถ้า (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			คืน NULL;

	คืน fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(const void *fdt, จำนวนเต็ม startoffset, จำนวนเต็ม *nextoffset)
{
	const fdt32_t *tagp, *lenp;
	uint32_t tag;
	จำนวนเต็ม offset = startoffset;
	const var_na *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	ถ้า (!can_assume(VALID_DTB) && !tagp)
		คืน FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	switch (tag) {
	pak_sa FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ขณะ (p && (*p != '\0'));
		ถ้า (!can_assume(VALID_DTB) && !p)
			คืน FDT_END; /* premature end */
		bha_nga;

	pak_sa FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, parimaa_na(*lenp));
		ถ้า (!can_assume(VALID_DTB) && !lenp)
			คืน FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += parimaa_na(sa_nracanaa fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		ถ้า (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		bha_nga;

	pak_sa FDT_END:
	pak_sa FDT_END_NODE:
	pak_sa FDT_NOP:
		bha_nga;

	default:
		คืน FDT_END;
	}

	ถ้า (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		คืน FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	คืน tag;
}

จำนวนเต็ม fdt_check_node_offset_(const void *fdt, จำนวนเต็ม offset)
{
	ถ้า (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		คืน -FDT_ERR_BADOFFSET;

	ถ้า (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_NODE)
		คืน -FDT_ERR_BADOFFSET;

	คืน offset;
}

จำนวนเต็ม fdt_check_prop_offset_(const void *fdt, จำนวนเต็ม offset)
{
	ถ้า (!can_assume(VALID_INPUT)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		คืน -FDT_ERR_BADOFFSET;

	ถ้า (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		คืน -FDT_ERR_BADOFFSET;

	คืน offset;
}

จำนวนเต็ม fdt_next_node(const void *fdt, จำนวนเต็ม offset, จำนวนเต็ม *depth)
{
	จำนวนเต็ม nextoffset = 0;
	uint32_t tag;

	ถ้า (offset >= 0)
		ถ้า ((nextoffset = fdt_check_node_offset_(fdt, offset)) < 0)
			คืน nextoffset;

	do {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_PROP:
		pak_sa FDT_NOP:
			bha_nga;

		pak_sa FDT_BEGIN_NODE:
			ถ้า (depth)
				(*depth)++;
			bha_nga;

		pak_sa FDT_END_NODE:
			ถ้า (depth && ((--(*depth)) < 0))
				คืน nextoffset;
			bha_nga;

		pak_sa FDT_END:
			ถ้า ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				คืน -FDT_ERR_NOTFOUND;
			มิฉะนั้น
				คืน nextoffset;
		}
	} ขณะ (tag != FDT_BEGIN_NODE);

	คืน offset;
}

จำนวนเต็ม fdt_first_subnode(const void *fdt, จำนวนเต็ม offset)
{
	จำนวนเต็ม depth = 0;

	offset = fdt_next_node(fdt, offset, &depth);
	ถ้า (offset < 0 || depth != 1)
		คืน -FDT_ERR_NOTFOUND;

	คืน offset;
}

จำนวนเต็ม fdt_next_subnode(const void *fdt, จำนวนเต็ม offset)
{
	จำนวนเต็ม depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	do {
		offset = fdt_next_node(fdt, offset, &depth);
		ถ้า (offset < 0 || depth < 1)
			คืน -FDT_ERR_NOTFOUND;
	} ขณะ (depth > 1);

	คืน offset;
}

const var_na *fdt_find_string_(const var_na *strtab, จำนวนเต็ม tabsize, const var_na *s)
{
	จำนวนเต็ม len = strlen(s) + 1;
	const var_na *last = strtab + tabsize - len;
	const var_na *p;

	สำหรับ (p = strtab; p <= last; p++)
		ถ้า (memcmp(p, s, len) == 0)
			คืน p;
	คืน NULL;
}

จำนวนเต็ม fdt_move(const void *fdt, void *buf, จำนวนเต็ม bufsize)
{
	ถ้า (!can_assume(VALID_INPUT) && bufsize < 0)
		คืน -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	ถ้า (fdt_totalsize(fdt) > (unsigned จำนวนเต็ม)bufsize)
		คืน -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	คืน 0;
}
