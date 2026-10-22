// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#അന്തര്ഭാവ "libfdt_env.h"

#അന്തര്ഭാവ <fdt.h>
#അന്തര്ഭാവ <libfdt.h>

#അന്തര്ഭാവ "libfdt_internal.h"

/*
 * Minimal sanity check for a read-only tree. fdt_ro_probe_() checks
 * that the given buffer contains what appears to be a flattened
 * device tree with sane information in its header.
 */
int32_t fdt_ro_probe_(സ്ഥിരം ശൂന്യം *fdt)
{
	uint32_t totalsize = fdt_totalsize(fdt);

	എങ്കിൽ (can_assume(VALID_DTB))
		തിരികെ totalsize;

	/* The device tree must be at an 8-byte aligned address */
	എങ്കിൽ ((uintptr_t)fdt & 7)
		തിരികെ -FDT_ERR_ALIGNMENT;

	എങ്കിൽ (fdt_magic(fdt) == FDT_MAGIC) {
		/* Complete tree */
		എങ്കിൽ (!can_assume(LATEST)) {
			എങ്കിൽ (fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
				തിരികെ -FDT_ERR_BADVERSION;
			എങ്കിൽ (fdt_last_comp_version(fdt) >
					FDT_LAST_SUPPORTED_VERSION)
				തിരികെ -FDT_ERR_BADVERSION;
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		/* Unfinished sequential-write blob */
		എങ്കിൽ (!can_assume(VALID_നിവേശം) && fdt_size_dt_struct(fdt) == 0)
			തിരികെ -FDT_ERR_BADSTATE;
	} അല്ലെങ്കിൽ {
		തിരികെ -FDT_ERR_BADMAGIC;
	}

	എങ്കിൽ (totalsize < INT32_MAX)
		തിരികെ totalsize;
	അല്ലെങ്കിൽ
		തിരികെ -FDT_ERR_TRUNCATED;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ check_off_(uint32_t hdrsize, uint32_t totalsize, uint32_t off)
{
	തിരികെ (off >= hdrsize) && (off <= totalsize);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ check_block_(uint32_t hdrsize, uint32_t totalsize,
			uint32_t base, uint32_t size)
{
	എങ്കിൽ (!check_off_(hdrsize, totalsize, base))
		തിരികെ 0; /* block start out of bounds */
	എങ്കിൽ ((base + size) < base)
		തിരികെ 0; /* overflow */
	എങ്കിൽ (!check_off_(hdrsize, totalsize, base + size))
		തിരികെ 0; /* block end out of bounds */
	തിരികെ 1;
}

size_t fdt_header_size_(uint32_t version)
{
	എങ്കിൽ (version <= 1)
		തിരികെ FDT_V1_SIZE;
	അല്ലെങ്കിൽ എങ്കിൽ (version <= 2)
		തിരികെ FDT_V2_SIZE;
	അല്ലെങ്കിൽ എങ്കിൽ (version <= 3)
		തിരികെ FDT_V3_SIZE;
	അല്ലെങ്കിൽ എങ്കിൽ (version <= 16)
		തിരികെ FDT_V16_SIZE;
	അല്ലെങ്കിൽ
		തിരികെ FDT_V17_SIZE;
}

size_t fdt_header_size(സ്ഥിരം ശൂന്യം *fdt)
{
	തിരികെ can_assume(LATEST) ? FDT_V17_SIZE :
		fdt_header_size_(fdt_version(fdt));
}

പൂർണ്ണസംഖ്യ fdt_check_header(സ്ഥിരം ശൂന്യം *fdt)
{
	size_t hdrsize;

	/* The device tree must be at an 8-byte aligned address */
	എങ്കിൽ ((uintptr_t)fdt & 7)
		തിരികെ -FDT_ERR_ALIGNMENT;

	എങ്കിൽ (fdt_magic(fdt) != FDT_MAGIC)
		തിരികെ -FDT_ERR_BADMAGIC;
	എങ്കിൽ (!can_assume(LATEST)) {
		എങ്കിൽ ((fdt_version(fdt) < FDT_FIRST_SUPPORTED_VERSION)
		    || (fdt_last_comp_version(fdt) >
			FDT_LAST_SUPPORTED_VERSION))
			തിരികെ -FDT_ERR_BADVERSION;
		എങ്കിൽ (fdt_version(fdt) < fdt_last_comp_version(fdt))
			തിരികെ -FDT_ERR_BADVERSION;
	}
	hdrsize = fdt_header_size(fdt);
	എങ്കിൽ (!can_assume(VALID_DTB)) {

		എങ്കിൽ ((fdt_totalsize(fdt) < hdrsize)
		    || (fdt_totalsize(fdt) > INT_MAX))
			തിരികെ -FDT_ERR_TRUNCATED;

		/* Bounds check memrsv block */
		എങ്കിൽ (!check_off_(hdrsize, fdt_totalsize(fdt),
				fdt_off_mem_rsvmap(fdt)))
			തിരികെ -FDT_ERR_TRUNCATED;
	}

	എങ്കിൽ (!can_assume(VALID_DTB)) {
		/* Bounds check structure block */
		എങ്കിൽ (!can_assume(LATEST) && fdt_version(fdt) < 17) {
			എങ്കിൽ (!check_off_(hdrsize, fdt_totalsize(fdt),
					fdt_off_dt_struct(fdt)))
				തിരികെ -FDT_ERR_TRUNCATED;
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (!check_block_(hdrsize, fdt_totalsize(fdt),
					  fdt_off_dt_struct(fdt),
					  fdt_size_dt_struct(fdt)))
				തിരികെ -FDT_ERR_TRUNCATED;
		}

		/* Bounds check strings block */
		എങ്കിൽ (!check_block_(hdrsize, fdt_totalsize(fdt),
				  fdt_off_dt_strings(fdt),
				  fdt_size_dt_strings(fdt)))
			തിരികെ -FDT_ERR_TRUNCATED;
	}

	തിരികെ 0;
}

സ്ഥിരം ശൂന്യം *fdt_offset_ptr(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ offset, unsigned പൂർണ്ണസംഖ്യ len)
{
	unsigned പൂർണ്ണസംഖ്യ uoffset = offset;
	unsigned പൂർണ്ണസംഖ്യ absoffset = offset + fdt_off_dt_struct(fdt);

	എങ്കിൽ (offset < 0)
		തിരികെ ശൂന്യ;

	എങ്കിൽ (!can_assume(VALID_നിവേശം))
		എങ്കിൽ ((absoffset < uoffset)
		    || ((absoffset + len) < absoffset)
		    || (absoffset + len) > fdt_totalsize(fdt))
			തിരികെ ശൂന്യ;

	എങ്കിൽ (can_assume(LATEST) || fdt_version(fdt) >= 0x11)
		എങ്കിൽ (((uoffset + len) < uoffset)
		    || ((offset + len) > fdt_size_dt_struct(fdt)))
			തിരികെ ശൂന്യ;

	തിരികെ fdt_offset_ptr_(fdt, offset);
}

uint32_t fdt_next_tag(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ startoffset, പൂർണ്ണസംഖ്യ *nextoffset)
{
	സ്ഥിരം fdt32_t *tagp, *lenp;
	uint32_t tag;
	പൂർണ്ണസംഖ്യ offset = startoffset;
	സ്ഥിരം അക്ഷരം *p;

	*nextoffset = -FDT_ERR_TRUNCATED;
	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	എങ്കിൽ (!can_assume(VALID_DTB) && !tagp)
		തിരികെ FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	*nextoffset = -FDT_ERR_BADSTRUCTURE;
	തിരഞ്ഞെടുപ്പ് (tag) {
	അവസ്ഥ FDT_BEGIN_കണ്ണി:
		/* skip name */
		ചെയ്യുക {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} ആകുമ്പോൾ (p && (*p != '\0'));
		എങ്കിൽ (!can_assume(VALID_DTB) && !p)
			തിരികെ FDT_END; /* premature end */
		നിർത്തുക;

	അവസ്ഥ FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset, വലുപ്പം(*lenp));
		എങ്കിൽ (!can_assume(VALID_DTB) && !lenp)
			തിരികെ FDT_END; /* premature end */
		/* skip-name offset, length and value */
		offset += വലുപ്പം(ഘടന fdt_property) - FDT_TAGSIZE
			+ fdt32_to_cpu(*lenp);
		എങ്കിൽ (!can_assume(LATEST) &&
		    fdt_version(fdt) < 0x10 && fdt32_to_cpu(*lenp) >= 8 &&
		    ((offset - fdt32_to_cpu(*lenp)) % 8) != 0)
			offset += 4;
		നിർത്തുക;

	അവസ്ഥ FDT_END:
	അവസ്ഥ FDT_END_കണ്ണി:
	അവസ്ഥ FDT_NOP:
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		തിരികെ FDT_END;
	}

	എങ്കിൽ (!fdt_offset_ptr(fdt, startoffset, offset - startoffset))
		തിരികെ FDT_END; /* premature end */

	*nextoffset = FDT_TAGALIGN(offset);
	തിരികെ tag;
}

പൂർണ്ണസംഖ്യ fdt_check_കണ്ണി_offset(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ offset)
{
	എങ്കിൽ (!can_assume(VALID_നിവേശം)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		തിരികെ -FDT_ERR_BADOFFSET;

	എങ്കിൽ (fdt_next_tag(fdt, offset, &offset) != FDT_BEGIN_കണ്ണി)
		തിരികെ -FDT_ERR_BADOFFSET;

	തിരികെ offset;
}

പൂർണ്ണസംഖ്യ fdt_check_prop_offset_(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ offset)
{
	എങ്കിൽ (!can_assume(VALID_നിവേശം)
	    && ((offset < 0) || (offset % FDT_TAGSIZE)))
		തിരികെ -FDT_ERR_BADOFFSET;

	എങ്കിൽ (fdt_next_tag(fdt, offset, &offset) != FDT_PROP)
		തിരികെ -FDT_ERR_BADOFFSET;

	തിരികെ offset;
}

പൂർണ്ണസംഖ്യ fdt_next_കണ്ണി(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ *depth)
{
	പൂർണ്ണസംഖ്യ nextoffset = 0;
	uint32_t tag;

	എങ്കിൽ (offset >= 0)
		എങ്കിൽ ((nextoffset = fdt_check_കണ്ണി_offset(fdt, offset)) < 0)
			തിരികെ nextoffset;

	ചെയ്യുക {
		offset = nextoffset;
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		തിരഞ്ഞെടുപ്പ് (tag) {
		അവസ്ഥ FDT_PROP:
		അവസ്ഥ FDT_NOP:
			നിർത്തുക;

		അവസ്ഥ FDT_BEGIN_കണ്ണി:
			എങ്കിൽ (depth)
				(*depth)++;
			നിർത്തുക;

		അവസ്ഥ FDT_END_കണ്ണി:
			എങ്കിൽ (depth && ((--(*depth)) < 0))
				തിരികെ nextoffset;
			നിർത്തുക;

		അവസ്ഥ FDT_END:
			എങ്കിൽ ((nextoffset >= 0)
			    || ((nextoffset == -FDT_ERR_TRUNCATED) && !depth))
				തിരികെ -FDT_ERR_NOTFOUND;
			അല്ലെങ്കിൽ
				തിരികെ nextoffset;
		}
	} ആകുമ്പോൾ (tag != FDT_BEGIN_കണ്ണി);

	തിരികെ offset;
}

പൂർണ്ണസംഖ്യ fdt_first_subnode(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ offset)
{
	പൂർണ്ണസംഖ്യ depth = 0;

	offset = fdt_next_കണ്ണി(fdt, offset, &depth);
	എങ്കിൽ (offset < 0 || depth != 1)
		തിരികെ -FDT_ERR_NOTFOUND;

	തിരികെ offset;
}

പൂർണ്ണസംഖ്യ fdt_next_subnode(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ offset)
{
	പൂർണ്ണസംഖ്യ depth = 1;

	/*
	 * With respect to the parent, the depth of the next subnode will be
	 * the same as the last.
	 */
	ചെയ്യുക {
		offset = fdt_next_കണ്ണി(fdt, offset, &depth);
		എങ്കിൽ (offset < 0 || depth < 1)
			തിരികെ -FDT_ERR_NOTFOUND;
	} ആകുമ്പോൾ (depth > 1);

	തിരികെ offset;
}

സ്ഥിരം അക്ഷരം *fdt_find_string_(സ്ഥിരം അക്ഷരം *strtab, പൂർണ്ണസംഖ്യ tabsize, സ്ഥിരം അക്ഷരം *s)
{
	പൂർണ്ണസംഖ്യ len = strlen(s) + 1;
	സ്ഥിരം അക്ഷരം *last = strtab + tabsize - len;
	സ്ഥിരം അക്ഷരം *p;

	വേണ്ടി (p = strtab; p <= last; p++)
		എങ്കിൽ (memcmp(p, s, len) == 0)
			തിരികെ p;
	തിരികെ ശൂന്യ;
}

പൂർണ്ണസംഖ്യ fdt_move(സ്ഥിരം ശൂന്യം *fdt, ശൂന്യം *buf, പൂർണ്ണസംഖ്യ bufsize)
{
	എങ്കിൽ (!can_assume(VALID_നിവേശം) && bufsize < 0)
		തിരികെ -FDT_ERR_NOSPACE;

	FDT_RO_PROBE(fdt);

	എങ്കിൽ (fdt_totalsize(fdt) > (unsigned പൂർണ്ണസംഖ്യ)bufsize)
		തിരികെ -FDT_ERR_NOSPACE;

	memmove(buf, fdt, fdt_totalsize(fdt));
	തിരികെ 0;
}
