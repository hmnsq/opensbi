// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static puur_naa_nka fdt_nodename_eq_(const void *fdt, puur_naa_nka offset,
			    const var_na *s, puur_naa_nka len)
{
	puur_naa_nka olen;
	const var_na *p = fdt_get_name(fdt, offset, &olen);

	ᮚᮦᮔ᮪ (!p || olen < len)
		/* short match */
		ᮝᮜᮤ 0;

	ᮚᮦᮔ᮪ (memcmp(p, s, len) != 0)
		ᮝᮜᮤ 0;

	ᮚᮦᮔ᮪ (p[len] == '\0')
		ᮝᮜᮤ 1;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (!memchr(s, '@', len) && (p[len] == '@'))
		ᮝᮜᮤ 1;
	ᮜᮤᮚ
		ᮝᮜᮤ 0;
}

const var_na *fdt_get_string(const void *fdt, puur_naa_nka stroffset, puur_naa_nka *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	puur_naa_nka err;
	const var_na *s, *n;

	ᮚᮦᮔ᮪ (can_assume(VALID_INPUT)) {
		s = (const var_na *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		ᮚᮦᮔ᮪ (lenp)
			*lenp = strlen(s);
		ᮝᮜᮤ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ᮚᮦᮔ᮪ (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	ᮚᮦᮔ᮪ (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	ᮚᮦᮔ᮪ (fdt_magic(fdt) == FDT_MAGIC) {
		ᮚᮦᮔ᮪ (stroffset < 0)
			goto fail;
		ᮚᮦᮔ᮪ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			ᮚᮦᮔ᮪ ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			ᮚᮦᮔ᮪ ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} ᮜᮤᮚ ᮚᮦᮔ᮪ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned puur_naa_nka sw_stroffset = -stroffset;

		ᮚᮦᮔ᮪ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		ᮚᮦᮔ᮪ (sw_stroffset < len)
			len = sw_stroffset;
	} ᮜᮤᮚ {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const var_na *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ᮚᮦᮔ᮪ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	ᮚᮦᮔ᮪ (lenp)
		*lenp = n - s;
	ᮝᮜᮤ s;

fail:
	ᮚᮦᮔ᮪ (lenp)
		*lenp = err;
	ᮝᮜᮤ NULL;
}

const var_na *fdt_string(const void *fdt, puur_naa_nka stroffset)
{
	ᮝᮜᮤ fdt_get_string(fdt, stroffset, NULL);
}

static puur_naa_nka fdt_string_eq_(const void *fdt, puur_naa_nka stroffset,
			  const var_na *s, puur_naa_nka len)
{
	puur_naa_nka slen;
	const var_na *p = fdt_get_string(fdt, stroffset, &slen);

	ᮝᮜᮤ p && (slen == len) && (memcmp(p, s, len) == 0);
}

puur_naa_nka fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	puur_naa_nka offset = -1;

	ᮞᮨᮓᮨᮜ (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		ᮚᮦᮔ᮪ (offset < 0) {
			ᮚᮦᮔ᮪ (offset == -FDT_ERR_NOTFOUND)
				bha_nga;

			ᮝᮜᮤ offset;
		}

		value = fdt_get_phandle(fdt, offset);

		ᮚᮦᮔ᮪ (value > max)
			max = value;
	}

	ᮚᮦᮔ᮪ (phandle)
		*phandle = max;

	ᮝᮜᮤ 0;
}

puur_naa_nka fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	puur_naa_nka err;

	err = fdt_find_max_phandle(fdt, &max);
	ᮚᮦᮔ᮪ (err < 0)
		ᮝᮜᮤ err;

	ᮚᮦᮔ᮪ (max == FDT_MAX_PHANDLE)
		ᮝᮜᮤ -FDT_ERR_NOPHANDLES;

	ᮚᮦᮔ᮪ (phandle)
		*phandle = max + 1;

	ᮝᮜᮤ 0;
}

static const sa_nracanaa fdt_reserve_entry *fdt_mem_rsv(const void *fdt, puur_naa_nka n)
{
	unsigned puur_naa_nka offset = n * parimaa_na(sa_nracanaa fdt_reserve_entry);
	unsigned puur_naa_nka absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	ᮚᮦᮔ᮪ (!can_assume(VALID_INPUT)) {
		ᮚᮦᮔ᮪ (absoffset < fdt_off_mem_rsvmap(fdt))
			ᮝᮜᮤ NULL;
		ᮚᮦᮔ᮪ (absoffset > fdt_totalsize(fdt) -
		    parimaa_na(sa_nracanaa fdt_reserve_entry))
			ᮝᮜᮤ NULL;
	}
	ᮝᮜᮤ fdt_mem_rsv_(fdt, n);
}

puur_naa_nka fdt_get_mem_rsv(const void *fdt, puur_naa_nka n, uint64_t *address, uint64_t *size)
{
	const sa_nracanaa fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ᮚᮦᮔ᮪ (!can_assume(VALID_INPUT) && !re)
		ᮝᮜᮤ -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ᮝᮜᮤ 0;
}

puur_naa_nka fdt_num_mem_rsv(const void *fdt)
{
	puur_naa_nka i;
	const sa_nracanaa fdt_reserve_entry *re;

	for (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		ᮚᮦᮔ᮪ (fdt64_ld_(&re->size) == 0)
			ᮝᮜᮤ i;
	}
	ᮝᮜᮤ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka nextprop_(const void *fdt, puur_naa_nka offset)
{
	uint32_t tag;
	puur_naa_nka nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_END:
			ᮚᮦᮔ᮪ (nextoffset >= 0)
				ᮝᮜᮤ -FDT_ERR_BADSTRUCTURE;
			ᮜᮤᮚ
				ᮝᮜᮤ nextoffset;

		pak_sa FDT_PROP:
			ᮝᮜᮤ offset;
		}
		offset = nextoffset;
	} ᮞᮨᮓᮨᮜ (tag == FDT_NOP);

	ᮝᮜᮤ -FDT_ERR_NOTFOUND;
}

puur_naa_nka fdt_subnode_offset_namelen(const void *fdt, puur_naa_nka offset,
			       const var_na *name, puur_naa_nka namelen)
{
	puur_naa_nka depth;

	FDT_RO_PROBE(fdt);

	for (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		ᮚᮦᮔ᮪ ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ᮝᮜᮤ offset;

	ᮚᮦᮔ᮪ (depth < 0)
		ᮝᮜᮤ -FDT_ERR_NOTFOUND;
	ᮝᮜᮤ offset; /* error */
}

puur_naa_nka fdt_subnode_offset(const void *fdt, puur_naa_nka parentoffset,
		       const var_na *name)
{
	ᮝᮜᮤ fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

puur_naa_nka fdt_path_offset_namelen(const void *fdt, const var_na *path, puur_naa_nka namelen)
{
	const var_na *end = path + namelen;
	const var_na *p = path;
	puur_naa_nka offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ᮚᮦᮔ᮪ (*path != '/') {
		const var_na *q = memchr(path, '/', end - p);

		ᮚᮦᮔ᮪ (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ᮚᮦᮔ᮪ (!p)
			ᮝᮜᮤ -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	ᮞᮨᮓᮨᮜ (p < end) {
		const var_na *q;

		ᮞᮨᮓᮨᮜ (*p == '/') {
			p++;
			ᮚᮦᮔ᮪ (p == end)
				ᮝᮜᮤ offset;
		}
		q = memchr(p, '/', end - p);
		ᮚᮦᮔ᮪ (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		ᮚᮦᮔ᮪ (offset < 0)
			ᮝᮜᮤ offset;

		p = q;
	}

	ᮝᮜᮤ offset;
}

puur_naa_nka fdt_path_offset(const void *fdt, const var_na *path)
{
	ᮝᮜᮤ fdt_path_offset_namelen(fdt, path, strlen(path));
}

const var_na *fdt_get_name(const void *fdt, puur_naa_nka nodeoffset, puur_naa_nka *len)
{
	const sa_nracanaa fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const var_na *nameptr;
	puur_naa_nka err;

	ᮚᮦᮔ᮪ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	ᮚᮦᮔ᮪ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const var_na *leaf;
		leaf = strrchr(nameptr, '/');
		ᮚᮦᮔ᮪ (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	ᮚᮦᮔ᮪ (len)
		*len = strlen(nameptr);

	ᮝᮜᮤ nameptr;

 fail:
	ᮚᮦᮔ᮪ (len)
		*len = err;
	ᮝᮜᮤ NULL;
}

puur_naa_nka fdt_first_property_offset(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka offset;

	ᮚᮦᮔ᮪ ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ᮝᮜᮤ offset;

	ᮝᮜᮤ nextprop_(fdt, offset);
}

puur_naa_nka fdt_next_property_offset(const void *fdt, puur_naa_nka offset)
{
	ᮚᮦᮔ᮪ ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ᮝᮜᮤ offset;

	ᮝᮜᮤ nextprop_(fdt, offset);
}

static const sa_nracanaa fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              puur_naa_nka offset,
						              puur_naa_nka *lenp)
{
	puur_naa_nka err;
	const sa_nracanaa fdt_property *prop;

	ᮚᮦᮔ᮪ (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		ᮚᮦᮔ᮪ (lenp)
			*lenp = err;
		ᮝᮜᮤ NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	ᮚᮦᮔ᮪ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ᮝᮜᮤ prop;
}

const sa_nracanaa fdt_property *fdt_get_property_by_offset(const void *fdt,
						      puur_naa_nka offset,
						      puur_naa_nka *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ᮚᮦᮔ᮪ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ᮚᮦᮔ᮪ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ᮝᮜᮤ NULL;
	}

	ᮝᮜᮤ fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const sa_nracanaa fdt_property *fdt_get_property_namelen_(const void *fdt,
						            puur_naa_nka offset,
						            const var_na *name,
						            puur_naa_nka namelen,
							    puur_naa_nka *lenp,
							    puur_naa_nka *poffset)
{
	for (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const sa_nracanaa fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		ᮚᮦᮔ᮪ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			bha_nga;
		}
		ᮚᮦᮔ᮪ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ᮚᮦᮔ᮪ (poffset)
				*poffset = offset;
			ᮝᮜᮤ prop;
		}
	}

	ᮚᮦᮔ᮪ (lenp)
		*lenp = offset;
	ᮝᮜᮤ NULL;
}


const sa_nracanaa fdt_property *fdt_get_property_namelen(const void *fdt,
						    puur_naa_nka offset,
						    const var_na *name,
						    puur_naa_nka namelen, puur_naa_nka *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ᮚᮦᮔ᮪ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ᮚᮦᮔ᮪ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ᮝᮜᮤ NULL;
	}

	ᮝᮜᮤ fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const sa_nracanaa fdt_property *fdt_get_property(const void *fdt,
					    puur_naa_nka nodeoffset,
					    const var_na *name, puur_naa_nka *lenp)
{
	ᮝᮜᮤ fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, puur_naa_nka nodeoffset,
				const var_na *name, puur_naa_nka namelen, puur_naa_nka *lenp)
{
	puur_naa_nka poffset;
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ᮚᮦᮔ᮪ (!prop)
		ᮝᮜᮤ NULL;

	/* Handle realignment */
	ᮚᮦᮔ᮪ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ᮝᮜᮤ prop->data + 4;
	ᮝᮜᮤ prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, puur_naa_nka offset,
				  const var_na **namep, puur_naa_nka *lenp)
{
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	ᮚᮦᮔ᮪ (!prop)
		ᮝᮜᮤ NULL;
	ᮚᮦᮔ᮪ (namep) {
		const var_na *name;
		puur_naa_nka namelen;

		ᮚᮦᮔ᮪ (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ᮚᮦᮔ᮪ (!name) {
				ᮚᮦᮔ᮪ (lenp)
					*lenp = namelen;
				ᮝᮜᮤ NULL;
			}
			*namep = name;
		} ᮜᮤᮚ {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ᮚᮦᮔ᮪ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ᮝᮜᮤ prop->data + 4;
	ᮝᮜᮤ prop->data;
}

const void *fdt_getprop(const void *fdt, puur_naa_nka nodeoffset,
			const var_na *name, puur_naa_nka *lenp)
{
	ᮝᮜᮤ fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, puur_naa_nka nodeoffset)
{
	const fdt32_t *php;
	puur_naa_nka len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ᮚᮦᮔ᮪ (!php || (len != parimaa_na(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ᮚᮦᮔ᮪ (!php || (len != parimaa_na(*php)))
			ᮝᮜᮤ 0;
	}

	ᮝᮜᮤ fdt32_ld_(php);
}

const var_na *fdt_get_alias_namelen(const void *fdt,
				  const var_na *name, puur_naa_nka namelen)
{
	puur_naa_nka aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	ᮚᮦᮔ᮪ (aliasoffset < 0)
		ᮝᮜᮤ NULL;

	ᮝᮜᮤ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const var_na *fdt_get_alias(const void *fdt, const var_na *name)
{
	ᮝᮜᮤ fdt_get_alias_namelen(fdt, name, strlen(name));
}

puur_naa_nka fdt_get_path(const void *fdt, puur_naa_nka nodeoffset, var_na *buf, puur_naa_nka buflen)
{
	puur_naa_nka pdepth = 0, p = 0;
	puur_naa_nka offset, depth, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	ᮚᮦᮔ᮪ (buflen < 2)
		ᮝᮜᮤ -FDT_ERR_NOSPACE;

	for (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ᮞᮨᮓᮨᮜ (pdepth > depth) {
			do {
				p--;
			} ᮞᮨᮓᮨᮜ (buf[p-1] != '/');
			pdepth--;
		}

		ᮚᮦᮔ᮪ (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			ᮚᮦᮔ᮪ (!name)
				ᮝᮜᮤ namelen;
			ᮚᮦᮔ᮪ ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ᮚᮦᮔ᮪ (offset == nodeoffset) {
			ᮚᮦᮔ᮪ (pdepth < (depth + 1))
				ᮝᮜᮤ -FDT_ERR_NOSPACE;

			ᮚᮦᮔ᮪ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ᮝᮜᮤ 0;
		}
	}

	ᮚᮦᮔ᮪ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ᮝᮜᮤ -FDT_ERR_BADOFFSET;
	ᮜᮤᮚ ᮚᮦᮔ᮪ (offset == -FDT_ERR_BADOFFSET)
		ᮝᮜᮤ -FDT_ERR_BADSTRUCTURE;

	ᮝᮜᮤ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_supernode_atdepth_offset(const void *fdt, puur_naa_nka nodeoffset,
				 puur_naa_nka supernodedepth, puur_naa_nka *nodedepth)
{
	puur_naa_nka offset, depth;
	puur_naa_nka supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	ᮚᮦᮔ᮪ (supernodedepth < 0)
		ᮝᮜᮤ -FDT_ERR_NOTFOUND;

	for (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ᮚᮦᮔ᮪ (depth == supernodedepth)
			supernodeoffset = offset;

		ᮚᮦᮔ᮪ (offset == nodeoffset) {
			ᮚᮦᮔ᮪ (nodedepth)
				*nodedepth = depth;

			ᮚᮦᮔ᮪ (supernodedepth > depth)
				ᮝᮜᮤ -FDT_ERR_NOTFOUND;
			ᮜᮤᮚ
				ᮝᮜᮤ supernodeoffset;
		}
	}

	ᮚᮦᮔ᮪ (!can_assume(VALID_INPUT)) {
		ᮚᮦᮔ᮪ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ᮝᮜᮤ -FDT_ERR_BADOFFSET;
		ᮜᮤᮚ ᮚᮦᮔ᮪ (offset == -FDT_ERR_BADOFFSET)
			ᮝᮜᮤ -FDT_ERR_BADSTRUCTURE;
	}

	ᮝᮜᮤ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_node_depth(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka nodedepth;
	puur_naa_nka err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	ᮚᮦᮔ᮪ (err)
		ᮝᮜᮤ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ᮝᮜᮤ nodedepth;
}

puur_naa_nka fdt_parent_offset(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka nodedepth = fdt_node_depth(fdt, nodeoffset);

	ᮚᮦᮔ᮪ (nodedepth < 0)
		ᮝᮜᮤ nodedepth;
	ᮝᮜᮤ fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

puur_naa_nka fdt_node_offset_by_prop_value(const void *fdt, puur_naa_nka startoffset,
				  const var_na *propname,
				  const void *propval, puur_naa_nka proplen)
{
	puur_naa_nka offset;
	const void *val;
	puur_naa_nka len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	for (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		ᮚᮦᮔ᮪ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ᮝᮜᮤ offset;
	}

	ᮝᮜᮤ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	puur_naa_nka offset;

	ᮚᮦᮔ᮪ ((phandle == 0) || (phandle == ~0U))
		ᮝᮜᮤ -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	for (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		ᮚᮦᮔ᮪ (fdt_get_phandle(fdt, offset) == phandle)
			ᮝᮜᮤ offset;
	}

	ᮝᮜᮤ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_stringlist_contains(const var_na *strlist, puur_naa_nka listlen, const var_na *str)
{
	puur_naa_nka len = strlen(str);
	const var_na *p;

	ᮞᮨᮓᮨᮜ (listlen >= len) {
		ᮚᮦᮔ᮪ (memcmp(str, strlist, len+1) == 0)
			ᮝᮜᮤ 1;
		p = memchr(strlist, '\0', listlen);
		ᮚᮦᮔ᮪ (!p)
			ᮝᮜᮤ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ᮝᮜᮤ 0;
}

puur_naa_nka fdt_stringlist_count(const void *fdt, puur_naa_nka nodeoffset, const var_na *property)
{
	const var_na *list, *end;
	puur_naa_nka length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ᮚᮦᮔ᮪ (!list)
		ᮝᮜᮤ length;

	end = list + length;

	ᮞᮨᮓᮨᮜ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ᮚᮦᮔ᮪ (list + length > end)
			ᮝᮜᮤ -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	ᮝᮜᮤ count;
}

puur_naa_nka fdt_stringlist_search(const void *fdt, puur_naa_nka nodeoffset, const var_na *property,
			  const var_na *string)
{
	puur_naa_nka length, len, idx = 0;
	const var_na *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ᮚᮦᮔ᮪ (!list)
		ᮝᮜᮤ length;

	len = strlen(string) + 1;
	end = list + length;

	ᮞᮨᮓᮨᮜ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ᮚᮦᮔ᮪ (list + length > end)
			ᮝᮜᮤ -FDT_ERR_BADVALUE;

		ᮚᮦᮔ᮪ (length == len && memcmp(list, string, length) == 0)
			ᮝᮜᮤ idx;

		list += length;
		idx++;
	}

	ᮝᮜᮤ -FDT_ERR_NOTFOUND;
}

const var_na *fdt_stringlist_get(const void *fdt, puur_naa_nka nodeoffset,
			       const var_na *property, puur_naa_nka idx,
			       puur_naa_nka *lenp)
{
	const var_na *list, *end;
	puur_naa_nka length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ᮚᮦᮔ᮪ (!list) {
		ᮚᮦᮔ᮪ (lenp)
			*lenp = length;

		ᮝᮜᮤ NULL;
	}

	end = list + length;

	ᮞᮨᮓᮨᮜ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ᮚᮦᮔ᮪ (list + length > end) {
			ᮚᮦᮔ᮪ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ᮝᮜᮤ NULL;
		}

		ᮚᮦᮔ᮪ (idx == 0) {
			ᮚᮦᮔ᮪ (lenp)
				*lenp = length - 1;

			ᮝᮜᮤ list;
		}

		list += length;
		idx--;
	}

	ᮚᮦᮔ᮪ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ᮝᮜᮤ NULL;
}

puur_naa_nka fdt_node_check_compatible(const void *fdt, puur_naa_nka nodeoffset,
			      const var_na *compatible)
{
	const void *prop;
	puur_naa_nka len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ᮚᮦᮔ᮪ (!prop)
		ᮝᮜᮤ len;

	ᮝᮜᮤ !fdt_stringlist_contains(prop, len, compatible);
}

puur_naa_nka fdt_node_offset_by_compatible(const void *fdt, puur_naa_nka startoffset,
				  const var_na *compatible)
{
	puur_naa_nka offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	for (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		ᮚᮦᮔ᮪ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ᮝᮜᮤ err;
		ᮜᮤᮚ ᮚᮦᮔ᮪ (err == 0)
			ᮝᮜᮤ offset;
	}

	ᮝᮜᮤ offset; /* error from fdt_next_node() */
}
