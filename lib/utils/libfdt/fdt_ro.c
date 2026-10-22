// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#ಅನ್ತರ್ಭಾವ "libfdt_env.h"

#ಅನ್ತರ್ಭಾವ <fdt.h>
#ಅನ್ತರ್ಭಾವ <libfdt.h>

#ಅನ್ತರ್ಭಾವ "libfdt_internal.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_nodename_eq_(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ offset,
			    ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ len)
{
	ಪೂರ್ಣಾಙ್ಕ olen;
	ಧ್ರುವ ವರ್ಣ *p = fdt_get_name(fdt, offset, &olen);

	ಯದಿ (!p || olen < len)
		/* short match */
		ಪ್ರತ್ಯಾಗಮ 0;

	ಯದಿ (memcmp(p, s, len) != 0)
		ಪ್ರತ್ಯಾಗಮ 0;

	ಯದಿ (p[len] == '\0')
		ಪ್ರತ್ಯಾಗಮ 1;
	ಅನ್ಯಥಾ ಯದಿ (!memchr(s, '@', len) && (p[len] == '@'))
		ಪ್ರತ್ಯಾಗಮ 1;
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ 0;
}

ಧ್ರುವ ವರ್ಣ *fdt_get_string(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ stroffset, ಪೂರ್ಣಾಙ್ಕ *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	ಪೂರ್ಣಾಙ್ಕ err;
	ಧ್ರುವ ವರ್ಣ *s, *n;

	ಯದಿ (can_assume(VALID_ಒಳಹರಿವು)) {
		s = (ಧ್ರುವ ವರ್ಣ *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		ಯದಿ (lenp)
			*lenp = strlen(s);
		ಪ್ರತ್ಯಾಗಮ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ಯದಿ (totalsize < 0)
		ಗಚ್ಛ fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	ಯದಿ (absoffset >= (unsigned)totalsize)
		ಗಚ್ಛ fail;
	len = totalsize - absoffset;

	ಯದಿ (fdt_magic(fdt) == FDT_MAGIC) {
		ಯದಿ (stroffset < 0)
			ಗಚ್ಛ fail;
		ಯದಿ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			ಯದಿ ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				ಗಚ್ಛ fail;
			ಯದಿ ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} ಅನ್ಯಥಾ ಯದಿ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned ಪೂರ್ಣಾಙ್ಕ sw_stroffset = -stroffset;

		ಯದಿ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			ಗಚ್ಛ fail;
		ಯದಿ (sw_stroffset < len)
			len = sw_stroffset;
	} ಅನ್ಯಥಾ {
		err = -FDT_ERR_INTERNAL;
		ಗಚ್ಛ fail;
	}

	s = (ಧ್ರುವ ವರ್ಣ *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ಯದಿ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		ಗಚ್ಛ fail;
	}

	ಯದಿ (lenp)
		*lenp = n - s;
	ಪ್ರತ್ಯಾಗಮ s;

fail:
	ಯದಿ (lenp)
		*lenp = err;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಧ್ರುವ ವರ್ಣ *fdt_string(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ stroffset)
{
	ಪ್ರತ್ಯಾಗಮ fdt_get_string(fdt, stroffset, ರಿಕ್ತ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fdt_string_eq_(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ stroffset,
			  ಧ್ರುವ ವರ್ಣ *s, ಪೂರ್ಣಾಙ್ಕ len)
{
	ಪೂರ್ಣಾಙ್ಕ slen;
	ಧ್ರುವ ವರ್ಣ *p = fdt_get_string(fdt, stroffset, &slen);

	ಪ್ರತ್ಯಾಗಮ p && (slen == len) && (memcmp(p, s, len) == 0);
}

ಪೂರ್ಣಾಙ್ಕ fdt_find_max_phandle(ಧ್ರುವ ಶೂನ್ಯ *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	ಪೂರ್ಣಾಙ್ಕ offset = -1;

	ಯಾವತ್ (true) {
		uint32_t ಮೌಲ್ಯ;

		offset = fdt_next_ಗಂಟು(fdt, offset, ರಿಕ್ತ);
		ಯದಿ (offset < 0) {
			ಯದಿ (offset == -FDT_ERR_NOTFOUND)
				ಭಙ್ಗ;

			ಪ್ರತ್ಯಾಗಮ offset;
		}

		ಮೌಲ್ಯ = fdt_get_phandle(fdt, offset);

		ಯದಿ (ಮೌಲ್ಯ > max)
			max = ಮೌಲ್ಯ;
	}

	ಯದಿ (phandle)
		*phandle = max;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_generate_phandle(ಧ್ರುವ ಶೂನ್ಯ *fdt, uint32_t *phandle)
{
	uint32_t max;
	ಪೂರ್ಣಾಙ್ಕ err;

	err = fdt_find_max_phandle(fdt, &max);
	ಯದಿ (err < 0)
		ಪ್ರತ್ಯಾಗಮ err;

	ಯದಿ (max == FDT_MAX_PHANDLE)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOPHANDLES;

	ಯದಿ (phandle)
		*phandle = max + 1;

	ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_reserve_entry *fdt_mem_rsv(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ n)
{
	unsigned ಪೂರ್ಣಾಙ್ಕ offset = n * ಪರಿಮಾಣ(ಸಂರಚನಾ fdt_reserve_entry);
	unsigned ಪೂರ್ಣಾಙ್ಕ absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	ಯದಿ (!can_assume(VALID_ಒಳಹರಿವು)) {
		ಯದಿ (absoffset < fdt_off_mem_rsvmap(fdt))
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		ಯದಿ (absoffset > fdt_totalsize(fdt) -
		    ಪರಿಮಾಣ(ಸಂರಚನಾ fdt_reserve_entry))
			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಪ್ರತ್ಯಾಗಮ fdt_mem_rsv_(fdt, n);
}

ಪೂರ್ಣಾಙ್ಕ fdt_get_mem_rsv(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ n, uint64_t *address, uint64_t *size)
{
	ಧ್ರುವ ಸಂರಚನಾ fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ಯದಿ (!can_assume(VALID_ಒಳಹರಿವು) && !re)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_ಅಂಕೆ_mem_rsv(ಧ್ರುವ ಶೂನ್ಯ *fdt)
{
	ಪೂರ್ಣಾಙ್ಕ i;
	ಧ್ರುವ ಸಂರಚನಾ fdt_reserve_entry *re;

	ಪ್ರತಿ (i = 0; (re = fdt_mem_rsv(fdt, i)) != ರಿಕ್ತ; i++) {
		ಯದಿ (fdt64_ld_(&re->size) == 0)
			ಪ್ರತ್ಯಾಗಮ i;
	}
	ಪ್ರತ್ಯಾಗಮ -FDT_ERR_TRUNCATED;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ nextprop_(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ offset)
{
	uint32_t tag;
	ಪೂರ್ಣಾಙ್ಕ nextoffset;

	ಕುರು {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		ವಿಕಲ್ಪ (tag) {
		ಪಕ್ಷ FDT_END:
			ಯದಿ (nextoffset >= 0)
				ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADSTRUCTURE;
			ಅನ್ಯಥಾ
				ಪ್ರತ್ಯಾಗಮ nextoffset;

		ಪಕ್ಷ FDT_PROP:
			ಪ್ರತ್ಯಾಗಮ offset;
		}
		offset = nextoffset;
	} ಯಾವತ್ (tag == FDT_NOP);

	ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOTFOUND;
}

ಪೂರ್ಣಾಙ್ಕ fdt_subnode_offset_namelen(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ offset,
			       ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ namelen)
{
	ಪೂರ್ಣಾಙ್ಕ depth;

	FDT_RO_PROBE(fdt);

	ಪ್ರತಿ (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_ಗಂಟು(fdt, offset, &depth))
		ಯದಿ ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ಪ್ರತ್ಯಾಗಮ offset;

	ಯದಿ (depth < 0)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOTFOUND;
	ಪ್ರತ್ಯಾಗಮ offset; /* error */
}

ಪೂರ್ಣಾಙ್ಕ fdt_subnode_offset(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ parentoffset,
		       ಧ್ರುವ ವರ್ಣ *name)
{
	ಪ್ರತ್ಯಾಗಮ fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

ಪೂರ್ಣಾಙ್ಕ fdt_path_offset_namelen(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ namelen)
{
	ಧ್ರುವ ವರ್ಣ *end = path + namelen;
	ಧ್ರುವ ವರ್ಣ *p = path;
	ಪೂರ್ಣಾಙ್ಕ offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ಯದಿ (*path != '/') {
		ಧ್ರುವ ವರ್ಣ *q = memchr(path, '/', end - p);

		ಯದಿ (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ಯದಿ (!p)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	ಯಾವತ್ (p < end) {
		ಧ್ರುವ ವರ್ಣ *q;

		ಯಾವತ್ (*p == '/') {
			p++;
			ಯದಿ (p == end)
				ಪ್ರತ್ಯಾಗಮ offset;
		}
		q = memchr(p, '/', end - p);
		ಯದಿ (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		ಯದಿ (offset < 0)
			ಪ್ರತ್ಯಾಗಮ offset;

		p = q;
	}

	ಪ್ರತ್ಯಾಗಮ offset;
}

ಪೂರ್ಣಾಙ್ಕ fdt_path_offset(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಧ್ರುವ ವರ್ಣ *path)
{
	ಪ್ರತ್ಯಾಗಮ fdt_path_offset_namelen(fdt, path, strlen(path));
}

ಧ್ರುವ ವರ್ಣ *fdt_get_name(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, ಪೂರ್ಣಾಙ್ಕ *len)
{
	ಧ್ರುವ ಸಂರಚನಾ fdt_ಗಂಟು_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	ಧ್ರುವ ವರ್ಣ *nameptr;
	ಪೂರ್ಣಾಙ್ಕ err;

	ಯದಿ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_ಗಂಟು_offset(fdt, nodeoffset)) < 0))
			ಗಚ್ಛ fail;

	nameptr = nh->name;

	ಯದಿ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ಧ್ರುವ ವರ್ಣ *leaf;
		leaf = strrchr(nameptr, '/');
		ಯದಿ (leaf == ರಿಕ್ತ) {
			err = -FDT_ERR_BADSTRUCTURE;
			ಗಚ್ಛ fail;
		}
		nameptr = leaf+1;
	}

	ಯದಿ (len)
		*len = strlen(nameptr);

	ಪ್ರತ್ಯಾಗಮ nameptr;

 fail:
	ಯದಿ (len)
		*len = err;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ fdt_first_property_offset(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset)
{
	ಪೂರ್ಣಾಙ್ಕ offset;

	ಯದಿ ((offset = fdt_check_ಗಂಟು_offset(fdt, nodeoffset)) < 0)
		ಪ್ರತ್ಯಾಗಮ offset;

	ಪ್ರತ್ಯಾಗಮ nextprop_(fdt, offset);
}

ಪೂರ್ಣಾಙ್ಕ fdt_next_property_offset(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ offset)
{
	ಯದಿ ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ಪ್ರತ್ಯಾಗಮ offset;

	ಪ್ರತ್ಯಾಗಮ nextprop_(fdt, offset);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_property *fdt_get_property_by_offset_(ಧ್ರುವ ಶೂನ್ಯ *fdt,
						              ಪೂರ್ಣಾಙ್ಕ offset,
						              ಪೂರ್ಣಾಙ್ಕ *lenp)
{
	ಪೂರ್ಣಾಙ್ಕ err;
	ಧ್ರುವ ಸಂರಚನಾ fdt_property *prop;

	ಯದಿ (!can_assume(VALID_ಒಳಹರಿವು) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		ಯದಿ (lenp)
			*lenp = err;
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	ಯದಿ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ಪ್ರತ್ಯಾಗಮ prop;
}

ಧ್ರುವ ಸಂರಚನಾ fdt_property *fdt_get_property_by_offset(ಧ್ರುವ ಶೂನ್ಯ *fdt,
						      ಪೂರ್ಣಾಙ್ಕ offset,
						      ಪೂರ್ಣಾಙ್ಕ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ಯದಿ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ಯದಿ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಪ್ರತ್ಯಾಗಮ fdt_get_property_by_offset_(fdt, offset, lenp);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fdt_property *fdt_get_property_namelen_(ಧ್ರುವ ಶೂನ್ಯ *fdt,
						            ಪೂರ್ಣಾಙ್ಕ offset,
						            ಧ್ರುವ ವರ್ಣ *name,
						            ಪೂರ್ಣಾಙ್ಕ namelen,
							    ಪೂರ್ಣಾಙ್ಕ *lenp,
							    ಪೂರ್ಣಾಙ್ಕ *poffset)
{
	ಪ್ರತಿ (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		ಧ್ರುವ ಸಂರಚನಾ fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		ಯದಿ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			ಭಙ್ಗ;
		}
		ಯದಿ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ಯದಿ (poffset)
				*poffset = offset;
			ಪ್ರತ್ಯಾಗಮ prop;
		}
	}

	ಯದಿ (lenp)
		*lenp = offset;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}


ಧ್ರುವ ಸಂರಚನಾ fdt_property *fdt_get_property_namelen(ಧ್ರುವ ಶೂನ್ಯ *fdt,
						    ಪೂರ್ಣಾಙ್ಕ offset,
						    ಧ್ರುವ ವರ್ಣ *name,
						    ಪೂರ್ಣಾಙ್ಕ namelen, ಪೂರ್ಣಾಙ್ಕ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ಯದಿ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ಯದಿ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	ಪ್ರತ್ಯಾಗಮ fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 ರಿಕ್ತ);
}


ಧ್ರುವ ಸಂರಚನಾ fdt_property *fdt_get_property(ಧ್ರುವ ಶೂನ್ಯ *fdt,
					    ಪೂರ್ಣಾಙ್ಕ nodeoffset,
					    ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ *lenp)
{
	ಪ್ರತ್ಯಾಗಮ fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

ಧ್ರುವ ಶೂನ್ಯ *fdt_getprop_namelen(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
				ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ namelen, ಪೂರ್ಣಾಙ್ಕ *lenp)
{
	ಪೂರ್ಣಾಙ್ಕ poffset;
	ಧ್ರುವ ಸಂರಚನಾ fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ಯದಿ (!prop)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	/* Handle realignment */
	ಯದಿ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + ಪರಿಮಾಣ(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ಪ್ರತ್ಯಾಗಮ prop->data + 4;
	ಪ್ರತ್ಯಾಗಮ prop->data;
}

ಧ್ರುವ ಶೂನ್ಯ *fdt_getprop_by_offset(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ offset,
				  ಧ್ರುವ ವರ್ಣ **namep, ಪೂರ್ಣಾಙ್ಕ *lenp)
{
	ಧ್ರುವ ಸಂರಚನಾ fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	ಯದಿ (!prop)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (namep) {
		ಧ್ರುವ ವರ್ಣ *name;
		ಪೂರ್ಣಾಙ್ಕ namelen;

		ಯದಿ (!can_assume(VALID_ಒಳಹರಿವು)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ಯದಿ (!name) {
				ಯದಿ (lenp)
					*lenp = namelen;
				ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
			}
			*namep = name;
		} ಅನ್ಯಥಾ {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ಯದಿ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + ಪರಿಮಾಣ(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ಪ್ರತ್ಯಾಗಮ prop->data + 4;
	ಪ್ರತ್ಯಾಗಮ prop->data;
}

ಧ್ರುವ ಶೂನ್ಯ *fdt_getprop(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
			ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ *lenp)
{
	ಪ್ರತ್ಯಾಗಮ fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset)
{
	ಧ್ರುವ fdt32_t *php;
	ಪೂರ್ಣಾಙ್ಕ len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ಯದಿ (!php || (len != ಪರಿಮಾಣ(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ಯದಿ (!php || (len != ಪರಿಮಾಣ(*php)))
			ಪ್ರತ್ಯಾಗಮ 0;
	}

	ಪ್ರತ್ಯಾಗಮ fdt32_ld_(php);
}

ಧ್ರುವ ವರ್ಣ *fdt_get_alias_namelen(ಧ್ರುವ ಶೂನ್ಯ *fdt,
				  ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ namelen)
{
	ಪೂರ್ಣಾಙ್ಕ aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	ಯದಿ (aliasoffset < 0)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ಪ್ರತ್ಯಾಗಮ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, ರಿಕ್ತ);
}

ಧ್ರುವ ವರ್ಣ *fdt_get_alias(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಧ್ರುವ ವರ್ಣ *name)
{
	ಪ್ರತ್ಯಾಗಮ fdt_get_alias_namelen(fdt, name, strlen(name));
}

ಪೂರ್ಣಾಙ್ಕ fdt_get_path(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, ವರ್ಣ *buf, ಪೂರ್ಣಾಙ್ಕ buflen)
{
	ಪೂರ್ಣಾಙ್ಕ pdepth = 0, p = 0;
	ಪೂರ್ಣಾಙ್ಕ offset, depth, namelen;
	ಧ್ರುವ ವರ್ಣ *name;

	FDT_RO_PROBE(fdt);

	ಯದಿ (buflen < 2)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;

	ಪ್ರತಿ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_ಗಂಟು(fdt, offset, &depth)) {
		ಯಾವತ್ (pdepth > depth) {
			ಕುರು {
				p--;
			} ಯಾವತ್ (buf[p-1] != '/');
			pdepth--;
		}

		ಯದಿ (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			ಯದಿ (!name)
				ಪ್ರತ್ಯಾಗಮ namelen;
			ಯದಿ ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ಯದಿ (offset == nodeoffset) {
			ಯದಿ (pdepth < (depth + 1))
				ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOSPACE;

			ಯದಿ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ಪ್ರತ್ಯಾಗಮ 0;
		}
	}

	ಯದಿ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOFFSET;
	ಅನ್ಯಥಾ ಯದಿ (offset == -FDT_ERR_BADOFFSET)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADSTRUCTURE;

	ಪ್ರತ್ಯಾಗಮ offset; /* error from fdt_next_node() */
}

ಪೂರ್ಣಾಙ್ಕ fdt_supernode_atdepth_offset(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
				 ಪೂರ್ಣಾಙ್ಕ supernodedepth, ಪೂರ್ಣಾಙ್ಕ *nodedepth)
{
	ಪೂರ್ಣಾಙ್ಕ offset, depth;
	ಪೂರ್ಣಾಙ್ಕ supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	ಯದಿ (supernodedepth < 0)
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOTFOUND;

	ಪ್ರತಿ (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_ಗಂಟು(fdt, offset, &depth)) {
		ಯದಿ (depth == supernodedepth)
			supernodeoffset = offset;

		ಯದಿ (offset == nodeoffset) {
			ಯದಿ (nodedepth)
				*nodedepth = depth;

			ಯದಿ (supernodedepth > depth)
				ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOTFOUND;
			ಅನ್ಯಥಾ
				ಪ್ರತ್ಯಾಗಮ supernodeoffset;
		}
	}

	ಯದಿ (!can_assume(VALID_ಒಳಹರಿವು)) {
		ಯದಿ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADOFFSET;
		ಅನ್ಯಥಾ ಯದಿ (offset == -FDT_ERR_BADOFFSET)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADSTRUCTURE;
	}

	ಪ್ರತ್ಯಾಗಮ offset; /* error from fdt_next_node() */
}

ಪೂರ್ಣಾಙ್ಕ fdt_ಗಂಟು_depth(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset)
{
	ಪೂರ್ಣಾಙ್ಕ nodedepth;
	ಪೂರ್ಣಾಙ್ಕ err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	ಯದಿ (err)
		ಪ್ರತ್ಯಾಗಮ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ಪ್ರತ್ಯಾಗಮ nodedepth;
}

ಪೂರ್ಣಾಙ್ಕ fdt_parent_offset(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset)
{
	ಪೂರ್ಣಾಙ್ಕ nodedepth = fdt_ಗಂಟು_depth(fdt, nodeoffset);

	ಯದಿ (nodedepth < 0)
		ಪ್ರತ್ಯಾಗಮ nodedepth;
	ಪ್ರತ್ಯಾಗಮ fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ fdt_ಗಂಟು_offset_by_prop_ಮೌಲ್ಯ(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ startoffset,
				  ಧ್ರುವ ವರ್ಣ *propname,
				  ಧ್ರುವ ಶೂನ್ಯ *propval, ಪೂರ್ಣಾಙ್ಕ proplen)
{
	ಪೂರ್ಣಾಙ್ಕ offset;
	ಧ್ರುವ ಶೂನ್ಯ *val;
	ಪೂರ್ಣಾಙ್ಕ len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	ಪ್ರತಿ (offset = fdt_next_ಗಂಟು(fdt, startoffset, ರಿಕ್ತ);
	     offset >= 0;
	     offset = fdt_next_ಗಂಟು(fdt, offset, ರಿಕ್ತ)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		ಯದಿ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ಪ್ರತ್ಯಾಗಮ offset;
	}

	ಪ್ರತ್ಯಾಗಮ offset; /* error from fdt_next_node() */
}

ಪೂರ್ಣಾಙ್ಕ fdt_ಗಂಟು_offset_by_phandle(ಧ್ರುವ ಶೂನ್ಯ *fdt, uint32_t phandle)
{
	ಪೂರ್ಣಾಙ್ಕ offset;

	ಯದಿ ((phandle == 0) || (phandle == ~0U))
		ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	ಪ್ರತಿ (offset = fdt_next_ಗಂಟು(fdt, -1, ರಿಕ್ತ);
	     offset >= 0;
	     offset = fdt_next_ಗಂಟು(fdt, offset, ರಿಕ್ತ)) {
		ಯದಿ (fdt_get_phandle(fdt, offset) == phandle)
			ಪ್ರತ್ಯಾಗಮ offset;
	}

	ಪ್ರತ್ಯಾಗಮ offset; /* error from fdt_next_node() */
}

ಪೂರ್ಣಾಙ್ಕ fdt_stringlist_contains(ಧ್ರುವ ವರ್ಣ *strlist, ಪೂರ್ಣಾಙ್ಕ listlen, ಧ್ರುವ ವರ್ಣ *str)
{
	ಪೂರ್ಣಾಙ್ಕ len = strlen(str);
	ಧ್ರುವ ವರ್ಣ *p;

	ಯಾವತ್ (listlen >= len) {
		ಯದಿ (memcmp(str, strlist, len+1) == 0)
			ಪ್ರತ್ಯಾಗಮ 1;
		p = memchr(strlist, '\0', listlen);
		ಯದಿ (!p)
			ಪ್ರತ್ಯಾಗಮ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fdt_stringlist_ಎಣಿಕೆ(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, ಧ್ರುವ ವರ್ಣ *property)
{
	ಧ್ರುವ ವರ್ಣ *list, *end;
	ಪೂರ್ಣಾಙ್ಕ length, ಎಣಿಕೆ = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ಯದಿ (!list)
		ಪ್ರತ್ಯಾಗಮ length;

	end = list + length;

	ಯಾವತ್ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ಯದಿ (list + length > end)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADVALUE;

		list += length;
		ಎಣಿಕೆ++;
	}

	ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;
}

ಪೂರ್ಣಾಙ್ಕ fdt_stringlist_ಹುಡುಕು(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset, ಧ್ರುವ ವರ್ಣ *property,
			  ಧ್ರುವ ವರ್ಣ *string)
{
	ಪೂರ್ಣಾಙ್ಕ length, len, idx = 0;
	ಧ್ರುವ ವರ್ಣ *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ಯದಿ (!list)
		ಪ್ರತ್ಯಾಗಮ length;

	len = strlen(string) + 1;
	end = list + length;

	ಯಾವತ್ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ಯದಿ (list + length > end)
			ಪ್ರತ್ಯಾಗಮ -FDT_ERR_BADVALUE;

		ಯದಿ (length == len && memcmp(list, string, length) == 0)
			ಪ್ರತ್ಯಾಗಮ idx;

		list += length;
		idx++;
	}

	ಪ್ರತ್ಯಾಗಮ -FDT_ERR_NOTFOUND;
}

ಧ್ರುವ ವರ್ಣ *fdt_stringlist_get(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
			       ಧ್ರುವ ವರ್ಣ *property, ಪೂರ್ಣಾಙ್ಕ idx,
			       ಪೂರ್ಣಾಙ್ಕ *lenp)
{
	ಧ್ರುವ ವರ್ಣ *list, *end;
	ಪೂರ್ಣಾಙ್ಕ length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ಯದಿ (!list) {
		ಯದಿ (lenp)
			*lenp = length;

		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}

	end = list + length;

	ಯಾವತ್ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ಯದಿ (list + length > end) {
			ಯದಿ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}

		ಯದಿ (idx == 0) {
			ಯದಿ (lenp)
				*lenp = length - 1;

			ಪ್ರತ್ಯಾಗಮ list;
		}

		list += length;
		idx--;
	}

	ಯದಿ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ fdt_ಗಂಟು_check_compatible(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ nodeoffset,
			      ಧ್ರುವ ವರ್ಣ *compatible)
{
	ಧ್ರುವ ಶೂನ್ಯ *prop;
	ಪೂರ್ಣಾಙ್ಕ len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ಯದಿ (!prop)
		ಪ್ರತ್ಯಾಗಮ len;

	ಪ್ರತ್ಯಾಗಮ !fdt_stringlist_contains(prop, len, compatible);
}

ಪೂರ್ಣಾಙ್ಕ fdt_ಗಂಟು_offset_by_compatible(ಧ್ರುವ ಶೂನ್ಯ *fdt, ಪೂರ್ಣಾಙ್ಕ startoffset,
				  ಧ್ರುವ ವರ್ಣ *compatible)
{
	ಪೂರ್ಣಾಙ್ಕ offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	ಪ್ರತಿ (offset = fdt_next_ಗಂಟು(fdt, startoffset, ರಿಕ್ತ);
	     offset >= 0;
	     offset = fdt_next_ಗಂಟು(fdt, offset, ರಿಕ್ತ)) {
		err = fdt_ಗಂಟು_check_compatible(fdt, offset, compatible);
		ಯದಿ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ಪ್ರತ್ಯಾಗಮ err;
		ಅನ್ಯಥಾ ಯದಿ (err == 0)
			ಪ್ರತ್ಯಾಗಮ offset;
	}

	ಪ್ರತ್ಯಾಗಮ offset; /* error from fdt_next_node() */
}
