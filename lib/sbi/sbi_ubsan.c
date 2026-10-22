/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ifdef UBSAN_ENABLED

#અન્તર્ભાવ <sbi/sbi_types.h>
#અન્તર્ભાવ <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#define KIND_INTEGER 0
#define KIND_FLOAT 1
#define KIND_UNKNOWN UINT16_MAX

અસ્થિર unsigned long sbi_ubsan_report_ગણતરી;

સંરચના CSourceLocation {
	અક્ષર *mFilename;
	uint32_t m_લીટી;
	uint32_t mColumn;
};

સંરચના CTypeDescriptor {
	uint16_t mTypeKind;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

સંરચના COverflowData {
	સંરચના CSourceLocation mLocation;
	સંરચના CTypeDescriptor *mType;
};

સંરચના CUnreachableData {
	સંરચના CSourceLocation mLocation;
};

સંરચના CCFICheckFailData {
	uint8_t mCheckKind;
	સંરચના CSourceLocation mLocation;
	સંરચના CTypeDescriptor *mType;
};

સંરચના CDynamicTypeCacheMissData {
	સંરચના CSourceLocation mLocation;
	સંરચના CTypeDescriptor *mType;
	શૂન્ય *mTypeInfo;
	uint8_t mTypeCheckKind;
};

સંરચના CFunctionTypeMismatchData {
	સંરચના CSourceLocation mLocation;
	સંરચના CTypeDescriptor *mType;
};

સંરચના CInvalidBuiltinData {
	સંરચના CSourceLocation mLocation;
	uint8_t mKind;
};

સંરચના CInvalid_કિંમત_Data {
	સંરચના CSourceLocation mLocation;
	સંરચના CTypeDescriptor *mType;
};

સંરચના CNonNullArgData {
	સંરચના CSourceLocation mLocation;
	સંરચના CSourceLocation mAttributeLocation;
	પૂર્ણાંક mArgIndex;
};

સંરચના CNonNullReturnData {
	સંરચના CSourceLocation mAttributeLocation;
};

સંરચના COutOfBoundsData {
	સંરચના CSourceLocation mLocation;
	સંરચના CTypeDescriptor *mArrayType;
	સંરચના CTypeDescriptor *mIndexType;
};

સંરચના CPointerOverflowData {
	સંરચના CSourceLocation mLocation;
};

સંરચના CShiftOutOfBoundsData {
	સંરચના CSourceLocation mLocation;
	સંરચના CTypeDescriptor *mLHSType;
	સંરચના CTypeDescriptor *mRHSType;
};

સંરચના CTypeMismatchData {
	સંરચના CSourceLocation mLocation;
	સંરચના CTypeDescriptor *mType;
	unsigned long mLogAlignment;
	uint8_t mTypeCheckKind;
};

સંરચના CTypeMismatchData_v1 {
	સંરચના CSourceLocation mLocation;
	સંરચના CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t mTypeCheckKind;
};

સંરચના CVLABoundData {
	સંરચના CSourceLocation mLocation;
	સંરચના CTypeDescriptor *mType;
};

સંરચના CFloatCastOverflowData {
	સંરચના CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	સંરચના CTypeDescriptor *mFromType;
	સંરચના CTypeDescriptor *mToType;
};

સંરચના CImplicitConversionData {
	સંરચના CSourceLocation mLocation;
	સંરચના CTypeDescriptor *mFromType;
	સંરચના CTypeDescriptor *mToType;
	uint8_t mKind;
};

સંરચના CAlignmentAssumptionData {
	સંરચના CSourceLocation mLocation;
	સંરચના CSourceLocation mAssumptionLocation;
	સંરચના CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
શૂન્ય __ubsan_handle_add_overflow(સંરચના COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
શૂન્ય __ubsan_handle_add_overflow_abort(સંરચના COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
શૂન્ય __ubsan_handle_alignment_assumption(સંરચના CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset);
શૂન્ય __ubsan_handle_alignment_assumption_abort(
	સંરચના CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset);
શૂન્ય __ubsan_handle_builtin_unreachable(સંરચના CUnreachableData *pData);
શૂન્ય __ubsan_handle_divrem_overflow(સંરચના COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS);
શૂન્ય __ubsan_handle_divrem_overflow_abort(સંરચના COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS);
શૂન્ય __ubsan_handle_function_type_mismatch(
	સંરચના CFunctionTypeMismatchData *pData, unsigned long ulFunction);
શૂન્ય __ubsan_handle_function_type_mismatch_abort(
	સંરચના CFunctionTypeMismatchData *pData, unsigned long ulFunction);
શૂન્ય __ubsan_handle_function_type_mismatch_v1(
	સંરચના CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
શૂન્ય __ubsan_handle_function_type_mismatch_v1_abort(
	સંરચના CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
શૂન્ય __ubsan_handle_invalid_builtin(સંરચના CInvalidBuiltinData *pData);
શૂન્ય __ubsan_handle_invalid_builtin_abort(સંરચના CInvalidBuiltinData *pData);
શૂન્ય __ubsan_handle_load_invalid_value(સંરચના CInvalid_કિંમત_Data *pData,
				       unsigned long ulVal);
શૂન્ય __ubsan_handle_load_invalid_value_abort(સંરચના CInvalid_કિંમત_Data *pData,
					     unsigned long ulVal);
શૂન્ય __ubsan_handle_missing_return(સંરચના CUnreachableData *pData);
શૂન્ય __ubsan_handle_mul_overflow(સંરચના COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
શૂન્ય __ubsan_handle_mul_overflow_abort(સંરચના COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
શૂન્ય __ubsan_handle_negate_overflow(સંરચના COverflowData *pData,
				    unsigned long ulOldVal);
શૂન્ય __ubsan_handle_negate_overflow_abort(સંરચના COverflowData *pData,
					  unsigned long ulOldVal);
શૂન્ય __ubsan_handle_nullability_arg(સંરચના CNonNullArgData *pData);
શૂન્ય __ubsan_handle_nullability_arg_abort(સંરચના CNonNullArgData *pData);
શૂન્ય __ubsan_handle_nullability_return_v1(
	સંરચના CNonNullReturnData *pData,
	સંરચના CSourceLocation *pLocationPointer);
શૂન્ય __ubsan_handle_nullability_return_v1_abort(
	સંરચના CNonNullReturnData *pData,
	સંરચના CSourceLocation *pLocationPointer);
શૂન્ય __ubsan_handle_out_of_bounds(સંરચના COutOfBoundsData *pData,
				  unsigned long ulIndex);
શૂન્ય __ubsan_handle_out_of_bounds_abort(સંરચના COutOfBoundsData *pData,
					unsigned long ulIndex);
શૂન્ય __ubsan_handle_pointer_overflow(સંરચના CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult);
શૂન્ય __ubsan_handle_pointer_overflow_abort(સંરચના CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult);
શૂન્ય __ubsan_handle_shift_out_of_bounds(સંરચના CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS);
શૂન્ય __ubsan_handle_shift_out_of_bounds_abort(
	સંરચના CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS);
શૂન્ય __ubsan_handle_sub_overflow(સંરચના COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
શૂન્ય __ubsan_handle_sub_overflow_abort(સંરચના COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
શૂન્ય __ubsan_handle_type_mismatch(સંરચના CTypeMismatchData *pData,
				  unsigned long ulPointer);
શૂન્ય __ubsan_handle_type_mismatch_abort(સંરચના CTypeMismatchData *pData,
					unsigned long ulPointer);
શૂન્ય __ubsan_handle_type_mismatch_v1(સંરચના CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer);
શૂન્ય __ubsan_handle_type_mismatch_v1_abort(સંરચના CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer);
શૂન્ય __ubsan_handle_vla_bound_not_positive(સંરચના CVLABoundData *pData,
					   unsigned long ulBound);
શૂન્ય __ubsan_handle_vla_bound_not_positive_abort(સંરચના CVLABoundData *pData,
						 unsigned long ulBound);
શૂન્ય __ubsan_get_current_report_data(અચલ અક્ષર **ppOutIssueKind,
				     અચલ અક્ષર **ppOutMessage,
				     અચલ અક્ષર **ppOutFilename,
				     uint32_t *p_Out_લીટી, uint32_t *pOutCol,
				     અક્ષર **ppOutMemoryAddr);
સ્થિર શૂન્ય HandleOverflow(bool isFatal, સંરચના COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   અચલ અક્ષર *szOperation);
સ્થિર શૂન્ય HandleNegateOverflow(bool isFatal, સંરચના COverflowData *pData,
				 unsigned long ul_Old_કિંમત);
સ્થિર શૂન્ય HandleBuiltinUnreachable(bool isFatal,
				     સંરચના CUnreachableData *pData);
સ્થિર શૂન્ય HandleTypeMismatch(bool isFatal, સંરચના CSourceLocation *mLocation,
			       સંરચના CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer);
સ્થિર શૂન્ય HandleVlaBoundNotPositive(bool isFatal, સંરચના CVLABoundData *pData,
				      unsigned long ulBound);
સ્થિર શૂન્ય HandleOutOfBounds(bool isFatal, સંરચના COutOfBoundsData *pData,
			      unsigned long ulIndex);
સ્થિર શૂન્ય HandleShiftOutOfBounds(bool isFatal,
				   સંરચના CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS);
સ્થિર શૂન્ય Handle_Load_Invalid_કિંમત(bool isFatal,
				   સંરચના CInvalid_કિંમત_Data *pData,
				   unsigned long ul_કિંમત);
સ્થિર શૂન્ય HandleInvalidBuiltin(bool isFatal,
				 સંરચના CInvalidBuiltinData *pData);
સ્થિર શૂન્ય HandleFunctionTypeMismatch(bool isFatal,
				       સંરચના CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction);
સ્થિર શૂન્ય HandleMissingReturn(bool isFatal, સંરચના CUnreachableData *pData);
સ્થિર શૂન્ય HandlePointerOverflow(bool isFatal,
				  સંરચના CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult);
સ્થિર શૂન્ય HandleAlignmentAssumption(bool isFatal,
				      સંરચના CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset);

#define સંખ્યા_SIGNED_BIT 1
#define સંખ્યા_MAXLEN 128
#define __arraycount(__a) (કદ(__a) / કદ(__a[0]))
#define __BIT(__n) (1UL << (__n))
#define SEPARATOR sbi_printf("===========================================\n")
#define ACK_REPORTED (1U << 31)

સ્થિર bool isAlreadyReported(સંરચના CSourceLocation *pLocation)
{
	uint32_t si_Old_કિંમત;
	અસ્થિર uint32_t *p_લીટી;

	જો (!pLocation)
		પાછા false;

	p_લીટી = &pLocation->m_લીટી;

	કરો {
		si_Old_કિંમત = *p_લીટી;
	} જ્યારે (__sync_val_compare_and_swap(p_લીટી, si_Old_કિંમત,
					     si_Old_કિંમત | ACK_REPORTED) !=
		 si_Old_કિંમત);

	જો (!(si_Old_કિંમત & ACK_REPORTED)) {

		sbi_ubsan_report_ગણતરી++;

		પાછા false;
	}

	પાછા true;
}

સ્થિર શૂન્ય HandleOverflow(bool isFatal, સંરચના COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   અચલ અક્ષર *szOperation)
{
	જો (!pData) {
		પાછા;
	}

	જો (isAlreadyReported(&pData->mLocation)) {
		પાછા;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_લીટી & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & સંખ્યા_SIGNED_BIT;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	જો (is_signed) {
		sbi_printf("%ld %s %ld ", (long)ulLHS, szOperation,
			   (long)ulRHS);
	} નહીંતર {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

સ્થિર શૂન્ય HandleNegateOverflow(bool isFatal, સંરચના COverflowData *pData,
				 unsigned long ul_Old_કિંમત)
{
	જો (!pData) {
		પાછા;
	}
	જો (isAlreadyReported(&pData->mLocation)) {
		પાછા;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_લીટી & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & સંખ્યા_SIGNED_BIT;

	sbi_printf("UBSan: Negation of ");

	જો (is_signed) {
		sbi_printf("%ld", (long)ul_Old_કિંમત);
	} નહીંતર {
		sbi_printf("%lu", ul_Old_કિંમત);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

સ્થિર શૂન્ય HandleBuiltinUnreachable(bool isFatal,
				     સંરચના CUnreachableData *pData)
{
	જો (!pData) {
		પાછા;
	}
	જો (isAlreadyReported(&pData->mLocation)) {
		પાછા;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_લીટી & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	SEPARATOR;
}

અચલ અક્ષર *rgczTypeCheckKinds[] = { "load of",
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

સ્થિર શૂન્ય HandleTypeMismatch(bool isFatal, સંરચના CSourceLocation *mLocation,
			       સંરચના CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer)
{

	જો ((!mLocation) || (!mType)) {
		પાછા;
	}

	જો (isAlreadyReported(mLocation)) {
		પાછા;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_લીટી & ~ACK_REPORTED,
		   mLocation->mColumn);

	અચલ અક્ષર *kind = (mTypeCheckKind < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[mTypeCheckKind]
				   : "access to";

	જો (ulPointer == 0) {
		sbi_printf("%s null pointer of type %s\n", kind,
			   mType->mTypeName);
	} નહીંતર જો (
		(mLogAlignment - 1) &
		ulPointer) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			kind, (શૂન્ય *)ulPointer, mType->mTypeName,
			mLogAlignment);
	} નહીંતર {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			kind, (શૂન્ય *)ulPointer, mType->mTypeName);
	}
	SEPARATOR;
}

સ્થિર શૂન્ય HandleVlaBoundNotPositive(bool isFatal, સંરચના CVLABoundData *pData,
				      unsigned long ulBound)
{
	જો (!pData) {
		પાછા;
	}
	જો (isAlreadyReported(&pData->mLocation)) {
		પાછા;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_લીટી & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("variable length array bound value ");

	bool is_signed = pData->mType->mTypeInfo & સંખ્યા_SIGNED_BIT;

	જો (is_signed) {
		sbi_printf("%ld", (long)ulBound);
	} નહીંતર {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

સ્થિર શૂન્ય HandleOutOfBounds(bool isFatal, સંરચના COutOfBoundsData *pData,
			      unsigned long ulIndex)
{
	જો (!pData) {
		પાછા;
	}
	જો (isAlreadyReported(&pData->mLocation)) {
		પાછા;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_લીટી & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mIndexType->mTypeInfo & સંખ્યા_SIGNED_BIT;

	જો (is_signed) {
		sbi_printf("index %ld", (long)ulIndex);
	} નહીંતર {
		sbi_printf("index %lu", ulIndex);
	}

	sbi_printf(" is out of range for type %s\n",
		   pData->mArrayType->mTypeName);

	SEPARATOR;
}

સ્થિર bool is_Negative_સંખ્યા(સંરચના CTypeDescriptor *pType, unsigned long ulVal)
{
	જો (!(pType->mTypeInfo & સંખ્યા_SIGNED_BIT)) {
		પાછા false;
	}

	પાછા (long)ulVal < 0;
}

સ્થિર size_t type_width(સંરચના CTypeDescriptor *pType)
{
	પાછા 1UL << (pType->mTypeInfo >> 1);
}

સ્થિર શૂન્ય HandleShiftOutOfBounds(bool isFatal,
				   સંરચના CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS)
{
	જો (!pData) {
		પાછા;
	}

	જો (isAlreadyReported(&pData->mLocation)) {
		પાછા;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_લીટી & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	જો (is_Negative_સંખ્યા(pData->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (long)ulRHS);
	} નહીંતર જો (ulRHS >= type_width(pData->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (unsigned long)type_width(pData->mLHSType),
			pData->mLHSType->mTypeName);

	} નહીંતર જો (is_Negative_સંખ્યા(pData->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (long)ulLHS);
	} નહીંતર {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, pData->mLHSType->mTypeName);
	}

	SEPARATOR;
}

સ્થિર શૂન્ય Handle_Load_Invalid_કિંમત(bool isFatal,
				   સંરચના CInvalid_કિંમત_Data *pData,
				   unsigned long ul_કિંમત)
{
	જો (!pData) {
		પાછા;
	}

	જો (isAlreadyReported(&pData->mLocation)) {
		પાછા;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_લીટી & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & સંખ્યા_SIGNED_BIT;

	sbi_printf("load of value ");

	જો (is_signed) {
		sbi_printf("%ld ", (long)ul_કિંમત);
	} નહીંતર {
		sbi_printf("%lu ", ul_કિંમત);
	}

	sbi_printf("is not a valid value for type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

અચલ અક્ષર *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

સ્થિર શૂન્ય HandleInvalidBuiltin(bool isFatal,
				 સંરચના CInvalidBuiltinData *pData)
{
	જો (!pData) {
		પાછા;
	}

	જો (isAlreadyReported(&pData->mLocation)) {
		પાછા;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_લીટી & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	અચલ અક્ષર *builtin =
		(pData->mKind < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[pData->mKind]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

સ્થિર શૂન્ય HandleFunctionTypeMismatch(bool isFatal,
				       સંરચના CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction)
{
	જો (!pData) {
		પાછા;
	}

	જો (isAlreadyReported(&pData->mLocation)) {
		પાછા;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_લીટી & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, pData->mType->mTypeName);

	SEPARATOR;
}

સ્થિર શૂન્ય HandleMissingReturn(bool isFatal, સંરચના CUnreachableData *pData)
{
	જો (!pData) {
		પાછા;
	}

	જો (isAlreadyReported(&pData->mLocation)) {
		પાછા;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_લીટી & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

સ્થિર શૂન્ય HandlePointerOverflow(bool isFatal,
				  સંરચના CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult)
{
	જો (!pData) {
		પાછા;
	}

	જો (isAlreadyReported(&pData->mLocation)) {
		પાછા;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_લીટી & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ulBase, ulResult);

	SEPARATOR;
}

સ્થિર શૂન્ય HandleAlignmentAssumption(bool isFatal,
				      સંરચના CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset)
{

	જો (!pData) {
		પાછા;
	}

	જો (isAlreadyReported(&pData->mLocation)) {
		પાછા;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_લીટી & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	unsigned long ulRealPointer = ulPointer - ulOffset;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (શૂન્ય *)ulRealPointer, (શૂન્ય *)ulOffset);

	જો (pData->mAssumptionLocation.mFilename != ખાલી) {
		sbi_printf(", assumption made in %s:%u:%u",
			   pData->mAssumptionLocation.mFilename,
			   pData->mAssumptionLocation.m_લીટી,
			   pData->mAssumptionLocation.mColumn);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
શૂન્ય __ubsan_handle_add_overflow(સંરચના COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "+");
}

શૂન્ય __ubsan_handle_add_overflow_abort(સંરચના COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "+");
}

શૂન્ય __ubsan_handle_sub_overflow(સંરચના COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "-");
}

શૂન્ય __ubsan_handle_sub_overflow_abort(સંરચના COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "-");
}

શૂન્ય __ubsan_handle_mul_overflow(સંરચના COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "*");
}

શૂન્ય __ubsan_handle_mul_overflow_abort(સંરચના COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "*");
}

શૂન્ય __ubsan_handle_negate_overflow(સંરચના COverflowData *pData,
				    unsigned long ul_Old_કિંમત)
{
	HandleNegateOverflow(false, pData, ul_Old_કિંમત);
}

શૂન્ય __ubsan_handle_negate_overflow_abort(સંરચના COverflowData *pData,
					  unsigned long ul_Old_કિંમત)
{
	HandleNegateOverflow(true, pData, ul_Old_કિંમત);
}

શૂન્ય __ubsan_handle_divrem_overflow(સંરચના COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "divrem");
}

શૂન્ય __ubsan_handle_divrem_overflow_abort(સંરચના COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "divrem");
}

શૂન્ય __ubsan_handle_type_mismatch_v1(સંરચના CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer)
{
	HandleTypeMismatch(false, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

શૂન્ય __ubsan_handle_type_mismatch_v1_abort(સંરચના CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer)
{
	HandleTypeMismatch(true, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

શૂન્ય __ubsan_handle_out_of_bounds(સંરચના COutOfBoundsData *pData,
				  unsigned long ulIndex)
{
	HandleOutOfBounds(false, pData, ulIndex);
}

શૂન્ય __ubsan_handle_out_of_bounds_abort(સંરચના COutOfBoundsData *pData,
					unsigned long ulIndex)
{
	HandleOutOfBounds(true, pData, ulIndex);
}

શૂન્ય __ubsan_handle_shift_out_of_bounds(સંરચના CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS)
{
	HandleShiftOutOfBounds(false, pData, ulLHS, ulRHS);
}

શૂન્ય __ubsan_handle_shift_out_of_bounds_abort(
	સંરચના CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS)
{
	HandleShiftOutOfBounds(true, pData, ulLHS, ulRHS);
}

શૂન્ય __ubsan_handle_pointer_overflow(સંરચના CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult)
{
	HandlePointerOverflow(false, pData, ulBase, ulResult);
}

શૂન્ય __ubsan_handle_pointer_overflow_abort(સંરચના CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult)
{
	HandlePointerOverflow(true, pData, ulBase, ulResult);
}

શૂન્ય __ubsan_handle_alignment_assumption(સંરચના CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset)
{
	HandleAlignmentAssumption(false, pData, ulPointer, ulAlignment,
				  ulOffset);
}

શૂન્ય __ubsan_handle_alignment_assumption_abort(
	સંરચના CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset)
{
	HandleAlignmentAssumption(true, pData, ulPointer, ulAlignment,
				  ulOffset);
}

શૂન્ય __ubsan_handle_builtin_unreachable(સંરચના CUnreachableData *pData)
{
	HandleBuiltinUnreachable(true, pData);
}

શૂન્ય __ubsan_handle_invalid_builtin(સંરચના CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

શૂન્ય __ubsan_handle_invalid_builtin_abort(સંરચના CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

શૂન્ય __ubsan_handle_load_invalid_value(સંરચના CInvalid_કિંમત_Data *pData,
				       unsigned long ul_કિંમત)
{
	Handle_Load_Invalid_કિંમત(false, pData, ul_કિંમત);
}

શૂન્ય __ubsan_handle_load_invalid_value_abort(સંરચના CInvalid_કિંમત_Data *pData,
					     unsigned long ul_કિંમત)
{
	Handle_Load_Invalid_કિંમત(true, pData, ul_કિંમત);
}

શૂન્ય __ubsan_handle_missing_return(સંરચના CUnreachableData *pData)
{
	HandleMissingReturn(true, pData);
}

શૂન્ય __ubsan_handle_vla_bound_not_positive(સંરચના CVLABoundData *pData,
					   unsigned long ulBound)
{
	HandleVlaBoundNotPositive(false, pData, ulBound);
}

શૂન્ય __ubsan_handle_vla_bound_not_positive_abort(સંરચના CVLABoundData *pData,
						 unsigned long ulBound)
{
	HandleVlaBoundNotPositive(true, pData, ulBound);
}

શૂન્ય __ubsan_handle_function_type_mismatch(
	સંરચના CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(false, pData, ulFunction);
}

શૂન્ય __ubsan_handle_function_type_mismatch_abort(
	સંરચના CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(true, pData, ulFunction);
}
#endif
