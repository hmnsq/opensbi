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
	بنية sbi_dlist node;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 num_hwirq;

	/** Write MSI function of this handler */
	فراغ (*write_msi)(u32 hwirq, ثابت بنية sbi_irqchip_msi_msg *msg, فراغ *priv);

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
	אם (!hd || !hd->chip || !hd->chip->process_hwirqs)
		החזר SBI_ENODEV;

	החזר hd->chip->process_hwirqs(hd->chip);
}

صحيح sbi_irqchip_process_hwirq(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_data *data;

	אם (!chip || chip->num_hwirq <= hwirq)
		החזר SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	אם (!data->raw_handler)
		החזר SBI_ENOENT;

	החזר data->raw_handler(chip, hwirq);
}

ساكن مضمن u32 sbi_irqchip_get_irq_state(بنية sbi_irqchip_device *chip,
					     u32 hwirq)
{
	אם (!chip || !chip->hwirqs || hwirq >= chip->num_hwirq)
		החזר 0;

	החזר chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(بنية sbi_irqchip_device *chip,
				   u32 hwirq)
{
	החזר !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

صحيح sbi_irqchip_unmask_hwirq(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_data *data;

	אם (!chip || chip->num_hwirq <= hwirq)
		החזר SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	אם (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		החזר SBI_EALREADY;

	אם (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	החזר 0;
}

صحيح sbi_irqchip_mask_hwirq(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_data *data;

	אם (!chip || chip->num_hwirq <= hwirq)
		החזר SBI_EINVAL;

	אם (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		החזר SBI_EALREADY;

	אם (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	החזר 0;
}

ساكن بنية sbi_irqchip_handler *sbi_irqchip_find_handler(بنية sbi_irqchip_device *chip,
							    u32 hwirq)
{
	بنية sbi_irqchip_handler *h;

	אם (!chip || chip->num_hwirq <= hwirq)
		החזר عدم;

	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		אם (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq))
			החזר h;
	}

	החזר عدم;
}

صحيح sbi_irqchip_set_hwirq_priv(بنية sbi_irqchip_device *chip, u32 hwirq, فراغ *priv)
{
	بنية sbi_irqchip_hwirq_data *data;

	אם (!chip || chip->num_hwirq <= hwirq)
	     החזר SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	החזר 0;
}

فراغ *sbi_irqchip_get_hwirq_priv(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_data *data;
	אם (!chip || chip->num_hwirq <= hwirq)
	    החזר عدم;

	data = &chip->hwirqs[hwirq];

	החזר data->priv;
}

صحيح sbi_irqchip_raw_handler_default(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_handler *h;
	صحيح rc = SBI_OK;

	אם (!chip || chip->num_hwirq <= hwirq)
		החזר SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	אם (!h)
		החזר SBI_EINVAL;

	אם (h->callback)
		rc = h->callback(hwirq, h->priv);

	אם (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	החזר rc;
}

صحيح sbi_irqchip_set_raw_handler(بنية sbi_irqchip_device *chip, u32 hwirq,
				صحيح (*raw_hndl)(بنية sbi_irqchip_device *, u32))
{
	بنية sbi_irqchip_hwirq_data *data;

	אם (!chip || chip->num_hwirq <= hwirq)
		החזר SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	החזר 0;
}

صحيح sbi_irqchip_write_msi(بنية sbi_irqchip_device *chip, u32 hwirq,
			  ثابت بنية sbi_irqchip_msi_msg *msg)
{
	بنية sbi_irqchip_handler *h;

	אם (!chip || chip->num_hwirq <= hwirq || !msg)
		החזר SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	אם (!h)
		החזר SBI_EFAIL;
	אם (!h->write_msi)
		החזר SBI_ENOTSUPP;

	h->write_msi(hwirq, msg, h->priv);
	החזר 0;
}

صحيح sbi_irqchip_get_affinity(بنية sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	אם (!chip || chip->num_hwirq <= hwirq)
		החזר SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	אם (!sbi_irqchip_find_handler(chip, hwirq))
		החזר SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	החזר 0;
}

صحيح sbi_irqchip_set_affinity(بنية sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	بنية sbi_irqchip_hwirq_data *data;
	صحيح rc;

	אם (!chip || chip->num_hwirq <= hwirq || sbi_hart_count() <= hart_index)
		החזר SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	אם (!sbi_irqchip_find_handler(chip, hwirq))
		החזר SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	אם (data->hart_index != hart_index) {
		אם (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			אם (rc)
				החזר rc;
		}
		data->hart_index = hart_index;
	}

	החזר 0;
}

ساكن صحيح __sbi_irqchip_handler_set_affinity(بنية sbi_irqchip_device *chip,
					      بنية sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	صحيح rc;

	עבור (i = 0; i < h->num_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		אם (rc)
			החזר rc;

		אם (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			استمر;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		אם (rc)
			החזר rc;
	}

	החזר 0;
}

ساكن صحيح __sbi_irqchip_register_handler(بنية sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
					  فراغ (*write_msi)(u32 hwirq,
							    ثابت بنية sbi_irqchip_msi_msg *msg,
							    فراغ *priv),
					  صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv)
{
	بنية sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	صحيح rc;

	עבור (i = first_hwirq; i < (first_hwirq + num_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		אם (h)
			החזר SBI_EALREADY;
	}

	h = sbi_zalloc(حجم(*h));
	אם (!h)
		החזר SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->num_hwirq = num_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = عدم;
	sbi_list_for_each_entry(th, &chip->handler_list, node) {
		אם (h->first_hwirq < th->first_hwirq) {
			nh = th;
			اكسر;
		}
	}
	אם (nh)
		sbi_list_add(&h->node, &nh->node);
	אחרת
		sbi_list_add_tail(&h->node, &chip->handler_list);

	אם (chip->hwirq_setup) {
		עבור (i = 0; i < h->num_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			אם (rc) {
				אם (chip->hwirq_cleanup) {
					עבור (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->node);
				sbi_free(h);
				החזר rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	אם (rc) {
		אם (chip->hwirq_cleanup) {
			עבור (i = 0; i < h->num_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->node);
		sbi_free(h);
		החזר rc;
	}

	אם (chip->hwirq_unmask) {
		עבור (i = 0; i < h->num_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	החזר 0;
}

صحيح sbi_irqchip_register_msi(بنية sbi_irqchip_device *chip, u32 num_hwirq,
			     فراغ (*write_msi)(u32 hwirq,
					       ثابت بنية sbi_irqchip_msi_msg *msg,
					       فراغ *priv),
			     صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv,
			     u32 *out_first_hwirq)
{
	بنية sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	אם (!chip || !chip->hwirq_set_affinity || !num_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		החזר SBI_EINVAL;
	אם (chip->num_hwirq < num_hwirq)
		החזר SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		אם (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq)) {
			hwirq = h->first_hwirq + h->num_hwirq;
		} אחרת אם (hwirq < h->first_hwirq) {
			אם (h->first_hwirq - hwirq >= num_hwirq) {
				found = true;
				اكسر;
			} אחרת {
				hwirq = h->first_hwirq + h->num_hwirq;
			}
		}
	}
	אם (!found && (chip->num_hwirq - hwirq) >= num_hwirq)
		found = true;
	אם (!found)
		החזר SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	החזר __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      num_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

صحيح sbi_irqchip_register_handler(بنية sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
				 صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv)
{
	אם (!chip || !num_hwirq || !callback)
		החזר SBI_EINVAL;
	אם (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		החזר SBI_EBAD_RANGE;

	החזר __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq, hwirq_flags,
					      عدم, callback, priv);
}

صحيح sbi_irqchip_register_reserved(بنية sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 num_hwirq)
{
	אם (!chip || !num_hwirq)
		החזר SBI_EINVAL;
	אם (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		החזר SBI_EBAD_RANGE;

	החזר __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, عدم, عدم, عدم);
}

صحيح sbi_irqchip_unregister_handler(بنية sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 num_hwirq)
{
	بنية sbi_irqchip_handler *fh, *lh;
	u32 i;

	אם (!chip || !num_hwirq)
		החזר SBI_EINVAL;
	אם (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		החזר SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	אם (!fh || fh->first_hwirq != first_hwirq || fh->num_hwirq != num_hwirq)
		החזר SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + num_hwirq - 1);
	אם (!lh || lh != fh)
		החזר SBI_ENODEV;

	אם (chip->hwirq_mask) {
		עבור (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	אם (chip->hwirq_cleanup) {
		עבור (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->node);
	החזר 0;
}

بنية sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   بنية sbi_irqchip_device *first)
{
	بنية sbi_irqchip_device *chip;
	bool found = (first == عدم);

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		אם (!found) {
			אם (first == chip)
				found = true;
			استمر;
		}
		אם ((chip->caps & caps) == caps)
			החזר chip;
	}

	החזר عدم;
}

بنية sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	بنية sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		אם (chip->id == id)
			החזר chip;
	}

	החזר عدم;
}

صحيح sbi_irqchip_add_device(بنية sbi_irqchip_device *chip)
{
	بنية sbi_irqchip_hart_data *hd;
	بنية sbi_scratch *scratch;
	u32 i, h;

	אם (!chip || !chip->num_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		החזר SBI_EINVAL;

	אם (sbi_irqchip_find_device(chip->id))
		החזר SBI_EALREADY;

	אם (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			אם (!scratch)
				استمر;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			אם (hd->chip && hd->chip != chip)
				החזר SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(حجم(*chip->hwirqs) * chip->num_hwirq);
	אם (!chip->hwirqs)
		החזר SBI_ENOMEM;
	עבור (i = 0; i < chip->num_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->node, &irqchip_list);
	החזר 0;
}

صحيح sbi_irqchip_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);
	بنية sbi_irqchip_hart_data *hd;
	بنية sbi_irqchip_device *chip;
	صحيح rc;

	אם (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(حجم(بنية sbi_irqchip_hart_data));
		אם (!irqchip_hart_data_off)
			החזר SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		אם (rc)
			החזר rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		אם (!chip->warm_init)
			استمر;
		אם (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			استمر;
		rc = chip->warm_init(chip);
		אם (rc)
			החזר rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	אם (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	החזר 0;
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
		אם (i == current_hartindex())
			استمر;
		אם (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			استمر;
		migrate_hidx = i;
		migrate = true;
		اكسر;
	}

	אם (!migrate)
		اذهب skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		sbi_list_for_each_entry(h, &chip->handler_list, node) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			אם (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	אם (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
