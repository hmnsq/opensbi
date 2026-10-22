// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#അന്തര്ഭാവ "libfdt_env.h"

#അന്തര്ഭാവ <fdt.h>
#അന്തര്ഭാവ <libfdt.h>

#അന്തര്ഭാവ "libfdt_internal.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_nodename_eq_(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ offset,
			    സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ len)
{
	പൂർണ്ണസംഖ്യ olen;
	സ്ഥിരം അക്ഷരം *p = fdt_get_name(fdt, offset, &olen);

	എങ്കിൽ (!p || olen < len)
		/* short match */
		തിരികെ 0;

	എങ്കിൽ (memcmp(p, s, len) != 0)
		തിരികെ 0;

	എങ്കിൽ (p[len] == '\0')
		തിരികെ 1;
	അല്ലെങ്കിൽ എങ്കിൽ (!memchr(s, '@', len) && (p[len] == '@'))
		തിരികെ 1;
	അല്ലെങ്കിൽ
		തിരികെ 0;
}

സ്ഥിരം അക്ഷരം *fdt_get_string(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ stroffset, പൂർണ്ണസംഖ്യ *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	പൂർണ്ണസംഖ്യ err;
	സ്ഥിരം അക്ഷരം *s, *n;

	എങ്കിൽ (can_assume(VALID_നിവേശം)) {
		s = (സ്ഥിരം അക്ഷരം *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		എങ്കിൽ (lenp)
			*lenp = strlen(s);
		തിരികെ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	എങ്കിൽ (totalsize < 0)
		പോകുക fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	എങ്കിൽ (absoffset >= (unsigned)totalsize)
		പോകുക fail;
	len = totalsize - absoffset;

	എങ്കിൽ (fdt_magic(fdt) == FDT_MAGIC) {
		എങ്കിൽ (stroffset < 0)
			പോകുക fail;
		എങ്കിൽ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			എങ്കിൽ ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				പോകുക fail;
			എങ്കിൽ ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned പൂർണ്ണസംഖ്യ sw_stroffset = -stroffset;

		എങ്കിൽ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			പോകുക fail;
		എങ്കിൽ (sw_stroffset < len)
			len = sw_stroffset;
	} അല്ലെങ്കിൽ {
		err = -FDT_ERR_INTERNAL;
		പോകുക fail;
	}

	s = (സ്ഥിരം അക്ഷരം *)fdt + absoffset;
	n = memchr(s, '\0', len);
	എങ്കിൽ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		പോകുക fail;
	}

	എങ്കിൽ (lenp)
		*lenp = n - s;
	തിരികെ s;

fail:
	എങ്കിൽ (lenp)
		*lenp = err;
	തിരികെ ശൂന്യ;
}

സ്ഥിരം അക്ഷരം *fdt_string(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ stroffset)
{
	തിരികെ fdt_get_string(fdt, stroffset, ശൂന്യ);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fdt_string_eq_(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ stroffset,
			  സ്ഥിരം അക്ഷരം *s, പൂർണ്ണസംഖ്യ len)
{
	പൂർണ്ണസംഖ്യ slen;
	സ്ഥിരം അക്ഷരം *p = fdt_get_string(fdt, stroffset, &slen);

	തിരികെ p && (slen == len) && (memcmp(p, s, len) == 0);
}

പൂർണ്ണസംഖ്യ fdt_find_max_phandle(സ്ഥിരം ശൂന്യം *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	പൂർണ്ണസംഖ്യ offset = -1;

	ആകുമ്പോൾ (true) {
		uint32_t മൂല്യം;

		offset = fdt_next_കണ്ണി(fdt, offset, ശൂന്യ);
		എങ്കിൽ (offset < 0) {
			എങ്കിൽ (offset == -FDT_ERR_NOTFOUND)
				നിർത്തുക;

			തിരികെ offset;
		}

		മൂല്യം = fdt_get_phandle(fdt, offset);

		എങ്കിൽ (മൂല്യം > max)
			max = മൂല്യം;
	}

	എങ്കിൽ (phandle)
		*phandle = max;

	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_generate_phandle(സ്ഥിരം ശൂന്യം *fdt, uint32_t *phandle)
{
	uint32_t max;
	പൂർണ്ണസംഖ്യ err;

	err = fdt_find_max_phandle(fdt, &max);
	എങ്കിൽ (err < 0)
		തിരികെ err;

	എങ്കിൽ (max == FDT_MAX_PHANDLE)
		തിരികെ -FDT_ERR_NOPHANDLES;

	എങ്കിൽ (phandle)
		*phandle = max + 1;

	തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_reserve_entry *fdt_mem_rsv(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ n)
{
	unsigned പൂർണ്ണസംഖ്യ offset = n * വലുപ്പം(ഘടന fdt_reserve_entry);
	unsigned പൂർണ്ണസംഖ്യ absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	എങ്കിൽ (!can_assume(VALID_നിവേശം)) {
		എങ്കിൽ (absoffset < fdt_off_mem_rsvmap(fdt))
			തിരികെ ശൂന്യ;
		എങ്കിൽ (absoffset > fdt_totalsize(fdt) -
		    വലുപ്പം(ഘടന fdt_reserve_entry))
			തിരികെ ശൂന്യ;
	}
	തിരികെ fdt_mem_rsv_(fdt, n);
}

പൂർണ്ണസംഖ്യ fdt_get_mem_rsv(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ n, uint64_t *address, uint64_t *size)
{
	സ്ഥിരം ഘടന fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	എങ്കിൽ (!can_assume(VALID_നിവേശം) && !re)
		തിരികെ -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_അക്കം_mem_rsv(സ്ഥിരം ശൂന്യം *fdt)
{
	പൂർണ്ണസംഖ്യ i;
	സ്ഥിരം ഘടന fdt_reserve_entry *re;

	വേണ്ടി (i = 0; (re = fdt_mem_rsv(fdt, i)) != ശൂന്യ; i++) {
		എങ്കിൽ (fdt64_ld_(&re->size) == 0)
			തിരികെ i;
	}
	തിരികെ -FDT_ERR_TRUNCATED;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ nextprop_(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ offset)
{
	uint32_t tag;
	പൂർണ്ണസംഖ്യ nextoffset;

	ചെയ്യുക {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		തിരഞ്ഞെടുപ്പ് (tag) {
		അവസ്ഥ FDT_END:
			എങ്കിൽ (nextoffset >= 0)
				തിരികെ -FDT_ERR_BADSTRUCTURE;
			അല്ലെങ്കിൽ
				തിരികെ nextoffset;

		അവസ്ഥ FDT_PROP:
			തിരികെ offset;
		}
		offset = nextoffset;
	} ആകുമ്പോൾ (tag == FDT_NOP);

	തിരികെ -FDT_ERR_NOTFOUND;
}

പൂർണ്ണസംഖ്യ fdt_subnode_offset_namelen(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ offset,
			       സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ namelen)
{
	പൂർണ്ണസംഖ്യ depth;

	FDT_RO_PROBE(fdt);

	വേണ്ടി (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_കണ്ണി(fdt, offset, &depth))
		എങ്കിൽ ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			തിരികെ offset;

	എങ്കിൽ (depth < 0)
		തിരികെ -FDT_ERR_NOTFOUND;
	തിരികെ offset; /* error */
}

പൂർണ്ണസംഖ്യ fdt_subnode_offset(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ parentoffset,
		       സ്ഥിരം അക്ഷരം *name)
{
	തിരികെ fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

പൂർണ്ണസംഖ്യ fdt_path_offset_namelen(സ്ഥിരം ശൂന്യം *fdt, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ namelen)
{
	സ്ഥിരം അക്ഷരം *end = path + namelen;
	സ്ഥിരം അക്ഷരം *p = path;
	പൂർണ്ണസംഖ്യ offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	എങ്കിൽ (*path != '/') {
		സ്ഥിരം അക്ഷരം *q = memchr(path, '/', end - p);

		എങ്കിൽ (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		എങ്കിൽ (!p)
			തിരികെ -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	ആകുമ്പോൾ (p < end) {
		സ്ഥിരം അക്ഷരം *q;

		ആകുമ്പോൾ (*p == '/') {
			p++;
			എങ്കിൽ (p == end)
				തിരികെ offset;
		}
		q = memchr(p, '/', end - p);
		എങ്കിൽ (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		എങ്കിൽ (offset < 0)
			തിരികെ offset;

		p = q;
	}

	തിരികെ offset;
}

പൂർണ്ണസംഖ്യ fdt_path_offset(സ്ഥിരം ശൂന്യം *fdt, സ്ഥിരം അക്ഷരം *path)
{
	തിരികെ fdt_path_offset_namelen(fdt, path, strlen(path));
}

സ്ഥിരം അക്ഷരം *fdt_get_name(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, പൂർണ്ണസംഖ്യ *len)
{
	സ്ഥിരം ഘടന fdt_കണ്ണി_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	സ്ഥിരം അക്ഷരം *nameptr;
	പൂർണ്ണസംഖ്യ err;

	എങ്കിൽ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_കണ്ണി_offset(fdt, nodeoffset)) < 0))
			പോകുക fail;

	nameptr = nh->name;

	എങ്കിൽ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		സ്ഥിരം അക്ഷരം *leaf;
		leaf = strrchr(nameptr, '/');
		എങ്കിൽ (leaf == ശൂന്യ) {
			err = -FDT_ERR_BADSTRUCTURE;
			പോകുക fail;
		}
		nameptr = leaf+1;
	}

	എങ്കിൽ (len)
		*len = strlen(nameptr);

	തിരികെ nameptr;

 fail:
	എങ്കിൽ (len)
		*len = err;
	തിരികെ ശൂന്യ;
}

പൂർണ്ണസംഖ്യ fdt_first_property_offset(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset)
{
	പൂർണ്ണസംഖ്യ offset;

	എങ്കിൽ ((offset = fdt_check_കണ്ണി_offset(fdt, nodeoffset)) < 0)
		തിരികെ offset;

	തിരികെ nextprop_(fdt, offset);
}

പൂർണ്ണസംഖ്യ fdt_next_property_offset(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ offset)
{
	എങ്കിൽ ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		തിരികെ offset;

	തിരികെ nextprop_(fdt, offset);
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_property *fdt_get_property_by_offset_(സ്ഥിരം ശൂന്യം *fdt,
						              പൂർണ്ണസംഖ്യ offset,
						              പൂർണ്ണസംഖ്യ *lenp)
{
	പൂർണ്ണസംഖ്യ err;
	സ്ഥിരം ഘടന fdt_property *prop;

	എങ്കിൽ (!can_assume(VALID_നിവേശം) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		എങ്കിൽ (lenp)
			*lenp = err;
		തിരികെ ശൂന്യ;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	എങ്കിൽ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	തിരികെ prop;
}

സ്ഥിരം ഘടന fdt_property *fdt_get_property_by_offset(സ്ഥിരം ശൂന്യം *fdt,
						      പൂർണ്ണസംഖ്യ offset,
						      പൂർണ്ണസംഖ്യ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	എങ്കിൽ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		എങ്കിൽ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		തിരികെ ശൂന്യ;
	}

	തിരികെ fdt_get_property_by_offset_(fdt, offset, lenp);
}

സ്ഥിതം സ്ഥിരം ഘടന fdt_property *fdt_get_property_namelen_(സ്ഥിരം ശൂന്യം *fdt,
						            പൂർണ്ണസംഖ്യ offset,
						            സ്ഥിരം അക്ഷരം *name,
						            പൂർണ്ണസംഖ്യ namelen,
							    പൂർണ്ണസംഖ്യ *lenp,
							    പൂർണ്ണസംഖ്യ *poffset)
{
	വേണ്ടി (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		സ്ഥിരം ഘടന fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		എങ്കിൽ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			നിർത്തുക;
		}
		എങ്കിൽ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			എങ്കിൽ (poffset)
				*poffset = offset;
			തിരികെ prop;
		}
	}

	എങ്കിൽ (lenp)
		*lenp = offset;
	തിരികെ ശൂന്യ;
}


സ്ഥിരം ഘടന fdt_property *fdt_get_property_namelen(സ്ഥിരം ശൂന്യം *fdt,
						    പൂർണ്ണസംഖ്യ offset,
						    സ്ഥിരം അക്ഷരം *name,
						    പൂർണ്ണസംഖ്യ namelen, പൂർണ്ണസംഖ്യ *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	എങ്കിൽ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		എങ്കിൽ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		തിരികെ ശൂന്യ;
	}

	തിരികെ fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 ശൂന്യ);
}


സ്ഥിരം ഘടന fdt_property *fdt_get_property(സ്ഥിരം ശൂന്യം *fdt,
					    പൂർണ്ണസംഖ്യ nodeoffset,
					    സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ *lenp)
{
	തിരികെ fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

സ്ഥിരം ശൂന്യം *fdt_getprop_namelen(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
				സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ namelen, പൂർണ്ണസംഖ്യ *lenp)
{
	പൂർണ്ണസംഖ്യ poffset;
	സ്ഥിരം ഘടന fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	എങ്കിൽ (!prop)
		തിരികെ ശൂന്യ;

	/* Handle realignment */
	എങ്കിൽ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + വലുപ്പം(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		തിരികെ prop->data + 4;
	തിരികെ prop->data;
}

സ്ഥിരം ശൂന്യം *fdt_getprop_by_offset(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ offset,
				  സ്ഥിരം അക്ഷരം **namep, പൂർണ്ണസംഖ്യ *lenp)
{
	സ്ഥിരം ഘടന fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	എങ്കിൽ (!prop)
		തിരികെ ശൂന്യ;
	എങ്കിൽ (namep) {
		സ്ഥിരം അക്ഷരം *name;
		പൂർണ്ണസംഖ്യ namelen;

		എങ്കിൽ (!can_assume(VALID_നിവേശം)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			എങ്കിൽ (!name) {
				എങ്കിൽ (lenp)
					*lenp = namelen;
				തിരികെ ശൂന്യ;
			}
			*namep = name;
		} അല്ലെങ്കിൽ {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	എങ്കിൽ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + വലുപ്പം(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		തിരികെ prop->data + 4;
	തിരികെ prop->data;
}

സ്ഥിരം ശൂന്യം *fdt_getprop(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
			സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ *lenp)
{
	തിരികെ fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset)
{
	സ്ഥിരം fdt32_t *php;
	പൂർണ്ണസംഖ്യ len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	എങ്കിൽ (!php || (len != വലുപ്പം(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		എങ്കിൽ (!php || (len != വലുപ്പം(*php)))
			തിരികെ 0;
	}

	തിരികെ fdt32_ld_(php);
}

സ്ഥിരം അക്ഷരം *fdt_get_alias_namelen(സ്ഥിരം ശൂന്യം *fdt,
				  സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ namelen)
{
	പൂർണ്ണസംഖ്യ aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	എങ്കിൽ (aliasoffset < 0)
		തിരികെ ശൂന്യ;

	തിരികെ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, ശൂന്യ);
}

സ്ഥിരം അക്ഷരം *fdt_get_alias(സ്ഥിരം ശൂന്യം *fdt, സ്ഥിരം അക്ഷരം *name)
{
	തിരികെ fdt_get_alias_namelen(fdt, name, strlen(name));
}

പൂർണ്ണസംഖ്യ fdt_get_path(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, അക്ഷരം *buf, പൂർണ്ണസംഖ്യ buflen)
{
	പൂർണ്ണസംഖ്യ pdepth = 0, p = 0;
	പൂർണ്ണസംഖ്യ offset, depth, namelen;
	സ്ഥിരം അക്ഷരം *name;

	FDT_RO_PROBE(fdt);

	എങ്കിൽ (buflen < 2)
		തിരികെ -FDT_ERR_NOSPACE;

	വേണ്ടി (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_കണ്ണി(fdt, offset, &depth)) {
		ആകുമ്പോൾ (pdepth > depth) {
			ചെയ്യുക {
				p--;
			} ആകുമ്പോൾ (buf[p-1] != '/');
			pdepth--;
		}

		എങ്കിൽ (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			എങ്കിൽ (!name)
				തിരികെ namelen;
			എങ്കിൽ ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		എങ്കിൽ (offset == nodeoffset) {
			എങ്കിൽ (pdepth < (depth + 1))
				തിരികെ -FDT_ERR_NOSPACE;

			എങ്കിൽ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			തിരികെ 0;
		}
	}

	എങ്കിൽ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		തിരികെ -FDT_ERR_BADOFFSET;
	അല്ലെങ്കിൽ എങ്കിൽ (offset == -FDT_ERR_BADOFFSET)
		തിരികെ -FDT_ERR_BADSTRUCTURE;

	തിരികെ offset; /* error from fdt_next_node() */
}

പൂർണ്ണസംഖ്യ fdt_supernode_atdepth_offset(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
				 പൂർണ്ണസംഖ്യ supernodedepth, പൂർണ്ണസംഖ്യ *nodedepth)
{
	പൂർണ്ണസംഖ്യ offset, depth;
	പൂർണ്ണസംഖ്യ supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	എങ്കിൽ (supernodedepth < 0)
		തിരികെ -FDT_ERR_NOTFOUND;

	വേണ്ടി (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_കണ്ണി(fdt, offset, &depth)) {
		എങ്കിൽ (depth == supernodedepth)
			supernodeoffset = offset;

		എങ്കിൽ (offset == nodeoffset) {
			എങ്കിൽ (nodedepth)
				*nodedepth = depth;

			എങ്കിൽ (supernodedepth > depth)
				തിരികെ -FDT_ERR_NOTFOUND;
			അല്ലെങ്കിൽ
				തിരികെ supernodeoffset;
		}
	}

	എങ്കിൽ (!can_assume(VALID_നിവേശം)) {
		എങ്കിൽ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			തിരികെ -FDT_ERR_BADOFFSET;
		അല്ലെങ്കിൽ എങ്കിൽ (offset == -FDT_ERR_BADOFFSET)
			തിരികെ -FDT_ERR_BADSTRUCTURE;
	}

	തിരികെ offset; /* error from fdt_next_node() */
}

പൂർണ്ണസംഖ്യ fdt_കണ്ണി_depth(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset)
{
	പൂർണ്ണസംഖ്യ nodedepth;
	പൂർണ്ണസംഖ്യ err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	എങ്കിൽ (err)
		തിരികെ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	തിരികെ nodedepth;
}

പൂർണ്ണസംഖ്യ fdt_parent_offset(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset)
{
	പൂർണ്ണസംഖ്യ nodedepth = fdt_കണ്ണി_depth(fdt, nodeoffset);

	എങ്കിൽ (nodedepth < 0)
		തിരികെ nodedepth;
	തിരികെ fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, ശൂന്യ);
}

പൂർണ്ണസംഖ്യ fdt_കണ്ണി_offset_by_prop_മൂല്യം(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ startoffset,
				  സ്ഥിരം അക്ഷരം *propname,
				  സ്ഥിരം ശൂന്യം *propval, പൂർണ്ണസംഖ്യ proplen)
{
	പൂർണ്ണസംഖ്യ offset;
	സ്ഥിരം ശൂന്യം *val;
	പൂർണ്ണസംഖ്യ len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	വേണ്ടി (offset = fdt_next_കണ്ണി(fdt, startoffset, ശൂന്യ);
	     offset >= 0;
	     offset = fdt_next_കണ്ണി(fdt, offset, ശൂന്യ)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		എങ്കിൽ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			തിരികെ offset;
	}

	തിരികെ offset; /* error from fdt_next_node() */
}

പൂർണ്ണസംഖ്യ fdt_കണ്ണി_offset_by_phandle(സ്ഥിരം ശൂന്യം *fdt, uint32_t phandle)
{
	പൂർണ്ണസംഖ്യ offset;

	എങ്കിൽ ((phandle == 0) || (phandle == ~0U))
		തിരികെ -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	വേണ്ടി (offset = fdt_next_കണ്ണി(fdt, -1, ശൂന്യ);
	     offset >= 0;
	     offset = fdt_next_കണ്ണി(fdt, offset, ശൂന്യ)) {
		എങ്കിൽ (fdt_get_phandle(fdt, offset) == phandle)
			തിരികെ offset;
	}

	തിരികെ offset; /* error from fdt_next_node() */
}

പൂർണ്ണസംഖ്യ fdt_stringlist_contains(സ്ഥിരം അക്ഷരം *strlist, പൂർണ്ണസംഖ്യ listlen, സ്ഥിരം അക്ഷരം *str)
{
	പൂർണ്ണസംഖ്യ len = strlen(str);
	സ്ഥിരം അക്ഷരം *p;

	ആകുമ്പോൾ (listlen >= len) {
		എങ്കിൽ (memcmp(str, strlist, len+1) == 0)
			തിരികെ 1;
		p = memchr(strlist, '\0', listlen);
		എങ്കിൽ (!p)
			തിരികെ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fdt_stringlist_എണ്ണം(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, സ്ഥിരം അക്ഷരം *property)
{
	സ്ഥിരം അക്ഷരം *list, *end;
	പൂർണ്ണസംഖ്യ length, എണ്ണം = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	എങ്കിൽ (!list)
		തിരികെ length;

	end = list + length;

	ആകുമ്പോൾ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		എങ്കിൽ (list + length > end)
			തിരികെ -FDT_ERR_BADVALUE;

		list += length;
		എണ്ണം++;
	}

	തിരികെ എണ്ണം;
}

പൂർണ്ണസംഖ്യ fdt_stringlist_തിരയൽ(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset, സ്ഥിരം അക്ഷരം *property,
			  സ്ഥിരം അക്ഷരം *string)
{
	പൂർണ്ണസംഖ്യ length, len, idx = 0;
	സ്ഥിരം അക്ഷരം *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	എങ്കിൽ (!list)
		തിരികെ length;

	len = strlen(string) + 1;
	end = list + length;

	ആകുമ്പോൾ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		എങ്കിൽ (list + length > end)
			തിരികെ -FDT_ERR_BADVALUE;

		എങ്കിൽ (length == len && memcmp(list, string, length) == 0)
			തിരികെ idx;

		list += length;
		idx++;
	}

	തിരികെ -FDT_ERR_NOTFOUND;
}

സ്ഥിരം അക്ഷരം *fdt_stringlist_get(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
			       സ്ഥിരം അക്ഷരം *property, പൂർണ്ണസംഖ്യ idx,
			       പൂർണ്ണസംഖ്യ *lenp)
{
	സ്ഥിരം അക്ഷരം *list, *end;
	പൂർണ്ണസംഖ്യ length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	എങ്കിൽ (!list) {
		എങ്കിൽ (lenp)
			*lenp = length;

		തിരികെ ശൂന്യ;
	}

	end = list + length;

	ആകുമ്പോൾ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		എങ്കിൽ (list + length > end) {
			എങ്കിൽ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			തിരികെ ശൂന്യ;
		}

		എങ്കിൽ (idx == 0) {
			എങ്കിൽ (lenp)
				*lenp = length - 1;

			തിരികെ list;
		}

		list += length;
		idx--;
	}

	എങ്കിൽ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	തിരികെ ശൂന്യ;
}

പൂർണ്ണസംഖ്യ fdt_കണ്ണി_check_compatible(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ nodeoffset,
			      സ്ഥിരം അക്ഷരം *compatible)
{
	സ്ഥിരം ശൂന്യം *prop;
	പൂർണ്ണസംഖ്യ len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	എങ്കിൽ (!prop)
		തിരികെ len;

	തിരികെ !fdt_stringlist_contains(prop, len, compatible);
}

പൂർണ്ണസംഖ്യ fdt_കണ്ണി_offset_by_compatible(സ്ഥിരം ശൂന്യം *fdt, പൂർണ്ണസംഖ്യ startoffset,
				  സ്ഥിരം അക്ഷരം *compatible)
{
	പൂർണ്ണസംഖ്യ offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	വേണ്ടി (offset = fdt_next_കണ്ണി(fdt, startoffset, ശൂന്യ);
	     offset >= 0;
	     offset = fdt_next_കണ്ണി(fdt, offset, ശൂന്യ)) {
		err = fdt_കണ്ണി_check_compatible(fdt, offset, compatible);
		എങ്കിൽ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			തിരികെ err;
		അല്ലെങ്കിൽ എങ്കിൽ (err == 0)
			തിരികെ offset;
	}

	തിരികെ offset; /* error from fdt_next_node() */
}
