/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#समावेश <sbi/riscv_asm.h>
#समावेश <sbi/riscv_atomic.h>
#समावेश <sbi/riscv_barrier.h>
#समावेश <sbi/sbi_bitops.h>
#समावेश <sbi/sbi_domain.h>
#समावेश <sbi/sbi_error.h>
#समावेश <sbi/sbi_hart.h>
#समावेश <sbi/sbi_heap.h>
#समावेश <sbi/sbi_hsm.h>
#समावेश <sbi/sbi_init.h>
#समावेश <sbi/sbi_ipi.h>
#समावेश <sbi/sbi_list.h>
#समावेश <sbi/sbi_platform.h>
#समावेश <sbi/sbi_pmu.h>
#समावेश <sbi/sbi_string.h>
#समावेश <sbi/sbi_tlb.h>

काष्ठा sbi_ipi_तथ्य {
	अचिन्हित दीर्घ ipi_type;
};

_Static_assert(
	8 * माप(((काष्ठा sbi_ipi_तथ्य*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

काष्ठा sbi_ipi_device_गाँठ {
	काष्ठा sbi_dlist शीर्ष;
	स्थिर काष्ठा sbi_ipi_device *dev;
};

अटल अचिन्हित दीर्घ ipi_तथ्य_off;
अटल स्थिर काष्ठा sbi_ipi_device *ipi_dev = शून्य;
अटल SBI_LIST_शीर्ष(ipi_dev_गाँठ_list);
अटल स्थिर काष्ठा sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

अटल पूर्णांक sbi_ipi_send(काष्ठा sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, व्योम *तथ्य)
{
	पूर्णांक ret = 0;
	काष्ठा sbi_scratch *remote_scratch = शून्य;
	काष्ठा sbi_ipi_तथ्य *ipi_तथ्य;
	स्थिर काष्ठा sbi_ipi_event_ops *ipi_ops;

	अगर ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		वापस SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_to_scratch(remote_hartindex);
	अगर (!remote_scratch)
		वापस SBI_EINVAL;

	ipi_तथ्य = sbi_scratch_offset_ptr(remote_scratch, ipi_तथ्य_off);

	अगर (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, तथ्य);
		अगर (ret != SBI_IPI_UPDATE_SUCCESS)
			वापस ret;
	} अन्यथा अगर (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		वापस 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	अगर (!__atomic_fetch_or(&ipi_तथ्य->ipi_type,
				BIT(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_SENT);

	वापस ret;
}

अटल पूर्णांक sbi_ipi_sync(काष्ठा sbi_scratch *scratch, u32 event)
{
	स्थिर काष्ठा sbi_ipi_event_ops *ipi_ops;

	अगर ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		वापस SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	अगर (ipi_ops->sync)
		ipi_ops->sync(scratch);

	वापस 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
पूर्णांक sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, व्योम *तथ्य)
{
	पूर्णांक rc = 0;
	bool retry_needed;
	ulong i;
	काष्ठा sbi_hartmask target_mask;
	काष्ठा sbi_domain *dom = sbi_domain_thishart_ptr();
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	अगर (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		वापस 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_mask(dom, &target_mask);
	अगर (rc)
		वापस rc;

	अगर (hbase != -1UL) {
		काष्ठा sbi_hartmask tmp_mask = { 0 };
		काष्ठा sbi_hartmask domain_mask;
		पूर्णांक गिनती = sbi_popcount(hmask);

		क्रम (i = hbase; hmask; i++, hmask >>= 1) {
			अगर (hmask & 1UL)
				sbi_hartmask_set_hartid(i, &tmp_mask);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_mask);
		अगर (rc)
			वापस rc;

		sbi_hartmask_and(&domain_mask, &domain_mask, &tmp_mask);
		अगर (sbi_hartmask_weight(&domain_mask) != गिनती)
			वापस SBI_EINVAL;

		sbi_hartmask_and(&target_mask, &target_mask, &tmp_mask);
	}

	/* Send IPIs */
	करो {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &target_mask) {
			rc = sbi_ipi_send(scratch, i, event, तथ्य);
			अगर (rc < 0)
				जाओ done;
			अगर (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			अन्यथा
				sbi_hartmask_clear_hartindex(i, &target_mask);
			rc = 0;
		}
	} जबतक (retry_needed);

done:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	वापस rc;
}

पूर्णांक sbi_ipi_event_create(स्थिर काष्ठा sbi_ipi_event_ops *ops)
{
	पूर्णांक i, ret = SBI_ENOSPC;

	अगर (!ops || !ops->process)
		वापस SBI_EINVAL;

	क्रम (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		अगर (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			अवरोध;
		}
	}

	वापस ret;
}

व्योम sbi_ipi_event_destroy(u32 event)
{
	अगर (SBI_IPI_EVENT_MAX <= event)
		वापस;

	ipi_ops_array[event] = शून्य;
}

अटल व्योम sbi_ipi_process_smode(काष्ठा sbi_scratch *scratch)
{
	csr_set(CSR_MIP, MIP_SSIP);
}

अटल काष्ठा sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

अटल u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

पूर्णांक sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	वापस sbi_ipi_send_many(hmask, hbase, ipi_smode_event, शून्य);
}

व्योम sbi_ipi_clear_smode(व्योम)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

अटल पूर्णांक sbi_ipi_update_halt(काष्ठा sbi_scratch *scratch,
			       काष्ठा sbi_scratch *remote_scratch,
			       u32 remote_hartindex, व्योम *तथ्य)
{
	/* Never send a halt IPI to the local hart. */
	वापस scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

अटल व्योम sbi_ipi_process_halt(काष्ठा sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

अटल काष्ठा sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

अटल u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

पूर्णांक sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	वापस sbi_ipi_send_many(hmask, hbase, ipi_halt_event, शून्य);
}

व्योम sbi_ipi_process(व्योम)
{
	अचिन्हित दीर्घ ipi_type;
	अचिन्हित पूर्णांक ipi_event;
	स्थिर काष्ठा sbi_ipi_event_ops *ipi_ops;
	काष्ठा sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	काष्ठा sbi_ipi_तथ्य *ipi_तथ्य =
			sbi_scratch_offset_ptr(scratch, ipi_तथ्य_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_तथ्य->ipi_type, 0);
	ipi_event = 0;
	जबतक (ipi_type) {
		अगर (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			अगर (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

पूर्णांक sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	काष्ठा sbi_ipi_device_गाँठ *entry;

	अगर (!ipi_dev || !ipi_dev->ipi_send)
		वापस SBI_EINVAL;

	/*
	 * Ensure that memory or MMIO writes done before
	 * this function are not observed after the memory
	 * or MMIO writes done by the ipi_send() device
	 * callback. This also allows the ipi_send() device
	 * callback to use relaxed MMIO writes.
	 *
	 * This pairs with the wmb() in sbi_ipi_raw_clear().
	 */
	wmb();

	अगर (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_गाँठ_list, शीर्ष) {
			अगर (entry->dev->ipi_send)
				entry->dev->ipi_send(hartindex);
		}
	} अन्यथा {
		ipi_dev->ipi_send(hartindex);
	}

	वापस 0;
}

व्योम sbi_ipi_raw_clear(bool all_devices)
{
	काष्ठा sbi_ipi_device_गाँठ *entry;

	अगर (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_गाँठ_list, शीर्ष) {
			अगर (entry->dev->ipi_clear)
				entry->dev->ipi_clear();
		}
	} अन्यथा {
		अगर (ipi_dev && ipi_dev->ipi_clear)
			ipi_dev->ipi_clear();
	}

	/*
	 * Ensure that memory or MMIO writes after this
	 * function returns are not observed before the
	 * memory or MMIO writes done by the ipi_clear()
	 * device callback. This also allows ipi_clear()
	 * device callback to use relaxed MMIO writes.
	 *
	 * This pairs with the wmb() in sbi_ipi_raw_send().
	 */
	wmb();
}

स्थिर काष्ठा sbi_ipi_device *sbi_ipi_get_device(व्योम)
{
	वापस ipi_dev;
}

व्योम sbi_ipi_add_device(स्थिर काष्ठा sbi_ipi_device *dev)
{
	काष्ठा sbi_ipi_device_गाँठ *entry;

	अगर (!dev)
		वापस;

	entry = sbi_zalloc(माप(*entry));
	अगर (!entry)
		वापस;
	SBI_INIT_LIST_शीर्ष(&entry->शीर्ष);
	entry->dev = dev;
	sbi_list_add_पुच्छ(&entry->शीर्ष, &ipi_dev_गाँठ_list);

	अगर (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

पूर्णांक sbi_ipi_init(काष्ठा sbi_scratch *scratch, bool cold_boot)
{
	पूर्णांक ret;
	काष्ठा sbi_ipi_तथ्य *ipi_तथ्य;

	अगर (cold_boot) {
		ipi_तथ्य_off = sbi_scratch_alloc_offset(माप(*ipi_तथ्य));
		अगर (!ipi_तथ्य_off)
			वापस SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		अगर (ret < 0)
			वापस ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		अगर (ret < 0)
			वापस ret;
		ipi_halt_event = ret;
	} अन्यथा {
		अगर (!ipi_तथ्य_off)
			वापस SBI_ENOMEM;
		अगर (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			वापस SBI_ENOSPC;
	}

	ipi_तथ्य = sbi_scratch_offset_ptr(scratch, ipi_तथ्य_off);
	ipi_तथ्य->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_set(CSR_MIE, MIP_MSIP);

	वापस 0;
}

व्योम sbi_ipi_exit(काष्ठा sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
