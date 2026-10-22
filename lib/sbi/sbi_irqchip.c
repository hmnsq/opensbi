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

#وصف_ڪريو IRQ_ENABLED	ٻٽو_انگ(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_اشاريو;

	/** chip's private data */
	فراغ *priv;
};

/** Internal irqchip interrupt handler */
بنية sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	بنية sbi_dlist ڳنڍ;

	/** First hardware IRQ handled by this handler */
	u32 پهريون_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 انگ_hwirq;

	/** Write MSI function of this handler */
	فراغ (*write_msi)(u32 hwirq, ثابت بنية sbi_irqchip_msi_نياپو *نياپو, فراغ *priv);

	/** Callback function of this handler */
	صحيح (*callback)(u32 hwirq, فراغ *priv);

	/** Callback private data */
	فراغ *priv;
};

بنية sbi_irqchip_hart_معلومات {
	بنية sbi_irqchip_device *chip;
};

ساكن بنانشان ڊگهو irqchip_hart_معلومات_off;
ساكن SBI_LIST_سر(irqchip_list);

صحيح sbi_irqchip_process(فراغ)
{
	بنية sbi_irqchip_hart_معلومات *hd;

	hd = sbi_scratch_thishart_وٿي_ptr(irqchip_hart_معلومات_off);
	إذا (!hd || !hd->chip || !hd->chip->process_hwirqs)
		أرجع SBI_ENODEV;

	أرجع hd->chip->process_hwirqs(hd->chip);
}

صحيح sbi_irqchip_process_hwirq(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_معلومات *معلومات;

	إذا (!chip || chip->انگ_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	معلومات = &chip->hwirqs[hwirq];
	إذا (!معلومات->raw_handler)
		أرجع SBI_ENOENT;

	أرجع معلومات->raw_handler(chip, hwirq);
}

ساكن مضمن u32 sbi_irqchip_get_irq_state(بنية sbi_irqchip_device *chip,
					     u32 hwirq)
{
	إذا (!chip || !chip->hwirqs || hwirq >= chip->انگ_hwirq)
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

	إذا (!chip || chip->انگ_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	معلومات = &chip->hwirqs[hwirq];
	إذا (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		أرجع SBI_EALREADY;

	إذا (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	معلومات->irq_state |= IRQ_ENABLED;
	أرجع 0;
}

صحيح sbi_irqchip_نقاب_hwirq(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_معلومات *معلومات;

	إذا (!chip || chip->انگ_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	إذا (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		أرجع SBI_EALREADY;

	إذا (chip->hwirq_نقاب)
		chip->hwirq_نقاب(chip, hwirq);

	معلومات = &chip->hwirqs[hwirq];
	معلومات->irq_state &= ~IRQ_ENABLED;
	أرجع 0;
}

ساكن بنية sbi_irqchip_handler *sbi_irqchip_ڳوليو_handler(بنية sbi_irqchip_device *chip,
							    u32 hwirq)
{
	بنية sbi_irqchip_handler *h;

	إذا (!chip || chip->انگ_hwirq <= hwirq)
		أرجع عدم;

	sbi_list_for_each_داخلا(h, &chip->handler_list, ڳنڍ) {
		إذا (h->پهريون_hwirq <= hwirq && hwirq < (h->پهريون_hwirq + h->انگ_hwirq))
			أرجع h;
	}

	أرجع عدم;
}

صحيح sbi_irqchip_سيٽ_hwirq_priv(بنية sbi_irqchip_device *chip, u32 hwirq, فراغ *priv)
{
	بنية sbi_irqchip_hwirq_معلومات *معلومات;

	إذا (!chip || chip->انگ_hwirq <= hwirq)
	     أرجع SBI_EINVAL;

	معلومات = &chip->hwirqs[hwirq];
	معلومات->priv = priv;

	أرجع 0;
}

فراغ *sbi_irqchip_get_hwirq_priv(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_معلومات *معلومات;
	إذا (!chip || chip->انگ_hwirq <= hwirq)
	    أرجع عدم;

	معلومات = &chip->hwirqs[hwirq];

	أرجع معلومات->priv;
}

صحيح sbi_irqchip_raw_handler_default(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_handler *h;
	صحيح rc = SBI_OK;

	إذا (!chip || chip->انگ_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	h = sbi_irqchip_ڳوليو_handler(chip, hwirq);
	إذا (!h)
		أرجع SBI_EINVAL;

	إذا (h->callback)
		rc = h->callback(hwirq, h->priv);

	إذا (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	أرجع rc;
}

صحيح sbi_irqchip_سيٽ_raw_handler(بنية sbi_irqchip_device *chip, u32 hwirq,
				صحيح (*raw_hndl)(بنية sbi_irqchip_device *, u32))
{
	بنية sbi_irqchip_hwirq_معلومات *معلومات;

	إذا (!chip || chip->انگ_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	معلومات = &chip->hwirqs[hwirq];
	معلومات->raw_handler = raw_hndl;
	أرجع 0;
}

صحيح sbi_irqchip_write_msi(بنية sbi_irqchip_device *chip, u32 hwirq,
			  ثابت بنية sbi_irqchip_msi_نياپو *نياپو)
{
	بنية sbi_irqchip_handler *h;

	إذا (!chip || chip->انگ_hwirq <= hwirq || !نياپو)
		أرجع SBI_EINVAL;

	h = sbi_irqchip_ڳوليو_handler(chip, hwirq);
	إذا (!h)
		أرجع SBI_EFAIL;
	إذا (!h->write_msi)
		أرجع SBI_ENOTSUPP;

	h->write_msi(hwirq, نياپو, h->priv);
	أرجع 0;
}

صحيح sbi_irqchip_get_affinity(بنية sbi_irqchip_device *chip, u32 hwirq,
			     u32 *ٻاهريون_hart_اشاريو)
{
	إذا (!chip || chip->انگ_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	إذا (!sbi_irqchip_ڳوليو_handler(chip, hwirq))
		أرجع SBI_ENOTSUPP;

	*ٻاهريون_hart_اشاريو = chip->hwirqs[hwirq].hart_اشاريو;
	أرجع 0;
}

صحيح sbi_irqchip_سيٽ_affinity(بنية sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_اشاريو)
{
	بنية sbi_irqchip_hwirq_معلومات *معلومات;
	صحيح rc;

	إذا (!chip || chip->انگ_hwirq <= hwirq || sbi_hart_ڳڻپ() <= hart_اشاريو)
		أرجع SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	إذا (!sbi_irqchip_ڳوليو_handler(chip, hwirq))
		أرجع SBI_ENOTSUPP;

	معلومات = &chip->hwirqs[hwirq];
	إذا (معلومات->hart_اشاريو != hart_اشاريو) {
		إذا (chip->hwirq_سيٽ_affinity) {
			rc = chip->hwirq_سيٽ_affinity(chip, hwirq, hart_اشاريو);
			إذا (rc)
				أرجع rc;
		}
		معلومات->hart_اشاريو = hart_اشاريو;
	}

	أرجع 0;
}

ساكن صحيح __sbi_irqchip_handler_set_affinity(بنية sbi_irqchip_device *chip,
					      بنية sbi_irqchip_handler *h,
					      u32 compare_hart_اشاريو,
					      u32 hart_اشاريو)
{
	u32 i, current_hart_اشاريو;
	صحيح rc;

	لكل (i = 0; i < h->انگ_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->پهريون_hwirq + i,
					      &current_hart_اشاريو);
		إذا (rc)
			أرجع rc;

		إذا (compare_hart_اشاريو != -1U &&
		    current_hart_اشاريو != compare_hart_اشاريو)
			استمر;

		rc = sbi_irqchip_سيٽ_affinity(chip, h->پهريون_hwirq + i, hart_اشاريو);
		إذا (rc)
			أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح __sbi_irqchip_register_handler(بنية sbi_irqchip_device *chip,
					  u32 پهريون_hwirq, u32 انگ_hwirq, u32 hwirq_flags,
					  فراغ (*write_msi)(u32 hwirq,
							    ثابت بنية sbi_irqchip_msi_نياپو *نياپو,
							    فراغ *priv),
					  صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv)
{
	بنية sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	صحيح rc;

	لكل (i = پهريون_hwirq; i < (پهريون_hwirq + انگ_hwirq); i++) {
		h = sbi_irqchip_ڳوليو_handler(chip, i);
		إذا (h)
			أرجع SBI_EALREADY;
	}

	h = sbi_zalloc(حجم(*h));
	إذا (!h)
		أرجع SBI_ENOMEM;
	h->پهريون_hwirq = پهريون_hwirq;
	h->انگ_hwirq = انگ_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = عدم;
	sbi_list_for_each_داخلا(th, &chip->handler_list, ڳنڍ) {
		إذا (h->پهريون_hwirq < th->پهريون_hwirq) {
			nh = th;
			اكسر;
		}
	}
	إذا (nh)
		sbi_list_جوڙيو(&h->ڳنڍ, &nh->ڳنڍ);
	وإلا
		sbi_list_جوڙيو_پڇ(&h->ڳنڍ, &chip->handler_list);

	إذا (chip->hwirq_تياري) {
		لكل (i = 0; i < h->انگ_hwirq; i++) {
			rc = chip->hwirq_تياري(chip, h->پهريون_hwirq + i, hwirq_flags);
			إذا (rc) {
				إذا (chip->hwirq_cleanup) {
					لكل (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->پهريون_hwirq + j);
				}
				sbi_list_del(&h->ڳنڍ);
				sbi_free(h);
				أرجع rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	إذا (rc) {
		إذا (chip->hwirq_cleanup) {
			لكل (i = 0; i < h->انگ_hwirq; i++)
				chip->hwirq_cleanup(chip, h->پهريون_hwirq + i);
		}
		sbi_list_del(&h->ڳنڍ);
		sbi_free(h);
		أرجع rc;
	}

	إذا (chip->hwirq_unmask) {
		لكل (i = 0; i < h->انگ_hwirq; i++)
			chip->hwirq_unmask(chip, h->پهريون_hwirq + i);
	}

	أرجع 0;
}

صحيح sbi_irqchip_رجسٽر_msi(بنية sbi_irqchip_device *chip, u32 انگ_hwirq,
			     فراغ (*write_msi)(u32 hwirq,
					       ثابت بنية sbi_irqchip_msi_نياپو *نياپو,
					       فراغ *priv),
			     صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv,
			     u32 *ٻاهريون_پهريون_hwirq)
{
	بنية sbi_irqchip_handler *h;
	bool مليو;
	u32 hwirq;

	إذا (!chip || !chip->hwirq_سيٽ_affinity || !انگ_hwirq ||
	    !write_msi || !callback || !ٻاهريون_پهريون_hwirq)
		أرجع SBI_EINVAL;
	إذا (chip->انگ_hwirq < انگ_hwirq)
		أرجع SBI_EBAD_RANGE;

	hwirq = 0;
	مليو = false;
	sbi_list_for_each_داخلا(h, &chip->handler_list, ڳنڍ) {
		إذا (h->پهريون_hwirq <= hwirq && hwirq < (h->پهريون_hwirq + h->انگ_hwirq)) {
			hwirq = h->پهريون_hwirq + h->انگ_hwirq;
		} وإلا إذا (hwirq < h->پهريون_hwirq) {
			إذا (h->پهريون_hwirq - hwirq >= انگ_hwirq) {
				مليو = true;
				اكسر;
			} وإلا {
				hwirq = h->پهريون_hwirq + h->انگ_hwirq;
			}
		}
	}
	إذا (!مليو && (chip->انگ_hwirq - hwirq) >= انگ_hwirq)
		مليو = true;
	إذا (!مليو)
		أرجع SBI_ENOSPC;
	*ٻاهريون_پهريون_hwirq = hwirq;

	أرجع __sbi_irqchip_register_handler(chip, *ٻاهريون_پهريون_hwirq,
					      انگ_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

صحيح sbi_irqchip_رجسٽر_handler(بنية sbi_irqchip_device *chip,
				 u32 پهريون_hwirq, u32 انگ_hwirq, u32 hwirq_flags,
				 صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv)
{
	إذا (!chip || !انگ_hwirq || !callback)
		أرجع SBI_EINVAL;
	إذا (chip->انگ_hwirq <= پهريون_hwirq ||
	    chip->انگ_hwirq <= (پهريون_hwirq + انگ_hwirq - 1))
		أرجع SBI_EBAD_RANGE;

	أرجع __sbi_irqchip_register_handler(chip, پهريون_hwirq, انگ_hwirq, hwirq_flags,
					      عدم, callback, priv);
}

صحيح sbi_irqchip_رجسٽر_reserved(بنية sbi_irqchip_device *chip,
				  u32 پهريون_hwirq, u32 انگ_hwirq)
{
	إذا (!chip || !انگ_hwirq)
		أرجع SBI_EINVAL;
	إذا (chip->انگ_hwirq <= پهريون_hwirq ||
	    chip->انگ_hwirq <= (پهريون_hwirq + انگ_hwirq - 1))
		أرجع SBI_EBAD_RANGE;

	أرجع __sbi_irqchip_register_handler(chip, پهريون_hwirq, انگ_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, عدم, عدم, عدم);
}

صحيح sbi_irqchip_unregister_handler(بنية sbi_irqchip_device *chip,
				   u32 پهريون_hwirq, u32 انگ_hwirq)
{
	بنية sbi_irqchip_handler *fh, *lh;
	u32 i;

	إذا (!chip || !انگ_hwirq)
		أرجع SBI_EINVAL;
	إذا (chip->انگ_hwirq <= پهريون_hwirq ||
	    chip->انگ_hwirq <= (پهريون_hwirq + انگ_hwirq - 1))
		أرجع SBI_EBAD_RANGE;

	fh = sbi_irqchip_ڳوليو_handler(chip, پهريون_hwirq);
	إذا (!fh || fh->پهريون_hwirq != پهريون_hwirq || fh->انگ_hwirq != انگ_hwirq)
		أرجع SBI_ENODEV;
	lh = sbi_irqchip_ڳوليو_handler(chip, پهريون_hwirq + انگ_hwirq - 1);
	إذا (!lh || lh != fh)
		أرجع SBI_ENODEV;

	إذا (chip->hwirq_نقاب) {
		لكل (i = 0; i < fh->انگ_hwirq; i++)
			chip->hwirq_نقاب(chip, fh->پهريون_hwirq + i);
	}

	إذا (chip->hwirq_cleanup) {
		لكل (i = 0; i < fh->انگ_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->پهريون_hwirq + i);
	}

	sbi_list_del(&fh->ڳنڍ);
	أرجع 0;
}

بنية sbi_irqchip_device *sbi_irqchip_ڳوليو_device_by_caps(بنانشان ڊگهو caps,
							   بنية sbi_irqchip_device *پهريون)
{
	بنية sbi_irqchip_device *chip;
	bool مليو = (پهريون == عدم);

	sbi_list_for_each_داخلا(chip, &irqchip_list, ڳنڍ) {
		إذا (!مليو) {
			إذا (پهريون == chip)
				مليو = true;
			استمر;
		}
		إذا ((chip->caps & caps) == caps)
			أرجع chip;
	}

	أرجع عدم;
}

بنية sbi_irqchip_device *sbi_irqchip_ڳوليو_device(u32 id)
{
	بنية sbi_irqchip_device *chip;

	sbi_list_for_each_داخلا(chip, &irqchip_list, ڳنڍ) {
		إذا (chip->id == id)
			أرجع chip;
	}

	أرجع عدم;
}

صحيح sbi_irqchip_جوڙيو_device(بنية sbi_irqchip_device *chip)
{
	بنية sbi_irqchip_hart_معلومات *hd;
	بنية sbi_scratch *scratch;
	u32 i, h;

	إذا (!chip || !chip->انگ_hwirq || !sbi_hartmask_وزن(&chip->هدف_harts))
		أرجع SBI_EINVAL;

	إذا (sbi_irqchip_ڳوليو_device(chip->id))
		أرجع SBI_EALREADY;

	إذا (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->هدف_harts) {
			scratch = sbi_hartindex_منزل_scratch(h);
			إذا (!scratch)
				استمر;

			hd = sbi_scratch_وٿي_ptr(scratch, irqchip_hart_معلومات_off);
			إذا (hd->chip && hd->chip != chip)
				أرجع SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(حجم(*chip->hwirqs) * chip->انگ_hwirq);
	إذا (!chip->hwirqs)
		أرجع SBI_ENOMEM;
	لكل (i = 0; i < chip->انگ_hwirq; i++) {
		sbi_irqchip_سيٽ_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_اشاريو = -1U;
	}

	SBI_INIT_LIST_سر(&chip->handler_list);

	sbi_list_جوڙيو_پڇ(&chip->ڳنڍ, &irqchip_list);
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
			sbi_scratch_alloc_وٿي(حجم(بنية sbi_irqchip_hart_معلومات));
		إذا (!irqchip_hart_معلومات_off)
			أرجع SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		إذا (rc)
			أرجع rc;
	}

	sbi_list_for_each_داخلا(chip, &irqchip_list, ڳنڍ) {
		إذا (!chip->warm_init)
			استمر;
		إذا (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->هدف_harts))
			استمر;
		rc = chip->warm_init(chip);
		إذا (rc)
			أرجع rc;
	}

	hd = sbi_scratch_thishart_وٿي_ptr(irqchip_hart_معلومات_off);
	إذا (hd && hd->chip && hd->chip->process_hwirqs)
		csr_سيٽ(CSR_MIE, MIP_MEIP);

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
	sbi_list_for_each_داخلا(chip, &irqchip_list, ڳنڍ) {
		sbi_list_for_each_داخلا(h, &chip->handler_list, ڳنڍ) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			إذا (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->پهريون_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_وٿي_ptr(irqchip_hart_معلومات_off);
	إذا (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
