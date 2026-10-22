/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#अन्तर्भाव <sbi/riscv_locks.h>
#अन्तर्भाव <sbi/sbi_error.h>
#अन्तर्भाव <sbi/sbi_heap.h>
#अन्तर्भाव <sbi/sbi_list.h>
#अन्तर्भाव <sbi/sbi_scratch.h>
#अन्तर्भाव <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#परिभाषय HEAP_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#परिभाषय HEAP_ग्रन्थिः_BATCH_आकार		8

संरचना heap_ग्रन्थिः {
	संरचना sbi_dlist शीर्ष;
	अचिह्नित दीर्घ addr;
	अचिह्नित दीर्घ आकार;
};

संरचना sbi_heap_control {
	spinlock_t तालकम्;
	अचिह्नित दीर्घ आधारः_2 GUARDED_BY(&तालकम्);
	अचिह्नित दीर्घ आकार GUARDED_BY(&तालकम्);
	अचिह्नित दीर्घ resv GUARDED_BY(&तालकम्);
	संरचना sbi_dlist free_ग्रन्थिः_list GUARDED_BY(&तालकम्);
	संरचना sbi_dlist free_space_list GUARDED_BY(&तालकम्);
	संरचना sbi_dlist प्रयुक्तम्_space_list GUARDED_BY(&तालकम्);
	संरचना heap_ग्रन्थिः init_free_space_ग्रन्थिः GUARDED_BY(&तालकम्);
};

संरचना sbi_heap_control global_hpctrl;

स्थिर bool alloc_nodes(संरचना sbi_heap_control *hpctrl) MUST_HOLD(&hpctrl->तालकम्)
{
	size_t आकार = HEAP_ग्रन्थिः_BATCH_आकार * परिमाण(संरचना heap_ग्रन्थिः);
	संरचना heap_ग्रन्थिः *n, *new = रिक्त;

	/* alloc_with_align() requires at most two free nodes */
	यदि (hpctrl->free_ग्रन्थिः_list.अग्र != hpctrl->free_ग्रन्थिः_list.पूर्व)
		प्रत्यागम true;

	sbi_list_for_each_प्रविष्टिः_reverse(n, &hpctrl->free_space_list, शीर्ष) {
		यदि (n->आकार >= आकार) {
			n->आकार -= आकार;
			यदि (!n->आकार) {
				sbi_list_del(&n->शीर्ष);
				sbi_list_सङ्कलय_पुच्छ(&n->शीर्ष, &hpctrl->free_ग्रन्थिः_list);
			}
			new = (शून्य *)(n->addr + n->आकार);
			भङ्ग;
		}
	}
	यदि (!new)
		प्रत्यागम false;

	प्रति (size_t i = 0; i < HEAP_ग्रन्थिः_BATCH_आकार; i++)
		sbi_list_सङ्कलय_पुच्छ(&new[i].शीर्ष, &hpctrl->free_ग्रन्थिः_list);
	hpctrl->resv += आकार;

	प्रत्यागम true;
}

स्थिर शून्य *alloc_with_align(संरचना sbi_heap_control *hpctrl, size_t align,
			      size_t आकार)
	MUST_NOT_HOLD(&hpctrl->तालकम्)
{
	शून्य *ret = रिक्त;
	संरचना heap_ग्रन्थिः *n, *np;
	अचिह्नित दीर्घ निम्नतमम्_aligned;
	size_t pad;

	यदि (!आकार)
		प्रत्यागम रिक्त;

	आकार += align - 1;
	आकार &= ~((अचिह्नित दीर्घ)align - 1);

	spin_तालकम्(&hpctrl->तालकम्);

	/* Ensure at least two free nodes are available for use below */
	यदि (!alloc_nodes(hpctrl))
		गच्छ बहिर्गमनम्;

	np = रिक्त;
	sbi_list_for_each_प्रविष्टिः(n, &hpctrl->free_space_list, शीर्ष) {
		निम्नतमम्_aligned = ROUNDUP(n->addr, align);
		pad = निम्नतमम्_aligned - n->addr;

		यदि (आकार + pad <= n->आकार) {
			np = n;
			भङ्ग;
		}
	}
	यदि (!np)
		गच्छ बहिर्गमनम्;

	यदि (pad) {
		n = sbi_list_first_प्रविष्टिः(&hpctrl->free_ग्रन्थिः_list,
					 संरचना heap_ग्रन्थिः, शीर्ष);
		sbi_list_del(&n->शीर्ष);

		n->addr = np->addr;
		n->आकार = pad;
		sbi_list_सङ्कलय_पुच्छ(&n->शीर्ष, &np->शीर्ष);

		np->addr += pad;
		np->आकार -= pad;
	}

	यदि (आकार < np->आकार) {
		n = sbi_list_first_प्रविष्टिः(&hpctrl->free_ग्रन्थिः_list,
					 संरचना heap_ग्रन्थिः, शीर्ष);
		sbi_list_del(&n->शीर्ष);

		n->addr = np->addr + आकार;
		n->आकार = np->आकार - आकार;
		sbi_list_सङ्कलय(&n->शीर्ष, &np->शीर्ष);

		np->आकार = आकार;
	}

	sbi_list_del(&np->शीर्ष);
	sbi_list_सङ्कलय_पुच्छ(&np->शीर्ष, &hpctrl->प्रयुक्तम्_space_list);
	ret = (शून्य *)np->addr;

बहिर्गमनम्:
	spin_unlock(&hpctrl->तालकम्);

	प्रत्यागम ret;
}

शून्य *sbi_malloc_from(संरचना sbi_heap_control *hpctrl, size_t आकार)
{
	प्रत्यागम alloc_with_align(hpctrl, HEAP_ALLOC_ALIGN, आकार);
}

शून्य *sbi_aligned_alloc_from(संरचना sbi_heap_control *hpctrl,
			     size_t alignment, size_t आकार)
{
	यदि (alignment < HEAP_ALLOC_ALIGN)
		alignment = HEAP_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	यदि ((alignment & (alignment - 1)) != 0)
		प्रत्यागम रिक्त;

	/* Make sure size is multiple of alignment */
	यदि (आकार % alignment != 0)
		प्रत्यागम रिक्त;

	प्रत्यागम alloc_with_align(hpctrl, alignment, आकार);
}

शून्य *sbi_zalloc_from(संरचना sbi_heap_control *hpctrl, size_t आकार)
{
	शून्य *ret = sbi_malloc_from(hpctrl, आकार);

	यदि (ret)
		sbi_memset(ret, 0, आकार);
	प्रत्यागम ret;
}

शून्य sbi_free_from(संरचना sbi_heap_control *hpctrl, शून्य *ptr)
{
	संरचना heap_ग्रन्थिः *n, *np;

	यदि (!ptr)
		प्रत्यागम;

	spin_तालकम्(&hpctrl->तालकम्);

	np = रिक्त;
	sbi_list_for_each_प्रविष्टिः(n, &hpctrl->प्रयुक्तम्_space_list, शीर्ष) {
		यदि ((n->addr <= (अचिह्नित दीर्घ)ptr) &&
		    ((अचिह्नित दीर्घ)ptr < (n->addr + n->आकार))) {
			np = n;
			भङ्ग;
		}
	}
	यदि (!np) {
		spin_unlock(&hpctrl->तालकम्);
		प्रत्यागम;
	}

	sbi_list_del(&np->शीर्ष);

	sbi_list_for_each_प्रविष्टिः(n, &hpctrl->free_space_list, शीर्ष) {
		यदि ((np->addr + np->आकार) == n->addr) {
			n->addr = np->addr;
			n->आकार += np->आकार;
			sbi_list_सङ्कलय_पुच्छ(&np->शीर्ष, &hpctrl->free_ग्रन्थिः_list);
			np = रिक्त;
			भङ्ग;
		} अन्यथा यदि (np->addr == (n->addr + n->आकार)) {
			n->आकार += np->आकार;
			sbi_list_सङ्कलय_पुच्छ(&np->शीर्ष, &hpctrl->free_ग्रन्थिः_list);
			np = रिक्त;
			भङ्ग;
		} अन्यथा यदि ((n->addr + n->आकार) < np->addr) {
			sbi_list_सङ्कलय(&np->शीर्ष, &n->शीर्ष);
			np = रिक्त;
			भङ्ग;
		}
	}
	यदि (np)
		sbi_list_सङ्कलय_पुच्छ(&np->शीर्ष, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->तालकम्);
}

अचिह्नित दीर्घ sbi_heap_free_space_from(संरचना sbi_heap_control *hpctrl)
{
	संरचना heap_ग्रन्थिः *n;
	अचिह्नित दीर्घ ret = 0;

	spin_तालकम्(&hpctrl->तालकम्);
	sbi_list_for_each_प्रविष्टिः(n, &hpctrl->free_space_list, शीर्ष)
		ret += n->आकार;
	spin_unlock(&hpctrl->तालकम्);

	प्रत्यागम ret;
}

अचिह्नित दीर्घ sbi_heap_प्रयुक्तम्_space_from(संरचना sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	प्रत्यागम hpctrl->आकार - hpctrl->resv - sbi_heap_free_space();
}

अचिह्नित दीर्घ sbi_heap_reserved_space_from(संरचना sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	प्रत्यागम hpctrl->resv;
}

पूर्णाङ्क sbi_heap_init_new(संरचना sbi_heap_control *hpctrl, अचिह्नित दीर्घ आधारः_2,
		      अचिह्नित दीर्घ आकार) NO_THREAD_SAFETY_ANALYSIS
{
	संरचना heap_ग्रन्थिः *n;

	/* Initialize heap control */
	SPIN_तालकम्_INIT(hpctrl->तालकम्);
	hpctrl->आधारः_2 = आधारः_2;
	hpctrl->आकार = आकार;
	hpctrl->resv = 0;
	SBI_INIT_LIST_शीर्ष(&hpctrl->free_ग्रन्थिः_list);
	SBI_INIT_LIST_शीर्ष(&hpctrl->free_space_list);
	SBI_INIT_LIST_शीर्ष(&hpctrl->प्रयुक्तम्_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_ग्रन्थिः;
	n->addr = आधारः_2;
	n->आकार = आकार;
	sbi_list_सङ्कलय_पुच्छ(&n->शीर्ष, &hpctrl->free_space_list);

	प्रत्यागम 0;
}

पूर्णाङ्क sbi_heap_init(संरचना sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	यदि (!scratch->fw_heap_आकार ||
	    (scratch->fw_heap_आकार & (HEAP_आधारः_ALIGN - 1)) ||
	    (scratch->fw_heap_विस्थापनम् < scratch->fw_rw_विस्थापनम्) ||
	    (scratch->fw_आकार < (scratch->fw_heap_विस्थापनम् + scratch->fw_heap_आकार)) ||
	    (scratch->fw_heap_विस्थापनम् & (HEAP_आधारः_ALIGN - 1)))
		प्रत्यागम SBI_EINVAL;

	प्रत्यागम sbi_heap_init_new(&global_hpctrl,
				  scratch->fw_start + scratch->fw_heap_विस्थापनम्,
				  scratch->fw_heap_आकार);
}

पूर्णाङ्क sbi_heap_alloc_new(संरचना sbi_heap_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, परिमाण(संरचना sbi_heap_control));
	प्रत्यागम 0;
}
