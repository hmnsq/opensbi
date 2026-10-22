// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#شامل "libfdt_env.h"

#شامل <fdt.h>
#شامل <libfdt.h>

#شامل "libfdt_internal.h"

/**
 * overlay_get_target_phandle - retrieves the target phandle of a fragment
 * @fdto: pointer to the device tree overlay blob
 * @fragment: node offset of the fragment in the overlay
 *
 * overlay_get_target_phandle() retrieves the target phandle of an
 * overlay fragment when that fragment uses a phandle (target
 * property) instead of a path (target-path property).
 *
 * returns:
 *      the phandle pointed by the target property
 *      0, if the phandle was not found
 *	-1, if the phandle was malformed
 */
ایستا uint32_t overlay_get_target_phandle(ثابت تهی *fdto, صحیح fragment)
{
	ثابت fdt32_t *val;
	صحیح len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	اگر (!val)
		بازگشت 0;

	اگر ((len != اندازه(*val)) || (fdt32_to_cpu(*val) == (uint32_t)-1))
		بازگشت (uint32_t)-1;

	بازگشت fdt32_to_cpu(*val);
}

/**
 * overlay_get_target - retrieves the offset of a fragment's target
 * @fdt: Base device tree blob
 * @fdto: Device tree overlay blob
 * @fragment: node offset of the fragment in the overlay
 * @pathp: pointer which receives the path of the target (or NULL)
 *
 * overlay_get_target() retrieves the target offset in the base
 * device tree of a fragment, no matter how the actual targeting is
 * done (through a phandle or a path)
 *
 * returns:
 *      the targeted node offset in the base device tree
 *      Negative error code on error
 */
ایستا صحیح overlay_get_target(ثابت تهی *fdt, ثابت تهی *fdto,
			      صحیح fragment, نویسه ثابت **pathp)
{
	uint32_t phandle;
	ثابت نویسه *path = پوچ;
	صحیح path_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_target_phandle(fdto, fragment);
	اگر (phandle == (uint32_t)-1)
		بازگشت -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	اگر (!phandle) {
		/* And then a path based lookup */
		path = fdt_getprop(fdto, fragment, "target-path", &path_len);
		اگر (path)
			ret = fdt_path_offset(fdt, path);
		وگرنه
			ret = path_len;
	} وگرنه
		ret = fdt_گره_offset_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	اگر (ret < 0 && path_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	اگر (ret < 0)
		بازگشت ret;

	/* return pointer to path (if available) */
	اگر (pathp)
		*pathp = path ? path : پوچ;

	بازگشت ret;
}

/**
 * overlay_phandle_add_offset - Increases a phandle by an offset
 * @fdt: Base device tree blob
 * @node: Device tree overlay blob
 * @name: Name of the property to modify (phandle or linux,phandle)
 * @delta: offset to apply
 *
 * overlay_phandle_add_offset() increments a node phandle by a given
 * offset.
 *
 * returns:
 *      0 on success.
 *      Negative error code on error
 */
ایستا صحیح overlay_phandle_add_offset(تهی *fdt, صحیح گره,
				      ثابت نویسه *name, uint32_t delta)
{
	ثابت fdt32_t *val;
	uint32_t adj_val;
	صحیح len;

	val = fdt_getprop(fdt, گره, name, &len);
	اگر (!val)
		بازگشت len;

	اگر (len != اندازه(*val))
		بازگشت -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_to_cpu(*val);
	اگر ((adj_val + delta) < adj_val)
		بازگشت -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	اگر (adj_val == (uint32_t)-1)
		بازگشت -FDT_ERR_NOPHANDLES;

	بازگشت fdt_setprop_inplace_u32(fdt, گره, name, adj_val);
}

/**
 * overlay_adjust_node_phandles - Offsets the phandles of a node
 * @fdto: Device tree overlay blob
 * @node: Offset of the node we want to adjust
 * @delta: Offset to shift the phandles of
 *
 * overlay_adjust_node_phandles() adds a constant to all the phandles
 * of a given node. This is mainly use as part of the overlay
 * application process, when we want to update all the overlay
 * phandles to not conflict with the overlays of the base device tree.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ایستا صحیح overlay_adjust_گره_phandles(تهی *fdto, صحیح گره,
					uint32_t delta)
{
	صحیح child;
	صحیح ret;

	ret = overlay_phandle_add_offset(fdto, گره, "phandle", delta);
	اگر (ret && ret != -FDT_ERR_NOTFOUND)
		بازگشت ret;

	ret = overlay_phandle_add_offset(fdto, گره, "linux,phandle", delta);
	اگر (ret && ret != -FDT_ERR_NOTFOUND)
		بازگشت ret;

	fdt_for_each_subnode(child, fdto, گره) {
		ret = overlay_adjust_گره_phandles(fdto, child, delta);
		اگر (ret)
			بازگشت ret;
	}

	بازگشت 0;
}

/**
 * overlay_adjust_local_phandles - Adjust the phandles of a whole overlay
 * @fdto: Device tree overlay blob
 * @delta: Offset to shift the phandles of
 *
 * overlay_adjust_local_phandles() adds a constant to all the
 * phandles of an overlay. This is mainly use as part of the overlay
 * application process, when we want to update all the overlay
 * phandles to not conflict with the overlays of the base device tree.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ایستا صحیح overlay_adjust_local_phandles(تهی *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	بازگشت overlay_adjust_گره_phandles(fdto, 0, delta);
}

/**
 * overlay_update_local_node_references - Adjust the overlay references
 * @fdto: Device tree overlay blob
 * @tree_node: Node offset of the node to operate on
 * @fixup_node: Node offset of the matching local fixups node
 * @delta: Offset to shift the phandles of
 *
 * overlay_update_local_nodes_references() update the phandles
 * pointing to a node within the device tree overlay by adding a
 * constant delta.
 *
 * This is mainly used as part of a device tree application process,
 * where you want the device tree overlays phandles to not conflict
 * with the ones from the base device tree before merging them.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ایستا صحیح overlay_update_local_گره_references(تهی *fdto,
						صحیح گره_درخت,
						صحیح fixup_گره,
						uint32_t delta)
{
	صحیح fixup_prop;
	صحیح fixup_child;
	صحیح ret;

	fdt_for_each_property_offset(fixup_prop, fdto, fixup_گره) {
		ثابت fdt32_t *fixup_val;
		ثابت نویسه *درخت_val;
		ثابت نویسه *name;
		صحیح fixup_len;
		صحیح درخت_len;
		صحیح i;

		fixup_val = fdt_getprop_by_offset(fdto, fixup_prop,
						  &name, &fixup_len);
		اگر (!fixup_val)
			بازگشت fixup_len;

		اگر (fixup_len % اندازه(uint32_t))
			بازگشت -FDT_ERR_BADOVERLAY;
		fixup_len /= اندازه(uint32_t);

		درخت_val = fdt_getprop(fdto, گره_درخت, name, &درخت_len);
		اگر (!درخت_val) {
			اگر (درخت_len == -FDT_ERR_NOTFOUND)
				بازگشت -FDT_ERR_BADOVERLAY;

			بازگشت درخت_len;
		}

		برای (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_to_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			memcpy(&adj_val, درخت_val + poffset, اندازه(adj_val));

			adj_val = cpu_to_fdt32(fdt32_to_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  گره_درخت,
								  name,
								  strlen(name),
								  poffset,
								  &adj_val,
								  اندازه(adj_val));
			اگر (ret == -FDT_ERR_NOSPACE)
				بازگشت -FDT_ERR_BADOVERLAY;

			اگر (ret)
				بازگشت ret;
		}
	}

	fdt_for_each_subnode(fixup_child, fdto, fixup_گره) {
		ثابت نویسه *fixup_child_name = fdt_get_name(fdto, fixup_child,
							    پوچ);
		صحیح درخت_child;

		درخت_child = fdt_subnode_offset(fdto, گره_درخت,
						fixup_child_name);
		اگر (درخت_child == -FDT_ERR_NOTFOUND)
			بازگشت -FDT_ERR_BADOVERLAY;
		اگر (درخت_child < 0)
			بازگشت درخت_child;

		ret = overlay_update_local_گره_references(fdto,
							   درخت_child,
							   fixup_child,
							   delta);
		اگر (ret)
			بازگشت ret;
	}

	بازگشت 0;
}

/**
 * overlay_update_local_references - Adjust the overlay references
 * @fdto: Device tree overlay blob
 * @delta: Offset to shift the phandles of
 *
 * overlay_update_local_references() update all the phandles pointing
 * to a node within the device tree overlay by adding a constant
 * delta to not conflict with the base overlay.
 *
 * This is mainly used as part of a device tree application process,
 * where you want the device tree overlays phandles to not conflict
 * with the ones from the base device tree before merging them.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ایستا صحیح overlay_update_local_references(تهی *fdto, uint32_t delta)
{
	صحیح fixups;

	fixups = fdt_path_offset(fdto, "/__local_fixups__");
	اگر (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		اگر (fixups == -FDT_ERR_NOTFOUND)
			بازگشت 0;

		بازگشت fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	بازگشت overlay_update_local_گره_references(fdto, 0, fixups,
						    delta);
}

/**
 * overlay_fixup_one_phandle - Set an overlay phandle to the base one
 * @fdt: Base Device Tree blob
 * @fdto: Device tree overlay blob
 * @symbols_off: Node offset of the symbols node in the base device tree
 * @path: Path to a node holding a phandle in the overlay
 * @path_len: number of path characters to consider
 * @name: Name of the property holding the phandle reference in the overlay
 * @name_len: number of name characters to consider
 * @poffset: Offset within the overlay property where the phandle is stored
 * @label: Label of the node referenced by the phandle
 *
 * overlay_fixup_one_phandle() resolves an overlay phandle pointing to
 * a node in the base device tree.
 *
 * This is part of the device tree overlay application process, when
 * you want all the phandles in the overlay to point to the actual
 * base dt nodes.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ایستا صحیح overlay_fixup_one_phandle(تهی *fdt, تهی *fdto,
				     صحیح symbols_off,
				     ثابت نویسه *path, uint32_t path_len,
				     ثابت نویسه *name, uint32_t name_len,
				     صحیح poffset, ثابت نویسه *label)
{
	ثابت نویسه *نماد_path;
	uint32_t phandle;
	fdt32_t phandle_prop;
	صحیح نماد_off, fixup_off;
	صحیح prop_len;

	اگر (symbols_off < 0)
		بازگشت symbols_off;

	نماد_path = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	اگر (!نماد_path)
		بازگشت prop_len;

	نماد_off = fdt_path_offset(fdt, نماد_path);
	اگر (نماد_off < 0)
		بازگشت نماد_off;

	phandle = fdt_get_phandle(fdt, نماد_off);
	اگر (!phandle)
		بازگشت -FDT_ERR_NOTFOUND;

	fixup_off = fdt_path_offset_namelen(fdto, path, path_len);
	اگر (fixup_off == -FDT_ERR_NOTFOUND)
		بازگشت -FDT_ERR_BADOVERLAY;
	اگر (fixup_off < 0)
		بازگشت fixup_off;

	phandle_prop = cpu_to_fdt32(phandle);
	بازگشت fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   اندازه(phandle_prop));
}

/**
 * overlay_fixup_phandle - Set an overlay phandle to the base one
 * @fdt: Base Device Tree blob
 * @fdto: Device tree overlay blob
 * @symbols_off: Node offset of the symbols node in the base device tree
 * @property: Property offset in the overlay holding the list of fixups
 *
 * overlay_fixup_phandle() resolves all the overlay phandles pointed
 * to in a __fixups__ property, and updates them to match the phandles
 * in use in the base device tree.
 *
 * This is part of the device tree overlay application process, when
 * you want all the phandles in the overlay to point to the actual
 * base dt nodes.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ایستا صحیح overlay_fixup_phandle(تهی *fdt, تهی *fdto, صحیح symbols_off,
				 صحیح property)
{
	ثابت نویسه *مقدار;
	ثابت نویسه *label;
	صحیح len;

	مقدار = fdt_getprop_by_offset(fdto, property,
				      &label, &len);
	اگر (!مقدار) {
		اگر (len == -FDT_ERR_NOTFOUND)
			بازگشت -FDT_ERR_INTERNAL;

		بازگشت len;
	}

	انجامبده {
		ثابت نویسه *path, *name, *fixup_end;
		ثابت نویسه *fixup_str = مقدار;
		uint32_t path_len, name_len;
		uint32_t fixup_len;
		نویسه *sep, *endptr;
		صحیح poffset, ret;

		fixup_end = memchr(مقدار, '\0', len);
		اگر (!fixup_end)
			بازگشت -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_end - fixup_str;

		len -= fixup_len + 1;
		مقدار += fixup_len + 1;

		path = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		اگر (!sep || *sep != ':')
			بازگشت -FDT_ERR_BADOVERLAY;

		path_len = sep - path;
		اگر (path_len == (fixup_len - 1))
			بازگشت -FDT_ERR_BADOVERLAY;

		fixup_len -= path_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		اگر (!sep || *sep != ':')
			بازگشت -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		اگر (!name_len)
			بازگشت -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		اگر ((*endptr != '\0') || (endptr <= (sep + 1)))
			بازگشت -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						path, path_len, name, name_len,
						poffset, label);
		اگر (ret)
			بازگشت ret;
	} وقتی (len > 0);

	بازگشت 0;
}

/**
 * overlay_fixup_phandles - Resolve the overlay phandles to the base
 *                          device tree
 * @fdt: Base Device Tree blob
 * @fdto: Device tree overlay blob
 *
 * overlay_fixup_phandles() resolves all the overlay phandles pointing
 * to nodes in the base device tree.
 *
 * This is one of the steps of the device tree overlay application
 * process, when you want all the phandles in the overlay to point to
 * the actual base dt nodes.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ایستا صحیح overlay_fixup_phandles(تهی *fdt, تهی *fdto)
{
	صحیح fixups_off, symbols_off;
	صحیح property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_path_offset(fdto, "/__fixups__");
	اگر (fixups_off == -FDT_ERR_NOTFOUND)
		بازگشت 0; /* nothing to do */
	اگر (fixups_off < 0)
		بازگشت fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_path_offset(fdt, "/__symbols__");
	اگر ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		بازگشت symbols_off;

	fdt_for_each_property_offset(property, fdto, fixups_off) {
		صحیح ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		اگر (ret)
			بازگشت ret;
	}

	بازگشت 0;
}

/**
 * overlay_apply_node - Merges a node into the base device tree
 * @fdt: Base Device Tree blob
 * @target: Node offset in the base device tree to apply the fragment to
 * @fdto: Device tree overlay blob
 * @node: Node offset in the overlay holding the changes to merge
 *
 * overlay_apply_node() merges a node into a target base device tree
 * node pointed.
 *
 * This is part of the final step in the device tree overlay
 * application process, when all the phandles have been adjusted and
 * resolved and you just have to merge overlay into the base device
 * tree.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ایستا صحیح overlay_apply_گره(تهی *fdt, صحیح target,
			      تهی *fdto, صحیح گره)
{
	صحیح property;
	صحیح subnode;

	fdt_for_each_property_offset(property, fdto, گره) {
		ثابت نویسه *name;
		ثابت تهی *prop;
		صحیح prop_len;
		صحیح ret;

		prop = fdt_getprop_by_offset(fdto, property, &name,
					     &prop_len);
		اگر (prop_len == -FDT_ERR_NOTFOUND)
			بازگشت -FDT_ERR_INTERNAL;
		اگر (prop_len < 0)
			بازگشت prop_len;

		ret = fdt_setprop(fdt, target, name, prop, prop_len);
		اگر (ret)
			بازگشت ret;
	}

	fdt_for_each_subnode(subnode, fdto, گره) {
		ثابت نویسه *name = fdt_get_name(fdto, subnode, پوچ);
		صحیح nnode;
		صحیح ret;

		nnode = fdt_add_subnode(fdt, target, name);
		اگر (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_offset(fdt, target, name);
			اگر (nnode == -FDT_ERR_NOTFOUND)
				بازگشت -FDT_ERR_INTERNAL;
		}

		اگر (nnode < 0)
			بازگشت nnode;

		ret = overlay_apply_گره(fdt, nnode, fdto, subnode);
		اگر (ret)
			بازگشت ret;
	}

	بازگشت 0;
}

/**
 * overlay_merge - Merge an overlay into its base device tree
 * @fdt: Base Device Tree blob
 * @fdto: Device tree overlay blob
 *
 * overlay_merge() merges an overlay into its base device tree.
 *
 * This is the next to last step in the device tree overlay application
 * process, when all the phandles have been adjusted and resolved and
 * you just have to merge overlay into the base device tree.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ایستا صحیح overlay_merge(تهی *fdt, تهی *fdto)
{
	صحیح fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		صحیح overlay;
		صحیح target;
		صحیح ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_offset(fdto, fragment, "__overlay__");
		اگر (overlay == -FDT_ERR_NOTFOUND)
			ادامه;

		اگر (overlay < 0)
			بازگشت overlay;

		target = overlay_get_target(fdt, fdto, fragment, پوچ);
		اگر (target < 0)
			بازگشت target;

		ret = overlay_apply_گره(fdt, target, fdto, overlay);
		اگر (ret)
			بازگشت ret;
	}

	بازگشت 0;
}

ایستا صحیح get_path_len(ثابت تهی *fdt, صحیح nodeoffset)
{
	صحیح len = 0, namelen;
	ثابت نویسه *name;

	FDT_RO_PROBE(fdt);

	برای (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		اگر (!name)
			بازگشت namelen;

		/* root? we're done */
		اگر (namelen == 0)
			بشکن;

		nodeoffset = fdt_parent_offset(fdt, nodeoffset);
		اگر (nodeoffset < 0)
			بازگشت nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	اگر (len == 0)
		len++;
	بازگشت len;
}

/**
 * overlay_symbol_update - Update the symbols of base tree after a merge
 * @fdt: Base Device Tree blob
 * @fdto: Device tree overlay blob
 *
 * overlay_symbol_update() updates the symbols of the base tree with the
 * symbols of the applied overlay
 *
 * This is the last step in the device tree overlay application
 * process, allowing the reference of overlay symbols by subsequent
 * overlay operations.
 *
 * returns:
 *      0 on success
 *      Negative error code on failure
 */
ایستا صحیح overlay_نماد_update(تهی *fdt, تهی *fdto)
{
	صحیح ریشه_sym, ov_sym, prop, path_len, fragment, target;
	صحیح len, frag_name_len, ret, rel_path_len;
	ثابت نویسه *s, *e;
	ثابت نویسه *path;
	ثابت نویسه *name;
	ثابت نویسه *frag_name;
	ثابت نویسه *rel_path;
	ثابت نویسه *target_path;
	نویسه *buf;
	تهی *p;

	ov_sym = fdt_subnode_offset(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	اگر (ov_sym < 0)
		بازگشت 0;

	ریشه_sym = fdt_subnode_offset(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	اگر (ریشه_sym == -FDT_ERR_NOTFOUND)
		ریشه_sym = fdt_add_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	اگر (ریشه_sym < 0)
		بازگشت ریشه_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_offset(prop, fdto, ov_sym) {
		path = fdt_getprop_by_offset(fdto, prop, &name, &path_len);
		اگر (!path)
			بازگشت path_len;

		/* verify it's a string property (terminated by a single \0) */
		اگر (path_len < 1 || memchr(path, '\0', path_len) != &path[path_len - 1])
			بازگشت -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = path + path_len;

		اگر (*path != '/')
			بازگشت -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(path + 1, '/');
		اگر (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			ادامه;
		}

		frag_name = path + 1;
		frag_name_len = s - path - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = اندازه("/__overlay__/") - 1;
		اگر ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_path = s + len;
			rel_path_len = e - rel_path - 1;
		} وگرنه اگر ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_path = "";
			rel_path_len = 0;
		} وگرنه {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			ادامه;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_offset_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		اگر (ret < 0)
			بازگشت -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_offset(fdto, fragment, "__overlay__");
		اگر (ret < 0)
			بازگشت -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_target(fdt, fdto, fragment, &target_path);
		اگر (ret < 0)
			بازگشت ret;
		target = ret;

		/* if we have a target path use */
		اگر (!target_path) {
			ret = get_path_len(fdt, target);
			اگر (ret < 0)
				بازگشت ret;
			len = ret;
		} وگرنه {
			len = strlen(target_path);
		}

		ret = fdt_setprop_placeholder(fdt, ریشه_sym, name,
				len + (len > 1) + rel_path_len + 1, &p);
		اگر (ret < 0)
			بازگشت ret;

		اگر (!target_path) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_target(fdt, fdto, fragment, &target_path);
			اگر (ret < 0)
				بازگشت ret;
			target = ret;
		}

		buf = p;
		اگر (len > 1) { /* target is not root */
			اگر (!target_path) {
				ret = fdt_get_path(fdt, target, buf, len + 1);
				اگر (ret < 0)
					بازگشت ret;
			} وگرنه
				memcpy(buf, target_path, len + 1);

		} وگرنه
			len--;

		buf[len] = '/';
		memcpy(buf + len + 1, rel_path, rel_path_len);
		buf[len + 1 + rel_path_len] = '\0';
	}

	بازگشت 0;
}

صحیح fdt_overlay_apply(تهی *fdt, تهی *fdto)
{
	uint32_t delta;
	صحیح ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_find_max_phandle(fdt, &delta);
	اگر (ret)
		برو err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	اگر (ret)
		برو err;

	ret = overlay_update_local_references(fdto, delta);
	اگر (ret)
		برو err;

	ret = overlay_fixup_phandles(fdt, fdto);
	اگر (ret)
		برو err;

	ret = overlay_merge(fdt, fdto);
	اگر (ret)
		برو err;

	ret = overlay_نماد_update(fdt, fdto);
	اگر (ret)
		برو err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	بازگشت 0;

err:
	/*
	 * The overlay might have been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	/*
	 * The base device tree might have been damaged, erase its
	 * magic.
	 */
	fdt_set_magic(fdt, ~0);

	بازگشت ret;
}
