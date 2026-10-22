// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#અન્તર્ભાવ "libfdt_env.h"

#અન્તર્ભાવ <fdt.h>
#અન્તર્ભાવ <libfdt.h>

#અન્તર્ભાવ "libfdt_internal.h"

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
સ્થિર uint32_t overlay_get_target_phandle(અચલ શૂન્ય *fdto, પૂર્ણાંક fragment)
{
	અચલ fdt32_t *val;
	પૂર્ણાંક len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	જો (!val)
		પાછા 0;

	જો ((len != કદ(*val)) || (fdt32_to_cpu(*val) == (uint32_t)-1))
		પાછા (uint32_t)-1;

	પાછા fdt32_to_cpu(*val);
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
સ્થિર પૂર્ણાંક overlay_get_target(અચલ શૂન્ય *fdt, અચલ શૂન્ય *fdto,
			      પૂર્ણાંક fragment, અક્ષર અચલ **pathp)
{
	uint32_t phandle;
	અચલ અક્ષર *path = ખાલી;
	પૂર્ણાંક path_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_target_phandle(fdto, fragment);
	જો (phandle == (uint32_t)-1)
		પાછા -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	જો (!phandle) {
		/* And then a path based lookup */
		path = fdt_getprop(fdto, fragment, "target-path", &path_len);
		જો (path)
			ret = fdt_path_offset(fdt, path);
		નહીંતર
			ret = path_len;
	} નહીંતર
		ret = fdt_ગાંઠ_offset_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	જો (ret < 0 && path_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	જો (ret < 0)
		પાછા ret;

	/* return pointer to path (if available) */
	જો (pathp)
		*pathp = path ? path : ખાલી;

	પાછા ret;
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
સ્થિર પૂર્ણાંક overlay_phandle_add_offset(શૂન્ય *fdt, પૂર્ણાંક ગાંઠ,
				      અચલ અક્ષર *name, uint32_t delta)
{
	અચલ fdt32_t *val;
	uint32_t adj_val;
	પૂર્ણાંક len;

	val = fdt_getprop(fdt, ગાંઠ, name, &len);
	જો (!val)
		પાછા len;

	જો (len != કદ(*val))
		પાછા -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_to_cpu(*val);
	જો ((adj_val + delta) < adj_val)
		પાછા -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	જો (adj_val == (uint32_t)-1)
		પાછા -FDT_ERR_NOPHANDLES;

	પાછા fdt_setprop_inplace_u32(fdt, ગાંઠ, name, adj_val);
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
સ્થિર પૂર્ણાંક overlay_adjust_ગાંઠ_phandles(શૂન્ય *fdto, પૂર્ણાંક ગાંઠ,
					uint32_t delta)
{
	પૂર્ણાંક child;
	પૂર્ણાંક ret;

	ret = overlay_phandle_add_offset(fdto, ગાંઠ, "phandle", delta);
	જો (ret && ret != -FDT_ERR_NOTFOUND)
		પાછા ret;

	ret = overlay_phandle_add_offset(fdto, ગાંઠ, "linux,phandle", delta);
	જો (ret && ret != -FDT_ERR_NOTFOUND)
		પાછા ret;

	fdt_for_each_subnode(child, fdto, ગાંઠ) {
		ret = overlay_adjust_ગાંઠ_phandles(fdto, child, delta);
		જો (ret)
			પાછા ret;
	}

	પાછા 0;
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
સ્થિર પૂર્ણાંક overlay_adjust_local_phandles(શૂન્ય *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	પાછા overlay_adjust_ગાંઠ_phandles(fdto, 0, delta);
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
સ્થિર પૂર્ણાંક overlay_update_local_ગાંઠ_references(શૂન્ય *fdto,
						પૂર્ણાંક વૃક્ષ_ગાંઠ,
						પૂર્ણાંક fixup_ગાંઠ,
						uint32_t delta)
{
	પૂર્ણાંક fixup_prop;
	પૂર્ણાંક fixup_child;
	પૂર્ણાંક ret;

	fdt_for_each_property_offset(fixup_prop, fdto, fixup_ગાંઠ) {
		અચલ fdt32_t *fixup_val;
		અચલ અક્ષર *વૃક્ષ_val;
		અચલ અક્ષર *name;
		પૂર્ણાંક fixup_len;
		પૂર્ણાંક વૃક્ષ_len;
		પૂર્ણાંક i;

		fixup_val = fdt_getprop_by_offset(fdto, fixup_prop,
						  &name, &fixup_len);
		જો (!fixup_val)
			પાછા fixup_len;

		જો (fixup_len % કદ(uint32_t))
			પાછા -FDT_ERR_BADOVERLAY;
		fixup_len /= કદ(uint32_t);

		વૃક્ષ_val = fdt_getprop(fdto, વૃક્ષ_ગાંઠ, name, &વૃક્ષ_len);
		જો (!વૃક્ષ_val) {
			જો (વૃક્ષ_len == -FDT_ERR_NOTFOUND)
				પાછા -FDT_ERR_BADOVERLAY;

			પાછા વૃક્ષ_len;
		}

		માટે (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_to_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			memcpy(&adj_val, વૃક્ષ_val + poffset, કદ(adj_val));

			adj_val = cpu_to_fdt32(fdt32_to_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  વૃક્ષ_ગાંઠ,
								  name,
								  strlen(name),
								  poffset,
								  &adj_val,
								  કદ(adj_val));
			જો (ret == -FDT_ERR_NOSPACE)
				પાછા -FDT_ERR_BADOVERLAY;

			જો (ret)
				પાછા ret;
		}
	}

	fdt_for_each_subnode(fixup_child, fdto, fixup_ગાંઠ) {
		અચલ અક્ષર *fixup_child_name = fdt_get_name(fdto, fixup_child,
							    ખાલી);
		પૂર્ણાંક વૃક્ષ_child;

		વૃક્ષ_child = fdt_subnode_offset(fdto, વૃક્ષ_ગાંઠ,
						fixup_child_name);
		જો (વૃક્ષ_child == -FDT_ERR_NOTFOUND)
			પાછા -FDT_ERR_BADOVERLAY;
		જો (વૃક્ષ_child < 0)
			પાછા વૃક્ષ_child;

		ret = overlay_update_local_ગાંઠ_references(fdto,
							   વૃક્ષ_child,
							   fixup_child,
							   delta);
		જો (ret)
			પાછા ret;
	}

	પાછા 0;
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
સ્થિર પૂર્ણાંક overlay_update_local_references(શૂન્ય *fdto, uint32_t delta)
{
	પૂર્ણાંક fixups;

	fixups = fdt_path_offset(fdto, "/__local_fixups__");
	જો (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		જો (fixups == -FDT_ERR_NOTFOUND)
			પાછા 0;

		પાછા fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	પાછા overlay_update_local_ગાંઠ_references(fdto, 0, fixups,
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
સ્થિર પૂર્ણાંક overlay_fixup_one_phandle(શૂન્ય *fdt, શૂન્ય *fdto,
				     પૂર્ણાંક symbols_off,
				     અચલ અક્ષર *path, uint32_t path_len,
				     અચલ અક્ષર *name, uint32_t name_len,
				     પૂર્ણાંક poffset, અચલ અક્ષર *label)
{
	અચલ અક્ષર *પ્રતીક_path;
	uint32_t phandle;
	fdt32_t phandle_prop;
	પૂર્ણાંક પ્રતીક_off, fixup_off;
	પૂર્ણાંક prop_len;

	જો (symbols_off < 0)
		પાછા symbols_off;

	પ્રતીક_path = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	જો (!પ્રતીક_path)
		પાછા prop_len;

	પ્રતીક_off = fdt_path_offset(fdt, પ્રતીક_path);
	જો (પ્રતીક_off < 0)
		પાછા પ્રતીક_off;

	phandle = fdt_get_phandle(fdt, પ્રતીક_off);
	જો (!phandle)
		પાછા -FDT_ERR_NOTFOUND;

	fixup_off = fdt_path_offset_namelen(fdto, path, path_len);
	જો (fixup_off == -FDT_ERR_NOTFOUND)
		પાછા -FDT_ERR_BADOVERLAY;
	જો (fixup_off < 0)
		પાછા fixup_off;

	phandle_prop = cpu_to_fdt32(phandle);
	પાછા fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   કદ(phandle_prop));
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
સ્થિર પૂર્ણાંક overlay_fixup_phandle(શૂન્ય *fdt, શૂન્ય *fdto, પૂર્ણાંક symbols_off,
				 પૂર્ણાંક property)
{
	અચલ અક્ષર *કિંમત;
	અચલ અક્ષર *label;
	પૂર્ણાંક len;

	કિંમત = fdt_getprop_by_offset(fdto, property,
				      &label, &len);
	જો (!કિંમત) {
		જો (len == -FDT_ERR_NOTFOUND)
			પાછા -FDT_ERR_INTERNAL;

		પાછા len;
	}

	કરો {
		અચલ અક્ષર *path, *name, *fixup_end;
		અચલ અક્ષર *fixup_str = કિંમત;
		uint32_t path_len, name_len;
		uint32_t fixup_len;
		અક્ષર *sep, *endptr;
		પૂર્ણાંક poffset, ret;

		fixup_end = memchr(કિંમત, '\0', len);
		જો (!fixup_end)
			પાછા -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_end - fixup_str;

		len -= fixup_len + 1;
		કિંમત += fixup_len + 1;

		path = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		જો (!sep || *sep != ':')
			પાછા -FDT_ERR_BADOVERLAY;

		path_len = sep - path;
		જો (path_len == (fixup_len - 1))
			પાછા -FDT_ERR_BADOVERLAY;

		fixup_len -= path_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		જો (!sep || *sep != ':')
			પાછા -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		જો (!name_len)
			પાછા -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		જો ((*endptr != '\0') || (endptr <= (sep + 1)))
			પાછા -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						path, path_len, name, name_len,
						poffset, label);
		જો (ret)
			પાછા ret;
	} જ્યારે (len > 0);

	પાછા 0;
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
સ્થિર પૂર્ણાંક overlay_fixup_phandles(શૂન્ય *fdt, શૂન્ય *fdto)
{
	પૂર્ણાંક fixups_off, symbols_off;
	પૂર્ણાંક property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_path_offset(fdto, "/__fixups__");
	જો (fixups_off == -FDT_ERR_NOTFOUND)
		પાછા 0; /* nothing to do */
	જો (fixups_off < 0)
		પાછા fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_path_offset(fdt, "/__symbols__");
	જો ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		પાછા symbols_off;

	fdt_for_each_property_offset(property, fdto, fixups_off) {
		પૂર્ણાંક ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		જો (ret)
			પાછા ret;
	}

	પાછા 0;
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
સ્થિર પૂર્ણાંક overlay_apply_ગાંઠ(શૂન્ય *fdt, પૂર્ણાંક target,
			      શૂન્ય *fdto, પૂર્ણાંક ગાંઠ)
{
	પૂર્ણાંક property;
	પૂર્ણાંક subnode;

	fdt_for_each_property_offset(property, fdto, ગાંઠ) {
		અચલ અક્ષર *name;
		અચલ શૂન્ય *prop;
		પૂર્ણાંક prop_len;
		પૂર્ણાંક ret;

		prop = fdt_getprop_by_offset(fdto, property, &name,
					     &prop_len);
		જો (prop_len == -FDT_ERR_NOTFOUND)
			પાછા -FDT_ERR_INTERNAL;
		જો (prop_len < 0)
			પાછા prop_len;

		ret = fdt_setprop(fdt, target, name, prop, prop_len);
		જો (ret)
			પાછા ret;
	}

	fdt_for_each_subnode(subnode, fdto, ગાંઠ) {
		અચલ અક્ષર *name = fdt_get_name(fdto, subnode, ખાલી);
		પૂર્ણાંક nnode;
		પૂર્ણાંક ret;

		nnode = fdt_add_subnode(fdt, target, name);
		જો (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_offset(fdt, target, name);
			જો (nnode == -FDT_ERR_NOTFOUND)
				પાછા -FDT_ERR_INTERNAL;
		}

		જો (nnode < 0)
			પાછા nnode;

		ret = overlay_apply_ગાંઠ(fdt, nnode, fdto, subnode);
		જો (ret)
			પાછા ret;
	}

	પાછા 0;
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
સ્થિર પૂર્ણાંક overlay_merge(શૂન્ય *fdt, શૂન્ય *fdto)
{
	પૂર્ણાંક fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		પૂર્ણાંક overlay;
		પૂર્ણાંક target;
		પૂર્ણાંક ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_offset(fdto, fragment, "__overlay__");
		જો (overlay == -FDT_ERR_NOTFOUND)
			ચાલુ;

		જો (overlay < 0)
			પાછા overlay;

		target = overlay_get_target(fdt, fdto, fragment, ખાલી);
		જો (target < 0)
			પાછા target;

		ret = overlay_apply_ગાંઠ(fdt, target, fdto, overlay);
		જો (ret)
			પાછા ret;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક get_path_len(અચલ શૂન્ય *fdt, પૂર્ણાંક nodeoffset)
{
	પૂર્ણાંક len = 0, namelen;
	અચલ અક્ષર *name;

	FDT_RO_PROBE(fdt);

	માટે (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		જો (!name)
			પાછા namelen;

		/* root? we're done */
		જો (namelen == 0)
			તોડો;

		nodeoffset = fdt_parent_offset(fdt, nodeoffset);
		જો (nodeoffset < 0)
			પાછા nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	જો (len == 0)
		len++;
	પાછા len;
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
સ્થિર પૂર્ણાંક overlay_પ્રતીક_update(શૂન્ય *fdt, શૂન્ય *fdto)
{
	પૂર્ણાંક મૂળ_sym, ov_sym, prop, path_len, fragment, target;
	પૂર્ણાંક len, frag_name_len, ret, rel_path_len;
	અચલ અક્ષર *s, *e;
	અચલ અક્ષર *path;
	અચલ અક્ષર *name;
	અચલ અક્ષર *frag_name;
	અચલ અક્ષર *rel_path;
	અચલ અક્ષર *target_path;
	અક્ષર *buf;
	શૂન્ય *p;

	ov_sym = fdt_subnode_offset(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	જો (ov_sym < 0)
		પાછા 0;

	મૂળ_sym = fdt_subnode_offset(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	જો (મૂળ_sym == -FDT_ERR_NOTFOUND)
		મૂળ_sym = fdt_add_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	જો (મૂળ_sym < 0)
		પાછા મૂળ_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_offset(prop, fdto, ov_sym) {
		path = fdt_getprop_by_offset(fdto, prop, &name, &path_len);
		જો (!path)
			પાછા path_len;

		/* verify it's a string property (terminated by a single \0) */
		જો (path_len < 1 || memchr(path, '\0', path_len) != &path[path_len - 1])
			પાછા -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = path + path_len;

		જો (*path != '/')
			પાછા -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(path + 1, '/');
		જો (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			ચાલુ;
		}

		frag_name = path + 1;
		frag_name_len = s - path - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = કદ("/__overlay__/") - 1;
		જો ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_path = s + len;
			rel_path_len = e - rel_path - 1;
		} નહીંતર જો ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_path = "";
			rel_path_len = 0;
		} નહીંતર {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			ચાલુ;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_offset_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		જો (ret < 0)
			પાછા -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_offset(fdto, fragment, "__overlay__");
		જો (ret < 0)
			પાછા -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_target(fdt, fdto, fragment, &target_path);
		જો (ret < 0)
			પાછા ret;
		target = ret;

		/* if we have a target path use */
		જો (!target_path) {
			ret = get_path_len(fdt, target);
			જો (ret < 0)
				પાછા ret;
			len = ret;
		} નહીંતર {
			len = strlen(target_path);
		}

		ret = fdt_setprop_placeholder(fdt, મૂળ_sym, name,
				len + (len > 1) + rel_path_len + 1, &p);
		જો (ret < 0)
			પાછા ret;

		જો (!target_path) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_target(fdt, fdto, fragment, &target_path);
			જો (ret < 0)
				પાછા ret;
			target = ret;
		}

		buf = p;
		જો (len > 1) { /* target is not root */
			જો (!target_path) {
				ret = fdt_get_path(fdt, target, buf, len + 1);
				જો (ret < 0)
					પાછા ret;
			} નહીંતર
				memcpy(buf, target_path, len + 1);

		} નહીંતર
			len--;

		buf[len] = '/';
		memcpy(buf + len + 1, rel_path, rel_path_len);
		buf[len + 1 + rel_path_len] = '\0';
	}

	પાછા 0;
}

પૂર્ણાંક fdt_overlay_apply(શૂન્ય *fdt, શૂન્ય *fdto)
{
	uint32_t delta;
	પૂર્ણાંક ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_find_max_phandle(fdt, &delta);
	જો (ret)
		જાઓ err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	જો (ret)
		જાઓ err;

	ret = overlay_update_local_references(fdto, delta);
	જો (ret)
		જાઓ err;

	ret = overlay_fixup_phandles(fdt, fdto);
	જો (ret)
		જાઓ err;

	ret = overlay_merge(fdt, fdto);
	જો (ret)
		જાઓ err;

	ret = overlay_પ્રતીક_update(fdt, fdto);
	જો (ret)
		જાઓ err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	પાછા 0;

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

	પાછા ret;
}
