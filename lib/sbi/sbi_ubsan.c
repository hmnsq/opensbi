/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#جے_تعریف_شدہ UBSAN_ENABLED

#ضمن <sbi/sbi_types.h>
#ضمن <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#تعریف_کرو قسم_INTEGER 0
#تعریف_کرو قسم_FLOAT 1
#تعریف_کرو قسم_UNKNOWN UINT16_MAX

متطاير بنانشانی لما sbi_ubsan_report_گنتی;

بنية CSourceLocation {
	حرف *mFilename;
	uint32_t m_سطر;
	uint32_t m_کالم;
};

بنية CTypeDescriptor {
	uint16_t m_Type_قسم;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

بنية COverflow_انکڑے {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CUnreachable_انکڑے {
	بنية CSourceLocation mLocation;
};

بنية CCFICheck_Fail_انکڑے {
	uint8_t m_Check_قسم;
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CDynamic_Type_Cache_کھنج_انکڑے {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
	فراغ *mTypeInfo;
	uint8_t m_Type_Check_قسم;
};

بنية CFunction_Type_Mismatch_انکڑے {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CInvalid_Builtin_انکڑے {
	بنية CSourceLocation mLocation;
	uint8_t m_قسم;
};

بنية CInvalid_قدر_انکڑے {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CNon_Null_Arg_انکڑے {
	بنية CSourceLocation mLocation;
	بنية CSourceLocation mAttributeLocation;
	صحيح m_Arg_اشاریہ;
};

بنية CNon_Null_Return_انکڑے {
	بنية CSourceLocation mAttributeLocation;
};

بنية COut_دا_Bounds_انکڑے {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mArrayType;
	بنية CTypeDescriptor *m_اشاریہ_Type;
};

بنية CPointer_حد_توں_ودھ_انکڑے {
	بنية CSourceLocation mLocation;
};

بنية CShift_باہرلا_دا_Bounds_انکڑے {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mLHSType;
	بنية CTypeDescriptor *mRHSType;
};

بنية CType_Mismatch_انکڑے {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
	بنانشانی لما mLogAlignment;
	uint8_t m_Type_Check_قسم;
};

بنية CType_Mismatch_انکڑے_v1 {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t m_Type_Check_قسم;
};

بنية CVLABound_انکڑے {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CFloat_Cast_حد_توں_ودھ_انکڑے {
	بنية CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	بنية CTypeDescriptor *mFromType;
	بنية CTypeDescriptor *m_منزل_Type;
};

بنية CImplicit_Conversion_انکڑے {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mFromType;
	بنية CTypeDescriptor *m_منزل_Type;
	uint8_t m_قسم;
};

بنية CAlignment_Assumption_انکڑے {
	بنية CSourceLocation mLocation;
	بنية CSourceLocation mAssumptionLocation;
	بنية CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
فراغ __ubsan_handle_add_overflow(بنية COverflow_انکڑے *p_انکڑے,
				 بنانشانی لما ulLHS, بنانشانی لما ulRHS);
فراغ __ubsan_handle_add_overflow_abort(بنية COverflow_انکڑے *p_انکڑے,
				       بنانشانی لما ulLHS,
				       بنانشانی لما ulRHS);
فراغ __ubsan_handle_alignment_assumption(بنية CAlignment_Assumption_انکڑے *p_انکڑے,
					 بنانشانی لما ul_اشارہ_گر,
					 بنانشانی لما ulAlignment,
					 بنانشانی لما ul_وتھ);
فراغ __ubsan_handle_alignment_assumption_abort(
	بنية CAlignment_Assumption_انکڑے *p_انکڑے, بنانشانی لما ul_اشارہ_گر,
	بنانشانی لما ulAlignment, بنانشانی لما ul_وتھ);
فراغ __ubsan_handle_builtin_unreachable(بنية CUnreachable_انکڑے *p_انکڑے);
فراغ __ubsan_handle_divrem_overflow(بنية COverflow_انکڑے *p_انکڑے,
				    بنانشانی لما ulLHS, بنانشانی لما ulRHS);
فراغ __ubsan_handle_divrem_overflow_abort(بنية COverflow_انکڑے *p_انکڑے,
					  بنانشانی لما ulLHS,
					  بنانشانی لما ulRHS);
فراغ __ubsan_handle_function_type_mismatch(
	بنية CFunction_Type_Mismatch_انکڑے *p_انکڑے, بنانشانی لما ulFunction);
فراغ __ubsan_handle_function_type_mismatch_abort(
	بنية CFunction_Type_Mismatch_انکڑے *p_انکڑے, بنانشانی لما ulFunction);
فراغ __ubsan_handle_function_type_mismatch_v1(
	بنية CFunction_Type_Mismatch_انکڑے *p_انکڑے, بنانشانی لما ulFunction,
	بنانشانی لما ulCalleeRTTI, بنانشانی لما ulFnRTTI);
فراغ __ubsan_handle_function_type_mismatch_v1_abort(
	بنية CFunction_Type_Mismatch_انکڑے *p_انکڑے, بنانشانی لما ulFunction,
	بنانشانی لما ulCalleeRTTI, بنانشانی لما ulFnRTTI);
فراغ __ubsan_handle_invalid_builtin(بنية CInvalid_Builtin_انکڑے *p_انکڑے);
فراغ __ubsan_handle_invalid_builtin_abort(بنية CInvalid_Builtin_انکڑے *p_انکڑے);
فراغ __ubsan_handle_load_invalid_value(بنية CInvalid_قدر_انکڑے *p_انکڑے,
				       بنانشانی لما ulVal);
فراغ __ubsan_handle_load_invalid_value_abort(بنية CInvalid_قدر_انکڑے *p_انکڑے,
					     بنانشانی لما ulVal);
فراغ __ubsan_handle_missing_return(بنية CUnreachable_انکڑے *p_انکڑے);
فراغ __ubsan_handle_mul_overflow(بنية COverflow_انکڑے *p_انکڑے,
				 بنانشانی لما ulLHS, بنانشانی لما ulRHS);
فراغ __ubsan_handle_mul_overflow_abort(بنية COverflow_انکڑے *p_انکڑے,
				       بنانشانی لما ulLHS,
				       بنانشانی لما ulRHS);
فراغ __ubsan_handle_negate_overflow(بنية COverflow_انکڑے *p_انکڑے,
				    بنانشانی لما ulOldVal);
فراغ __ubsan_handle_negate_overflow_abort(بنية COverflow_انکڑے *p_انکڑے,
					  بنانشانی لما ulOldVal);
فراغ __ubsan_handle_nullability_arg(بنية CNon_Null_Arg_انکڑے *p_انکڑے);
فراغ __ubsan_handle_nullability_arg_abort(بنية CNon_Null_Arg_انکڑے *p_انکڑے);
فراغ __ubsan_handle_nullability_return_v1(
	بنية CNon_Null_Return_انکڑے *p_انکڑے,
	بنية CSourceLocation *p_Location_اشارہ_گر);
فراغ __ubsan_handle_nullability_return_v1_abort(
	بنية CNon_Null_Return_انکڑے *p_انکڑے,
	بنية CSourceLocation *p_Location_اشارہ_گر);
فراغ __ubsan_handle_out_of_bounds(بنية COut_دا_Bounds_انکڑے *p_انکڑے,
				  بنانشانی لما ul_اشاریہ);
فراغ __ubsan_handle_out_of_bounds_abort(بنية COut_دا_Bounds_انکڑے *p_انکڑے,
					بنانشانی لما ul_اشاریہ);
فراغ __ubsan_handle_pointer_overflow(بنية CPointer_حد_توں_ودھ_انکڑے *p_انکڑے,
				     بنانشانی لما ul_ادھار,
				     بنانشانی لما ul_نتیجہ);
فراغ __ubsan_handle_pointer_overflow_abort(بنية CPointer_حد_توں_ودھ_انکڑے *p_انکڑے,
					   بنانشانی لما ul_ادھار,
					   بنانشانی لما ul_نتیجہ);
فراغ __ubsan_handle_shift_out_of_bounds(بنية CShift_باہرلا_دا_Bounds_انکڑے *p_انکڑے,
					بنانشانی لما ulLHS,
					بنانشانی لما ulRHS);
فراغ __ubsan_handle_shift_out_of_bounds_abort(
	بنية CShift_باہرلا_دا_Bounds_انکڑے *p_انکڑے, بنانشانی لما ulLHS,
	بنانشانی لما ulRHS);
فراغ __ubsan_handle_sub_overflow(بنية COverflow_انکڑے *p_انکڑے,
				 بنانشانی لما ulLHS, بنانشانی لما ulRHS);
فراغ __ubsan_handle_sub_overflow_abort(بنية COverflow_انکڑے *p_انکڑے,
				       بنانشانی لما ulLHS,
				       بنانشانی لما ulRHS);
فراغ __ubsan_handle_type_mismatch(بنية CType_Mismatch_انکڑے *p_انکڑے,
				  بنانشانی لما ul_اشارہ_گر);
فراغ __ubsan_handle_type_mismatch_abort(بنية CType_Mismatch_انکڑے *p_انکڑے,
					بنانشانی لما ul_اشارہ_گر);
فراغ __ubsan_handle_type_mismatch_v1(بنية CType_Mismatch_انکڑے_v1 *p_انکڑے,
				     بنانشانی لما ul_اشارہ_گر);
فراغ __ubsan_handle_type_mismatch_v1_abort(بنية CType_Mismatch_انکڑے_v1 *p_انکڑے,
					   بنانشانی لما ul_اشارہ_گر);
فراغ __ubsan_handle_vla_bound_not_positive(بنية CVLABound_انکڑے *p_انکڑے,
					   بنانشانی لما ulBound);
فراغ __ubsan_handle_vla_bound_not_positive_abort(بنية CVLABound_انکڑے *p_انکڑے,
						 بنانشانی لما ulBound);
فراغ __ubsan_get_current_report_data(ثابت حرف **pp_باہرلا_Issue_قسم,
				     ثابت حرف **pp_باہرلا_سنیہا,
				     ثابت حرف **pp_باہرلا_Filename,
				     uint32_t *p_باہرلا_سطر, uint32_t *p_باہرلا_خانہ,
				     حرف **pp_باہرلا_حافظہ_Addr);
ساكن فراغ Handle_حد_توں_ودھ(bool isFatal, بنية COverflow_انکڑے *p_انکڑے,
			   بنانشانی لما ulLHS, بنانشانی لما ulRHS,
			   ثابت حرف *szOperation);
ساكن فراغ Handle_Negate_حد_توں_ودھ(bool isFatal, بنية COverflow_انکڑے *p_انکڑے,
				 بنانشانی لما ul_Old_قدر);
ساكن فراغ HandleBuiltinUnreachable(bool isFatal,
				     بنية CUnreachable_انکڑے *p_انکڑے);
ساكن فراغ HandleTypeMismatch(bool isFatal, بنية CSourceLocation *mLocation,
			       بنية CTypeDescriptor *mType,
			       بنانشانی لما mLogAlignment,
			       uint8_t m_Type_Check_قسم, بنانشانی لما ul_اشارہ_گر);
ساكن فراغ Handle_Vla_Bound_Not_مثبت(bool isFatal, بنية CVLABound_انکڑے *p_انکڑے,
				      بنانشانی لما ulBound);
ساكن فراغ Handle_باہرلا_دا_Bounds(bool isFatal, بنية COut_دا_Bounds_انکڑے *p_انکڑے,
			      بنانشانی لما ul_اشاریہ);
ساكن فراغ Handle_کھسکاؤ_باہرلا_دا_Bounds(bool isFatal,
				   بنية CShift_باہرلا_دا_Bounds_انکڑے *p_انکڑے,
				   بنانشانی لما ulLHS, بنانشانی لما ulRHS);
ساكن فراغ Handle_لدو_Invalid_قدر(bool isFatal,
				   بنية CInvalid_قدر_انکڑے *p_انکڑے,
				   بنانشانی لما ul_قدر);
ساكن فراغ HandleInvalidBuiltin(bool isFatal,
				 بنية CInvalid_Builtin_انکڑے *p_انکڑے);
ساكن فراغ HandleFunctionTypeMismatch(bool isFatal,
				       بنية CFunction_Type_Mismatch_انکڑے *p_انکڑے,
				       بنانشانی لما ulFunction);
ساكن فراغ HandleMissingReturn(bool isFatal, بنية CUnreachable_انکڑے *p_انکڑے);
ساكن فراغ Handle_اشارہ_گر_حد_توں_ودھ(bool isFatal,
				  بنية CPointer_حد_توں_ودھ_انکڑے *p_انکڑے,
				  بنانشانی لما ul_ادھار, بنانشانی لما ul_نتیجہ);
ساكن فراغ HandleAlignmentAssumption(bool isFatal,
				      بنية CAlignment_Assumption_انکڑے *p_انکڑے,
				      بنانشانی لما ul_اشارہ_گر,
				      بنانشانی لما ulAlignment,
				      بنانشانی لما ul_وتھ);

#تعریف_کرو نمبر_SIGNED_ثنائی_ہندسہ 1
#تعریف_کرو نمبر_MAXLEN 128
#تعریف_کرو __arraycount(__a) (حجم(__a) / حجم(__a[0]))
#تعریف_کرو __BIT(__n) (1UL << (__n))
#تعریف_کرو SEPARATOR sbi_printf("===========================================\n")
#تعریف_کرو ACK_REPORTED (1U << 31)

ساكن bool isAlreadyReported(بنية CSourceLocation *pLocation)
{
	uint32_t si_Old_قدر;
	متطاير uint32_t *p_سطر;

	إذا (!pLocation)
		أرجع false;

	p_سطر = &pLocation->m_سطر;

	افعل {
		si_Old_قدر = *p_سطر;
	} طالما (__sync_val_compare_and_swap(p_سطر, si_Old_قدر,
					     si_Old_قدر | ACK_REPORTED) !=
		 si_Old_قدر);

	إذا (!(si_Old_قدر & ACK_REPORTED)) {

		sbi_ubsan_report_گنتی++;

		أرجع false;
	}

	أرجع true;
}

ساكن فراغ Handle_حد_توں_ودھ(bool isFatal, بنية COverflow_انکڑے *p_انکڑے,
			   بنانشانی لما ulLHS, بنانشانی لما ulRHS,
			   ثابت حرف *szOperation)
{
	إذا (!p_انکڑے) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_انکڑے->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_انکڑے->mLocation.mFilename,
		   p_انکڑے->mLocation.m_سطر & ~ACK_REPORTED,
		   p_انکڑے->mLocation.m_کالم);

	bool is_signed = p_انکڑے->mType->mTypeInfo & نمبر_SIGNED_ثنائی_ہندسہ;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	إذا (is_signed) {
		sbi_printf("%ld %s %ld ", (لما)ulLHS, szOperation,
			   (لما)ulRHS);
	} وإلا {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_انکڑے->mType->mTypeName);

	SEPARATOR;
}

ساكن فراغ Handle_Negate_حد_توں_ودھ(bool isFatal, بنية COverflow_انکڑے *p_انکڑے,
				 بنانشانی لما ul_Old_قدر)
{
	إذا (!p_انکڑے) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_انکڑے->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_انکڑے->mLocation.mFilename,
		   p_انکڑے->mLocation.m_سطر & ~ACK_REPORTED,
		   p_انکڑے->mLocation.m_کالم);

	bool is_signed = p_انکڑے->mType->mTypeInfo & نمبر_SIGNED_ثنائی_ہندسہ;

	sbi_printf("UBSan: Negation of ");

	إذا (is_signed) {
		sbi_printf("%ld", (لما)ul_Old_قدر);
	} وإلا {
		sbi_printf("%lu", ul_Old_قدر);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_انکڑے->mType->mTypeName);

	SEPARATOR;
}

ساكن فراغ HandleBuiltinUnreachable(bool isFatal,
				     بنية CUnreachable_انکڑے *p_انکڑے)
{
	إذا (!p_انکڑے) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_انکڑے->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_انکڑے->mLocation.mFilename,
		   p_انکڑے->mLocation.m_سطر & ~ACK_REPORTED,
		   p_انکڑے->mLocation.m_کالم);

	SEPARATOR;
}

ثابت حرف *rgczTypeCheckKinds[] = { "load of",
				     "store to",
				     "reference binding to",
				     "member access within",
				     "member call on",
				     "constructor call on",
				     "downcast of",
				     "downcast of",
				     "upcast of",
				     "cast to virtual base of",
				     "_Nonnull binding to",
				     "dynamic operation on" };

ساكن فراغ HandleTypeMismatch(bool isFatal, بنية CSourceLocation *mLocation,
			       بنية CTypeDescriptor *mType,
			       بنانشانی لما mLogAlignment,
			       uint8_t m_Type_Check_قسم, بنانشانی لما ul_اشارہ_گر)
{

	إذا ((!mLocation) || (!mType)) {
		أرجع;
	}

	إذا (isAlreadyReported(mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_سطر & ~ACK_REPORTED,
		   mLocation->m_کالم);

	ثابت حرف *قسم = (m_Type_Check_قسم < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[m_Type_Check_قسم]
				   : "access to";

	إذا (ul_اشارہ_گر == 0) {
		sbi_printf("%s null pointer of type %s\n", قسم,
			   mType->mTypeName);
	} وإلا إذا (
		(mLogAlignment - 1) &
		ul_اشارہ_گر) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			قسم, (فراغ *)ul_اشارہ_گر, mType->mTypeName,
			mLogAlignment);
	} وإلا {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			قسم, (فراغ *)ul_اشارہ_گر, mType->mTypeName);
	}
	SEPARATOR;
}

ساكن فراغ Handle_Vla_Bound_Not_مثبت(bool isFatal, بنية CVLABound_انکڑے *p_انکڑے,
				      بنانشانی لما ulBound)
{
	إذا (!p_انکڑے) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_انکڑے->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_انکڑے->mLocation.mFilename,
		   p_انکڑے->mLocation.m_سطر & ~ACK_REPORTED,
		   p_انکڑے->mLocation.m_کالم);

	sbi_printf("variable length array bound value ");

	bool is_signed = p_انکڑے->mType->mTypeInfo & نمبر_SIGNED_ثنائی_ہندسہ;

	إذا (is_signed) {
		sbi_printf("%ld", (لما)ulBound);
	} وإلا {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

ساكن فراغ Handle_باہرلا_دا_Bounds(bool isFatal, بنية COut_دا_Bounds_انکڑے *p_انکڑے,
			      بنانشانی لما ul_اشاریہ)
{
	إذا (!p_انکڑے) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_انکڑے->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_انکڑے->mLocation.mFilename,
		   p_انکڑے->mLocation.m_سطر & ~ACK_REPORTED,
		   p_انکڑے->mLocation.m_کالم);

	bool is_signed = p_انکڑے->m_اشاریہ_Type->mTypeInfo & نمبر_SIGNED_ثنائی_ہندسہ;

	إذا (is_signed) {
		sbi_printf("index %ld", (لما)ul_اشاریہ);
	} وإلا {
		sbi_printf("index %lu", ul_اشاریہ);
	}

	sbi_printf(" is out of range for type %s\n",
		   p_انکڑے->mArrayType->mTypeName);

	SEPARATOR;
}

ساكن bool is_Negative_نمبر(بنية CTypeDescriptor *pType, بنانشانی لما ulVal)
{
	إذا (!(pType->mTypeInfo & نمبر_SIGNED_ثنائی_ہندسہ)) {
		أرجع false;
	}

	أرجع (لما)ulVal < 0;
}

ساكن size_t type_چوڑائی(بنية CTypeDescriptor *pType)
{
	أرجع 1UL << (pType->mTypeInfo >> 1);
}

ساكن فراغ Handle_کھسکاؤ_باہرلا_دا_Bounds(bool isFatal,
				   بنية CShift_باہرلا_دا_Bounds_انکڑے *p_انکڑے,
				   بنانشانی لما ulLHS, بنانشانی لما ulRHS)
{
	إذا (!p_انکڑے) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_انکڑے->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_انکڑے->mLocation.mFilename,
		   p_انکڑے->mLocation.m_سطر & ~ACK_REPORTED,
		   p_انکڑے->mLocation.m_کالم);

	إذا (is_Negative_نمبر(p_انکڑے->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (لما)ulRHS);
	} وإلا إذا (ulRHS >= type_چوڑائی(p_انکڑے->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (بنانشانی لما)type_چوڑائی(p_انکڑے->mLHSType),
			p_انکڑے->mLHSType->mTypeName);

	} وإلا إذا (is_Negative_نمبر(p_انکڑے->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (لما)ulLHS);
	} وإلا {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, p_انکڑے->mLHSType->mTypeName);
	}

	SEPARATOR;
}

ساكن فراغ Handle_لدو_Invalid_قدر(bool isFatal,
				   بنية CInvalid_قدر_انکڑے *p_انکڑے,
				   بنانشانی لما ul_قدر)
{
	إذا (!p_انکڑے) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_انکڑے->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_انکڑے->mLocation.mFilename,
		   p_انکڑے->mLocation.m_سطر & ~ACK_REPORTED,
		   p_انکڑے->mLocation.m_کالم);

	bool is_signed = p_انکڑے->mType->mTypeInfo & نمبر_SIGNED_ثنائی_ہندسہ;

	sbi_printf("load of value ");

	إذا (is_signed) {
		sbi_printf("%ld ", (لما)ul_قدر);
	} وإلا {
		sbi_printf("%lu ", ul_قدر);
	}

	sbi_printf("is not a valid value for type %s\n",
		   p_انکڑے->mType->mTypeName);

	SEPARATOR;
}

ثابت حرف *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

ساكن فراغ HandleInvalidBuiltin(bool isFatal,
				 بنية CInvalid_Builtin_انکڑے *p_انکڑے)
{
	إذا (!p_انکڑے) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_انکڑے->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_انکڑے->mLocation.mFilename,
		   p_انکڑے->mLocation.m_سطر & ~ACK_REPORTED,
		   p_انکڑے->mLocation.m_کالم);

	ثابت حرف *builtin =
		(p_انکڑے->m_قسم < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[p_انکڑے->m_قسم]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

ساكن فراغ HandleFunctionTypeMismatch(bool isFatal,
				       بنية CFunction_Type_Mismatch_انکڑے *p_انکڑے,
				       بنانشانی لما ulFunction)
{
	إذا (!p_انکڑے) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_انکڑے->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_انکڑے->mLocation.mFilename,
		   p_انکڑے->mLocation.m_سطر & ~ACK_REPORTED,
		   p_انکڑے->mLocation.m_کالم);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, p_انکڑے->mType->mTypeName);

	SEPARATOR;
}

ساكن فراغ HandleMissingReturn(bool isFatal, بنية CUnreachable_انکڑے *p_انکڑے)
{
	إذا (!p_انکڑے) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_انکڑے->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_انکڑے->mLocation.mFilename,
		   p_انکڑے->mLocation.m_سطر & ~ACK_REPORTED,
		   p_انکڑے->mLocation.m_کالم);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

ساكن فراغ Handle_اشارہ_گر_حد_توں_ودھ(bool isFatal,
				  بنية CPointer_حد_توں_ودھ_انکڑے *p_انکڑے,
				  بنانشانی لما ul_ادھار, بنانشانی لما ul_نتیجہ)
{
	إذا (!p_انکڑے) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_انکڑے->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_انکڑے->mLocation.mFilename,
		   p_انکڑے->mLocation.m_سطر & ~ACK_REPORTED,
		   p_انکڑے->mLocation.m_کالم);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ul_ادھار, ul_نتیجہ);

	SEPARATOR;
}

ساكن فراغ HandleAlignmentAssumption(bool isFatal,
				      بنية CAlignment_Assumption_انکڑے *p_انکڑے,
				      بنانشانی لما ul_اشارہ_گر,
				      بنانشانی لما ulAlignment,
				      بنانشانی لما ul_وتھ)
{

	إذا (!p_انکڑے) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_انکڑے->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_انکڑے->mLocation.mFilename,
		   p_انکڑے->mLocation.m_سطر & ~ACK_REPORTED,
		   p_انکڑے->mLocation.m_کالم);

	بنانشانی لما ul_Real_اشارہ_گر = ul_اشارہ_گر - ul_وتھ;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (فراغ *)ul_Real_اشارہ_گر, (فراغ *)ul_وتھ);

	إذا (p_انکڑے->mAssumptionLocation.mFilename != عدم) {
		sbi_printf(", assumption made in %s:%u:%u",
			   p_انکڑے->mAssumptionLocation.mFilename,
			   p_انکڑے->mAssumptionLocation.m_سطر,
			   p_انکڑے->mAssumptionLocation.m_کالم);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
فراغ __ubsan_handle_add_overflow(بنية COverflow_انکڑے *p_انکڑے,
				 بنانشانی لما ulLHS, بنانشانی لما ulRHS)
{
	Handle_حد_توں_ودھ(false, p_انکڑے, ulLHS, ulRHS, "+");
}

فراغ __ubsan_handle_add_overflow_abort(بنية COverflow_انکڑے *p_انکڑے,
				       بنانشانی لما ulLHS, بنانشانی لما ulRHS)
{
	Handle_حد_توں_ودھ(true, p_انکڑے, ulLHS, ulRHS, "+");
}

فراغ __ubsan_handle_sub_overflow(بنية COverflow_انکڑے *p_انکڑے,
				 بنانشانی لما ulLHS, بنانشانی لما ulRHS)
{
	Handle_حد_توں_ودھ(false, p_انکڑے, ulLHS, ulRHS, "-");
}

فراغ __ubsan_handle_sub_overflow_abort(بنية COverflow_انکڑے *p_انکڑے,
				       بنانشانی لما ulLHS, بنانشانی لما ulRHS)
{
	Handle_حد_توں_ودھ(true, p_انکڑے, ulLHS, ulRHS, "-");
}

فراغ __ubsan_handle_mul_overflow(بنية COverflow_انکڑے *p_انکڑے,
				 بنانشانی لما ulLHS, بنانشانی لما ulRHS)
{
	Handle_حد_توں_ودھ(false, p_انکڑے, ulLHS, ulRHS, "*");
}

فراغ __ubsan_handle_mul_overflow_abort(بنية COverflow_انکڑے *p_انکڑے,
				       بنانشانی لما ulLHS, بنانشانی لما ulRHS)
{
	Handle_حد_توں_ودھ(true, p_انکڑے, ulLHS, ulRHS, "*");
}

فراغ __ubsan_handle_negate_overflow(بنية COverflow_انکڑے *p_انکڑے,
				    بنانشانی لما ul_Old_قدر)
{
	Handle_Negate_حد_توں_ودھ(false, p_انکڑے, ul_Old_قدر);
}

فراغ __ubsan_handle_negate_overflow_abort(بنية COverflow_انکڑے *p_انکڑے,
					  بنانشانی لما ul_Old_قدر)
{
	Handle_Negate_حد_توں_ودھ(true, p_انکڑے, ul_Old_قدر);
}

فراغ __ubsan_handle_divrem_overflow(بنية COverflow_انکڑے *p_انکڑے,
				    بنانشانی لما ulLHS, بنانشانی لما ulRHS)
{
	Handle_حد_توں_ودھ(false, p_انکڑے, ulLHS, ulRHS, "divrem");
}

فراغ __ubsan_handle_divrem_overflow_abort(بنية COverflow_انکڑے *p_انکڑے,
					  بنانشانی لما ulLHS,
					  بنانشانی لما ulRHS)
{
	Handle_حد_توں_ودھ(true, p_انکڑے, ulLHS, ulRHS, "divrem");
}

فراغ __ubsan_handle_type_mismatch_v1(بنية CType_Mismatch_انکڑے_v1 *p_انکڑے,
				     بنانشانی لما ul_اشارہ_گر)
{
	HandleTypeMismatch(false, &p_انکڑے->mLocation, p_انکڑے->mType,
			   __BIT(p_انکڑے->mLogAlignment), p_انکڑے->m_Type_Check_قسم,
			   ul_اشارہ_گر);
}

فراغ __ubsan_handle_type_mismatch_v1_abort(بنية CType_Mismatch_انکڑے_v1 *p_انکڑے,
					   بنانشانی لما ul_اشارہ_گر)
{
	HandleTypeMismatch(true, &p_انکڑے->mLocation, p_انکڑے->mType,
			   __BIT(p_انکڑے->mLogAlignment), p_انکڑے->m_Type_Check_قسم,
			   ul_اشارہ_گر);
}

فراغ __ubsan_handle_out_of_bounds(بنية COut_دا_Bounds_انکڑے *p_انکڑے,
				  بنانشانی لما ul_اشاریہ)
{
	Handle_باہرلا_دا_Bounds(false, p_انکڑے, ul_اشاریہ);
}

فراغ __ubsan_handle_out_of_bounds_abort(بنية COut_دا_Bounds_انکڑے *p_انکڑے,
					بنانشانی لما ul_اشاریہ)
{
	Handle_باہرلا_دا_Bounds(true, p_انکڑے, ul_اشاریہ);
}

فراغ __ubsan_handle_shift_out_of_bounds(بنية CShift_باہرلا_دا_Bounds_انکڑے *p_انکڑے,
					بنانشانی لما ulLHS,
					بنانشانی لما ulRHS)
{
	Handle_کھسکاؤ_باہرلا_دا_Bounds(false, p_انکڑے, ulLHS, ulRHS);
}

فراغ __ubsan_handle_shift_out_of_bounds_abort(
	بنية CShift_باہرلا_دا_Bounds_انکڑے *p_انکڑے, بنانشانی لما ulLHS,
	بنانشانی لما ulRHS)
{
	Handle_کھسکاؤ_باہرلا_دا_Bounds(true, p_انکڑے, ulLHS, ulRHS);
}

فراغ __ubsan_handle_pointer_overflow(بنية CPointer_حد_توں_ودھ_انکڑے *p_انکڑے,
				     بنانشانی لما ul_ادھار,
				     بنانشانی لما ul_نتیجہ)
{
	Handle_اشارہ_گر_حد_توں_ودھ(false, p_انکڑے, ul_ادھار, ul_نتیجہ);
}

فراغ __ubsan_handle_pointer_overflow_abort(بنية CPointer_حد_توں_ودھ_انکڑے *p_انکڑے,
					   بنانشانی لما ul_ادھار,
					   بنانشانی لما ul_نتیجہ)
{
	Handle_اشارہ_گر_حد_توں_ودھ(true, p_انکڑے, ul_ادھار, ul_نتیجہ);
}

فراغ __ubsan_handle_alignment_assumption(بنية CAlignment_Assumption_انکڑے *p_انکڑے,
					 بنانشانی لما ul_اشارہ_گر,
					 بنانشانی لما ulAlignment,
					 بنانشانی لما ul_وتھ)
{
	HandleAlignmentAssumption(false, p_انکڑے, ul_اشارہ_گر, ulAlignment,
				  ul_وتھ);
}

فراغ __ubsan_handle_alignment_assumption_abort(
	بنية CAlignment_Assumption_انکڑے *p_انکڑے, بنانشانی لما ul_اشارہ_گر,
	بنانشانی لما ulAlignment, بنانشانی لما ul_وتھ)
{
	HandleAlignmentAssumption(true, p_انکڑے, ul_اشارہ_گر, ulAlignment,
				  ul_وتھ);
}

فراغ __ubsan_handle_builtin_unreachable(بنية CUnreachable_انکڑے *p_انکڑے)
{
	HandleBuiltinUnreachable(true, p_انکڑے);
}

فراغ __ubsan_handle_invalid_builtin(بنية CInvalid_Builtin_انکڑے *p_انکڑے)
{
	HandleInvalidBuiltin(true, p_انکڑے);
}

فراغ __ubsan_handle_invalid_builtin_abort(بنية CInvalid_Builtin_انکڑے *p_انکڑے)
{
	HandleInvalidBuiltin(true, p_انکڑے);
}

فراغ __ubsan_handle_load_invalid_value(بنية CInvalid_قدر_انکڑے *p_انکڑے,
				       بنانشانی لما ul_قدر)
{
	Handle_لدو_Invalid_قدر(false, p_انکڑے, ul_قدر);
}

فراغ __ubsan_handle_load_invalid_value_abort(بنية CInvalid_قدر_انکڑے *p_انکڑے,
					     بنانشانی لما ul_قدر)
{
	Handle_لدو_Invalid_قدر(true, p_انکڑے, ul_قدر);
}

فراغ __ubsan_handle_missing_return(بنية CUnreachable_انکڑے *p_انکڑے)
{
	HandleMissingReturn(true, p_انکڑے);
}

فراغ __ubsan_handle_vla_bound_not_positive(بنية CVLABound_انکڑے *p_انکڑے,
					   بنانشانی لما ulBound)
{
	Handle_Vla_Bound_Not_مثبت(false, p_انکڑے, ulBound);
}

فراغ __ubsan_handle_vla_bound_not_positive_abort(بنية CVLABound_انکڑے *p_انکڑے,
						 بنانشانی لما ulBound)
{
	Handle_Vla_Bound_Not_مثبت(true, p_انکڑے, ulBound);
}

فراغ __ubsan_handle_function_type_mismatch(
	بنية CFunction_Type_Mismatch_انکڑے *p_انکڑے, بنانشانی لما ulFunction)
{
	HandleFunctionTypeMismatch(false, p_انکڑے, ulFunction);
}

فراغ __ubsan_handle_function_type_mismatch_abort(
	بنية CFunction_Type_Mismatch_انکڑے *p_انکڑے, بنانشانی لما ulFunction)
{
	HandleFunctionTypeMismatch(true, p_انکڑے, ulFunction);
}
#جے_انت
