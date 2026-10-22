/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ifdef UBSAN_ENABLED

#অন্তর্ভাব <sbi/sbi_types.h>
#অন্তর্ভাব <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#define KIND_INTEGER 0
#define KIND_FLOAT 1
#define KIND_UNKNOWN UINT16_MAX

অস্থির unsigned long sbi_ubsan_report_গুনতি;

কাঠামো CSourceLocation {
	অক্ষর *mFilename;
	uint32_t m_সারি;
	uint32_t mColumn;
};

কাঠামো CTypeDescriptor {
	uint16_t mTypeKind;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

কাঠামো COverflow_তথ্য {
	কাঠামো CSourceLocation mLocation;
	কাঠামো CTypeDescriptor *mType;
};

কাঠামো CUnreachable_তথ্য {
	কাঠামো CSourceLocation mLocation;
};

কাঠামো CCFICheck_Fail_তথ্য {
	uint8_t mCheckKind;
	কাঠামো CSourceLocation mLocation;
	কাঠামো CTypeDescriptor *mType;
};

কাঠামো CDynamic_Type_Cache_Miss_তথ্য {
	কাঠামো CSourceLocation mLocation;
	কাঠামো CTypeDescriptor *mType;
	শূন্য *mTypeInfo;
	uint8_t mTypeCheckKind;
};

কাঠামো CFunction_Type_Mismatch_তথ্য {
	কাঠামো CSourceLocation mLocation;
	কাঠামো CTypeDescriptor *mType;
};

কাঠামো CInvalid_Builtin_তথ্য {
	কাঠামো CSourceLocation mLocation;
	uint8_t mKind;
};

কাঠামো CInvalid_মান_তথ্য {
	কাঠামো CSourceLocation mLocation;
	কাঠামো CTypeDescriptor *mType;
};

কাঠামো CNon_Null_Arg_তথ্য {
	কাঠামো CSourceLocation mLocation;
	কাঠামো CSourceLocation mAttributeLocation;
	পূর্ণসংখ্যা m_Arg_সূচক;
};

কাঠামো CNon_Null_Return_তথ্য {
	কাঠামো CSourceLocation mAttributeLocation;
};

কাঠামো COut_Of_Bounds_তথ্য {
	কাঠামো CSourceLocation mLocation;
	কাঠামো CTypeDescriptor *mArrayType;
	কাঠামো CTypeDescriptor *m_সূচক_Type;
};

কাঠামো CPointer_Overflow_তথ্য {
	কাঠামো CSourceLocation mLocation;
};

কাঠামো CShift_Out_Of_Bounds_তথ্য {
	কাঠামো CSourceLocation mLocation;
	কাঠামো CTypeDescriptor *mLHSType;
	কাঠামো CTypeDescriptor *mRHSType;
};

কাঠামো CType_Mismatch_তথ্য {
	কাঠামো CSourceLocation mLocation;
	কাঠামো CTypeDescriptor *mType;
	unsigned long mLogAlignment;
	uint8_t mTypeCheckKind;
};

কাঠামো CType_Mismatch_তথ্য_v1 {
	কাঠামো CSourceLocation mLocation;
	কাঠামো CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t mTypeCheckKind;
};

কাঠামো CVLABound_তথ্য {
	কাঠামো CSourceLocation mLocation;
	কাঠামো CTypeDescriptor *mType;
};

কাঠামো CFloat_Cast_Overflow_তথ্য {
	কাঠামো CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	কাঠামো CTypeDescriptor *mFromType;
	কাঠামো CTypeDescriptor *mToType;
};

কাঠামো CImplicit_Conversion_তথ্য {
	কাঠামো CSourceLocation mLocation;
	কাঠামো CTypeDescriptor *mFromType;
	কাঠামো CTypeDescriptor *mToType;
	uint8_t mKind;
};

কাঠামো CAlignment_Assumption_তথ্য {
	কাঠামো CSourceLocation mLocation;
	কাঠামো CSourceLocation mAssumptionLocation;
	কাঠামো CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
শূন্য __ubsan_handle_add_overflow(কাঠামো COverflow_তথ্য *p_তথ্য,
				 unsigned long ulLHS, unsigned long ulRHS);
শূন্য __ubsan_handle_add_overflow_abort(কাঠামো COverflow_তথ্য *p_তথ্য,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
শূন্য __ubsan_handle_alignment_assumption(কাঠামো CAlignment_Assumption_তথ্য *p_তথ্য,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset);
শূন্য __ubsan_handle_alignment_assumption_abort(
	কাঠামো CAlignment_Assumption_তথ্য *p_তথ্য, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset);
শূন্য __ubsan_handle_builtin_unreachable(কাঠামো CUnreachable_তথ্য *p_তথ্য);
শূন্য __ubsan_handle_divrem_overflow(কাঠামো COverflow_তথ্য *p_তথ্য,
				    unsigned long ulLHS, unsigned long ulRHS);
শূন্য __ubsan_handle_divrem_overflow_abort(কাঠামো COverflow_তথ্য *p_তথ্য,
					  unsigned long ulLHS,
					  unsigned long ulRHS);
শূন্য __ubsan_handle_function_type_mismatch(
	কাঠামো CFunction_Type_Mismatch_তথ্য *p_তথ্য, unsigned long ulFunction);
শূন্য __ubsan_handle_function_type_mismatch_abort(
	কাঠামো CFunction_Type_Mismatch_তথ্য *p_তথ্য, unsigned long ulFunction);
শূন্য __ubsan_handle_function_type_mismatch_v1(
	কাঠামো CFunction_Type_Mismatch_তথ্য *p_তথ্য, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
শূন্য __ubsan_handle_function_type_mismatch_v1_abort(
	কাঠামো CFunction_Type_Mismatch_তথ্য *p_তথ্য, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
শূন্য __ubsan_handle_invalid_builtin(কাঠামো CInvalid_Builtin_তথ্য *p_তথ্য);
শূন্য __ubsan_handle_invalid_builtin_abort(কাঠামো CInvalid_Builtin_তথ্য *p_তথ্য);
শূন্য __ubsan_handle_load_invalid_value(কাঠামো CInvalid_মান_তথ্য *p_তথ্য,
				       unsigned long ulVal);
শূন্য __ubsan_handle_load_invalid_value_abort(কাঠামো CInvalid_মান_তথ্য *p_তথ্য,
					     unsigned long ulVal);
শূন্য __ubsan_handle_missing_return(কাঠামো CUnreachable_তথ্য *p_তথ্য);
শূন্য __ubsan_handle_mul_overflow(কাঠামো COverflow_তথ্য *p_তথ্য,
				 unsigned long ulLHS, unsigned long ulRHS);
শূন্য __ubsan_handle_mul_overflow_abort(কাঠামো COverflow_তথ্য *p_তথ্য,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
শূন্য __ubsan_handle_negate_overflow(কাঠামো COverflow_তথ্য *p_তথ্য,
				    unsigned long ulOldVal);
শূন্য __ubsan_handle_negate_overflow_abort(কাঠামো COverflow_তথ্য *p_তথ্য,
					  unsigned long ulOldVal);
শূন্য __ubsan_handle_nullability_arg(কাঠামো CNon_Null_Arg_তথ্য *p_তথ্য);
শূন্য __ubsan_handle_nullability_arg_abort(কাঠামো CNon_Null_Arg_তথ্য *p_তথ্য);
শূন্য __ubsan_handle_nullability_return_v1(
	কাঠামো CNon_Null_Return_তথ্য *p_তথ্য,
	কাঠামো CSourceLocation *pLocationPointer);
শূন্য __ubsan_handle_nullability_return_v1_abort(
	কাঠামো CNon_Null_Return_তথ্য *p_তথ্য,
	কাঠামো CSourceLocation *pLocationPointer);
শূন্য __ubsan_handle_out_of_bounds(কাঠামো COut_Of_Bounds_তথ্য *p_তথ্য,
				  unsigned long ul_সূচক);
শূন্য __ubsan_handle_out_of_bounds_abort(কাঠামো COut_Of_Bounds_তথ্য *p_তথ্য,
					unsigned long ul_সূচক);
শূন্য __ubsan_handle_pointer_overflow(কাঠামো CPointer_Overflow_তথ্য *p_তথ্য,
				     unsigned long ulBase,
				     unsigned long ul_ফলাফল);
শূন্য __ubsan_handle_pointer_overflow_abort(কাঠামো CPointer_Overflow_তথ্য *p_তথ্য,
					   unsigned long ulBase,
					   unsigned long ul_ফলাফল);
শূন্য __ubsan_handle_shift_out_of_bounds(কাঠামো CShift_Out_Of_Bounds_তথ্য *p_তথ্য,
					unsigned long ulLHS,
					unsigned long ulRHS);
শূন্য __ubsan_handle_shift_out_of_bounds_abort(
	কাঠামো CShift_Out_Of_Bounds_তথ্য *p_তথ্য, unsigned long ulLHS,
	unsigned long ulRHS);
শূন্য __ubsan_handle_sub_overflow(কাঠামো COverflow_তথ্য *p_তথ্য,
				 unsigned long ulLHS, unsigned long ulRHS);
শূন্য __ubsan_handle_sub_overflow_abort(কাঠামো COverflow_তথ্য *p_তথ্য,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
শূন্য __ubsan_handle_type_mismatch(কাঠামো CType_Mismatch_তথ্য *p_তথ্য,
				  unsigned long ulPointer);
শূন্য __ubsan_handle_type_mismatch_abort(কাঠামো CType_Mismatch_তথ্য *p_তথ্য,
					unsigned long ulPointer);
শূন্য __ubsan_handle_type_mismatch_v1(কাঠামো CType_Mismatch_তথ্য_v1 *p_তথ্য,
				     unsigned long ulPointer);
শূন্য __ubsan_handle_type_mismatch_v1_abort(কাঠামো CType_Mismatch_তথ্য_v1 *p_তথ্য,
					   unsigned long ulPointer);
শূন্য __ubsan_handle_vla_bound_not_positive(কাঠামো CVLABound_তথ্য *p_তথ্য,
					   unsigned long ulBound);
শূন্য __ubsan_handle_vla_bound_not_positive_abort(কাঠামো CVLABound_তথ্য *p_তথ্য,
						 unsigned long ulBound);
শূন্য __ubsan_get_current_report_data(ধ্রুবক অক্ষর **ppOutIssueKind,
				     ধ্রুবক অক্ষর **ppOutMessage,
				     ধ্রুবক অক্ষর **ppOutFilename,
				     uint32_t *p_Out_সারি, uint32_t *pOutCol,
				     অক্ষর **ppOutMemoryAddr);
স্থির শূন্য HandleOverflow(bool isFatal, কাঠামো COverflow_তথ্য *p_তথ্য,
			   unsigned long ulLHS, unsigned long ulRHS,
			   ধ্রুবক অক্ষর *szOperation);
স্থির শূন্য HandleNegateOverflow(bool isFatal, কাঠামো COverflow_তথ্য *p_তথ্য,
				 unsigned long ul_Old_মান);
স্থির শূন্য HandleBuiltinUnreachable(bool isFatal,
				     কাঠামো CUnreachable_তথ্য *p_তথ্য);
স্থির শূন্য HandleTypeMismatch(bool isFatal, কাঠামো CSourceLocation *mLocation,
			       কাঠামো CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer);
স্থির শূন্য HandleVlaBoundNotPositive(bool isFatal, কাঠামো CVLABound_তথ্য *p_তথ্য,
				      unsigned long ulBound);
স্থির শূন্য HandleOutOfBounds(bool isFatal, কাঠামো COut_Of_Bounds_তথ্য *p_তথ্য,
			      unsigned long ul_সূচক);
স্থির শূন্য HandleShiftOutOfBounds(bool isFatal,
				   কাঠামো CShift_Out_Of_Bounds_তথ্য *p_তথ্য,
				   unsigned long ulLHS, unsigned long ulRHS);
স্থির শূন্য Handle_Load_Invalid_মান(bool isFatal,
				   কাঠামো CInvalid_মান_তথ্য *p_তথ্য,
				   unsigned long ul_মান);
স্থির শূন্য HandleInvalidBuiltin(bool isFatal,
				 কাঠামো CInvalid_Builtin_তথ্য *p_তথ্য);
স্থির শূন্য HandleFunctionTypeMismatch(bool isFatal,
				       কাঠামো CFunction_Type_Mismatch_তথ্য *p_তথ্য,
				       unsigned long ulFunction);
স্থির শূন্য HandleMissingReturn(bool isFatal, কাঠামো CUnreachable_তথ্য *p_তথ্য);
স্থির শূন্য HandlePointerOverflow(bool isFatal,
				  কাঠামো CPointer_Overflow_তথ্য *p_তথ্য,
				  unsigned long ulBase, unsigned long ul_ফলাফল);
স্থির শূন্য HandleAlignmentAssumption(bool isFatal,
				      কাঠামো CAlignment_Assumption_তথ্য *p_তথ্য,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset);

#define সংখ্যা_SIGNED_BIT 1
#define সংখ্যা_MAXLEN 128
#define __arraycount(__a) (আকার(__a) / আকার(__a[0]))
#define __BIT(__n) (1UL << (__n))
#define SEPARATOR sbi_printf("===========================================\n")
#define ACK_REPORTED (1U << 31)

স্থির bool isAlreadyReported(কাঠামো CSourceLocation *pLocation)
{
	uint32_t si_Old_মান;
	অস্থির uint32_t *p_সারি;

	যদি (!pLocation)
		ফেরত false;

	p_সারি = &pLocation->m_সারি;

	করো {
		si_Old_মান = *p_সারি;
	} যখন (__sync_val_compare_and_swap(p_সারি, si_Old_মান,
					     si_Old_মান | ACK_REPORTED) !=
		 si_Old_মান);

	যদি (!(si_Old_মান & ACK_REPORTED)) {

		sbi_ubsan_report_গুনতি++;

		ফেরত false;
	}

	ফেরত true;
}

স্থির শূন্য HandleOverflow(bool isFatal, কাঠামো COverflow_তথ্য *p_তথ্য,
			   unsigned long ulLHS, unsigned long ulRHS,
			   ধ্রুবক অক্ষর *szOperation)
{
	যদি (!p_তথ্য) {
		ফেরত;
	}

	যদি (isAlreadyReported(&p_তথ্য->mLocation)) {
		ফেরত;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_তথ্য->mLocation.mFilename,
		   p_তথ্য->mLocation.m_সারি & ~ACK_REPORTED,
		   p_তথ্য->mLocation.mColumn);

	bool is_signed = p_তথ্য->mType->mTypeInfo & সংখ্যা_SIGNED_BIT;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	যদি (is_signed) {
		sbi_printf("%ld %s %ld ", (long)ulLHS, szOperation,
			   (long)ulRHS);
	} অন্যথা {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_তথ্য->mType->mTypeName);

	SEPARATOR;
}

স্থির শূন্য HandleNegateOverflow(bool isFatal, কাঠামো COverflow_তথ্য *p_তথ্য,
				 unsigned long ul_Old_মান)
{
	যদি (!p_তথ্য) {
		ফেরত;
	}
	যদি (isAlreadyReported(&p_তথ্য->mLocation)) {
		ফেরত;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_তথ্য->mLocation.mFilename,
		   p_তথ্য->mLocation.m_সারি & ~ACK_REPORTED,
		   p_তথ্য->mLocation.mColumn);

	bool is_signed = p_তথ্য->mType->mTypeInfo & সংখ্যা_SIGNED_BIT;

	sbi_printf("UBSan: Negation of ");

	যদি (is_signed) {
		sbi_printf("%ld", (long)ul_Old_মান);
	} অন্যথা {
		sbi_printf("%lu", ul_Old_মান);
	}

	sbi_printf("cannot be represented in type %s\n",
		   p_তথ্য->mType->mTypeName);

	SEPARATOR;
}

স্থির শূন্য HandleBuiltinUnreachable(bool isFatal,
				     কাঠামো CUnreachable_তথ্য *p_তথ্য)
{
	যদি (!p_তথ্য) {
		ফেরত;
	}
	যদি (isAlreadyReported(&p_তথ্য->mLocation)) {
		ফেরত;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_তথ্য->mLocation.mFilename,
		   p_তথ্য->mLocation.m_সারি & ~ACK_REPORTED,
		   p_তথ্য->mLocation.mColumn);

	SEPARATOR;
}

ধ্রুবক অক্ষর *rgczTypeCheckKinds[] = { "load of",
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

স্থির শূন্য HandleTypeMismatch(bool isFatal, কাঠামো CSourceLocation *mLocation,
			       কাঠামো CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer)
{

	যদি ((!mLocation) || (!mType)) {
		ফেরত;
	}

	যদি (isAlreadyReported(mLocation)) {
		ফেরত;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_সারি & ~ACK_REPORTED,
		   mLocation->mColumn);

	ধ্রুবক অক্ষর *kind = (mTypeCheckKind < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[mTypeCheckKind]
				   : "access to";

	যদি (ulPointer == 0) {
		sbi_printf("%s null pointer of type %s\n", kind,
			   mType->mTypeName);
	} অন্যথা যদি (
		(mLogAlignment - 1) &
		ulPointer) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			kind, (শূন্য *)ulPointer, mType->mTypeName,
			mLogAlignment);
	} অন্যথা {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			kind, (শূন্য *)ulPointer, mType->mTypeName);
	}
	SEPARATOR;
}

স্থির শূন্য HandleVlaBoundNotPositive(bool isFatal, কাঠামো CVLABound_তথ্য *p_তথ্য,
				      unsigned long ulBound)
{
	যদি (!p_তথ্য) {
		ফেরত;
	}
	যদি (isAlreadyReported(&p_তথ্য->mLocation)) {
		ফেরত;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_তথ্য->mLocation.mFilename,
		   p_তথ্য->mLocation.m_সারি & ~ACK_REPORTED,
		   p_তথ্য->mLocation.mColumn);

	sbi_printf("variable length array bound value ");

	bool is_signed = p_তথ্য->mType->mTypeInfo & সংখ্যা_SIGNED_BIT;

	যদি (is_signed) {
		sbi_printf("%ld", (long)ulBound);
	} অন্যথা {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

স্থির শূন্য HandleOutOfBounds(bool isFatal, কাঠামো COut_Of_Bounds_তথ্য *p_তথ্য,
			      unsigned long ul_সূচক)
{
	যদি (!p_তথ্য) {
		ফেরত;
	}
	যদি (isAlreadyReported(&p_তথ্য->mLocation)) {
		ফেরত;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_তথ্য->mLocation.mFilename,
		   p_তথ্য->mLocation.m_সারি & ~ACK_REPORTED,
		   p_তথ্য->mLocation.mColumn);

	bool is_signed = p_তথ্য->m_সূচক_Type->mTypeInfo & সংখ্যা_SIGNED_BIT;

	যদি (is_signed) {
		sbi_printf("index %ld", (long)ul_সূচক);
	} অন্যথা {
		sbi_printf("index %lu", ul_সূচক);
	}

	sbi_printf(" is out of range for type %s\n",
		   p_তথ্য->mArrayType->mTypeName);

	SEPARATOR;
}

স্থির bool is_Negative_সংখ্যা(কাঠামো CTypeDescriptor *pType, unsigned long ulVal)
{
	যদি (!(pType->mTypeInfo & সংখ্যা_SIGNED_BIT)) {
		ফেরত false;
	}

	ফেরত (long)ulVal < 0;
}

স্থির size_t type_width(কাঠামো CTypeDescriptor *pType)
{
	ফেরত 1UL << (pType->mTypeInfo >> 1);
}

স্থির শূন্য HandleShiftOutOfBounds(bool isFatal,
				   কাঠামো CShift_Out_Of_Bounds_তথ্য *p_তথ্য,
				   unsigned long ulLHS, unsigned long ulRHS)
{
	যদি (!p_তথ্য) {
		ফেরত;
	}

	যদি (isAlreadyReported(&p_তথ্য->mLocation)) {
		ফেরত;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_তথ্য->mLocation.mFilename,
		   p_তথ্য->mLocation.m_সারি & ~ACK_REPORTED,
		   p_তথ্য->mLocation.mColumn);

	যদি (is_Negative_সংখ্যা(p_তথ্য->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (long)ulRHS);
	} অন্যথা যদি (ulRHS >= type_width(p_তথ্য->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (unsigned long)type_width(p_তথ্য->mLHSType),
			p_তথ্য->mLHSType->mTypeName);

	} অন্যথা যদি (is_Negative_সংখ্যা(p_তথ্য->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (long)ulLHS);
	} অন্যথা {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, p_তথ্য->mLHSType->mTypeName);
	}

	SEPARATOR;
}

স্থির শূন্য Handle_Load_Invalid_মান(bool isFatal,
				   কাঠামো CInvalid_মান_তথ্য *p_তথ্য,
				   unsigned long ul_মান)
{
	যদি (!p_তথ্য) {
		ফেরত;
	}

	যদি (isAlreadyReported(&p_তথ্য->mLocation)) {
		ফেরত;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_তথ্য->mLocation.mFilename,
		   p_তথ্য->mLocation.m_সারি & ~ACK_REPORTED,
		   p_তথ্য->mLocation.mColumn);

	bool is_signed = p_তথ্য->mType->mTypeInfo & সংখ্যা_SIGNED_BIT;

	sbi_printf("load of value ");

	যদি (is_signed) {
		sbi_printf("%ld ", (long)ul_মান);
	} অন্যথা {
		sbi_printf("%lu ", ul_মান);
	}

	sbi_printf("is not a valid value for type %s\n",
		   p_তথ্য->mType->mTypeName);

	SEPARATOR;
}

ধ্রুবক অক্ষর *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

স্থির শূন্য HandleInvalidBuiltin(bool isFatal,
				 কাঠামো CInvalid_Builtin_তথ্য *p_তথ্য)
{
	যদি (!p_তথ্য) {
		ফেরত;
	}

	যদি (isAlreadyReported(&p_তথ্য->mLocation)) {
		ফেরত;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_তথ্য->mLocation.mFilename,
		   p_তথ্য->mLocation.m_সারি & ~ACK_REPORTED,
		   p_তথ্য->mLocation.mColumn);

	ধ্রুবক অক্ষর *builtin =
		(p_তথ্য->mKind < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[p_তথ্য->mKind]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

স্থির শূন্য HandleFunctionTypeMismatch(bool isFatal,
				       কাঠামো CFunction_Type_Mismatch_তথ্য *p_তথ্য,
				       unsigned long ulFunction)
{
	যদি (!p_তথ্য) {
		ফেরত;
	}

	যদি (isAlreadyReported(&p_তথ্য->mLocation)) {
		ফেরত;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_তথ্য->mLocation.mFilename,
		   p_তথ্য->mLocation.m_সারি & ~ACK_REPORTED,
		   p_তথ্য->mLocation.mColumn);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, p_তথ্য->mType->mTypeName);

	SEPARATOR;
}

স্থির শূন্য HandleMissingReturn(bool isFatal, কাঠামো CUnreachable_তথ্য *p_তথ্য)
{
	যদি (!p_তথ্য) {
		ফেরত;
	}

	যদি (isAlreadyReported(&p_তথ্য->mLocation)) {
		ফেরত;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_তথ্য->mLocation.mFilename,
		   p_তথ্য->mLocation.m_সারি & ~ACK_REPORTED,
		   p_তথ্য->mLocation.mColumn);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

স্থির শূন্য HandlePointerOverflow(bool isFatal,
				  কাঠামো CPointer_Overflow_তথ্য *p_তথ্য,
				  unsigned long ulBase, unsigned long ul_ফলাফল)
{
	যদি (!p_তথ্য) {
		ফেরত;
	}

	যদি (isAlreadyReported(&p_তথ্য->mLocation)) {
		ফেরত;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_তথ্য->mLocation.mFilename,
		   p_তথ্য->mLocation.m_সারি & ~ACK_REPORTED,
		   p_তথ্য->mLocation.mColumn);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ulBase, ul_ফলাফল);

	SEPARATOR;
}

স্থির শূন্য HandleAlignmentAssumption(bool isFatal,
				      কাঠামো CAlignment_Assumption_তথ্য *p_তথ্য,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset)
{

	যদি (!p_তথ্য) {
		ফেরত;
	}

	যদি (isAlreadyReported(&p_তথ্য->mLocation)) {
		ফেরত;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   p_তথ্য->mLocation.mFilename,
		   p_তথ্য->mLocation.m_সারি & ~ACK_REPORTED,
		   p_তথ্য->mLocation.mColumn);

	unsigned long ulRealPointer = ulPointer - ulOffset;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (শূন্য *)ulRealPointer, (শূন্য *)ulOffset);

	যদি (p_তথ্য->mAssumptionLocation.mFilename != ফাঁকা) {
		sbi_printf(", assumption made in %s:%u:%u",
			   p_তথ্য->mAssumptionLocation.mFilename,
			   p_তথ্য->mAssumptionLocation.m_সারি,
			   p_তথ্য->mAssumptionLocation.mColumn);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
শূন্য __ubsan_handle_add_overflow(কাঠামো COverflow_তথ্য *p_তথ্য,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, p_তথ্য, ulLHS, ulRHS, "+");
}

শূন্য __ubsan_handle_add_overflow_abort(কাঠামো COverflow_তথ্য *p_তথ্য,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, p_তথ্য, ulLHS, ulRHS, "+");
}

শূন্য __ubsan_handle_sub_overflow(কাঠামো COverflow_তথ্য *p_তথ্য,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, p_তথ্য, ulLHS, ulRHS, "-");
}

শূন্য __ubsan_handle_sub_overflow_abort(কাঠামো COverflow_তথ্য *p_তথ্য,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, p_তথ্য, ulLHS, ulRHS, "-");
}

শূন্য __ubsan_handle_mul_overflow(কাঠামো COverflow_তথ্য *p_তথ্য,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, p_তথ্য, ulLHS, ulRHS, "*");
}

শূন্য __ubsan_handle_mul_overflow_abort(কাঠামো COverflow_তথ্য *p_তথ্য,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, p_তথ্য, ulLHS, ulRHS, "*");
}

শূন্য __ubsan_handle_negate_overflow(কাঠামো COverflow_তথ্য *p_তথ্য,
				    unsigned long ul_Old_মান)
{
	HandleNegateOverflow(false, p_তথ্য, ul_Old_মান);
}

শূন্য __ubsan_handle_negate_overflow_abort(কাঠামো COverflow_তথ্য *p_তথ্য,
					  unsigned long ul_Old_মান)
{
	HandleNegateOverflow(true, p_তথ্য, ul_Old_মান);
}

শূন্য __ubsan_handle_divrem_overflow(কাঠামো COverflow_তথ্য *p_তথ্য,
				    unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, p_তথ্য, ulLHS, ulRHS, "divrem");
}

শূন্য __ubsan_handle_divrem_overflow_abort(কাঠামো COverflow_তথ্য *p_তথ্য,
					  unsigned long ulLHS,
					  unsigned long ulRHS)
{
	HandleOverflow(true, p_তথ্য, ulLHS, ulRHS, "divrem");
}

শূন্য __ubsan_handle_type_mismatch_v1(কাঠামো CType_Mismatch_তথ্য_v1 *p_তথ্য,
				     unsigned long ulPointer)
{
	HandleTypeMismatch(false, &p_তথ্য->mLocation, p_তথ্য->mType,
			   __BIT(p_তথ্য->mLogAlignment), p_তথ্য->mTypeCheckKind,
			   ulPointer);
}

শূন্য __ubsan_handle_type_mismatch_v1_abort(কাঠামো CType_Mismatch_তথ্য_v1 *p_তথ্য,
					   unsigned long ulPointer)
{
	HandleTypeMismatch(true, &p_তথ্য->mLocation, p_তথ্য->mType,
			   __BIT(p_তথ্য->mLogAlignment), p_তথ্য->mTypeCheckKind,
			   ulPointer);
}

শূন্য __ubsan_handle_out_of_bounds(কাঠামো COut_Of_Bounds_তথ্য *p_তথ্য,
				  unsigned long ul_সূচক)
{
	HandleOutOfBounds(false, p_তথ্য, ul_সূচক);
}

শূন্য __ubsan_handle_out_of_bounds_abort(কাঠামো COut_Of_Bounds_তথ্য *p_তথ্য,
					unsigned long ul_সূচক)
{
	HandleOutOfBounds(true, p_তথ্য, ul_সূচক);
}

শূন্য __ubsan_handle_shift_out_of_bounds(কাঠামো CShift_Out_Of_Bounds_তথ্য *p_তথ্য,
					unsigned long ulLHS,
					unsigned long ulRHS)
{
	HandleShiftOutOfBounds(false, p_তথ্য, ulLHS, ulRHS);
}

শূন্য __ubsan_handle_shift_out_of_bounds_abort(
	কাঠামো CShift_Out_Of_Bounds_তথ্য *p_তথ্য, unsigned long ulLHS,
	unsigned long ulRHS)
{
	HandleShiftOutOfBounds(true, p_তথ্য, ulLHS, ulRHS);
}

শূন্য __ubsan_handle_pointer_overflow(কাঠামো CPointer_Overflow_তথ্য *p_তথ্য,
				     unsigned long ulBase,
				     unsigned long ul_ফলাফল)
{
	HandlePointerOverflow(false, p_তথ্য, ulBase, ul_ফলাফল);
}

শূন্য __ubsan_handle_pointer_overflow_abort(কাঠামো CPointer_Overflow_তথ্য *p_তথ্য,
					   unsigned long ulBase,
					   unsigned long ul_ফলাফল)
{
	HandlePointerOverflow(true, p_তথ্য, ulBase, ul_ফলাফল);
}

শূন্য __ubsan_handle_alignment_assumption(কাঠামো CAlignment_Assumption_তথ্য *p_তথ্য,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset)
{
	HandleAlignmentAssumption(false, p_তথ্য, ulPointer, ulAlignment,
				  ulOffset);
}

শূন্য __ubsan_handle_alignment_assumption_abort(
	কাঠামো CAlignment_Assumption_তথ্য *p_তথ্য, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset)
{
	HandleAlignmentAssumption(true, p_তথ্য, ulPointer, ulAlignment,
				  ulOffset);
}

শূন্য __ubsan_handle_builtin_unreachable(কাঠামো CUnreachable_তথ্য *p_তথ্য)
{
	HandleBuiltinUnreachable(true, p_তথ্য);
}

শূন্য __ubsan_handle_invalid_builtin(কাঠামো CInvalid_Builtin_তথ্য *p_তথ্য)
{
	HandleInvalidBuiltin(true, p_তথ্য);
}

শূন্য __ubsan_handle_invalid_builtin_abort(কাঠামো CInvalid_Builtin_তথ্য *p_তথ্য)
{
	HandleInvalidBuiltin(true, p_তথ্য);
}

শূন্য __ubsan_handle_load_invalid_value(কাঠামো CInvalid_মান_তথ্য *p_তথ্য,
				       unsigned long ul_মান)
{
	Handle_Load_Invalid_মান(false, p_তথ্য, ul_মান);
}

শূন্য __ubsan_handle_load_invalid_value_abort(কাঠামো CInvalid_মান_তথ্য *p_তথ্য,
					     unsigned long ul_মান)
{
	Handle_Load_Invalid_মান(true, p_তথ্য, ul_মান);
}

শূন্য __ubsan_handle_missing_return(কাঠামো CUnreachable_তথ্য *p_তথ্য)
{
	HandleMissingReturn(true, p_তথ্য);
}

শূন্য __ubsan_handle_vla_bound_not_positive(কাঠামো CVLABound_তথ্য *p_তথ্য,
					   unsigned long ulBound)
{
	HandleVlaBoundNotPositive(false, p_তথ্য, ulBound);
}

শূন্য __ubsan_handle_vla_bound_not_positive_abort(কাঠামো CVLABound_তথ্য *p_তথ্য,
						 unsigned long ulBound)
{
	HandleVlaBoundNotPositive(true, p_তথ্য, ulBound);
}

শূন্য __ubsan_handle_function_type_mismatch(
	কাঠামো CFunction_Type_Mismatch_তথ্য *p_তথ্য, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(false, p_তথ্য, ulFunction);
}

শূন্য __ubsan_handle_function_type_mismatch_abort(
	কাঠামো CFunction_Type_Mismatch_তথ্য *p_তথ্য, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(true, p_তথ্য, ulFunction);
}
#endif
