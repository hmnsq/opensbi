/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <apatel@ventanamicro.com>
 */

#include <sbi/sbi_console.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_hsm.h>
#include <sbi/sbi_irqchip.h>
#include <sbi/sbi_list.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_scratch.h>

/** Internal irqchip hardware interrupt data */
struct sbi_irqchip_hwirq_data {
	/** raw hardware interrupt handler */
	цяло (*raw_handler)(struct sbi_irqchip_device *chip, u32 hwirq);

#define IRQ_ENABLED	BIT(0)
	/** interrupt state
	 * bit 0 - 1: enabled, 0: disabled */
	u32 irq_state;

	/** target hart index */
	u32 hart_index;

	/** chip's private data */
	void *priv;
};

/** Internal irqchip interrupt handler */
struct sbi_irqchip_handler {
	/** Node in the list of irqchip handlers (private) */
	struct sbi_dlist node;

	/** First hardware IRQ handled by this handler */
	u32 first_hwirq;

	/** Number of consecutive hardware IRQs handled by this handler */
	u32 num_hwirq;

	/** Write MSI function of this handler */
	void (*write_msi)(u32 hwirq, const struct sbi_irqchip_msi_msg *msg, void *priv);

	/** Callback function of this handler */
	цяло (*callback)(u32 hwirq, void *priv);

	/** Callback private data */
	void *priv;
};

struct sbi_irqchip_hart_data {
	struct sbi_irqchip_device *chip;
};

static unsigned long irqchip_hart_data_off;
static SBI_LIST_HEAD(irqchip_list);

цяло sbi_irqchip_process(void)
{
	struct sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ако (!hd || !hd->chip || !hd->chip->process_hwirqs)
		върни SBI_ENODEV;

	върни hd->chip->process_hwirqs(hd->chip);
}

цяло sbi_irqchip_process_hwirq(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;

	ако (!chip || chip->num_hwirq <= hwirq)
		върни SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	ако (!data->raw_handler)
		върни SBI_ENOENT;

	върни data->raw_handler(chip, hwirq);
}

static inline u32 sbi_irqchip_get_irq_state(struct sbi_irqchip_device *chip,
					     u32 hwirq)
{
	ако (!chip || !chip->hwirqs || hwirq >= chip->num_hwirq)
		върни 0;

	върни chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(struct sbi_irqchip_device *chip,
				   u32 hwirq)
{
	върни !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

цяло sbi_irqchip_unmask_hwirq(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;

	ако (!chip || chip->num_hwirq <= hwirq)
		върни SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	ако (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		върни SBI_EALREADY;

	ако (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	върни 0;
}

цяло sbi_irqchip_mask_hwirq(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;

	ако (!chip || chip->num_hwirq <= hwirq)
		върни SBI_EINVAL;

	ако (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		върни SBI_EALREADY;

	ако (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	върни 0;
}

static struct sbi_irqchip_handler *sbi_irqchip_find_handler(struct sbi_irqchip_device *chip,
							    u32 hwirq)
{
	struct sbi_irqchip_handler *h;

	ако (!chip || chip->num_hwirq <= hwirq)
		върни NULL;

	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		ако (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq))
			върни h;
	}

	върни NULL;
}

цяло sbi_irqchip_set_hwirq_priv(struct sbi_irqchip_device *chip, u32 hwirq, void *priv)
{
	struct sbi_irqchip_hwirq_data *data;

	ако (!chip || chip->num_hwirq <= hwirq)
	     върни SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	върни 0;
}

void *sbi_irqchip_get_hwirq_priv(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;
	ако (!chip || chip->num_hwirq <= hwirq)
	    върни NULL;

	data = &chip->hwirqs[hwirq];

	върни data->priv;
}

цяло sbi_irqchip_raw_handler_default(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_handler *h;
	цяло rc = SBI_OK;

	ако (!chip || chip->num_hwirq <= hwirq)
		върни SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	ако (!h)
		върни SBI_EINVAL;

	ако (h->callback)
		rc = h->callback(hwirq, h->priv);

	ако (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	върни rc;
}

цяло sbi_irqchip_set_raw_handler(struct sbi_irqchip_device *chip, u32 hwirq,
				цяло (*raw_hndl)(struct sbi_irqchip_device *, u32))
{
	struct sbi_irqchip_hwirq_data *data;

	ако (!chip || chip->num_hwirq <= hwirq)
		върни SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	върни 0;
}

цяло sbi_irqchip_write_msi(struct sbi_irqchip_device *chip, u32 hwirq,
			  const struct sbi_irqchip_msi_msg *msg)
{
	struct sbi_irqchip_handler *h;

	ако (!chip || chip->num_hwirq <= hwirq || !msg)
		върни SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	ако (!h)
		върни SBI_EFAIL;
	ако (!h->write_msi)
		върни SBI_ENOTSUPP;

	h->write_msi(hwirq, msg, h->priv);
	върни 0;
}

цяло sbi_irqchip_get_affinity(struct sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	ако (!chip || chip->num_hwirq <= hwirq)
		върни SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ако (!sbi_irqchip_find_handler(chip, hwirq))
		върни SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	върни 0;
}

цяло sbi_irqchip_set_affinity(struct sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	struct sbi_irqchip_hwirq_data *data;
	цяло rc;

	ако (!chip || chip->num_hwirq <= hwirq || sbi_hart_count() <= hart_index)
		върни SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	ако (!sbi_irqchip_find_handler(chip, hwirq))
		върни SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	ако (data->hart_index != hart_index) {
		ако (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			ако (rc)
				върни rc;
		}
		data->hart_index = hart_index;
	}

	върни 0;
}

static цяло __sbi_irqchip_handler_set_affinity(struct sbi_irqchip_device *chip,
					      struct sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	цяло rc;

	за (i = 0; i < h->num_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		ако (rc)
			върни rc;

		ако (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			continue;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		ако (rc)
			върни rc;
	}

	върни 0;
}

static цяло __sbi_irqchip_register_handler(struct sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
					  void (*write_msi)(u32 hwirq,
							    const struct sbi_irqchip_msi_msg *msg,
							    void *priv),
					  цяло (*callback)(u32 hwirq, void *priv), void *priv)
{
	struct sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	цяло rc;

	за (i = first_hwirq; i < (first_hwirq + num_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		ако (h)
			върни SBI_EALREADY;
	}

	h = sbi_zalloc(sizeof(*h));
	ако (!h)
		върни SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->num_hwirq = num_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = NULL;
	sbi_list_for_each_entry(th, &chip->handler_list, node) {
		ако (h->first_hwirq < th->first_hwirq) {
			nh = th;
			break;
		}
	}
	ако (nh)
		sbi_list_add(&h->node, &nh->node);
	иначе
		sbi_list_add_tail(&h->node, &chip->handler_list);

	ако (chip->hwirq_setup) {
		за (i = 0; i < h->num_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			ако (rc) {
				ако (chip->hwirq_cleanup) {
					за (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->node);
				sbi_free(h);
				върни rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	ако (rc) {
		ако (chip->hwirq_cleanup) {
			за (i = 0; i < h->num_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->node);
		sbi_free(h);
		върни rc;
	}

	ако (chip->hwirq_unmask) {
		за (i = 0; i < h->num_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	върни 0;
}

цяло sbi_irqchip_register_msi(struct sbi_irqchip_device *chip, u32 num_hwirq,
			     void (*write_msi)(u32 hwirq,
					       const struct sbi_irqchip_msi_msg *msg,
					       void *priv),
			     цяло (*callback)(u32 hwirq, void *priv), void *priv,
			     u32 *out_first_hwirq)
{
	struct sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	ако (!chip || !chip->hwirq_set_affinity || !num_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		върни SBI_EINVAL;
	ако (chip->num_hwirq < num_hwirq)
		върни SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		ако (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq)) {
			hwirq = h->first_hwirq + h->num_hwirq;
		} иначе ако (hwirq < h->first_hwirq) {
			ако (h->first_hwirq - hwirq >= num_hwirq) {
				found = true;
				break;
			} иначе {
				hwirq = h->first_hwirq + h->num_hwirq;
			}
		}
	}
	ако (!found && (chip->num_hwirq - hwirq) >= num_hwirq)
		found = true;
	ако (!found)
		върни SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	върни __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      num_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

цяло sbi_irqchip_register_handler(struct sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
				 цяло (*callback)(u32 hwirq, void *priv), void *priv)
{
	ако (!chip || !num_hwirq || !callback)
		върни SBI_EINVAL;
	ако (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		върни SBI_EBAD_RANGE;

	върни __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq, hwirq_flags,
					      NULL, callback, priv);
}

цяло sbi_irqchip_register_reserved(struct sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 num_hwirq)
{
	ако (!chip || !num_hwirq)
		върни SBI_EINVAL;
	ако (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		върни SBI_EBAD_RANGE;

	върни __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, NULL, NULL, NULL);
}

цяло sbi_irqchip_unregister_handler(struct sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 num_hwirq)
{
	struct sbi_irqchip_handler *fh, *lh;
	u32 i;

	ако (!chip || !num_hwirq)
		върни SBI_EINVAL;
	ако (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		върни SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	ако (!fh || fh->first_hwirq != first_hwirq || fh->num_hwirq != num_hwirq)
		върни SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + num_hwirq - 1);
	ако (!lh || lh != fh)
		върни SBI_ENODEV;

	ако (chip->hwirq_mask) {
		за (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	ако (chip->hwirq_cleanup) {
		за (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->node);
	върни 0;
}

struct sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   struct sbi_irqchip_device *first)
{
	struct sbi_irqchip_device *chip;
	bool found = (first == NULL);

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		ако (!found) {
			ако (first == chip)
				found = true;
			continue;
		}
		ако ((chip->caps & caps) == caps)
			върни chip;
	}

	върни NULL;
}

struct sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	struct sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		ако (chip->id == id)
			върни chip;
	}

	върни NULL;
}

цяло sbi_irqchip_add_device(struct sbi_irqchip_device *chip)
{
	struct sbi_irqchip_hart_data *hd;
	struct sbi_scratch *scratch;
	u32 i, h;

	ако (!chip || !chip->num_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		върни SBI_EINVAL;

	ако (sbi_irqchip_find_device(chip->id))
		върни SBI_EALREADY;

	ако (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			ако (!scratch)
				continue;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			ако (hd->chip && hd->chip != chip)
				върни SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(sizeof(*chip->hwirqs) * chip->num_hwirq);
	ако (!chip->hwirqs)
		върни SBI_ENOMEM;
	за (i = 0; i < chip->num_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->node, &irqchip_list);
	върни 0;
}

цяло sbi_irqchip_init(struct sbi_scratch *scratch, bool cold_boot)
{
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);
	struct sbi_irqchip_hart_data *hd;
	struct sbi_irqchip_device *chip;
	цяло rc;

	ако (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(sizeof(struct sbi_irqchip_hart_data));
		ако (!irqchip_hart_data_off)
			върни SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		ако (rc)
			върни rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		ако (!chip->warm_init)
			continue;
		ако (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			continue;
		rc = chip->warm_init(chip);
		ако (rc)
			върни rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ако (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	върни 0;
}

void sbi_irqchip_exit(struct sbi_scratch *scratch)
{
	struct sbi_irqchip_hart_data *hd;
	struct sbi_irqchip_device *chip;
	struct sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	цяло rc;

	sbi_for_each_hartindex(i) {
		ако (i == current_hartindex())
			continue;
		ако (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			continue;
		migrate_hidx = i;
		migrate = true;
		break;
	}

	ако (!migrate)
		goto skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		sbi_list_for_each_entry(h, &chip->handler_list, node) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			ако (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	ако (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
