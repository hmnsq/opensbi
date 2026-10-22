/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#که_تعریف_شوی UBSAN_ENABLED

#ضمن <sbi/sbi_types.h>
#ضمن <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#تعریف_کړه ډول_INTEGER 0
#تعریف_کړه ډول_FLOAT 1
#تعریف_کړه ډول_UNKNOWN UINT16_MAX

متطاير بینښه اوږد sbi_ubsan_report_شمېر;

بنية CSourceLocation {
	حرف *mFilename;
	uint32_t m_کرښه;
	uint32_t m_ستنه;
};

بنية CTypeDescriptor {
	uint16_t m_Type_ډول;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

بنية COverflow_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CUnreachable_معلومات {
	بنية CSourceLocation mLocation;
};

بنية CCFICheck_Fail_معلومات {
	uint8_t m_Check_ډول;
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CDynamic_Type_Cache_نه_موندنه_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
	فراغ *mTypeInfo;
	uint8_t m_Type_Check_ډول;
};

بنية CFunction_Type_Mismatch_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CInvalid_Builtin_معلومات {
	بنية CSourceLocation mLocation;
	uint8_t m_ډول;
};

بنية CInvalid_ارزښت_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CNon_Null_Arg_معلومات {
	بنية CSourceLocation mLocation;
	بنية CSourceLocation mAttributeLocation;
	صحيح m_Arg_شاخص;
};

بنية CNon_Null_Return_معلومات {
	بنية CSourceLocation mAttributeLocation;
};

بنية COut_د_Bounds_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mArrayType;
	بنية CTypeDescriptor *m_شاخص_Type;
};

بنية CPointer_ډکېدنه_معلومات {
	بنية CSourceLocation mLocation;
};

بنية CShift_بهر_د_Bounds_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mLHSType;
	بنية CTypeDescriptor *mRHSType;
};

بنية CType_Mismatch_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
	بینښه اوږد mLogAlignment;
	uint8_t m_Type_Check_ډول;
};

بنية CType_Mismatch_معلومات_v1 {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t m_Type_Check_ډول;
};

بنية CVLABound_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mType;
};

بنية CFloat_Cast_ډکېدنه_معلومات {
	بنية CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	بنية CTypeDescriptor *mFromType;
	بنية CTypeDescriptor *m_منزل_Type;
};

بنية CImplicit_Conversion_معلومات {
	بنية CSourceLocation mLocation;
	بنية CTypeDescriptor *mFromType;
	بنية CTypeDescriptor *m_منزل_Type;
	uint8_t m_ډول;
};

بنية CAlignment_Assumption_معلومات {
	بنية CSourceLocation mLocation;
	بنية CSourceLocation mAssumptionLocation;
	بنية CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
فراغ __ubsan_handle_add_overflow(بنية COverflow_معلومات *p_معلومات,
				 بینښه اوږد ulLHS, بینښه اوږد ulRHS);
فراغ __ubsan_handle_add_overflow_abort(بنية COverflow_معلومات *p_معلومات,
				       بینښه اوږد ulLHS,
				       بینښه اوږد ulRHS);
فراغ __ubsan_handle_alignment_assumption(بنية CAlignment_Assumption_معلومات *p_معلومات,
					 بینښه اوږد ul_اشاره_ګر,
					 بینښه اوږد ulAlignment,
					 بینښه اوږد ul_دننه_واټن);
فراغ __ubsan_handle_alignment_assumption_abort(
	بنية CAlignment_Assumption_معلومات *p_معلومات, بینښه اوږد ul_اشاره_ګر,
	بینښه اوږد ulAlignment, بینښه اوږد ul_دننه_واټن);
فراغ __ubsan_handle_builtin_unreachable(بنية CUnreachable_معلومات *p_معلومات);
فراغ __ubsan_handle_divrem_overflow(بنية COverflow_معلومات *p_معلومات,
				    بینښه اوږد ulLHS, بینښه اوږد ulRHS);
فراغ __ubsan_handle_divrem_overflow_abort(بنية COverflow_معلومات *p_معلومات,
					  بینښه اوږد ulLHS,
					  بینښه اوږد ulRHS);
فراغ __ubsan_handle_function_type_mismatch(
	بنية CFunction_Type_Mismatch_معلومات *p_معلومات, بینښه اوږد ulFunction);
فراغ __ubsan_handle_function_type_mismatch_abort(
	بنية CFunction_Type_Mismatch_معلومات *p_معلومات, بینښه اوږد ulFunction);
فراغ __ubsan_handle_function_type_mismatch_v1(
	بنية CFunction_Type_Mismatch_معلومات *p_معلومات, بینښه اوږد ulFunction,
	بینښه اوږد ulCalleeRTTI, بینښه اوږد ulFnRTTI);
فراغ __ubsan_handle_function_type_mismatch_v1_abort(
	بنية CFunction_Type_Mismatch_معلومات *p_معلومات, بینښه اوږد ulFunction,
	بینښه اوږد ulCalleeRTTI, بینښه اوږد ulFnRTTI);
فراغ __ubsan_handle_invalid_builtin(بنية CInvalid_Builtin_معلومات *p_معلومات);
فراغ __ubsan_handle_invalid_builtin_abort(بنية CInvalid_Builtin_معلومات *p_معلومات);
فراغ __ubsan_handle_load_invalid_value(بنية CInvalid_ارزښت_معلومات *p_معلومات,
				       بینښه اوږد ulVal);
فراغ __ubsan_handle_load_invalid_value_abort(بنية CInvalid_ارزښت_معلومات *p_معلومات,
					     بینښه اوږد ulVal);
فراغ __ubsan_handle_missing_return(بنية CUnreachable_معلومات *p_معلومات);
فراغ __ubsan_handle_mul_overflow(بنية COverflow_معلومات *p_معلومات,
				 بینښه اوږد ulLHS, بینښه اوږد ulRHS);
فراغ __ubsan_handle_mul_overflow_abort(بنية COverflow_معلومات *p_معلومات,
				       بینښه اوږد ulLHS,
				       بینښه اوږد ulRHS);
فراغ __ubsan_handle_negate_overflow(بنية COverflow_معلومات *p_معلومات,
				    بینښه اوږد ulOldVal);
فراغ __ubsan_handle_negate_overflow_abort(بنية COverflow_معلومات *p_معلومات,
					  بینښه اوږد ulOldVal);
فراغ __ubsan_handle_nullability_arg(بنية CNon_Null_Arg_معلومات *p_معلومات);
فراغ __ubsan_handle_nullability_arg_abort(بنية CNon_Null_Arg_معلومات *p_معلومات);
فراغ __ubsan_handle_nullability_return_v1(
	بنية CNon_Null_Return_معلومات *p_معلومات,
	بنية CSourceLocation *p_Location_اشاره_ګر);
فراغ __ubsan_handle_nullability_return_v1_abort(
	بنية CNon_Null_Return_معلومات *p_معلومات,
	بنية CSourceLocation *p_Location_اشاره_ګر);
فراغ __ubsan_handle_out_of_bounds(بنية COut_د_Bounds_معلومات *p_معلومات,
				  بینښه اوږد ul_شاخص);
فراغ __ubsan_handle_out_of_bounds_abort(بنية COut_د_Bounds_معلومات *p_معلومات,
					بینښه اوږد ul_شاخص);
فراغ __ubsan_handle_pointer_overflow(بنية CPointer_ډکېدنه_معلومات *p_معلومات,
				     بینښه اوږد ul_بنسټ,
				     بینښه اوږد ul_پایله);
فراغ __ubsan_handle_pointer_overflow_abort(بنية CPointer_ډکېدنه_معلومات *p_معلومات,
					   بینښه اوږد ul_بنسټ,
					   بینښه اوږد ul_پایله);
فراغ __ubsan_handle_shift_out_of_bounds(بنية CShift_بهر_د_Bounds_معلومات *p_معلومات,
					بینښه اوږد ulLHS,
					بینښه اوږد ulRHS);
فراغ __ubsan_handle_shift_out_of_bounds_abort(
	بنية CShift_بهر_د_Bounds_معلومات *p_معلومات, بینښه اوږد ulLHS,
	بینښه اوږد ulRHS);
فراغ __ubsan_handle_sub_overflow(بنية COverflow_معلومات *p_معلومات,
				 بینښه اوږد ulLHS, بینښه اوږد ulRHS);
فراغ __ubsan_handle_sub_overflow_abort(بنية COverflow_معلومات *p_معلومات,
				       بینښه اوږد ulLHS,
				       بینښه اوږد ulRHS);
فراغ __ubsan_handle_type_mismatch(بنية CType_Mismatch_معلومات *p_معلومات,
				  بینښه اوږد ul_اشاره_ګر);
فراغ __ubsan_handle_type_mismatch_abort(بنية CType_Mismatch_معلومات *p_معلومات,
					بینښه اوږد ul_اشاره_ګر);
فراغ __ubsan_handle_type_mismatch_v1(بنية CType_Mismatch_معلومات_v1 *p_معلومات,
				     بینښه اوږد ul_اشاره_ګر);
فراغ __ubsan_handle_type_mismatch_v1_abort(بنية CType_Mismatch_معلومات_v1 *p_معلومات,
					   بینښه اوږد ul_اشاره_ګر);
فراغ __ubsan_handle_vla_bound_not_positive(بنية CVLABound_معلومات *p_معلومات,
					   بینښه اوږد ulBound);
فراغ __ubsan_handle_vla_bound_not_positive_abort(بنية CVLABound_معلومات *p_معلومات,
						 بینښه اوږد ulBound);
فراغ __ubsan_get_current_report_data(ثابت حرف **pp_بهر_Issue_ډول,
				     ثابت حرف **pp_بهر_پیغام,
				     ثابت حرف **pp_بهر_Filename,
				     uint32_t *p_بهر_کرښه, uint32_t *p_بهر_ستون,
				     حرف **pp_بهر_حافظه_Addr);
ساكن فراغ Handle_ډکېدنه(bool isFatal, بنية COverflow_معلومات *p_معلومات,
			   بینښه اوږد ulLHS, بینښه اوږد ulRHS,
			   ثابت حرف *szOperation);
ساكن فراغ Handle_Negate_ډکېدنه(bool isFatal, بنية COverflow_معلومات *p_معلومات,
				 بینښه اوږد ul_Old_ارزښت);
ساكن فراغ HandleBuiltinUnreachable(bool isFatal,
				     بنية CUnreachable_معلومات *p_معلومات);
ساكن فراغ HandleTypeMismatch(bool isFatal, بنية CSourceLocation *mLocation,
			       بنية CTypeDescriptor *mType,
			       بینښه اوږد mLogAlignment,
			       uint8_t m_Type_Check_ډول, بینښه اوږد ul_اشاره_ګر);
ساكن فراغ Handle_Vla_Bound_Not_مثبت(bool isFatal, بنية CVLABound_معلومات *p_معلومات,
				      بینښه اوږد ulBound);
ساكن فراغ Handle_بهر_د_Bounds(bool isFatal, بنية COut_د_Bounds_معلومات *p_معلومات,
			      بینښه اوږد ul_شاخص);
ساكن فراغ Handle_ښويدنه_بهر_د_Bounds(bool isFatal,
				   بنية CShift_بهر_د_Bounds_معلومات *p_معلومات,
				   بینښه اوږد ulLHS, بینښه اوږد ulRHS);
ساكن فراغ Handle_بارول_Invalid_ارزښت(bool isFatal,
				   بنية CInvalid_ارزښت_معلومات *p_معلومات,
				   بینښه اوږد ul_ارزښت);
ساكن فراغ HandleInvalidBuiltin(bool isFatal,
				 بنية CInvalid_Builtin_معلومات *p_معلومات);
ساكن فراغ HandleFunctionTypeMismatch(bool isFatal,
				       بنية CFunction_Type_Mismatch_معلومات *p_معلومات,
				       بینښه اوږد ulFunction);
ساكن فراغ HandleMissingReturn(bool isFatal, بنية CUnreachable_معلومات *p_معلومات);
ساكن فراغ Handle_اشاره_ګر_ډکېدنه(bool isFatal,
				  بنية CPointer_ډکېدنه_معلومات *p_معلومات,
				  بینښه اوږد ul_بنسټ, بینښه اوږد ul_پایله);
ساكن فراغ HandleAlignmentAssumption(bool isFatal,
				      بنية CAlignment_Assumption_معلومات *p_معلومات,
				      بینښه اوږد ul_اشاره_ګر,
				      بینښه اوږد ulAlignment,
				      بینښه اوږد ul_دننه_واټن);

#تعریف_کړه شمېره_SIGNED_بټ 1
#تعریف_کړه شمېره_MAXLEN 128
#تعریف_کړه __arraycount(__a) (حجم(__a) / حجم(__a[0]))
#تعریف_کړه __BIT(__n) (1UL << (__n))
#تعریف_کړه SEPARATOR sbi_printf("===========================================\n")
#تعریف_کړه ACK_REPORTED (1U << 31)

ساكن bool isAlreadyReported(بنية CSourceLocation *pLocation)
{
	uint32_t si_Old_ارزښت;
	متطاير uint32_t *p_کرښه;

	إذا (!pLocation)
		أرجع false;

	p_کرښه = &pLocation->m_کرښه;

	افعل {
		si_Old_ارزښت = *p_کرښه;
	} طالما (__sync_val_compare_and_swap(p_کرښه, si_Old_ارزښت,
					     si_Old_ارزښت | ACK_REPORTED) !=
		 si_Old_ارزښت);

	إذا (!(si_Old_ارزښت & ACK_REPORTED)) {

		sbi_ubsan_report_شمېر++;

		أرجع false;
	}

	أرجع true;
}

ساكن فراغ Handle_ډکېدنه(bool isFatal, بنية COverflow_معلومات *p_معلومات,
			   بینښه اوږد ulLHS, بینښه اوږد ulRHS,
			   ثابت حرف *szOperation)
{
	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_کرښه & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ستنه);

	bool is_signed = p_معلومات->mType->mTypeInfo & شمېره_SIGNED_بټ;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	إذا (is_signed) {
		sbi_printf("%ld %s %ld ", (اوږد)ulLHS, szOperation,
			   (اوږد)ulRHS);
	} وإلا {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_معلومات->mType->mTypeName);

	SEPARATOR;
}

ساكن فراغ Handle_Negate_ډکېدنه(bool isFatal, بنية COverflow_معلومات *p_معلومات,
				 بینښه اوږد ul_Old_ارزښت)
{
	إذا (!p_معلومات) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_کرښه & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ستنه);

	bool is_signed = p_معلومات->mType->mTypeInfo & شمېره_SIGNED_بټ;

	sbi_printf("UBSan: Negation of ");

	إذا (is_signed) {
		sbi_printf("%ld", (اوږد)ul_Old_ارزښت);
	} وإلا {
		sbi_printf("%lu", ul_Old_ارزښت);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_معلومات->mType->mTypeName);

	SEPARATOR;
}

ساكن فراغ HandleBuiltinUnreachable(bool isFatal,
				     بنية CUnreachable_معلومات *p_معلومات)
{
	إذا (!p_معلومات) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_کرښه & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ستنه);

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
			       بینښه اوږد mLogAlignment,
			       uint8_t m_Type_Check_ډول, بینښه اوږد ul_اشاره_ګر)
{

	إذا ((!mLocation) || (!mType)) {
		أرجع;
	}

	إذا (isAlreadyReported(mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_کرښه & ~ACK_REPORTED,
		   mLocation->m_ستنه);

	ثابت حرف *ډول = (m_Type_Check_ډول < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[m_Type_Check_ډول]
				   : "access to";

	إذا (ul_اشاره_ګر == 0) {
		sbi_printf("%s null pointer of type %s\n", ډول,
			   mType->mTypeName);
	} وإلا إذا (
		(mLogAlignment - 1) &
		ul_اشاره_ګر) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			ډول, (فراغ *)ul_اشاره_ګر, mType->mTypeName,
			mLogAlignment);
	} وإلا {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			ډول, (فراغ *)ul_اشاره_ګر, mType->mTypeName);
	}
	SEPARATOR;
}

ساكن فراغ Handle_Vla_Bound_Not_مثبت(bool isFatal, بنية CVLABound_معلومات *p_معلومات,
				      بینښه اوږد ulBound)
{
	إذا (!p_معلومات) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_کرښه & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ستنه);

	sbi_printf("variable length array bound value ");

	bool is_signed = p_معلومات->mType->mTypeInfo & شمېره_SIGNED_بټ;

	إذا (is_signed) {
		sbi_printf("%ld", (اوږد)ulBound);
	} وإلا {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

ساكن فراغ Handle_بهر_د_Bounds(bool isFatal, بنية COut_د_Bounds_معلومات *p_معلومات,
			      بینښه اوږد ul_شاخص)
{
	إذا (!p_معلومات) {
		أرجع;
	}
	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_کرښه & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ستنه);

	bool is_signed = p_معلومات->m_شاخص_Type->mTypeInfo & شمېره_SIGNED_بټ;

	إذا (is_signed) {
		sbi_printf("index %ld", (اوږد)ul_شاخص);
	} وإلا {
		sbi_printf("index %lu", ul_شاخص);
	}

	sbi_printf(" is out of range for type %s\n",
		   p_معلومات->mArrayType->mTypeName);

	SEPARATOR;
}

ساكن bool is_Negative_شمېره(بنية CTypeDescriptor *pType, بینښه اوږد ulVal)
{
	إذا (!(pType->mTypeInfo & شمېره_SIGNED_بټ)) {
		أرجع false;
	}

	أرجع (اوږد)ulVal < 0;
}

ساكن size_t type_پلنوالی(بنية CTypeDescriptor *pType)
{
	أرجع 1UL << (pType->mTypeInfo >> 1);
}

ساكن فراغ Handle_ښويدنه_بهر_د_Bounds(bool isFatal,
				   بنية CShift_بهر_د_Bounds_معلومات *p_معلومات,
				   بینښه اوږد ulLHS, بینښه اوږد ulRHS)
{
	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_کرښه & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ستنه);

	إذا (is_Negative_شمېره(p_معلومات->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (اوږد)ulRHS);
	} وإلا إذا (ulRHS >= type_پلنوالی(p_معلومات->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (بینښه اوږد)type_پلنوالی(p_معلومات->mLHSType),
			p_معلومات->mLHSType->mTypeName);

	} وإلا إذا (is_Negative_شمېره(p_معلومات->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (اوږد)ulLHS);
	} وإلا {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, p_معلومات->mLHSType->mTypeName);
	}

	SEPARATOR;
}

ساكن فراغ Handle_بارول_Invalid_ارزښت(bool isFatal,
				   بنية CInvalid_ارزښت_معلومات *p_معلومات,
				   بینښه اوږد ul_ارزښت)
{
	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_کرښه & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ستنه);

	bool is_signed = p_معلومات->mType->mTypeInfo & شمېره_SIGNED_بټ;

	sbi_printf("load of value ");

	إذا (is_signed) {
		sbi_printf("%ld ", (اوږد)ul_ارزښت);
	} وإلا {
		sbi_printf("%lu ", ul_ارزښت);
	}

	sbi_printf("is not a valid value for type %s\n",
		   p_معلومات->mType->mTypeName);

	SEPARATOR;
}

ثابت حرف *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

ساكن فراغ HandleInvalidBuiltin(bool isFatal,
				 بنية CInvalid_Builtin_معلومات *p_معلومات)
{
	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_کرښه & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ستنه);

	ثابت حرف *builtin =
		(p_معلومات->m_ډول < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[p_معلومات->m_ډول]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

ساكن فراغ HandleFunctionTypeMismatch(bool isFatal,
				       بنية CFunction_Type_Mismatch_معلومات *p_معلومات,
				       بینښه اوږد ulFunction)
{
	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_کرښه & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ستنه);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, p_معلومات->mType->mTypeName);

	SEPARATOR;
}

ساكن فراغ HandleMissingReturn(bool isFatal, بنية CUnreachable_معلومات *p_معلومات)
{
	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_کرښه & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ستنه);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

ساكن فراغ Handle_اشاره_ګر_ډکېدنه(bool isFatal,
				  بنية CPointer_ډکېدنه_معلومات *p_معلومات,
				  بینښه اوږد ul_بنسټ, بینښه اوږد ul_پایله)
{
	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_کرښه & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ستنه);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ul_بنسټ, ul_پایله);

	SEPARATOR;
}

ساكن فراغ HandleAlignmentAssumption(bool isFatal,
				      بنية CAlignment_Assumption_معلومات *p_معلومات,
				      بینښه اوږد ul_اشاره_ګر,
				      بینښه اوږد ulAlignment,
				      بینښه اوږد ul_دننه_واټن)
{

	إذا (!p_معلومات) {
		أرجع;
	}

	إذا (isAlreadyReported(&p_معلومات->mLocation)) {
		أرجع;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_معلومات->mLocation.mFilename,
		   p_معلومات->mLocation.m_کرښه & ~ACK_REPORTED,
		   p_معلومات->mLocation.m_ستنه);

	بینښه اوږد ul_Real_اشاره_ګر = ul_اشاره_ګر - ul_دننه_واټن;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (فراغ *)ul_Real_اشاره_ګر, (فراغ *)ul_دننه_واټن);

	إذا (p_معلومات->mAssumptionLocation.mFilename != عدم) {
		sbi_printf(", assumption made in %s:%u:%u",
			   p_معلومات->mAssumptionLocation.mFilename,
			   p_معلومات->mAssumptionLocation.m_کرښه,
			   p_معلومات->mAssumptionLocation.m_ستنه);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
فراغ __ubsan_handle_add_overflow(بنية COverflow_معلومات *p_معلومات,
				 بینښه اوږد ulLHS, بینښه اوږد ulRHS)
{
	Handle_ډکېدنه(false, p_معلومات, ulLHS, ulRHS, "+");
}

فراغ __ubsan_handle_add_overflow_abort(بنية COverflow_معلومات *p_معلومات,
				       بینښه اوږد ulLHS, بینښه اوږد ulRHS)
{
	Handle_ډکېدنه(true, p_معلومات, ulLHS, ulRHS, "+");
}

فراغ __ubsan_handle_sub_overflow(بنية COverflow_معلومات *p_معلومات,
				 بینښه اوږد ulLHS, بینښه اوږد ulRHS)
{
	Handle_ډکېدنه(false, p_معلومات, ulLHS, ulRHS, "-");
}

فراغ __ubsan_handle_sub_overflow_abort(بنية COverflow_معلومات *p_معلومات,
				       بینښه اوږد ulLHS, بینښه اوږد ulRHS)
{
	Handle_ډکېدنه(true, p_معلومات, ulLHS, ulRHS, "-");
}

فراغ __ubsan_handle_mul_overflow(بنية COverflow_معلومات *p_معلومات,
				 بینښه اوږد ulLHS, بینښه اوږد ulRHS)
{
	Handle_ډکېدنه(false, p_معلومات, ulLHS, ulRHS, "*");
}

فراغ __ubsan_handle_mul_overflow_abort(بنية COverflow_معلومات *p_معلومات,
				       بینښه اوږد ulLHS, بینښه اوږد ulRHS)
{
	Handle_ډکېدنه(true, p_معلومات, ulLHS, ulRHS, "*");
}

فراغ __ubsan_handle_negate_overflow(بنية COverflow_معلومات *p_معلومات,
				    بینښه اوږد ul_Old_ارزښت)
{
	Handle_Negate_ډکېدنه(false, p_معلومات, ul_Old_ارزښت);
}

فراغ __ubsan_handle_negate_overflow_abort(بنية COverflow_معلومات *p_معلومات,
					  بینښه اوږد ul_Old_ارزښت)
{
	Handle_Negate_ډکېدنه(true, p_معلومات, ul_Old_ارزښت);
}

فراغ __ubsan_handle_divrem_overflow(بنية COverflow_معلومات *p_معلومات,
				    بینښه اوږد ulLHS, بینښه اوږد ulRHS)
{
	Handle_ډکېدنه(false, p_معلومات, ulLHS, ulRHS, "divrem");
}

فراغ __ubsan_handle_divrem_overflow_abort(بنية COverflow_معلومات *p_معلومات,
					  بینښه اوږد ulLHS,
					  بینښه اوږد ulRHS)
{
	Handle_ډکېدنه(true, p_معلومات, ulLHS, ulRHS, "divrem");
}

فراغ __ubsan_handle_type_mismatch_v1(بنية CType_Mismatch_معلومات_v1 *p_معلومات,
				     بینښه اوږد ul_اشاره_ګر)
{
	HandleTypeMismatch(false, &p_معلومات->mLocation, p_معلومات->mType,
			   __BIT(p_معلومات->mLogAlignment), p_معلومات->m_Type_Check_ډول,
			   ul_اشاره_ګر);
}

فراغ __ubsan_handle_type_mismatch_v1_abort(بنية CType_Mismatch_معلومات_v1 *p_معلومات,
					   بینښه اوږد ul_اشاره_ګر)
{
	HandleTypeMismatch(true, &p_معلومات->mLocation, p_معلومات->mType,
			   __BIT(p_معلومات->mLogAlignment), p_معلومات->m_Type_Check_ډول,
			   ul_اشاره_ګر);
}

فراغ __ubsan_handle_out_of_bounds(بنية COut_د_Bounds_معلومات *p_معلومات,
				  بینښه اوږد ul_شاخص)
{
	Handle_بهر_د_Bounds(false, p_معلومات, ul_شاخص);
}

فراغ __ubsan_handle_out_of_bounds_abort(بنية COut_د_Bounds_معلومات *p_معلومات,
					بینښه اوږد ul_شاخص)
{
	Handle_بهر_د_Bounds(true, p_معلومات, ul_شاخص);
}

فراغ __ubsan_handle_shift_out_of_bounds(بنية CShift_بهر_د_Bounds_معلومات *p_معلومات,
					بینښه اوږد ulLHS,
					بینښه اوږد ulRHS)
{
	Handle_ښويدنه_بهر_د_Bounds(false, p_معلومات, ulLHS, ulRHS);
}

فراغ __ubsan_handle_shift_out_of_bounds_abort(
	بنية CShift_بهر_د_Bounds_معلومات *p_معلومات, بینښه اوږد ulLHS,
	بینښه اوږد ulRHS)
{
	Handle_ښويدنه_بهر_د_Bounds(true, p_معلومات, ulLHS, ulRHS);
}

فراغ __ubsan_handle_pointer_overflow(بنية CPointer_ډکېدنه_معلومات *p_معلومات,
				     بینښه اوږد ul_بنسټ,
				     بینښه اوږد ul_پایله)
{
	Handle_اشاره_ګر_ډکېدنه(false, p_معلومات, ul_بنسټ, ul_پایله);
}

فراغ __ubsan_handle_pointer_overflow_abort(بنية CPointer_ډکېدنه_معلومات *p_معلومات,
					   بینښه اوږد ul_بنسټ,
					   بینښه اوږد ul_پایله)
{
	Handle_اشاره_ګر_ډکېدنه(true, p_معلومات, ul_بنسټ, ul_پایله);
}

فراغ __ubsan_handle_alignment_assumption(بنية CAlignment_Assumption_معلومات *p_معلومات,
					 بینښه اوږد ul_اشاره_ګر,
					 بینښه اوږد ulAlignment,
					 بینښه اوږد ul_دننه_واټن)
{
	HandleAlignmentAssumption(false, p_معلومات, ul_اشاره_ګر, ulAlignment,
				  ul_دننه_واټن);
}

فراغ __ubsan_handle_alignment_assumption_abort(
	بنية CAlignment_Assumption_معلومات *p_معلومات, بینښه اوږد ul_اشاره_ګر,
	بینښه اوږد ulAlignment, بینښه اوږد ul_دننه_واټن)
{
	HandleAlignmentAssumption(true, p_معلومات, ul_اشاره_ګر, ulAlignment,
				  ul_دننه_واټن);
}

فراغ __ubsan_handle_builtin_unreachable(بنية CUnreachable_معلومات *p_معلومات)
{
	HandleBuiltinUnreachable(true, p_معلومات);
}

فراغ __ubsan_handle_invalid_builtin(بنية CInvalid_Builtin_معلومات *p_معلومات)
{
	HandleInvalidBuiltin(true, p_معلومات);
}

فراغ __ubsan_handle_invalid_builtin_abort(بنية CInvalid_Builtin_معلومات *p_معلومات)
{
	HandleInvalidBuiltin(true, p_معلومات);
}

فراغ __ubsan_handle_load_invalid_value(بنية CInvalid_ارزښت_معلومات *p_معلومات,
				       بینښه اوږد ul_ارزښت)
{
	Handle_بارول_Invalid_ارزښت(false, p_معلومات, ul_ارزښت);
}

فراغ __ubsan_handle_load_invalid_value_abort(بنية CInvalid_ارزښت_معلومات *p_معلومات,
					     بینښه اوږد ul_ارزښت)
{
	Handle_بارول_Invalid_ارزښت(true, p_معلومات, ul_ارزښت);
}

فراغ __ubsan_handle_missing_return(بنية CUnreachable_معلومات *p_معلومات)
{
	HandleMissingReturn(true, p_معلومات);
}

فراغ __ubsan_handle_vla_bound_not_positive(بنية CVLABound_معلومات *p_معلومات,
					   بینښه اوږد ulBound)
{
	Handle_Vla_Bound_Not_مثبت(false, p_معلومات, ulBound);
}

فراغ __ubsan_handle_vla_bound_not_positive_abort(بنية CVLABound_معلومات *p_معلومات,
						 بینښه اوږد ulBound)
{
	Handle_Vla_Bound_Not_مثبت(true, p_معلومات, ulBound);
}

فراغ __ubsan_handle_function_type_mismatch(
	بنية CFunction_Type_Mismatch_معلومات *p_معلومات, بینښه اوږد ulFunction)
{
	HandleFunctionTypeMismatch(false, p_معلومات, ulFunction);
}

فراغ __ubsan_handle_function_type_mismatch_abort(
	بنية CFunction_Type_Mismatch_معلومات *p_معلومات, بینښه اوږد ulFunction)
{
	HandleFunctionTypeMismatch(true, p_معلومات, ulFunction);
}
#د_که_پای
