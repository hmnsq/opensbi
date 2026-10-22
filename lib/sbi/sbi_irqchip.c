/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#অন্তর্ভাব <sbi/sbi_console.h>
#অন্তর্ভাব <sbi/sbi_heap.h>
#অন্তর্ভাব <sbi/sbi_hsm.h>
#অন্তর্ভাব <sbi/sbi_irqchip.h>
#অন্তর্ভাব <sbi/sbi_list.h>
#অন্তর্ভাব <sbi/sbi_platform.h>
#অন্তর্ভাব <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
কাঠামো sbi_irqchip_hwirq_তথ্য {
	/** raw hardware interrupt handler */
	পূর্ণসংখ্যা (*raw_handler)(কাঠামো sbi_irqchip_device *chip, u32 hwirq);

#define IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_সূচক;

	/** chip's private data */
	শূন্য *priv;
};

/** Internal irqchip interrupt handler */
কাঠামো sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	কাঠামো sbi_dlist গ্রন্থি;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 অঙ্ক_hwirq;

	/** Write MSI function of this handler */
	শূন্য (*write_msi)(u32 hwirq, ধ্রুবক কাঠামো sbi_irqchip_msi_বার্তা *বার্তা, শূন্য *priv);

	/** Callback function of this handler */
	পূর্ণসংখ্যা (*callback)(u32 hwirq, শূন্য *priv);

	/** Callback private data */
	শূন্য *priv;
};

কাঠামো sbi_irqchip_hart_তথ্য {
	কাঠামো sbi_irqchip_device *chip;
};

স্থির unsigned long irqchip_hart_তথ্য_off;
স্থির SBI_LIST_শীর্ষ(irqchip_list);

পূর্ণসংখ্যা sbi_irqchip_process(শূন্য)
{
	কাঠামো sbi_irqchip_hart_তথ্য *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_তথ্য_off);
	যদি (!hd || !hd->chip || !hd->chip->process_hwirqs)
		ফেরত SBI_ENODEV;

	ফেরত hd->chip->process_hwirqs(hd->chip);
}

পূর্ণসংখ্যা sbi_irqchip_process_hwirq(কাঠামো sbi_irqchip_device *chip, u32 hwirq)
{
	কাঠামো sbi_irqchip_hwirq_তথ্য *তথ্য;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
		ফেরত SBI_EINVAL;

	তথ্য = &chip->hwirqs[hwirq];
	যদি (!তথ্য->raw_handler)
		ফেরত SBI_ENOENT;

	ফেরত তথ্য->raw_handler(chip, hwirq);
}

স্থির অন্তঃরেখ u32 sbi_irqchip_get_irq_state(কাঠামো sbi_irqchip_device *chip,
					     u32 hwirq)
{
	যদি (!chip || !chip->hwirqs || hwirq >= chip->অঙ্ক_hwirq)
		ফেরত 0;

	ফেরত chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(কাঠামো sbi_irqchip_device *chip,
				   u32 hwirq)
{
	ফেরত !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

পূর্ণসংখ্যা sbi_irqchip_unmask_hwirq(কাঠামো sbi_irqchip_device *chip, u32 hwirq)
{
	কাঠামো sbi_irqchip_hwirq_তথ্য *তথ্য;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
		ফেরত SBI_EINVAL;

	তথ্য = &chip->hwirqs[hwirq];
	যদি (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ফেরত SBI_EALREADY;

	যদি (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	তথ্য->irq_state |= IRQ_ENABLED;
	ফেরত 0;
}

পূর্ণসংখ্যা sbi_irqchip_mask_hwirq(কাঠামো sbi_irqchip_device *chip, u32 hwirq)
{
	কাঠামো sbi_irqchip_hwirq_তথ্য *তথ্য;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
		ফেরত SBI_EINVAL;

	যদি (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		ফেরত SBI_EALREADY;

	যদি (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	তথ্য = &chip->hwirqs[hwirq];
	তথ্য->irq_state &= ~IRQ_ENABLED;
	ফেরত 0;
}

স্থির কাঠামো sbi_irqchip_handler *sbi_irqchip_find_handler(কাঠামো sbi_irqchip_device *chip,
							    u32 hwirq)
{
	কাঠামো sbi_irqchip_handler *h;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
		ফেরত ফাঁকা;

	sbi_list_for_each_entry(h, &chip->handler_list, গ্রন্থি) {
		যদি (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->অঙ্ক_hwirq))
			ফেরত h;
	}

	ফেরত ফাঁকা;
}

পূর্ণসংখ্যা sbi_irqchip_set_hwirq_priv(কাঠামো sbi_irqchip_device *chip, u32 hwirq, শূন্য *priv)
{
	কাঠামো sbi_irqchip_hwirq_তথ্য *তথ্য;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
	     ফেরত SBI_EINVAL;

	তথ্য = &chip->hwirqs[hwirq];
	তথ্য->priv = priv;

	ফেরত 0;
}

শূন্য *sbi_irqchip_get_hwirq_priv(কাঠামো sbi_irqchip_device *chip, u32 hwirq)
{
	কাঠামো sbi_irqchip_hwirq_তথ্য *তথ্য;
	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
	    ফেরত ফাঁকা;

	তথ্য = &chip->hwirqs[hwirq];

	ফেরত তথ্য->priv;
}

পূর্ণসংখ্যা sbi_irqchip_raw_handler_default(কাঠামো sbi_irqchip_device *chip, u32 hwirq)
{
	কাঠামো sbi_irqchip_handler *h;
	পূর্ণসংখ্যা rc = SBI_OK;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
		ফেরত SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	যদি (!h)
		ফেরত SBI_EINVAL;

	যদি (h->callback)
		rc = h->callback(hwirq, h->priv);

	যদি (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	ফেরত rc;
}

পূর্ণসংখ্যা sbi_irqchip_set_raw_handler(কাঠামো sbi_irqchip_device *chip, u32 hwirq,
				পূর্ণসংখ্যা (*raw_hndl)(কাঠামো sbi_irqchip_device *, u32))
{
	কাঠামো sbi_irqchip_hwirq_তথ্য *তথ্য;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
		ফেরত SBI_EINVAL;

	তথ্য = &chip->hwirqs[hwirq];
	তথ্য->raw_handler = raw_hndl;
	ফেরত 0;
}

পূর্ণসংখ্যা sbi_irqchip_write_msi(কাঠামো sbi_irqchip_device *chip, u32 hwirq,
			  ধ্রুবক কাঠামো sbi_irqchip_msi_বার্তা *বার্তা)
{
	কাঠামো sbi_irqchip_handler *h;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq || !বার্তা)
		ফেরত SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	যদি (!h)
		ফেরত SBI_EFAIL;
	যদি (!h->write_msi)
		ফেরত SBI_ENOTSUPP;

	h->write_msi(hwirq, বার্তা, h->priv);
	ফেরত 0;
}

পূর্ণসংখ্যা sbi_irqchip_get_affinity(কাঠামো sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_সূচক)
{
	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq)
		ফেরত SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	যদি (!sbi_irqchip_find_handler(chip, hwirq))
		ফেরত SBI_ENOTSUPP;

	*out_hart_সূচক = chip->hwirqs[hwirq].hart_সূচক;
	ফেরত 0;
}

পূর্ণসংখ্যা sbi_irqchip_set_affinity(কাঠামো sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_সূচক)
{
	কাঠামো sbi_irqchip_hwirq_তথ্য *তথ্য;
	পূর্ণসংখ্যা rc;

	যদি (!chip || chip->অঙ্ক_hwirq <= hwirq || sbi_hart_গুনতি() <= hart_সূচক)
		ফেরত SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	যদি (!sbi_irqchip_find_handler(chip, hwirq))
		ফেরত SBI_ENOTSUPP;

	তথ্য = &chip->hwirqs[hwirq];
	যদি (তথ্য->hart_সূচক != hart_সূচক) {
		যদি (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_সূচক);
			যদি (rc)
				ফেরত rc;
		}
		তথ্য->hart_সূচক = hart_সূচক;
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা __sbi_irqchip_handler_set_affinity(কাঠামো sbi_irqchip_device *chip,
					      কাঠামো sbi_irqchip_handler *h,
					      u32 compare_hart_সূচক,
					      u32 hart_সূচক)
{
	u32 i, current_hart_সূচক;
	পূর্ণসংখ্যা rc;

	জন্য (i = 0; i < h->অঙ্ক_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_সূচক);
		যদি (rc)
			ফেরত rc;

		যদি (compare_hart_সূচক != -1U &&
		    current_hart_সূচক != compare_hart_সূচক)
			চালু;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_সূচক);
		যদি (rc)
			ফেরত rc;
	}

	ফেরত 0;
}

স্থির পূর্ণসংখ্যা __sbi_irqchip_register_handler(কাঠামো sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 অঙ্ক_hwirq, u32 hwirq_flags,
					  শূন্য (*write_msi)(u32 hwirq,
							    ধ্রুবক কাঠামো sbi_irqchip_msi_বার্তা *বার্তা,
							    শূন্য *priv),
					  পূর্ণসংখ্যা (*callback)(u32 hwirq, শূন্য *priv), শূন্য *priv)
{
	কাঠামো sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	পূর্ণসংখ্যা rc;

	জন্য (i = first_hwirq; i < (first_hwirq + অঙ্ক_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		যদি (h)
			ফেরত SBI_EALREADY;
	}

	h = sbi_zalloc(আকার(*h));
	যদি (!h)
		ফেরত SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->অঙ্ক_hwirq = অঙ্ক_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = ফাঁকা;
	sbi_list_for_each_entry(th, &chip->handler_list, গ্রন্থি) {
		যদি (h->first_hwirq < th->first_hwirq) {
			nh = th;
			বিরতি;
		}
	}
	যদি (nh)
		sbi_list_add(&h->গ্রন্থি, &nh->গ্রন্থি);
	অন্যথা
		sbi_list_add_লেজ(&h->গ্রন্থি, &chip->handler_list);

	যদি (chip->hwirq_setup) {
		জন্য (i = 0; i < h->অঙ্ক_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			যদি (rc) {
				যদি (chip->hwirq_cleanup) {
					জন্য (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->গ্রন্থি);
				sbi_free(h);
				ফেরত rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	যদি (rc) {
		যদি (chip->hwirq_cleanup) {
			জন্য (i = 0; i < h->অঙ্ক_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->গ্রন্থি);
		sbi_free(h);
		ফেরত rc;
	}

	যদি (chip->hwirq_unmask) {
		জন্য (i = 0; i < h->অঙ্ক_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	ফেরত 0;
}

পূর্ণসংখ্যা sbi_irqchip_register_msi(কাঠামো sbi_irqchip_device *chip, u32 অঙ্ক_hwirq,
			     শূন্য (*write_msi)(u32 hwirq,
					       ধ্রুবক কাঠামো sbi_irqchip_msi_বার্তা *বার্তা,
					       শূন্য *priv),
			     পূর্ণসংখ্যা (*callback)(u32 hwirq, শূন্য *priv), শূন্য *priv,
			     u32 *out_first_hwirq)
{
	কাঠামো sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	যদি (!chip || !chip->hwirq_set_affinity || !অঙ্ক_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		ফেরত SBI_EINVAL;
	যদি (chip->অঙ্ক_hwirq < অঙ্ক_hwirq)
		ফেরত SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, গ্রন্থি) {
		যদি (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->অঙ্ক_hwirq)) {
			hwirq = h->first_hwirq + h->অঙ্ক_hwirq;
		} অন্যথা যদি (hwirq < h->first_hwirq) {
			যদি (h->first_hwirq - hwirq >= অঙ্ক_hwirq) {
				found = true;
				বিরতি;
			} অন্যথা {
				hwirq = h->first_hwirq + h->অঙ্ক_hwirq;
			}
		}
	}
	যদি (!found && (chip->অঙ্ক_hwirq - hwirq) >= অঙ্ক_hwirq)
		found = true;
	যদি (!found)
		ফেরত SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	ফেরত __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      অঙ্ক_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

পূর্ণসংখ্যা sbi_irqchip_register_handler(কাঠামো sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 অঙ্ক_hwirq, u32 hwirq_flags,
				 পূর্ণসংখ্যা (*callback)(u32 hwirq, শূন্য *priv), শূন্য *priv)
{
	যদি (!chip || !অঙ্ক_hwirq || !callback)
		ফেরত SBI_EINVAL;
	যদি (chip->অঙ্ক_hwirq <= first_hwirq ||
	    chip->অঙ্ক_hwirq <= (first_hwirq + অঙ্ক_hwirq - 1))
		ফেরত SBI_EBAD_RANGE;

	ফেরত __sbi_irqchip_register_handler(chip, first_hwirq, অঙ্ক_hwirq, hwirq_flags,
					      ফাঁকা, callback, priv);
}

পূর্ণসংখ্যা sbi_irqchip_register_reserved(কাঠামো sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 অঙ্ক_hwirq)
{
	যদি (!chip || !অঙ্ক_hwirq)
		ফেরত SBI_EINVAL;
	যদি (chip->অঙ্ক_hwirq <= first_hwirq ||
	    chip->অঙ্ক_hwirq <= (first_hwirq + অঙ্ক_hwirq - 1))
		ফেরত SBI_EBAD_RANGE;

	ফেরত __sbi_irqchip_register_handler(chip, first_hwirq, অঙ্ক_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, ফাঁকা, ফাঁকা, ফাঁকা);
}

পূর্ণসংখ্যা sbi_irqchip_unregister_handler(কাঠামো sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 অঙ্ক_hwirq)
{
	কাঠামো sbi_irqchip_handler *fh, *lh;
	u32 i;

	যদি (!chip || !অঙ্ক_hwirq)
		ফেরত SBI_EINVAL;
	যদি (chip->অঙ্ক_hwirq <= first_hwirq ||
	    chip->অঙ্ক_hwirq <= (first_hwirq + অঙ্ক_hwirq - 1))
		ফেরত SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	যদি (!fh || fh->first_hwirq != first_hwirq || fh->অঙ্ক_hwirq != অঙ্ক_hwirq)
		ফেরত SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + অঙ্ক_hwirq - 1);
	যদি (!lh || lh != fh)
		ফেরত SBI_ENODEV;

	যদি (chip->hwirq_mask) {
		জন্য (i = 0; i < fh->অঙ্ক_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	যদি (chip->hwirq_cleanup) {
		জন্য (i = 0; i < fh->অঙ্ক_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->গ্রন্থি);
	ফেরত 0;
}

কাঠামো sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   কাঠামো sbi_irqchip_device *first)
{
	কাঠামো sbi_irqchip_device *chip;
	bool found = (first == ফাঁকা);

	sbi_list_for_each_entry(chip, &irqchip_list, গ্রন্থি) {
		যদি (!found) {
			যদি (first == chip)
				found = true;
			চালু;
		}
		যদি ((chip->caps & caps) == caps)
			ফেরত chip;
	}

	ফেরত ফাঁকা;
}

কাঠামো sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	কাঠামো sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, গ্রন্থি) {
		যদি (chip->id == id)
			ফেরত chip;
	}

	ফেরত ফাঁকা;
}

পূর্ণসংখ্যা sbi_irqchip_add_device(কাঠামো sbi_irqchip_device *chip)
{
	কাঠামো sbi_irqchip_hart_তথ্য *hd;
	কাঠামো sbi_scratch *scratch;
	u32 i, h;

	যদি (!chip || !chip->অঙ্ক_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		ফেরত SBI_EINVAL;

	যদি (sbi_irqchip_find_device(chip->id))
		ফেরত SBI_EALREADY;

	যদি (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			যদি (!scratch)
				চালু;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_তথ্য_off);
			যদি (hd->chip && hd->chip != chip)
				ফেরত SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(আকার(*chip->hwirqs) * chip->অঙ্ক_hwirq);
	যদি (!chip->hwirqs)
		ফেরত SBI_ENOMEM;
	জন্য (i = 0; i < chip->অঙ্ক_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_সূচক = -1U;
	}

	SBI_INIT_LIST_শীর্ষ(&chip->handler_list);

	sbi_list_add_লেজ(&chip->গ্রন্থি, &irqchip_list);
	ফেরত 0;
}

পূর্ণসংখ্যা sbi_irqchip_init(কাঠামো sbi_scratch *scratch, bool cold_boot)
{
	ধ্রুবক কাঠামো sbi_platform *plat = sbi_platform_ptr(scratch);
	কাঠামো sbi_irqchip_hart_তথ্য *hd;
	কাঠামো sbi_irqchip_device *chip;
	পূর্ণসংখ্যা rc;

	যদি (cold_boot) {
		irqchip_hart_তথ্য_off =
			sbi_scratch_alloc_offset(আকার(কাঠামো sbi_irqchip_hart_তথ্য));
		যদি (!irqchip_hart_তথ্য_off)
			ফেরত SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		যদি (rc)
			ফেরত rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, গ্রন্থি) {
		যদি (!chip->warm_init)
			চালু;
		যদি (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			চালু;
		rc = chip->warm_init(chip);
		যদি (rc)
			ফেরত rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_তথ্য_off);
	যদি (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	ফেরত 0;
}

শূন্য sbi_irqchip_exit(কাঠামো sbi_scratch *scratch)
{
	কাঠামো sbi_irqchip_hart_তথ্য *hd;
	কাঠামো sbi_irqchip_device *chip;
	কাঠামো sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	পূর্ণসংখ্যা rc;

	sbi_for_each_hartindex(i) {
		যদি (i == current_hartindex())
			চালু;
		যদি (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			চালু;
		migrate_hidx = i;
		migrate = true;
		বিরতি;
	}

	যদি (!migrate)
		যাও skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, গ্রন্থি) {
		sbi_list_for_each_entry(h, &chip->handler_list, গ্রন্থি) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			যদি (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_তথ্য_off);
	যদি (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
