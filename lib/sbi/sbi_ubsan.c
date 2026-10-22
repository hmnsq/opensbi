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

চঞ্চল unsigned long sbi_ubsan_report_গুনতি;

সংরচনা CSourceLocation {
	বর্ণ *mFilename;
	uint32_t m_সারি;
	uint32_t mColumn;
};

সংরচনা CTypeDescriptor {
	uint16_t mTypeKind;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

সংরচনা COverflowData {
	সংরচনা CSourceLocation mLocation;
	সংরচনা CTypeDescriptor *mType;
};

সংরচনা CUnreachableData {
	সংরচনা CSourceLocation mLocation;
};

সংরচনা CCFICheckFailData {
	uint8_t mCheckKind;
	সংরচনা CSourceLocation mLocation;
	সংরচনা CTypeDescriptor *mType;
};

সংরচনা CDynamicTypeCacheMissData {
	সংরচনা CSourceLocation mLocation;
	সংরচনা CTypeDescriptor *mType;
	শূন্য *mTypeInfo;
	uint8_t mTypeCheckKind;
};

সংরচনা CFunctionTypeMismatchData {
	সংরচনা CSourceLocation mLocation;
	সংরচনা CTypeDescriptor *mType;
};

সংরচনা CInvalidBuiltinData {
	সংরচনা CSourceLocation mLocation;
	uint8_t mKind;
};

সংরচনা CInvalid_মান_Data {
	সংরচনা CSourceLocation mLocation;
	সংরচনা CTypeDescriptor *mType;
};

সংরচনা CNonNullArgData {
	সংরচনা CSourceLocation mLocation;
	সংরচনা CSourceLocation mAttributeLocation;
	পূর্ণাঙ্ক mArgIndex;
};

সংরচনা CNonNullReturnData {
	সংরচনা CSourceLocation mAttributeLocation;
};

সংরচনা COutOfBoundsData {
	সংরচনা CSourceLocation mLocation;
	সংরচনা CTypeDescriptor *mArrayType;
	সংরচনা CTypeDescriptor *mIndexType;
};

সংরচনা CPointerOverflowData {
	সংরচনা CSourceLocation mLocation;
};

সংরচনা CShiftOutOfBoundsData {
	সংরচনা CSourceLocation mLocation;
	সংরচনা CTypeDescriptor *mLHSType;
	সংরচনা CTypeDescriptor *mRHSType;
};

সংরচনা CTypeMismatchData {
	সংরচনা CSourceLocation mLocation;
	সংরচনা CTypeDescriptor *mType;
	unsigned long mLogAlignment;
	uint8_t mTypeCheckKind;
};

সংরচনা CTypeMismatchData_v1 {
	সংরচনা CSourceLocation mLocation;
	সংরচনা CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t mTypeCheckKind;
};

সংরচনা CVLABoundData {
	সংরচনা CSourceLocation mLocation;
	সংরচনা CTypeDescriptor *mType;
};

সংরচনা CFloatCastOverflowData {
	সংরচনা CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	সংরচনা CTypeDescriptor *mFromType;
	সংরচনা CTypeDescriptor *mToType;
};

সংরচনা CImplicitConversionData {
	সংরচনা CSourceLocation mLocation;
	সংরচনা CTypeDescriptor *mFromType;
	সংরচনা CTypeDescriptor *mToType;
	uint8_t mKind;
};

সংরচনা CAlignmentAssumptionData {
	সংরচনা CSourceLocation mLocation;
	সংরচনা CSourceLocation mAssumptionLocation;
	সংরচনা CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
শূন্য __ubsan_handle_add_overflow(সংরচনা COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
শূন্য __ubsan_handle_add_overflow_abort(সংরচনা COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
শূন্য __ubsan_handle_alignment_assumption(সংরচনা CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset);
শূন্য __ubsan_handle_alignment_assumption_abort(
	সংরচনা CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset);
শূন্য __ubsan_handle_builtin_unreachable(সংরচনা CUnreachableData *pData);
শূন্য __ubsan_handle_divrem_overflow(সংরচনা COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS);
শূন্য __ubsan_handle_divrem_overflow_abort(সংরচনা COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS);
শূন্য __ubsan_handle_function_type_mismatch(
	সংরচনা CFunctionTypeMismatchData *pData, unsigned long ulFunction);
শূন্য __ubsan_handle_function_type_mismatch_abort(
	সংরচনা CFunctionTypeMismatchData *pData, unsigned long ulFunction);
শূন্য __ubsan_handle_function_type_mismatch_v1(
	সংরচনা CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
শূন্য __ubsan_handle_function_type_mismatch_v1_abort(
	সংরচনা CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
শূন্য __ubsan_handle_invalid_builtin(সংরচনা CInvalidBuiltinData *pData);
শূন্য __ubsan_handle_invalid_builtin_abort(সংরচনা CInvalidBuiltinData *pData);
শূন্য __ubsan_handle_load_invalid_value(সংরচনা CInvalid_মান_Data *pData,
				       unsigned long ulVal);
শূন্য __ubsan_handle_load_invalid_value_abort(সংরচনা CInvalid_মান_Data *pData,
					     unsigned long ulVal);
শূন্য __ubsan_handle_missing_return(সংরচনা CUnreachableData *pData);
শূন্য __ubsan_handle_mul_overflow(সংরচনা COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
শূন্য __ubsan_handle_mul_overflow_abort(সংরচনা COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
শূন্য __ubsan_handle_negate_overflow(সংরচনা COverflowData *pData,
				    unsigned long ulOldVal);
শূন্য __ubsan_handle_negate_overflow_abort(সংরচনা COverflowData *pData,
					  unsigned long ulOldVal);
শূন্য __ubsan_handle_nullability_arg(সংরচনা CNonNullArgData *pData);
শূন্য __ubsan_handle_nullability_arg_abort(সংরচনা CNonNullArgData *pData);
শূন্য __ubsan_handle_nullability_return_v1(
	সংরচনা CNonNullReturnData *pData,
	সংরচনা CSourceLocation *pLocationPointer);
শূন্য __ubsan_handle_nullability_return_v1_abort(
	সংরচনা CNonNullReturnData *pData,
	সংরচনা CSourceLocation *pLocationPointer);
শূন্য __ubsan_handle_out_of_bounds(সংরচনা COutOfBoundsData *pData,
				  unsigned long ulIndex);
শূন্য __ubsan_handle_out_of_bounds_abort(সংরচনা COutOfBoundsData *pData,
					unsigned long ulIndex);
শূন্য __ubsan_handle_pointer_overflow(সংরচনা CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult);
শূন্য __ubsan_handle_pointer_overflow_abort(সংরচনা CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult);
শূন্য __ubsan_handle_shift_out_of_bounds(সংরচনা CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS);
শূন্য __ubsan_handle_shift_out_of_bounds_abort(
	সংরচনা CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS);
শূন্য __ubsan_handle_sub_overflow(সংরচনা COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
শূন্য __ubsan_handle_sub_overflow_abort(সংরচনা COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
শূন্য __ubsan_handle_type_mismatch(সংরচনা CTypeMismatchData *pData,
				  unsigned long ulPointer);
শূন্য __ubsan_handle_type_mismatch_abort(সংরচনা CTypeMismatchData *pData,
					unsigned long ulPointer);
শূন্য __ubsan_handle_type_mismatch_v1(সংরচনা CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer);
শূন্য __ubsan_handle_type_mismatch_v1_abort(সংরচনা CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer);
শূন্য __ubsan_handle_vla_bound_not_positive(সংরচনা CVLABoundData *pData,
					   unsigned long ulBound);
শূন্য __ubsan_handle_vla_bound_not_positive_abort(সংরচনা CVLABoundData *pData,
						 unsigned long ulBound);
শূন্য __ubsan_get_current_report_data(ধ্রুব বর্ণ **ppOutIssueKind,
				     ধ্রুব বর্ণ **ppOutMessage,
				     ধ্রুব বর্ণ **ppOutFilename,
				     uint32_t *p_Out_সারি, uint32_t *pOutCol,
				     বর্ণ **ppOutMemoryAddr);
স্থির শূন্য HandleOverflow(bool isFatal, সংরচনা COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   ধ্রুব বর্ণ *szOperation);
স্থির শূন্য HandleNegateOverflow(bool isFatal, সংরচনা COverflowData *pData,
				 unsigned long ul_Old_মান);
স্থির শূন্য HandleBuiltinUnreachable(bool isFatal,
				     সংরচনা CUnreachableData *pData);
স্থির শূন্য HandleTypeMismatch(bool isFatal, সংরচনা CSourceLocation *mLocation,
			       সংরচনা CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer);
স্থির শূন্য HandleVlaBoundNotPositive(bool isFatal, সংরচনা CVLABoundData *pData,
				      unsigned long ulBound);
স্থির শূন্য HandleOutOfBounds(bool isFatal, সংরচনা COutOfBoundsData *pData,
			      unsigned long ulIndex);
স্থির শূন্য HandleShiftOutOfBounds(bool isFatal,
				   সংরচনা CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS);
স্থির শূন্য Handle_Load_Invalid_মান(bool isFatal,
				   সংরচনা CInvalid_মান_Data *pData,
				   unsigned long ul_মান);
স্থির শূন্য HandleInvalidBuiltin(bool isFatal,
				 সংরচনা CInvalidBuiltinData *pData);
স্থির শূন্য HandleFunctionTypeMismatch(bool isFatal,
				       সংরচনা CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction);
স্থির শূন্য HandleMissingReturn(bool isFatal, সংরচনা CUnreachableData *pData);
স্থির শূন্য HandlePointerOverflow(bool isFatal,
				  সংরচনা CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult);
স্থির শূন্য HandleAlignmentAssumption(bool isFatal,
				      সংরচনা CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset);

#define সংখ্যা_SIGNED_BIT 1
#define সংখ্যা_MAXLEN 128
#define __arraycount(__a) (পরিমাণ(__a) / পরিমাণ(__a[0]))
#define __BIT(__n) (1UL << (__n))
#define SEPARATOR sbi_printf("===========================================\n")
#define ACK_REPORTED (1U << 31)

স্থির bool isAlreadyReported(সংরচনা CSourceLocation *pLocation)
{
	uint32_t si_Old_মান;
	চঞ্চল uint32_t *p_সারি;

	যদি (!pLocation)
		প্রত্যাগম false;

	p_সারি = &pLocation->m_সারি;

	কুরু {
		si_Old_মান = *p_সারি;
	} যাবত্ (__sync_val_compare_and_swap(p_সারি, si_Old_মান,
					     si_Old_মান | ACK_REPORTED) !=
		 si_Old_মান);

	যদি (!(si_Old_মান & ACK_REPORTED)) {

		sbi_ubsan_report_গুনতি++;

		প্রত্যাগম false;
	}

	প্রত্যাগম true;
}

স্থির শূন্য HandleOverflow(bool isFatal, সংরচনা COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   ধ্রুব বর্ণ *szOperation)
{
	যদি (!pData) {
		প্রত্যাগম;
	}

	যদি (isAlreadyReported(&pData->mLocation)) {
		প্রত্যাগম;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_সারি & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & সংখ্যা_SIGNED_BIT;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	যদি (is_signed) {
		sbi_printf("%ld %s %ld ", (long)ulLHS, szOperation,
			   (long)ulRHS);
	} অন্যথা {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

স্থির শূন্য HandleNegateOverflow(bool isFatal, সংরচনা COverflowData *pData,
				 unsigned long ul_Old_মান)
{
	যদি (!pData) {
		প্রত্যাগম;
	}
	যদি (isAlreadyReported(&pData->mLocation)) {
		প্রত্যাগম;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_সারি & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & সংখ্যা_SIGNED_BIT;

	sbi_printf("UBSan: Negation of ");

	যদি (is_signed) {
		sbi_printf("%ld", (long)ul_Old_মান);
	} অন্যথা {
		sbi_printf("%lu", ul_Old_মান);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

স্থির শূন্য HandleBuiltinUnreachable(bool isFatal,
				     সংরচনা CUnreachableData *pData)
{
	যদি (!pData) {
		প্রত্যাগম;
	}
	যদি (isAlreadyReported(&pData->mLocation)) {
		প্রত্যাগম;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_সারি & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	SEPARATOR;
}

ধ্রুব বর্ণ *rgczTypeCheckKinds[] = { "load of",
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

স্থির শূন্য HandleTypeMismatch(bool isFatal, সংরচনা CSourceLocation *mLocation,
			       সংরচনা CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer)
{

	যদি ((!mLocation) || (!mType)) {
		প্রত্যাগম;
	}

	যদি (isAlreadyReported(mLocation)) {
		প্রত্যাগম;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_সারি & ~ACK_REPORTED,
		   mLocation->mColumn);

	ধ্রুব বর্ণ *kind = (mTypeCheckKind < __arraycount(rgczTypeCheckKinds))
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

স্থির শূন্য HandleVlaBoundNotPositive(bool isFatal, সংরচনা CVLABoundData *pData,
				      unsigned long ulBound)
{
	যদি (!pData) {
		প্রত্যাগম;
	}
	যদি (isAlreadyReported(&pData->mLocation)) {
		প্রত্যাগম;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_সারি & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("variable length array bound value ");

	bool is_signed = pData->mType->mTypeInfo & সংখ্যা_SIGNED_BIT;

	যদি (is_signed) {
		sbi_printf("%ld", (long)ulBound);
	} অন্যথা {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

স্থির শূন্য HandleOutOfBounds(bool isFatal, সংরচনা COutOfBoundsData *pData,
			      unsigned long ulIndex)
{
	যদি (!pData) {
		প্রত্যাগম;
	}
	যদি (isAlreadyReported(&pData->mLocation)) {
		প্রত্যাগম;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_সারি & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mIndexType->mTypeInfo & সংখ্যা_SIGNED_BIT;

	যদি (is_signed) {
		sbi_printf("index %ld", (long)ulIndex);
	} অন্যথা {
		sbi_printf("index %lu", ulIndex);
	}

	sbi_printf(" is out of range for type %s\n",
		   pData->mArrayType->mTypeName);

	SEPARATOR;
}

স্থির bool is_Negative_সংখ্যা(সংরচনা CTypeDescriptor *pType, unsigned long ulVal)
{
	যদি (!(pType->mTypeInfo & সংখ্যা_SIGNED_BIT)) {
		প্রত্যাগম false;
	}

	প্রত্যাগম (long)ulVal < 0;
}

স্থির size_t type_width(সংরচনা CTypeDescriptor *pType)
{
	প্রত্যাগম 1UL << (pType->mTypeInfo >> 1);
}

স্থির শূন্য HandleShiftOutOfBounds(bool isFatal,
				   সংরচনা CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS)
{
	যদি (!pData) {
		প্রত্যাগম;
	}

	যদি (isAlreadyReported(&pData->mLocation)) {
		প্রত্যাগম;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_সারি & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	যদি (is_Negative_সংখ্যা(pData->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (long)ulRHS);
	} অন্যথা যদি (ulRHS >= type_width(pData->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (unsigned long)type_width(pData->mLHSType),
			pData->mLHSType->mTypeName);

	} অন্যথা যদি (is_Negative_সংখ্যা(pData->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (long)ulLHS);
	} অন্যথা {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, pData->mLHSType->mTypeName);
	}

	SEPARATOR;
}

স্থির শূন্য Handle_Load_Invalid_মান(bool isFatal,
				   সংরচনা CInvalid_মান_Data *pData,
				   unsigned long ul_মান)
{
	যদি (!pData) {
		প্রত্যাগম;
	}

	যদি (isAlreadyReported(&pData->mLocation)) {
		প্রত্যাগম;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_সারি & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & সংখ্যা_SIGNED_BIT;

	sbi_printf("load of value ");

	যদি (is_signed) {
		sbi_printf("%ld ", (long)ul_মান);
	} অন্যথা {
		sbi_printf("%lu ", ul_মান);
	}

	sbi_printf("is not a valid value for type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

ধ্রুব বর্ণ *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

স্থির শূন্য HandleInvalidBuiltin(bool isFatal,
				 সংরচনা CInvalidBuiltinData *pData)
{
	যদি (!pData) {
		প্রত্যাগম;
	}

	যদি (isAlreadyReported(&pData->mLocation)) {
		প্রত্যাগম;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_সারি & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	ধ্রুব বর্ণ *builtin =
		(pData->mKind < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[pData->mKind]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

স্থির শূন্য HandleFunctionTypeMismatch(bool isFatal,
				       সংরচনা CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction)
{
	যদি (!pData) {
		প্রত্যাগম;
	}

	যদি (isAlreadyReported(&pData->mLocation)) {
		প্রত্যাগম;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_সারি & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, pData->mType->mTypeName);

	SEPARATOR;
}

স্থির শূন্য HandleMissingReturn(bool isFatal, সংরচনা CUnreachableData *pData)
{
	যদি (!pData) {
		প্রত্যাগম;
	}

	যদি (isAlreadyReported(&pData->mLocation)) {
		প্রত্যাগম;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_সারি & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

স্থির শূন্য HandlePointerOverflow(bool isFatal,
				  সংরচনা CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult)
{
	যদি (!pData) {
		প্রত্যাগম;
	}

	যদি (isAlreadyReported(&pData->mLocation)) {
		প্রত্যাগম;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_সারি & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ulBase, ulResult);

	SEPARATOR;
}

স্থির শূন্য HandleAlignmentAssumption(bool isFatal,
				      সংরচনা CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset)
{

	যদি (!pData) {
		প্রত্যাগম;
	}

	যদি (isAlreadyReported(&pData->mLocation)) {
		প্রত্যাগম;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_সারি & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	unsigned long ulRealPointer = ulPointer - ulOffset;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (শূন্য *)ulRealPointer, (শূন্য *)ulOffset);

	যদি (pData->mAssumptionLocation.mFilename != রিক্ত) {
		sbi_printf(", assumption made in %s:%u:%u",
			   pData->mAssumptionLocation.mFilename,
			   pData->mAssumptionLocation.m_সারি,
			   pData->mAssumptionLocation.mColumn);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
শূন্য __ubsan_handle_add_overflow(সংরচনা COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "+");
}

শূন্য __ubsan_handle_add_overflow_abort(সংরচনা COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "+");
}

শূন্য __ubsan_handle_sub_overflow(সংরচনা COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "-");
}

শূন্য __ubsan_handle_sub_overflow_abort(সংরচনা COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "-");
}

শূন্য __ubsan_handle_mul_overflow(সংরচনা COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "*");
}

শূন্য __ubsan_handle_mul_overflow_abort(সংরচনা COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "*");
}

শূন্য __ubsan_handle_negate_overflow(সংরচনা COverflowData *pData,
				    unsigned long ul_Old_মান)
{
	HandleNegateOverflow(false, pData, ul_Old_মান);
}

শূন্য __ubsan_handle_negate_overflow_abort(সংরচনা COverflowData *pData,
					  unsigned long ul_Old_মান)
{
	HandleNegateOverflow(true, pData, ul_Old_মান);
}

শূন্য __ubsan_handle_divrem_overflow(সংরচনা COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "divrem");
}

শূন্য __ubsan_handle_divrem_overflow_abort(সংরচনা COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "divrem");
}

শূন্য __ubsan_handle_type_mismatch_v1(সংরচনা CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer)
{
	HandleTypeMismatch(false, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

শূন্য __ubsan_handle_type_mismatch_v1_abort(সংরচনা CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer)
{
	HandleTypeMismatch(true, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

শূন্য __ubsan_handle_out_of_bounds(সংরচনা COutOfBoundsData *pData,
				  unsigned long ulIndex)
{
	HandleOutOfBounds(false, pData, ulIndex);
}

শূন্য __ubsan_handle_out_of_bounds_abort(সংরচনা COutOfBoundsData *pData,
					unsigned long ulIndex)
{
	HandleOutOfBounds(true, pData, ulIndex);
}

শূন্য __ubsan_handle_shift_out_of_bounds(সংরচনা CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS)
{
	HandleShiftOutOfBounds(false, pData, ulLHS, ulRHS);
}

শূন্য __ubsan_handle_shift_out_of_bounds_abort(
	সংরচনা CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS)
{
	HandleShiftOutOfBounds(true, pData, ulLHS, ulRHS);
}

শূন্য __ubsan_handle_pointer_overflow(সংরচনা CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult)
{
	HandlePointerOverflow(false, pData, ulBase, ulResult);
}

শূন্য __ubsan_handle_pointer_overflow_abort(সংরচনা CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult)
{
	HandlePointerOverflow(true, pData, ulBase, ulResult);
}

শূন্য __ubsan_handle_alignment_assumption(সংরচনা CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset)
{
	HandleAlignmentAssumption(false, pData, ulPointer, ulAlignment,
				  ulOffset);
}

শূন্য __ubsan_handle_alignment_assumption_abort(
	সংরচনা CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset)
{
	HandleAlignmentAssumption(true, pData, ulPointer, ulAlignment,
				  ulOffset);
}

শূন্য __ubsan_handle_builtin_unreachable(সংরচনা CUnreachableData *pData)
{
	HandleBuiltinUnreachable(true, pData);
}

শূন্য __ubsan_handle_invalid_builtin(সংরচনা CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

শূন্য __ubsan_handle_invalid_builtin_abort(সংরচনা CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

শূন্য __ubsan_handle_load_invalid_value(সংরচনা CInvalid_মান_Data *pData,
				       unsigned long ul_মান)
{
	Handle_Load_Invalid_মান(false, pData, ul_মান);
}

শূন্য __ubsan_handle_load_invalid_value_abort(সংরচনা CInvalid_মান_Data *pData,
					     unsigned long ul_মান)
{
	Handle_Load_Invalid_মান(true, pData, ul_মান);
}

শূন্য __ubsan_handle_missing_return(সংরচনা CUnreachableData *pData)
{
	HandleMissingReturn(true, pData);
}

শূন্য __ubsan_handle_vla_bound_not_positive(সংরচনা CVLABoundData *pData,
					   unsigned long ulBound)
{
	HandleVlaBoundNotPositive(false, pData, ulBound);
}

শূন্য __ubsan_handle_vla_bound_not_positive_abort(সংরচনা CVLABoundData *pData,
						 unsigned long ulBound)
{
	HandleVlaBoundNotPositive(true, pData, ulBound);
}

শূন্য __ubsan_handle_function_type_mismatch(
	সংরচনা CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(false, pData, ulFunction);
}

শূন্য __ubsan_handle_function_type_mismatch_abort(
	সংরচনা CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(true, pData, ulFunction);
}
#endif
