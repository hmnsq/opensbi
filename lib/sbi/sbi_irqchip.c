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
بنية sbi_irqchip_hwirq_data {
	/** raw hardware interrupt handler */
	صحيح (*raw_handler)(بنية sbi_irqchip_device *chip, u32 hwirq);

#define IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_index;

	/** chip's private data */
	فراغ *priv;
};

/** Internal irqchip interrupt handler */
بنية sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	بنية sbi_dlist گرہ;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 عدد_hwirq;

	/** Write MSI function of this handler */
	فراغ (*write_msi)(u32 hwirq, ثابت بنية sbi_irqchip_msi_پیغام *پیغام, فراغ *priv);

	/** Callback function of this handler */
	صحيح (*callback)(u32 hwirq, فراغ *priv);

	/** Callback private data */
	فراغ *priv;
};

بنية sbi_irqchip_hart_data {
	بنية sbi_irqchip_device *chip;
};

ساكن unsigned long irqchip_hart_data_off;
ساكن SBI_LIST_HEAD(irqchip_list);

صحيح sbi_irqchip_process(فراغ)
{
	بنية sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	إذا (!hd || !hd->chip || !hd->chip->process_hwirqs)
		أرجع SBI_ENODEV;

	أرجع hd->chip->process_hwirqs(hd->chip);
}

صحيح sbi_irqchip_process_hwirq(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_data *data;

	إذا (!chip || chip->عدد_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	إذا (!data->raw_handler)
		أرجع SBI_ENOENT;

	أرجع data->raw_handler(chip, hwirq);
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
	بنية sbi_irqchip_hwirq_data *data;

	إذا (!chip || chip->عدد_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	إذا (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		أرجع SBI_EALREADY;

	إذا (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	أرجع 0;
}

صحيح sbi_irqchip_mask_hwirq(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_data *data;

	إذا (!chip || chip->عدد_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	إذا (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		أرجع SBI_EALREADY;

	إذا (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	أرجع 0;
}

ساكن بنية sbi_irqchip_handler *sbi_irqchip_find_handler(بنية sbi_irqchip_device *chip,
							    u32 hwirq)
{
	بنية sbi_irqchip_handler *h;

	إذا (!chip || chip->عدد_hwirq <= hwirq)
		أرجع عدم;

	sbi_list_for_each_entry(h, &chip->handler_list, گرہ) {
		إذا (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->عدد_hwirq))
			أرجع h;
	}

	أرجع عدم;
}

صحيح sbi_irqchip_set_hwirq_priv(بنية sbi_irqchip_device *chip, u32 hwirq, فراغ *priv)
{
	بنية sbi_irqchip_hwirq_data *data;

	إذا (!chip || chip->عدد_hwirq <= hwirq)
	     أرجع SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	أرجع 0;
}

فراغ *sbi_irqchip_get_hwirq_priv(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_data *data;
	إذا (!chip || chip->عدد_hwirq <= hwirq)
	    أرجع عدم;

	data = &chip->hwirqs[hwirq];

	أرجع data->priv;
}

صحيح sbi_irqchip_raw_handler_default(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_handler *h;
	صحيح rc = SBI_OK;

	إذا (!chip || chip->عدد_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	إذا (!h)
		أرجع SBI_EINVAL;

	إذا (h->callback)
		rc = h->callback(hwirq, h->priv);

	إذا (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	أرجع rc;
}

صحيح sbi_irqchip_set_raw_handler(بنية sbi_irqchip_device *chip, u32 hwirq,
				صحيح (*raw_hndl)(بنية sbi_irqchip_device *, u32))
{
	بنية sbi_irqchip_hwirq_data *data;

	إذا (!chip || chip->عدد_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	أرجع 0;
}

صحيح sbi_irqchip_write_msi(بنية sbi_irqchip_device *chip, u32 hwirq,
			  ثابت بنية sbi_irqchip_msi_پیغام *پیغام)
{
	بنية sbi_irqchip_handler *h;

	إذا (!chip || chip->عدد_hwirq <= hwirq || !پیغام)
		أرجع SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	إذا (!h)
		أرجع SBI_EFAIL;
	إذا (!h->write_msi)
		أرجع SBI_ENOTSUPP;

	h->write_msi(hwirq, پیغام, h->priv);
	أرجع 0;
}

صحيح sbi_irqchip_get_affinity(بنية sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	إذا (!chip || chip->عدد_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	إذا (!sbi_irqchip_find_handler(chip, hwirq))
		أرجع SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	أرجع 0;
}

صحيح sbi_irqchip_set_affinity(بنية sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	بنية sbi_irqchip_hwirq_data *data;
	صحيح rc;

	إذا (!chip || chip->عدد_hwirq <= hwirq || sbi_hart_گنتی() <= hart_index)
		أرجع SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	إذا (!sbi_irqchip_find_handler(chip, hwirq))
		أرجع SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	إذا (data->hart_index != hart_index) {
		إذا (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			إذا (rc)
				أرجع rc;
		}
		data->hart_index = hart_index;
	}

	أرجع 0;
}

ساكن صحيح __sbi_irqchip_handler_set_affinity(بنية sbi_irqchip_device *chip,
					      بنية sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	صحيح rc;

	لكل (i = 0; i < h->عدد_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		إذا (rc)
			أرجع rc;

		إذا (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			استمر;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		إذا (rc)
			أرجع rc;
	}

	أرجع 0;
}

ساكن صحيح __sbi_irqchip_register_handler(بنية sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 عدد_hwirq, u32 hwirq_flags,
					  فراغ (*write_msi)(u32 hwirq,
							    ثابت بنية sbi_irqchip_msi_پیغام *پیغام,
							    فراغ *priv),
					  صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv)
{
	بنية sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	صحيح rc;

	لكل (i = first_hwirq; i < (first_hwirq + عدد_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		إذا (h)
			أرجع SBI_EALREADY;
	}

	h = sbi_zalloc(حجم(*h));
	إذا (!h)
		أرجع SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->عدد_hwirq = عدد_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = عدم;
	sbi_list_for_each_entry(th, &chip->handler_list, گرہ) {
		إذا (h->first_hwirq < th->first_hwirq) {
			nh = th;
			اكسر;
		}
	}
	إذا (nh)
		sbi_list_add(&h->گرہ, &nh->گرہ);
	وإلا
		sbi_list_add_tail(&h->گرہ, &chip->handler_list);

	إذا (chip->hwirq_setup) {
		لكل (i = 0; i < h->عدد_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			إذا (rc) {
				إذا (chip->hwirq_cleanup) {
					لكل (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->گرہ);
				sbi_free(h);
				أرجع rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	إذا (rc) {
		إذا (chip->hwirq_cleanup) {
			لكل (i = 0; i < h->عدد_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->گرہ);
		sbi_free(h);
		أرجع rc;
	}

	إذا (chip->hwirq_unmask) {
		لكل (i = 0; i < h->عدد_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	أرجع 0;
}

صحيح sbi_irqchip_register_msi(بنية sbi_irqchip_device *chip, u32 عدد_hwirq,
			     فراغ (*write_msi)(u32 hwirq,
					       ثابت بنية sbi_irqchip_msi_پیغام *پیغام,
					       فراغ *priv),
			     صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv,
			     u32 *out_first_hwirq)
{
	بنية sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	إذا (!chip || !chip->hwirq_set_affinity || !عدد_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		أرجع SBI_EINVAL;
	إذا (chip->عدد_hwirq < عدد_hwirq)
		أرجع SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, گرہ) {
		إذا (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->عدد_hwirq)) {
			hwirq = h->first_hwirq + h->عدد_hwirq;
		} وإلا إذا (hwirq < h->first_hwirq) {
			إذا (h->first_hwirq - hwirq >= عدد_hwirq) {
				found = true;
				اكسر;
			} وإلا {
				hwirq = h->first_hwirq + h->عدد_hwirq;
			}
		}
	}
	إذا (!found && (chip->عدد_hwirq - hwirq) >= عدد_hwirq)
		found = true;
	إذا (!found)
		أرجع SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	أرجع __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      عدد_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

صحيح sbi_irqchip_register_handler(بنية sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 عدد_hwirq, u32 hwirq_flags,
				 صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv)
{
	إذا (!chip || !عدد_hwirq || !callback)
		أرجع SBI_EINVAL;
	إذا (chip->عدد_hwirq <= first_hwirq ||
	    chip->عدد_hwirq <= (first_hwirq + عدد_hwirq - 1))
		أرجع SBI_EBAD_RANGE;

	أرجع __sbi_irqchip_register_handler(chip, first_hwirq, عدد_hwirq, hwirq_flags,
					      عدم, callback, priv);
}

صحيح sbi_irqchip_register_reserved(بنية sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 عدد_hwirq)
{
	إذا (!chip || !عدد_hwirq)
		أرجع SBI_EINVAL;
	إذا (chip->عدد_hwirq <= first_hwirq ||
	    chip->عدد_hwirq <= (first_hwirq + عدد_hwirq - 1))
		أرجع SBI_EBAD_RANGE;

	أرجع __sbi_irqchip_register_handler(chip, first_hwirq, عدد_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, عدم, عدم, عدم);
}

صحيح sbi_irqchip_unregister_handler(بنية sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 عدد_hwirq)
{
	بنية sbi_irqchip_handler *fh, *lh;
	u32 i;

	إذا (!chip || !عدد_hwirq)
		أرجع SBI_EINVAL;
	إذا (chip->عدد_hwirq <= first_hwirq ||
	    chip->عدد_hwirq <= (first_hwirq + عدد_hwirq - 1))
		أرجع SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	إذا (!fh || fh->first_hwirq != first_hwirq || fh->عدد_hwirq != عدد_hwirq)
		أرجع SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + عدد_hwirq - 1);
	إذا (!lh || lh != fh)
		أرجع SBI_ENODEV;

	إذا (chip->hwirq_mask) {
		لكل (i = 0; i < fh->عدد_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	إذا (chip->hwirq_cleanup) {
		لكل (i = 0; i < fh->عدد_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->گرہ);
	أرجع 0;
}

بنية sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   بنية sbi_irqchip_device *first)
{
	بنية sbi_irqchip_device *chip;
	bool found = (first == عدم);

	sbi_list_for_each_entry(chip, &irqchip_list, گرہ) {
		إذا (!found) {
			إذا (first == chip)
				found = true;
			استمر;
		}
		إذا ((chip->caps & caps) == caps)
			أرجع chip;
	}

	أرجع عدم;
}

بنية sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	بنية sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, گرہ) {
		إذا (chip->id == id)
			أرجع chip;
	}

	أرجع عدم;
}

صحيح sbi_irqchip_add_device(بنية sbi_irqchip_device *chip)
{
	بنية sbi_irqchip_hart_data *hd;
	بنية sbi_scratch *scratch;
	u32 i, h;

	إذا (!chip || !chip->عدد_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		أرجع SBI_EINVAL;

	إذا (sbi_irqchip_find_device(chip->id))
		أرجع SBI_EALREADY;

	إذا (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			إذا (!scratch)
				استمر;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			إذا (hd->chip && hd->chip != chip)
				أرجع SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(حجم(*chip->hwirqs) * chip->عدد_hwirq);
	إذا (!chip->hwirqs)
		أرجع SBI_ENOMEM;
	لكل (i = 0; i < chip->عدد_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->گرہ, &irqchip_list);
	أرجع 0;
}

صحيح sbi_irqchip_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);
	بنية sbi_irqchip_hart_data *hd;
	بنية sbi_irqchip_device *chip;
	صحيح rc;

	إذا (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(حجم(بنية sbi_irqchip_hart_data));
		إذا (!irqchip_hart_data_off)
			أرجع SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		إذا (rc)
			أرجع rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, گرہ) {
		إذا (!chip->warm_init)
			استمر;
		إذا (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			استمر;
		rc = chip->warm_init(chip);
		إذا (rc)
			أرجع rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	إذا (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	أرجع 0;
}

فراغ sbi_irqchip_exit(بنية sbi_scratch *scratch)
{
	بنية sbi_irqchip_hart_data *hd;
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
	sbi_list_for_each_entry(chip, &irqchip_list, گرہ) {
		sbi_list_for_each_entry(h, &chip->handler_list, گرہ) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			إذا (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	إذا (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
