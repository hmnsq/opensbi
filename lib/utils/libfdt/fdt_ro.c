// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#অন্তর্ভাব "libfdt_env.h"

#অন্তর্ভাব <fdt.h>
#অন্তর্ভাব <libfdt.h>

#অন্তর্ভাব "libfdt_internal.h"

স্থির পূর্ণাঙ্ক fdt_nodename_eq_(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক offset,
			    ধ্রুব বর্ণ *s, পূর্ণাঙ্ক len)
{
	পূর্ণাঙ্ক olen;
	ধ্রুব বর্ণ *p = fdt_get_name(fdt, offset, &olen);

	যদি (!p || olen < len)
		/* short match */
		প্রত্যাগম 0;

	যদি (memcmp(p, s, len) != 0)
		প্রত্যাগম 0;

	যদি (p[len] == '\0')
		প্রত্যাগম 1;
	অন্যথা যদি (!memchr(s, '@', len) && (p[len] == '@'))
		প্রত্যাগম 1;
	অন্যথা
		প্রত্যাগম 0;
}

ধ্রুব বর্ণ *fdt_get_string(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক stroffset, পূর্ণাঙ্ক *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	পূর্ণাঙ্ক err;
	ধ্রুব বর্ণ *s, *n;

	যদি (can_assume(VALID_প্রবেশ)) {
		s = (ধ্রুব বর্ণ *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		যদি (lenp)
			*lenp = strlen(s);
		প্রত্যাগম s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	যদি (totalsize < 0)
		গচ্ছ fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	যদি (absoffset >= (unsigned)totalsize)
		গচ্ছ fail;
	len = totalsize - absoffset;

	যদি (fdt_magic(fdt) == FDT_MAGIC) {
		যদি (stroffset < 0)
			গচ্ছ fail;
		যদি (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			যদি ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				গচ্ছ fail;
			যদি ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} অন্যথা যদি (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned পূর্ণাঙ্ক sw_stroffset = -stroffset;

		যদি ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			গচ্ছ fail;
		যদি (sw_stroffset < len)
			len = sw_stroffset;
	} অন্যথা {
		err = -FDT_ERR_INTERNAL;
		গচ্ছ fail;
	}

	s = (ধ্রুব বর্ণ *)fdt + absoffset;
	n = memchr(s, '\0', len);
	যদি (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		গচ্ছ fail;
	}

	যদি (lenp)
		*lenp = n - s;
	প্রত্যাগম s;

fail:
	যদি (lenp)
		*lenp = err;
	প্রত্যাগম রিক্ত;
}

ধ্রুব বর্ণ *fdt_string(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক stroffset)
{
	প্রত্যাগম fdt_get_string(fdt, stroffset, রিক্ত);
}

স্থির পূর্ণাঙ্ক fdt_string_eq_(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক stroffset,
			  ধ্রুব বর্ণ *s, পূর্ণাঙ্ক len)
{
	পূর্ণাঙ্ক slen;
	ধ্রুব বর্ণ *p = fdt_get_string(fdt, stroffset, &slen);

	প্রত্যাগম p && (slen == len) && (memcmp(p, s, len) == 0);
}

পূর্ণাঙ্ক fdt_find_max_phandle(ধ্রুব শূন্য *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	পূর্ণাঙ্ক offset = -1;

	যাবত্ (true) {
		uint32_t মান;

		offset = fdt_next_গ্রন্থি(fdt, offset, রিক্ত);
		যদি (offset < 0) {
			যদি (offset == -FDT_ERR_NOTFOUND)
				ভঙ্গ;

			প্রত্যাগম offset;
		}

		মান = fdt_get_phandle(fdt, offset);

		যদি (মান > max)
			max = মান;
	}

	যদি (phandle)
		*phandle = max;

	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_generate_phandle(ধ্রুব শূন্য *fdt, uint32_t *phandle)
{
	uint32_t max;
	পূর্ণাঙ্ক err;

	err = fdt_find_max_phandle(fdt, &max);
	যদি (err < 0)
		প্রত্যাগম err;

	যদি (max == FDT_MAX_PHANDLE)
		প্রত্যাগম -FDT_ERR_NOPHANDLES;

	যদি (phandle)
		*phandle = max + 1;

	প্রত্যাগম 0;
}

স্থির ধ্রুব সংরচনা fdt_reserve_entry *fdt_mem_rsv(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক n)
{
	unsigned পূর্ণাঙ্ক offset = n * পরিমাণ(সংরচনা fdt_reserve_entry);
	unsigned পূর্ণাঙ্ক absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	যদি (!can_assume(VALID_প্রবেশ)) {
		যদি (absoffset < fdt_off_mem_rsvmap(fdt))
			প্রত্যাগম রিক্ত;
		যদি (absoffset > fdt_totalsize(fdt) -
		    পরিমাণ(সংরচনা fdt_reserve_entry))
			প্রত্যাগম রিক্ত;
	}
	প্রত্যাগম fdt_mem_rsv_(fdt, n);
}

পূর্ণাঙ্ক fdt_get_mem_rsv(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক n, uint64_t *address, uint64_t *size)
{
	ধ্রুব সংরচনা fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	যদি (!can_assume(VALID_প্রবেশ) && !re)
		প্রত্যাগম -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_অঙ্ক_mem_rsv(ধ্রুব শূন্য *fdt)
{
	পূর্ণাঙ্ক i;
	ধ্রুব সংরচনা fdt_reserve_entry *re;

	প্রতি (i = 0; (re = fdt_mem_rsv(fdt, i)) != রিক্ত; i++) {
		যদি (fdt64_ld_(&re->size) == 0)
			প্রত্যাগম i;
	}
	প্রত্যাগম -FDT_ERR_TRUNCATED;
}

স্থির পূর্ণাঙ্ক nextprop_(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক offset)
{
	uint32_t tag;
	পূর্ণাঙ্ক nextoffset;

	কুরু {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		বিকল্প (tag) {
		পক্ষ FDT_END:
			যদি (nextoffset >= 0)
				প্রত্যাগম -FDT_ERR_BADSTRUCTURE;
			অন্যথা
				প্রত্যাগম nextoffset;

		পক্ষ FDT_PROP:
			প্রত্যাগম offset;
		}
		offset = nextoffset;
	} যাবত্ (tag == FDT_NOP);

	প্রত্যাগম -FDT_ERR_NOTFOUND;
}

পূর্ণাঙ্ক fdt_subnode_offset_namelen(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক offset,
			       ধ্রুব বর্ণ *name, পূর্ণাঙ্ক namelen)
{
	পূর্ণাঙ্ক depth;

	FDT_RO_PROBE(fdt);

	প্রতি (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_গ্রন্থি(fdt, offset, &depth))
		যদি ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			প্রত্যাগম offset;

	যদি (depth < 0)
		প্রত্যাগম -FDT_ERR_NOTFOUND;
	প্রত্যাগম offset; /* error */
}

পূর্ণাঙ্ক fdt_subnode_offset(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক parentoffset,
		       ধ্রুব বর্ণ *name)
{
	প্রত্যাগম fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

পূর্ণাঙ্ক fdt_path_offset_namelen(ধ্রুব শূন্য *fdt, ধ্রুব বর্ণ *path, পূর্ণাঙ্ক namelen)
{
	ধ্রুব বর্ণ *end = path + namelen;
	ধ্রুব বর্ণ *p = path;
	পূর্ণাঙ্ক offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	যদি (*path != '/') {
		ধ্রুব বর্ণ *q = memchr(path, '/', end - p);

		যদি (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		যদি (!p)
			প্রত্যাগম -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	যাবত্ (p < end) {
		ধ্রুব বর্ণ *q;

		যাবত্ (*p == '/') {
			p++;
			যদি (p == end)
				প্রত্যাগম offset;
		}
		q = memchr(p, '/', end - p);
		যদি (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		যদি (offset < 0)
			প্রত্যাগম offset;

		p = q;
	}

	প্রত্যাগম offset;
}

পূর্ণাঙ্ক fdt_path_offset(ধ্রুব শূন্য *fdt, ধ্রুব বর্ণ *path)
{
	প্রত্যাগম fdt_path_offset_namelen(fdt, path, strlen(path));
}

ধ্রুব বর্ণ *fdt_get_name(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, পূর্ণাঙ্ক *len)
{
	ধ্রুব সংরচনা fdt_গ্রন্থি_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	ধ্রুব বর্ণ *nameptr;
	পূর্ণাঙ্ক err;

	যদি (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_গ্রন্থি_offset(fdt, nodeoffset)) < 0))
			গচ্ছ fail;

	nameptr = nh->name;

	যদি (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ধ্রুব বর্ণ *leaf;
		leaf = strrchr(nameptr, '/');
		যদি (leaf == রিক্ত) {
			err = -FDT_ERR_BADSTRUCTURE;
			গচ্ছ fail;
		}
		nameptr = leaf+1;
	}

	যদি (len)
		*len = strlen(nameptr);

	প্রত্যাগম nameptr;

 fail:
	যদি (len)
		*len = err;
	প্রত্যাগম রিক্ত;
}

পূর্ণাঙ্ক fdt_first_property_offset(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoffset)
{
	পূর্ণাঙ্ক offset;

	যদি ((offset = fdt_check_গ্রন্থি_offset(fdt, nodeoffset)) < 0)
		প্রত্যাগম offset;

	প্রত্যাগম nextprop_(fdt, offset);
}

পূর্ণাঙ্ক fdt_next_property_offset(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক offset)
{
	যদি ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		প্রত্যাগম offset;

	প্রত্যাগম nextprop_(fdt, offset);
}

স্থির ধ্রুব সংরচনা fdt_property *fdt_get_property_by_offset_(ধ্রুব শূন্য *fdt,
						              পূর্ণাঙ্ক offset,
						              পূর্ণাঙ্ক *lenp)
{
	পূর্ণাঙ্ক err;
	ধ্রুব সংরচনা fdt_property *prop;

	যদি (!can_assume(VALID_প্রবেশ) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		যদি (lenp)
			*lenp = err;
		প্রত্যাগম রিক্ত;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	যদি (lenp)
		*lenp = fdt32_ld_(&prop->len);

	প্রত্যাগম prop;
}

ধ্রুব সংরচনা fdt_property *fdt_get_property_by_offset(ধ্রুব শূন্য *fdt,
						      পূর্ণাঙ্ক offset,
						      পূর্ণাঙ্ক *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	যদি (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		যদি (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		প্রত্যাগম রিক্ত;
	}

	প্রত্যাগম fdt_get_property_by_offset_(fdt, offset, lenp);
}

স্থির ধ্রুব সংরচনা fdt_property *fdt_get_property_namelen_(ধ্রুব শূন্য *fdt,
						            পূর্ণাঙ্ক offset,
						            ধ্রুব বর্ণ *name,
						            পূর্ণাঙ্ক namelen,
							    পূর্ণাঙ্ক *lenp,
							    পূর্ণাঙ্ক *poffset)
{
	প্রতি (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		ধ্রুব সংরচনা fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		যদি (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			ভঙ্গ;
		}
		যদি (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			যদি (poffset)
				*poffset = offset;
			প্রত্যাগম prop;
		}
	}

	যদি (lenp)
		*lenp = offset;
	প্রত্যাগম রিক্ত;
}


ধ্রুব সংরচনা fdt_property *fdt_get_property_namelen(ধ্রুব শূন্য *fdt,
						    পূর্ণাঙ্ক offset,
						    ধ্রুব বর্ণ *name,
						    পূর্ণাঙ্ক namelen, পূর্ণাঙ্ক *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	যদি (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		যদি (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		প্রত্যাগম রিক্ত;
	}

	প্রত্যাগম fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 রিক্ত);
}


ধ্রুব সংরচনা fdt_property *fdt_get_property(ধ্রুব শূন্য *fdt,
					    পূর্ণাঙ্ক nodeoffset,
					    ধ্রুব বর্ণ *name, পূর্ণাঙ্ক *lenp)
{
	প্রত্যাগম fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

ধ্রুব শূন্য *fdt_getprop_namelen(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
				ধ্রুব বর্ণ *name, পূর্ণাঙ্ক namelen, পূর্ণাঙ্ক *lenp)
{
	পূর্ণাঙ্ক poffset;
	ধ্রুব সংরচনা fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	যদি (!prop)
		প্রত্যাগম রিক্ত;

	/* Handle realignment */
	যদি (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + পরিমাণ(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		প্রত্যাগম prop->data + 4;
	প্রত্যাগম prop->data;
}

ধ্রুব শূন্য *fdt_getprop_by_offset(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক offset,
				  ধ্রুব বর্ণ **namep, পূর্ণাঙ্ক *lenp)
{
	ধ্রুব সংরচনা fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	যদি (!prop)
		প্রত্যাগম রিক্ত;
	যদি (namep) {
		ধ্রুব বর্ণ *name;
		পূর্ণাঙ্ক namelen;

		যদি (!can_assume(VALID_প্রবেশ)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			যদি (!name) {
				যদি (lenp)
					*lenp = namelen;
				প্রত্যাগম রিক্ত;
			}
			*namep = name;
		} অন্যথা {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	যদি (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + পরিমাণ(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		প্রত্যাগম prop->data + 4;
	প্রত্যাগম prop->data;
}

ধ্রুব শূন্য *fdt_getprop(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
			ধ্রুব বর্ণ *name, পূর্ণাঙ্ক *lenp)
{
	প্রত্যাগম fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoffset)
{
	ধ্রুব fdt32_t *php;
	পূর্ণাঙ্ক len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	যদি (!php || (len != পরিমাণ(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		যদি (!php || (len != পরিমাণ(*php)))
			প্রত্যাগম 0;
	}

	প্রত্যাগম fdt32_ld_(php);
}

ধ্রুব বর্ণ *fdt_get_alias_namelen(ধ্রুব শূন্য *fdt,
				  ধ্রুব বর্ণ *name, পূর্ণাঙ্ক namelen)
{
	পূর্ণাঙ্ক aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	যদি (aliasoffset < 0)
		প্রত্যাগম রিক্ত;

	প্রত্যাগম fdt_getprop_namelen(fdt, aliasoffset, name, namelen, রিক্ত);
}

ধ্রুব বর্ণ *fdt_get_alias(ধ্রুব শূন্য *fdt, ধ্রুব বর্ণ *name)
{
	প্রত্যাগম fdt_get_alias_namelen(fdt, name, strlen(name));
}

পূর্ণাঙ্ক fdt_get_path(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, বর্ণ *buf, পূর্ণাঙ্ক buflen)
{
	পূর্ণাঙ্ক pdepth = 0, p = 0;
	পূর্ণাঙ্ক offset, depth, namelen;
	ধ্রুব বর্ণ *name;

	FDT_RO_PROBE(fdt);

	যদি (buflen < 2)
		প্রত্যাগম -FDT_ERR_NOSPACE;

	প্রতি (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_গ্রন্থি(fdt, offset, &depth)) {
		যাবত্ (pdepth > depth) {
			কুরু {
				p--;
			} যাবত্ (buf[p-1] != '/');
			pdepth--;
		}

		যদি (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			যদি (!name)
				প্রত্যাগম namelen;
			যদি ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		যদি (offset == nodeoffset) {
			যদি (pdepth < (depth + 1))
				প্রত্যাগম -FDT_ERR_NOSPACE;

			যদি (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			প্রত্যাগম 0;
		}
	}

	যদি ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		প্রত্যাগম -FDT_ERR_BADOFFSET;
	অন্যথা যদি (offset == -FDT_ERR_BADOFFSET)
		প্রত্যাগম -FDT_ERR_BADSTRUCTURE;

	প্রত্যাগম offset; /* error from fdt_next_node() */
}

পূর্ণাঙ্ক fdt_supernode_atdepth_offset(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
				 পূর্ণাঙ্ক supernodedepth, পূর্ণাঙ্ক *nodedepth)
{
	পূর্ণাঙ্ক offset, depth;
	পূর্ণাঙ্ক supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	যদি (supernodedepth < 0)
		প্রত্যাগম -FDT_ERR_NOTFOUND;

	প্রতি (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_গ্রন্থি(fdt, offset, &depth)) {
		যদি (depth == supernodedepth)
			supernodeoffset = offset;

		যদি (offset == nodeoffset) {
			যদি (nodedepth)
				*nodedepth = depth;

			যদি (supernodedepth > depth)
				প্রত্যাগম -FDT_ERR_NOTFOUND;
			অন্যথা
				প্রত্যাগম supernodeoffset;
		}
	}

	যদি (!can_assume(VALID_প্রবেশ)) {
		যদি ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			প্রত্যাগম -FDT_ERR_BADOFFSET;
		অন্যথা যদি (offset == -FDT_ERR_BADOFFSET)
			প্রত্যাগম -FDT_ERR_BADSTRUCTURE;
	}

	প্রত্যাগম offset; /* error from fdt_next_node() */
}

পূর্ণাঙ্ক fdt_গ্রন্থি_depth(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoffset)
{
	পূর্ণাঙ্ক nodedepth;
	পূর্ণাঙ্ক err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	যদি (err)
		প্রত্যাগম (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	প্রত্যাগম nodedepth;
}

পূর্ণাঙ্ক fdt_parent_offset(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoffset)
{
	পূর্ণাঙ্ক nodedepth = fdt_গ্রন্থি_depth(fdt, nodeoffset);

	যদি (nodedepth < 0)
		প্রত্যাগম nodedepth;
	প্রত্যাগম fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, রিক্ত);
}

পূর্ণাঙ্ক fdt_গ্রন্থি_offset_by_prop_মান(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক startoffset,
				  ধ্রুব বর্ণ *propname,
				  ধ্রুব শূন্য *propval, পূর্ণাঙ্ক proplen)
{
	পূর্ণাঙ্ক offset;
	ধ্রুব শূন্য *val;
	পূর্ণাঙ্ক len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	প্রতি (offset = fdt_next_গ্রন্থি(fdt, startoffset, রিক্ত);
	     offset >= 0;
	     offset = fdt_next_গ্রন্থি(fdt, offset, রিক্ত)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		যদি (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			প্রত্যাগম offset;
	}

	প্রত্যাগম offset; /* error from fdt_next_node() */
}

পূর্ণাঙ্ক fdt_গ্রন্থি_offset_by_phandle(ধ্রুব শূন্য *fdt, uint32_t phandle)
{
	পূর্ণাঙ্ক offset;

	যদি ((phandle == 0) || (phandle == ~0U))
		প্রত্যাগম -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	প্রতি (offset = fdt_next_গ্রন্থি(fdt, -1, রিক্ত);
	     offset >= 0;
	     offset = fdt_next_গ্রন্থি(fdt, offset, রিক্ত)) {
		যদি (fdt_get_phandle(fdt, offset) == phandle)
			প্রত্যাগম offset;
	}

	প্রত্যাগম offset; /* error from fdt_next_node() */
}

পূর্ণাঙ্ক fdt_stringlist_contains(ধ্রুব বর্ণ *strlist, পূর্ণাঙ্ক listlen, ধ্রুব বর্ণ *str)
{
	পূর্ণাঙ্ক len = strlen(str);
	ধ্রুব বর্ণ *p;

	যাবত্ (listlen >= len) {
		যদি (memcmp(str, strlist, len+1) == 0)
			প্রত্যাগম 1;
		p = memchr(strlist, '\0', listlen);
		যদি (!p)
			প্রত্যাগম 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	প্রত্যাগম 0;
}

পূর্ণাঙ্ক fdt_stringlist_গুনতি(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, ধ্রুব বর্ণ *property)
{
	ধ্রুব বর্ণ *list, *end;
	পূর্ণাঙ্ক length, গুনতি = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	যদি (!list)
		প্রত্যাগম length;

	end = list + length;

	যাবত্ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		যদি (list + length > end)
			প্রত্যাগম -FDT_ERR_BADVALUE;

		list += length;
		গুনতি++;
	}

	প্রত্যাগম গুনতি;
}

পূর্ণাঙ্ক fdt_stringlist_অনুসন্ধান(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoffset, ধ্রুব বর্ণ *property,
			  ধ্রুব বর্ণ *string)
{
	পূর্ণাঙ্ক length, len, idx = 0;
	ধ্রুব বর্ণ *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	যদি (!list)
		প্রত্যাগম length;

	len = strlen(string) + 1;
	end = list + length;

	যাবত্ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		যদি (list + length > end)
			প্রত্যাগম -FDT_ERR_BADVALUE;

		যদি (length == len && memcmp(list, string, length) == 0)
			প্রত্যাগম idx;

		list += length;
		idx++;
	}

	প্রত্যাগম -FDT_ERR_NOTFOUND;
}

ধ্রুব বর্ণ *fdt_stringlist_get(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
			       ধ্রুব বর্ণ *property, পূর্ণাঙ্ক idx,
			       পূর্ণাঙ্ক *lenp)
{
	ধ্রুব বর্ণ *list, *end;
	পূর্ণাঙ্ক length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	যদি (!list) {
		যদি (lenp)
			*lenp = length;

		প্রত্যাগম রিক্ত;
	}

	end = list + length;

	যাবত্ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		যদি (list + length > end) {
			যদি (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			প্রত্যাগম রিক্ত;
		}

		যদি (idx == 0) {
			যদি (lenp)
				*lenp = length - 1;

			প্রত্যাগম list;
		}

		list += length;
		idx--;
	}

	যদি (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	প্রত্যাগম রিক্ত;
}

পূর্ণাঙ্ক fdt_গ্রন্থি_check_compatible(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক nodeoffset,
			      ধ্রুব বর্ণ *compatible)
{
	ধ্রুব শূন্য *prop;
	পূর্ণাঙ্ক len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	যদি (!prop)
		প্রত্যাগম len;

	প্রত্যাগম !fdt_stringlist_contains(prop, len, compatible);
}

পূর্ণাঙ্ক fdt_গ্রন্থি_offset_by_compatible(ধ্রুব শূন্য *fdt, পূর্ণাঙ্ক startoffset,
				  ধ্রুব বর্ণ *compatible)
{
	পূর্ণাঙ্ক offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	প্রতি (offset = fdt_next_গ্রন্থি(fdt, startoffset, রিক্ত);
	     offset >= 0;
	     offset = fdt_next_গ্রন্থি(fdt, offset, রিক্ত)) {
		err = fdt_গ্রন্থি_check_compatible(fdt, offset, compatible);
		যদি ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			প্রত্যাগম err;
		অন্যথা যদি (err == 0)
			প্রত্যাগম offset;
	}

	প্রত্যাগম offset; /* error from fdt_next_node() */
}
