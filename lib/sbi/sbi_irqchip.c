/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#شامل <sbi/sbi_console.h>
#شامل <sbi/sbi_heap.h>
#شامل <sbi/sbi_hsm.h>
#شامل <sbi/sbi_irqchip.h>
#شامل <sbi/sbi_list.h>
#شامل <sbi/sbi_platform.h>
#شامل <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
ساخت sbi_irqchip_hwirq_ڈیٹا {
	/** raw hardware interrupt handler */
	صحیح (*raw_handler)(ساخت sbi_irqchip_device *chip, u32 hwirq);

#تعریف_کرو IRQ_ENABLED	ثنائی_ہندسہ(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_اشاریہ;

	/** chip's private data */
	خالی *priv;
};

/** Internal irqchip interrupt handler */
ساخت sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	ساخت sbi_dlist گرہ;

	/** First hardware IRQ handled by this handler */
	u32 پہلا_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 عدد_hwirq;

	/** Write MSI function of this handler */
	خالی (*write_msi)(u32 hwirq, ثابت ساخت sbi_irqchip_msi_پیغام *پیغام, خالی *priv);

	/** Callback function of this handler */
	صحیح (*callback)(u32 hwirq, خالی *priv);

	/** Callback private data */
	خالی *priv;
};

ساخت sbi_irqchip_hart_ڈیٹا {
	ساخت sbi_irqchip_device *chip;
};

ساکن غیرعلامتی طویل irqchip_hart_ڈیٹا_off;
ساکن SBI_LIST_سر(irqchip_list);

صحیح sbi_irqchip_process(خالی)
{
	ساخت sbi_irqchip_hart_ڈیٹا *hd;

	hd = sbi_scratch_thishart_انحراف_ptr(irqchip_hart_ڈیٹا_off);
	اگر (!hd || !hd->chip || !hd->chip->process_hwirqs)
		واپس SBI_ENODEV;

	واپس hd->chip->process_hwirqs(hd->chip);
}

صحیح sbi_irqchip_process_hwirq(ساخت sbi_irqchip_device *chip, u32 hwirq)
{
	ساخت sbi_irqchip_hwirq_ڈیٹا *ڈیٹا;

	اگر (!chip || chip->عدد_hwirq <= hwirq)
		واپس SBI_EINVAL;

	ڈیٹا = &chip->hwirqs[hwirq];
	اگر (!ڈیٹا->raw_handler)
		واپس SBI_ENOENT;

	واپس ڈیٹا->raw_handler(chip, hwirq);
}

ساکن اندرونی u32 sbi_irqchip_get_irq_state(ساخت sbi_irqchip_device *chip,
					     u32 hwirq)
{
	اگر (!chip || !chip->hwirqs || hwirq >= chip->عدد_hwirq)
		واپس 0;

	واپس chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(ساخت sbi_irqchip_device *chip,
				   u32 hwirq)
{
	واپس !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

صحیح sbi_irqchip_unmask_hwirq(ساخت sbi_irqchip_device *chip, u32 hwirq)
{
	ساخت sbi_irqchip_hwirq_ڈیٹا *ڈیٹا;

	اگر (!chip || chip->عدد_hwirq <= hwirq)
		واپس SBI_EINVAL;

	ڈیٹا = &chip->hwirqs[hwirq];
	اگر (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		واپس SBI_EALREADY;

	اگر (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	ڈیٹا->irq_state |= IRQ_ENABLED;
	واپس 0;
}

صحیح sbi_irqchip_نقاب_hwirq(ساخت sbi_irqchip_device *chip, u32 hwirq)
{
	ساخت sbi_irqchip_hwirq_ڈیٹا *ڈیٹا;

	اگر (!chip || chip->عدد_hwirq <= hwirq)
		واپس SBI_EINVAL;

	اگر (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		واپس SBI_EALREADY;

	اگر (chip->hwirq_نقاب)
		chip->hwirq_نقاب(chip, hwirq);

	ڈیٹا = &chip->hwirqs[hwirq];
	ڈیٹا->irq_state &= ~IRQ_ENABLED;
	واپس 0;
}

ساکن ساخت sbi_irqchip_handler *sbi_irqchip_ڈھونڈو_handler(ساخت sbi_irqchip_device *chip,
							    u32 hwirq)
{
	ساخت sbi_irqchip_handler *h;

	اگر (!chip || chip->عدد_hwirq <= hwirq)
		واپس تہی;

	sbi_list_for_each_اندراج(h, &chip->handler_list, گرہ) {
		اگر (h->پہلا_hwirq <= hwirq && hwirq < (h->پہلا_hwirq + h->عدد_hwirq))
			واپس h;
	}

	واپس تہی;
}

صحیح sbi_irqchip_سیٹ_hwirq_priv(ساخت sbi_irqchip_device *chip, u32 hwirq, خالی *priv)
{
	ساخت sbi_irqchip_hwirq_ڈیٹا *ڈیٹا;

	اگر (!chip || chip->عدد_hwirq <= hwirq)
	     واپس SBI_EINVAL;

	ڈیٹا = &chip->hwirqs[hwirq];
	ڈیٹا->priv = priv;

	واپس 0;
}

خالی *sbi_irqchip_get_hwirq_priv(ساخت sbi_irqchip_device *chip, u32 hwirq)
{
	ساخت sbi_irqchip_hwirq_ڈیٹا *ڈیٹا;
	اگر (!chip || chip->عدد_hwirq <= hwirq)
	    واپس تہی;

	ڈیٹا = &chip->hwirqs[hwirq];

	واپس ڈیٹا->priv;
}

صحیح sbi_irqchip_raw_handler_default(ساخت sbi_irqchip_device *chip, u32 hwirq)
{
	ساخت sbi_irqchip_handler *h;
	صحیح rc = SBI_OK;

	اگر (!chip || chip->عدد_hwirq <= hwirq)
		واپس SBI_EINVAL;

	h = sbi_irqchip_ڈھونڈو_handler(chip, hwirq);
	اگر (!h)
		واپس SBI_EINVAL;

	اگر (h->callback)
		rc = h->callback(hwirq, h->priv);

	اگر (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	واپس rc;
}

صحیح sbi_irqchip_سیٹ_raw_handler(ساخت sbi_irqchip_device *chip, u32 hwirq,
				صحیح (*raw_hndl)(ساخت sbi_irqchip_device *, u32))
{
	ساخت sbi_irqchip_hwirq_ڈیٹا *ڈیٹا;

	اگر (!chip || chip->عدد_hwirq <= hwirq)
		واپس SBI_EINVAL;

	ڈیٹا = &chip->hwirqs[hwirq];
	ڈیٹا->raw_handler = raw_hndl;
	واپس 0;
}

صحیح sbi_irqchip_write_msi(ساخت sbi_irqchip_device *chip, u32 hwirq,
			  ثابت ساخت sbi_irqchip_msi_پیغام *پیغام)
{
	ساخت sbi_irqchip_handler *h;

	اگر (!chip || chip->عدد_hwirq <= hwirq || !پیغام)
		واپس SBI_EINVAL;

	h = sbi_irqchip_ڈھونڈو_handler(chip, hwirq);
	اگر (!h)
		واپس SBI_EFAIL;
	اگر (!h->write_msi)
		واپس SBI_ENOTSUPP;

	h->write_msi(hwirq, پیغام, h->priv);
	واپس 0;
}

صحیح sbi_irqchip_get_affinity(ساخت sbi_irqchip_device *chip, u32 hwirq,
			     u32 *برآمد_hart_اشاریہ)
{
	اگر (!chip || chip->عدد_hwirq <= hwirq)
		واپس SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	اگر (!sbi_irqchip_ڈھونڈو_handler(chip, hwirq))
		واپس SBI_ENOTSUPP;

	*برآمد_hart_اشاریہ = chip->hwirqs[hwirq].hart_اشاریہ;
	واپس 0;
}

صحیح sbi_irqchip_سیٹ_affinity(ساخت sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_اشاریہ)
{
	ساخت sbi_irqchip_hwirq_ڈیٹا *ڈیٹا;
	صحیح rc;

	اگر (!chip || chip->عدد_hwirq <= hwirq || sbi_hart_گنتی() <= hart_اشاریہ)
		واپس SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	اگر (!sbi_irqchip_ڈھونڈو_handler(chip, hwirq))
		واپس SBI_ENOTSUPP;

	ڈیٹا = &chip->hwirqs[hwirq];
	اگر (ڈیٹا->hart_اشاریہ != hart_اشاریہ) {
		اگر (chip->hwirq_سیٹ_affinity) {
			rc = chip->hwirq_سیٹ_affinity(chip, hwirq, hart_اشاریہ);
			اگر (rc)
				واپس rc;
		}
		ڈیٹا->hart_اشاریہ = hart_اشاریہ;
	}

	واپس 0;
}

ساکن صحیح __sbi_irqchip_handler_set_affinity(ساخت sbi_irqchip_device *chip,
					      ساخت sbi_irqchip_handler *h,
					      u32 compare_hart_اشاریہ,
					      u32 hart_اشاریہ)
{
	u32 i, current_hart_اشاریہ;
	صحیح rc;

	برائے (i = 0; i < h->عدد_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->پہلا_hwirq + i,
					      &current_hart_اشاریہ);
		اگر (rc)
			واپس rc;

		اگر (compare_hart_اشاریہ != -1U &&
		    current_hart_اشاریہ != compare_hart_اشاریہ)
			جاری;

		rc = sbi_irqchip_سیٹ_affinity(chip, h->پہلا_hwirq + i, hart_اشاریہ);
		اگر (rc)
			واپس rc;
	}

	واپس 0;
}

ساکن صحیح __sbi_irqchip_register_handler(ساخت sbi_irqchip_device *chip,
					  u32 پہلا_hwirq, u32 عدد_hwirq, u32 hwirq_flags,
					  خالی (*write_msi)(u32 hwirq,
							    ثابت ساخت sbi_irqchip_msi_پیغام *پیغام,
							    خالی *priv),
					  صحیح (*callback)(u32 hwirq, خالی *priv), خالی *priv)
{
	ساخت sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	صحیح rc;

	برائے (i = پہلا_hwirq; i < (پہلا_hwirq + عدد_hwirq); i++) {
		h = sbi_irqchip_ڈھونڈو_handler(chip, i);
		اگر (h)
			واپس SBI_EALREADY;
	}

	h = sbi_zalloc(حجم(*h));
	اگر (!h)
		واپس SBI_ENOMEM;
	h->پہلا_hwirq = پہلا_hwirq;
	h->عدد_hwirq = عدد_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = تہی;
	sbi_list_for_each_اندراج(th, &chip->handler_list, گرہ) {
		اگر (h->پہلا_hwirq < th->پہلا_hwirq) {
			nh = th;
			توڑو;
		}
	}
	اگر (nh)
		sbi_list_جوڑو(&h->گرہ, &nh->گرہ);
	ورنہ
		sbi_list_جوڑو_دم(&h->گرہ, &chip->handler_list);

	اگر (chip->hwirq_تیاری) {
		برائے (i = 0; i < h->عدد_hwirq; i++) {
			rc = chip->hwirq_تیاری(chip, h->پہلا_hwirq + i, hwirq_flags);
			اگر (rc) {
				اگر (chip->hwirq_cleanup) {
					برائے (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->پہلا_hwirq + j);
				}
				sbi_list_del(&h->گرہ);
				sbi_free(h);
				واپس rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	اگر (rc) {
		اگر (chip->hwirq_cleanup) {
			برائے (i = 0; i < h->عدد_hwirq; i++)
				chip->hwirq_cleanup(chip, h->پہلا_hwirq + i);
		}
		sbi_list_del(&h->گرہ);
		sbi_free(h);
		واپس rc;
	}

	اگر (chip->hwirq_unmask) {
		برائے (i = 0; i < h->عدد_hwirq; i++)
			chip->hwirq_unmask(chip, h->پہلا_hwirq + i);
	}

	واپس 0;
}

صحیح sbi_irqchip_رجسٹر_msi(ساخت sbi_irqchip_device *chip, u32 عدد_hwirq,
			     خالی (*write_msi)(u32 hwirq,
					       ثابت ساخت sbi_irqchip_msi_پیغام *پیغام,
					       خالی *priv),
			     صحیح (*callback)(u32 hwirq, خالی *priv), خالی *priv,
			     u32 *برآمد_پہلا_hwirq)
{
	ساخت sbi_irqchip_handler *h;
	bool ملا;
	u32 hwirq;

	اگر (!chip || !chip->hwirq_سیٹ_affinity || !عدد_hwirq ||
	    !write_msi || !callback || !برآمد_پہلا_hwirq)
		واپس SBI_EINVAL;
	اگر (chip->عدد_hwirq < عدد_hwirq)
		واپس SBI_EBAD_RANGE;

	hwirq = 0;
	ملا = false;
	sbi_list_for_each_اندراج(h, &chip->handler_list, گرہ) {
		اگر (h->پہلا_hwirq <= hwirq && hwirq < (h->پہلا_hwirq + h->عدد_hwirq)) {
			hwirq = h->پہلا_hwirq + h->عدد_hwirq;
		} ورنہ اگر (hwirq < h->پہلا_hwirq) {
			اگر (h->پہلا_hwirq - hwirq >= عدد_hwirq) {
				ملا = true;
				توڑو;
			} ورنہ {
				hwirq = h->پہلا_hwirq + h->عدد_hwirq;
			}
		}
	}
	اگر (!ملا && (chip->عدد_hwirq - hwirq) >= عدد_hwirq)
		ملا = true;
	اگر (!ملا)
		واپس SBI_ENOSPC;
	*برآمد_پہلا_hwirq = hwirq;

	واپس __sbi_irqchip_register_handler(chip, *برآمد_پہلا_hwirq,
					      عدد_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

صحیح sbi_irqchip_رجسٹر_handler(ساخت sbi_irqchip_device *chip,
				 u32 پہلا_hwirq, u32 عدد_hwirq, u32 hwirq_flags,
				 صحیح (*callback)(u32 hwirq, خالی *priv), خالی *priv)
{
	اگر (!chip || !عدد_hwirq || !callback)
		واپس SBI_EINVAL;
	اگر (chip->عدد_hwirq <= پہلا_hwirq ||
	    chip->عدد_hwirq <= (پہلا_hwirq + عدد_hwirq - 1))
		واپس SBI_EBAD_RANGE;

	واپس __sbi_irqchip_register_handler(chip, پہلا_hwirq, عدد_hwirq, hwirq_flags,
					      تہی, callback, priv);
}

صحیح sbi_irqchip_رجسٹر_reserved(ساخت sbi_irqchip_device *chip,
				  u32 پہلا_hwirq, u32 عدد_hwirq)
{
	اگر (!chip || !عدد_hwirq)
		واپس SBI_EINVAL;
	اگر (chip->عدد_hwirq <= پہلا_hwirq ||
	    chip->عدد_hwirq <= (پہلا_hwirq + عدد_hwirq - 1))
		واپس SBI_EBAD_RANGE;

	واپس __sbi_irqchip_register_handler(chip, پہلا_hwirq, عدد_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, تہی, تہی, تہی);
}

صحیح sbi_irqchip_unregister_handler(ساخت sbi_irqchip_device *chip,
				   u32 پہلا_hwirq, u32 عدد_hwirq)
{
	ساخت sbi_irqchip_handler *fh, *lh;
	u32 i;

	اگر (!chip || !عدد_hwirq)
		واپس SBI_EINVAL;
	اگر (chip->عدد_hwirq <= پہلا_hwirq ||
	    chip->عدد_hwirq <= (پہلا_hwirq + عدد_hwirq - 1))
		واپس SBI_EBAD_RANGE;

	fh = sbi_irqchip_ڈھونڈو_handler(chip, پہلا_hwirq);
	اگر (!fh || fh->پہلا_hwirq != پہلا_hwirq || fh->عدد_hwirq != عدد_hwirq)
		واپس SBI_ENODEV;
	lh = sbi_irqchip_ڈھونڈو_handler(chip, پہلا_hwirq + عدد_hwirq - 1);
	اگر (!lh || lh != fh)
		واپس SBI_ENODEV;

	اگر (chip->hwirq_نقاب) {
		برائے (i = 0; i < fh->عدد_hwirq; i++)
			chip->hwirq_نقاب(chip, fh->پہلا_hwirq + i);
	}

	اگر (chip->hwirq_cleanup) {
		برائے (i = 0; i < fh->عدد_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->پہلا_hwirq + i);
	}

	sbi_list_del(&fh->گرہ);
	واپس 0;
}

ساخت sbi_irqchip_device *sbi_irqchip_ڈھونڈو_device_by_caps(غیرعلامتی طویل caps,
							   ساخت sbi_irqchip_device *پہلا)
{
	ساخت sbi_irqchip_device *chip;
	bool ملا = (پہلا == تہی);

	sbi_list_for_each_اندراج(chip, &irqchip_list, گرہ) {
		اگر (!ملا) {
			اگر (پہلا == chip)
				ملا = true;
			جاری;
		}
		اگر ((chip->caps & caps) == caps)
			واپس chip;
	}

	واپس تہی;
}

ساخت sbi_irqchip_device *sbi_irqchip_ڈھونڈو_device(u32 id)
{
	ساخت sbi_irqchip_device *chip;

	sbi_list_for_each_اندراج(chip, &irqchip_list, گرہ) {
		اگر (chip->id == id)
			واپس chip;
	}

	واپس تہی;
}

صحیح sbi_irqchip_جوڑو_device(ساخت sbi_irqchip_device *chip)
{
	ساخت sbi_irqchip_hart_ڈیٹا *hd;
	ساخت sbi_scratch *scratch;
	u32 i, h;

	اگر (!chip || !chip->عدد_hwirq || !sbi_hartmask_وزن(&chip->ہدف_harts))
		واپس SBI_EINVAL;

	اگر (sbi_irqchip_ڈھونڈو_device(chip->id))
		واپس SBI_EALREADY;

	اگر (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->ہدف_harts) {
			scratch = sbi_hartindex_مقصد_scratch(h);
			اگر (!scratch)
				جاری;

			hd = sbi_scratch_انحراف_ptr(scratch, irqchip_hart_ڈیٹا_off);
			اگر (hd->chip && hd->chip != chip)
				واپس SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(حجم(*chip->hwirqs) * chip->عدد_hwirq);
	اگر (!chip->hwirqs)
		واپس SBI_ENOMEM;
	برائے (i = 0; i < chip->عدد_hwirq; i++) {
		sbi_irqchip_سیٹ_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_اشاریہ = -1U;
	}

	SBI_INIT_LIST_سر(&chip->handler_list);

	sbi_list_جوڑو_دم(&chip->گرہ, &irqchip_list);
	واپس 0;
}

صحیح sbi_irqchip_init(ساخت sbi_scratch *scratch, bool cold_boot)
{
	ثابت ساخت sbi_platform *plat = sbi_platform_ptr(scratch);
	ساخت sbi_irqchip_hart_ڈیٹا *hd;
	ساخت sbi_irqchip_device *chip;
	صحیح rc;

	اگر (cold_boot) {
		irqchip_hart_ڈیٹا_off =
			sbi_scratch_alloc_انحراف(حجم(ساخت sbi_irqchip_hart_ڈیٹا));
		اگر (!irqchip_hart_ڈیٹا_off)
			واپس SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		اگر (rc)
			واپس rc;
	}

	sbi_list_for_each_اندراج(chip, &irqchip_list, گرہ) {
		اگر (!chip->warm_init)
			جاری;
		اگر (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->ہدف_harts))
			جاری;
		rc = chip->warm_init(chip);
		اگر (rc)
			واپس rc;
	}

	hd = sbi_scratch_thishart_انحراف_ptr(irqchip_hart_ڈیٹا_off);
	اگر (hd && hd->chip && hd->chip->process_hwirqs)
		csr_سیٹ(CSR_MIE, MIP_MEIP);

	واپس 0;
}

خالی sbi_irqchip_exit(ساخت sbi_scratch *scratch)
{
	ساخت sbi_irqchip_hart_ڈیٹا *hd;
	ساخت sbi_irqchip_device *chip;
	ساخت sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	صحیح rc;

	sbi_for_each_hartindex(i) {
		اگر (i == current_hartindex())
			جاری;
		اگر (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			جاری;
		migrate_hidx = i;
		migrate = true;
		توڑو;
	}

	اگر (!migrate)
		جاؤ skip_migrate;
	sbi_list_for_each_اندراج(chip, &irqchip_list, گرہ) {
		sbi_list_for_each_اندراج(h, &chip->handler_list, گرہ) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			اگر (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->پہلا_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_انحراف_ptr(irqchip_hart_ڈیٹا_off);
	اگر (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
