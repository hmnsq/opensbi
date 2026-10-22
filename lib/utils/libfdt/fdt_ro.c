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

	ᬬᬾᬦ᭄ (!p || olen < len)
		/* short match */
		ᬯᬮᬶ 0;

	ᬬᬾᬦ᭄ (memcmp(p, s, len) != 0)
		ᬯᬮᬶ 0;

	ᬬᬾᬦ᭄ (p[len] == '\0')
		ᬯᬮᬶ 1;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (!memchr(s, '@', len) && (p[len] == '@'))
		ᬯᬮᬶ 1;
	ᬮᬶᬬ
		ᬯᬮᬶ 0;
}

const var_na *fdt_get_string(const void *fdt, puur_naa_nka stroffset, puur_naa_nka *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	puur_naa_nka err;
	const var_na *s, *n;

	ᬬᬾᬦ᭄ (can_assume(VALID_INPUT)) {
		s = (const var_na *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		ᬬᬾᬦ᭄ (lenp)
			*lenp = strlen(s);
		ᬯᬮᬶ s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	ᬬᬾᬦ᭄ (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	ᬬᬾᬦ᭄ (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	ᬬᬾᬦ᭄ (fdt_magic(fdt) == FDT_MAGIC) {
		ᬬᬾᬦ᭄ (stroffset < 0)
			goto fail;
		ᬬᬾᬦ᭄ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			ᬬᬾᬦ᭄ ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			ᬬᬾᬦ᭄ ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} ᬮᬶᬬ ᬬᬾᬦ᭄ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned puur_naa_nka sw_stroffset = -stroffset;

		ᬬᬾᬦ᭄ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		ᬬᬾᬦ᭄ (sw_stroffset < len)
			len = sw_stroffset;
	} ᬮᬶᬬ {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const var_na *)fdt + absoffset;
	n = memchr(s, '\0', len);
	ᬬᬾᬦ᭄ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	ᬬᬾᬦ᭄ (lenp)
		*lenp = n - s;
	ᬯᬮᬶ s;

fail:
	ᬬᬾᬦ᭄ (lenp)
		*lenp = err;
	ᬯᬮᬶ NULL;
}

const var_na *fdt_string(const void *fdt, puur_naa_nka stroffset)
{
	ᬯᬮᬶ fdt_get_string(fdt, stroffset, NULL);
}

static puur_naa_nka fdt_string_eq_(const void *fdt, puur_naa_nka stroffset,
			  const var_na *s, puur_naa_nka len)
{
	puur_naa_nka slen;
	const var_na *p = fdt_get_string(fdt, stroffset, &slen);

	ᬯᬮᬶ p && (slen == len) && (memcmp(p, s, len) == 0);
}

puur_naa_nka fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	puur_naa_nka offset = -1;

	ᬲᭂᬤᭂᬮ (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		ᬬᬾᬦ᭄ (offset < 0) {
			ᬬᬾᬦ᭄ (offset == -FDT_ERR_NOTFOUND)
				bha_nga;

			ᬯᬮᬶ offset;
		}

		value = fdt_get_phandle(fdt, offset);

		ᬬᬾᬦ᭄ (value > max)
			max = value;
	}

	ᬬᬾᬦ᭄ (phandle)
		*phandle = max;

	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	puur_naa_nka err;

	err = fdt_find_max_phandle(fdt, &max);
	ᬬᬾᬦ᭄ (err < 0)
		ᬯᬮᬶ err;

	ᬬᬾᬦ᭄ (max == FDT_MAX_PHANDLE)
		ᬯᬮᬶ -FDT_ERR_NOPHANDLES;

	ᬬᬾᬦ᭄ (phandle)
		*phandle = max + 1;

	ᬯᬮᬶ 0;
}

static const sa_nracanaa fdt_reserve_entry *fdt_mem_rsv(const void *fdt, puur_naa_nka n)
{
	unsigned puur_naa_nka offset = n * parimaa_na(sa_nracanaa fdt_reserve_entry);
	unsigned puur_naa_nka absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	ᬬᬾᬦ᭄ (!can_assume(VALID_INPUT)) {
		ᬬᬾᬦ᭄ (absoffset < fdt_off_mem_rsvmap(fdt))
			ᬯᬮᬶ NULL;
		ᬬᬾᬦ᭄ (absoffset > fdt_totalsize(fdt) -
		    parimaa_na(sa_nracanaa fdt_reserve_entry))
			ᬯᬮᬶ NULL;
	}
	ᬯᬮᬶ fdt_mem_rsv_(fdt, n);
}

puur_naa_nka fdt_get_mem_rsv(const void *fdt, puur_naa_nka n, uint64_t *address, uint64_t *size)
{
	const sa_nracanaa fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	ᬬᬾᬦ᭄ (!can_assume(VALID_INPUT) && !re)
		ᬯᬮᬶ -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_num_mem_rsv(const void *fdt)
{
	puur_naa_nka i;
	const sa_nracanaa fdt_reserve_entry *re;

	for (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		ᬬᬾᬦ᭄ (fdt64_ld_(&re->size) == 0)
			ᬯᬮᬶ i;
	}
	ᬯᬮᬶ -FDT_ERR_TRUNCATED;
}

static puur_naa_nka nextprop_(const void *fdt, puur_naa_nka offset)
{
	uint32_t tag;
	puur_naa_nka nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_END:
			ᬬᬾᬦ᭄ (nextoffset >= 0)
				ᬯᬮᬶ -FDT_ERR_BADSTRUCTURE;
			ᬮᬶᬬ
				ᬯᬮᬶ nextoffset;

		pak_sa FDT_PROP:
			ᬯᬮᬶ offset;
		}
		offset = nextoffset;
	} ᬲᭂᬤᭂᬮ (tag == FDT_NOP);

	ᬯᬮᬶ -FDT_ERR_NOTFOUND;
}

puur_naa_nka fdt_subnode_offset_namelen(const void *fdt, puur_naa_nka offset,
			       const var_na *name, puur_naa_nka namelen)
{
	puur_naa_nka depth;

	FDT_RO_PROBE(fdt);

	for (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		ᬬᬾᬦ᭄ ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ᬯᬮᬶ offset;

	ᬬᬾᬦ᭄ (depth < 0)
		ᬯᬮᬶ -FDT_ERR_NOTFOUND;
	ᬯᬮᬶ offset; /* error */
}

puur_naa_nka fdt_subnode_offset(const void *fdt, puur_naa_nka parentoffset,
		       const var_na *name)
{
	ᬯᬮᬶ fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

puur_naa_nka fdt_path_offset_namelen(const void *fdt, const var_na *path, puur_naa_nka namelen)
{
	const var_na *end = path + namelen;
	const var_na *p = path;
	puur_naa_nka offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	ᬬᬾᬦ᭄ (*path != '/') {
		const var_na *q = memchr(path, '/', end - p);

		ᬬᬾᬦ᭄ (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		ᬬᬾᬦ᭄ (!p)
			ᬯᬮᬶ -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	ᬲᭂᬤᭂᬮ (p < end) {
		const var_na *q;

		ᬲᭂᬤᭂᬮ (*p == '/') {
			p++;
			ᬬᬾᬦ᭄ (p == end)
				ᬯᬮᬶ offset;
		}
		q = memchr(p, '/', end - p);
		ᬬᬾᬦ᭄ (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		ᬬᬾᬦ᭄ (offset < 0)
			ᬯᬮᬶ offset;

		p = q;
	}

	ᬯᬮᬶ offset;
}

puur_naa_nka fdt_path_offset(const void *fdt, const var_na *path)
{
	ᬯᬮᬶ fdt_path_offset_namelen(fdt, path, strlen(path));
}

const var_na *fdt_get_name(const void *fdt, puur_naa_nka nodeoffset, puur_naa_nka *len)
{
	const sa_nracanaa fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const var_na *nameptr;
	puur_naa_nka err;

	ᬬᬾᬦ᭄ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	ᬬᬾᬦ᭄ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const var_na *leaf;
		leaf = strrchr(nameptr, '/');
		ᬬᬾᬦ᭄ (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	ᬬᬾᬦ᭄ (len)
		*len = strlen(nameptr);

	ᬯᬮᬶ nameptr;

 fail:
	ᬬᬾᬦ᭄ (len)
		*len = err;
	ᬯᬮᬶ NULL;
}

puur_naa_nka fdt_first_property_offset(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka offset;

	ᬬᬾᬦ᭄ ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ᬯᬮᬶ offset;

	ᬯᬮᬶ nextprop_(fdt, offset);
}

puur_naa_nka fdt_next_property_offset(const void *fdt, puur_naa_nka offset)
{
	ᬬᬾᬦ᭄ ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ᬯᬮᬶ offset;

	ᬯᬮᬶ nextprop_(fdt, offset);
}

static const sa_nracanaa fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              puur_naa_nka offset,
						              puur_naa_nka *lenp)
{
	puur_naa_nka err;
	const sa_nracanaa fdt_property *prop;

	ᬬᬾᬦ᭄ (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		ᬬᬾᬦ᭄ (lenp)
			*lenp = err;
		ᬯᬮᬶ NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	ᬬᬾᬦ᭄ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ᬯᬮᬶ prop;
}

const sa_nracanaa fdt_property *fdt_get_property_by_offset(const void *fdt,
						      puur_naa_nka offset,
						      puur_naa_nka *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	ᬬᬾᬦ᭄ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ᬬᬾᬦ᭄ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ᬯᬮᬶ NULL;
	}

	ᬯᬮᬶ fdt_get_property_by_offset_(fdt, offset, lenp);
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
		ᬬᬾᬦ᭄ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			bha_nga;
		}
		ᬬᬾᬦ᭄ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			ᬬᬾᬦ᭄ (poffset)
				*poffset = offset;
			ᬯᬮᬶ prop;
		}
	}

	ᬬᬾᬦ᭄ (lenp)
		*lenp = offset;
	ᬯᬮᬶ NULL;
}


const sa_nracanaa fdt_property *fdt_get_property_namelen(const void *fdt,
						    puur_naa_nka offset,
						    const var_na *name,
						    puur_naa_nka namelen, puur_naa_nka *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	ᬬᬾᬦ᭄ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		ᬬᬾᬦ᭄ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ᬯᬮᬶ NULL;
	}

	ᬯᬮᬶ fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const sa_nracanaa fdt_property *fdt_get_property(const void *fdt,
					    puur_naa_nka nodeoffset,
					    const var_na *name, puur_naa_nka *lenp)
{
	ᬯᬮᬶ fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, puur_naa_nka nodeoffset,
				const var_na *name, puur_naa_nka namelen, puur_naa_nka *lenp)
{
	puur_naa_nka poffset;
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	ᬬᬾᬦ᭄ (!prop)
		ᬯᬮᬶ NULL;

	/* Handle realignment */
	ᬬᬾᬦ᭄ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ᬯᬮᬶ prop->data + 4;
	ᬯᬮᬶ prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, puur_naa_nka offset,
				  const var_na **namep, puur_naa_nka *lenp)
{
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	ᬬᬾᬦ᭄ (!prop)
		ᬯᬮᬶ NULL;
	ᬬᬾᬦ᭄ (namep) {
		const var_na *name;
		puur_naa_nka namelen;

		ᬬᬾᬦ᭄ (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			ᬬᬾᬦ᭄ (!name) {
				ᬬᬾᬦ᭄ (lenp)
					*lenp = namelen;
				ᬯᬮᬶ NULL;
			}
			*namep = name;
		} ᬮᬶᬬ {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	ᬬᬾᬦ᭄ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ᬯᬮᬶ prop->data + 4;
	ᬯᬮᬶ prop->data;
}

const void *fdt_getprop(const void *fdt, puur_naa_nka nodeoffset,
			const var_na *name, puur_naa_nka *lenp)
{
	ᬯᬮᬶ fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, puur_naa_nka nodeoffset)
{
	const fdt32_t *php;
	puur_naa_nka len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	ᬬᬾᬦ᭄ (!php || (len != parimaa_na(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		ᬬᬾᬦ᭄ (!php || (len != parimaa_na(*php)))
			ᬯᬮᬶ 0;
	}

	ᬯᬮᬶ fdt32_ld_(php);
}

const var_na *fdt_get_alias_namelen(const void *fdt,
				  const var_na *name, puur_naa_nka namelen)
{
	puur_naa_nka aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	ᬬᬾᬦ᭄ (aliasoffset < 0)
		ᬯᬮᬶ NULL;

	ᬯᬮᬶ fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const var_na *fdt_get_alias(const void *fdt, const var_na *name)
{
	ᬯᬮᬶ fdt_get_alias_namelen(fdt, name, strlen(name));
}

puur_naa_nka fdt_get_path(const void *fdt, puur_naa_nka nodeoffset, var_na *buf, puur_naa_nka buflen)
{
	puur_naa_nka pdepth = 0, p = 0;
	puur_naa_nka offset, depth, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	ᬬᬾᬦ᭄ (buflen < 2)
		ᬯᬮᬶ -FDT_ERR_NOSPACE;

	for (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ᬲᭂᬤᭂᬮ (pdepth > depth) {
			do {
				p--;
			} ᬲᭂᬤᭂᬮ (buf[p-1] != '/');
			pdepth--;
		}

		ᬬᬾᬦ᭄ (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			ᬬᬾᬦ᭄ (!name)
				ᬯᬮᬶ namelen;
			ᬬᬾᬦ᭄ ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		ᬬᬾᬦ᭄ (offset == nodeoffset) {
			ᬬᬾᬦ᭄ (pdepth < (depth + 1))
				ᬯᬮᬶ -FDT_ERR_NOSPACE;

			ᬬᬾᬦ᭄ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ᬯᬮᬶ 0;
		}
	}

	ᬬᬾᬦ᭄ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ᬯᬮᬶ -FDT_ERR_BADOFFSET;
	ᬮᬶᬬ ᬬᬾᬦ᭄ (offset == -FDT_ERR_BADOFFSET)
		ᬯᬮᬶ -FDT_ERR_BADSTRUCTURE;

	ᬯᬮᬶ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_supernode_atdepth_offset(const void *fdt, puur_naa_nka nodeoffset,
				 puur_naa_nka supernodedepth, puur_naa_nka *nodedepth)
{
	puur_naa_nka offset, depth;
	puur_naa_nka supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	ᬬᬾᬦ᭄ (supernodedepth < 0)
		ᬯᬮᬶ -FDT_ERR_NOTFOUND;

	for (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		ᬬᬾᬦ᭄ (depth == supernodedepth)
			supernodeoffset = offset;

		ᬬᬾᬦ᭄ (offset == nodeoffset) {
			ᬬᬾᬦ᭄ (nodedepth)
				*nodedepth = depth;

			ᬬᬾᬦ᭄ (supernodedepth > depth)
				ᬯᬮᬶ -FDT_ERR_NOTFOUND;
			ᬮᬶᬬ
				ᬯᬮᬶ supernodeoffset;
		}
	}

	ᬬᬾᬦ᭄ (!can_assume(VALID_INPUT)) {
		ᬬᬾᬦ᭄ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ᬯᬮᬶ -FDT_ERR_BADOFFSET;
		ᬮᬶᬬ ᬬᬾᬦ᭄ (offset == -FDT_ERR_BADOFFSET)
			ᬯᬮᬶ -FDT_ERR_BADSTRUCTURE;
	}

	ᬯᬮᬶ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_node_depth(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka nodedepth;
	puur_naa_nka err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	ᬬᬾᬦ᭄ (err)
		ᬯᬮᬶ (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ᬯᬮᬶ nodedepth;
}

puur_naa_nka fdt_parent_offset(const void *fdt, puur_naa_nka nodeoffset)
{
	puur_naa_nka nodedepth = fdt_node_depth(fdt, nodeoffset);

	ᬬᬾᬦ᭄ (nodedepth < 0)
		ᬯᬮᬶ nodedepth;
	ᬯᬮᬶ fdt_supernode_atdepth_offset(fdt, nodeoffset,
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
		ᬬᬾᬦ᭄ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ᬯᬮᬶ offset;
	}

	ᬯᬮᬶ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	puur_naa_nka offset;

	ᬬᬾᬦ᭄ ((phandle == 0) || (phandle == ~0U))
		ᬯᬮᬶ -FDT_ERR_BADPHANDLE;

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
		ᬬᬾᬦ᭄ (fdt_get_phandle(fdt, offset) == phandle)
			ᬯᬮᬶ offset;
	}

	ᬯᬮᬶ offset; /* error from fdt_next_node() */
}

puur_naa_nka fdt_stringlist_contains(const var_na *strlist, puur_naa_nka listlen, const var_na *str)
{
	puur_naa_nka len = strlen(str);
	const var_na *p;

	ᬲᭂᬤᭂᬮ (listlen >= len) {
		ᬬᬾᬦ᭄ (memcmp(str, strlist, len+1) == 0)
			ᬯᬮᬶ 1;
		p = memchr(strlist, '\0', listlen);
		ᬬᬾᬦ᭄ (!p)
			ᬯᬮᬶ 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ᬯᬮᬶ 0;
}

puur_naa_nka fdt_stringlist_count(const void *fdt, puur_naa_nka nodeoffset, const var_na *property)
{
	const var_na *list, *end;
	puur_naa_nka length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ᬬᬾᬦ᭄ (!list)
		ᬯᬮᬶ length;

	end = list + length;

	ᬲᭂᬤᭂᬮ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ᬬᬾᬦ᭄ (list + length > end)
			ᬯᬮᬶ -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	ᬯᬮᬶ count;
}

puur_naa_nka fdt_stringlist_search(const void *fdt, puur_naa_nka nodeoffset, const var_na *property,
			  const var_na *string)
{
	puur_naa_nka length, len, idx = 0;
	const var_na *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ᬬᬾᬦ᭄ (!list)
		ᬯᬮᬶ length;

	len = strlen(string) + 1;
	end = list + length;

	ᬲᭂᬤᭂᬮ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ᬬᬾᬦ᭄ (list + length > end)
			ᬯᬮᬶ -FDT_ERR_BADVALUE;

		ᬬᬾᬦ᭄ (length == len && memcmp(list, string, length) == 0)
			ᬯᬮᬶ idx;

		list += length;
		idx++;
	}

	ᬯᬮᬶ -FDT_ERR_NOTFOUND;
}

const var_na *fdt_stringlist_get(const void *fdt, puur_naa_nka nodeoffset,
			       const var_na *property, puur_naa_nka idx,
			       puur_naa_nka *lenp)
{
	const var_na *list, *end;
	puur_naa_nka length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	ᬬᬾᬦ᭄ (!list) {
		ᬬᬾᬦ᭄ (lenp)
			*lenp = length;

		ᬯᬮᬶ NULL;
	}

	end = list + length;

	ᬲᭂᬤᭂᬮ (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		ᬬᬾᬦ᭄ (list + length > end) {
			ᬬᬾᬦ᭄ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ᬯᬮᬶ NULL;
		}

		ᬬᬾᬦ᭄ (idx == 0) {
			ᬬᬾᬦ᭄ (lenp)
				*lenp = length - 1;

			ᬯᬮᬶ list;
		}

		list += length;
		idx--;
	}

	ᬬᬾᬦ᭄ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ᬯᬮᬶ NULL;
}

puur_naa_nka fdt_node_check_compatible(const void *fdt, puur_naa_nka nodeoffset,
			      const var_na *compatible)
{
	const void *prop;
	puur_naa_nka len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	ᬬᬾᬦ᭄ (!prop)
		ᬯᬮᬶ len;

	ᬯᬮᬶ !fdt_stringlist_contains(prop, len, compatible);
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
		ᬬᬾᬦ᭄ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ᬯᬮᬶ err;
		ᬮᬶᬬ ᬬᬾᬦ᭄ (err == 0)
			ᬯᬮᬶ offset;
	}

	ᬯᬮᬶ offset; /* error from fdt_next_node() */
}
