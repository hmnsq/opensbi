/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#شامل <sbi/riscv_asm.h>
#شامل <sbi/riscv_atomic.h>
#شامل <sbi/riscv_barrier.h>
#شامل <sbi/sbi_bitops.h>
#شامل <sbi/sbi_domain.h>
#شامل <sbi/sbi_error.h>
#شامل <sbi/sbi_hart.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_hsm.h>
#شامل <sbi/sbi_init.h>
#شامل <sbi/sbi_ipi.h>
#شامل <sbi/sbi_list.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_pmu.h>
#شامل <sbi/sbi_string.h>
#شامل <sbi/sbi_tlb.h>

ساختار sbi_ipi_data {
	unsigned long ipi_type;
};

_Static_assert(
	8 * اندازه(((ساختار sbi_ipi_data*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

ساختار sbi_ipi_device_گره {
	ساختار sbi_dlist head;
	ثابت ساختار sbi_ipi_device *dev;
};

ایستا unsigned long ipi_data_off;
ایستا ثابت ساختار sbi_ipi_device *ipi_dev = پوچ;
ایستا SBI_LIST_HEAD(ipi_dev_گره_list);
ایستا ثابت ساختار sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

ایستا صحیح sbi_ipi_send(ساختار sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, تهی *data)
{
	صحیح ret = 0;
	ساختار sbi_scratch *remote_scratch = پوچ;
	ساختار sbi_ipi_data *ipi_data;
	ثابت ساختار sbi_ipi_event_ops *ipi_ops;

	اگر ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		بازگشت SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_to_scratch(remote_hartindex);
	اگر (!remote_scratch)
		بازگشت SBI_EINVAL;

	ipi_data = sbi_scratch_offset_ptr(remote_scratch, ipi_data_off);

	اگر (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, data);
		اگر (ret != SBI_IPI_UPDATE_SUCCESS)
			بازگشت ret;
	} وگرنه اگر (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		بازگشت 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	اگر (!__atomic_fetch_or(&ipi_data->ipi_type,
				BIT(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_SENT);

	بازگشت ret;
}

ایستا صحیح sbi_ipi_sync(ساختار sbi_scratch *scratch, u32 event)
{
	ثابت ساختار sbi_ipi_event_ops *ipi_ops;

	اگر ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		بازگشت SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	اگر (ipi_ops->sync)
		ipi_ops->sync(scratch);

	بازگشت 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
صحیح sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, تهی *data)
{
	صحیح rc = 0;
	bool retry_needed;
	ulong i;
	ساختار sbi_hartmask target_mask;
	ساختار sbi_domain *dom = sbi_domain_thishart_ptr();
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	اگر (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		بازگشت 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_mask(dom, &target_mask);
	اگر (rc)
		بازگشت rc;

	اگر (hbase != -1UL) {
		ساختار sbi_hartmask tmp_mask = { 0 };
		ساختار sbi_hartmask domain_mask;
		صحیح شمار = sbi_popcount(hmask);

		برای (i = hbase; hmask; i++, hmask >>= 1) {
			اگر (hmask & 1UL)
				sbi_hartmask_set_hartid(i, &tmp_mask);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_mask);
		اگر (rc)
			بازگشت rc;

		sbi_hartmask_and(&domain_mask, &domain_mask, &tmp_mask);
		اگر (sbi_hartmask_weight(&domain_mask) != شمار)
			بازگشت SBI_EINVAL;

		sbi_hartmask_and(&target_mask, &target_mask, &tmp_mask);
	}

	/* Send IPIs */
	انجامبده {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &target_mask) {
			rc = sbi_ipi_send(scratch, i, event, data);
			اگر (rc < 0)
				برو done;
			اگر (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			وگرنه
				sbi_hartmask_clear_hartindex(i, &target_mask);
			rc = 0;
		}
	} وقتی (retry_needed);

done:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	بازگشت rc;
}

صحیح sbi_ipi_event_create(ثابت ساختار sbi_ipi_event_ops *ops)
{
	صحیح i, ret = SBI_ENOSPC;

	اگر (!ops || !ops->process)
		بازگشت SBI_EINVAL;

	برای (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		اگر (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			بشکن;
		}
	}

	بازگشت ret;
}

تهی sbi_ipi_event_destroy(u32 event)
{
	اگر (SBI_IPI_EVENT_MAX <= event)
		بازگشت;

	ipi_ops_array[event] = پوچ;
}

ایستا تهی sbi_ipi_process_smode(ساختار sbi_scratch *scratch)
{
	csr_set(CSR_MIP, MIP_SSIP);
}

ایستا ساختار sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

ایستا u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

صحیح sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	بازگشت sbi_ipi_send_many(hmask, hbase, ipi_smode_event, پوچ);
}

تهی sbi_ipi_clear_smode(تهی)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

ایستا صحیح sbi_ipi_update_halt(ساختار sbi_scratch *scratch,
			       ساختار sbi_scratch *remote_scratch,
			       u32 remote_hartindex, تهی *data)
{
	/* Never send a halt IPI to the local hart. */
	بازگشت scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

ایستا تهی sbi_ipi_process_halt(ساختار sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

ایستا ساختار sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

ایستا u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

صحیح sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	بازگشت sbi_ipi_send_many(hmask, hbase, ipi_halt_event, پوچ);
}

تهی sbi_ipi_process(تهی)
{
	unsigned long ipi_type;
	unsigned صحیح ipi_event;
	ثابت ساختار sbi_ipi_event_ops *ipi_ops;
	ساختار sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ساختار sbi_ipi_data *ipi_data =
			sbi_scratch_offset_ptr(scratch, ipi_data_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_data->ipi_type, 0);
	ipi_event = 0;
	وقتی (ipi_type) {
		اگر (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			اگر (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

صحیح sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	ساختار sbi_ipi_device_گره *entry;

	اگر (!ipi_dev || !ipi_dev->ipi_send)
		بازگشت SBI_EINVAL;

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

	اگر (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_گره_list, head) {
			اگر (entry->dev->ipi_send)
				entry->dev->ipi_send(hartindex);
		}
	} وگرنه {
		ipi_dev->ipi_send(hartindex);
	}

	بازگشت 0;
}

تهی sbi_ipi_raw_clear(bool all_devices)
{
	ساختار sbi_ipi_device_گره *entry;

	اگر (all_devices) {
		sbi_list_for_each_entry(entry, &ipi_dev_گره_list, head) {
			اگر (entry->dev->ipi_clear)
				entry->dev->ipi_clear();
		}
	} وگرنه {
		اگر (ipi_dev && ipi_dev->ipi_clear)
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

ثابت ساختار sbi_ipi_device *sbi_ipi_get_device(تهی)
{
	بازگشت ipi_dev;
}

تهی sbi_ipi_add_device(ثابت ساختار sbi_ipi_device *dev)
{
	ساختار sbi_ipi_device_گره *entry;

	اگر (!dev)
		بازگشت;

	entry = sbi_zalloc(اندازه(*entry));
	اگر (!entry)
		بازگشت;
	SBI_INIT_LIST_HEAD(&entry->head);
	entry->dev = dev;
	sbi_list_add_tail(&entry->head, &ipi_dev_گره_list);

	اگر (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

صحیح sbi_ipi_init(ساختار sbi_scratch *scratch, bool cold_boot)
{
	صحیح ret;
	ساختار sbi_ipi_data *ipi_data;

	اگر (cold_boot) {
		ipi_data_off = sbi_scratch_alloc_offset(اندازه(*ipi_data));
		اگر (!ipi_data_off)
			بازگشت SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		اگر (ret < 0)
			بازگشت ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		اگر (ret < 0)
			بازگشت ret;
		ipi_halt_event = ret;
	} وگرنه {
		اگر (!ipi_data_off)
			بازگشت SBI_ENOMEM;
		اگر (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			بازگشت SBI_ENOSPC;
	}

	ipi_data = sbi_scratch_offset_ptr(scratch, ipi_data_off);
	ipi_data->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_set(CSR_MIE, MIP_MSIP);

	بازگشت 0;
}

تهی sbi_ipi_exit(ساختار sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
