// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 */
#અન્તર્ભાવ "libfdt_env.h"

#અન્તર્ભાવ <fdt.h>
#અન્તર્ભાવ <libfdt.h>

#અન્તર્ભાવ "libfdt_internal.h"

સ્થિર પૂર્ણાંક fdt_nodename_eq_(અચલ શૂન્ય *fdt, પૂર્ણાંક offset,
			    અચલ અક્ષર *s, પૂર્ણાંક len)
{
	પૂર્ણાંક olen;
	અચલ અક્ષર *p = fdt_get_name(fdt, offset, &olen);

	જો (!p || olen < len)
		/* short match */
		પાછા 0;

	જો (memcmp(p, s, len) != 0)
		પાછા 0;

	જો (p[len] == '\0')
		પાછા 1;
	નહીંતર જો (!memchr(s, '@', len) && (p[len] == '@'))
		પાછા 1;
	નહીંતર
		પાછા 0;
}

અચલ અક્ષર *fdt_get_string(અચલ શૂન્ય *fdt, પૂર્ણાંક stroffset, પૂર્ણાંક *lenp)
{
	int32_t totalsize;
	uint32_t absoffset;
	size_t len;
	પૂર્ણાંક err;
	અચલ અક્ષર *s, *n;

	જો (can_assume(VALID_આગત)) {
		s = (અચલ અક્ષર *)fdt + fdt_off_dt_strings(fdt) + stroffset;

		જો (lenp)
			*lenp = strlen(s);
		પાછા s;
	}
	totalsize = fdt_ro_probe_(fdt);
	err = totalsize;
	જો (totalsize < 0)
		જાઓ fail;

	err = -FDT_ERR_BADOFFSET;
	absoffset = stroffset + fdt_off_dt_strings(fdt);
	જો (absoffset >= (unsigned)totalsize)
		જાઓ fail;
	len = totalsize - absoffset;

	જો (fdt_magic(fdt) == FDT_MAGIC) {
		જો (stroffset < 0)
			જાઓ fail;
		જો (can_assume(LATEST) || fdt_version(fdt) >= 17) {
			જો ((unsigned)stroffset >= fdt_size_dt_strings(fdt))
				જાઓ fail;
			જો ((fdt_size_dt_strings(fdt) - stroffset) < len)
				len = fdt_size_dt_strings(fdt) - stroffset;
		}
	} નહીંતર જો (fdt_magic(fdt) == FDT_SW_MAGIC) {
		unsigned પૂર્ણાંક sw_stroffset = -stroffset;

		જો ((stroffset >= 0) ||
		    (sw_stroffset > fdt_size_dt_strings(fdt)))
			જાઓ fail;
		જો (sw_stroffset < len)
			len = sw_stroffset;
	} નહીંતર {
		err = -FDT_ERR_INTERNAL;
		જાઓ fail;
	}

	s = (અચલ અક્ષર *)fdt + absoffset;
	n = memchr(s, '\0', len);
	જો (!n) {
		/* missing terminating NULL */
		err = -FDT_ERR_TRUNCATED;
		જાઓ fail;
	}

	જો (lenp)
		*lenp = n - s;
	પાછા s;

fail:
	જો (lenp)
		*lenp = err;
	પાછા ખાલી;
}

અચલ અક્ષર *fdt_string(અચલ શૂન્ય *fdt, પૂર્ણાંક stroffset)
{
	પાછા fdt_get_string(fdt, stroffset, ખાલી);
}

સ્થિર પૂર્ણાંક fdt_string_eq_(અચલ શૂન્ય *fdt, પૂર્ણાંક stroffset,
			  અચલ અક્ષર *s, પૂર્ણાંક len)
{
	પૂર્ણાંક slen;
	અચલ અક્ષર *p = fdt_get_string(fdt, stroffset, &slen);

	પાછા p && (slen == len) && (memcmp(p, s, len) == 0);
}

પૂર્ણાંક fdt_find_max_phandle(અચલ શૂન્ય *fdt, uint32_t *phandle)
{
	uint32_t max = 0;
	પૂર્ણાંક offset = -1;

	જ્યારે (true) {
		uint32_t કિંમત;

		offset = fdt_next_ગાંઠ(fdt, offset, ખાલી);
		જો (offset < 0) {
			જો (offset == -FDT_ERR_NOTFOUND)
				તોડો;

			પાછા offset;
		}

		કિંમત = fdt_get_phandle(fdt, offset);

		જો (કિંમત > max)
			max = કિંમત;
	}

	જો (phandle)
		*phandle = max;

	પાછા 0;
}

પૂર્ણાંક fdt_generate_phandle(અચલ શૂન્ય *fdt, uint32_t *phandle)
{
	uint32_t max;
	પૂર્ણાંક err;

	err = fdt_find_max_phandle(fdt, &max);
	જો (err < 0)
		પાછા err;

	જો (max == FDT_MAX_PHANDLE)
		પાછા -FDT_ERR_NOPHANDLES;

	જો (phandle)
		*phandle = max + 1;

	પાછા 0;
}

સ્થિર અચલ સંરચના fdt_reserve_entry *fdt_mem_rsv(અચલ શૂન્ય *fdt, પૂર્ણાંક n)
{
	unsigned પૂર્ણાંક offset = n * કદ(સંરચના fdt_reserve_entry);
	unsigned પૂર્ણાંક absoffset = fdt_off_mem_rsvmap(fdt) + offset;

	જો (!can_assume(VALID_આગત)) {
		જો (absoffset < fdt_off_mem_rsvmap(fdt))
			પાછા ખાલી;
		જો (absoffset > fdt_totalsize(fdt) -
		    કદ(સંરચના fdt_reserve_entry))
			પાછા ખાલી;
	}
	પાછા fdt_mem_rsv_(fdt, n);
}

પૂર્ણાંક fdt_get_mem_rsv(અચલ શૂન્ય *fdt, પૂર્ણાંક n, uint64_t *address, uint64_t *size)
{
	અચલ સંરચના fdt_reserve_entry *re;

	FDT_RO_PROBE(fdt);
	re = fdt_mem_rsv(fdt, n);
	જો (!can_assume(VALID_આગત) && !re)
		પાછા -FDT_ERR_BADOFFSET;

	*address = fdt64_ld_(&re->address);
	*size = fdt64_ld_(&re->size);
	પાછા 0;
}

પૂર્ણાંક fdt_અંક_mem_rsv(અચલ શૂન્ય *fdt)
{
	પૂર્ણાંક i;
	અચલ સંરચના fdt_reserve_entry *re;

	માટે (i = 0; (re = fdt_mem_rsv(fdt, i)) != ખાલી; i++) {
		જો (fdt64_ld_(&re->size) == 0)
			પાછા i;
	}
	પાછા -FDT_ERR_TRUNCATED;
}

સ્થિર પૂર્ણાંક nextprop_(અચલ શૂન્ય *fdt, પૂર્ણાંક offset)
{
	uint32_t tag;
	પૂર્ણાંક nextoffset;

	કરો {
		tag = fdt_next_tag(fdt, offset, &nextoffset);

		પસંદગી (tag) {
		સ્થિતિ FDT_END:
			જો (nextoffset >= 0)
				પાછા -FDT_ERR_BADSTRUCTURE;
			નહીંતર
				પાછા nextoffset;

		સ્થિતિ FDT_PROP:
			પાછા offset;
		}
		offset = nextoffset;
	} જ્યારે (tag == FDT_NOP);

	પાછા -FDT_ERR_NOTFOUND;
}

પૂર્ણાંક fdt_subnode_offset_namelen(અચલ શૂન્ય *fdt, પૂર્ણાંક offset,
			       અચલ અક્ષર *name, પૂર્ણાંક namelen)
{
	પૂર્ણાંક depth;

	FDT_RO_PROBE(fdt);

	માટે (depth = 0;
	     (offset >= 0) && (depth >= 0);
	     offset = fdt_next_ગાંઠ(fdt, offset, &depth))
		જો ((depth == 1)
		    && fdt_nodename_eq_(fdt, offset, name, namelen))
			પાછા offset;

	જો (depth < 0)
		પાછા -FDT_ERR_NOTFOUND;
	પાછા offset; /* error */
}

પૂર્ણાંક fdt_subnode_offset(અચલ શૂન્ય *fdt, પૂર્ણાંક parentoffset,
		       અચલ અક્ષર *name)
{
	પાછા fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

પૂર્ણાંક fdt_path_offset_namelen(અચલ શૂન્ય *fdt, અચલ અક્ષર *path, પૂર્ણાંક namelen)
{
	અચલ અક્ષર *end = path + namelen;
	અચલ અક્ષર *p = path;
	પૂર્ણાંક offset = 0;

	FDT_RO_PROBE(fdt);

	/* see if we have an alias */
	જો (*path != '/') {
		અચલ અક્ષર *q = memchr(path, '/', end - p);

		જો (!q)
			q = end;

		p = fdt_get_alias_namelen(fdt, p, q - p);
		જો (!p)
			પાછા -FDT_ERR_BADPATH;
		offset = fdt_path_offset(fdt, p);

		p = q;
	}

	જ્યારે (p < end) {
		અચલ અક્ષર *q;

		જ્યારે (*p == '/') {
			p++;
			જો (p == end)
				પાછા offset;
		}
		q = memchr(p, '/', end - p);
		જો (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		જો (offset < 0)
			પાછા offset;

		p = q;
	}

	પાછા offset;
}

પૂર્ણાંક fdt_path_offset(અચલ શૂન્ય *fdt, અચલ અક્ષર *path)
{
	પાછા fdt_path_offset_namelen(fdt, path, strlen(path));
}

અચલ અક્ષર *fdt_get_name(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset, પૂર્ણાંક *len)
{
	અચલ સંરચના fdt_ગાંઠ_header *nh = fdt_offset_ptr_(fdt, nodeoffset);
	અચલ અક્ષર *nameptr;
	પૂર્ણાંક err;

	જો (((err = fdt_ro_probe_(fdt)) < 0)
	    || ((err = fdt_check_ગાંઠ_offset(fdt, nodeoffset)) < 0))
			જાઓ fail;

	nameptr = nh->name;

	જો (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		/*
		 * For old FDT versions, match the naming conventions of V16:
		 * give only the leaf name (after all /). The actual tree
		 * contents are loosely checked.
		 */
		અચલ અક્ષર *leaf;
		leaf = strrchr(nameptr, '/');
		જો (leaf == ખાલી) {
			err = -FDT_ERR_BADSTRUCTURE;
			જાઓ fail;
		}
		nameptr = leaf+1;
	}

	જો (len)
		*len = strlen(nameptr);

	પાછા nameptr;

 fail:
	જો (len)
		*len = err;
	પાછા ખાલી;
}

પૂર્ણાંક fdt_first_property_offset(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset)
{
	પૂર્ણાંક offset;

	જો ((offset = fdt_check_ગાંઠ_offset(fdt, nodeoffset)) < 0)
		પાછા offset;

	પાછા nextprop_(fdt, offset);
}

પૂર્ણાંક fdt_next_property_offset(અચલ શૂન્ય *fdt, પૂર્ણાંક offset)
{
	જો ((offset = fdt_check_prop_offset_(fdt, offset)) < 0)
		પાછા offset;

	પાછા nextprop_(fdt, offset);
}

સ્થિર અચલ સંરચના fdt_property *fdt_get_property_by_offset_(અચલ શૂન્ય *fdt,
						              પૂર્ણાંક offset,
						              પૂર્ણાંક *lenp)
{
	પૂર્ણાંક err;
	અચલ સંરચના fdt_property *prop;

	જો (!can_assume(VALID_આગત) &&
	    (err = fdt_check_prop_offset_(fdt, offset)) < 0) {
		જો (lenp)
			*lenp = err;
		પાછા ખાલી;
	}

	prop = fdt_offset_ptr_(fdt, offset);

	જો (lenp)
		*lenp = fdt32_ld_(&prop->len);

	પાછા prop;
}

અચલ સંરચના fdt_property *fdt_get_property_by_offset(અચલ શૂન્ય *fdt,
						      પૂર્ણાંક offset,
						      પૂર્ણાંક *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */

	જો (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		જો (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		પાછા ખાલી;
	}

	પાછા fdt_get_property_by_offset_(fdt, offset, lenp);
}

સ્થિર અચલ સંરચના fdt_property *fdt_get_property_namelen_(અચલ શૂન્ય *fdt,
						            પૂર્ણાંક offset,
						            અચલ અક્ષર *name,
						            પૂર્ણાંક namelen,
							    પૂર્ણાંક *lenp,
							    પૂર્ણાંક *poffset)
{
	માટે (offset = fdt_first_property_offset(fdt, offset);
	     (offset >= 0);
	     (offset = fdt_next_property_offset(fdt, offset))) {
		અચલ સંરચના fdt_property *prop;

		prop = fdt_get_property_by_offset_(fdt, offset, lenp);
		જો (!can_assume(LIBFDT_FLAWLESS) && !prop) {
			offset = -FDT_ERR_INTERNAL;
			તોડો;
		}
		જો (fdt_string_eq_(fdt, fdt32_ld_(&prop->nameoff),
				   name, namelen)) {
			જો (poffset)
				*poffset = offset;
			પાછા prop;
		}
	}

	જો (lenp)
		*lenp = offset;
	પાછા ખાલી;
}


અચલ સંરચના fdt_property *fdt_get_property_namelen(અચલ શૂન્ય *fdt,
						    પૂર્ણાંક offset,
						    અચલ અક્ષર *name,
						    પૂર્ણાંક namelen, પૂર્ણાંક *lenp)
{
	/* Prior to version 16, properties may need realignment
	 * and this API does not work. fdt_getprop_*() will, however. */
	જો (!can_assume(LATEST) && fdt_version(fdt) < 0x10) {
		જો (lenp)
			*lenp = -FDT_ERR_BADVERSION;
		પાછા ખાલી;
	}

	પાછા fdt_get_property_namelen_(fdt, offset, name, namelen, lenp,
					 ખાલી);
}


અચલ સંરચના fdt_property *fdt_get_property(અચલ શૂન્ય *fdt,
					    પૂર્ણાંક nodeoffset,
					    અચલ અક્ષર *name, પૂર્ણાંક *lenp)
{
	પાછા fdt_get_property_namelen(fdt, nodeoffset, name,
					strlen(name), lenp);
}

અચલ શૂન્ય *fdt_getprop_namelen(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
				અચલ અક્ષર *name, પૂર્ણાંક namelen, પૂર્ણાંક *lenp)
{
	પૂર્ણાંક poffset;
	અચલ સંરચના fdt_property *prop;

	prop = fdt_get_property_namelen_(fdt, nodeoffset, name, namelen, lenp,
					 &poffset);
	જો (!prop)
		પાછા ખાલી;

	/* Handle realignment */
	જો (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (poffset + કદ(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		પાછા prop->data + 4;
	પાછા prop->data;
}

અચલ શૂન્ય *fdt_getprop_by_offset(અચલ શૂન્ય *fdt, પૂર્ણાંક offset,
				  અચલ અક્ષર **namep, પૂર્ણાંક *lenp)
{
	અચલ સંરચના fdt_property *prop;

	prop = fdt_get_property_by_offset_(fdt, offset, lenp);
	જો (!prop)
		પાછા ખાલી;
	જો (namep) {
		અચલ અક્ષર *name;
		પૂર્ણાંક namelen;

		જો (!can_assume(VALID_આગત)) {
			name = fdt_get_string(fdt, fdt32_ld_(&prop->nameoff),
					      &namelen);
			જો (!name) {
				જો (lenp)
					*lenp = namelen;
				પાછા ખાલી;
			}
			*namep = name;
		} નહીંતર {
			*namep = fdt_string(fdt, fdt32_ld_(&prop->nameoff));
		}
	}

	/* Handle realignment */
	જો (!can_assume(LATEST) && fdt_version(fdt) < 0x10 &&
	    (offset + કદ(*prop)) % 8 && fdt32_ld_(&prop->len) >= 8)
		પાછા prop->data + 4;
	પાછા prop->data;
}

અચલ શૂન્ય *fdt_getprop(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
			અચલ અક્ષર *name, પૂર્ણાંક *lenp)
{
	પાછા fdt_getprop_namelen(fdt, nodeoffset, name, strlen(name), lenp);
}

uint32_t fdt_get_phandle(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset)
{
	અચલ fdt32_t *php;
	પૂર્ણાંક len;

	/* FIXME: This is a bit sub-optimal, since we potentially scan
	 * over all the properties twice. */
	php = fdt_getprop(fdt, nodeoffset, "phandle", &len);
	જો (!php || (len != કદ(*php))) {
		php = fdt_getprop(fdt, nodeoffset, "linux,phandle", &len);
		જો (!php || (len != કદ(*php)))
			પાછા 0;
	}

	પાછા fdt32_ld_(php);
}

અચલ અક્ષર *fdt_get_alias_namelen(અચલ શૂન્ય *fdt,
				  અચલ અક્ષર *name, પૂર્ણાંક namelen)
{
	પૂર્ણાંક aliasoffset;

	aliasoffset = fdt_path_offset(fdt, "/aliases");
	જો (aliasoffset < 0)
		પાછા ખાલી;

	પાછા fdt_getprop_namelen(fdt, aliasoffset, name, namelen, ખાલી);
}

અચલ અક્ષર *fdt_get_alias(અચલ શૂન્ય *fdt, અચલ અક્ષર *name)
{
	પાછા fdt_get_alias_namelen(fdt, name, strlen(name));
}

પૂર્ણાંક fdt_get_path(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset, અક્ષર *buf, પૂર્ણાંક buflen)
{
	પૂર્ણાંક pdepth = 0, p = 0;
	પૂર્ણાંક offset, depth, namelen;
	અચલ અક્ષર *name;

	FDT_RO_PROBE(fdt);

	જો (buflen < 2)
		પાછા -FDT_ERR_NOSPACE;

	માટે (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_ગાંઠ(fdt, offset, &depth)) {
		જ્યારે (pdepth > depth) {
			કરો {
				p--;
			} જ્યારે (buf[p-1] != '/');
			pdepth--;
		}

		જો (pdepth >= depth) {
			name = fdt_get_name(fdt, offset, &namelen);
			જો (!name)
				પાછા namelen;
			જો ((p + namelen + 1) <= buflen) {
				memcpy(buf + p, name, namelen);
				p += namelen;
				buf[p++] = '/';
				pdepth++;
			}
		}

		જો (offset == nodeoffset) {
			જો (pdepth < (depth + 1))
				પાછા -FDT_ERR_NOSPACE;

			જો (p > 1) /* special case so that root path is "/", not "" */
				p--;
			buf[p] = '\0';
			પાછા 0;
		}
	}

	જો ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
		પાછા -FDT_ERR_BADOFFSET;
	નહીંતર જો (offset == -FDT_ERR_BADOFFSET)
		પાછા -FDT_ERR_BADSTRUCTURE;

	પાછા offset; /* error from fdt_next_node() */
}

પૂર્ણાંક fdt_supernode_atdepth_offset(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
				 પૂર્ણાંક supernodedepth, પૂર્ણાંક *nodedepth)
{
	પૂર્ણાંક offset, depth;
	પૂર્ણાંક supernodeoffset = -FDT_ERR_INTERNAL;

	FDT_RO_PROBE(fdt);

	જો (supernodedepth < 0)
		પાછા -FDT_ERR_NOTFOUND;

	માટે (offset = 0, depth = 0;
	     (offset >= 0) && (offset <= nodeoffset);
	     offset = fdt_next_ગાંઠ(fdt, offset, &depth)) {
		જો (depth == supernodedepth)
			supernodeoffset = offset;

		જો (offset == nodeoffset) {
			જો (nodedepth)
				*nodedepth = depth;

			જો (supernodedepth > depth)
				પાછા -FDT_ERR_NOTFOUND;
			નહીંતર
				પાછા supernodeoffset;
		}
	}

	જો (!can_assume(VALID_આગત)) {
		જો ((offset == -FDT_ERR_NOTFOUND) || (offset >= 0))
			પાછા -FDT_ERR_BADOFFSET;
		નહીંતર જો (offset == -FDT_ERR_BADOFFSET)
			પાછા -FDT_ERR_BADSTRUCTURE;
	}

	પાછા offset; /* error from fdt_next_node() */
}

પૂર્ણાંક fdt_ગાંઠ_depth(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset)
{
	પૂર્ણાંક nodedepth;
	પૂર્ણાંક err;

	err = fdt_supernode_atdepth_offset(fdt, nodeoffset, 0, &nodedepth);
	જો (err)
		પાછા (can_assume(LIBFDT_FLAWLESS) || err < 0) ? err :
			-FDT_ERR_INTERNAL;
	પાછા nodedepth;
}

પૂર્ણાંક fdt_parent_offset(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset)
{
	પૂર્ણાંક nodedepth = fdt_ગાંઠ_depth(fdt, nodeoffset);

	જો (nodedepth < 0)
		પાછા nodedepth;
	પાછા fdt_supernode_atdepth_offset(fdt, nodeoffset,
					    nodedepth - 1, ખાલી);
}

પૂર્ણાંક fdt_ગાંઠ_offset_by_prop_કિંમત(અચલ શૂન્ય *fdt, પૂર્ણાંક startoffset,
				  અચલ અક્ષર *propname,
				  અચલ શૂન્ય *propval, પૂર્ણાંક proplen)
{
	પૂર્ણાંક offset;
	અચલ શૂન્ય *val;
	પૂર્ણાંક len;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_getprop(), then if that didn't
	 * find what we want, we scan over them again making our way
	 * to the next node.  Still it's the easiest to implement
	 * approach; performance can come later. */
	માટે (offset = fdt_next_ગાંઠ(fdt, startoffset, ખાલી);
	     offset >= 0;
	     offset = fdt_next_ગાંઠ(fdt, offset, ખાલી)) {
		val = fdt_getprop(fdt, offset, propname, &len);
		જો (val && (len == proplen)
		    && (memcmp(val, propval, len) == 0))
			પાછા offset;
	}

	પાછા offset; /* error from fdt_next_node() */
}

પૂર્ણાંક fdt_ગાંઠ_offset_by_phandle(અચલ શૂન્ય *fdt, uint32_t phandle)
{
	પૂર્ણાંક offset;

	જો ((phandle == 0) || (phandle == ~0U))
		પાછા -FDT_ERR_BADPHANDLE;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we
	 * potentially scan each property of a node in
	 * fdt_get_phandle(), then if that didn't find what
	 * we want, we scan over them again making our way to the next
	 * node.  Still it's the easiest to implement approach;
	 * performance can come later. */
	માટે (offset = fdt_next_ગાંઠ(fdt, -1, ખાલી);
	     offset >= 0;
	     offset = fdt_next_ગાંઠ(fdt, offset, ખાલી)) {
		જો (fdt_get_phandle(fdt, offset) == phandle)
			પાછા offset;
	}

	પાછા offset; /* error from fdt_next_node() */
}

પૂર્ણાંક fdt_stringlist_contains(અચલ અક્ષર *strlist, પૂર્ણાંક listlen, અચલ અક્ષર *str)
{
	પૂર્ણાંક len = strlen(str);
	અચલ અક્ષર *p;

	જ્યારે (listlen >= len) {
		જો (memcmp(str, strlist, len+1) == 0)
			પાછા 1;
		p = memchr(strlist, '\0', listlen);
		જો (!p)
			પાછા 0; /* malformed strlist.. */
		listlen -= (p-strlist) + 1;
		strlist = p + 1;
	}
	પાછા 0;
}

પૂર્ણાંક fdt_stringlist_ગણતરી(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset, અચલ અક્ષર *property)
{
	અચલ અક્ષર *list, *end;
	પૂર્ણાંક length, ગણતરી = 0;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	જો (!list)
		પાછા length;

	end = list + length;

	જ્યારે (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		જો (list + length > end)
			પાછા -FDT_ERR_BADVALUE;

		list += length;
		ગણતરી++;
	}

	પાછા ગણતરી;
}

પૂર્ણાંક fdt_stringlist_શોધ(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset, અચલ અક્ષર *property,
			  અચલ અક્ષર *string)
{
	પૂર્ણાંક length, len, idx = 0;
	અચલ અક્ષર *list, *end;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	જો (!list)
		પાછા length;

	len = strlen(string) + 1;
	end = list + length;

	જ્યારે (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		જો (list + length > end)
			પાછા -FDT_ERR_BADVALUE;

		જો (length == len && memcmp(list, string, length) == 0)
			પાછા idx;

		list += length;
		idx++;
	}

	પાછા -FDT_ERR_NOTFOUND;
}

અચલ અક્ષર *fdt_stringlist_get(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
			       અચલ અક્ષર *property, પૂર્ણાંક idx,
			       પૂર્ણાંક *lenp)
{
	અચલ અક્ષર *list, *end;
	પૂર્ણાંક length;

	list = fdt_getprop(fdt, nodeoffset, property, &length);
	જો (!list) {
		જો (lenp)
			*lenp = length;

		પાછા ખાલી;
	}

	end = list + length;

	જ્યારે (list < end) {
		length = strnlen(list, end - list) + 1;

		/* Abort if the last string isn't properly NUL-terminated. */
		જો (list + length > end) {
			જો (lenp)
				*lenp = -FDT_ERR_BADVALUE;

			પાછા ખાલી;
		}

		જો (idx == 0) {
			જો (lenp)
				*lenp = length - 1;

			પાછા list;
		}

		list += length;
		idx--;
	}

	જો (lenp)
		*lenp = -FDT_ERR_NOTFOUND;

	પાછા ખાલી;
}

પૂર્ણાંક fdt_ગાંઠ_check_compatible(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset,
			      અચલ અક્ષર *compatible)
{
	અચલ શૂન્ય *prop;
	પૂર્ણાંક len;

	prop = fdt_getprop(fdt, nodeoffset, "compatible", &len);
	જો (!prop)
		પાછા len;

	પાછા !fdt_stringlist_contains(prop, len, compatible);
}

પૂર્ણાંક fdt_ગાંઠ_offset_by_compatible(અચલ શૂન્ય *fdt, પૂર્ણાંક startoffset,
				  અચલ અક્ષર *compatible)
{
	પૂર્ણાંક offset, err;

	FDT_RO_PROBE(fdt);

	/* FIXME: The algorithm here is pretty horrible: we scan each
	 * property of a node in fdt_node_check_compatible(), then if
	 * that didn't find what we want, we scan over them again
	 * making our way to the next node.  Still it's the easiest to
	 * implement approach; performance can come later. */
	માટે (offset = fdt_next_ગાંઠ(fdt, startoffset, ખાલી);
	     offset >= 0;
	     offset = fdt_next_ગાંઠ(fdt, offset, ખાલી)) {
		err = fdt_ગાંઠ_check_compatible(fdt, offset, compatible);
		જો ((err < 0) && (err != -FDT_ERR_NOTFOUND))
			પાછા err;
		નહીંતર જો (err == 0)
			પાછા offset;
	}

	પાછા offset; /* error from fdt_next_node() */
}
