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
	số_nguyên (*raw_handler)(struct sbi_irqchip_device *chip, u32 hwirq);

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
	số_nguyên (*callback)(u32 hwirq, void *priv);

	/** Callback private data */
	void *priv;
};

struct sbi_irqchip_hart_data {
	struct sbi_irqchip_device *chip;
};

static unsigned long irqchip_hart_data_off;
static SBI_LIST_HEAD(irqchip_list);

số_nguyên sbi_irqchip_process(void)
{
	struct sbi_irqchip_hart_data *hd;

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	nếu (!hd || !hd->chip || !hd->chip->process_hwirqs)
		trả_về SBI_ENODEV;

	trả_về hd->chip->process_hwirqs(hd->chip);
}

số_nguyên sbi_irqchip_process_hwirq(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;

	nếu (!chip || chip->num_hwirq <= hwirq)
		trả_về SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	nếu (!data->raw_handler)
		trả_về SBI_ENOENT;

	trả_về data->raw_handler(chip, hwirq);
}

static inline u32 sbi_irqchip_get_irq_state(struct sbi_irqchip_device *chip,
					     u32 hwirq)
{
	nếu (!chip || !chip->hwirqs || hwirq >= chip->num_hwirq)
		trả_về 0;

	trả_về chip->hwirqs[hwirq].irq_state;
}

bool sbi_irqchip_is_hwirq_enabled(struct sbi_irqchip_device *chip,
				   u32 hwirq)
{
	trả_về !!(sbi_irqchip_get_irq_state(chip, hwirq) & IRQ_ENABLED);
}

số_nguyên sbi_irqchip_unmask_hwirq(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;

	nếu (!chip || chip->num_hwirq <= hwirq)
		trả_về SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	nếu (sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		trả_về SBI_EALREADY;

	nếu (chip->hwirq_unmask)
		chip->hwirq_unmask(chip, hwirq);

	data->irq_state |= IRQ_ENABLED;
	trả_về 0;
}

số_nguyên sbi_irqchip_mask_hwirq(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;

	nếu (!chip || chip->num_hwirq <= hwirq)
		trả_về SBI_EINVAL;

	nếu (!sbi_irqchip_is_hwirq_enabled(chip, hwirq))
		trả_về SBI_EALREADY;

	nếu (chip->hwirq_mask)
		chip->hwirq_mask(chip, hwirq);

	data = &chip->hwirqs[hwirq];
	data->irq_state &= ~IRQ_ENABLED;
	trả_về 0;
}

static struct sbi_irqchip_handler *sbi_irqchip_find_handler(struct sbi_irqchip_device *chip,
							    u32 hwirq)
{
	struct sbi_irqchip_handler *h;

	nếu (!chip || chip->num_hwirq <= hwirq)
		trả_về NULL;

	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		nếu (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq))
			trả_về h;
	}

	trả_về NULL;
}

số_nguyên sbi_irqchip_set_hwirq_priv(struct sbi_irqchip_device *chip, u32 hwirq, void *priv)
{
	struct sbi_irqchip_hwirq_data *data;

	nếu (!chip || chip->num_hwirq <= hwirq)
	     trả_về SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->priv = priv;

	trả_về 0;
}

void *sbi_irqchip_get_hwirq_priv(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_hwirq_data *data;
	nếu (!chip || chip->num_hwirq <= hwirq)
	    trả_về NULL;

	data = &chip->hwirqs[hwirq];

	trả_về data->priv;
}

số_nguyên sbi_irqchip_raw_handler_default(struct sbi_irqchip_device *chip, u32 hwirq)
{
	struct sbi_irqchip_handler *h;
	số_nguyên rc = SBI_OK;

	nếu (!chip || chip->num_hwirq <= hwirq)
		trả_về SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	nếu (!h)
		trả_về SBI_EINVAL;

	nếu (h->callback)
		rc = h->callback(hwirq, h->priv);

	nếu (chip->hwirq_eoi)
		chip->hwirq_eoi(chip, hwirq);

	trả_về rc;
}

số_nguyên sbi_irqchip_set_raw_handler(struct sbi_irqchip_device *chip, u32 hwirq,
				số_nguyên (*raw_hndl)(struct sbi_irqchip_device *, u32))
{
	struct sbi_irqchip_hwirq_data *data;

	nếu (!chip || chip->num_hwirq <= hwirq)
		trả_về SBI_EINVAL;

	data = &chip->hwirqs[hwirq];
	data->raw_handler = raw_hndl;
	trả_về 0;
}

số_nguyên sbi_irqchip_write_msi(struct sbi_irqchip_device *chip, u32 hwirq,
			  const struct sbi_irqchip_msi_msg *msg)
{
	struct sbi_irqchip_handler *h;

	nếu (!chip || chip->num_hwirq <= hwirq || !msg)
		trả_về SBI_EINVAL;

	h = sbi_irqchip_find_handler(chip, hwirq);
	nếu (!h)
		trả_về SBI_EFAIL;
	nếu (!h->write_msi)
		trả_về SBI_ENOTSUPP;

	h->write_msi(hwirq, msg, h->priv);
	trả_về 0;
}

số_nguyên sbi_irqchip_get_affinity(struct sbi_irqchip_device *chip, u32 hwirq,
			     u32 *out_hart_index)
{
	nếu (!chip || chip->num_hwirq <= hwirq)
		trả_về SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	nếu (!sbi_irqchip_find_handler(chip, hwirq))
		trả_về SBI_ENOTSUPP;

	*out_hart_index = chip->hwirqs[hwirq].hart_index;
	trả_về 0;
}

số_nguyên sbi_irqchip_set_affinity(struct sbi_irqchip_device *chip, u32 hwirq,
			     u32 hart_index)
{
	struct sbi_irqchip_hwirq_data *data;
	số_nguyên rc;

	nếu (!chip || chip->num_hwirq <= hwirq || sbi_hart_count() <= hart_index)
		trả_về SBI_EINVAL;

	/*
	 * If no handler registered for hwirq then hwirq
	 * is not being used so return failure
	 */
	nếu (!sbi_irqchip_find_handler(chip, hwirq))
		trả_về SBI_ENOTSUPP;

	data = &chip->hwirqs[hwirq];
	nếu (data->hart_index != hart_index) {
		nếu (chip->hwirq_set_affinity) {
			rc = chip->hwirq_set_affinity(chip, hwirq, hart_index);
			nếu (rc)
				trả_về rc;
		}
		data->hart_index = hart_index;
	}

	trả_về 0;
}

static số_nguyên __sbi_irqchip_handler_set_affinity(struct sbi_irqchip_device *chip,
					      struct sbi_irqchip_handler *h,
					      u32 compare_hart_index,
					      u32 hart_index)
{
	u32 i, current_hart_index;
	số_nguyên rc;

	cho (i = 0; i < h->num_hwirq; i++) {
		rc = sbi_irqchip_get_affinity(chip, h->first_hwirq + i,
					      &current_hart_index);
		nếu (rc)
			trả_về rc;

		nếu (compare_hart_index != -1U &&
		    current_hart_index != compare_hart_index)
			continue;

		rc = sbi_irqchip_set_affinity(chip, h->first_hwirq + i, hart_index);
		nếu (rc)
			trả_về rc;
	}

	trả_về 0;
}

static số_nguyên __sbi_irqchip_register_handler(struct sbi_irqchip_device *chip,
					  u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
					  void (*write_msi)(u32 hwirq,
							    const struct sbi_irqchip_msi_msg *msg,
							    void *priv),
					  số_nguyên (*callback)(u32 hwirq, void *priv), void *priv)
{
	struct sbi_irqchip_handler *h, *th, *nh;
	u32 i, j;
	số_nguyên rc;

	cho (i = first_hwirq; i < (first_hwirq + num_hwirq); i++) {
		h = sbi_irqchip_find_handler(chip, i);
		nếu (h)
			trả_về SBI_EALREADY;
	}

	h = sbi_zalloc(sizeof(*h));
	nếu (!h)
		trả_về SBI_ENOMEM;
	h->first_hwirq = first_hwirq;
	h->num_hwirq = num_hwirq;
	h->write_msi = write_msi;
	h->callback = callback;
	h->priv = priv;

	nh = NULL;
	sbi_list_for_each_entry(th, &chip->handler_list, node) {
		nếu (h->first_hwirq < th->first_hwirq) {
			nh = th;
			break;
		}
	}
	nếu (nh)
		sbi_list_add(&h->node, &nh->node);
	không_thì
		sbi_list_add_tail(&h->node, &chip->handler_list);

	nếu (chip->hwirq_setup) {
		cho (i = 0; i < h->num_hwirq; i++) {
			rc = chip->hwirq_setup(chip, h->first_hwirq + i, hwirq_flags);
			nếu (rc) {
				nếu (chip->hwirq_cleanup) {
					cho (j = 0; j < i; j++)
						chip->hwirq_cleanup(chip, h->first_hwirq + j);
				}
				sbi_list_del(&h->node);
				sbi_free(h);
				trả_về rc;
			}
		}
	}

	rc = __sbi_irqchip_handler_set_affinity(chip, h, -1U, current_hartindex());
	nếu (rc) {
		nếu (chip->hwirq_cleanup) {
			cho (i = 0; i < h->num_hwirq; i++)
				chip->hwirq_cleanup(chip, h->first_hwirq + i);
		}
		sbi_list_del(&h->node);
		sbi_free(h);
		trả_về rc;
	}

	nếu (chip->hwirq_unmask) {
		cho (i = 0; i < h->num_hwirq; i++)
			chip->hwirq_unmask(chip, h->first_hwirq + i);
	}

	trả_về 0;
}

số_nguyên sbi_irqchip_register_msi(struct sbi_irqchip_device *chip, u32 num_hwirq,
			     void (*write_msi)(u32 hwirq,
					       const struct sbi_irqchip_msi_msg *msg,
					       void *priv),
			     số_nguyên (*callback)(u32 hwirq, void *priv), void *priv,
			     u32 *out_first_hwirq)
{
	struct sbi_irqchip_handler *h;
	bool found;
	u32 hwirq;

	nếu (!chip || !chip->hwirq_set_affinity || !num_hwirq ||
	    !write_msi || !callback || !out_first_hwirq)
		trả_về SBI_EINVAL;
	nếu (chip->num_hwirq < num_hwirq)
		trả_về SBI_EBAD_RANGE;

	hwirq = 0;
	found = false;
	sbi_list_for_each_entry(h, &chip->handler_list, node) {
		nếu (h->first_hwirq <= hwirq && hwirq < (h->first_hwirq + h->num_hwirq)) {
			hwirq = h->first_hwirq + h->num_hwirq;
		} không_thì nếu (hwirq < h->first_hwirq) {
			nếu (h->first_hwirq - hwirq >= num_hwirq) {
				found = true;
				break;
			} không_thì {
				hwirq = h->first_hwirq + h->num_hwirq;
			}
		}
	}
	nếu (!found && (chip->num_hwirq - hwirq) >= num_hwirq)
		found = true;
	nếu (!found)
		trả_về SBI_ENOSPC;
	*out_first_hwirq = hwirq;

	trả_về __sbi_irqchip_register_handler(chip, *out_first_hwirq,
					      num_hwirq, SBI_HWIRQ_FLAGS_NONE,
					      write_msi, callback, priv);
}

số_nguyên sbi_irqchip_register_handler(struct sbi_irqchip_device *chip,
				 u32 first_hwirq, u32 num_hwirq, u32 hwirq_flags,
				 số_nguyên (*callback)(u32 hwirq, void *priv), void *priv)
{
	nếu (!chip || !num_hwirq || !callback)
		trả_về SBI_EINVAL;
	nếu (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		trả_về SBI_EBAD_RANGE;

	trả_về __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq, hwirq_flags,
					      NULL, callback, priv);
}

số_nguyên sbi_irqchip_register_reserved(struct sbi_irqchip_device *chip,
				  u32 first_hwirq, u32 num_hwirq)
{
	nếu (!chip || !num_hwirq)
		trả_về SBI_EINVAL;
	nếu (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		trả_về SBI_EBAD_RANGE;

	trả_về __sbi_irqchip_register_handler(chip, first_hwirq, num_hwirq,
					      SBI_HWIRQ_FLAGS_NONE, NULL, NULL, NULL);
}

số_nguyên sbi_irqchip_unregister_handler(struct sbi_irqchip_device *chip,
				   u32 first_hwirq, u32 num_hwirq)
{
	struct sbi_irqchip_handler *fh, *lh;
	u32 i;

	nếu (!chip || !num_hwirq)
		trả_về SBI_EINVAL;
	nếu (chip->num_hwirq <= first_hwirq ||
	    chip->num_hwirq <= (first_hwirq + num_hwirq - 1))
		trả_về SBI_EBAD_RANGE;

	fh = sbi_irqchip_find_handler(chip, first_hwirq);
	nếu (!fh || fh->first_hwirq != first_hwirq || fh->num_hwirq != num_hwirq)
		trả_về SBI_ENODEV;
	lh = sbi_irqchip_find_handler(chip, first_hwirq + num_hwirq - 1);
	nếu (!lh || lh != fh)
		trả_về SBI_ENODEV;

	nếu (chip->hwirq_mask) {
		cho (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_mask(chip, fh->first_hwirq + i);
	}

	nếu (chip->hwirq_cleanup) {
		cho (i = 0; i < fh->num_hwirq; i++)
			chip->hwirq_cleanup(chip, fh->first_hwirq + i);
	}

	sbi_list_del(&fh->node);
	trả_về 0;
}

struct sbi_irqchip_device *sbi_irqchip_find_device_by_caps(unsigned long caps,
							   struct sbi_irqchip_device *first)
{
	struct sbi_irqchip_device *chip;
	bool found = (first == NULL);

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		nếu (!found) {
			nếu (first == chip)
				found = true;
			continue;
		}
		nếu ((chip->caps & caps) == caps)
			trả_về chip;
	}

	trả_về NULL;
}

struct sbi_irqchip_device *sbi_irqchip_find_device(u32 id)
{
	struct sbi_irqchip_device *chip;

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		nếu (chip->id == id)
			trả_về chip;
	}

	trả_về NULL;
}

số_nguyên sbi_irqchip_add_device(struct sbi_irqchip_device *chip)
{
	struct sbi_irqchip_hart_data *hd;
	struct sbi_scratch *scratch;
	u32 i, h;

	nếu (!chip || !chip->num_hwirq || !sbi_hartmask_weight(&chip->target_harts))
		trả_về SBI_EINVAL;

	nếu (sbi_irqchip_find_device(chip->id))
		trả_về SBI_EALREADY;

	nếu (chip->process_hwirqs) {
		sbi_hartmask_for_each_hartindex(h, &chip->target_harts) {
			scratch = sbi_hartindex_to_scratch(h);
			nếu (!scratch)
				continue;

			hd = sbi_scratch_offset_ptr(scratch, irqchip_hart_data_off);
			nếu (hd->chip && hd->chip != chip)
				trả_về SBI_EINVAL;

			hd->chip = chip;
		}
	}

	chip->hwirqs = sbi_zalloc(sizeof(*chip->hwirqs) * chip->num_hwirq);
	nếu (!chip->hwirqs)
		trả_về SBI_ENOMEM;
	cho (i = 0; i < chip->num_hwirq; i++) {
		sbi_irqchip_set_raw_handler(chip, i, sbi_irqchip_raw_handler_default);
		chip->hwirqs[i].hart_index = -1U;
	}

	SBI_INIT_LIST_HEAD(&chip->handler_list);

	sbi_list_add_tail(&chip->node, &irqchip_list);
	trả_về 0;
}

số_nguyên sbi_irqchip_init(struct sbi_scratch *scratch, bool cold_boot)
{
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);
	struct sbi_irqchip_hart_data *hd;
	struct sbi_irqchip_device *chip;
	số_nguyên rc;

	nếu (cold_boot) {
		irqchip_hart_data_off =
			sbi_scratch_alloc_offset(sizeof(struct sbi_irqchip_hart_data));
		nếu (!irqchip_hart_data_off)
			trả_về SBI_ENOMEM;
		rc = sbi_platform_irqchip_init(plat);
		nếu (rc)
			trả_về rc;
	}

	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		nếu (!chip->warm_init)
			continue;
		nếu (!sbi_hartmask_test_hartindex(current_hartindex(), &chip->target_harts))
			continue;
		rc = chip->warm_init(chip);
		nếu (rc)
			trả_về rc;
	}

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	nếu (hd && hd->chip && hd->chip->process_hwirqs)
		csr_set(CSR_MIE, MIP_MEIP);

	trả_về 0;
}

void sbi_irqchip_exit(struct sbi_scratch *scratch)
{
	struct sbi_irqchip_hart_data *hd;
	struct sbi_irqchip_device *chip;
	struct sbi_irqchip_handler *h;
	u32 migrate_hidx = -1U;
	bool migrate = false;
	số_nguyên rc;

	sbi_for_each_hartindex(i) {
		nếu (i == current_hartindex())
			continue;
		nếu (__sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOPPED ||
		    __sbi_hsm_hart_get_state(i) == SBI_HSM_STATE_STOP_PENDING)
			continue;
		migrate_hidx = i;
		migrate = true;
		break;
	}

	nếu (!migrate)
		goto skip_migrate;
	sbi_list_for_each_entry(chip, &irqchip_list, node) {
		sbi_list_for_each_entry(h, &chip->handler_list, node) {
			rc = __sbi_irqchip_handler_set_affinity(chip, h,
								current_hartindex(),
								migrate_hidx);
			nếu (rc) {
				sbi_printf("%s: chip 0x%x handler 0x%x set affinity (err %d)\n",
					   __func__, chip->id, h->first_hwirq, rc);
			}
		}
	}
skip_migrate:

	hd = sbi_scratch_thishart_offset_ptr(irqchip_hart_data_off);
	nếu (hd && hd->chip && hd->chip->process_hwirqs)
		csr_clear(CSR_MIE, MIP_MEIP);
}
