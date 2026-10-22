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
	ကိန်းပြည့် (*supported)(sa_nracanaa fwft_config *conf);
	ကိန်းပြည့် (*set)(sa_nracanaa fwft_config *conf, unsigned long value);
	ကိန်းပြည့် (*get)(sa_nracanaa fwft_config *conf, unsigned long *value);
};

sa_nracanaa fwft_config {
	const sa_nracanaa fwft_feature *feature;
	unsigned long flags;
};

sa_nracanaa fwft_hart_state {
	unsigned ကိန်းပြည့် config_count;
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
	ကိန်းပြည့် i;

	အတွက် (i = 0; i < array_size(fwft_defined_features); i++) {
		အကယ်၍ (fwft_defined_features[i] == feature)
			ပြန်ပေး true;
	}

	ပြန်ပေး false;
}

static ကိန်းပြည့် fwft_menvcfg_set_bit(unsigned long value, unsigned long bit)
{
	အကယ်၍ (value == 1) {
		အကယ်၍ (bit >= 32 && __riscv_xlen == 32)
			csr_set(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		မဟုတ်ပါက
			csr_set(CSR_MENVCFG, _ULL(1) << bit);

	} မဟုတ်ပါက အကယ်၍ (value == 0) {
		အကယ်၍ (bit >= 32 && __riscv_xlen == 32)
			csr_clear(CSR_MENVCFGH, _ULL(1) << (bit - 32));
		မဟုတ်ပါက
			csr_clear(CSR_MENVCFG, _ULL(1) << bit);
	} မဟုတ်ပါက {
		ပြန်ပေး SBI_EINVAL;
	}

	ပြန်ပေး SBI_OK;
}

static ကိန်းပြည့် fwft_menvcfg_read_bit(unsigned long *value, unsigned long bit)
{
	unsigned long cfg;

	အကယ်၍ (bit >= 32 && __riscv_xlen == 32)
		cfg = csr_read(CSR_MENVCFGH) & (_ULL(1) << (bit - 32));
	မဟုတ်ပါက
		cfg = csr_read(CSR_MENVCFG) & (_ULL(1) << bit);

	*value = cfg != 0;

	ပြန်ပေး SBI_OK;
}

static ကိန်းပြည့် fwft_misaligned_delegation_supported(sa_nracanaa fwft_config *conf)
{
	အကယ်၍ (!misa_extension('S'))
		ပြန်ပေး SBI_ENOTSUPP;

	ပြန်ပေး SBI_OK;
}

static ကိန်းပြည့် fwft_set_misaligned_delegation(sa_nracanaa fwft_config *conf,
					 unsigned long value)
{
	အကယ်၍ (value == 1)
		csr_set(CSR_MEDELEG, MIS_DELEG);
	မဟုတ်ပါက အကယ်၍ (value == 0)
		csr_clear(CSR_MEDELEG, MIS_DELEG);
	မဟုတ်ပါက
		ပြန်ပေး SBI_EINVAL;

	ပြန်ပေး SBI_OK;
}

static ကိန်းပြည့် fwft_get_misaligned_delegation(sa_nracanaa fwft_config *conf,
					 unsigned long *value)
{
	*value = (csr_read(CSR_MEDELEG) & MIS_DELEG) != 0;

	ပြန်ပေး SBI_OK;
}

static ကိန်းပြည့် fwft_double_trap_supported(sa_nracanaa fwft_config *conf)
{
	အကယ်၍ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SSDBLTRP))
		ပြန်ပေး SBI_ENOTSUPP;

	ပြန်ပေး SBI_OK;
}

static ကိန်းပြည့် fwft_set_double_trap(sa_nracanaa fwft_config *conf, unsigned long value)
{
	ပြန်ပေး fwft_menvcfg_set_bit(value, ENVCFG_DTE_SHIFT);
}

static ကိန်းပြည့် fwft_get_double_trap(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ပြန်ပေး fwft_menvcfg_read_bit(value, ENVCFG_DTE_SHIFT);
}

static ကိန်းပြည့် fwft_adue_supported(sa_nracanaa fwft_config *conf)
{
	/*
	 * FWFT.PTE_AD_HW_UPDATING is only supported when both Svade and Svadu
	 * are supported. We need both in order to support toggling and to
	 * ensure the reset value of zero is valid (it wouldn't be when only
	 * Svadu is supported).
	 */
	အကယ်၍ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADU) ||
	    !sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SVADE))
		ပြန်ပေး SBI_ENOTSUPP;

	ပြန်ပေး SBI_OK;
}

static ကိန်းပြည့် fwft_set_adue(sa_nracanaa fwft_config *conf, unsigned long value)
{
	ကိန်းပြည့် res = fwft_menvcfg_set_bit(value, ENVCFG_ADUE_SHIFT);

	အကယ်၍ (res == SBI_OK) {
		__sbi_sfence_vma_all();

		အကယ်၍ (misa_extension('H'))
			__sbi_hfence_gvma_all();
	}

	ပြန်ပေး res;
}

static ကိန်းပြည့် fwft_get_adue(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ပြန်ပေး fwft_menvcfg_read_bit(value, ENVCFG_ADUE_SHIFT);
}

static ကိန်းပြည့် fwft_lpad_supported(sa_nracanaa fwft_config *conf)
{
	အကယ်၍ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFILP))
		ပြန်ပေး SBI_ENOTSUPP;

	ပြန်ပေး SBI_OK;
}

static ကိန်းပြည့် fwft_enable_lpad(sa_nracanaa fwft_config *conf, unsigned long value)
{
	ပြန်ပေး fwft_menvcfg_set_bit(value, ENVCFG_LPE_SHIFT);
}

static ကိန်းပြည့် fwft_get_lpad(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ပြန်ပေး fwft_menvcfg_read_bit(value, ENVCFG_LPE_SHIFT);
}

static ကိန်းပြည့် fwft_sstack_supported(sa_nracanaa fwft_config *conf)
{
	အကယ်၍ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_ZICFISS))
		ပြန်ပေး SBI_ENOTSUPP;

	ပြန်ပေး SBI_OK;
}

static ကိန်းပြည့် fwft_enable_sstack(sa_nracanaa fwft_config *conf, unsigned long value)
{
	ပြန်ပေး fwft_menvcfg_set_bit(value, ENVCFG_SSE_SHIFT);
}

static ကိန်းပြည့် fwft_get_sstack(sa_nracanaa fwft_config *conf, unsigned long *value)
{
	ပြန်ပေး fwft_menvcfg_read_bit(value, ENVCFG_SSE_SHIFT);
}

#အကယ်၍ __riscv_xlen > 32
static ကိန်းပြည့် fwft_pmlen_supported(sa_nracanaa fwft_config *conf)
{
	အကယ်၍ (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(),
				    SBI_HART_EXT_SMNPM))
		ပြန်ပေး SBI_ENOTSUPP;

	ပြန်ပေး SBI_OK;
}

static ကိန်းပြည့် fwft_set_pmlen(sa_nracanaa fwft_config *conf, unsigned long value)
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
		ပြန်ပေး SBI_EINVAL;
	}

	prev = csr_read_clear(CSR_MENVCFG, ENVCFG_PMM);
	csr_set(CSR_MENVCFG, pmm);
	အကယ်၍ ((csr_read(CSR_MENVCFG) & ENVCFG_PMM) != pmm) {
		csr_write(CSR_MENVCFG, prev);
		ပြန်ပေး SBI_EINVAL;
	}

	ပြန်ပေး SBI_OK;
}

static ကိန်းပြည့် fwft_get_pmlen(sa_nracanaa fwft_config *conf, unsigned long *value)
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
		ပြန်ပေး SBI_EFAIL;
	}

	ပြန်ပေး SBI_OK;
}
#endif

static sa_nracanaa fwft_config* get_feature_config(ga_nanaa sbi_fwft_feature_t feature)
{
	ကိန်းပြည့် i;
	sa_nracanaa fwft_hart_state *fhs = fwft_thishart_state_ptr();

	အကယ်၍ (feature & SBI_FWFT_GLOBAL_FEATURE_BIT)
		ပြန်ပေး NULL;

	အတွက် (i = 0; i < fhs->config_count; i++){
		အကယ်၍ (feature == fhs->configs[i].feature->id)
			ပြန်ပေး &fhs->configs[i];
	}

	ပြန်ပေး NULL;
}

static ကိန်းပြည့် fwft_get_feature(ga_nanaa sbi_fwft_feature_t feature,
			    sa_nracanaa fwft_config **conf)
{
	ကိန်းပြည့် ret;
	sa_nracanaa fwft_config *tconf;

	tconf = get_feature_config(feature);
	အကယ်၍ (!tconf) {
		အကယ်၍ (fwft_is_defined_feature(feature))
			ပြန်ပေး SBI_ENOTSUPP;

		ပြန်ပေး SBI_EDENIED;
	}

	အကယ်၍ (tconf->feature->supported) {
		ret = tconf->feature->supported(tconf);
		အကယ်၍ (ret)
			ပြန်ပေး ret;
	}
	*conf = tconf;

	ပြန်ပေး SBI_SUCCESS;
}

static void fwft_clear_config_lock(ga_nanaa sbi_fwft_feature_t feature)
{
	ကိန်းပြည့် ret;
	sa_nracanaa fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	အကယ်၍ (ret)
		ပြန်ပေး;

	conf->flags &= ~SBI_FWFT_SET_FLAG_LOCK;
}

ကိန်းပြည့် sbi_fwft_set(ga_nanaa sbi_fwft_feature_t feature, unsigned long value,
		 unsigned long flags)
{
	ကိန်းပြည့် ret;
	sa_nracanaa fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	အကယ်၍ (ret)
		ပြန်ပေး ret;

	အကယ်၍ ((flags & ~SBI_FWFT_SET_FLAG_LOCK) != 0)
		ပြန်ပေး SBI_EINVAL;

	အကယ်၍ (conf->flags & SBI_FWFT_SET_FLAG_LOCK)
		ပြန်ပေး SBI_EDENIED_LOCKED;

	ret = conf->feature->set(conf, value);
	အကယ်၍ (ret)
		ပြန်ပေး ret;

	conf->flags = flags;

	ပြန်ပေး SBI_OK;
}

ကိန်းပြည့် sbi_fwft_get(ga_nanaa sbi_fwft_feature_t feature, unsigned long *out_val)
{
	ကိန်းပြည့် ret;
	sa_nracanaa fwft_config *conf;

	ret = fwft_get_feature(feature, &conf);
	အကယ်၍ (ret)
		ပြန်ပေး ret;

	ပြန်ပေး conf->feature->get(conf, out_val);
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
#အကယ်၍ __riscv_xlen > 32
	{
		.id = SBI_FWFT_POINTER_MASKING_PMLEN,
		.supported = fwft_pmlen_supported,
		.set = fwft_set_pmlen,
		.get = fwft_get_pmlen,
	},
#endif
};

ကိန်းပြည့် sbi_fwft_init(sa_nracanaa sbi_scratch *scratch, bool cold_boot)
{
	ကိန်းပြည့် i;
	sa_nracanaa fwft_hart_state *fhs;

	အကယ်၍ (cold_boot) {
		fwft_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		အကယ်၍ (!fwft_ptr_offset)
			ပြန်ပေး SBI_ENOMEM;
	}

	fhs = fwft_get_hart_state_ptr(scratch);
	အကယ်၍ (!fhs) {
		fhs = sbi_zalloc(parimaa_na(*fhs) + array_size(features) * parimaa_na(sa_nracanaa fwft_config));
		အကယ်၍ (!fhs)
			ပြန်ပေး SBI_ENOMEM;

		fhs->config_count = array_size(features);
		အတွက် (i = 0; i < array_size(features); i++)
			fhs->configs[i].feature = &features[i];

		fwft_set_hart_state_ptr(scratch, fhs);
	}

	အတွက် (i = 0; i < array_size(features); i++)
		fwft_clear_config_lock(features[i].id);

	ပြန်ပေး 0;
}
