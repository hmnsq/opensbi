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
	𐤀𐤌 (!hd || !hd->chip || !hd->chip->process_hwirqs)
		أرجع SBI_ENODEV;

	أرجع hd->chip->process_hwirqs(hd->chip);
}

صحيح sbi_irqchip_process_hwirq(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_data *data;

	𐤀𐤌 (!chip || chip->num_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	𐤀𐤌 (!data->raw_handler)
		أرجع SBI_ENOENT;

	أرجع data->raw_handler(chip, hwirq);
}

ساكن مضمن u32 sbi_irqchip_get_irq_state(بنية sbi_irqchip_device *chip,
					     u32 hwirq)
{
	𐤀𐤌 (!chip || !chip->hwirqs || hwirq >= chip->num_hwirq)
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

	𐤀𐤌 (!chip || chip->num_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	𐤀𐤌 (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		أرجع SBI_EALREADY;

	𐤀𐤌 (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	أرجع 0;
}

صحيح sbi_irqchip_mask_hwirq(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_data *data;

	𐤀𐤌 (!chip || chip->num_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	𐤀𐤌 (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		أرجع SBI_EALREADY;

	𐤀𐤌 (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	أرجع 0;
}

ساكن بنية sbi_irqchip_handler *sbi_irqchip_find_handler(بنية sbi_irqchip_device *chip,
							    u32 hwirq)
{
	بنية sbi_irqchip_handler *h;

	𐤀𐤌 (!chip || chip->num_hwirq <= hwirq)
		أرجع عدم;

	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		𐤀𐤌 (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq))
			أرجع h;
	}

	أرجع عدم;
}

صحيح sbi_irqchip_set_hwirq_priv(بنية sbi_irqchip_device *chip, u32 hwirq, فراغ *priv)
{
	بنية sbi_irqchip_hwirq_data *data;

	𐤀𐤌 (!chip || chip->num_hwirq <= hwirq)
	     أرجع SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	أرجع 0;
}

فراغ *sbi_irqchip_get_hwirq_priv(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_hwirq_data *data;
	𐤀𐤌 (!chip || chip->num_hwirq <= hwirq)
	    أرجع عدم;

	data = &chip->hwirqs[hwirq];

	أرجع data->priv;
}

صحيح sbi_irqchip_raw_handler_default(بنية sbi_irqchip_device *chip, u32 hwirq)
{
	بنية sbi_irqchip_handler *h;
	صحيح rc = SBI_OK;

	𐤀𐤌 (!chip || chip->num_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	𐤀𐤌 (!h)
		أرجع SBI_EINVAL;

	𐤀𐤌 (h->callback)
		rc = h->callback(hwirq, h->priv);

	𐤀𐤌 (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	أرجع rc;
}

صحيح sbi_irqchip_set_raw_handler(بنية sbi_irqchip_device *chip, u32 hwirq,
				صحيح (*raw_hndl)(بنية sbi_irqchip_device *, u32))
{
	بنية sbi_irqchip_hwirq_data *data;

	𐤀𐤌 (!chip || chip->num_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	أرجع 0;
}

صحيح sbi_irqchip_write_msi(بنية sbi_irqchip_device *chip, u32 hwirq,
			  ثابت بنية sbi_irqchip_msi_msg *msg)
{
	بنية sbi_irqchip_handler *h;

	𐤀𐤌 (!chip || chip->num_hwirq <= hwirq || !msg)
		أرجع SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	𐤀𐤌 (!h)
		أرجع SBI_EFAIL;
	𐤀𐤌 (!h->write_msi)
		أرجع SBI_ENOTSUPP;

	h->write_msi(hwirq, msg, h->priv);
	أرجع 0;
}

صحيح sbi_irqchip_get_affinity(بنية sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	𐤀𐤌 (!chip || chip->num_hwirq <= hwirq)
		أرجع SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	𐤀𐤌 (!sbi_irqchip_find_handler(chip, hwirq))
		أرجع SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	أرجع 0;
}

صحيح sbi_irqchip_set_affinity(بنية sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	بنية sbi_irqchip_hwirq_data *data;
	صحيح rc;

	𐤀𐤌 (!chip || chip->num_hwirq <= hwirq || sbi_hart_count() <= hart_index)
		أرجع SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	𐤀𐤌 (!sbi_irqchip_find_handler(chip, hwirq))
		أرجع SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	𐤀𐤌 (data->hart_index != hart_index) {
		𐤀𐤌 (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			𐤀𐤌 (rc)
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

	لكل (i = 0; i < h->num_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		𐤀𐤌 (rc)
			أرجع rc;

		𐤀𐤌 (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			استمر;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		𐤀𐤌 (rc)
			أرجع rc;
	}

	أرجع 0;
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

	لكل (i = first_hwirq; i < (first_hwirq + num_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		𐤀𐤌 (h)
			أرجع SBI_EALREADY;
	}

	h = sbi_zalloc(حجم(*h));
	𐤀𐤌 (!h)
		أرجع SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->num_hwirq = num_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = عدم;
	sbi_list_for_each_entry(th, &chip->handler_list, node) {
		𐤀𐤌 (h->first_hwirq < th->first_hwirq) {
			nh = th;
			اكسر;
		}
	}
	𐤀𐤌 (nh)
		sbi_list_add(&h->node, &nh->node);
	وإلا
		sbi_list_add_tail(&h->node, &chip->handler_list);

	𐤀𐤌 (chip->hwirq_setup) {
		لكل (i = 0; i < h->num_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			𐤀𐤌 (rc) {
				𐤀𐤌 (chip->hwirq_cleanup) {
					لكل (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->node);
				sbi_free(h);
				أرجع rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	𐤀𐤌 (rc) {
		𐤀𐤌 (chip->hwirq_cleanup) {
			لكل (i = 0; i < h->num_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->node);
		sbi_free(h);
		أرجع rc;
	}

	𐤀𐤌 (chip->hwirq_unmask) {
		لكل (i = 0; i < h->num_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	أرجع 0;
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

	𐤀𐤌 (!chip || !chip->hwirq_set_affinity || !num_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		أرجع SBI_EINVAL;
	𐤀𐤌 (chip->num_hwirq < num_hwirq)
		أرجع SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		𐤀𐤌 (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq)) {
			hwirq = h->first_hwirq + h->num_hwirq;
		} وإلا 𐤀𐤌 (hwirq < h->first_hwirq) {
			𐤀𐤌 (h->first_hwirq - hwirq >= num_hwirq) {
				found = true;
				اكسر;
			} وإلا {
				hwirq = h->first_hwirq + h->num_hwirq;
			}
		}
	}
	𐤀𐤌 (!found && (chip->num_hwirq - hwirq) >= num_hwirq)
		found = true;
	𐤀𐤌 (!found)
		أرجع SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	أرجع __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      num_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

صحيح sbi_irqchip_register_handler(بنية sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
				 صحيح (*callback)(u32 hwirq, فراغ *priv), فراغ *priv)
{
	𐤀𐤌 (!chip || !num_hwirq || !callback)
		أرجع SBI_EINVAL;
	𐤀𐤌 (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		أرجع SBI_EBAD_RANGE;

	أرجع __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq, hwirq_flags,
					      عدم, callback, priv);
}

صحيح sbi_irqchip_register_reserved(بنية sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 num_hwirq)
{
	𐤀𐤌 (!chip || !num_hwirq)
		أرجع SBI_EINVAL;
	𐤀𐤌 (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		أرجع SBI_EBAD_RANGE;

	أرجع __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, عدم, عدم, عدم);
}

صحيح sbi_irqchip_unregister_handler(بنية sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 num_hwirq)
{
	بنية sbi_irqchip_handler *fh, *lh;
	u32 i;

	𐤀𐤌 (!chip || !num_hwirq)
		أرجع SBI_EINVAL;
	𐤀𐤌 (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		أرجع SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	𐤀𐤌 (!fh || fh->first_hwirq != first_hwirq || fh->num_hwirq != num_hwirq)
		أرجع SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + num_hwirq - 1);
	𐤀𐤌 (!lh || lh != fh)
		أرجع SBI_ENODEV;

	𐤀𐤌 (chip->hwirq_mask) {
		لكل (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	𐤀𐤌 (chip->hwirq_cleanup) {
		لكل (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->node);
	أرجع 0;
}

بنية sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   بنية sbi_irqchip_device *first)
{
	بنية sbi_irqchip_device *chip;
	bool found = (first == عدم);

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		𐤀𐤌 (!found) {
			𐤀𐤌 (first == chip)
				found = true;
			استمر;
		}
		𐤀𐤌 ((chip->caps & caps) == caps)
			أرجع chip;
	}

	أرجع عدم;
}

بنية sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	بنية sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		𐤀𐤌 (chip->id == id)
			أرجع chip;
	}

	أرجع عدم;
}

صحيح sbi_irqchip_add_device(بنية sbi_irqchip_device *chip)
{
	بنية sbi_irqchip_hart_data *hd;
	بنية sbi_scratch *scratch;
	u32 i, h;

	𐤀𐤌 (!chip || !chip->num_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		أرجع SBI_EINVAL;

	𐤀𐤌 (sbi_irqchip_find_device(chip->id))
		أرجع SBI_EALREADY;

	𐤀𐤌 (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			𐤀𐤌 (!scratch)
				استمر;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			𐤀𐤌 (hd->chip && hd->chip != chip)
				أرجع SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(حجم(*chip->hwirqs) * chip->num_hwirq);
	𐤀𐤌 (!chip->hwirqs)
		أرجع SBI_ENOMEM;
	لكل (i = 0; i < chip->num_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->node, &irqchip_list);
	أرجع 0;
}

صحيح sbi_irqchip_init(بنية sbi_scratch *scratch, bool cold_boot)
{
	ثابت بنية sbi_platform *plat = sbi_platform_ptr(scratch);
	بنية sbi_irqchip_hart_data *hd;
	بنية sbi_irqchip_device *chip;
	صحيح rc;

	𐤀𐤌 (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(حجم(بنية sbi_irqchip_hart_data));
		𐤀𐤌 (!irqchip_hart_data_off)
			أرجع SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		𐤀𐤌 (rc)
			أرجع rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		𐤀𐤌 (!chip->warm_init)
			استمر;
		𐤀𐤌 (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			استمر;
		rc = chip->warm_init(chip);
		𐤀𐤌 (rc)
			أرجع rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	𐤀𐤌 (hd && hd->chip && hd->chip->process_hwirqs)
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
		𐤀𐤌 (i == current_hartindex())
			استمر;
		𐤀𐤌 (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			استمر;
		migrate_hidx = i;
		migrate = true;
		اكسر;
	}

	𐤀𐤌 (!migrate)
		اذهب skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		sbi_list_for_each_entry(h, &chip->handler_list, node) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			𐤀𐤌 (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	𐤀𐤌 (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
