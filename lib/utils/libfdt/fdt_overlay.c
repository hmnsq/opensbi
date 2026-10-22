// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#अन्तर्भाव "libfdt_env.h"

#अन्तर्भाव <fdt.h>
#अन्तर्भाव <libfdt.h>

#अन्तर्भाव "libfdt_internal.h"

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
स्थिर uint32_t overlay_get_target_phandle(स्थिरांक शून्य *fdto, पूर्णांक fragment)
{
	स्थिरांक fdt32_t *val;
	पूर्णांक len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	जर (!val)
		परत 0;

	जर ((len != आकार(*val)) || (fdt32_to_cpu(*val) == (uint32_t)-1))
		परत (uint32_t)-1;

	परत fdt32_to_cpu(*val);
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
स्थिर पूर्णांक overlay_get_target(स्थिरांक शून्य *fdt, स्थिरांक शून्य *fdto,
			      पूर्णांक fragment, अक्षर स्थिरांक **pathp)
{
	uint32_t phandle;
	स्थिरांक अक्षर *path = रिक्त;
	पूर्णांक path_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_target_phandle(fdto, fragment);
	जर (phandle == (uint32_t)-1)
		परत -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	जर (!phandle) {
		/* And then a path based lookup */
		path = fdt_getprop(fdto, fragment, "target-path", &path_len);
		जर (path)
			ret = fdt_path_offset(fdt, path);
		अन्यथा
			ret = path_len;
	} अन्यथा
		ret = fdt_गाठ_offset_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	जर (ret < 0 && path_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	जर (ret < 0)
		परत ret;

	/* return pointer to path (if available) */
	जर (pathp)
		*pathp = path ? path : रिक्त;

	परत ret;
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
स्थिर पूर्णांक overlay_phandle_add_offset(शून्य *fdt, पूर्णांक गाठ,
				      स्थिरांक अक्षर *name, uint32_t delta)
{
	स्थिरांक fdt32_t *val;
	uint32_t adj_val;
	पूर्णांक len;

	val = fdt_getprop(fdt, गाठ, name, &len);
	जर (!val)
		परत len;

	जर (len != आकार(*val))
		परत -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_to_cpu(*val);
	जर ((adj_val + delta) < adj_val)
		परत -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	जर (adj_val == (uint32_t)-1)
		परत -FDT_ERR_NOPHANDLES;

	परत fdt_setprop_inplace_u32(fdt, गाठ, name, adj_val);
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
स्थिर पूर्णांक overlay_adjust_गाठ_phandles(शून्य *fdto, पूर्णांक गाठ,
					uint32_t delta)
{
	पूर्णांक child;
	पूर्णांक ret;

	ret = overlay_phandle_add_offset(fdto, गाठ, "phandle", delta);
	जर (ret && ret != -FDT_ERR_NOTFOUND)
		परत ret;

	ret = overlay_phandle_add_offset(fdto, गाठ, "linux,phandle", delta);
	जर (ret && ret != -FDT_ERR_NOTFOUND)
		परत ret;

	fdt_for_each_subnode(child, fdto, गाठ) {
		ret = overlay_adjust_गाठ_phandles(fdto, child, delta);
		जर (ret)
			परत ret;
	}

	परत 0;
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
स्थिर पूर्णांक overlay_adjust_local_phandles(शून्य *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	परत overlay_adjust_गाठ_phandles(fdto, 0, delta);
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
स्थिर पूर्णांक overlay_update_local_गाठ_references(शून्य *fdto,
						पूर्णांक वृक्ष_गाठ,
						पूर्णांक fixup_गाठ,
						uint32_t delta)
{
	पूर्णांक fixup_prop;
	पूर्णांक fixup_child;
	पूर्णांक ret;

	fdt_for_each_property_offset(fixup_prop, fdto, fixup_गाठ) {
		स्थिरांक fdt32_t *fixup_val;
		स्थिरांक अक्षर *वृक्ष_val;
		स्थिरांक अक्षर *name;
		पूर्णांक fixup_len;
		पूर्णांक वृक्ष_len;
		पूर्णांक i;

		fixup_val = fdt_getprop_by_offset(fdto, fixup_prop,
						  &name, &fixup_len);
		जर (!fixup_val)
			परत fixup_len;

		जर (fixup_len % आकार(uint32_t))
			परत -FDT_ERR_BADOVERLAY;
		fixup_len /= आकार(uint32_t);

		वृक्ष_val = fdt_getprop(fdto, वृक्ष_गाठ, name, &वृक्ष_len);
		जर (!वृक्ष_val) {
			जर (वृक्ष_len == -FDT_ERR_NOTFOUND)
				परत -FDT_ERR_BADOVERLAY;

			परत वृक्ष_len;
		}

		साठी (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_to_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			स_नकल(&adj_val, वृक्ष_val + poffset, आकार(adj_val));

			adj_val = cpu_to_fdt32(fdt32_to_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  वृक्ष_गाठ,
								  name,
								  म_माप(name),
								  poffset,
								  &adj_val,
								  आकार(adj_val));
			जर (ret == -FDT_ERR_NOSPACE)
				परत -FDT_ERR_BADOVERLAY;

			जर (ret)
				परत ret;
		}
	}

	fdt_for_each_subnode(fixup_child, fdto, fixup_गाठ) {
		स्थिरांक अक्षर *fixup_child_name = fdt_get_name(fdto, fixup_child,
							    रिक्त);
		पूर्णांक वृक्ष_child;

		वृक्ष_child = fdt_subnode_offset(fdto, वृक्ष_गाठ,
						fixup_child_name);
		जर (वृक्ष_child == -FDT_ERR_NOTFOUND)
			परत -FDT_ERR_BADOVERLAY;
		जर (वृक्ष_child < 0)
			परत वृक्ष_child;

		ret = overlay_update_local_गाठ_references(fdto,
							   वृक्ष_child,
							   fixup_child,
							   delta);
		जर (ret)
			परत ret;
	}

	परत 0;
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
स्थिर पूर्णांक overlay_update_local_references(शून्य *fdto, uint32_t delta)
{
	पूर्णांक fixups;

	fixups = fdt_path_offset(fdto, "/__local_fixups__");
	जर (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		जर (fixups == -FDT_ERR_NOTFOUND)
			परत 0;

		परत fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	परत overlay_update_local_गाठ_references(fdto, 0, fixups,
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
स्थिर पूर्णांक overlay_fixup_one_phandle(शून्य *fdt, शून्य *fdto,
				     पूर्णांक symbols_off,
				     स्थिरांक अक्षर *path, uint32_t path_len,
				     स्थिरांक अक्षर *name, uint32_t name_len,
				     पूर्णांक poffset, स्थिरांक अक्षर *label)
{
	स्थिरांक अक्षर *चिन्ह_path;
	uint32_t phandle;
	fdt32_t phandle_prop;
	पूर्णांक चिन्ह_off, fixup_off;
	पूर्णांक prop_len;

	जर (symbols_off < 0)
		परत symbols_off;

	चिन्ह_path = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	जर (!चिन्ह_path)
		परत prop_len;

	चिन्ह_off = fdt_path_offset(fdt, चिन्ह_path);
	जर (चिन्ह_off < 0)
		परत चिन्ह_off;

	phandle = fdt_get_phandle(fdt, चिन्ह_off);
	जर (!phandle)
		परत -FDT_ERR_NOTFOUND;

	fixup_off = fdt_path_offset_namelen(fdto, path, path_len);
	जर (fixup_off == -FDT_ERR_NOTFOUND)
		परत -FDT_ERR_BADOVERLAY;
	जर (fixup_off < 0)
		परत fixup_off;

	phandle_prop = cpu_to_fdt32(phandle);
	परत fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   आकार(phandle_prop));
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
स्थिर पूर्णांक overlay_fixup_phandle(शून्य *fdt, शून्य *fdto, पूर्णांक symbols_off,
				 पूर्णांक property)
{
	स्थिरांक अक्षर *मूल्य;
	स्थिरांक अक्षर *label;
	पूर्णांक len;

	मूल्य = fdt_getprop_by_offset(fdto, property,
				      &label, &len);
	जर (!मूल्य) {
		जर (len == -FDT_ERR_NOTFOUND)
			परत -FDT_ERR_INTERNAL;

		परत len;
	}

	करा {
		स्थिरांक अक्षर *path, *name, *fixup_end;
		स्थिरांक अक्षर *fixup_str = मूल्य;
		uint32_t path_len, name_len;
		uint32_t fixup_len;
		अक्षर *sep, *endptr;
		पूर्णांक poffset, ret;

		fixup_end = स_प्रथम(मूल्य, '\0', len);
		जर (!fixup_end)
			परत -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_end - fixup_str;

		len -= fixup_len + 1;
		मूल्य += fixup_len + 1;

		path = fixup_str;
		sep = स_प्रथम(fixup_str, ':', fixup_len);
		जर (!sep || *sep != ':')
			परत -FDT_ERR_BADOVERLAY;

		path_len = sep - path;
		जर (path_len == (fixup_len - 1))
			परत -FDT_ERR_BADOVERLAY;

		fixup_len -= path_len + 1;
		name = sep + 1;
		sep = स_प्रथम(name, ':', fixup_len);
		जर (!sep || *sep != ':')
			परत -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		जर (!name_len)
			परत -FDT_ERR_BADOVERLAY;

		poffset = म_से_अदीर्घ(sep + 1, &endptr, 10);
		जर ((*endptr != '\0') || (endptr <= (sep + 1)))
			परत -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						path, path_len, name, name_len,
						poffset, label);
		जर (ret)
			परत ret;
	} पर्यंत (len > 0);

	परत 0;
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
स्थिर पूर्णांक overlay_fixup_phandles(शून्य *fdt, शून्य *fdto)
{
	पूर्णांक fixups_off, symbols_off;
	पूर्णांक property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_path_offset(fdto, "/__fixups__");
	जर (fixups_off == -FDT_ERR_NOTFOUND)
		परत 0; /* nothing to do */
	जर (fixups_off < 0)
		परत fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_path_offset(fdt, "/__symbols__");
	जर ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		परत symbols_off;

	fdt_for_each_property_offset(property, fdto, fixups_off) {
		पूर्णांक ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		जर (ret)
			परत ret;
	}

	परत 0;
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
स्थिर पूर्णांक overlay_apply_गाठ(शून्य *fdt, पूर्णांक target,
			      शून्य *fdto, पूर्णांक गाठ)
{
	पूर्णांक property;
	पूर्णांक subnode;

	fdt_for_each_property_offset(property, fdto, गाठ) {
		स्थिरांक अक्षर *name;
		स्थिरांक शून्य *prop;
		पूर्णांक prop_len;
		पूर्णांक ret;

		prop = fdt_getprop_by_offset(fdto, property, &name,
					     &prop_len);
		जर (prop_len == -FDT_ERR_NOTFOUND)
			परत -FDT_ERR_INTERNAL;
		जर (prop_len < 0)
			परत prop_len;

		ret = fdt_setprop(fdt, target, name, prop, prop_len);
		जर (ret)
			परत ret;
	}

	fdt_for_each_subnode(subnode, fdto, गाठ) {
		स्थिरांक अक्षर *name = fdt_get_name(fdto, subnode, रिक्त);
		पूर्णांक nnode;
		पूर्णांक ret;

		nnode = fdt_add_subnode(fdt, target, name);
		जर (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_offset(fdt, target, name);
			जर (nnode == -FDT_ERR_NOTFOUND)
				परत -FDT_ERR_INTERNAL;
		}

		जर (nnode < 0)
			परत nnode;

		ret = overlay_apply_गाठ(fdt, nnode, fdto, subnode);
		जर (ret)
			परत ret;
	}

	परत 0;
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
स्थिर पूर्णांक overlay_merge(शून्य *fdt, शून्य *fdto)
{
	पूर्णांक fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		पूर्णांक overlay;
		पूर्णांक target;
		पूर्णांक ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_offset(fdto, fragment, "__overlay__");
		जर (overlay == -FDT_ERR_NOTFOUND)
			सुरूठेवा;

		जर (overlay < 0)
			परत overlay;

		target = overlay_get_target(fdt, fdto, fragment, रिक्त);
		जर (target < 0)
			परत target;

		ret = overlay_apply_गाठ(fdt, target, fdto, overlay);
		जर (ret)
			परत ret;
	}

	परत 0;
}

स्थिर पूर्णांक get_path_len(स्थिरांक शून्य *fdt, पूर्णांक nodeoffset)
{
	पूर्णांक len = 0, namelen;
	स्थिरांक अक्षर *name;

	FDT_RO_PROBE(fdt);

	साठी (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		जर (!name)
			परत namelen;

		/* root? we're done */
		जर (namelen == 0)
			थांबा;

		nodeoffset = fdt_parent_offset(fdt, nodeoffset);
		जर (nodeoffset < 0)
			परत nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	जर (len == 0)
		len++;
	परत len;
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
स्थिर पूर्णांक overlay_चिन्ह_update(शून्य *fdt, शून्य *fdto)
{
	पूर्णांक मूळ_sym, ov_sym, prop, path_len, fragment, target;
	पूर्णांक len, frag_name_len, ret, rel_path_len;
	स्थिरांक अक्षर *s, *e;
	स्थिरांक अक्षर *path;
	स्थिरांक अक्षर *name;
	स्थिरांक अक्षर *frag_name;
	स्थिरांक अक्षर *rel_path;
	स्थिरांक अक्षर *target_path;
	अक्षर *buf;
	शून्य *p;

	ov_sym = fdt_subnode_offset(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	जर (ov_sym < 0)
		परत 0;

	मूळ_sym = fdt_subnode_offset(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	जर (मूळ_sym == -FDT_ERR_NOTFOUND)
		मूळ_sym = fdt_add_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	जर (मूळ_sym < 0)
		परत मूळ_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_offset(prop, fdto, ov_sym) {
		path = fdt_getprop_by_offset(fdto, prop, &name, &path_len);
		जर (!path)
			परत path_len;

		/* verify it's a string property (terminated by a single \0) */
		जर (path_len < 1 || स_प्रथम(path, '\0', path_len) != &path[path_len - 1])
			परत -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = path + path_len;

		जर (*path != '/')
			परत -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = म_अक्षर(path + 1, '/');
		जर (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			सुरूठेवा;
		}

		frag_name = path + 1;
		frag_name_len = s - path - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = आकार("/__overlay__/") - 1;
		जर ((e - s) > len && (स_भेद(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_path = s + len;
			rel_path_len = e - rel_path - 1;
		} अन्यथा जर ((e - s) == len
			   && (स_भेद(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_path = "";
			rel_path_len = 0;
		} अन्यथा {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			सुरूठेवा;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_offset_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		जर (ret < 0)
			परत -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_offset(fdto, fragment, "__overlay__");
		जर (ret < 0)
			परत -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_target(fdt, fdto, fragment, &target_path);
		जर (ret < 0)
			परत ret;
		target = ret;

		/* if we have a target path use */
		जर (!target_path) {
			ret = get_path_len(fdt, target);
			जर (ret < 0)
				परत ret;
			len = ret;
		} अन्यथा {
			len = म_माप(target_path);
		}

		ret = fdt_setprop_placeholder(fdt, मूळ_sym, name,
				len + (len > 1) + rel_path_len + 1, &p);
		जर (ret < 0)
			परत ret;

		जर (!target_path) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_target(fdt, fdto, fragment, &target_path);
			जर (ret < 0)
				परत ret;
			target = ret;
		}

		buf = p;
		जर (len > 1) { /* target is not root */
			जर (!target_path) {
				ret = fdt_get_path(fdt, target, buf, len + 1);
				जर (ret < 0)
					परत ret;
			} अन्यथा
				स_नकल(buf, target_path, len + 1);

		} अन्यथा
			len--;

		buf[len] = '/';
		स_नकल(buf + len + 1, rel_path, rel_path_len);
		buf[len + 1 + rel_path_len] = '\0';
	}

	परत 0;
}

पूर्णांक fdt_overlay_apply(शून्य *fdt, शून्य *fdto)
{
	uint32_t delta;
	पूर्णांक ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_find_max_phandle(fdt, &delta);
	जर (ret)
		जा err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	जर (ret)
		जा err;

	ret = overlay_update_local_references(fdto, delta);
	जर (ret)
		जा err;

	ret = overlay_fixup_phandles(fdt, fdto);
	जर (ret)
		जा err;

	ret = overlay_merge(fdt, fdto);
	जर (ret)
		जा err;

	ret = overlay_चिन्ह_update(fdt, fdto);
	जर (ret)
		जा err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_set_magic(fdto, ~0);

	परत 0;

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

	परत ret;
}
