// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#অন্তর্ভাব "libfdt_env.h"

#অন্তর্ভাব <fdt.h>
#অন্তর্ভাব <libfdt.h>

#অন্তর্ভাব "libfdt_internal.h"

স্থির পূর্ণসংখ্যা fdt_nodename_eq_(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা offset,
			    ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা len)
{
	পূর্ণসংখ্যা olen;
	ধ্রুবক অক্ষর *p = fdt_get_name(fdt, offset, &olen);

	যদি (!p || olen < len)
		/* short match */
		ফেরত 0;

	যদি (memcmp(p, s, len) != 0)
		ফেরত 0;

	যদি (p[len] == '\0')
		ফেরত 1;
	অন্যথা যদি (!memchr(s, '@', len) && (p[len] == '@'))
		ফেরত 1;
	অন্যথা
		ফেরত 0;
}

ধ্রুবক অক্ষর *fdt_get_string(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা stroffset, পূর্ণসংখ্যা *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	পূর্ণসংখ্যা err;
	ধ্রুবক অক্ষর *s, *n;

	যদি (can_assume(VALID_প্রবেশ)) {
		s = (ধ্রুবক অক্ষর *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		যদি (lenp)
			*lenp = strlen(s);
		ফেরত s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	যদি (totalsize < 0)
		যাও fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	যদি (absoffset >= (unsigned)totalsize)
		যাও fail;
	len = totalsize - absoffset;

	যদি (fdt_magic(fdt) == FDT_MAGIC) {
		যদি (stroffset < 0)
			যাও fail;
		যদি (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			যদি ((unsigned)stroffset >= fdt_আকার_dt_strings(fdt))
				যাও fail;
			যদি ((fdt_আকার_dt_strings(fdt) - stroffset) < len)
				len = fdt_আকার_dt_strings(fdt) - stroffset;
		}
	} অন্যথা যদি (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned পূর্ণসংখ্যা sw_stroffset = -stroffset;

		যদি ((stroffset >= 0) ||
		    (sw_stroffset > fdt_আকার_dt_strings(fdt)))
			যাও fail;
		যদি (sw_stroffset < len)
			len = sw_stroffset;
	} অন্যথা {
		err = -FDT_ERR_INTERNAL;
		যাও fail;
	}

	s = (ধ্রুবক অক্ষর *)fdt + absoffset;
	n = memchr(s, '\0', len);
	যদি (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		যাও fail;
	}

	যদি (lenp)
		*lenp = n - s;
	ফেরত s;

fail:
	যদি (lenp)
		*lenp = err;
	ফেরত ফাঁকা;
}

ধ্রুবক অক্ষর *fdt_string(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা stroffset)
{
	ফেরত fdt_get_string(fdt, stroffset, ফাঁকা);
}

স্থির পূর্ণসংখ্যা fdt_string_eq_(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা stroffset,
			  ধ্রুবক অক্ষর *s, পূর্ণসংখ্যা len)
{
	পূর্ণসংখ্যা slen;
	ধ্রুবক অক্ষর *p = fdt_get_string(fdt, stroffset, &slen);

	ফেরত p && (slen == len) && (memcmp(p, s, len) == 0);
}

পূর্ণসংখ্যা fdt_find_max_phandle(ধ্রুবক শূন্য *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	পূর্ণসংখ্যা offset = -1;

	যখন (true) {
		uint32_t মান;

		offset = fdt_পরবর্তী_গ্রন্থি(fdt, offset, ফাঁকা);
		যদি (offset < 0) {
			যদি (offset == -FDT_ERR_NOTFOUND)
				বিরতি;

			ফেরত offset;
		}

		মান = fdt_get_phandle(fdt, offset);

		যদি (মান > max)
			max = মান;
	}

	যদি (phandle)
		*phandle = max;

	ফেরত 0;
}

পূর্ণসংখ্যা fdt_generate_phandle(ধ্রুবক শূন্য *fdt, uint32_t *phandle)
{
	uint32_t max;
	পূর্ণসংখ্যা err;

	err = fdt_find_max_phandle(fdt, &max);
	যদি (err < 0)
		ফেরত err;

	যদি (max == FDT_MAX_PHANDLE)
		ফেরত -FDT_ERR_NOPHANDLES;

	যদি (phandle)
		*phandle = max + 1;

	ফেরত 0;
}

স্থির ধ্রুবক কাঠামো fdt_reserve_entry *fdt_mem_rsv(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা n)
{
	unsigned পূর্ণসংখ্যা offset = n * আকার(কাঠামো fdt_reserve_entry);
	unsigned পূর্ণসংখ্যা absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	যদি (!can_assume(VALID_প্রবেশ)) {
		যদি (absoffset < fdt_off_mem_rsvmap(fdt))
			ফেরত ফাঁকা;
		যদি (absoffset > fdt_totalsize(fdt) -
		    আকার(কাঠামো fdt_reserve_entry))
			ফেরত ফাঁকা;
	}
	ফেরত fdt_mem_rsv_(fdt, n);
}

পূর্ণসংখ্যা fdt_get_mem_rsv(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা n, uint64_t *address, uint64_t *আকার_2)
{
	ধ্রুবক কাঠামো fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	যদি (!can_assume(VALID_প্রবেশ) && !re)
		ফেরত -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*আকার_2 = fdt64_ld_(&re->আকার_2);
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_অঙ্ক_mem_rsv(ধ্রুবক শূন্য *fdt)
{
	পূর্ণসংখ্যা i;
	ধ্রুবক কাঠামো fdt_reserve_entry *re;

	জন্য (i = 0; (re = fdt_mem_rsv(fdt, i)) != ফাঁকা; i++) {
		যদি (fdt64_ld_(&re->আকার_2) == 0)
			ফেরত i;
	}
	ফেরত -FDT_ERR_TRUNCATED;
}

স্থির পূর্ণসংখ্যা nextprop_(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা offset)
{
	uint32_t tag;
	পূর্ণসংখ্যা nextoffset;

	করো {
		tag = fdt_পরবর্তী_tag(fdt, offset, &nextoffset);

		নির্বাচন (tag) {
		ক্ষেত্র FDT_END:
			যদি (nextoffset >= 0)
				ফেরত -FDT_ERR_BADSTRUCTURE;
			অন্যথা
				ফেরত nextoffset;

		ক্ষেত্র FDT_PROP:
			ফেরত offset;
		}
		offset = nextoffset;
	} যখন (tag == FDT_NOP);

	ফেরত -FDT_ERR_NOTFOUND;
}

পূর্ণসংখ্যা fdt_subnode_offset_namelen(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা offset,
			       ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা namelen)
{
	পূর্ণসংখ্যা depth;

	FDT_RO_PROBE(fdt);

	জন্য (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_পরবর্তী_গ্রন্থি(fdt, offset, &depth))
		যদি ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ফেরত offset;

	যদি (depth < 0)
		ফেরত -FDT_ERR_NOTFOUND;
	ফেরত offset; /* error */
}

পূর্ণসংখ্যা fdt_subnode_offset(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা parentoffset,
		       ধ্রুবক অক্ষর *name)
{
	ফেরত fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

পূর্ণসংখ্যা fdt_path_offset_namelen(ধ্রুবক শূন্য *fdt, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা namelen)
{
	ধ্রুবক অক্ষর *end = path + namelen;
	ধ্রুবক অক্ষর *p = path;
	পূর্ণসংখ্যা offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	যদি (*path != '/') {
		ধ্রুবক অক্ষর *q = memchr(path, '/', end - p);

		যদি (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		যদি (!p)
			ফেরত -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	যখন (p < end) {
		ধ্রুবক অক্ষর *q;

		যখন (*p == '/') {
			p++;
			যদি (p == end)
				ফেরত offset;
		}
		q = memchr(p, '/', end - p);
		যদি (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		যদি (offset < 0)
			ফেরত offset;

		p = q;
	}

	ফেরত offset;
}

পূর্ণসংখ্যা fdt_path_offset(ধ্রুবক শূন্য *fdt, ধ্রুবক অক্ষর *path)
{
	ফেরত fdt_path_offset_namelen(fdt, path, strlen(path));
}

ধ্রুবক অক্ষর *fdt_get_name(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, পূর্ণসংখ্যা *len)
{
	ধ্রুবক কাঠামো fdt_গ্রন্থি_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	ধ্রুবক অক্ষর *nameptr;
	পূর্ণসংখ্যা err;

	যদি (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_গ্রন্থি_offset(fdt, nodeoffset)) < 0))
			যাও fail;

	nameptr = nh->name;

	যদি (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		ধ্রুবক অক্ষর *leaf;
		leaf = strrchr(nameptr, '/');
		যদি (leaf == ফাঁকা) {
			err = -FDT_ERR_BADSTRUCTURE;
			যাও fail;
		}
		nameptr = leaf+1;
	}

	যদি (len)
		*len = strlen(nameptr);

	ফেরত nameptr;

 fail:
	যদি (len)
		*len = err;
	ফেরত ফাঁকা;
}

পূর্ণসংখ্যা fdt_first_property_offset(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset)
{
	পূর্ণসংখ্যা offset;

	যদি ((offset = fdt_check_গ্রন্থি_offset(fdt, nodeoffset)) < 0)
		ফেরত offset;

	ফেরত nextprop_(fdt, offset);
}

পূর্ণসংখ্যা fdt_পরবর্তী_property_offset(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা offset)
{
	যদি ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ফেরত offset;

	ফেরত nextprop_(fdt, offset);
}

স্থির ধ্রুবক কাঠামো fdt_property *fdt_get_property_by_offset_(ধ্রুবক শূন্য *fdt,
						              পূর্ণসংখ্যা offset,
						              পূর্ণসংখ্যা *lenp)
{
	পূর্ণসংখ্যা err;
	ধ্রুবক কাঠামো fdt_property *prop;

	যদি (!can_assume(VALID_প্রবেশ) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		যদি (lenp)
			*lenp = err;
		ফেরত ফাঁকা;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	যদি (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ফেরত prop;
}

ধ্রুবক কাঠামো fdt_property *fdt_get_property_by_offset(ধ্রুবক শূন্য *fdt,
						      পূর্ণসংখ্যা offset,
						      পূর্ণসংখ্যা *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	যদি (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		যদি (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ফেরত ফাঁকা;
	}

	ফেরত fdt_get_property_by_offset_(fdt, offset, lenp);
}

স্থির ধ্রুবক কাঠামো fdt_property *fdt_get_property_namelen_(ধ্রুবক শূন্য *fdt,
						            পূর্ণসংখ্যা offset,
						            ধ্রুবক অক্ষর *name,
						            পূর্ণসংখ্যা namelen,
							    পূর্ণসংখ্যা *lenp,
							    পূর্ণসংখ্যা *poffset)
{
	জন্য (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_পরবর্তী_property_offset(fdt, offset))) {
		ধ্রুবক কাঠামো fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		যদি (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			বিরতি;
		}
		যদি (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			যদি (poffset)
				*poffset = offset;
			ফেরত prop;
		}
	}

	যদি (lenp)
		*lenp = offset;
	ফেরত ফাঁকা;
}


ধ্রুবক কাঠামো fdt_property *fdt_get_property_namelen(ধ্রুবক শূন্য *fdt,
						    পূর্ণসংখ্যা offset,
						    ধ্রুবক অক্ষর *name,
						    পূর্ণসংখ্যা namelen, পূর্ণসংখ্যা *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	যদি (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		যদি (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ফেরত ফাঁকা;
	}

	ফেরত fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 ফাঁকা);
}


ধ্রুবক কাঠামো fdt_property *fdt_get_property(ধ্রুবক শূন্য *fdt,
					    পূর্ণসংখ্যা nodeoffset,
					    ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা *lenp)
{
	ফেরত fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

ধ্রুবক শূন্য *fdt_getprop_namelen(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
				ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা namelen, পূর্ণসংখ্যা *lenp)
{
	পূর্ণসংখ্যা poffset;
	ধ্রুবক কাঠামো fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	যদি (!prop)
		ফেরত ফাঁকা;

	/* Handle realignment */
	যদি (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + আকার(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ফেরত prop->তথ্য + 4;
	ফেরত prop->তথ্য;
}

ধ্রুবক শূন্য *fdt_getprop_by_offset(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা offset,
				  ধ্রুবক অক্ষর **namep, পূর্ণসংখ্যা *lenp)
{
	ধ্রুবক কাঠামো fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	যদি (!prop)
		ফেরত ফাঁকা;
	যদি (namep) {
		ধ্রুবক অক্ষর *name;
		পূর্ণসংখ্যা namelen;

		যদি (!can_assume(VALID_প্রবেশ)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			যদি (!name) {
				যদি (lenp)
					*lenp = namelen;
				ফেরত ফাঁকা;
			}
			*namep = name;
		} অন্যথা {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	যদি (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + আকার(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ফেরত prop->তথ্য + 4;
	ফেরত prop->তথ্য;
}

ধ্রুবক শূন্য *fdt_getprop(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
			ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা *lenp)
{
	ফেরত fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset)
{
	ধ্রুবক fdt32_t *php;
	পূর্ণসংখ্যা len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	যদি (!php || (len != আকার(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		যদি (!php || (len != আকার(*php)))
			ফেরত 0;
	}

	ফেরত fdt32_ld_(php);
}

ধ্রুবক অক্ষর *fdt_get_alias_namelen(ধ্রুবক শূন্য *fdt,
				  ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা namelen)
{
	পূর্ণসংখ্যা aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	যদি (aliasoffset < 0)
		ফেরত ফাঁকা;

	ফেরত fdt_getprop_namelen(fdt, aliasoffset, name, namelen, ফাঁকা);
}

ধ্রুবক অক্ষর *fdt_get_alias(ধ্রুবক শূন্য *fdt, ধ্রুবক অক্ষর *name)
{
	ফেরত fdt_get_alias_namelen(fdt, name, strlen(name));
}

পূর্ণসংখ্যা fdt_get_path(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, অক্ষর *buf, পূর্ণসংখ্যা buflen)
{
	পূর্ণসংখ্যা pdepth = 0, p = 0;
	পূর্ণসংখ্যা offset, depth, namelen;
	ধ্রুবক অক্ষর *name;

	FDT_RO_PROBE(fdt);

	যদি (buflen < 2)
		ফেরত -FDT_ERR_NOSPACE;

	জন্য (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_পরবর্তী_গ্রন্থি(fdt, offset, &depth)) {
		যখন (pdepth > depth) {
			করো {
				p--;
			} যখন (buf[p-1] != '/');
			pdepth--;
		}

		যদি (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			যদি (!name)
				ফেরত namelen;
			যদি ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		যদি (offset == nodeoffset) {
			যদি (pdepth < (depth + 1))
				ফেরত -FDT_ERR_NOSPACE;

			যদি (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ফেরত 0;
		}
	}

	যদি ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ফেরত -FDT_ERR_BADOFFSET;
	অন্যথা যদি (offset == -FDT_ERR_BADOFFSET)
		ফেরত -FDT_ERR_BADSTRUCTURE;

	ফেরত offset; /* error from fdt_next_node() */
}

পূর্ণসংখ্যা fdt_supernode_atdepth_offset(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
				 পূর্ণসংখ্যা supernodedepth, পূর্ণসংখ্যা *nodedepth)
{
	পূর্ণসংখ্যা offset, depth;
	পূর্ণসংখ্যা supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	যদি (supernodedepth < 0)
		ফেরত -FDT_ERR_NOTFOUND;

	জন্য (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_পরবর্তী_গ্রন্থি(fdt, offset, &depth)) {
		যদি (depth == supernodedepth)
			supernodeoffset = offset;

		যদি (offset == nodeoffset) {
			যদি (nodedepth)
				*nodedepth = depth;

			যদি (supernodedepth > depth)
				ফেরত -FDT_ERR_NOTFOUND;
			অন্যথা
				ফেরত supernodeoffset;
		}
	}

	যদি (!can_assume(VALID_প্রবেশ)) {
		যদি ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ফেরত -FDT_ERR_BADOFFSET;
		অন্যথা যদি (offset == -FDT_ERR_BADOFFSET)
			ফেরত -FDT_ERR_BADSTRUCTURE;
	}

	ফেরত offset; /* error from fdt_next_node() */
}

পূর্ণসংখ্যা fdt_গ্রন্থি_depth(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset)
{
	পূর্ণসংখ্যা nodedepth;
	পূর্ণসংখ্যা err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	যদি (err)
		ফেরত (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ফেরত nodedepth;
}

পূর্ণসংখ্যা fdt_parent_offset(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset)
{
	পূর্ণসংখ্যা nodedepth = fdt_গ্রন্থি_depth(fdt, nodeoffset);

	যদি (nodedepth < 0)
		ফেরত nodedepth;
	ফেরত fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, ফাঁকা);
}

পূর্ণসংখ্যা fdt_গ্রন্থি_offset_by_prop_মান(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা startoffset,
				  ধ্রুবক অক্ষর *propname,
				  ধ্রুবক শূন্য *propval, পূর্ণসংখ্যা proplen)
{
	পূর্ণসংখ্যা offset;
	ধ্রুবক শূন্য *val;
	পূর্ণসংখ্যা len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	জন্য (offset = fdt_পরবর্তী_গ্রন্থি(fdt, startoffset, ফাঁকা);
	     offset >= 0;
	     offset = fdt_পরবর্তী_গ্রন্থি(fdt, offset, ফাঁকা)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		যদি (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ফেরত offset;
	}

	ফেরত offset; /* error from fdt_next_node() */
}

পূর্ণসংখ্যা fdt_গ্রন্থি_offset_by_phandle(ধ্রুবক শূন্য *fdt, uint32_t phandle)
{
	পূর্ণসংখ্যা offset;

	যদি ((phandle == 0) || (phandle == ~0U))
		ফেরত -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	জন্য (offset = fdt_পরবর্তী_গ্রন্থি(fdt, -1, ফাঁকা);
	     offset >= 0;
	     offset = fdt_পরবর্তী_গ্রন্থি(fdt, offset, ফাঁকা)) {
		যদি (fdt_get_phandle(fdt, offset) == phandle)
			ফেরত offset;
	}

	ফেরত offset; /* error from fdt_next_node() */
}

পূর্ণসংখ্যা fdt_stringlist_contains(ধ্রুবক অক্ষর *strlist, পূর্ণসংখ্যা listlen, ধ্রুবক অক্ষর *str)
{
	পূর্ণসংখ্যা len = strlen(str);
	ধ্রুবক অক্ষর *p;

	যখন (listlen >= len) {
		যদি (memcmp(str, strlist, len+1) == 0)
			ফেরত 1;
		p = memchr(strlist, '\0', listlen);
		যদি (!p)
			ফেরত 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ফেরত 0;
}

পূর্ণসংখ্যা fdt_stringlist_গুনতি(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, ধ্রুবক অক্ষর *property)
{
	ধ্রুবক অক্ষর *list, *end;
	পূর্ণসংখ্যা দৈর্ঘ্য, গুনতি = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &দৈর্ঘ্য);
	যদি (!list)
		ফেরত দৈর্ঘ্য;

	end = list + দৈর্ঘ্য;

	যখন (list < end) {
		দৈর্ঘ্য = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		যদি (list + দৈর্ঘ্য > end)
			ফেরত -FDT_ERR_BADVALUE;

		list += দৈর্ঘ্য;
		গুনতি++;
	}

	ফেরত গুনতি;
}

পূর্ণসংখ্যা fdt_stringlist_অনুসন্ধান(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset, ধ্রুবক অক্ষর *property,
			  ধ্রুবক অক্ষর *string)
{
	পূর্ণসংখ্যা দৈর্ঘ্য, len, idx = 0;
	ধ্রুবক অক্ষর *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &দৈর্ঘ্য);
	যদি (!list)
		ফেরত দৈর্ঘ্য;

	len = strlen(string) + 1;
	end = list + দৈর্ঘ্য;

	যখন (list < end) {
		দৈর্ঘ্য = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		যদি (list + দৈর্ঘ্য > end)
			ফেরত -FDT_ERR_BADVALUE;

		যদি (দৈর্ঘ্য == len && memcmp(list, string, দৈর্ঘ্য) == 0)
			ফেরত idx;

		list += দৈর্ঘ্য;
		idx++;
	}

	ফেরত -FDT_ERR_NOTFOUND;
}

ধ্রুবক অক্ষর *fdt_stringlist_get(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
			       ধ্রুবক অক্ষর *property, পূর্ণসংখ্যা idx,
			       পূর্ণসংখ্যা *lenp)
{
	ধ্রুবক অক্ষর *list, *end;
	পূর্ণসংখ্যা দৈর্ঘ্য;

	list = fdt_getprop(fdt, nodeoffset, property, &দৈর্ঘ্য);
	যদি (!list) {
		যদি (lenp)
			*lenp = দৈর্ঘ্য;

		ফেরত ফাঁকা;
	}

	end = list + দৈর্ঘ্য;

	যখন (list < end) {
		দৈর্ঘ্য = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		যদি (list + দৈর্ঘ্য > end) {
			যদি (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ফেরত ফাঁকা;
		}

		যদি (idx == 0) {
			যদি (lenp)
				*lenp = দৈর্ঘ্য - 1;

			ফেরত list;
		}

		list += দৈর্ঘ্য;
		idx--;
	}

	যদি (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ফেরত ফাঁকা;
}

পূর্ণসংখ্যা fdt_গ্রন্থি_check_compatible(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা nodeoffset,
			      ধ্রুবক অক্ষর *compatible)
{
	ধ্রুবক শূন্য *prop;
	পূর্ণসংখ্যা len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	যদি (!prop)
		ফেরত len;

	ফেরত !fdt_stringlist_contains(prop, len, compatible);
}

পূর্ণসংখ্যা fdt_গ্রন্থি_offset_by_compatible(ধ্রুবক শূন্য *fdt, পূর্ণসংখ্যা startoffset,
				  ধ্রুবক অক্ষর *compatible)
{
	পূর্ণসংখ্যা offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	জন্য (offset = fdt_পরবর্তী_গ্রন্থি(fdt, startoffset, ফাঁকা);
	     offset >= 0;
	     offset = fdt_পরবর্তী_গ্রন্থি(fdt, offset, ফাঁকা)) {
		err = fdt_গ্রন্থি_check_compatible(fdt, offset, compatible);
		যদি ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ফেরত err;
		অন্যথা যদি (err == 0)
			ফেরত offset;
	}

	ফেরত offset; /* error from fdt_next_node() */
}
