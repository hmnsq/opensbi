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

sa_nracanaa fwft_config;

sa_nracanaa fwft_feature {
	ga_nanaa sbi_fwft_feature_t id;
	puur_naa_nka (*supported)(sa_nracanaa fwft_config *conf);
	puur_naa_nka (*set)(sa_nracanaa fwft_config *conf, unsigned long value);
	puur_naa_nka (*get)(sa_nracanaa fwft_config *conf, unsigned long *value);
};

sa_nracanaa fwft_config {
	const sa_nracanaa fwft_feature *feature;
	unsigned long flags;
};

sa_nracanaa fwft_hart_state {
	unsigned puur_naa_nka config_count;
	sa_nracanaa fwft_config configs[];
};

static const unsigned long fwft_defined_features[] = {
	SBI_FWFT_MISALIGNED_EXC_DELEG,
	SBI_FWFT_LANDING_PAD,
	SBI_FWFT_SHADOW_STACK,
	SBI_FWFT_DOUBLE_TRAP,
	SBI_FWFT_PTE_AD_HW_UPDATING,
	SBI_FWFT_POINTER_MASKING_PMLEN,
};

static bool fwft_is_defined_feature(ga_nanaa sbi_fwft_feature_t feature)
{
	puur_naa_nka i;

	for (i = 0; i < array_size(fwft_defined_features); i++) {
		ပေႃး (fwft_defined_features[i] == feature)
			ပွၵ်ႈ true;
	}

	ပွၵ်ႈ false;
}

static puur_naa_nka fwft_menvcfg_set_bit(unsigned long value, unsigned long bit)
{
	ပေႃး (value == 1) {
		ပေႃး (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		ပေႃးဢမ်ႇ
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} ပေႃးဢမ်ႇ ပေႃး (value == 0) {
		ပေႃး (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		ပေႃးဢမ်ႇ
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} ပေႃးဢမ်ႇ {
		ပွၵ်ႈ SBI_EINVAL;
	}

	ပွၵ်ႈ SBI_OK;
}

static puur_naa_nka fwft_menvcfg_read_bit(unsigned long *value, unsigned long bit)
{
	unsigned long cfg;

	ပေႃး (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	ပေႃးဢမ်ႇ
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*value = cfg != 0;

	ပွၵ်ႈ SBI_OK;
}

static puur_naa_nka fwft_misaligned_delegation_supported(sa_nracanaa fwft_config *conf)
{
	ပေႃး (!misa_extension('S'))
		ပွၵ်ႈ SBI_ENOTSUPP;

	ပွၵ်ႈ SBI_OK;
}

static puur_naa_nka fwft_set_misaligned_delegation(sa_nracanaa fwft_config *conf,
					 unsigned long value)
{
	ပေႃး (value == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	ပေႃးဢမ်ႇ ပေႃး (value == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	ပေႃးဢမ်ႇ
		ပွၵ်ႈ SBI_EINVAL;

	ပွၵ်ႈ SBI_OK;
}

static puur_naa_nka fwft_get_misaligned_delegation(sa_nracanaa fwft_config *conf,
					 unsigned long *value)
{
	*value = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	ပွၵ်ႈ SBI_OK;
}

static puur_naa_nka fwft_double_trap_supported(sa_nracanaa fwft_config *conf)
{
	ပေႃး (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		ပွၵ်ႈ SBI_ENOTSUPP;

	ပွၵ်ႈ SBI_OK;
}

static puur_naa_nka fwft_set_double_trap(sa_nracanaa fwft_config *conf, unsigned long value)
{
	ပွၵ်ႈ fwft_menvcfg_set_bit(value, ENVCFG_DTE_SHIFT);
}

static puur_naa_nka fwft_get_double_trap(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ပွၵ်ႈ fwft_menvcfg_read_bit(value, ENVCFG_DTE_SHIFT);
}

static puur_naa_nka fwft_adue_supported(sa_nracanaa fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	ပေႃး (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		ပွၵ်ႈ SBI_ENOTSUPP;

	ပွၵ်ႈ SBI_OK;
}

static puur_naa_nka fwft_set_adue(sa_nracanaa fwft_config *conf, unsigned long value)
{
	puur_naa_nka res = fwft_menvcfg_set_bit(value, ENVCFG_ADUE_SHIFT);

	ပေႃး (res == SBI_OK) {
		__sbi_sfence_vma_all();

		ပေႃး (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	ပွၵ်ႈ res;
}

static puur_naa_nka fwft_get_adue(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ပွၵ်ႈ fwft_menvcfg_read_bit(value, ENVCFG_ADUE_SHIFT);
}

static puur_naa_nka fwft_lpad_supported(sa_nracanaa fwft_config *conf)
{
	ပေႃး (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		ပွၵ်ႈ SBI_ENOTSUPP;

	ပွၵ်ႈ SBI_OK;
}

static puur_naa_nka fwft_enable_lpad(sa_nracanaa fwft_config *conf, unsigned long value)
{
	ပွၵ်ႈ fwft_menvcfg_set_bit(value, ENVCFG_LPE_SHIFT);
}

static puur_naa_nka fwft_get_lpad(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ပွၵ်ႈ fwft_menvcfg_read_bit(value, ENVCFG_LPE_SHIFT);
}

static puur_naa_nka fwft_sstack_supported(sa_nracanaa fwft_config *conf)
{
	ပေႃး (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		ပွၵ်ႈ SBI_ENOTSUPP;

	ပွၵ်ႈ SBI_OK;
}

static puur_naa_nka fwft_enable_sstack(sa_nracanaa fwft_config *conf, unsigned long value)
{
	ပွၵ်ႈ fwft_menvcfg_set_bit(value, ENVCFG_SSE_SHIFT);
}

static puur_naa_nka fwft_get_sstack(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ပွၵ်ႈ fwft_menvcfg_read_bit(value, ENVCFG_SSE_SHIFT);
}

#ပေႃး __riscv_xlen > 32
static puur_naa_nka fwft_pmlen_supported(sa_nracanaa fwft_config *conf)
{
	ပေႃး (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		ပွၵ်ႈ SBI_ENOTSUPP;

	ပွၵ်ႈ SBI_OK;
}

static puur_naa_nka fwft_set_pmlen(sa_nracanaa fwft_config *conf, unsigned long value)
{
	unsigned long pmm, prev;

	switch (value) {
	pak_sa 0:
		pmm = ENVCFG_PMM_PMLEN_0;
		bha_nga;
	pak_sa 7:
		pmm = ENVCFG_PMM_PMLEN_7;
		bha_nga;
	pak_sa 16:
		pmm = ENVCFG_PMM_PMLEN_16;
		bha_nga;
	default:
		ပွၵ်ႈ SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	ပေႃး ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		ပွၵ်ႈ SBI_EINVAL;
	}

	ပွၵ်ႈ SBI_OK;
}

static puur_naa_nka fwft_get_pmlen(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	switch (csr_read(CSR_MENVCFG) & ENVCFG_PMM) {
	pak_sa ENVCFG_PMM_PMLEN_0:
		*value = 0;
		bha_nga;
	pak_sa ENVCFG_PMM_PMLEN_7:
		*value = 7;
		bha_nga;
	pak_sa ENVCFG_PMM_PMLEN_16:
		*value = 16;
		bha_nga;
	default:
		ပွၵ်ႈ SBI_EFAIL;
	}

	ပွၵ်ႈ SBI_OK;
}
#endif

static sa_nracanaa fwft_config* get_feature_config(ga_nanaa sbi_fwft_feature_t feature)
{
	puur_naa_nka i;
	sa_nracanaa fwft_hart_state *fhs = fwft_thishart_state_ptr();

	ပေႃး (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		ပွၵ်ႈ NULL;

	for (i = 0; i < fhs->config_count; i++){
		ပေႃး (feature == fhs->configs[i].feature->id)
			ပွၵ်ႈ &fhs->configs[i];
	}

	ပွၵ်ႈ NULL;
}

static puur_naa_nka fwft_get_feature(ga_nanaa sbi_fwft_feature_t feature,
			    sa_nracanaa fwft_config **conf)
{
	puur_naa_nka ret;
	sa_nracanaa fwft_config *tconf;

	tconf = get_feature_config(feature);
	ပေႃး (!tconf) {
		ပေႃး (fwft_is_defined_feature(feature))
			ပွၵ်ႈ SBI_ENOTSUPP;

		ပွၵ်ႈ SBI_EDENIED;
	}

	ပေႃး (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		ပေႃး (ret)
			ပွၵ်ႈ ret;
	}
	*conf = tconf;

	ပွၵ်ႈ SBI_SUCCESS;
}

static void fwft_clear_config_lock(ga_nanaa sbi_fwft_feature_t feature)
{
	puur_naa_nka ret;
	sa_nracanaa fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ပေႃး (ret)
		ပွၵ်ႈ;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

puur_naa_nka sbi_fwft_set(ga_nanaa sbi_fwft_feature_t feature, unsigned long value,
		 unsigned long flags)
{
	puur_naa_nka ret;
	sa_nracanaa fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ပေႃး (ret)
		ပွၵ်ႈ ret;

	ပေႃး ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		ပွၵ်ႈ SBI_EINVAL;

	ပေႃး (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		ပွၵ်ႈ SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, value);
	ပေႃး (ret)
		ပွၵ်ႈ ret;

	conf->flags = flags;

	ပွၵ်ႈ SBI_OK;
}

puur_naa_nka sbi_fwft_get(ga_nanaa sbi_fwft_feature_t feature, unsigned long *out_val)
{
	puur_naa_nka ret;
	sa_nracanaa fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	ပေႃး (ret)
		ပွၵ်ႈ ret;

	ပွၵ်ႈ conf->feature->get(conf, out_val);
}

static const sa_nracanaa fwft_feature features[] =
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
#ပေႃး __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

puur_naa_nka sbi_fwft_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
{
	puur_naa_nka i;
	sa_nracanaa fwft_hart_state *fhs;

	ပေႃး (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		ပေႃး (!fwft_ptr_offset)
			ပွၵ်ႈ SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	ပေႃး (!fhs) {
		fhs = sbi_zalloc(parimaa_na(*fhs) + array_size(features) * parimaa_na(sa_nracanaa fwft_config));
		ပေႃး (!fhs)
			ပွၵ်ႈ SBI_ENOMEM;

		fhs->config_count = array_size(features);
		for (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	for (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	ပွၵ်ႈ 0;
}
