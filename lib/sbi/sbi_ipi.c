/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#ضمن <sbi/riscv_asm.h>
#ضمن <sbi/riscv_atomic.h>
#ضمن <sbi/riscv_barrier.h>
#ضمن <sbi/sbi_bitops.h>
#ضمن <sbi/sbi_domain.h>
#ضمن <sbi/sbi_error.h>
#ضمن <sbi/sbi_hart.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_hsm.h>
#ضمن <sbi/sbi_init.h>
#ضمن <sbi/sbi_ipi.h>
#ضمن <sbi/sbi_list.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_pmu.h>
#ضمن <sbi/sbi_string.h>
#ضمن <sbi/sbi_tlb.h>

بنية sbi_ipi_انکڑے {
	بنانشانی لما ipi_type;
};

_Static_assert(
	8 * حجم(((بنية sbi_ipi_انکڑے*)0)->ipi_type) == SBI_IPI_EVENT_MAX,
	"type of sbi_ipi_data.ipi_type has changed, please redefine SBI_IPI_EVENT_MAX"
	);

بنية sbi_ipi_device_گرہ {
	بنية sbi_dlist سر;
	ثابت بنية sbi_ipi_device *dev;
};

ساكن بنانشانی لما ipi_انکڑے_off;
ساكن ثابت بنية sbi_ipi_device *ipi_dev = عدم;
ساكن SBI_LIST_سر(ipi_dev_گرہ_list);
ساكن ثابت بنية sbi_ipi_event_ops *ipi_ops_array[SBI_IPI_EVENT_MAX];

ساكن صحيح sbi_ipi_send(بنية sbi_scratch *scratch, u32 remote_hartindex,
			u32 event, فراغ *انکڑے)
{
	صحيح ret = 0;
	بنية sbi_scratch *remote_scratch = عدم;
	بنية sbi_ipi_انکڑے *ipi_انکڑے;
	ثابت بنية sbi_ipi_event_ops *ipi_ops;

	إذا ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		أرجع SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	remote_scratch = sbi_hartindex_منزل_scratch(remote_hartindex);
	إذا (!remote_scratch)
		أرجع SBI_EINVAL;

	ipi_انکڑے = sbi_scratch_وتھ_ptr(remote_scratch, ipi_انکڑے_off);

	إذا (ipi_ops->update) {
		ret = ipi_ops->update(scratch, remote_scratch,
				      remote_hartindex, انکڑے);
		إذا (ret != SBI_IPI_UPDATE_SUCCESS)
			أرجع ret;
	} وإلا إذا (scratch == remote_scratch) {
		/*
		 * IPI events with an update() callback are expected to return
		 * SBI_IPI_UPDATE_BREAK for self-IPIs. For other events, check
		 * for self-IPI and execute the callback directly here.
		 */
		ipi_ops->process(scratch);
		أرجع 0;
	}

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt.
	 *
	 * Multiple harts may be trying to send IPI to the
	 * remote hart so call sbi_ipi_raw_send() only when
	 * the ipi_type was previously zero.
	 */
	إذا (!__atomic_fetch_or(&ipi_انکڑے->ipi_type,
				ثنائی_ہندسہ(event), __ATOMIC_RELAXED))
		ret = sbi_ipi_raw_send(remote_hartindex, false);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_گھلیا);

	أرجع ret;
}

ساكن صحيح sbi_ipi_sync(بنية sbi_scratch *scratch, u32 event)
{
	ثابت بنية sbi_ipi_event_ops *ipi_ops;

	إذا ((SBI_IPI_EVENT_MAX <= event) ||
	    !ipi_ops_array[event])
		أرجع SBI_EINVAL;
	ipi_ops = ipi_ops_array[event];

	إذا (ipi_ops->sync)
		ipi_ops->sync(scratch);

	أرجع 0;
}

/**
 * As this this function only handlers scalar values of hart mask, it must be
 * set to all online harts if the intention is to send IPIs to all the harts.
 * If hmask is zero, no IPIs will be sent.
 */
صحيح sbi_ipi_send_many(ulong hmask, ulong hbase, u32 event, فراغ *انکڑے)
{
	صحيح rc = 0;
	bool retry_needed;
	ulong i;
	بنية sbi_hartmask نشانہ_نقاب;
	بنية sbi_domain *dom = sbi_domain_thishart_ptr();
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	إذا (hmask == 0 && hbase != -1UL) {
		/* Nothing to do, but it's not an error either. */
		أرجع 0;
	}

	/* Find the target harts */
	rc = sbi_hsm_hart_interruptible_نقاب(dom, &نشانہ_نقاب);
	إذا (rc)
		أرجع rc;

	إذا (hbase != -1UL) {
		بنية sbi_hartmask tmp_نقاب = { 0 };
		بنية sbi_hartmask domain_نقاب;
		صحيح گنتی = sbi_popcount(hmask);

		لكل (i = hbase; hmask; i++, hmask >>= 1) {
			إذا (hmask & 1UL)
				sbi_hartmask_سیٹ_hartid(i, &tmp_نقاب);
		}

		/* Validate hartids against domain assignment, not HSM state */
		rc = sbi_domain_get_assigned_hartmask(dom, &domain_نقاب);
		إذا (rc)
			أرجع rc;

		sbi_hartmask_and(&domain_نقاب, &domain_نقاب, &tmp_نقاب);
		إذا (sbi_hartmask_بھار(&domain_نقاب) != گنتی)
			أرجع SBI_EINVAL;

		sbi_hartmask_and(&نشانہ_نقاب, &نشانہ_نقاب, &tmp_نقاب);
	}

	/* Send IPIs */
	افعل {
		retry_needed = false;
		sbi_hartmask_for_each_hartindex(i, &نشانہ_نقاب) {
			rc = sbi_ipi_send(scratch, i, event, انکڑے);
			إذا (rc < 0)
				اذهب پورا;
			إذا (rc == SBI_IPI_UPDATE_RETRY)
				retry_needed = true;
			وإلا
				sbi_hartmask_clear_hartindex(i, &نشانہ_نقاب);
			rc = 0;
		}
	} طالما (retry_needed);

پورا:
	/* Sync IPIs */
	sbi_ipi_sync(scratch, event);

	أرجع rc;
}

صحيح sbi_ipi_event_create(ثابت بنية sbi_ipi_event_ops *ops)
{
	صحيح i, ret = SBI_ENOSPC;

	إذا (!ops || !ops->process)
		أرجع SBI_EINVAL;

	لكل (i = 0; i < SBI_IPI_EVENT_MAX; i++) {
		إذا (!ipi_ops_array[i]) {
			ret = i;
			ipi_ops_array[i] = ops;
			اكسر;
		}
	}

	أرجع ret;
}

فراغ sbi_ipi_event_destroy(u32 event)
{
	إذا (SBI_IPI_EVENT_MAX <= event)
		أرجع;

	ipi_ops_array[event] = عدم;
}

ساكن فراغ sbi_ipi_process_smode(بنية sbi_scratch *scratch)
{
	csr_سیٹ(CSR_MIP, MIP_SSIP);
}

ساكن بنية sbi_ipi_event_ops ipi_smode_ops = {
	.name = "IPI_SMODE",
	.process = sbi_ipi_process_smode,
};

ساكن u32 ipi_smode_event = SBI_IPI_EVENT_MAX;

صحيح sbi_ipi_send_smode(ulong hmask, ulong hbase)
{
	أرجع sbi_ipi_send_many(hmask, hbase, ipi_smode_event, عدم);
}

فراغ sbi_ipi_clear_smode(فراغ)
{
	csr_clear(CSR_MIP, MIP_SSIP);
}

ساكن صحيح sbi_ipi_update_halt(بنية sbi_scratch *scratch,
			       بنية sbi_scratch *remote_scratch,
			       u32 remote_hartindex, فراغ *انکڑے)
{
	/* Never send a halt IPI to the local hart. */
	أرجع scratch == remote_scratch ?
		SBI_IPI_UPDATE_BREAK : SBI_IPI_UPDATE_SUCCESS;
}

ساكن فراغ sbi_ipi_process_halt(بنية sbi_scratch *scratch)
{
	sbi_hsm_hart_stop(scratch, true);
}

ساكن بنية sbi_ipi_event_ops ipi_halt_ops = {
	.name = "IPI_HALT",
	.update = sbi_ipi_update_halt,
	.process = sbi_ipi_process_halt,
};

ساكن u32 ipi_halt_event = SBI_IPI_EVENT_MAX;

صحيح sbi_ipi_send_halt(ulong hmask, ulong hbase)
{
	أرجع sbi_ipi_send_many(hmask, hbase, ipi_halt_event, عدم);
}

فراغ sbi_ipi_process(فراغ)
{
	بنانشانی لما ipi_type;
	بنانشانی صحيح ipi_event;
	ثابت بنية sbi_ipi_event_ops *ipi_ops;
	بنية sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	بنية sbi_ipi_انکڑے *ipi_انکڑے =
			sbi_scratch_وتھ_ptr(scratch, ipi_انکڑے_off);

	sbi_pmu_ctr_incr_fw(SBI_PMU_FW_IPI_RECVD);
	sbi_ipi_raw_clear(false);

	ipi_type = atomic_raw_xchg_ulong(&ipi_انکڑے->ipi_type, 0);
	ipi_event = 0;
	طالما (ipi_type) {
		إذا (ipi_type & 1UL) {
			ipi_ops = ipi_ops_array[ipi_event];
			إذا (ipi_ops)
				ipi_ops->process(scratch);
		}
		ipi_type = ipi_type >> 1;
		ipi_event++;
	}
}

صحيح sbi_ipi_raw_send(u32 hartindex, bool all_devices)
{
	بنية sbi_ipi_device_گرہ *اندراج_2;

	إذا (!ipi_dev || !ipi_dev->ipi_send)
		أرجع SBI_EINVAL;

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

	إذا (all_devices) {
		sbi_list_for_each_اندراج(اندراج_2, &ipi_dev_گرہ_list, سر) {
			إذا (اندراج_2->dev->ipi_send)
				اندراج_2->dev->ipi_send(hartindex);
		}
	} وإلا {
		ipi_dev->ipi_send(hartindex);
	}

	أرجع 0;
}

فراغ sbi_ipi_raw_clear(bool all_devices)
{
	بنية sbi_ipi_device_گرہ *اندراج_2;

	إذا (all_devices) {
		sbi_list_for_each_اندراج(اندراج_2, &ipi_dev_گرہ_list, سر) {
			إذا (اندراج_2->dev->ipi_clear)
				اندراج_2->dev->ipi_clear();
		}
	} وإلا {
		إذا (ipi_dev && ipi_dev->ipi_clear)
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

ثابت بنية sbi_ipi_device *sbi_ipi_get_device(فراغ)
{
	أرجع ipi_dev;
}

فراغ sbi_ipi_جوڑو_device(ثابت بنية sbi_ipi_device *dev)
{
	بنية sbi_ipi_device_گرہ *اندراج_2;

	إذا (!dev)
		أرجع;

	اندراج_2 = sbi_zalloc(حجم(*اندراج_2));
	إذا (!اندراج_2)
		أرجع;
	SBI_INIT_LIST_سر(&اندراج_2->سر);
	اندراج_2->dev = dev;
	sbi_list_جوڑو_پوچھ(&اندراج_2->سر, &ipi_dev_گرہ_list);

	إذا (!ipi_dev || ipi_dev->rating < dev->rating)
		ipi_dev = dev;
}

صحيح sbi_ipi_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	صحيح ret;
	بنية sbi_ipi_انکڑے *ipi_انکڑے;

	إذا (cold_boot) {
		ipi_انکڑے_off = sbi_scratch_alloc_وتھ(حجم(*ipi_انکڑے));
		إذا (!ipi_انکڑے_off)
			أرجع SBI_ENOMEM;
		ret = sbi_ipi_event_create(&ipi_smode_ops);
		إذا (ret < 0)
			أرجع ret;
		ipi_smode_event = ret;
		ret = sbi_ipi_event_create(&ipi_halt_ops);
		إذا (ret < 0)
			أرجع ret;
		ipi_halt_event = ret;
	} وإلا {
		إذا (!ipi_انکڑے_off)
			أرجع SBI_ENOMEM;
		إذا (SBI_IPI_EVENT_MAX <= ipi_smode_event ||
		    SBI_IPI_EVENT_MAX <= ipi_halt_event)
			أرجع SBI_ENOSPC;
	}

	ipi_انکڑے = sbi_scratch_وتھ_ptr(scratch, ipi_انکڑے_off);
	ipi_انکڑے->ipi_type = 0x00;

	/* Clear any pending IPIs for the current hart */
	sbi_ipi_raw_clear(true);

	/* Enable software interrupts */
	csr_سیٹ(CSR_MIE, MIP_MSIP);

	أرجع 0;
}

فراغ sbi_ipi_exit(بنية sbi_scratch *scratch)
{
	/* Disable software interrupts */
	csr_clear(CSR_MIE, MIP_MSIP);

	/* Process pending IPIs */
	sbi_ipi_process();
}
