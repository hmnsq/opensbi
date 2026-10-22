/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#ضمن <sbi/sbi_console.h>
#ضمن <sbi/sbi_heap.h>
#ضمن <sbi/sbi_hsm.h>
#ضمن <sbi/sbi_irqchip.h>
#ضمن <sbi/sbi_list.h>
#ضمن <sbi/sbi_platform.h>
#ضمن <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
بنية sbi_irqchip_hwirq_معلومات {
	/** raw hardware interrupt handler */
	صحيح (*raw_handler)(بنية sbi_irqchip_device *chip, u32 hwirq);

#تعریف_کړه IRQ_ENABLED	بټ(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_شاخص;

	/** chip's private data */
	فراغ *priv;
};

/** Internal irqchip interrupt handler */
بنية sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	بنية sbi_dlist غوټه;

	/** First hardware IRQ handled by this handler */
	u32 لومړی_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 عدد_hwirq;

	/** Write MSI function of this handler */
	فراغ (*write_msi)(u32 hwirq, ثابت بنية sbi_irqchip_msi_پیغام *پیغام, فراغ *priv);

	/** Callback function of this handler */
	صحيح (*callback)(u32 hwirq, فراغ *priv);

	/** Callback private data */
	فراغ *priv;
};

بنية sbi_irqchip_hart_معلومات {
	بنية sbi_irqchip_device *chip;
};

ساكن بینښه اوږد irqchip_hart_معلومات_off;
ساكن SBI_LIST_سر(irqchip_list);

صحيح sbi_irqchip_process(فراغ)
{
	بنية sbi_irqchip_hart_معلومات *hd;

	hd = sbi_scratch_thishart_دننه_واټن_ptr(irqchip_hart_معلومات_off);
	إذا (!hd || !hd->chip || !hd->chip->process_hwirqs)
		أرجع SBI_ENODEV;

	أرجع hd->chip->process_hwirqs(hd->chip);
}

صحيح sbi_irqchip_process_hwirq(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_معلومات *معلومات;

	إذا (!chip || chip->عدد_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	معلومات = &chip->hwirqs[hwirq];
	إذا (!معلومات->raw_handler)
		أرجع SBI_ENOENT;

	أرجع معلومات->raw_handler(chip, hwirq);
}

ساكن مضمن u32 sbi_irqchip_get_irq_state(بنية sbi_irqchip_device *chip,
					     u32 hwirq)
{
	إذا (!chip || !chip->hwirqs || hwirq >= chip->عدد_hwirq)
		أرجع 0;

	أرجع chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(بنية sbi_irqchip_device *chip,
				   u32 hwirq)
{
	أرجع !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

صحيح sbi_irqchip_unmask_hwirq(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_معلومات *معلومات;

	إذا (!chip || chip->عدد_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	معلومات = &chip->hwirqs[hwirq];
	إذا (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		أرجع SBI_EALREADY;

	إذا (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	معلومات->irq_state |= IRQ_ENABLED;
	أرجع 0;
}

صحيح sbi_irqchip_ماسک_hwirq(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_معلومات *معلومات;

	إذا (!chip || chip->عدد_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	إذا (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		أرجع SBI_EALREADY;

	إذا (chip->hwirq_ماسک)
		chip->hwirq_ماسک(chip, hwirq);

	معلومات = &chip->hwirqs[hwirq];
	معلومات->irq_state &= ~IRQ_ENABLED;
	أرجع 0;
}

ساكن بنية sbi_irqchip_handler *sbi_irqchip_پیدا_کړه_handler(بنية sbi_irqchip_device *chip,
							    u32 hwirq)
{
	بنية sbi_irqchip_handler *h;

	إذا (!chip || chip->عدد_hwirq <= hwirq)
		أرجع عدم;

	sbi_list_for_each_ثبت(h, &chip->handler_list, غوټه) {
		إذا (h->لومړی_hwirq <= hwirq && hwirq < (h->لومړی_hwirq + h->عدد_hwirq))
			أرجع h;
	}

	أرجع عدم;
}

صحيح sbi_irqchip_سېټ_hwirq_priv(بنية sbi_irqchip_device *chip, u32 hwirq, فراغ *priv)
{
	بنية sbi_irqchip_hwirq_معلومات *معلومات;

	إذا (!chip || chip->عدد_hwirq <= hwirq)
	     أرجع SBI_EINVAL;

	معلومات = &chip->hwirqs[hwirq];
	معلومات->priv = priv;

	أرجع 0;
}

فراغ *sbi_irqchip_get_hwirq_priv(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_معلومات *معلومات;
	إذا (!chip || chip->عدد_hwirq <= hwirq)
	    أرجع عدم;

	معلومات = &chip->hwirqs[hwirq];

	أرجع معلومات->priv;
}

صحيح sbi_irqchip_raw_handler_default(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_handler *h;
	صحيح rc = SBI_OK;

	إذا (!chip || chip->عدد_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	h = sbi_irqchip_پیدا_کړه_handler(chip, hwirq);
	إذا (!h)
		أرجع SBI_EINVAL;

	إذا (h->callback)
		rc = h->callback(hwirq, h->priv);

	إذا (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	أرجع rc;
}

صحيح sbi_irqchip_سېټ_raw_handler(بنية sbi_irqchip_device *chip, u32 hwirq,
				صحيح (*raw_hndl)(بنية sbi_irqchip_device *, u32))
{
	بنية sbi_irqchip_hwirq_معلومات *معلومات;

	إذا (!chip || chip->عدد_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	معلومات = &chip->hwirqs[hwirq];
	معلومات->raw_handler = raw_hndl;
	أرجع 0;
}

صحيح sbi_irqchip_write_msi(بنية sbi_irqchip_device *chip, u32 hwirq,
			  ثابت بنية sbi_irqchip_msi_پیغام *پیغام)
{
	بنية sbi_irqchip_handler *h;

	إذا (!chip || chip->عدد_hwirq <= hwirq || !پیغام)
		أرجع SBI_EINVAL;

	h = sbi_irqchip_پیدا_کړه_handler(chip, hwirq);
	إذا (!h)
		أرجع SBI_EFAIL;
	إذا (!h->write_msi)
		أرجع SBI_ENOTSUPP;

	h->write_msi(hwirq, پیغام, h->priv);
	أرجع 0;
}

صحيح sbi_irqchip_get_affinity(بنية sbi_irqchip_device *chip, u32 hwirq,
			     u32 *بهر_hart_شاخص)
{
	إذا (!chip || chip->عدد_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	إذا (!sbi_irqchip_پیدا_کړه_handler(chip, hwirq))
		أرجع SBI_ENOTSUPP;

	*بهر_hart_شاخص = chip->hwirqs[hwirq].hart_شاخص;
	أرجع 0;
}

صحيح sbi_irqchip_سېټ_affinity(بنية sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_شاخص)
{
	بنية sbi_irqchip_hwirq_معلومات *معلومات;
	صحيح rc;

	إذا (!chip || chip->عدد_hwirq <= hwirq || sbi_hart_شمېر() <= hart_شاخص)
		أرجع SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	إذا (!sbi_irqchip_پیدا_کړه_handler(chip, hwirq))
		أرجع SBI_ENOTSUPP;

	معلومات = &chip->hwirqs[hwirq];
	إذا (معلومات->hart_شاخص != hart_شاخص) {
		إذا (chip->hwirq_سېټ_affinity) {
			rc = chip->hwirq_سېټ_affinity(chip, hwirq, hart_شاخص);
			إذا (rc)
				أرجع rc;
		}
		معلومات->hart_شاخص = hart_شاخص;
	}

	أرجع 0;
}

ساكن صحيح __sbi_irqchip_handler_set_affinity(بنية sbi_irqchip_device *chip,
					      بنية sbi_irqchip_handler *h,
					      u32 compare_hart_شاخص,
					      u32 hart_شاخص)
{
	u32 i, current_hart_شاخص;
	صحيح rc;

	لكل (i = 0; i < h->عدد_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->لومړی_hwirq + i,
					      &current_hart_شاخص);
		إذا (rc)
			أرجع rc;

		إذا (compare_hart_شاخص != -1U &&
		    current_hart_شاخص != compare_hart_شاخص)
			استمر;

		rc = sbi_irqchip_سېټ_affinity(chip, h->لومړی_hwirq + i, hart_شاخص);
		إذا (rc)
			أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح __sbi_irqchip_register_handler(بنية sbi_irqchip_device *chip,
					  u32 لومړی_hwirq, u32 عدد_hwirq, u32 hwirq_flags,
					  فراغ (*write_msi)(u32 hwirq,
							    ثابت بنية sbi_irqchip_msi_پیغام *پیغام,
							    فراغ *priv),
					  صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv)
{
	بنية sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	صحيح rc;

	لكل (i = لومړی_hwirq; i < (لومړی_hwirq + عدد_hwirq); i++) {
		h = sbi_irqchip_پیدا_کړه_handler(chip, i);
		إذا (h)
			أرجع SBI_EALREADY;
	}

	h = sbi_zalloc(حجم(*h));
	إذا (!h)
		أرجع SBI_ENOMEM;
	h->لومړی_hwirq = لومړی_hwirq;
	h->عدد_hwirq = عدد_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = عدم;
	sbi_list_for_each_ثبت(th, &chip->handler_list, غوټه) {
		إذا (h->لومړی_hwirq < th->لومړی_hwirq) {
			nh = th;
			اكسر;
		}
	}
	إذا (nh)
		sbi_list_جمع_کړه(&h->غوټه, &nh->غوټه);
	وإلا
		sbi_list_جمع_کړه_لکۍ(&h->غوټه, &chip->handler_list);

	إذا (chip->hwirq_تیاری) {
		لكل (i = 0; i < h->عدد_hwirq; i++) {
			rc = chip->hwirq_تیاری(chip, h->لومړی_hwirq + i, hwirq_flags);
			إذا (rc) {
				إذا (chip->hwirq_cleanup) {
					لكل (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->لومړی_hwirq + j);
				}
				sbi_list_del(&h->غوټه);
				sbi_free(h);
				أرجع rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	إذا (rc) {
		إذا (chip->hwirq_cleanup) {
			لكل (i = 0; i < h->عدد_hwirq; i++)
				chip->hwirq_cleanup(chip, h->لومړی_hwirq + i);
		}
		sbi_list_del(&h->غوټه);
		sbi_free(h);
		أرجع rc;
	}

	إذا (chip->hwirq_unmask) {
		لكل (i = 0; i < h->عدد_hwirq; i++)
			chip->hwirq_unmask(chip, h->لومړی_hwirq + i);
	}

	أرجع 0;
}

صحيح sbi_irqchip_ثبتوونکی_msi(بنية sbi_irqchip_device *chip, u32 عدد_hwirq,
			     فراغ (*write_msi)(u32 hwirq,
					       ثابت بنية sbi_irqchip_msi_پیغام *پیغام,
					       فراغ *priv),
			     صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv,
			     u32 *بهر_لومړی_hwirq)
{
	بنية sbi_irqchip_handler *h;
	bool وموندل;
	u32 hwirq;

	إذا (!chip || !chip->hwirq_سېټ_affinity || !عدد_hwirq ||
	    !write_msi || !callback || !بهر_لومړی_hwirq)
		أرجع SBI_EINVAL;
	إذا (chip->عدد_hwirq < عدد_hwirq)
		أرجع SBI_EBAD_RANGE;

	hwirq = 0;
	وموندل = false;
	sbi_list_for_each_ثبت(h, &chip->handler_list, غوټه) {
		إذا (h->لومړی_hwirq <= hwirq && hwirq < (h->لومړی_hwirq + h->عدد_hwirq)) {
			hwirq = h->لومړی_hwirq + h->عدد_hwirq;
		} وإلا إذا (hwirq < h->لومړی_hwirq) {
			إذا (h->لومړی_hwirq - hwirq >= عدد_hwirq) {
				وموندل = true;
				اكسر;
			} وإلا {
				hwirq = h->لومړی_hwirq + h->عدد_hwirq;
			}
		}
	}
	إذا (!وموندل && (chip->عدد_hwirq - hwirq) >= عدد_hwirq)
		وموندل = true;
	إذا (!وموندل)
		أرجع SBI_ENOSPC;
	*بهر_لومړی_hwirq = hwirq;

	أرجع __sbi_irqchip_register_handler(chip, *بهر_لومړی_hwirq,
					      عدد_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

صحيح sbi_irqchip_ثبتوونکی_handler(بنية sbi_irqchip_device *chip,
				 u32 لومړی_hwirq, u32 عدد_hwirq, u32 hwirq_flags,
				 صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv)
{
	إذا (!chip || !عدد_hwirq || !callback)
		أرجع SBI_EINVAL;
	إذا (chip->عدد_hwirq <= لومړی_hwirq ||
	    chip->عدد_hwirq <= (لومړی_hwirq + عدد_hwirq - 1))
		أرجع SBI_EBAD_RANGE;

	أرجع __sbi_irqchip_register_handler(chip, لومړی_hwirq, عدد_hwirq, hwirq_flags,
					      عدم, callback, priv);
}

صحيح sbi_irqchip_ثبتوونکی_reserved(بنية sbi_irqchip_device *chip,
				  u32 لومړی_hwirq, u32 عدد_hwirq)
{
	إذا (!chip || !عدد_hwirq)
		أرجع SBI_EINVAL;
	إذا (chip->عدد_hwirq <= لومړی_hwirq ||
	    chip->عدد_hwirq <= (لومړی_hwirq + عدد_hwirq - 1))
		أرجع SBI_EBAD_RANGE;

	أرجع __sbi_irqchip_register_handler(chip, لومړی_hwirq, عدد_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, عدم, عدم, عدم);
}

صحيح sbi_irqchip_unregister_handler(بنية sbi_irqchip_device *chip,
				   u32 لومړی_hwirq, u32 عدد_hwirq)
{
	بنية sbi_irqchip_handler *fh, *lh;
	u32 i;

	إذا (!chip || !عدد_hwirq)
		أرجع SBI_EINVAL;
	إذا (chip->عدد_hwirq <= لومړی_hwirq ||
	    chip->عدد_hwirq <= (لومړی_hwirq + عدد_hwirq - 1))
		أرجع SBI_EBAD_RANGE;

	fh = sbi_irqchip_پیدا_کړه_handler(chip, لومړی_hwirq);
	إذا (!fh || fh->لومړی_hwirq != لومړی_hwirq || fh->عدد_hwirq != عدد_hwirq)
		أرجع SBI_ENODEV;
	lh = sbi_irqchip_پیدا_کړه_handler(chip, لومړی_hwirq + عدد_hwirq - 1);
	إذا (!lh || lh != fh)
		أرجع SBI_ENODEV;

	إذا (chip->hwirq_ماسک) {
		لكل (i = 0; i < fh->عدد_hwirq; i++)
			chip->hwirq_ماسک(chip, fh->لومړی_hwirq + i);
	}

	إذا (chip->hwirq_cleanup) {
		لكل (i = 0; i < fh->عدد_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->لومړی_hwirq + i);
	}

	sbi_list_del(&fh->غوټه);
	أرجع 0;
}

بنية sbi_irqchip_device *sbi_irqchip_پیدا_کړه_device_by_caps(بینښه اوږد caps,
							   بنية sbi_irqchip_device *لومړی)
{
	بنية sbi_irqchip_device *chip;
	bool وموندل = (لومړی == عدم);

	sbi_list_for_each_ثبت(chip, &irqchip_list, غوټه) {
		إذا (!وموندل) {
			إذا (لومړی == chip)
				وموندل = true;
			استمر;
		}
		إذا ((chip->caps & caps) == caps)
			أرجع chip;
	}

	أرجع عدم;
}

بنية sbi_irqchip_device *sbi_irqchip_پیدا_کړه_device(u32 id)
{
	بنية sbi_irqchip_device *chip;

	sbi_list_for_each_ثبت(chip, &irqchip_list, غوټه) {
		إذا (chip->id == id)
			أرجع chip;
	}

	أرجع عدم;
}

صحيح sbi_irqchip_جمع_کړه_device(بنية sbi_irqchip_device *chip)
{
	بنية sbi_irqchip_hart_معلومات *hd;
	بنية sbi_scratch *scratch;
	u32 i, h;

	إذا (!chip || !chip->عدد_hwirq || !sbi_hartmask_وزن(&chip->موخه_harts))
		أرجع SBI_EINVAL;

	إذا (sbi_irqchip_پیدا_کړه_device(chip->id))
		أرجع SBI_EALREADY;

	إذا (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->موخه_harts) {
			scratch = sbi_hartindex_منزل_scratch(h);
			إذا (!scratch)
				استمر;

			hd = sbi_scratch_دننه_واټن_ptr(scratch, irqchip_hart_معلومات_off);
			إذا (hd->chip && hd->chip != chip)
				أرجع SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(حجم(*chip->hwirqs) * chip->عدد_hwirq);
	إذا (!chip->hwirqs)
		أرجع SBI_ENOMEM;
	لكل (i = 0; i < chip->عدد_hwirq; i++) {
		sbi_irqchip_سېټ_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_شاخص = -1U;
	}

	SBI_INIT_LIST_سر(&chip->handler_list);

	sbi_list_جمع_کړه_لکۍ(&chip->غوټه, &irqchip_list);
	أرجع 0;
}

صحيح sbi_irqchip_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);
	بنية sbi_irqchip_hart_معلومات *hd;
	بنية sbi_irqchip_device *chip;
	صحيح rc;

	إذا (cold_boot) {
		irqchip_hart_معلومات_off =
			sbi_scratch_alloc_دننه_واټن(حجم(بنية sbi_irqchip_hart_معلومات));
		إذا (!irqchip_hart_معلومات_off)
			أرجع SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		إذا (rc)
			أرجع rc;
	}

	sbi_list_for_each_ثبت(chip, &irqchip_list, غوټه) {
		إذا (!chip->warm_init)
			استمر;
		إذا (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->موخه_harts))
			استمر;
		rc = chip->warm_init(chip);
		إذا (rc)
			أرجع rc;
	}

	hd = sbi_scratch_thishart_دننه_واټن_ptr(irqchip_hart_معلومات_off);
	إذا (hd && hd->chip && hd->chip->process_hwirqs)
		csr_سېټ(CSR_MIE, MIP_MEIP);

	أرجع 0;
}

فراغ sbi_irqchip_exit(بنية sbi_scratch *scratch)
{
	بنية sbi_irqchip_hart_معلومات *hd;
	بنية sbi_irqchip_device *chip;
	بنية sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	صحيح rc;

	sbi_for_each_hartindex(i) {
		إذا (i == current_hartindex())
			استمر;
		إذا (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			استمر;
		migrate_hidx = i;
		migrate = true;
		اكسر;
	}

	إذا (!migrate)
		اذهب skip_migrate;
	sbi_list_for_each_ثبت(chip, &irqchip_list, غوټه) {
		sbi_list_for_each_ثبت(h, &chip->handler_list, غوټه) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			إذا (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->لومړی_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_دننه_واټن_ptr(irqchip_hart_معلومات_off);
	إذا (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
