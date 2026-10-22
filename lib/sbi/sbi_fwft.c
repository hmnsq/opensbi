/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2024 Rivos Inc.
 *
 * Authors:
 *   Clément Léger <cleger@rivosinc.com>
 */

#include <sbi/sbi_console.h>
#include <sbi/sbi_bitmap.h>
#include <sbi/sbi_ecall_interface.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_hart.h>
#include <sbi/sbi_heap.h>
#include <sbi/sbi_hfence.h>
#include <sbi/sbi_scratch.h>
#include <sbi/sbi_string.h>
#include <sbi/sbi_tlb.h>
#include <sbi/sbi_types.h>

#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>

/** Offset of pointer to FWFT HART state in scratch space */
static unsigned long fwft_ptr_offset;

#define fwft_get_hart_state_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), void *, fwft_ptr_offset)

#define fwft_thishart_state_ptr()					\
	fwft_get_hart_state_ptr(sbi_scratch_thishart_ptr())

#define fwft_set_hart_state_ptr(__scratch, __phs)			\
	sbi_scratch_write_type((__scratch), void *, fwft_ptr_offset, (__phs))

#define MIS_DELEG (1UL << CAUSE_MISALIGNED_LOAD | 1UL << CAUSE_MISALIGNED_STORE)

struct fwft_config;

struct fwft_feature {
	enum sbi_fwft_feature_t id;
	ціле (*supported)(struct fwft_config *conf);
	ціле (*set)(struct fwft_config *conf, unsigned long value);
	ціле (*get)(struct fwft_config *conf, unsigned long *value);
};

struct fwft_config {
	const struct fwft_feature *feature;
	unsigned long flags;
};

struct fwft_hart_state {
	unsigned ціле config_count;
	struct fwft_config configs[];
};

static const unsigned long fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_STACK,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

static bool fwft_is_defined_feature(enum sbi_fwft_feature_t feature)
{
	ціле i;

	для (i = 0; i < array_size(fwft_defined_features); i++) {
		якщо (fwft_defined_features[i] == feature)
			повернути true;
	}

	повернути false;
}

static ціле fwft_menvcfg_set_bit(unsigned long value, unsigned long bit)
{
	якщо (value == 1) {
		якщо (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		інакше
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} інакше якщо (value == 0) {
		якщо (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		інакше
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} інакше {
		повернути SBI_EINVAL;
	}

	повернути SBI_OK;
}

static ціле fwft_menvcfg_read_bit(unsigned long *value, unsigned long bit)
{
	unsigned long cfg;

	якщо (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	інакше
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*value = cfg != 0;

	повернути SBI_OK;
}

static ціле fwft_misaligned_delegation_supported(struct fwft_config *conf)
{
	якщо (!misa_extension('S'))
		повернути SBI_ENOTSUPP;

	повернути SBI_OK;
}

static ціле fwft_set_misaligned_delegation(struct fwft_config *conf,
					 unsigned long value)
{
	якщо (value == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	інакше якщо (value == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	інакше
		повернути SBI_EINVAL;

	повернути SBI_OK;
}

static ціле fwft_get_misaligned_delegation(struct fwft_config *conf,
					 unsigned long *value)
{
	*value = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	повернути SBI_OK;
}

static ціле fwft_double_trap_supported(struct fwft_config *conf)
{
	якщо (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		повернути SBI_ENOTSUPP;

	повернути SBI_OK;
}

static ціле fwft_set_double_trap(struct fwft_config *conf, unsigned long value)
{
	повернути fwft_menvcfg_set_bit(value, ENVCFG_DTE_SHIFT);
}

static ціле fwft_get_double_trap(struct fwft_config *conf, unsigned long *value)
{
	повернути fwft_menvcfg_read_bit(value, ENVCFG_DTE_SHIFT);
}

static ціле fwft_adue_supported(struct fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	якщо (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		повернути SBI_ENOTSUPP;

	повернути SBI_OK;
}

static ціле fwft_set_adue(struct fwft_config *conf, unsigned long value)
{
	ціле res = fwft_menvcfg_set_bit(value, ENVCFG_ADUE_SHIFT);

	якщо (res == SBI_OK) {
		__sbi_sfence_vma_all();

		якщо (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	повернути res;
}

static ціле fwft_get_adue(struct fwft_config *conf, unsigned long *value)
{
	повернути fwft_menvcfg_read_bit(value, ENVCFG_ADUE_SHIFT);
}

static ціле fwft_lpad_supported(struct fwft_config *conf)
{
	якщо (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		повернути SBI_ENOTSUPP;

	повернути SBI_OK;
}

static ціле fwft_enable_lpad(struct fwft_config *conf, unsigned long value)
{
	повернути fwft_menvcfg_set_bit(value, ENVCFG_LPE_SHIFT);
}

static ціле fwft_get_lpad(struct fwft_config *conf, unsigned long *value)
{
	повернути fwft_menvcfg_read_bit(value, ENVCFG_LPE_SHIFT);
}

static ціле fwft_sstack_supported(struct fwft_config *conf)
{
	якщо (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		повернути SBI_ENOTSUPP;

	повернути SBI_OK;
}

static ціле fwft_enable_sstack(struct fwft_config *conf, unsigned long value)
{
	повернути fwft_menvcfg_set_bit(value, ENVCFG_SSE_SHIFT);
}

static ціле fwft_get_sstack(struct fwft_config *conf, unsigned long *value)
{
	повернути fwft_menvcfg_read_bit(value, ENVCFG_SSE_SHIFT);
}

#якщо __riscv_xlen > 32
static ціле fwft_pmlen_supported(struct fwft_config *conf)
{
	якщо (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		повернути SBI_ENOTSUPP;

	повернути SBI_OK;
}

static ціле fwft_set_pmlen(struct fwft_config *conf, unsigned long value)
{
	unsigned long pmm, prev;

	switch (value) {
	case 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		break;
	case 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		break;
	case 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		break;
	default:
		повернути SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	якщо ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		повернути SBI_EINVAL;
	}

	повернути SBI_OK;
}

static ціле fwft_get_pmlen(struct fwft_config *conf, unsigned long *value)
{
	switch (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	case ENVCFG_PMM_PMLEN_0:
		*value = 0;
		break;
	case ENVCFG_PMM_PMLEN_7:
		*value = 7;
		break;
	case ENVCFG_PMM_PMLEN_16:
		*value = 16;
		break;
	default:
		повернути SBI_EFAIL;
	}

	повернути SBI_OK;
}
#endif

static struct fwft_config* get_feature_config(enum sbi_fwft_feature_t feature)
{
	ціле i;
	struct fwft_hart_state *fhs = fwft_thishart_state_ptr();

	якщо (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		повернути NULL;

	для (i = 0; i < fhs->config_count; i++){
		якщо (feature == fhs->configs[i].feature->id)
			повернути &fhs->configs[i];
	}

	повернути NULL;
}

static ціле fwft_get_feature(enum sbi_fwft_feature_t feature,
			    struct fwft_config **conf)
{
	ціле ret;
	struct fwft_config *tconf;

	tconf = get_feature_config(feature);
	якщо (!tconf) {
		якщо (fwft_is_defined_feature(feature))
			повернути SBI_ENOTSUPP;

		повернути SBI_EDENIED;
	}

	якщо (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		якщо (ret)
			повернути ret;
	}
	*conf = tconf;

	повернути SBI_SUCCESS;
}

static void fwft_clear_config_lock(enum sbi_fwft_feature_t feature)
{
	ціле ret;
	struct fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	якщо (ret)
		повернути;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

ціле sbi_fwft_set(enum sbi_fwft_feature_t feature, unsigned long value,
		 unsigned long flags)
{
	ціле ret;
	struct fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	якщо (ret)
		повернути ret;

	якщо ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		повернути SBI_EINVAL;

	якщо (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		повернути SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, value);
	якщо (ret)
		повернути ret;

	conf->flags = flags;

	повернути SBI_OK;
}

ціле sbi_fwft_get(enum sbi_fwft_feature_t feature, unsigned long *out_val)
{
	ціле ret;
	struct fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	якщо (ret)
		повернути ret;

	повернути conf->feature->get(conf, out_val);
}

static const struct fwft_feature features[] =
{
	{
		.id = SBI_FWFT_MISALIGNED_EXC_DELEG,
		.supported = fwft_misaligned_delegation_supported,
		.set = fwft_set_misaligned_delegation,
		.get = fwft_get_misaligned_delegation,
	},
	{
		.id = SBI_FWFT_LANDING_PAD,
		.supported = fwft_lpad_supported,
		.set = fwft_enable_lpad,
		.get = fwft_get_lpad,
	},
	{
		.id = SBI_FWFT_SHADOW_STACK,
		.supported = fwft_sstack_supported,
		.set = fwft_enable_sstack,
		.get = fwft_get_sstack,
	},
	{
		.id = SBI_FWFT_DOUBLE_TRAP,
		.supported = fwft_double_trap_supported,
		.set = fwft_set_double_trap,
		.get = fwft_get_double_trap,
	},
	{
		.id = SBI_FWFT_PTE_AD_HW_UPDATING,
		.supported = fwft_adue_supported,
		.set = fwft_set_adue,
		.get = fwft_get_adue,
	},
#якщо __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

ціле sbi_fwft_init(struct sbi_scratch *scratch, bool cold_boot)
{
	ціле i;
	struct fwft_hart_state *fhs;

	якщо (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		якщо (!fwft_ptr_offset)
			повернути SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	якщо (!fhs) {
		fhs = sbi_zalloc(sizeof(*fhs) + array_size(features) * sizeof(struct fwft_config));
		якщо (!fhs)
			повернути SBI_ENOMEM;

		fhs->config_count = array_size(features);
		для (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	для (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	повернути 0;
}
