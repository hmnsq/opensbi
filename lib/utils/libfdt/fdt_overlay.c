// SPDX-License-Identifier: GPL-2.0-or-later OR BSD-2-Clause
/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2016 Free Electrons
 * Copyright (C) 2016 NextThing Co.
 */
#అన్తర్భావ "libfdt_env.h"

#అన్తర్భావ <fdt.h>
#అన్తర్భావ <libfdt.h>

#అన్తర్భావ "libfdt_internal.h"

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
స్థిర uint32_t overlay_get_లక్ష్యం_phandle(స్థిరరాశి శూన్య *fdto, పూర్ణాంకం fragment)
{
	స్థిరరాశి fdt32_t *val;
	పూర్ణాంకం len;

	val = fdt_getprop(fdto, fragment, "target", &len);
	ఒకవేళ (!val)
		తిరిగి 0;

	ఒకవేళ ((len != పరిమాణం(*val)) || (fdt32_గమ్యం_cpu(*val) == (uint32_t)-1))
		తిరిగి (uint32_t)-1;

	తిరిగి fdt32_గమ్యం_cpu(*val);
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
స్థిర పూర్ణాంకం overlay_get_లక్ష్యం(స్థిరరాశి శూన్య *fdt, స్థిరరాశి శూన్య *fdto,
			      పూర్ణాంకం fragment, అక్షరం స్థిరరాశి **pathp)
{
	uint32_t phandle;
	స్థిరరాశి అక్షరం *మార్గం = ఖాళీ;
	పూర్ణాంకం మార్గం_len = 0, ret;

	/* Try first to do a phandle based lookup */
	phandle = overlay_get_లక్ష్యం_phandle(fdto, fragment);
	ఒకవేళ (phandle == (uint32_t)-1)
		తిరిగి -FDT_ERR_BADPHANDLE;

	/* no phandle, try path */
	ఒకవేళ (!phandle) {
		/* And then a path based lookup */
		మార్గం = fdt_getprop(fdto, fragment, "target-path", &మార్గం_len);
		ఒకవేళ (మార్గం)
			ret = fdt_మార్గం_స్థానభ్రంశం(fdt, మార్గం);
		లేకపోతే
			ret = మార్గం_len;
	} లేకపోతే
		ret = fdt_కణుపు_స్థానభ్రంశం_by_phandle(fdt, phandle);

	/*
	* If we haven't found either a target or a
	* target-path property in a node that contains a
	* __overlay__ subnode (we wouldn't be called
	* otherwise), consider it a improperly written
	* overlay
	*/
	ఒకవేళ (ret < 0 && మార్గం_len == -FDT_ERR_NOTFOUND)
		ret = -FDT_ERR_BADOVERLAY;

	/* return on error */
	ఒకవేళ (ret < 0)
		తిరిగి ret;

	/* return pointer to path (if available) */
	ఒకవేళ (pathp)
		*pathp = మార్గం ? మార్గం : ఖాళీ;

	తిరిగి ret;
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
స్థిర పూర్ణాంకం overlay_phandle_కూడు_స్థానభ్రంశం(శూన్య *fdt, పూర్ణాంకం కణుపు,
				      స్థిరరాశి అక్షరం *name, uint32_t delta)
{
	స్థిరరాశి fdt32_t *val;
	uint32_t adj_val;
	పూర్ణాంకం len;

	val = fdt_getprop(fdt, కణుపు, name, &len);
	ఒకవేళ (!val)
		తిరిగి len;

	ఒకవేళ (len != పరిమాణం(*val))
		తిరిగి -FDT_ERR_BADPHANDLE;

	adj_val = fdt32_గమ్యం_cpu(*val);
	ఒకవేళ ((adj_val + delta) < adj_val)
		తిరిగి -FDT_ERR_NOPHANDLES;

	adj_val += delta;
	ఒకవేళ (adj_val == (uint32_t)-1)
		తిరిగి -FDT_ERR_NOPHANDLES;

	తిరిగి fdt_setprop_inplace_u32(fdt, కణుపు, name, adj_val);
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
స్థిర పూర్ణాంకం overlay_adjust_కణుపు_phandles(శూన్య *fdto, పూర్ణాంకం కణుపు,
					uint32_t delta)
{
	పూర్ణాంకం బిడ్డ;
	పూర్ణాంకం ret;

	ret = overlay_phandle_కూడు_స్థానభ్రంశం(fdto, కణుపు, "phandle", delta);
	ఒకవేళ (ret && ret != -FDT_ERR_NOTFOUND)
		తిరిగి ret;

	ret = overlay_phandle_కూడు_స్థానభ్రంశం(fdto, కణుపు, "linux,phandle", delta);
	ఒకవేళ (ret && ret != -FDT_ERR_NOTFOUND)
		తిరిగి ret;

	fdt_for_each_subnode(బిడ్డ, fdto, కణుపు) {
		ret = overlay_adjust_కణుపు_phandles(fdto, బిడ్డ, delta);
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	తిరిగి 0;
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
స్థిర పూర్ణాంకం overlay_adjust_local_phandles(శూన్య *fdto, uint32_t delta)
{
	/*
	 * Start adjusting the phandles from the overlay root
	 */
	తిరిగి overlay_adjust_కణుపు_phandles(fdto, 0, delta);
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
స్థిర పూర్ణాంకం overlay_update_local_కణుపు_references(శూన్య *fdto,
						పూర్ణాంకం వృక్ష_కణుపు,
						పూర్ణాంకం fixup_కణుపు,
						uint32_t delta)
{
	పూర్ణాంకం fixup_prop;
	పూర్ణాంకం fixup_బిడ్డ;
	పూర్ణాంకం ret;

	fdt_for_each_property_స్థానభ్రంశం(fixup_prop, fdto, fixup_కణుపు) {
		స్థిరరాశి fdt32_t *fixup_val;
		స్థిరరాశి అక్షరం *వృక్షం_val;
		స్థిరరాశి అక్షరం *name;
		పూర్ణాంకం fixup_len;
		పూర్ణాంకం వృక్షం_len;
		పూర్ణాంకం i;

		fixup_val = fdt_getprop_by_స్థానభ్రంశం(fdto, fixup_prop,
						  &name, &fixup_len);
		ఒకవేళ (!fixup_val)
			తిరిగి fixup_len;

		ఒకవేళ (fixup_len % పరిమాణం(uint32_t))
			తిరిగి -FDT_ERR_BADOVERLAY;
		fixup_len /= పరిమాణం(uint32_t);

		వృక్షం_val = fdt_getprop(fdto, వృక్ష_కణుపు, name, &వృక్షం_len);
		ఒకవేళ (!వృక్షం_val) {
			ఒకవేళ (వృక్షం_len == -FDT_ERR_NOTFOUND)
				తిరిగి -FDT_ERR_BADOVERLAY;

			తిరిగి వృక్షం_len;
		}

		కొరకు (i = 0; i < fixup_len; i++) {
			fdt32_t adj_val;
			uint32_t poffset;

			poffset = fdt32_గమ్యం_cpu(fixup_val[i]);

			/*
			 * phandles to fixup can be unaligned.
			 *
			 * Use a memcpy for the architectures that do
			 * not support unaligned accesses.
			 */
			జ్ఞాపకం_నకలు(&adj_val, వృక్షం_val + poffset, పరిమాణం(adj_val));

			adj_val = cpu_గమ్యం_fdt32(fdt32_గమ్యం_cpu(adj_val) + delta);

			ret = fdt_setprop_inplace_namelen_partial(fdto,
								  వృక్ష_కణుపు,
								  name,
								  అక్షరశ్రేణి_పొడవు(name),
								  poffset,
								  &adj_val,
								  పరిమాణం(adj_val));
			ఒకవేళ (ret == -FDT_ERR_NOSPACE)
				తిరిగి -FDT_ERR_BADOVERLAY;

			ఒకవేళ (ret)
				తిరిగి ret;
		}
	}

	fdt_for_each_subnode(fixup_బిడ్డ, fdto, fixup_కణుపు) {
		స్థిరరాశి అక్షరం *fixup_బిడ్డ_name = fdt_get_name(fdto, fixup_బిడ్డ,
							    ఖాళీ);
		పూర్ణాంకం వృక్షం_బిడ్డ;

		వృక్షం_బిడ్డ = fdt_subnode_స్థానభ్రంశం(fdto, వృక్ష_కణుపు,
						fixup_బిడ్డ_name);
		ఒకవేళ (వృక్షం_బిడ్డ == -FDT_ERR_NOTFOUND)
			తిరిగి -FDT_ERR_BADOVERLAY;
		ఒకవేళ (వృక్షం_బిడ్డ < 0)
			తిరిగి వృక్షం_బిడ్డ;

		ret = overlay_update_local_కణుపు_references(fdto,
							   వృక్షం_బిడ్డ,
							   fixup_బిడ్డ,
							   delta);
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	తిరిగి 0;
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
స్థిర పూర్ణాంకం overlay_update_local_references(శూన్య *fdto, uint32_t delta)
{
	పూర్ణాంకం fixups;

	fixups = fdt_మార్గం_స్థానభ్రంశం(fdto, "/__local_fixups__");
	ఒకవేళ (fixups < 0) {
		/* There's no local phandles to adjust, bail out */
		ఒకవేళ (fixups == -FDT_ERR_NOTFOUND)
			తిరిగి 0;

		తిరిగి fixups;
	}

	/*
	 * Update our local references from the root of the tree
	 */
	తిరిగి overlay_update_local_కణుపు_references(fdto, 0, fixups,
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
స్థిర పూర్ణాంకం overlay_fixup_one_phandle(శూన్య *fdt, శూన్య *fdto,
				     పూర్ణాంకం symbols_off,
				     స్థిరరాశి అక్షరం *మార్గం, uint32_t మార్గం_len,
				     స్థిరరాశి అక్షరం *name, uint32_t name_len,
				     పూర్ణాంకం poffset, స్థిరరాశి అక్షరం *label)
{
	స్థిరరాశి అక్షరం *చిహ్నం_మార్గం;
	uint32_t phandle;
	fdt32_t phandle_prop;
	పూర్ణాంకం చిహ్నం_off, fixup_off;
	పూర్ణాంకం prop_len;

	ఒకవేళ (symbols_off < 0)
		తిరిగి symbols_off;

	చిహ్నం_మార్గం = fdt_getprop(fdt, symbols_off, label,
				  &prop_len);
	ఒకవేళ (!చిహ్నం_మార్గం)
		తిరిగి prop_len;

	చిహ్నం_off = fdt_మార్గం_స్థానభ్రంశం(fdt, చిహ్నం_మార్గం);
	ఒకవేళ (చిహ్నం_off < 0)
		తిరిగి చిహ్నం_off;

	phandle = fdt_get_phandle(fdt, చిహ్నం_off);
	ఒకవేళ (!phandle)
		తిరిగి -FDT_ERR_NOTFOUND;

	fixup_off = fdt_మార్గం_స్థానభ్రంశం_namelen(fdto, మార్గం, మార్గం_len);
	ఒకవేళ (fixup_off == -FDT_ERR_NOTFOUND)
		తిరిగి -FDT_ERR_BADOVERLAY;
	ఒకవేళ (fixup_off < 0)
		తిరిగి fixup_off;

	phandle_prop = cpu_గమ్యం_fdt32(phandle);
	తిరిగి fdt_setprop_inplace_namelen_partial(fdto, fixup_off,
						   name, name_len, poffset,
						   &phandle_prop,
						   పరిమాణం(phandle_prop));
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
స్థిర పూర్ణాంకం overlay_fixup_phandle(శూన్య *fdt, శూన్య *fdto, పూర్ణాంకం symbols_off,
				 పూర్ణాంకం property)
{
	స్థిరరాశి అక్షరం *విలువ;
	స్థిరరాశి అక్షరం *label;
	పూర్ణాంకం len;

	విలువ = fdt_getprop_by_స్థానభ్రంశం(fdto, property,
				      &label, &len);
	ఒకవేళ (!విలువ) {
		ఒకవేళ (len == -FDT_ERR_NOTFOUND)
			తిరిగి -FDT_ERR_అంతర్గత;

		తిరిగి len;
	}

	చేయి {
		స్థిరరాశి అక్షరం *మార్గం, *name, *fixup_చివర;
		స్థిరరాశి అక్షరం *fixup_str = విలువ;
		uint32_t మార్గం_len, name_len;
		uint32_t fixup_len;
		అక్షరం *sep, *endptr;
		పూర్ణాంకం poffset, ret;

		fixup_చివర = memchr(విలువ, '\0', len);
		ఒకవేళ (!fixup_చివర)
			తిరిగి -FDT_ERR_BADOVERLAY;
		fixup_len = fixup_చివర - fixup_str;

		len -= fixup_len + 1;
		విలువ += fixup_len + 1;

		మార్గం = fixup_str;
		sep = memchr(fixup_str, ':', fixup_len);
		ఒకవేళ (!sep || *sep != ':')
			తిరిగి -FDT_ERR_BADOVERLAY;

		మార్గం_len = sep - మార్గం;
		ఒకవేళ (మార్గం_len == (fixup_len - 1))
			తిరిగి -FDT_ERR_BADOVERLAY;

		fixup_len -= మార్గం_len + 1;
		name = sep + 1;
		sep = memchr(name, ':', fixup_len);
		ఒకవేళ (!sep || *sep != ':')
			తిరిగి -FDT_ERR_BADOVERLAY;

		name_len = sep - name;
		ఒకవేళ (!name_len)
			తిరిగి -FDT_ERR_BADOVERLAY;

		poffset = strtoul(sep + 1, &endptr, 10);
		ఒకవేళ ((*endptr != '\0') || (endptr <= (sep + 1)))
			తిరిగి -FDT_ERR_BADOVERLAY;

		ret = overlay_fixup_one_phandle(fdt, fdto, symbols_off,
						మార్గం, మార్గం_len, name, name_len,
						poffset, label);
		ఒకవేళ (ret)
			తిరిగి ret;
	} అయితే (len > 0);

	తిరిగి 0;
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
స్థిర పూర్ణాంకం overlay_fixup_phandles(శూన్య *fdt, శూన్య *fdto)
{
	పూర్ణాంకం fixups_off, symbols_off;
	పూర్ణాంకం property;

	/* We can have overlays without any fixups */
	fixups_off = fdt_మార్గం_స్థానభ్రంశం(fdto, "/__fixups__");
	ఒకవేళ (fixups_off == -FDT_ERR_NOTFOUND)
		తిరిగి 0; /* nothing to do */
	ఒకవేళ (fixups_off < 0)
		తిరిగి fixups_off;

	/* And base DTs without symbols */
	symbols_off = fdt_మార్గం_స్థానభ్రంశం(fdt, "/__symbols__");
	ఒకవేళ ((symbols_off < 0 && (symbols_off != -FDT_ERR_NOTFOUND)))
		తిరిగి symbols_off;

	fdt_for_each_property_స్థానభ్రంశం(property, fdto, fixups_off) {
		పూర్ణాంకం ret;

		ret = overlay_fixup_phandle(fdt, fdto, symbols_off, property);
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	తిరిగి 0;
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
స్థిర పూర్ణాంకం overlay_apply_కణుపు(శూన్య *fdt, పూర్ణాంకం లక్ష్యం,
			      శూన్య *fdto, పూర్ణాంకం కణుపు)
{
	పూర్ణాంకం property;
	పూర్ణాంకం subnode;

	fdt_for_each_property_స్థానభ్రంశం(property, fdto, కణుపు) {
		స్థిరరాశి అక్షరం *name;
		స్థిరరాశి శూన్య *prop;
		పూర్ణాంకం prop_len;
		పూర్ణాంకం ret;

		prop = fdt_getprop_by_స్థానభ్రంశం(fdto, property, &name,
					     &prop_len);
		ఒకవేళ (prop_len == -FDT_ERR_NOTFOUND)
			తిరిగి -FDT_ERR_అంతర్గత;
		ఒకవేళ (prop_len < 0)
			తిరిగి prop_len;

		ret = fdt_setprop(fdt, లక్ష్యం, name, prop, prop_len);
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	fdt_for_each_subnode(subnode, fdto, కణుపు) {
		స్థిరరాశి అక్షరం *name = fdt_get_name(fdto, subnode, ఖాళీ);
		పూర్ణాంకం nnode;
		పూర్ణాంకం ret;

		nnode = fdt_కూడు_subnode(fdt, లక్ష్యం, name);
		ఒకవేళ (nnode == -FDT_ERR_EXISTS) {
			nnode = fdt_subnode_స్థానభ్రంశం(fdt, లక్ష్యం, name);
			ఒకవేళ (nnode == -FDT_ERR_NOTFOUND)
				తిరిగి -FDT_ERR_అంతర్గత;
		}

		ఒకవేళ (nnode < 0)
			తిరిగి nnode;

		ret = overlay_apply_కణుపు(fdt, nnode, fdto, subnode);
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	తిరిగి 0;
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
స్థిర పూర్ణాంకం overlay_విలీనం(శూన్య *fdt, శూన్య *fdto)
{
	పూర్ణాంకం fragment;

	fdt_for_each_subnode(fragment, fdto, 0) {
		పూర్ణాంకం overlay;
		పూర్ణాంకం లక్ష్యం;
		పూర్ణాంకం ret;

		/*
		 * Each fragments will have an __overlay__ node. If
		 * they don't, it's not supposed to be merged
		 */
		overlay = fdt_subnode_స్థానభ్రంశం(fdto, fragment, "__overlay__");
		ఒకవేళ (overlay == -FDT_ERR_NOTFOUND)
			కొనసాగు;

		ఒకవేళ (overlay < 0)
			తిరిగి overlay;

		లక్ష్యం = overlay_get_లక్ష్యం(fdt, fdto, fragment, ఖాళీ);
		ఒకవేళ (లక్ష్యం < 0)
			తిరిగి లక్ష్యం;

		ret = overlay_apply_కణుపు(fdt, లక్ష్యం, fdto, overlay);
		ఒకవేళ (ret)
			తిరిగి ret;
	}

	తిరిగి 0;
}

స్థిర పూర్ణాంకం get_మార్గం_len(స్థిరరాశి శూన్య *fdt, పూర్ణాంకం nodeoffset)
{
	పూర్ణాంకం len = 0, namelen;
	స్థిరరాశి అక్షరం *name;

	FDT_RO_PROBE(fdt);

	కొరకు (;;) {
		name = fdt_get_name(fdt, nodeoffset, &namelen);
		ఒకవేళ (!name)
			తిరిగి namelen;

		/* root? we're done */
		ఒకవేళ (namelen == 0)
			ఆపు;

		nodeoffset = fdt_మాతృ_స్థానభ్రంశం(fdt, nodeoffset);
		ఒకవేళ (nodeoffset < 0)
			తిరిగి nodeoffset;
		len += namelen + 1;
	}

	/* in case of root pretend it's "/" */
	ఒకవేళ (len == 0)
		len++;
	తిరిగి len;
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
స్థిర పూర్ణాంకం overlay_చిహ్నం_update(శూన్య *fdt, శూన్య *fdto)
{
	పూర్ణాంకం మూలం_sym, ov_sym, prop, మార్గం_len, fragment, లక్ష్యం;
	పూర్ణాంకం len, frag_name_len, ret, rel_మార్గం_len;
	స్థిరరాశి అక్షరం *s, *e;
	స్థిరరాశి అక్షరం *మార్గం;
	స్థిరరాశి అక్షరం *name;
	స్థిరరాశి అక్షరం *frag_name;
	స్థిరరాశి అక్షరం *rel_మార్గం;
	స్థిరరాశి అక్షరం *లక్ష్యం_మార్గం;
	అక్షరం *buf;
	శూన్య *p;

	ov_sym = fdt_subnode_స్థానభ్రంశం(fdto, 0, "__symbols__");

	/* if no overlay symbols exist no problem */
	ఒకవేళ (ov_sym < 0)
		తిరిగి 0;

	మూలం_sym = fdt_subnode_స్థానభ్రంశం(fdt, 0, "__symbols__");

	/* it no root symbols exist we should create them */
	ఒకవేళ (మూలం_sym == -FDT_ERR_NOTFOUND)
		మూలం_sym = fdt_కూడు_subnode(fdt, 0, "__symbols__");

	/* any error is fatal now */
	ఒకవేళ (మూలం_sym < 0)
		తిరిగి మూలం_sym;

	/* iterate over each overlay symbol */
	fdt_for_each_property_స్థానభ్రంశం(prop, fdto, ov_sym) {
		మార్గం = fdt_getprop_by_స్థానభ్రంశం(fdto, prop, &name, &మార్గం_len);
		ఒకవేళ (!మార్గం)
			తిరిగి మార్గం_len;

		/* verify it's a string property (terminated by a single \0) */
		ఒకవేళ (మార్గం_len < 1 || memchr(మార్గం, '\0', మార్గం_len) != &మార్గం[మార్గం_len - 1])
			తిరిగి -FDT_ERR_BADVALUE;

		/* keep end marker to avoid strlen() */
		e = మార్గం + మార్గం_len;

		ఒకవేళ (*మార్గం != '/')
			తిరిగి -FDT_ERR_BADVALUE;

		/* get fragment name first */
		s = strchr(మార్గం + 1, '/');
		ఒకవేళ (!s) {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			కొనసాగు;
		}

		frag_name = మార్గం + 1;
		frag_name_len = s - మార్గం - 1;

		/* verify format; safe since "s" lies in \0 terminated prop */
		len = పరిమాణం("/__overlay__/") - 1;
		ఒకవేళ ((e - s) > len && (memcmp(s, "/__overlay__/", len) == 0)) {
			/* /<fragment-name>/__overlay__/<relative-subnode-path> */
			rel_మార్గం = s + len;
			rel_మార్గం_len = e - rel_మార్గం - 1;
		} లేకపోతే ఒకవేళ ((e - s) == len
			   && (memcmp(s, "/__overlay__", len - 1) == 0)) {
			/* /<fragment-name>/__overlay__ */
			rel_మార్గం = "";
			rel_మార్గం_len = 0;
		} లేకపోతే {
			/* Symbol refers to something that won't end
			 * up in the target tree */
			కొనసాగు;
		}

		/* find the fragment index in which the symbol lies */
		ret = fdt_subnode_స్థానభ్రంశం_namelen(fdto, 0, frag_name,
					       frag_name_len);
		/* not found? */
		ఒకవేళ (ret < 0)
			తిరిగి -FDT_ERR_BADOVERLAY;
		fragment = ret;

		/* an __overlay__ subnode must exist */
		ret = fdt_subnode_స్థానభ్రంశం(fdto, fragment, "__overlay__");
		ఒకవేళ (ret < 0)
			తిరిగి -FDT_ERR_BADOVERLAY;

		/* get the target of the fragment */
		ret = overlay_get_లక్ష్యం(fdt, fdto, fragment, &లక్ష్యం_మార్గం);
		ఒకవేళ (ret < 0)
			తిరిగి ret;
		లక్ష్యం = ret;

		/* if we have a target path use */
		ఒకవేళ (!లక్ష్యం_మార్గం) {
			ret = get_మార్గం_len(fdt, లక్ష్యం);
			ఒకవేళ (ret < 0)
				తిరిగి ret;
			len = ret;
		} లేకపోతే {
			len = అక్షరశ్రేణి_పొడవు(లక్ష్యం_మార్గం);
		}

		ret = fdt_setprop_placeholder(fdt, మూలం_sym, name,
				len + (len > 1) + rel_మార్గం_len + 1, &p);
		ఒకవేళ (ret < 0)
			తిరిగి ret;

		ఒకవేళ (!లక్ష్యం_మార్గం) {
			/* again in case setprop_placeholder changed it */
			ret = overlay_get_లక్ష్యం(fdt, fdto, fragment, &లక్ష్యం_మార్గం);
			ఒకవేళ (ret < 0)
				తిరిగి ret;
			లక్ష్యం = ret;
		}

		buf = p;
		ఒకవేళ (len > 1) { /* target is not root */
			ఒకవేళ (!లక్ష్యం_మార్గం) {
				ret = fdt_get_మార్గం(fdt, లక్ష్యం, buf, len + 1);
				ఒకవేళ (ret < 0)
					తిరిగి ret;
			} లేకపోతే
				జ్ఞాపకం_నకలు(buf, లక్ష్యం_మార్గం, len + 1);

		} లేకపోతే
			len--;

		buf[len] = '/';
		జ్ఞాపకం_నకలు(buf + len + 1, rel_మార్గం, rel_మార్గం_len);
		buf[len + 1 + rel_మార్గం_len] = '\0';
	}

	తిరిగి 0;
}

పూర్ణాంకం fdt_overlay_apply(శూన్య *fdt, శూన్య *fdto)
{
	uint32_t delta;
	పూర్ణాంకం ret;

	FDT_RO_PROBE(fdt);
	FDT_RO_PROBE(fdto);

	ret = fdt_కనుగొను_max_phandle(fdt, &delta);
	ఒకవేళ (ret)
		వెళ్ళు err;

	ret = overlay_adjust_local_phandles(fdto, delta);
	ఒకవేళ (ret)
		వెళ్ళు err;

	ret = overlay_update_local_references(fdto, delta);
	ఒకవేళ (ret)
		వెళ్ళు err;

	ret = overlay_fixup_phandles(fdt, fdto);
	ఒకవేళ (ret)
		వెళ్ళు err;

	ret = overlay_విలీనం(fdt, fdto);
	ఒకవేళ (ret)
		వెళ్ళు err;

	ret = overlay_చిహ్నం_update(fdt, fdto);
	ఒకవేళ (ret)
		వెళ్ళు err;

	/*
	 * The overlay has been damaged, erase its magic.
	 */
	fdt_సమితి_magic(fdto, ~0);

	తిరిగి 0;

err:
	/*
	 * The overlay might have been damaged, erase its magic.
	 */
	fdt_సమితి_magic(fdto, ~0);

	/*
	 * The base device tree might have been damaged, erase its
	 * magic.
	 */
	fdt_సమితి_magic(fdt, ~0);

	తిరిగి ret;
}
