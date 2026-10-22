// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#ضمن "libfdt_env.h"

#ضمن <fdt.h>
#ضمن <libfdt.h>

#ضمن "libfdt_internal.h"

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
ساكن uint32_t overlay_get_target_phandle(ثابت فراغ *fdto, صحيح fragment)
{
	ثابت fdt32_t *val;
	صحيح len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	אם (!val)
		החזר 0;

	אם ((len != حجم(*val)) || (fdt32_to_cpu(*val) == (uint32_t)-1))
		החזר (uint32_t)-1;

	החזר fdt32_to_cpu(*val);
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
ساكن صحيح overlay_get_target(ثابت فراغ *fdt, ثابت فراغ *fdto,
			      صحيح fragment, حرف ثابت **pathp)
{
	uint32_t phandle;
	ثابت حرف *path = عدم;
	صحيح path_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_target_phandle(fdto, fragment);
	אם (phandle == (uint32_t)-1)
		החזר -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	אם (!phandle) {
		/* And then a path based lookup */
		path = fdt_getprop(fdto, fragment, "target-path", &path_len);
		אם (path)
			ret = fdt_path_offset(fdt, path);
		אחרת
			ret = path_len;
	} אחרת
		ret = fdt_node_offset_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	אם (ret < 0 && path_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	אם (ret < 0)
		החזר ret;

	/* return pointer to path (if available) */
	אם (pathp)
		*pathp = path ? path : عدم;

	החזר ret;
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
ساكن صحيح overlay_phandle_add_offset(فراغ *fdt, صحيح node,
				      ثابت حرف *name, uint32_t delta)
{
	ثابت fdt32_t *val;
	uint32_t adj_val;
	صحيح len;

	val = fdt_getprop(fdt, node, name, &len);
	אם (!val)
		החזר len;

	אם (len != حجم(*val))
		החזר -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_to_cpu(*val);
	אם ((adj_val + delta) < adj_val)
		החזר -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	אם (adj_val == (uint32_t)-1)
		החזר -FDT_ERR_NOPHANDLES;

	החזר fdt_setprop_inplace_u32(fdt, node, name, adj_val);
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
ساكن صحيح overlay_adjust_node_phandles(فراغ *fdto, صحيح node,
					uint32_t delta)
{
	صحيح child;
	صحيح ret;

	ret = overlay_phandle_add_offset(fdto, node, "phandle", delta);
	אם (ret && ret != -FDT_ERR_NOTFOUND)
		החזר ret;

	ret = overlay_phandle_add_offset(fdto, node, "linux,phandle", delta);
	אם (ret && ret != -FDT_ERR_NOTFOUND)
		החזר ret;

	fdt_for_each_subnode(child, fdto, node) {
		ret = overlay_adjust_node_phandles(fdto, child, delta);
		אם (ret)
			החזר ret;
	}

	החזר 0;
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
ساكن صحيح overlay_adjust_local_phandles(فراغ *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	החזר overlay_adjust_node_phandles(fdto, 0, delta);
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
ساكن صحيح overlay_update_local_node_references(فراغ *fdto,
						صحيح tree_node,
						صحيح fixup_node,
						uint32_t delta)
{
	صحيح fixup_prop;
	صحيح fixup_child;
	صحيح ret;

	fdt_for_each_property_offset(fixup_prop, fdto, fixup_node) {
		ثابت fdt32_t *fixup_val;
		ثابت حرف *tree_val;
		ثابت حرف *name;
		صحيح fixup_len;
		صحيح tree_len;
		صحيح i;

		fixup_val = fdt_getprop_by_offset(fdto, fixup_prop,
						  &name, &fixup_len);
		אם (!fixup_val)
			החזר fixup_len;

		אם (fixup_len % حجم(uint32_t))
			החזר -FDT_ERR_BADOVERLAY;
		fixup_len /= حجم(uint32_t);

		tree_val = fdt_getprop(fdto, tree_node, name, &tree_len);
		אם (!tree_val) {
			אם (tree_len == -FDT_ERR_NOTFOUND)
				החזר -FDT_ERR_BADOVERLAY;

			החזר tree_len;
		}

		עבור (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_to_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			memcpy(&adj_val, tree_val + poffset, حجم(adj_val));

			adj_val = cpu_to_fdt32(fdt32_to_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  tree_node,
								  name,
								  strlen(name),
								  poffset,
								  &adj_val,
								  حجم(adj_val));
			אם (ret == -FDT_ERR_NOSPACE)
				החזר -FDT_ERR_BADOVERLAY;

			אם (ret)
				החזר ret;
		}
	}

	fdt_for_each_subnode(fixup_child, fdto, fixup_node) {
		ثابت حرف *fixup_child_name = fdt_get_name(fdto, fixup_child,
							    عدم);
		صحيح tree_child;

		tree_child = fdt_subnode_offset(fdto, tree_node,
						fixup_child_name);
		אם (tree_child == -FDT_ERR_NOTFOUND)
			החזר -FDT_ERR_BADOVERLAY;
		אם (tree_child < 0)
			החזר tree_child;

		ret = overlay_update_local_node_references(fdto,
							   tree_child,
							   fixup_child,
							   delta);
		אם (ret)
			החזר ret;
	}

	החזר 0;
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
ساكن صحيح overlay_update_local_references(فراغ *fdto, uint32_t delta)
{
	صحيح fixups;

	fixups = fdt_path_offset(fdto, "/__local_fixups__");
	אם (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		אם (fixups == -FDT_ERR_NOTFOUND)
			החזר 0;

		החזר fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	החזר overlay_update_local_node_references(fdto, 0, fixups,
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
ساكن صحيح overlay_fixup_one_phandle(فراغ *fdt, فراغ *fdto,
				     صحيح symbols_off,
				     ثابت حرف *path, uint32_t path_len,
				     ثابت حرف *name, uint32_t name_len,
				     صحيح poffset, ثابت حرف *label)
{
	ثابت حرف *symbol_path;
	uint32_t phandle;
	fdt32_t phandle_prop;
	صحيح symbol_off, fixup_off;
	صحيح prop_len;

	אם (symbols_off < 0)
		החזר symbols_off;

	symbol_path = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	אם (!symbol_path)
		החזר prop_len;

	symbol_off = fdt_path_offset(fdt, symbol_path);
	אם (symbol_off < 0)
		החזר symbol_off;

	phandle = fdt_get_phandle(fdt, symbol_off);
	אם (!phandle)
		החזר -FDT_ERR_NOTFOUND;

	fixup_off = fdt_path_offset_namelen(fdto, path, path_len);
	אם (fixup_off == -FDT_ERR_NOTFOUND)
		החזר -FDT_ERR_BADOVERLAY;
	אם (fixup_off < 0)
		החזר fixup_off;

	phandle_prop = cpu_to_fdt32(phandle);
	החזר fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   حجم(phandle_prop));
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
ساكن صحيح overlay_fixup_phandle(فراغ *fdt, فراغ *fdto, صحيح symbols_off,
				 صحيح property)
{
	ثابت حرف *value;
	ثابت حرف *label;
	صحيح len;

	value = fdt_getprop_by_offset(fdto, property,
				      &label, &len);
	אם (!value) {
		אם (len == -FDT_ERR_NOTFOUND)
			החזר -FDT_ERR_INTERNAL;

		החזר len;
	}

	עשה {
		ثابت حرف *path, *name, *fixup_end;
		ثابت حرف *fixup_str = value;
		uint32_t path_len, name_len;
		uint32_t fixup_len;
		حرف *sep, *endptr;
		صحيح poffset, ret;

		fixup_end = memchr(value, '\0', len);
		אם (!fixup_end)
			החזר -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_end - fixup_str;

		len -= fixup_len + 1;
		value += fixup_len + 1;

		path = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		אם (!sep || *sep != ':')
			החזר -FDT_ERR_BADOVERLAY;

		path_len = sep - path;
		אם (path_len == (fixup_len - 1))
			החזר -FDT_ERR_BADOVERLAY;

		fixup_len -= path_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		אם (!sep || *sep != ':')
			החזר -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		אם (!name_len)
			החזר -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		אם ((*endptr != '\0') || (endptr <= (sep + 1)))
			החזר -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						path, path_len, name, name_len,
						poffset, label);
		אם (ret)
			החזר ret;
	} כלעוד (len > 0);

	החזר 0;
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
ساكن صحيح overlay_fixup_phandles(فراغ *fdt, فراغ *fdto)
{
	صحيح fixups_off, symbols_off;
	صحيح property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_path_offset(fdto, "/__fixups__");
	אם (fixups_off == -FDT_ERR_NOTFOUND)
		החזר 0; /* nothing to do */
	אם (fixups_off < 0)
		החזר fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_path_offset(fdt, "/__symbols__");
	אם ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		החזר symbols_off;

	fdt_for_each_property_offset(property, fdto, fixups_off) {
		صحيح ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		אם (ret)
			החזר ret;
	}

	החזר 0;
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
ساكن صحيح overlay_apply_node(فراغ *fdt, صحيح target,
			      فراغ *fdto, صحيح node)
{
	صحيح property;
	صحيح subnode;

	fdt_for_each_property_offset(property, fdto, node) {
		ثابت حرف *name;
		ثابت فراغ *prop;
		صحيح prop_len;
		صحيح ret;

		prop = fdt_getprop_by_offset(fdto, property, &name,
					     &prop_len);
		אם (prop_len == -FDT_ERR_NOTFOUND)
			החזר -FDT_ERR_INTERNAL;
		אם (prop_len < 0)
			החזר prop_len;

		ret = fdt_setprop(fdt, target, name, prop, prop_len);
		אם (ret)
			החזר ret;
	}

	fdt_for_each_subnode(subnode, fdto, node) {
		ثابت حرف *name = fdt_get_name(fdto, subnode, عدم);
		صحيح nnode;
		صحيح ret;

		nnode = fdt_add_subnode(fdt, target, name);
		אם (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_offset(fdt, target, name);
			אם (nnode == -FDT_ERR_NOTFOUND)
				החזר -FDT_ERR_INTERNAL;
		}

		אם (nnode < 0)
			החזר nnode;

		ret = overlay_apply_node(fdt, nnode, fdto, subnode);
		אם (ret)
			החזר ret;
	}

	החזר 0;
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
ساكن صحيح overlay_merge(فراغ *fdt, فراغ *fdto)
{
	صحيح fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		صحيح overlay;
		صحيح target;
		صحيح ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_offset(fdto, fragment, "__overlay__");
		אם (overlay == -FDT_ERR_NOTFOUND)
			استمر;

		אם (overlay < 0)
			החזר overlay;

		target = overlay_get_target(fdt, fdto, fragment, عدم);
		אם (target < 0)
			החזר target;

		ret = overlay_apply_node(fdt, target, fdto, overlay);
		אם (ret)
			החזר ret;
	}

	החזר 0;
}

ساكن صحيح get_path_len(ثابت فراغ *fdt, صحيح nodeoffset)
{
	صحيح len = 0, namelen;
	ثابت حرف *name;

	FDT_RO_PROBE(fdt);

	עבור (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		אם (!name)
			החזר namelen;

		/* root? we're done */
		אם (namelen == 0)
			اكسر;

		nodeoffset = fdt_parent_offset(fdt, nodeoffset);
		אם (nodeoffset < 0)
			החזר nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	אם (len == 0)
		len++;
	החזר len;
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
ساكن صحيح overlay_symbol_update(فراغ *fdt, فراغ *fdto)
{
	صحيح root_sym, ov_sym, prop, path_len, fragment, target;
	صحيح len, frag_name_len, ret, rel_path_len;
	ثابت حرف *s, *e;
	ثابت حرف *path;
	ثابت حرف *name;
	ثابت حرف *frag_name;
	ثابت حرف *rel_path;
	ثابت حرف *target_path;
	حرف *buf;
	فراغ *p;

	ov_sym = fdt_subnode_offset(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	אם (ov_sym < 0)
		החזר 0;

	root_sym = fdt_subnode_offset(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	אם (root_sym == -FDT_ERR_NOTFOUND)
		root_sym = fdt_add_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	אם (root_sym < 0)
		החזר root_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_offset(prop, fdto, ov_sym) {
		path = fdt_getprop_by_offset(fdto, prop, &name, &path_len);
		אם (!path)
			החזר path_len;

		/* verify it's a string property (terminated by a single \0) */
		אם (path_len < 1 || memchr(path, '\0', path_len) != &path[path_len - 1])
			החזר -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = path + path_len;

		אם (*path != '/')
			החזר -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(path + 1, '/');
		אם (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			استمر;
		}

		frag_name = path + 1;
		frag_name_len = s - path - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = حجم("/__overlay__/") - 1;
		אם ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_path = s + len;
			rel_path_len = e - rel_path - 1;
		} אחרת אם ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_path = "";
			rel_path_len = 0;
		} אחרת {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			استمر;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_offset_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		אם (ret < 0)
			החזר -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_offset(fdto, fragment, "__overlay__");
		אם (ret < 0)
			החזר -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_target(fdt, fdto, fragment, &target_path);
		אם (ret < 0)
			החזר ret;
		target = ret;

		/* if we have a target path use */
		אם (!target_path) {
			ret = get_path_len(fdt, target);
			אם (ret < 0)
				החזר ret;
			len = ret;
		} אחרת {
			len = strlen(target_path);
		}

		ret = fdt_setprop_placeholder(fdt, root_sym, name,
				len + (len > 1) + rel_path_len + 1, &p);
		אם (ret < 0)
			החזר ret;

		אם (!target_path) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_target(fdt, fdto, fragment, &target_path);
			אם (ret < 0)
				החזר ret;
			target = ret;
		}

		buf = p;
		אם (len > 1) { /* target is not root */
			אם (!target_path) {
				ret = fdt_get_path(fdt, target, buf, len + 1);
				אם (ret < 0)
					החזר ret;
			} אחרת
				memcpy(buf, target_path, len + 1);

		} אחרת
			len--;

		buf[len] = '/';
		memcpy(buf + len + 1, rel_path, rel_path_len);
		buf[len + 1 + rel_path_len] = '\0';
	}

	החזר 0;
}

صحيح fdt_overlay_apply(فراغ *fdt, فراغ *fdto)
{
	uint32_t delta;
	صحيح ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_find_max_phandle(fdt, &delta);
	אם (ret)
		اذهب err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	אם (ret)
		اذهب err;

	ret = overlay_update_local_references(fdto, delta);
	אם (ret)
		اذهب err;

	ret = overlay_fixup_phandles(fdt, fdto);
	אם (ret)
		اذهب err;

	ret = overlay_merge(fdt, fdto);
	אם (ret)
		اذهب err;

	ret = overlay_symbol_update(fdt, fdto);
	אם (ret)
		اذهب err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	החזר 0;

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

	החזר ret;
}
