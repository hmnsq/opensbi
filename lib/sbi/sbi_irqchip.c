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
ساختار sbi_irqchip_hwirq_data {
	/** raw hardware interrupt handler */
	صحیح (*raw_handler)(ساختار sbi_irqchip_device *chip, u32 hwirq);

#define IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_index;

	/** chip's private data */
	تهی *priv;
};

/** Internal irqchip interrupt handler */
ساختار sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	ساختار sbi_dlist گره;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 رقم_hwirq;

	/** Write MSI function of this handler */
	تهی (*write_msi)(u32 hwirq, ثابت ساختار sbi_irqchip_msi_پیام *پیام, تهی *priv);

	/** Callback function of this handler */
	صحیح (*callback)(u32 hwirq, تهی *priv);

	/** Callback private data */
	تهی *priv;
};

ساختار sbi_irqchip_hart_data {
	ساختار sbi_irqchip_device *chip;
};

ایستا unsigned long irqchip_hart_data_off;
ایستا SBI_LIST_HEAD(irqchip_list);

صحیح sbi_irqchip_process(تهی)
{
	ساختار sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	اگر (!hd || !hd->chip || !hd->chip->process_hwirqs)
		بازگشت SBI_ENODEV;

	بازگشت hd->chip->process_hwirqs(hd->chip);
}

صحیح sbi_irqchip_process_hwirq(ساختار sbi_irqchip_device *chip, u32 hwirq)
{
	ساختار sbi_irqchip_hwirq_data *data;

	اگر (!chip || chip->رقم_hwirq <= hwirq)
		بازگشت SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	اگر (!data->raw_handler)
		بازگشت SBI_ENOENT;

	بازگشت data->raw_handler(chip, hwirq);
}

ایستا درونخطی u32 sbi_irqchip_get_irq_state(ساختار sbi_irqchip_device *chip,
					     u32 hwirq)
{
	اگر (!chip || !chip->hwirqs || hwirq >= chip->رقم_hwirq)
		بازگشت 0;

	بازگشت chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(ساختار sbi_irqchip_device *chip,
				   u32 hwirq)
{
	بازگشت !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

صحیح sbi_irqchip_unmask_hwirq(ساختار sbi_irqchip_device *chip, u32 hwirq)
{
	ساختار sbi_irqchip_hwirq_data *data;

	اگر (!chip || chip->رقم_hwirq <= hwirq)
		بازگشت SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	اگر (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		بازگشت SBI_EALREADY;

	اگر (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	بازگشت 0;
}

صحیح sbi_irqchip_mask_hwirq(ساختار sbi_irqchip_device *chip, u32 hwirq)
{
	ساختار sbi_irqchip_hwirq_data *data;

	اگر (!chip || chip->رقم_hwirq <= hwirq)
		بازگشت SBI_EINVAL;

	اگر (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		بازگشت SBI_EALREADY;

	اگر (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	بازگشت 0;
}

ایستا ساختار sbi_irqchip_handler *sbi_irqchip_find_handler(ساختار sbi_irqchip_device *chip,
							    u32 hwirq)
{
	ساختار sbi_irqchip_handler *h;

	اگر (!chip || chip->رقم_hwirq <= hwirq)
		بازگشت پوچ;

	sbi_list_for_each_entry(h, &chip->handler_list, گره) {
		اگر (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->رقم_hwirq))
			بازگشت h;
	}

	بازگشت پوچ;
}

صحیح sbi_irqchip_set_hwirq_priv(ساختار sbi_irqchip_device *chip, u32 hwirq, تهی *priv)
{
	ساختار sbi_irqchip_hwirq_data *data;

	اگر (!chip || chip->رقم_hwirq <= hwirq)
	     بازگشت SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	بازگشت 0;
}

تهی *sbi_irqchip_get_hwirq_priv(ساختار sbi_irqchip_device *chip, u32 hwirq)
{
	ساختار sbi_irqchip_hwirq_data *data;
	اگر (!chip || chip->رقم_hwirq <= hwirq)
	    بازگشت پوچ;

	data = &chip->hwirqs[hwirq];

	بازگشت data->priv;
}

صحیح sbi_irqchip_raw_handler_default(ساختار sbi_irqchip_device *chip, u32 hwirq)
{
	ساختار sbi_irqchip_handler *h;
	صحیح rc = SBI_OK;

	اگر (!chip || chip->رقم_hwirq <= hwirq)
		بازگشت SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	اگر (!h)
		بازگشت SBI_EINVAL;

	اگر (h->callback)
		rc = h->callback(hwirq, h->priv);

	اگر (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	بازگشت rc;
}

صحیح sbi_irqchip_set_raw_handler(ساختار sbi_irqchip_device *chip, u32 hwirq,
				صحیح (*raw_hndl)(ساختار sbi_irqchip_device *, u32))
{
	ساختار sbi_irqchip_hwirq_data *data;

	اگر (!chip || chip->رقم_hwirq <= hwirq)
		بازگشت SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	بازگشت 0;
}

صحیح sbi_irqchip_write_msi(ساختار sbi_irqchip_device *chip, u32 hwirq,
			  ثابت ساختار sbi_irqchip_msi_پیام *پیام)
{
	ساختار sbi_irqchip_handler *h;

	اگر (!chip || chip->رقم_hwirq <= hwirq || !پیام)
		بازگشت SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	اگر (!h)
		بازگشت SBI_EFAIL;
	اگر (!h->write_msi)
		بازگشت SBI_ENOTSUPP;

	h->write_msi(hwirq, پیام, h->priv);
	بازگشت 0;
}

صحیح sbi_irqchip_get_affinity(ساختار sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	اگر (!chip || chip->رقم_hwirq <= hwirq)
		بازگشت SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	اگر (!sbi_irqchip_find_handler(chip, hwirq))
		بازگشت SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	بازگشت 0;
}

صحیح sbi_irqchip_set_affinity(ساختار sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	ساختار sbi_irqchip_hwirq_data *data;
	صحیح rc;

	اگر (!chip || chip->رقم_hwirq <= hwirq || sbi_hart_شمار() <= hart_index)
		بازگشت SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	اگر (!sbi_irqchip_find_handler(chip, hwirq))
		بازگشت SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	اگر (data->hart_index != hart_index) {
		اگر (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			اگر (rc)
				بازگشت rc;
		}
		data->hart_index = hart_index;
	}

	بازگشت 0;
}

ایستا صحیح __sbi_irqchip_handler_set_affinity(ساختار sbi_irqchip_device *chip,
					      ساختار sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	صحیح rc;

	برای (i = 0; i < h->رقم_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		اگر (rc)
			بازگشت rc;

		اگر (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			ادامه;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		اگر (rc)
			بازگشت rc;
	}

	بازگشت 0;
}

ایستا صحیح __sbi_irqchip_register_handler(ساختار sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 رقم_hwirq, u32 hwirq_flags,
					  تهی (*write_msi)(u32 hwirq,
							    ثابت ساختار sbi_irqchip_msi_پیام *پیام,
							    تهی *priv),
					  صحیح (*callback)(u32 hwirq, تهی *priv), تهی *priv)
{
	ساختار sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	صحیح rc;

	برای (i = first_hwirq; i < (first_hwirq + رقم_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		اگر (h)
			بازگشت SBI_EALREADY;
	}

	h = sbi_zalloc(اندازه(*h));
	اگر (!h)
		بازگشت SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->رقم_hwirq = رقم_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = پوچ;
	sbi_list_for_each_entry(th, &chip->handler_list, گره) {
		اگر (h->first_hwirq < th->first_hwirq) {
			nh = th;
			بشکن;
		}
	}
	اگر (nh)
		sbi_list_add(&h->گره, &nh->گره);
	وگرنه
		sbi_list_add_tail(&h->گره, &chip->handler_list);

	اگر (chip->hwirq_setup) {
		برای (i = 0; i < h->رقم_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			اگر (rc) {
				اگر (chip->hwirq_cleanup) {
					برای (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->گره);
				sbi_free(h);
				بازگشت rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	اگر (rc) {
		اگر (chip->hwirq_cleanup) {
			برای (i = 0; i < h->رقم_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->گره);
		sbi_free(h);
		بازگشت rc;
	}

	اگر (chip->hwirq_unmask) {
		برای (i = 0; i < h->رقم_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	بازگشت 0;
}

صحیح sbi_irqchip_register_msi(ساختار sbi_irqchip_device *chip, u32 رقم_hwirq,
			     تهی (*write_msi)(u32 hwirq,
					       ثابت ساختار sbi_irqchip_msi_پیام *پیام,
					       تهی *priv),
			     صحیح (*callback)(u32 hwirq, تهی *priv), تهی *priv,
			     u32 *out_first_hwirq)
{
	ساختار sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	اگر (!chip || !chip->hwirq_set_affinity || !رقم_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		بازگشت SBI_EINVAL;
	اگر (chip->رقم_hwirq < رقم_hwirq)
		بازگشت SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, گره) {
		اگر (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->رقم_hwirq)) {
			hwirq = h->first_hwirq + h->رقم_hwirq;
		} وگرنه اگر (hwirq < h->first_hwirq) {
			اگر (h->first_hwirq - hwirq >= رقم_hwirq) {
				found = true;
				بشکن;
			} وگرنه {
				hwirq = h->first_hwirq + h->رقم_hwirq;
			}
		}
	}
	اگر (!found && (chip->رقم_hwirq - hwirq) >= رقم_hwirq)
		found = true;
	اگر (!found)
		بازگشت SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	بازگشت __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      رقم_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

صحیح sbi_irqchip_register_handler(ساختار sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 رقم_hwirq, u32 hwirq_flags,
				 صحیح (*callback)(u32 hwirq, تهی *priv), تهی *priv)
{
	اگر (!chip || !رقم_hwirq || !callback)
		بازگشت SBI_EINVAL;
	اگر (chip->رقم_hwirq <= first_hwirq ||
	    chip->رقم_hwirq <= (first_hwirq + رقم_hwirq - 1))
		بازگشت SBI_EBAD_RANGE;

	بازگشت __sbi_irqchip_register_handler(chip, first_hwirq, رقم_hwirq, hwirq_flags,
					      پوچ, callback, priv);
}

صحیح sbi_irqchip_register_reserved(ساختار sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 رقم_hwirq)
{
	اگر (!chip || !رقم_hwirq)
		بازگشت SBI_EINVAL;
	اگر (chip->رقم_hwirq <= first_hwirq ||
	    chip->رقم_hwirq <= (first_hwirq + رقم_hwirq - 1))
		بازگشت SBI_EBAD_RANGE;

	بازگشت __sbi_irqchip_register_handler(chip, first_hwirq, رقم_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, پوچ, پوچ, پوچ);
}

صحیح sbi_irqchip_unregister_handler(ساختار sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 رقم_hwirq)
{
	ساختار sbi_irqchip_handler *fh, *lh;
	u32 i;

	اگر (!chip || !رقم_hwirq)
		بازگشت SBI_EINVAL;
	اگر (chip->رقم_hwirq <= first_hwirq ||
	    chip->رقم_hwirq <= (first_hwirq + رقم_hwirq - 1))
		بازگشت SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	اگر (!fh || fh->first_hwirq != first_hwirq || fh->رقم_hwirq != رقم_hwirq)
		بازگشت SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + رقم_hwirq - 1);
	اگر (!lh || lh != fh)
		بازگشت SBI_ENODEV;

	اگر (chip->hwirq_mask) {
		برای (i = 0; i < fh->رقم_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	اگر (chip->hwirq_cleanup) {
		برای (i = 0; i < fh->رقم_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->گره);
	بازگشت 0;
}

ساختار sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   ساختار sbi_irqchip_device *first)
{
	ساختار sbi_irqchip_device *chip;
	bool found = (first == پوچ);

	sbi_list_for_each_entry(chip, &irqchip_list, گره) {
		اگر (!found) {
			اگر (first == chip)
				found = true;
			ادامه;
		}
		اگر ((chip->caps & caps) == caps)
			بازگشت chip;
	}

	بازگشت پوچ;
}

ساختار sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	ساختار sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, گره) {
		اگر (chip->id == id)
			بازگشت chip;
	}

	بازگشت پوچ;
}

صحیح sbi_irqchip_add_device(ساختار sbi_irqchip_device *chip)
{
	ساختار sbi_irqchip_hart_data *hd;
	ساختار sbi_scratch *scratch;
	u32 i, h;

	اگر (!chip || !chip->رقم_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		بازگشت SBI_EINVAL;

	اگر (sbi_irqchip_find_device(chip->id))
		بازگشت SBI_EALREADY;

	اگر (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			اگر (!scratch)
				ادامه;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			اگر (hd->chip && hd->chip != chip)
				بازگشت SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(اندازه(*chip->hwirqs) * chip->رقم_hwirq);
	اگر (!chip->hwirqs)
		بازگشت SBI_ENOMEM;
	برای (i = 0; i < chip->رقم_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->گره, &irqchip_list);
	بازگشت 0;
}

صحیح sbi_irqchip_init(ساختار sbi_scratch *scratch, bool cold_boot)
{
	ثابت ساختار sbi_platform *plat = sbi_platform_ptr(scratch);
	ساختار sbi_irqchip_hart_data *hd;
	ساختار sbi_irqchip_device *chip;
	صحیح rc;

	اگر (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(اندازه(ساختار sbi_irqchip_hart_data));
		اگر (!irqchip_hart_data_off)
			بازگشت SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		اگر (rc)
			بازگشت rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, گره) {
		اگر (!chip->warm_init)
			ادامه;
		اگر (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			ادامه;
		rc = chip->warm_init(chip);
		اگر (rc)
			بازگشت rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	اگر (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	بازگشت 0;
}

تهی sbi_irqchip_exit(ساختار sbi_scratch *scratch)
{
	ساختار sbi_irqchip_hart_data *hd;
	ساختار sbi_irqchip_device *chip;
	ساختار sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	صحیح rc;

	sbi_for_each_hartindex(i) {
		اگر (i == current_hartindex())
			ادامه;
		اگر (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			ادامه;
		migrate_hidx = i;
		migrate = true;
		بشکن;
	}

	اگر (!migrate)
		برو skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, گره) {
		sbi_list_for_each_entry(h, &chip->handler_list, گره) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			اگر (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	اگر (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
