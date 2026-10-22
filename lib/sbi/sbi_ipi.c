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

ساخت sbi_ipi_ڈیٹا {
	غیرعلامتی طویل ipi_type;
};

_Static_assert(
	8 * حجم(((ساخت sbi_ipi_ڈیٹا*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

ساخت sbi_ipi_device_گرہ {
	ساخت sbi_dlist سر;
	ثابت ساخت sbi_ipi_device *dev;
};

ساکن غیرعلامتی طویل ipi_ڈیٹا_off;
ساکن ثابت ساخت sbi_ipi_device *ipi_dev = تہی;
ساکن SBI_LIST_سر(ipi_dev_گرہ_list);
ساکن ثابت ساخت sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

ساکن صحیح sbi_ipi_send(ساخت sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, خالی *ڈیٹا)
{
	صحیح ret = 0;
	ساخت sbi_scratch *remote_scratch = تہی;
	ساخت sbi_ipi_ڈیٹا *ipi_ڈیٹا;
	ثابت ساخت sbi_ipi_event_ops *ipi_ops;

	اگر ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		واپس SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_مقصد_scratch(remote_hartindex);
	اگر (!remote_scratch)
		واپس SBI_EINVAL;

	ipi_ڈیٹا = sbi_scratch_انحراف_ptr(remote_scratch, ipi_ڈیٹا_off);

	اگر (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, ڈیٹا);
		اگر (ret != SBI_IPI_UPDATE_SUCCESS)
			واپس ret;
	} ورنہ اگر (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		واپس 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	اگر (!__atomic_fetch_or(&ipi_ڈیٹا->ipi_type,
				ثنائی_ہندسہ(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_بھیجا);

	واپس ret;
}

ساکن صحیح sbi_ipi_sync(ساخت sbi_scratch *scratch, u32 event)
{
	ثابت ساخت sbi_ipi_event_ops *ipi_ops;

	اگر ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		واپس SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	اگر (ipi_ops->sync)
		ipi_ops->sync(scratch);

	واپس 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
صحیح sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, خالی *ڈیٹا)
{
	صحیح rc = 0;
	bool retry_needed;
	ulong i;
	ساخت sbi_hartmask ہدف_نقاب;
	ساخت sbi_domain *dom = sbi_domain_thishart_ptr();
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	اگر (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		واپس 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_نقاب(dom, &ہدف_نقاب);
	اگر (rc)
		واپس rc;

	اگر (hbase != -1UL) {
		ساخت sbi_hartmask tmp_نقاب = { 0 };
		ساخت sbi_hartmask domain_نقاب;
		صحیح گنتی = sbi_popcount(hmask);

		برائے (i = hbase; hmask; i++, hmask >>= 1) {
			اگر (hmask & 1UL)
				sbi_hartmask_سیٹ_hartid(i, &tmp_نقاب);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_نقاب);
		اگر (rc)
			واپس rc;

		sbi_hartmask_and(&domain_نقاب, &domain_نقاب, &tmp_نقاب);
		اگر (sbi_hartmask_وزن(&domain_نقاب) != گنتی)
			واپس SBI_EINVAL;

		sbi_hartmask_and(&ہدف_نقاب, &ہدف_نقاب, &tmp_نقاب);
	}

	/* Send IPIs */
	کرو {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &ہدف_نقاب) {
			rc = sbi_ipi_send(scratch, i, event, ڈیٹا);
			اگر (rc < 0)
				جاؤ مکمل;
			اگر (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			ورنہ
				sbi_hartmask_clear_hartindex(i, &ہدف_نقاب);
			rc = 0;
		}
	} جب (retry_needed);

مکمل:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	واپس rc;
}

صحیح sbi_ipi_event_create(ثابت ساخت sbi_ipi_event_ops *ops)
{
	صحیح i, ret = SBI_ENOSPC;

	اگر (!ops || !ops->process)
		واپس SBI_EINVAL;

	برائے (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		اگر (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			توڑو;
		}
	}

	واپس ret;
}

خالی sbi_ipi_event_destroy(u32 event)
{
	اگر (SBI_IPI_EVENT_MAX <= event)
		واپس;

	ipi_ops_array[event] = تہی;
}

ساکن خالی sbi_ipi_process_smode(ساخت sbi_scratch *scratch)
{
	csr_سیٹ(CSR_MIP, MIP_SSIP);
}

ساکن ساخت sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

ساکن u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

صحیح sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	واپس sbi_ipi_send_many(hmask, hbase, ipi_smode_event, تہی);
}

خالی sbi_ipi_clear_smode(خالی)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

ساکن صحیح sbi_ipi_update_halt(ساخت sbi_scratch *scratch,
			       ساخت sbi_scratch *remote_scratch,
			       u32 remote_hartindex, خالی *ڈیٹا)
{
	/* Never send a halt IPI to the local hart. */
	واپس scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

ساکن خالی sbi_ipi_process_halt(ساخت sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

ساکن ساخت sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

ساکن u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

صحیح sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	واپس sbi_ipi_send_many(hmask, hbase, ipi_halt_event, تہی);
}

خالی sbi_ipi_process(خالی)
{
	غیرعلامتی طویل ipi_type;
	غیرعلامتی صحیح ipi_event;
	ثابت ساخت sbi_ipi_event_ops *ipi_ops;
	ساخت sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	ساخت sbi_ipi_ڈیٹا *ipi_ڈیٹا =
			sbi_scratch_انحراف_ptr(scratch, ipi_ڈیٹا_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_ڈیٹا->ipi_type, 0);
	ipi_event = 0;
	جب (ipi_type) {
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
	ساخت sbi_ipi_device_گرہ *اندراج_3;

	اگر (!ipi_dev || !ipi_dev->ipi_send)
		واپس SBI_EINVAL;

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
		sbi_list_for_each_اندراج(اندراج_3, &ipi_dev_گرہ_list, سر) {
			اگر (اندراج_3->dev->ipi_send)
				اندراج_3->dev->ipi_send(hartindex);
		}
	} ورنہ {
		ipi_dev->ipi_send(hartindex);
	}

	واپس 0;
}

خالی sbi_ipi_raw_clear(bool all_devices)
{
	ساخت sbi_ipi_device_گرہ *اندراج_3;

	اگر (all_devices) {
		sbi_list_for_each_اندراج(اندراج_3, &ipi_dev_گرہ_list, سر) {
			اگر (اندراج_3->dev->ipi_clear)
				اندراج_3->dev->ipi_clear();
		}
	} ورنہ {
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

ثابت ساخت sbi_ipi_device *sbi_ipi_get_device(خالی)
{
	واپس ipi_dev;
}

خالی sbi_ipi_جوڑو_device(ثابت ساخت sbi_ipi_device *dev)
{
	ساخت sbi_ipi_device_گرہ *اندراج_3;

	اگر (!dev)
		واپس;

	اندراج_3 = sbi_zalloc(حجم(*اندراج_3));
	اگر (!اندراج_3)
		واپس;
	SBI_INIT_LIST_سر(&اندراج_3->سر);
	اندراج_3->dev = dev;
	sbi_list_جوڑو_دم(&اندراج_3->سر, &ipi_dev_گرہ_list);

	اگر (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

صحیح sbi_ipi_init(ساخت sbi_scratch *scratch, bool cold_boot)
{
	صحیح ret;
	ساخت sbi_ipi_ڈیٹا *ipi_ڈیٹا;

	اگر (cold_boot) {
		ipi_ڈیٹا_off = sbi_scratch_alloc_انحراف(حجم(*ipi_ڈیٹا));
		اگر (!ipi_ڈیٹا_off)
			واپس SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		اگر (ret < 0)
			واپس ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		اگر (ret < 0)
			واپس ret;
		ipi_halt_event = ret;
	} ورنہ {
		اگر (!ipi_ڈیٹا_off)
			واپس SBI_ENOMEM;
		اگر (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			واپس SBI_ENOSPC;
	}

	ipi_ڈیٹا = sbi_scratch_انحراف_ptr(scratch, ipi_ڈیٹا_off);
	ipi_ڈیٹا->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_سیٹ(CSR_MIE, MIP_MSIP);

	واپس 0;
}

خالی sbi_ipi_exit(ساخت sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
