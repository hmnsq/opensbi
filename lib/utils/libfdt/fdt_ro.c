// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

static ကိန်းပြည့် fdt_nodename_eq_(const void *fdt, ကိန်းပြည့် offset,
			    const var_na *s, ကိန်းပြည့် len)
{
	ကိန်းပြည့် olen;
	const var_na *p = fdt_get_name(fdt, offset, &olen);

	အကယ်၍ (!p || olen < len)
		/* short match */
		ပြန်ပေး 0;

	အကယ်၍ (memcmp(p, s, len) != 0)
		ပြန်ပေး 0;

	အကယ်၍ (p[len] == '\0')
		ပြန်ပေး 1;
	မဟုတ်ပါက အကယ်၍ (!memchr(s, '@', len) && (p[len] == '@'))
		ပြန်ပေး 1;
	မဟုတ်ပါက
		ပြန်ပေး 0;
}

const var_na *fdt_get_string(const void *fdt, ကိန်းပြည့် stroffset, ကိန်းပြည့် *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	ကိန်းပြည့် err;
	const var_na *s, *n;

	အကယ်၍ (can_assume(VALID_INPUT)) {
		s = (const var_na *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		အကယ်၍ (lenp)
			*lenp = strlen(s);
		ပြန်ပေး s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	အကယ်၍ (totalsize < 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	အကယ်၍ (absoffset >= (unsigned)totalsize)
		goto fail;
	len = totalsize - absoffset;

	အကယ်၍ (fdt_magic(fdt) == FDT_MAGIC) {
		အကယ်၍ (stroffset < 0)
			goto fail;
		အကယ်၍ (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			အကယ်၍ ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				goto fail;
			အကယ်၍ ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} မဟုတ်ပါက အကယ်၍ (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned ကိန်းပြည့် sw_stroffset = -stroffset;

		အကယ်၍ ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			goto fail;
		အကယ်၍ (sw_stroffset < len)
			len = sw_stroffset;
	} မဟုတ်ပါက {
		err = -FDT_ERR_INTERNAL;
		goto fail;
	}

	s = (const var_na *)fdt + absoffset;
	n = memchr(s, '\0', len);
	အကယ်၍ (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		goto fail;
	}

	အကယ်၍ (lenp)
		*lenp = n - s;
	ပြန်ပေး s;

fail:
	အကယ်၍ (lenp)
		*lenp = err;
	ပြန်ပေး NULL;
}

const var_na *fdt_string(const void *fdt, ကိန်းပြည့် stroffset)
{
	ပြန်ပေး fdt_get_string(fdt, stroffset, NULL);
}

static ကိန်းပြည့် fdt_string_eq_(const void *fdt, ကိန်းပြည့် stroffset,
			  const var_na *s, ကိန်းပြည့် len)
{
	ကိန်းပြည့် slen;
	const var_na *p = fdt_get_string(fdt, stroffset, &slen);

	ပြန်ပေး p && (slen == len) && (memcmp(p, s, len) == 0);
}

ကိန်းပြည့် fdt_find_max_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	ကိန်းပြည့် offset = -1;

	နေစဉ် (true) {
		uint32_t value;

		offset = fdt_next_node(fdt, offset, NULL);
		အကယ်၍ (offset < 0) {
			အကယ်၍ (offset == -FDT_ERR_NOTFOUND)
				bha_nga;

			ပြန်ပေး offset;
		}

		value = fdt_get_phandle(fdt, offset);

		အကယ်၍ (value > max)
			max = value;
	}

	အကယ်၍ (phandle)
		*phandle = max;

	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_generate_phandle(const void *fdt, uint32_t *phandle)
{
	uint32_t max;
	ကိန်းပြည့် err;

	err = fdt_find_max_phandle(fdt, &max);
	အကယ်၍ (err < 0)
		ပြန်ပေး err;

	အကယ်၍ (max == FDT_MAX_PHANDLE)
		ပြန်ပေး -FDT_ERR_NOPHANDLES;

	အကယ်၍ (phandle)
		*phandle = max + 1;

	ပြန်ပေး 0;
}

static const sa_nracanaa fdt_reserve_entry *fdt_mem_rsv(const void *fdt, ကိန်းပြည့် n)
{
	unsigned ကိန်းပြည့် offset = n * parimaa_na(sa_nracanaa fdt_reserve_entry);
	unsigned ကိန်းပြည့် absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	အကယ်၍ (!can_assume(VALID_INPUT)) {
		အကယ်၍ (absoffset < fdt_off_mem_rsvmap(fdt))
			ပြန်ပေး NULL;
		အကယ်၍ (absoffset > fdt_totalsize(fdt) -
		    parimaa_na(sa_nracanaa fdt_reserve_entry))
			ပြန်ပေး NULL;
	}
	ပြန်ပေး fdt_mem_rsv_(fdt, n);
}

ကိန်းပြည့် fdt_get_mem_rsv(const void *fdt, ကိန်းပြည့် n, uint64_t *address, uint64_t *size)
{
	const sa_nracanaa fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	အကယ်၍ (!can_assume(VALID_INPUT) && !re)
		ပြန်ပေး -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_num_mem_rsv(const void *fdt)
{
	ကိန်းပြည့် i;
	const sa_nracanaa fdt_reserve_entry *re;

	အတွက် (i = 0; (re = fdt_mem_rsv(fdt, i)) != NULL; i++) {
		အကယ်၍ (fdt64_ld_(&re->size) == 0)
			ပြန်ပေး i;
	}
	ပြန်ပေး -FDT_ERR_TRUNCATED;
}

static ကိန်းပြည့် nextprop_(const void *fdt, ကိန်းပြည့် offset)
{
	uint32_t tag;
	ကိန်းပြည့် nextoffset;

	do {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		pak_sa FDT_END:
			အကယ်၍ (nextoffset >= 0)
				ပြန်ပေး -FDT_ERR_BADSTRUCTURE;
			မဟုတ်ပါက
				ပြန်ပေး nextoffset;

		pak_sa FDT_PROP:
			ပြန်ပေး offset;
		}
		offset = nextoffset;
	} နေစဉ် (tag == FDT_NOP);

	ပြန်ပေး -FDT_ERR_NOTFOUND;
}

ကိန်းပြည့် fdt_subnode_offset_namelen(const void *fdt, ကိန်းပြည့် offset,
			       const var_na *name, ကိန်းပြည့် namelen)
{
	ကိန်းပြည့် depth;

	FDT_RO_PROBE(fdt);

	အတွက် (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_node(fdt, offset, &depth))
		အကယ်၍ ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			ပြန်ပေး offset;

	အကယ်၍ (depth < 0)
		ပြန်ပေး -FDT_ERR_NOTFOUND;
	ပြန်ပေး offset; /* error */
}

ကိန်းပြည့် fdt_subnode_offset(const void *fdt, ကိန်းပြည့် parentoffset,
		       const var_na *name)
{
	ပြန်ပေး fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

ကိန်းပြည့် fdt_path_offset_namelen(const void *fdt, const var_na *path, ကိန်းပြည့် namelen)
{
	const var_na *end = path + namelen;
	const var_na *p = path;
	ကိန်းပြည့် offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	အကယ်၍ (*path != '/') {
		const var_na *q = memchr(path, '/', end - p);

		အကယ်၍ (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		အကယ်၍ (!p)
			ပြန်ပေး -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	နေစဉ် (p < end) {
		const var_na *q;

		နေစဉ် (*p == '/') {
			p++;
			အကယ်၍ (p == end)
				ပြန်ပေး offset;
		}
		q = memchr(p, '/', end - p);
		အကယ်၍ (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		အကယ်၍ (offset < 0)
			ပြန်ပေး offset;

		p = q;
	}

	ပြန်ပေး offset;
}

ကိန်းပြည့် fdt_path_offset(const void *fdt, const var_na *path)
{
	ပြန်ပေး fdt_path_offset_namelen(fdt, path, strlen(path));
}

const var_na *fdt_get_name(const void *fdt, ကိန်းပြည့် nodeoffset, ကိန်းပြည့် *len)
{
	const sa_nracanaa fdt_node_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	const var_na *nameptr;
	ကိန်းပြည့် err;

	အကယ်၍ (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_node_offset_(fdt, nodeoffset)) < 0))
			goto fail;

	nameptr = nh->name;

	အကယ်၍ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		const var_na *leaf;
		leaf = strrchr(nameptr, '/');
		အကယ်၍ (leaf == NULL) {
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
		nameptr = leaf+1;
	}

	အကယ်၍ (len)
		*len = strlen(nameptr);

	ပြန်ပေး nameptr;

 fail:
	အကယ်၍ (len)
		*len = err;
	ပြန်ပေး NULL;
}

ကိန်းပြည့် fdt_first_property_offset(const void *fdt, ကိန်းပြည့် nodeoffset)
{
	ကိန်းပြည့် offset;

	အကယ်၍ ((offset = fdt_check_node_offset_(fdt, nodeoffset)) < 0)
		ပြန်ပေး offset;

	ပြန်ပေး nextprop_(fdt, offset);
}

ကိန်းပြည့် fdt_next_property_offset(const void *fdt, ကိန်းပြည့် offset)
{
	အကယ်၍ ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		ပြန်ပေး offset;

	ပြန်ပေး nextprop_(fdt, offset);
}

static const sa_nracanaa fdt_property *fdt_get_property_by_offset_(const void *fdt,
						              ကိန်းပြည့် offset,
						              ကိန်းပြည့် *lenp)
{
	ကိန်းပြည့် err;
	const sa_nracanaa fdt_property *prop;

	အကယ်၍ (!can_assume(VALID_INPUT) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		အကယ်၍ (lenp)
			*lenp = err;
		ပြန်ပေး NULL;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	အကယ်၍ (lenp)
		*lenp = fdt32_ld_(&prop->len);

	ပြန်ပေး prop;
}

const sa_nracanaa fdt_property *fdt_get_property_by_offset(const void *fdt,
						      ကိန်းပြည့် offset,
						      ကိန်းပြည့် *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	အကယ်၍ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		အကယ်၍ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ပြန်ပေး NULL;
	}

	ပြန်ပေး fdt_get_property_by_offset_(fdt, offset, lenp);
}

static const sa_nracanaa fdt_property *fdt_get_property_namelen_(const void *fdt,
						            ကိန်းပြည့် offset,
						            const var_na *name,
						            ကိန်းပြည့် namelen,
							    ကိန်းပြည့် *lenp,
							    ကိန်းပြည့် *poffset)
{
	အတွက် (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		const sa_nracanaa fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		အကယ်၍ (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			bha_nga;
		}
		အကယ်၍ (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			အကယ်၍ (poffset)
				*poffset = offset;
			ပြန်ပေး prop;
		}
	}

	အကယ်၍ (lenp)
		*lenp = offset;
	ပြန်ပေး NULL;
}


const sa_nracanaa fdt_property *fdt_get_property_namelen(const void *fdt,
						    ကိန်းပြည့် offset,
						    const var_na *name,
						    ကိန်းပြည့် namelen, ကိန်းပြည့် *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	အကယ်၍ (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		အကယ်၍ (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		ပြန်ပေး NULL;
	}

	ပြန်ပေး fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 NULL);
}


const sa_nracanaa fdt_property *fdt_get_property(const void *fdt,
					    ကိန်းပြည့် nodeoffset,
					    const var_na *name, ကိန်းပြည့် *lenp)
{
	ပြန်ပေး fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

const void *fdt_getprop_namelen(const void *fdt, ကိန်းပြည့် nodeoffset,
				const var_na *name, ကိန်းပြည့် namelen, ကိန်းပြည့် *lenp)
{
	ကိန်းပြည့် poffset;
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	အကယ်၍ (!prop)
		ပြန်ပေး NULL;

	/* Handle realignment */
	အကယ်၍ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ပြန်ပေး prop->data + 4;
	ပြန်ပေး prop->data;
}

const void *fdt_getprop_by_offset(const void *fdt, ကိန်းပြည့် offset,
				  const var_na **namep, ကိန်းပြည့် *lenp)
{
	const sa_nracanaa fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	အကယ်၍ (!prop)
		ပြန်ပေး NULL;
	အကယ်၍ (namep) {
		const var_na *name;
		ကိန်းပြည့် namelen;

		အကယ်၍ (!can_assume(VALID_INPUT)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			အကယ်၍ (!name) {
				အကယ်၍ (lenp)
					*lenp = namelen;
				ပြန်ပေး NULL;
			}
			*namep = name;
		} မဟုတ်ပါက {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	အကယ်၍ (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + parimaa_na(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		ပြန်ပေး prop->data + 4;
	ပြန်ပေး prop->data;
}

const void *fdt_getprop(const void *fdt, ကိန်းပြည့် nodeoffset,
			const var_na *name, ကိန်းပြည့် *lenp)
{
	ပြန်ပေး fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(const void *fdt, ကိန်းပြည့် nodeoffset)
{
	const fdt32_t *php;
	ကိန်းပြည့် len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	အကယ်၍ (!php || (len != parimaa_na(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		အကယ်၍ (!php || (len != parimaa_na(*php)))
			ပြန်ပေး 0;
	}

	ပြန်ပေး fdt32_ld_(php);
}

const var_na *fdt_get_alias_namelen(const void *fdt,
				  const var_na *name, ကိန်းပြည့် namelen)
{
	ကိန်းပြည့် aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	အကယ်၍ (aliasoffset < 0)
		ပြန်ပေး NULL;

	ပြန်ပေး fdt_getprop_namelen(fdt, aliasoffset, name, namelen, NULL);
}

const var_na *fdt_get_alias(const void *fdt, const var_na *name)
{
	ပြန်ပေး fdt_get_alias_namelen(fdt, name, strlen(name));
}

ကိန်းပြည့် fdt_get_path(const void *fdt, ကိန်းပြည့် nodeoffset, var_na *buf, ကိန်းပြည့် buflen)
{
	ကိန်းပြည့် pdepth = 0, p = 0;
	ကိန်းပြည့် offset, depth, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	အကယ်၍ (buflen < 2)
		ပြန်ပေး -FDT_ERR_NOSPACE;

	အတွက် (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		နေစဉ် (pdepth > depth) {
			do {
				p--;
			} နေစဉ် (buf[p-1] != '/');
			pdepth--;
		}

		အကယ်၍ (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			အကယ်၍ (!name)
				ပြန်ပေး namelen;
			အကယ်၍ ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		အကယ်၍ (offset == nodeoffset) {
			အကယ်၍ (pdepth < (depth + 1))
				ပြန်ပေး -FDT_ERR_NOSPACE;

			အကယ်၍ (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			ပြန်ပေး 0;
		}
	}

	အကယ်၍ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		ပြန်ပေး -FDT_ERR_BADOFFSET;
	မဟုတ်ပါက အကယ်၍ (offset == -FDT_ERR_BADOFFSET)
		ပြန်ပေး -FDT_ERR_BADSTRUCTURE;

	ပြန်ပေး offset; /* error from fdt_next_node() */
}

ကိန်းပြည့် fdt_supernode_atdepth_offset(const void *fdt, ကိန်းပြည့် nodeoffset,
				 ကိန်းပြည့် supernodedepth, ကိန်းပြည့် *nodedepth)
{
	ကိန်းပြည့် offset, depth;
	ကိန်းပြည့် supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	အကယ်၍ (supernodedepth < 0)
		ပြန်ပေး -FDT_ERR_NOTFOUND;

	အတွက် (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_node(fdt, offset, &depth)) {
		အကယ်၍ (depth == supernodedepth)
			supernodeoffset = offset;

		အကယ်၍ (offset == nodeoffset) {
			အကယ်၍ (nodedepth)
				*nodedepth = depth;

			အကယ်၍ (supernodedepth > depth)
				ပြန်ပေး -FDT_ERR_NOTFOUND;
			မဟုတ်ပါက
				ပြန်ပေး supernodeoffset;
		}
	}

	အကယ်၍ (!can_assume(VALID_INPUT)) {
		အကယ်၍ ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			ပြန်ပေး -FDT_ERR_BADOFFSET;
		မဟုတ်ပါက အကယ်၍ (offset == -FDT_ERR_BADOFFSET)
			ပြန်ပေး -FDT_ERR_BADSTRUCTURE;
	}

	ပြန်ပေး offset; /* error from fdt_next_node() */
}

ကိန်းပြည့် fdt_node_depth(const void *fdt, ကိန်းပြည့် nodeoffset)
{
	ကိန်းပြည့် nodedepth;
	ကိန်းပြည့် err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	အကယ်၍ (err)
		ပြန်ပေး (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	ပြန်ပေး nodedepth;
}

ကိန်းပြည့် fdt_parent_offset(const void *fdt, ကိန်းပြည့် nodeoffset)
{
	ကိန်းပြည့် nodedepth = fdt_node_depth(fdt, nodeoffset);

	အကယ်၍ (nodedepth < 0)
		ပြန်ပေး nodedepth;
	ပြန်ပေး fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, NULL);
}

ကိန်းပြည့် fdt_node_offset_by_prop_value(const void *fdt, ကိန်းပြည့် startoffset,
				  const var_na *propname,
				  const void *propval, ကိန်းပြည့် proplen)
{
	ကိန်းပြည့် offset;
	const void *val;
	ကိန်းပြည့် len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	အတွက် (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		အကယ်၍ (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			ပြန်ပေး offset;
	}

	ပြန်ပေး offset; /* error from fdt_next_node() */
}

ကိန်းပြည့် fdt_node_offset_by_phandle(const void *fdt, uint32_t phandle)
{
	ကိန်းပြည့် offset;

	အကယ်၍ ((phandle == 0) || (phandle == ~0U))
		ပြန်ပေး -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	အတွက် (offset = fdt_next_node(fdt, -1, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		အကယ်၍ (fdt_get_phandle(fdt, offset) == phandle)
			ပြန်ပေး offset;
	}

	ပြန်ပေး offset; /* error from fdt_next_node() */
}

ကိန်းပြည့် fdt_stringlist_contains(const var_na *strlist, ကိန်းပြည့် listlen, const var_na *str)
{
	ကိန်းပြည့် len = strlen(str);
	const var_na *p;

	နေစဉ် (listlen >= len) {
		အကယ်၍ (memcmp(str, strlist, len+1) == 0)
			ပြန်ပေး 1;
		p = memchr(strlist, '\0', listlen);
		အကယ်၍ (!p)
			ပြန်ပေး 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	ပြန်ပေး 0;
}

ကိန်းပြည့် fdt_stringlist_count(const void *fdt, ကိန်းပြည့် nodeoffset, const var_na *property)
{
	const var_na *list, *end;
	ကိန်းပြည့် length, count = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	အကယ်၍ (!list)
		ပြန်ပေး length;

	end = list + length;

	နေစဉ် (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		အကယ်၍ (list + length > end)
			ပြန်ပေး -FDT_ERR_BADVALUE;

		list += length;
		count++;
	}

	ပြန်ပေး count;
}

ကိန်းပြည့် fdt_stringlist_search(const void *fdt, ကိန်းပြည့် nodeoffset, const var_na *property,
			  const var_na *string)
{
	ကိန်းပြည့် length, len, idx = 0;
	const var_na *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	အကယ်၍ (!list)
		ပြန်ပေး length;

	len = strlen(string) + 1;
	end = list + length;

	နေစဉ် (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		အကယ်၍ (list + length > end)
			ပြန်ပေး -FDT_ERR_BADVALUE;

		အကယ်၍ (length == len && memcmp(list, string, length) == 0)
			ပြန်ပေး idx;

		list += length;
		idx++;
	}

	ပြန်ပေး -FDT_ERR_NOTFOUND;
}

const var_na *fdt_stringlist_get(const void *fdt, ကိန်းပြည့် nodeoffset,
			       const var_na *property, ကိန်းပြည့် idx,
			       ကိန်းပြည့် *lenp)
{
	const var_na *list, *end;
	ကိန်းပြည့် length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	အကယ်၍ (!list) {
		အကယ်၍ (lenp)
			*lenp = length;

		ပြန်ပေး NULL;
	}

	end = list + length;

	နေစဉ် (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		အကယ်၍ (list + length > end) {
			အကယ်၍ (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			ပြန်ပေး NULL;
		}

		အကယ်၍ (idx == 0) {
			အကယ်၍ (lenp)
				*lenp = length - 1;

			ပြန်ပေး list;
		}

		list += length;
		idx--;
	}

	အကယ်၍ (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	ပြန်ပေး NULL;
}

ကိန်းပြည့် fdt_node_check_compatible(const void *fdt, ကိန်းပြည့် nodeoffset,
			      const var_na *compatible)
{
	const void *prop;
	ကိန်းပြည့် len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	အကယ်၍ (!prop)
		ပြန်ပေး len;

	ပြန်ပေး !fdt_stringlist_contains(prop, len, compatible);
}

ကိန်းပြည့် fdt_node_offset_by_compatible(const void *fdt, ကိန်းပြည့် startoffset,
				  const var_na *compatible)
{
	ကိန်းပြည့် offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	အတွက် (offset = fdt_next_node(fdt, startoffset, NULL);
	     offset >= 0;
	     offset = fdt_next_node(fdt, offset, NULL)) {
		err = fdt_node_check_compatible(fdt, offset, compatible);
		အကယ်၍ ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			ပြန်ပေး err;
		မဟုတ်ပါက အကယ်၍ (err == 0)
			ပြန်ပေး offset;
	}

	ပြန်ပေး offset; /* error from fdt_next_node() */
}
