/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#અન્તર્ભાવ <sbi/sbi_console.h>
#અન્તર્ભાવ <sbi/sbi_heap.h>
#અન્તર્ભાવ <sbi/sbi_hsm.h>
#અન્તર્ભાવ <sbi/sbi_irqchip.h>
#અન્તર્ભાવ <sbi/sbi_list.h>
#અન્તર્ભાવ <sbi/sbi_platform.h>
#અન્તર્ભાવ <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
સંરચના sbi_irqchip_hwirq_data {
	/** raw hardware interrupt handler */
	પૂર્ણાંક (*raw_handler)(સંરચના sbi_irqchip_device *chip, u32 hwirq);

#define IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_index;

	/** chip's private data */
	શૂન્ય *priv;
};

/** Internal irqchip interrupt handler */
સંરચના sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	સંરચના sbi_dlist ગાંઠ;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 અંક_hwirq;

	/** Write MSI function of this handler */
	શૂન્ય (*write_msi)(u32 hwirq, અચલ સંરચના sbi_irqchip_msi_સંદેશ *સંદેશ, શૂન્ય *priv);

	/** Callback function of this handler */
	પૂર્ણાંક (*callback)(u32 hwirq, શૂન્ય *priv);

	/** Callback private data */
	શૂન્ય *priv;
};

સંરચના sbi_irqchip_hart_data {
	સંરચના sbi_irqchip_device *chip;
};

સ્થિર unsigned long irqchip_hart_data_off;
સ્થિર SBI_LIST_HEAD(irqchip_list);

પૂર્ણાંક sbi_irqchip_process(શૂન્ય)
{
	સંરચના sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	જો (!hd || !hd->chip || !hd->chip->process_hwirqs)
		પાછા SBI_ENODEV;

	પાછા hd->chip->process_hwirqs(hd->chip);
}

પૂર્ણાંક sbi_irqchip_process_hwirq(સંરચના sbi_irqchip_device *chip, u32 hwirq)
{
	સંરચના sbi_irqchip_hwirq_data *data;

	જો (!chip || chip->અંક_hwirq <= hwirq)
		પાછા SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	જો (!data->raw_handler)
		પાછા SBI_ENOENT;

	પાછા data->raw_handler(chip, hwirq);
}

સ્થિર અંતઃરેખ u32 sbi_irqchip_get_irq_state(સંરચના sbi_irqchip_device *chip,
					     u32 hwirq)
{
	જો (!chip || !chip->hwirqs || hwirq >= chip->અંક_hwirq)
		પાછા 0;

	પાછા chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(સંરચના sbi_irqchip_device *chip,
				   u32 hwirq)
{
	પાછા !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

પૂર્ણાંક sbi_irqchip_unmask_hwirq(સંરચના sbi_irqchip_device *chip, u32 hwirq)
{
	સંરચના sbi_irqchip_hwirq_data *data;

	જો (!chip || chip->અંક_hwirq <= hwirq)
		પાછા SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	જો (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		પાછા SBI_EALREADY;

	જો (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	પાછા 0;
}

પૂર્ણાંક sbi_irqchip_mask_hwirq(સંરચના sbi_irqchip_device *chip, u32 hwirq)
{
	સંરચના sbi_irqchip_hwirq_data *data;

	જો (!chip || chip->અંક_hwirq <= hwirq)
		પાછા SBI_EINVAL;

	જો (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		પાછા SBI_EALREADY;

	જો (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	પાછા 0;
}

સ્થિર સંરચના sbi_irqchip_handler *sbi_irqchip_find_handler(સંરચના sbi_irqchip_device *chip,
							    u32 hwirq)
{
	સંરચના sbi_irqchip_handler *h;

	જો (!chip || chip->અંક_hwirq <= hwirq)
		પાછા ખાલી;

	sbi_list_for_each_entry(h, &chip->handler_list, ગાંઠ) {
		જો (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->અંક_hwirq))
			પાછા h;
	}

	પાછા ખાલી;
}

પૂર્ણાંક sbi_irqchip_set_hwirq_priv(સંરચના sbi_irqchip_device *chip, u32 hwirq, શૂન્ય *priv)
{
	સંરચના sbi_irqchip_hwirq_data *data;

	જો (!chip || chip->અંક_hwirq <= hwirq)
	     પાછા SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	પાછા 0;
}

શૂન્ય *sbi_irqchip_get_hwirq_priv(સંરચના sbi_irqchip_device *chip, u32 hwirq)
{
	સંરચના sbi_irqchip_hwirq_data *data;
	જો (!chip || chip->અંક_hwirq <= hwirq)
	    પાછા ખાલી;

	data = &chip->hwirqs[hwirq];

	પાછા data->priv;
}

પૂર્ણાંક sbi_irqchip_raw_handler_default(સંરચના sbi_irqchip_device *chip, u32 hwirq)
{
	સંરચના sbi_irqchip_handler *h;
	પૂર્ણાંક rc = SBI_OK;

	જો (!chip || chip->અંક_hwirq <= hwirq)
		પાછા SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	જો (!h)
		પાછા SBI_EINVAL;

	જો (h->callback)
		rc = h->callback(hwirq, h->priv);

	જો (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	પાછા rc;
}

પૂર્ણાંક sbi_irqchip_set_raw_handler(સંરચના sbi_irqchip_device *chip, u32 hwirq,
				પૂર્ણાંક (*raw_hndl)(સંરચના sbi_irqchip_device *, u32))
{
	સંરચના sbi_irqchip_hwirq_data *data;

	જો (!chip || chip->અંક_hwirq <= hwirq)
		પાછા SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	પાછા 0;
}

પૂર્ણાંક sbi_irqchip_write_msi(સંરચના sbi_irqchip_device *chip, u32 hwirq,
			  અચલ સંરચના sbi_irqchip_msi_સંદેશ *સંદેશ)
{
	સંરચના sbi_irqchip_handler *h;

	જો (!chip || chip->અંક_hwirq <= hwirq || !સંદેશ)
		પાછા SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	જો (!h)
		પાછા SBI_EFAIL;
	જો (!h->write_msi)
		પાછા SBI_ENOTSUPP;

	h->write_msi(hwirq, સંદેશ, h->priv);
	પાછા 0;
}

પૂર્ણાંક sbi_irqchip_get_affinity(સંરચના sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	જો (!chip || chip->અંક_hwirq <= hwirq)
		પાછા SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	જો (!sbi_irqchip_find_handler(chip, hwirq))
		પાછા SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	પાછા 0;
}

પૂર્ણાંક sbi_irqchip_set_affinity(સંરચના sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	સંરચના sbi_irqchip_hwirq_data *data;
	પૂર્ણાંક rc;

	જો (!chip || chip->અંક_hwirq <= hwirq || sbi_hart_ગણતરી() <= hart_index)
		પાછા SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	જો (!sbi_irqchip_find_handler(chip, hwirq))
		પાછા SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	જો (data->hart_index != hart_index) {
		જો (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			જો (rc)
				પાછા rc;
		}
		data->hart_index = hart_index;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક __sbi_irqchip_handler_set_affinity(સંરચના sbi_irqchip_device *chip,
					      સંરચના sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	પૂર્ણાંક rc;

	માટે (i = 0; i < h->અંક_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		જો (rc)
			પાછા rc;

		જો (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			ચાલુ;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		જો (rc)
			પાછા rc;
	}

	પાછા 0;
}

સ્થિર પૂર્ણાંક __sbi_irqchip_register_handler(સંરચના sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 અંક_hwirq, u32 hwirq_flags,
					  શૂન્ય (*write_msi)(u32 hwirq,
							    અચલ સંરચના sbi_irqchip_msi_સંદેશ *સંદેશ,
							    શૂન્ય *priv),
					  પૂર્ણાંક (*callback)(u32 hwirq, શૂન્ય *priv), શૂન્ય *priv)
{
	સંરચના sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	પૂર્ણાંક rc;

	માટે (i = first_hwirq; i < (first_hwirq + અંક_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		જો (h)
			પાછા SBI_EALREADY;
	}

	h = sbi_zalloc(કદ(*h));
	જો (!h)
		પાછા SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->અંક_hwirq = અંક_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = ખાલી;
	sbi_list_for_each_entry(th, &chip->handler_list, ગાંઠ) {
		જો (h->first_hwirq < th->first_hwirq) {
			nh = th;
			તોડો;
		}
	}
	જો (nh)
		sbi_list_add(&h->ગાંઠ, &nh->ગાંઠ);
	નહીંતર
		sbi_list_add_tail(&h->ગાંઠ, &chip->handler_list);

	જો (chip->hwirq_setup) {
		માટે (i = 0; i < h->અંક_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			જો (rc) {
				જો (chip->hwirq_cleanup) {
					માટે (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->ગાંઠ);
				sbi_free(h);
				પાછા rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	જો (rc) {
		જો (chip->hwirq_cleanup) {
			માટે (i = 0; i < h->અંક_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->ગાંઠ);
		sbi_free(h);
		પાછા rc;
	}

	જો (chip->hwirq_unmask) {
		માટે (i = 0; i < h->અંક_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	પાછા 0;
}

પૂર્ણાંક sbi_irqchip_register_msi(સંરચના sbi_irqchip_device *chip, u32 અંક_hwirq,
			     શૂન્ય (*write_msi)(u32 hwirq,
					       અચલ સંરચના sbi_irqchip_msi_સંદેશ *સંદેશ,
					       શૂન્ય *priv),
			     પૂર્ણાંક (*callback)(u32 hwirq, શૂન્ય *priv), શૂન્ય *priv,
			     u32 *out_first_hwirq)
{
	સંરચના sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	જો (!chip || !chip->hwirq_set_affinity || !અંક_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		પાછા SBI_EINVAL;
	જો (chip->અંક_hwirq < અંક_hwirq)
		પાછા SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, ગાંઠ) {
		જો (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->અંક_hwirq)) {
			hwirq = h->first_hwirq + h->અંક_hwirq;
		} નહીંતર જો (hwirq < h->first_hwirq) {
			જો (h->first_hwirq - hwirq >= અંક_hwirq) {
				found = true;
				તોડો;
			} નહીંતર {
				hwirq = h->first_hwirq + h->અંક_hwirq;
			}
		}
	}
	જો (!found && (chip->અંક_hwirq - hwirq) >= અંક_hwirq)
		found = true;
	જો (!found)
		પાછા SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	પાછા __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      અંક_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

પૂર્ણાંક sbi_irqchip_register_handler(સંરચના sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 અંક_hwirq, u32 hwirq_flags,
				 પૂર્ણાંક (*callback)(u32 hwirq, શૂન્ય *priv), શૂન્ય *priv)
{
	જો (!chip || !અંક_hwirq || !callback)
		પાછા SBI_EINVAL;
	જો (chip->અંક_hwirq <= first_hwirq ||
	    chip->અંક_hwirq <= (first_hwirq + અંક_hwirq - 1))
		પાછા SBI_EBAD_RANGE;

	પાછા __sbi_irqchip_register_handler(chip, first_hwirq, અંક_hwirq, hwirq_flags,
					      ખાલી, callback, priv);
}

પૂર્ણાંક sbi_irqchip_register_reserved(સંરચના sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 અંક_hwirq)
{
	જો (!chip || !અંક_hwirq)
		પાછા SBI_EINVAL;
	જો (chip->અંક_hwirq <= first_hwirq ||
	    chip->અંક_hwirq <= (first_hwirq + અંક_hwirq - 1))
		પાછા SBI_EBAD_RANGE;

	પાછા __sbi_irqchip_register_handler(chip, first_hwirq, અંક_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, ખાલી, ખાલી, ખાલી);
}

પૂર્ણાંક sbi_irqchip_unregister_handler(સંરચના sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 અંક_hwirq)
{
	સંરચના sbi_irqchip_handler *fh, *lh;
	u32 i;

	જો (!chip || !અંક_hwirq)
		પાછા SBI_EINVAL;
	જો (chip->અંક_hwirq <= first_hwirq ||
	    chip->અંક_hwirq <= (first_hwirq + અંક_hwirq - 1))
		પાછા SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	જો (!fh || fh->first_hwirq != first_hwirq || fh->અંક_hwirq != અંક_hwirq)
		પાછા SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + અંક_hwirq - 1);
	જો (!lh || lh != fh)
		પાછા SBI_ENODEV;

	જો (chip->hwirq_mask) {
		માટે (i = 0; i < fh->અંક_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	જો (chip->hwirq_cleanup) {
		માટે (i = 0; i < fh->અંક_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->ગાંઠ);
	પાછા 0;
}

સંરચના sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   સંરચના sbi_irqchip_device *first)
{
	સંરચના sbi_irqchip_device *chip;
	bool found = (first == ખાલી);

	sbi_list_for_each_entry(chip, &irqchip_list, ગાંઠ) {
		જો (!found) {
			જો (first == chip)
				found = true;
			ચાલુ;
		}
		જો ((chip->caps & caps) == caps)
			પાછા chip;
	}

	પાછા ખાલી;
}

સંરચના sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	સંરચના sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, ગાંઠ) {
		જો (chip->id == id)
			પાછા chip;
	}

	પાછા ખાલી;
}

પૂર્ણાંક sbi_irqchip_add_device(સંરચના sbi_irqchip_device *chip)
{
	સંરચના sbi_irqchip_hart_data *hd;
	સંરચના sbi_scratch *scratch;
	u32 i, h;

	જો (!chip || !chip->અંક_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		પાછા SBI_EINVAL;

	જો (sbi_irqchip_find_device(chip->id))
		પાછા SBI_EALREADY;

	જો (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			જો (!scratch)
				ચાલુ;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			જો (hd->chip && hd->chip != chip)
				પાછા SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(કદ(*chip->hwirqs) * chip->અંક_hwirq);
	જો (!chip->hwirqs)
		પાછા SBI_ENOMEM;
	માટે (i = 0; i < chip->અંક_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->ગાંઠ, &irqchip_list);
	પાછા 0;
}

પૂર્ણાંક sbi_irqchip_init(સંરચના sbi_scratch *scratch, bool cold_boot)
{
	અચલ સંરચના sbi_platform *plat = sbi_platform_ptr(scratch);
	સંરચના sbi_irqchip_hart_data *hd;
	સંરચના sbi_irqchip_device *chip;
	પૂર્ણાંક rc;

	જો (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(કદ(સંરચના sbi_irqchip_hart_data));
		જો (!irqchip_hart_data_off)
			પાછા SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		જો (rc)
			પાછા rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, ગાંઠ) {
		જો (!chip->warm_init)
			ચાલુ;
		જો (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			ચાલુ;
		rc = chip->warm_init(chip);
		જો (rc)
			પાછા rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	જો (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	પાછા 0;
}

શૂન્ય sbi_irqchip_exit(સંરચના sbi_scratch *scratch)
{
	સંરચના sbi_irqchip_hart_data *hd;
	સંરચના sbi_irqchip_device *chip;
	સંરચના sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	પૂર્ણાંક rc;

	sbi_for_each_hartindex(i) {
		જો (i == current_hartindex())
			ચાલુ;
		જો (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			ચાલુ;
		migrate_hidx = i;
		migrate = true;
		તોડો;
	}

	જો (!migrate)
		જાઓ skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, ગાંઠ) {
		sbi_list_for_each_entry(h, &chip->handler_list, ગાંઠ) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			જો (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	જો (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
