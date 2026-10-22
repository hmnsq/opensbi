// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

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
static uint32_t overlay_get_target_phandle(const void *fdto, ཧྲིལ fragment)
{
	const fdt32_t *val;
	ཧྲིལ len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	གལ་ཏེ (!val)
		ཕྱིར 0;

	གལ་ཏེ ((len != parimaa_na(*val)) || (fdt32_to_cpu(*val) == (uint32_t)-1))
		ཕྱིར (uint32_t)-1;

	ཕྱིར fdt32_to_cpu(*val);
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
static ཧྲིལ overlay_get_target(const void *fdt, const void *fdto,
			      ཧྲིལ fragment, var_na const **pathp)
{
	uint32_t phandle;
	const var_na *path = NULL;
	ཧྲིལ path_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_target_phandle(fdto, fragment);
	གལ་ཏེ (phandle == (uint32_t)-1)
		ཕྱིར -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	གལ་ཏེ (!phandle) {
		/* And then a path based lookup */
		path = fdt_getprop(fdto, fragment, "target-path", &path_len);
		གལ་ཏེ (path)
			ret = fdt_path_offset(fdt, path);
		དེ་མིན
			ret = path_len;
	} དེ་མིན
		ret = fdt_node_offset_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	གལ་ཏེ (ret < 0 && path_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	གལ་ཏེ (ret < 0)
		ཕྱིར ret;

	/* return pointer to path (if available) */
	གལ་ཏེ (pathp)
		*pathp = path ? path : NULL;

	ཕྱིར ret;
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
static ཧྲིལ overlay_phandle_add_offset(void *fdt, ཧྲིལ node,
				      const var_na *name, uint32_t delta)
{
	const fdt32_t *val;
	uint32_t adj_val;
	ཧྲིལ len;

	val = fdt_getprop(fdt, node, name, &len);
	གལ་ཏེ (!val)
		ཕྱིར len;

	གལ་ཏེ (len != parimaa_na(*val))
		ཕྱིར -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_to_cpu(*val);
	གལ་ཏེ ((adj_val + delta) < adj_val)
		ཕྱིར -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	གལ་ཏེ (adj_val == (uint32_t)-1)
		ཕྱིར -FDT_ERR_NOPHANDLES;

	ཕྱིར fdt_setprop_inplace_u32(fdt, node, name, adj_val);
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
static ཧྲིལ overlay_adjust_node_phandles(void *fdto, ཧྲིལ node,
					uint32_t delta)
{
	ཧྲིལ child;
	ཧྲིལ ret;

	ret = overlay_phandle_add_offset(fdto, node, "phandle", delta);
	གལ་ཏེ (ret && ret != -FDT_ERR_NOTFOUND)
		ཕྱིར ret;

	ret = overlay_phandle_add_offset(fdto, node, "linux,phandle", delta);
	གལ་ཏེ (ret && ret != -FDT_ERR_NOTFOUND)
		ཕྱིར ret;

	fdt_for_each_subnode(child, fdto, node) {
		ret = overlay_adjust_node_phandles(fdto, child, delta);
		གལ་ཏེ (ret)
			ཕྱིར ret;
	}

	ཕྱིར 0;
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
static ཧྲིལ overlay_adjust_local_phandles(void *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	ཕྱིར overlay_adjust_node_phandles(fdto, 0, delta);
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
static ཧྲིལ overlay_update_local_node_references(void *fdto,
						ཧྲིལ tree_node,
						ཧྲིལ fixup_node,
						uint32_t delta)
{
	ཧྲིལ fixup_prop;
	ཧྲིལ fixup_child;
	ཧྲིལ ret;

	fdt_for_each_property_offset(fixup_prop, fdto, fixup_node) {
		const fdt32_t *fixup_val;
		const var_na *tree_val;
		const var_na *name;
		ཧྲིལ fixup_len;
		ཧྲིལ tree_len;
		ཧྲིལ i;

		fixup_val = fdt_getprop_by_offset(fdto, fixup_prop,
						  &name, &fixup_len);
		གལ་ཏེ (!fixup_val)
			ཕྱིར fixup_len;

		གལ་ཏེ (fixup_len % parimaa_na(uint32_t))
			ཕྱིར -FDT_ERR_BADOVERLAY;
		fixup_len /= parimaa_na(uint32_t);

		tree_val = fdt_getprop(fdto, tree_node, name, &tree_len);
		གལ་ཏེ (!tree_val) {
			གལ་ཏེ (tree_len == -FDT_ERR_NOTFOUND)
				ཕྱིར -FDT_ERR_BADOVERLAY;

			ཕྱིར tree_len;
		}

		ཆེད་དུ (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_to_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			memcpy(&adj_val, tree_val + poffset, parimaa_na(adj_val));

			adj_val = cpu_to_fdt32(fdt32_to_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  tree_node,
								  name,
								  strlen(name),
								  poffset,
								  &adj_val,
								  parimaa_na(adj_val));
			གལ་ཏེ (ret == -FDT_ERR_NOSPACE)
				ཕྱིར -FDT_ERR_BADOVERLAY;

			གལ་ཏེ (ret)
				ཕྱིར ret;
		}
	}

	fdt_for_each_subnode(fixup_child, fdto, fixup_node) {
		const var_na *fixup_child_name = fdt_get_name(fdto, fixup_child,
							    NULL);
		ཧྲིལ tree_child;

		tree_child = fdt_subnode_offset(fdto, tree_node,
						fixup_child_name);
		གལ་ཏེ (tree_child == -FDT_ERR_NOTFOUND)
			ཕྱིར -FDT_ERR_BADOVERLAY;
		གལ་ཏེ (tree_child < 0)
			ཕྱིར tree_child;

		ret = overlay_update_local_node_references(fdto,
							   tree_child,
							   fixup_child,
							   delta);
		གལ་ཏེ (ret)
			ཕྱིར ret;
	}

	ཕྱིར 0;
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
static ཧྲིལ overlay_update_local_references(void *fdto, uint32_t delta)
{
	ཧྲིལ fixups;

	fixups = fdt_path_offset(fdto, "/__local_fixups__");
	གལ་ཏེ (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		གལ་ཏེ (fixups == -FDT_ERR_NOTFOUND)
			ཕྱིར 0;

		ཕྱིར fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	ཕྱིར overlay_update_local_node_references(fdto, 0, fixups,
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
static ཧྲིལ overlay_fixup_one_phandle(void *fdt, void *fdto,
				     ཧྲིལ symbols_off,
				     const var_na *path, uint32_t path_len,
				     const var_na *name, uint32_t name_len,
				     ཧྲིལ poffset, const var_na *label)
{
	const var_na *symbol_path;
	uint32_t phandle;
	fdt32_t phandle_prop;
	ཧྲིལ symbol_off, fixup_off;
	ཧྲིལ prop_len;

	གལ་ཏེ (symbols_off < 0)
		ཕྱིར symbols_off;

	symbol_path = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	གལ་ཏེ (!symbol_path)
		ཕྱིར prop_len;

	symbol_off = fdt_path_offset(fdt, symbol_path);
	གལ་ཏེ (symbol_off < 0)
		ཕྱིར symbol_off;

	phandle = fdt_get_phandle(fdt, symbol_off);
	གལ་ཏེ (!phandle)
		ཕྱིར -FDT_ERR_NOTFOUND;

	fixup_off = fdt_path_offset_namelen(fdto, path, path_len);
	གལ་ཏེ (fixup_off == -FDT_ERR_NOTFOUND)
		ཕྱིར -FDT_ERR_BADOVERLAY;
	གལ་ཏེ (fixup_off < 0)
		ཕྱིར fixup_off;

	phandle_prop = cpu_to_fdt32(phandle);
	ཕྱིར fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   parimaa_na(phandle_prop));
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
static ཧྲིལ overlay_fixup_phandle(void *fdt, void *fdto, ཧྲིལ symbols_off,
				 ཧྲིལ property)
{
	const var_na *value;
	const var_na *label;
	ཧྲིལ len;

	value = fdt_getprop_by_offset(fdto, property,
				      &label, &len);
	གལ་ཏེ (!value) {
		གལ་ཏེ (len == -FDT_ERR_NOTFOUND)
			ཕྱིར -FDT_ERR_INTERNAL;

		ཕྱིར len;
	}

	do {
		const var_na *path, *name, *fixup_end;
		const var_na *fixup_str = value;
		uint32_t path_len, name_len;
		uint32_t fixup_len;
		var_na *sep, *endptr;
		ཧྲིལ poffset, ret;

		fixup_end = memchr(value, '\0', len);
		གལ་ཏེ (!fixup_end)
			ཕྱིར -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_end - fixup_str;

		len -= fixup_len + 1;
		value += fixup_len + 1;

		path = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		གལ་ཏེ (!sep || *sep != ':')
			ཕྱིར -FDT_ERR_BADOVERLAY;

		path_len = sep - path;
		གལ་ཏེ (path_len == (fixup_len - 1))
			ཕྱིར -FDT_ERR_BADOVERLAY;

		fixup_len -= path_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		གལ་ཏེ (!sep || *sep != ':')
			ཕྱིར -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		གལ་ཏེ (!name_len)
			ཕྱིར -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		གལ་ཏེ ((*endptr != '\0') || (endptr <= (sep + 1)))
			ཕྱིར -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						path, path_len, name, name_len,
						poffset, label);
		གལ་ཏེ (ret)
			ཕྱིར ret;
	} སྐབས (len > 0);

	ཕྱིར 0;
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
static ཧྲིལ overlay_fixup_phandles(void *fdt, void *fdto)
{
	ཧྲིལ fixups_off, symbols_off;
	ཧྲིལ property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_path_offset(fdto, "/__fixups__");
	གལ་ཏེ (fixups_off == -FDT_ERR_NOTFOUND)
		ཕྱིར 0; /* nothing to do */
	གལ་ཏེ (fixups_off < 0)
		ཕྱིར fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_path_offset(fdt, "/__symbols__");
	གལ་ཏེ ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		ཕྱིར symbols_off;

	fdt_for_each_property_offset(property, fdto, fixups_off) {
		ཧྲིལ ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		གལ་ཏེ (ret)
			ཕྱིར ret;
	}

	ཕྱིར 0;
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
static ཧྲིལ overlay_apply_node(void *fdt, ཧྲིལ target,
			      void *fdto, ཧྲིལ node)
{
	ཧྲིལ property;
	ཧྲིལ subnode;

	fdt_for_each_property_offset(property, fdto, node) {
		const var_na *name;
		const void *prop;
		ཧྲིལ prop_len;
		ཧྲིལ ret;

		prop = fdt_getprop_by_offset(fdto, property, &name,
					     &prop_len);
		གལ་ཏེ (prop_len == -FDT_ERR_NOTFOUND)
			ཕྱིར -FDT_ERR_INTERNAL;
		གལ་ཏེ (prop_len < 0)
			ཕྱིར prop_len;

		ret = fdt_setprop(fdt, target, name, prop, prop_len);
		གལ་ཏེ (ret)
			ཕྱིར ret;
	}

	fdt_for_each_subnode(subnode, fdto, node) {
		const var_na *name = fdt_get_name(fdto, subnode, NULL);
		ཧྲིལ nnode;
		ཧྲིལ ret;

		nnode = fdt_add_subnode(fdt, target, name);
		གལ་ཏེ (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_offset(fdt, target, name);
			གལ་ཏེ (nnode == -FDT_ERR_NOTFOUND)
				ཕྱིར -FDT_ERR_INTERNAL;
		}

		གལ་ཏེ (nnode < 0)
			ཕྱིར nnode;

		ret = overlay_apply_node(fdt, nnode, fdto, subnode);
		གལ་ཏེ (ret)
			ཕྱིར ret;
	}

	ཕྱིར 0;
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
static ཧྲིལ overlay_merge(void *fdt, void *fdto)
{
	ཧྲིལ fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		ཧྲིལ overlay;
		ཧྲིལ target;
		ཧྲིལ ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_offset(fdto, fragment, "__overlay__");
		གལ་ཏེ (overlay == -FDT_ERR_NOTFOUND)
			continue;

		གལ་ཏེ (overlay < 0)
			ཕྱིར overlay;

		target = overlay_get_target(fdt, fdto, fragment, NULL);
		གལ་ཏེ (target < 0)
			ཕྱིར target;

		ret = overlay_apply_node(fdt, target, fdto, overlay);
		གལ་ཏེ (ret)
			ཕྱིར ret;
	}

	ཕྱིར 0;
}

static ཧྲིལ get_path_len(const void *fdt, ཧྲིལ nodeoffset)
{
	ཧྲིལ len = 0, namelen;
	const var_na *name;

	FDT_RO_PROBE(fdt);

	ཆེད་དུ (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		གལ་ཏེ (!name)
			ཕྱིར namelen;

		/* root? we're done */
		གལ་ཏེ (namelen == 0)
			bha_nga;

		nodeoffset = fdt_parent_offset(fdt, nodeoffset);
		གལ་ཏེ (nodeoffset < 0)
			ཕྱིར nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	གལ་ཏེ (len == 0)
		len++;
	ཕྱིར len;
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
static ཧྲིལ overlay_symbol_update(void *fdt, void *fdto)
{
	ཧྲིལ root_sym, ov_sym, prop, path_len, fragment, target;
	ཧྲིལ len, frag_name_len, ret, rel_path_len;
	const var_na *s, *e;
	const var_na *path;
	const var_na *name;
	const var_na *frag_name;
	const var_na *rel_path;
	const var_na *target_path;
	var_na *buf;
	void *p;

	ov_sym = fdt_subnode_offset(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	གལ་ཏེ (ov_sym < 0)
		ཕྱིར 0;

	root_sym = fdt_subnode_offset(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	གལ་ཏེ (root_sym == -FDT_ERR_NOTFOUND)
		root_sym = fdt_add_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	གལ་ཏེ (root_sym < 0)
		ཕྱིར root_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_offset(prop, fdto, ov_sym) {
		path = fdt_getprop_by_offset(fdto, prop, &name, &path_len);
		གལ་ཏེ (!path)
			ཕྱིར path_len;

		/* verify it's a string property (terminated by a single \0) */
		གལ་ཏེ (path_len < 1 || memchr(path, '\0', path_len) != &path[path_len - 1])
			ཕྱིར -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = path + path_len;

		གལ་ཏེ (*path != '/')
			ཕྱིར -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(path + 1, '/');
		གལ་ཏེ (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			continue;
		}

		frag_name = path + 1;
		frag_name_len = s - path - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = parimaa_na("/__overlay__/") - 1;
		གལ་ཏེ ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_path = s + len;
			rel_path_len = e - rel_path - 1;
		} དེ་མིན གལ་ཏེ ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_path = "";
			rel_path_len = 0;
		} དེ་མིན {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			continue;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_offset_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		གལ་ཏེ (ret < 0)
			ཕྱིར -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_offset(fdto, fragment, "__overlay__");
		གལ་ཏེ (ret < 0)
			ཕྱིར -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_target(fdt, fdto, fragment, &target_path);
		གལ་ཏེ (ret < 0)
			ཕྱིར ret;
		target = ret;

		/* if we have a target path use */
		གལ་ཏེ (!target_path) {
			ret = get_path_len(fdt, target);
			གལ་ཏེ (ret < 0)
				ཕྱིར ret;
			len = ret;
		} དེ་མིན {
			len = strlen(target_path);
		}

		ret = fdt_setprop_placeholder(fdt, root_sym, name,
				len + (len > 1) + rel_path_len + 1, &p);
		གལ་ཏེ (ret < 0)
			ཕྱིར ret;

		གལ་ཏེ (!target_path) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_target(fdt, fdto, fragment, &target_path);
			གལ་ཏེ (ret < 0)
				ཕྱིར ret;
			target = ret;
		}

		buf = p;
		གལ་ཏེ (len > 1) { /* target is not root */
			གལ་ཏེ (!target_path) {
				ret = fdt_get_path(fdt, target, buf, len + 1);
				གལ་ཏེ (ret < 0)
					ཕྱིར ret;
			} དེ་མིན
				memcpy(buf, target_path, len + 1);

		} དེ་མིན
			len--;

		buf[len] = '/';
		memcpy(buf + len + 1, rel_path, rel_path_len);
		buf[len + 1 + rel_path_len] = '\0';
	}

	ཕྱིར 0;
}

ཧྲིལ fdt_overlay_apply(void *fdt, void *fdto)
{
	uint32_t delta;
	ཧྲིལ ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_find_max_phandle(fdt, &delta);
	གལ་ཏེ (ret)
		goto err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	གལ་ཏེ (ret)
		goto err;

	ret = overlay_update_local_references(fdto, delta);
	གལ་ཏེ (ret)
		goto err;

	ret = overlay_fixup_phandles(fdt, fdto);
	གལ་ཏེ (ret)
		goto err;

	ret = overlay_merge(fdt, fdto);
	གལ་ཏེ (ret)
		goto err;

	ret = overlay_symbol_update(fdt, fdto);
	གལ་ཏེ (ret)
		goto err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	ཕྱིར 0;

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

	ཕྱིར ret;
}
