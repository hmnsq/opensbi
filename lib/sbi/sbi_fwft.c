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
	整数 (*supported)(struct fwft_config *conf);
	整数 (*set)(struct fwft_config *conf, unsigned long value);
	整数 (*get)(struct fwft_config *conf, unsigned long *value);
};

struct fwft_config {
	const struct fwft_feature *feature;
	unsigned long flags;
};

struct fwft_hart_state {
	unsigned 整数 config_count;
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
	整数 i;

	ために (i = 0; i < array_size(fwft_defined_features); i++) {
		もし (fwft_defined_features[i] == feature)
			戻る true;
	}

	戻る false;
}

static 整数 fwft_menvcfg_set_bit(unsigned long value, unsigned long bit)
{
	もし (value == 1) {
		もし (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		否则
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} 否则 もし (value == 0) {
		もし (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		否则
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} 否则 {
		戻る SBI_EINVAL;
	}

	戻る SBI_OK;
}

static 整数 fwft_menvcfg_read_bit(unsigned long *value, unsigned long bit)
{
	unsigned long cfg;

	もし (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	否则
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*value = cfg != 0;

	戻る SBI_OK;
}

static 整数 fwft_misaligned_delegation_supported(struct fwft_config *conf)
{
	もし (!misa_extension('S'))
		戻る SBI_ENOTSUPP;

	戻る SBI_OK;
}

static 整数 fwft_set_misaligned_delegation(struct fwft_config *conf,
					 unsigned long value)
{
	もし (value == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	否则 もし (value == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	否则
		戻る SBI_EINVAL;

	戻る SBI_OK;
}

static 整数 fwft_get_misaligned_delegation(struct fwft_config *conf,
					 unsigned long *value)
{
	*value = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	戻る SBI_OK;
}

static 整数 fwft_double_trap_supported(struct fwft_config *conf)
{
	もし (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		戻る SBI_ENOTSUPP;

	戻る SBI_OK;
}

static 整数 fwft_set_double_trap(struct fwft_config *conf, unsigned long value)
{
	戻る fwft_menvcfg_set_bit(value, ENVCFG_DTE_SHIFT);
}

static 整数 fwft_get_double_trap(struct fwft_config *conf, unsigned long *value)
{
	戻る fwft_menvcfg_read_bit(value, ENVCFG_DTE_SHIFT);
}

static 整数 fwft_adue_supported(struct fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	もし (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		戻る SBI_ENOTSUPP;

	戻る SBI_OK;
}

static 整数 fwft_set_adue(struct fwft_config *conf, unsigned long value)
{
	整数 res = fwft_menvcfg_set_bit(value, ENVCFG_ADUE_SHIFT);

	もし (res == SBI_OK) {
		__sbi_sfence_vma_all();

		もし (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	戻る res;
}

static 整数 fwft_get_adue(struct fwft_config *conf, unsigned long *value)
{
	戻る fwft_menvcfg_read_bit(value, ENVCFG_ADUE_SHIFT);
}

static 整数 fwft_lpad_supported(struct fwft_config *conf)
{
	もし (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		戻る SBI_ENOTSUPP;

	戻る SBI_OK;
}

static 整数 fwft_enable_lpad(struct fwft_config *conf, unsigned long value)
{
	戻る fwft_menvcfg_set_bit(value, ENVCFG_LPE_SHIFT);
}

static 整数 fwft_get_lpad(struct fwft_config *conf, unsigned long *value)
{
	戻る fwft_menvcfg_read_bit(value, ENVCFG_LPE_SHIFT);
}

static 整数 fwft_sstack_supported(struct fwft_config *conf)
{
	もし (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		戻る SBI_ENOTSUPP;

	戻る SBI_OK;
}

static 整数 fwft_enable_sstack(struct fwft_config *conf, unsigned long value)
{
	戻る fwft_menvcfg_set_bit(value, ENVCFG_SSE_SHIFT);
}

static 整数 fwft_get_sstack(struct fwft_config *conf, unsigned long *value)
{
	戻る fwft_menvcfg_read_bit(value, ENVCFG_SSE_SHIFT);
}

#もし __riscv_xlen > 32
static 整数 fwft_pmlen_supported(struct fwft_config *conf)
{
	もし (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		戻る SBI_ENOTSUPP;

	戻る SBI_OK;
}

static 整数 fwft_set_pmlen(struct fwft_config *conf, unsigned long value)
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
		戻る SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	もし ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		戻る SBI_EINVAL;
	}

	戻る SBI_OK;
}

static 整数 fwft_get_pmlen(struct fwft_config *conf, unsigned long *value)
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
		戻る SBI_EFAIL;
	}

	戻る SBI_OK;
}
#endif

static struct fwft_config* get_feature_config(enum sbi_fwft_feature_t feature)
{
	整数 i;
	struct fwft_hart_state *fhs = fwft_thishart_state_ptr();

	もし (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		戻る NULL;

	ために (i = 0; i < fhs->config_count; i++){
		もし (feature == fhs->configs[i].feature->id)
			戻る &fhs->configs[i];
	}

	戻る NULL;
}

static 整数 fwft_get_feature(enum sbi_fwft_feature_t feature,
			    struct fwft_config **conf)
{
	整数 ret;
	struct fwft_config *tconf;

	tconf = get_feature_config(feature);
	もし (!tconf) {
		もし (fwft_is_defined_feature(feature))
			戻る SBI_ENOTSUPP;

		戻る SBI_EDENIED;
	}

	もし (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		もし (ret)
			戻る ret;
	}
	*conf = tconf;

	戻る SBI_SUCCESS;
}

static void fwft_clear_config_lock(enum sbi_fwft_feature_t feature)
{
	整数 ret;
	struct fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	もし (ret)
		戻る;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

整数 sbi_fwft_set(enum sbi_fwft_feature_t feature, unsigned long value,
		 unsigned long flags)
{
	整数 ret;
	struct fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	もし (ret)
		戻る ret;

	もし ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		戻る SBI_EINVAL;

	もし (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		戻る SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, value);
	もし (ret)
		戻る ret;

	conf->flags = flags;

	戻る SBI_OK;
}

整数 sbi_fwft_get(enum sbi_fwft_feature_t feature, unsigned long *out_val)
{
	整数 ret;
	struct fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	もし (ret)
		戻る ret;

	戻る conf->feature->get(conf, out_val);
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
#もし __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

整数 sbi_fwft_init(struct sbi_scratch *scratch, bool cold_boot)
{
	整数 i;
	struct fwft_hart_state *fhs;

	もし (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		もし (!fwft_ptr_offset)
			戻る SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	もし (!fhs) {
		fhs = sbi_zalloc(sizeof(*fhs) + array_size(features) * sizeof(struct fwft_config));
		もし (!fhs)
			戻る SBI_ENOMEM;

		fhs->config_count = array_size(features);
		ために (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	ために (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	戻る 0;
}
