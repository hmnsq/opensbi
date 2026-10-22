/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel<apatel@ventanamicro.com>
 */

#समावेश <sbi/riscv_locks.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_list.h>
#समावेश <sbi/sbi_scratch.h>
#समावेश <sbi/sbi_string.h>

/* Minimum size and alignment of heap allocations */
#घोषणा HEAP_ALLOC_ALIGN		64

/* Number of heap nodes to allocate at once */
#घोषणा HEAP_गाँठ_BATCH_आकार		8

काष्ठा heap_गाँठ {
	काष्ठा sbi_dlist शीर्ष;
	अचिन्हित दीर्घ addr;
	अचिन्हित दीर्घ आकार;
};

काष्ठा sbi_heap_control {
	spinlock_t lock;
	अचिन्हित दीर्घ base GUARDED_BY(&lock);
	अचिन्हित दीर्घ आकार GUARDED_BY(&lock);
	अचिन्हित दीर्घ resv GUARDED_BY(&lock);
	काष्ठा sbi_dlist free_गाँठ_list GUARDED_BY(&lock);
	काष्ठा sbi_dlist free_space_list GUARDED_BY(&lock);
	काष्ठा sbi_dlist used_space_list GUARDED_BY(&lock);
	काष्ठा heap_गाँठ init_free_space_गाँठ GUARDED_BY(&lock);
};

काष्ठा sbi_heap_control global_hpctrl;

अटल bool alloc_nodes(काष्ठा sbi_heap_control *hpctrl) MUST_HOLD(&hpctrl->lock)
{
	माप_प्रकार आकार = HEAP_गाँठ_BATCH_आकार * माप(काष्ठा heap_गाँठ);
	काष्ठा heap_गाँठ *n, *new = शून्य;

	/* alloc_with_align() requires at most two free nodes */
	अगर (hpctrl->free_गाँठ_list.अग्र != hpctrl->free_गाँठ_list.पूर्व)
		वापस true;

	sbi_list_for_each_entry_reverse(n, &hpctrl->free_space_list, शीर्ष) {
		अगर (n->आकार >= आकार) {
			n->आकार -= आकार;
			अगर (!n->आकार) {
				sbi_list_del(&n->शीर्ष);
				sbi_list_add_पुच्छ(&n->शीर्ष, &hpctrl->free_गाँठ_list);
			}
			new = (व्योम *)(n->addr + n->आकार);
			अवरोध;
		}
	}
	अगर (!new)
		वापस false;

	क्रम (माप_प्रकार i = 0; i < HEAP_गाँठ_BATCH_आकार; i++)
		sbi_list_add_पुच्छ(&new[i].शीर्ष, &hpctrl->free_गाँठ_list);
	hpctrl->resv += आकार;

	वापस true;
}

अटल व्योम *alloc_with_align(काष्ठा sbi_heap_control *hpctrl, माप_प्रकार align,
			      माप_प्रकार आकार)
	MUST_NOT_HOLD(&hpctrl->lock)
{
	व्योम *ret = शून्य;
	काष्ठा heap_गाँठ *n, *np;
	अचिन्हित दीर्घ lowest_aligned;
	माप_प्रकार pad;

	अगर (!आकार)
		वापस शून्य;

	आकार += align - 1;
	आकार &= ~((अचिन्हित दीर्घ)align - 1);

	spin_lock(&hpctrl->lock);

	/* Ensure at least two free nodes are available for use below */
	अगर (!alloc_nodes(hpctrl))
		जाओ out;

	np = शून्य;
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, शीर्ष) {
		lowest_aligned = ROUNDUP(n->addr, align);
		pad = lowest_aligned - n->addr;

		अगर (आकार + pad <= n->आकार) {
			np = n;
			अवरोध;
		}
	}
	अगर (!np)
		जाओ out;

	अगर (pad) {
		n = sbi_list_first_entry(&hpctrl->free_गाँठ_list,
					 काष्ठा heap_गाँठ, शीर्ष);
		sbi_list_del(&n->शीर्ष);

		n->addr = np->addr;
		n->आकार = pad;
		sbi_list_add_पुच्छ(&n->शीर्ष, &np->शीर्ष);

		np->addr += pad;
		np->आकार -= pad;
	}

	अगर (आकार < np->आकार) {
		n = sbi_list_first_entry(&hpctrl->free_गाँठ_list,
					 काष्ठा heap_गाँठ, शीर्ष);
		sbi_list_del(&n->शीर्ष);

		n->addr = np->addr + आकार;
		n->आकार = np->आकार - आकार;
		sbi_list_add(&n->शीर्ष, &np->शीर्ष);

		np->आकार = आकार;
	}

	sbi_list_del(&np->शीर्ष);
	sbi_list_add_पुच्छ(&np->शीर्ष, &hpctrl->used_space_list);
	ret = (व्योम *)np->addr;

out:
	spin_unlock(&hpctrl->lock);

	वापस ret;
}

व्योम *sbi_malloc_from(काष्ठा sbi_heap_control *hpctrl, माप_प्रकार आकार)
{
	वापस alloc_with_align(hpctrl, HEAP_ALLOC_ALIGN, आकार);
}

व्योम *sbi_aligned_alloc_from(काष्ठा sbi_heap_control *hpctrl,
			     माप_प्रकार alignment, माप_प्रकार आकार)
{
	अगर (alignment < HEAP_ALLOC_ALIGN)
		alignment = HEAP_ALLOC_ALIGN;

	/* Make sure alignment is power of two */
	अगर ((alignment & (alignment - 1)) != 0)
		वापस शून्य;

	/* Make sure size is multiple of alignment */
	अगर (आकार % alignment != 0)
		वापस शून्य;

	वापस alloc_with_align(hpctrl, alignment, आकार);
}

व्योम *sbi_zalloc_from(काष्ठा sbi_heap_control *hpctrl, माप_प्रकार आकार)
{
	व्योम *ret = sbi_malloc_from(hpctrl, आकार);

	अगर (ret)
		sbi_memset(ret, 0, आकार);
	वापस ret;
}

व्योम sbi_free_from(काष्ठा sbi_heap_control *hpctrl, व्योम *ptr)
{
	काष्ठा heap_गाँठ *n, *np;

	अगर (!ptr)
		वापस;

	spin_lock(&hpctrl->lock);

	np = शून्य;
	sbi_list_for_each_entry(n, &hpctrl->used_space_list, शीर्ष) {
		अगर ((n->addr <= (अचिन्हित दीर्घ)ptr) &&
		    ((अचिन्हित दीर्घ)ptr < (n->addr + n->आकार))) {
			np = n;
			अवरोध;
		}
	}
	अगर (!np) {
		spin_unlock(&hpctrl->lock);
		वापस;
	}

	sbi_list_del(&np->शीर्ष);

	sbi_list_for_each_entry(n, &hpctrl->free_space_list, शीर्ष) {
		अगर ((np->addr + np->आकार) == n->addr) {
			n->addr = np->addr;
			n->आकार += np->आकार;
			sbi_list_add_पुच्छ(&np->शीर्ष, &hpctrl->free_गाँठ_list);
			np = शून्य;
			अवरोध;
		} अन्यथा अगर (np->addr == (n->addr + n->आकार)) {
			n->आकार += np->आकार;
			sbi_list_add_पुच्छ(&np->शीर्ष, &hpctrl->free_गाँठ_list);
			np = शून्य;
			अवरोध;
		} अन्यथा अगर ((n->addr + n->आकार) < np->addr) {
			sbi_list_add(&np->शीर्ष, &n->शीर्ष);
			np = शून्य;
			अवरोध;
		}
	}
	अगर (np)
		sbi_list_add_पुच्छ(&np->शीर्ष, &hpctrl->free_space_list);

	spin_unlock(&hpctrl->lock);
}

अचिन्हित दीर्घ sbi_heap_free_space_from(काष्ठा sbi_heap_control *hpctrl)
{
	काष्ठा heap_गाँठ *n;
	अचिन्हित दीर्घ ret = 0;

	spin_lock(&hpctrl->lock);
	sbi_list_for_each_entry(n, &hpctrl->free_space_list, शीर्ष)
		ret += n->आकार;
	spin_unlock(&hpctrl->lock);

	वापस ret;
}

अचिन्हित दीर्घ sbi_heap_used_space_from(काष्ठा sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	वापस hpctrl->आकार - hpctrl->resv - sbi_heap_free_space();
}

अचिन्हित दीर्घ sbi_heap_reserved_space_from(काष्ठा sbi_heap_control *hpctrl)
	NO_THREAD_SAFETY_ANALYSIS
{
	वापस hpctrl->resv;
}

पूर्णांक sbi_heap_init_new(काष्ठा sbi_heap_control *hpctrl, अचिन्हित दीर्घ base,
		      अचिन्हित दीर्घ आकार) NO_THREAD_SAFETY_ANALYSIS
{
	काष्ठा heap_गाँठ *n;

	/* Initialize heap control */
	SPIN_LOCK_INIT(hpctrl->lock);
	hpctrl->base = base;
	hpctrl->आकार = आकार;
	hpctrl->resv = 0;
	SBI_INIT_LIST_शीर्ष(&hpctrl->free_गाँठ_list);
	SBI_INIT_LIST_शीर्ष(&hpctrl->free_space_list);
	SBI_INIT_LIST_शीर्ष(&hpctrl->used_space_list);

	/* Prepare free space list */
	n = &hpctrl->init_free_space_गाँठ;
	n->addr = base;
	n->आकार = आकार;
	sbi_list_add_पुच्छ(&n->शीर्ष, &hpctrl->free_space_list);

	वापस 0;
}

पूर्णांक sbi_heap_init(काष्ठा sbi_scratch *scratch)
{
	/* Sanity checks on heap offset and size */
	अगर (!scratch->fw_heap_आकार ||
	    (scratch->fw_heap_आकार & (HEAP_BASE_ALIGN - 1)) ||
	    (scratch->fw_heap_offset < scratch->fw_rw_offset) ||
	    (scratch->fw_आकार < (scratch->fw_heap_offset + scratch->fw_heap_आकार)) ||
	    (scratch->fw_heap_offset & (HEAP_BASE_ALIGN - 1)))
		वापस SBI_EINVAL;

	वापस sbi_heap_init_new(&global_hpctrl,
				  scratch->fw_start + scratch->fw_heap_offset,
				  scratch->fw_heap_आकार);
}

पूर्णांक sbi_heap_alloc_new(काष्ठा sbi_heap_control **hpctrl)
{
	*hpctrl = sbi_calloc(1, माप(काष्ठा sbi_heap_control));
	वापस 0;
}
