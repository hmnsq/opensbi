/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ifdef UBSAN_ENABLED

#അന്തര്ഭാവ <sbi/sbi_types.h>
#അന്തര്ഭാവ <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#define KIND_INTEGER 0
#define KIND_FLOAT 1
#define KIND_UNKNOWN UINT16_MAX

അസ്ഥിര unsigned long sbi_ubsan_report_എണ്ണം;

ഘടന CSourceLocation {
	അക്ഷരം *mFilename;
	uint32_t m_വരി;
	uint32_t mColumn;
};

ഘടന CTypeDescriptor {
	uint16_t mTypeKind;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

ഘടന COverflowData {
	ഘടന CSourceLocation mLocation;
	ഘടന CTypeDescriptor *mType;
};

ഘടന CUnreachableData {
	ഘടന CSourceLocation mLocation;
};

ഘടന CCFICheckFailData {
	uint8_t mCheckKind;
	ഘടന CSourceLocation mLocation;
	ഘടന CTypeDescriptor *mType;
};

ഘടന CDynamicTypeCacheMissData {
	ഘടന CSourceLocation mLocation;
	ഘടന CTypeDescriptor *mType;
	ശൂന്യം *mTypeInfo;
	uint8_t mTypeCheckKind;
};

ഘടന CFunctionTypeMismatchData {
	ഘടന CSourceLocation mLocation;
	ഘടന CTypeDescriptor *mType;
};

ഘടന CInvalidBuiltinData {
	ഘടന CSourceLocation mLocation;
	uint8_t mKind;
};

ഘടന CInvalid_മൂല്യം_Data {
	ഘടന CSourceLocation mLocation;
	ഘടന CTypeDescriptor *mType;
};

ഘടന CNonNullArgData {
	ഘടന CSourceLocation mLocation;
	ഘടന CSourceLocation mAttributeLocation;
	പൂർണ്ണസംഖ്യ mArgIndex;
};

ഘടന CNonNullReturnData {
	ഘടന CSourceLocation mAttributeLocation;
};

ഘടന COutOfBoundsData {
	ഘടന CSourceLocation mLocation;
	ഘടന CTypeDescriptor *mArrayType;
	ഘടന CTypeDescriptor *mIndexType;
};

ഘടന CPointerOverflowData {
	ഘടന CSourceLocation mLocation;
};

ഘടന CShiftOutOfBoundsData {
	ഘടന CSourceLocation mLocation;
	ഘടന CTypeDescriptor *mLHSType;
	ഘടന CTypeDescriptor *mRHSType;
};

ഘടന CTypeMismatchData {
	ഘടന CSourceLocation mLocation;
	ഘടന CTypeDescriptor *mType;
	unsigned long mLogAlignment;
	uint8_t mTypeCheckKind;
};

ഘടന CTypeMismatchData_v1 {
	ഘടന CSourceLocation mLocation;
	ഘടന CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t mTypeCheckKind;
};

ഘടന CVLABoundData {
	ഘടന CSourceLocation mLocation;
	ഘടന CTypeDescriptor *mType;
};

ഘടന CFloatCastOverflowData {
	ഘടന CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	ഘടന CTypeDescriptor *mFromType;
	ഘടന CTypeDescriptor *mToType;
};

ഘടന CImplicitConversionData {
	ഘടന CSourceLocation mLocation;
	ഘടന CTypeDescriptor *mFromType;
	ഘടന CTypeDescriptor *mToType;
	uint8_t mKind;
};

ഘടന CAlignmentAssumptionData {
	ഘടന CSourceLocation mLocation;
	ഘടന CSourceLocation mAssumptionLocation;
	ഘടന CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
ശൂന്യം __ubsan_handle_add_overflow(ഘടന COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
ശൂന്യം __ubsan_handle_add_overflow_abort(ഘടന COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
ശൂന്യം __ubsan_handle_alignment_assumption(ഘടന CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset);
ശൂന്യം __ubsan_handle_alignment_assumption_abort(
	ഘടന CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset);
ശൂന്യം __ubsan_handle_builtin_unreachable(ഘടന CUnreachableData *pData);
ശൂന്യം __ubsan_handle_divrem_overflow(ഘടന COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS);
ശൂന്യം __ubsan_handle_divrem_overflow_abort(ഘടന COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS);
ശൂന്യം __ubsan_handle_function_type_mismatch(
	ഘടന CFunctionTypeMismatchData *pData, unsigned long ulFunction);
ശൂന്യം __ubsan_handle_function_type_mismatch_abort(
	ഘടന CFunctionTypeMismatchData *pData, unsigned long ulFunction);
ശൂന്യം __ubsan_handle_function_type_mismatch_v1(
	ഘടന CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
ശൂന്യം __ubsan_handle_function_type_mismatch_v1_abort(
	ഘടന CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
ശൂന്യം __ubsan_handle_invalid_builtin(ഘടന CInvalidBuiltinData *pData);
ശൂന്യം __ubsan_handle_invalid_builtin_abort(ഘടന CInvalidBuiltinData *pData);
ശൂന്യം __ubsan_handle_load_invalid_value(ഘടന CInvalid_മൂല്യം_Data *pData,
				       unsigned long ulVal);
ശൂന്യം __ubsan_handle_load_invalid_value_abort(ഘടന CInvalid_മൂല്യം_Data *pData,
					     unsigned long ulVal);
ശൂന്യം __ubsan_handle_missing_return(ഘടന CUnreachableData *pData);
ശൂന്യം __ubsan_handle_mul_overflow(ഘടന COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
ശൂന്യം __ubsan_handle_mul_overflow_abort(ഘടന COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
ശൂന്യം __ubsan_handle_negate_overflow(ഘടന COverflowData *pData,
				    unsigned long ulOldVal);
ശൂന്യം __ubsan_handle_negate_overflow_abort(ഘടന COverflowData *pData,
					  unsigned long ulOldVal);
ശൂന്യം __ubsan_handle_nullability_arg(ഘടന CNonNullArgData *pData);
ശൂന്യം __ubsan_handle_nullability_arg_abort(ഘടന CNonNullArgData *pData);
ശൂന്യം __ubsan_handle_nullability_return_v1(
	ഘടന CNonNullReturnData *pData,
	ഘടന CSourceLocation *pLocationPointer);
ശൂന്യം __ubsan_handle_nullability_return_v1_abort(
	ഘടന CNonNullReturnData *pData,
	ഘടന CSourceLocation *pLocationPointer);
ശൂന്യം __ubsan_handle_out_of_bounds(ഘടന COutOfBoundsData *pData,
				  unsigned long ulIndex);
ശൂന്യം __ubsan_handle_out_of_bounds_abort(ഘടന COutOfBoundsData *pData,
					unsigned long ulIndex);
ശൂന്യം __ubsan_handle_pointer_overflow(ഘടന CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult);
ശൂന്യം __ubsan_handle_pointer_overflow_abort(ഘടന CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult);
ശൂന്യം __ubsan_handle_shift_out_of_bounds(ഘടന CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS);
ശൂന്യം __ubsan_handle_shift_out_of_bounds_abort(
	ഘടന CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS);
ശൂന്യം __ubsan_handle_sub_overflow(ഘടന COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
ശൂന്യം __ubsan_handle_sub_overflow_abort(ഘടന COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
ശൂന്യം __ubsan_handle_type_mismatch(ഘടന CTypeMismatchData *pData,
				  unsigned long ulPointer);
ശൂന്യം __ubsan_handle_type_mismatch_abort(ഘടന CTypeMismatchData *pData,
					unsigned long ulPointer);
ശൂന്യം __ubsan_handle_type_mismatch_v1(ഘടന CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer);
ശൂന്യം __ubsan_handle_type_mismatch_v1_abort(ഘടന CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer);
ശൂന്യം __ubsan_handle_vla_bound_not_positive(ഘടന CVLABoundData *pData,
					   unsigned long ulBound);
ശൂന്യം __ubsan_handle_vla_bound_not_positive_abort(ഘടന CVLABoundData *pData,
						 unsigned long ulBound);
ശൂന്യം __ubsan_get_current_report_data(സ്ഥിരം അക്ഷരം **ppOutIssueKind,
				     സ്ഥിരം അക്ഷരം **ppOutMessage,
				     സ്ഥിരം അക്ഷരം **ppOutFilename,
				     uint32_t *p_Out_വരി, uint32_t *pOutCol,
				     അക്ഷരം **ppOutMemoryAddr);
സ്ഥിതം ശൂന്യം HandleOverflow(bool isFatal, ഘടന COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   സ്ഥിരം അക്ഷരം *szOperation);
സ്ഥിതം ശൂന്യം HandleNegateOverflow(bool isFatal, ഘടന COverflowData *pData,
				 unsigned long ul_Old_മൂല്യം);
സ്ഥിതം ശൂന്യം HandleBuiltinUnreachable(bool isFatal,
				     ഘടന CUnreachableData *pData);
സ്ഥിതം ശൂന്യം HandleTypeMismatch(bool isFatal, ഘടന CSourceLocation *mLocation,
			       ഘടന CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer);
സ്ഥിതം ശൂന്യം HandleVlaBoundNotPositive(bool isFatal, ഘടന CVLABoundData *pData,
				      unsigned long ulBound);
സ്ഥിതം ശൂന്യം HandleOutOfBounds(bool isFatal, ഘടന COutOfBoundsData *pData,
			      unsigned long ulIndex);
സ്ഥിതം ശൂന്യം HandleShiftOutOfBounds(bool isFatal,
				   ഘടന CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS);
സ്ഥിതം ശൂന്യം Handle_Load_Invalid_മൂല്യം(bool isFatal,
				   ഘടന CInvalid_മൂല്യം_Data *pData,
				   unsigned long ul_മൂല്യം);
സ്ഥിതം ശൂന്യം HandleInvalidBuiltin(bool isFatal,
				 ഘടന CInvalidBuiltinData *pData);
സ്ഥിതം ശൂന്യം HandleFunctionTypeMismatch(bool isFatal,
				       ഘടന CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction);
സ്ഥിതം ശൂന്യം HandleMissingReturn(bool isFatal, ഘടന CUnreachableData *pData);
സ്ഥിതം ശൂന്യം HandlePointerOverflow(bool isFatal,
				  ഘടന CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult);
സ്ഥിതം ശൂന്യം HandleAlignmentAssumption(bool isFatal,
				      ഘടന CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset);

#define സംഖ്യ_SIGNED_BIT 1
#define സംഖ്യ_MAXLEN 128
#define __arraycount(__a) (വലുപ്പം(__a) / വലുപ്പം(__a[0]))
#define __BIT(__n) (1UL << (__n))
#define SEPARATOR sbi_printf("===========================================\n")
#define ACK_REPORTED (1U << 31)

സ്ഥിതം bool isAlreadyReported(ഘടന CSourceLocation *pLocation)
{
	uint32_t si_Old_മൂല്യം;
	അസ്ഥിര uint32_t *p_വരി;

	എങ്കിൽ (!pLocation)
		തിരികെ false;

	p_വരി = &pLocation->m_വരി;

	ചെയ്യുക {
		si_Old_മൂല്യം = *p_വരി;
	} ആകുമ്പോൾ (__sync_val_compare_and_swap(p_വരി, si_Old_മൂല്യം,
					     si_Old_മൂല്യം | ACK_REPORTED) !=
		 si_Old_മൂല്യം);

	എങ്കിൽ (!(si_Old_മൂല്യം & ACK_REPORTED)) {

		sbi_ubsan_report_എണ്ണം++;

		തിരികെ false;
	}

	തിരികെ true;
}

സ്ഥിതം ശൂന്യം HandleOverflow(bool isFatal, ഘടന COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   സ്ഥിരം അക്ഷരം *szOperation)
{
	എങ്കിൽ (!pData) {
		തിരികെ;
	}

	എങ്കിൽ (isAlreadyReported(&pData->mLocation)) {
		തിരികെ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_വരി & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & സംഖ്യ_SIGNED_BIT;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	എങ്കിൽ (is_signed) {
		sbi_printf("%ld %s %ld ", (long)ulLHS, szOperation,
			   (long)ulRHS);
	} അല്ലെങ്കിൽ {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

സ്ഥിതം ശൂന്യം HandleNegateOverflow(bool isFatal, ഘടന COverflowData *pData,
				 unsigned long ul_Old_മൂല്യം)
{
	എങ്കിൽ (!pData) {
		തിരികെ;
	}
	എങ്കിൽ (isAlreadyReported(&pData->mLocation)) {
		തിരികെ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_വരി & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & സംഖ്യ_SIGNED_BIT;

	sbi_printf("UBSan: Negation of ");

	എങ്കിൽ (is_signed) {
		sbi_printf("%ld", (long)ul_Old_മൂല്യം);
	} അല്ലെങ്കിൽ {
		sbi_printf("%lu", ul_Old_മൂല്യം);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

സ്ഥിതം ശൂന്യം HandleBuiltinUnreachable(bool isFatal,
				     ഘടന CUnreachableData *pData)
{
	എങ്കിൽ (!pData) {
		തിരികെ;
	}
	എങ്കിൽ (isAlreadyReported(&pData->mLocation)) {
		തിരികെ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_വരി & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	SEPARATOR;
}

സ്ഥിരം അക്ഷരം *rgczTypeCheckKinds[] = { "load of",
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

സ്ഥിതം ശൂന്യം HandleTypeMismatch(bool isFatal, ഘടന CSourceLocation *mLocation,
			       ഘടന CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer)
{

	എങ്കിൽ ((!mLocation) || (!mType)) {
		തിരികെ;
	}

	എങ്കിൽ (isAlreadyReported(mLocation)) {
		തിരികെ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_വരി & ~ACK_REPORTED,
		   mLocation->mColumn);

	സ്ഥിരം അക്ഷരം *kind = (mTypeCheckKind < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[mTypeCheckKind]
				   : "access to";

	എങ്കിൽ (ulPointer == 0) {
		sbi_printf("%s null pointer of type %s\n", kind,
			   mType->mTypeName);
	} അല്ലെങ്കിൽ എങ്കിൽ (
		(mLogAlignment - 1) &
		ulPointer) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			kind, (ശൂന്യം *)ulPointer, mType->mTypeName,
			mLogAlignment);
	} അല്ലെങ്കിൽ {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			kind, (ശൂന്യം *)ulPointer, mType->mTypeName);
	}
	SEPARATOR;
}

സ്ഥിതം ശൂന്യം HandleVlaBoundNotPositive(bool isFatal, ഘടന CVLABoundData *pData,
				      unsigned long ulBound)
{
	എങ്കിൽ (!pData) {
		തിരികെ;
	}
	എങ്കിൽ (isAlreadyReported(&pData->mLocation)) {
		തിരികെ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_വരി & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("variable length array bound value ");

	bool is_signed = pData->mType->mTypeInfo & സംഖ്യ_SIGNED_BIT;

	എങ്കിൽ (is_signed) {
		sbi_printf("%ld", (long)ulBound);
	} അല്ലെങ്കിൽ {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

സ്ഥിതം ശൂന്യം HandleOutOfBounds(bool isFatal, ഘടന COutOfBoundsData *pData,
			      unsigned long ulIndex)
{
	എങ്കിൽ (!pData) {
		തിരികെ;
	}
	എങ്കിൽ (isAlreadyReported(&pData->mLocation)) {
		തിരികെ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_വരി & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mIndexType->mTypeInfo & സംഖ്യ_SIGNED_BIT;

	എങ്കിൽ (is_signed) {
		sbi_printf("index %ld", (long)ulIndex);
	} അല്ലെങ്കിൽ {
		sbi_printf("index %lu", ulIndex);
	}

	sbi_printf(" is out of range for type %s\n",
		   pData->mArrayType->mTypeName);

	SEPARATOR;
}

സ്ഥിതം bool is_Negative_സംഖ്യ(ഘടന CTypeDescriptor *pType, unsigned long ulVal)
{
	എങ്കിൽ (!(pType->mTypeInfo & സംഖ്യ_SIGNED_BIT)) {
		തിരികെ false;
	}

	തിരികെ (long)ulVal < 0;
}

സ്ഥിതം size_t type_width(ഘടന CTypeDescriptor *pType)
{
	തിരികെ 1UL << (pType->mTypeInfo >> 1);
}

സ്ഥിതം ശൂന്യം HandleShiftOutOfBounds(bool isFatal,
				   ഘടന CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS)
{
	എങ്കിൽ (!pData) {
		തിരികെ;
	}

	എങ്കിൽ (isAlreadyReported(&pData->mLocation)) {
		തിരികെ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_വരി & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	എങ്കിൽ (is_Negative_സംഖ്യ(pData->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (long)ulRHS);
	} അല്ലെങ്കിൽ എങ്കിൽ (ulRHS >= type_width(pData->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (unsigned long)type_width(pData->mLHSType),
			pData->mLHSType->mTypeName);

	} അല്ലെങ്കിൽ എങ്കിൽ (is_Negative_സംഖ്യ(pData->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (long)ulLHS);
	} അല്ലെങ്കിൽ {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, pData->mLHSType->mTypeName);
	}

	SEPARATOR;
}

സ്ഥിതം ശൂന്യം Handle_Load_Invalid_മൂല്യം(bool isFatal,
				   ഘടന CInvalid_മൂല്യം_Data *pData,
				   unsigned long ul_മൂല്യം)
{
	എങ്കിൽ (!pData) {
		തിരികെ;
	}

	എങ്കിൽ (isAlreadyReported(&pData->mLocation)) {
		തിരികെ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_വരി & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & സംഖ്യ_SIGNED_BIT;

	sbi_printf("load of value ");

	എങ്കിൽ (is_signed) {
		sbi_printf("%ld ", (long)ul_മൂല്യം);
	} അല്ലെങ്കിൽ {
		sbi_printf("%lu ", ul_മൂല്യം);
	}

	sbi_printf("is not a valid value for type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

സ്ഥിരം അക്ഷരം *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

സ്ഥിതം ശൂന്യം HandleInvalidBuiltin(bool isFatal,
				 ഘടന CInvalidBuiltinData *pData)
{
	എങ്കിൽ (!pData) {
		തിരികെ;
	}

	എങ്കിൽ (isAlreadyReported(&pData->mLocation)) {
		തിരികെ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_വരി & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	സ്ഥിരം അക്ഷരം *builtin =
		(pData->mKind < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[pData->mKind]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

സ്ഥിതം ശൂന്യം HandleFunctionTypeMismatch(bool isFatal,
				       ഘടന CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction)
{
	എങ്കിൽ (!pData) {
		തിരികെ;
	}

	എങ്കിൽ (isAlreadyReported(&pData->mLocation)) {
		തിരികെ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_വരി & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, pData->mType->mTypeName);

	SEPARATOR;
}

സ്ഥിതം ശൂന്യം HandleMissingReturn(bool isFatal, ഘടന CUnreachableData *pData)
{
	എങ്കിൽ (!pData) {
		തിരികെ;
	}

	എങ്കിൽ (isAlreadyReported(&pData->mLocation)) {
		തിരികെ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_വരി & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

സ്ഥിതം ശൂന്യം HandlePointerOverflow(bool isFatal,
				  ഘടന CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult)
{
	എങ്കിൽ (!pData) {
		തിരികെ;
	}

	എങ്കിൽ (isAlreadyReported(&pData->mLocation)) {
		തിരികെ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_വരി & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ulBase, ulResult);

	SEPARATOR;
}

സ്ഥിതം ശൂന്യം HandleAlignmentAssumption(bool isFatal,
				      ഘടന CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset)
{

	എങ്കിൽ (!pData) {
		തിരികെ;
	}

	എങ്കിൽ (isAlreadyReported(&pData->mLocation)) {
		തിരികെ;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_വരി & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	unsigned long ulRealPointer = ulPointer - ulOffset;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (ശൂന്യം *)ulRealPointer, (ശൂന്യം *)ulOffset);

	എങ്കിൽ (pData->mAssumptionLocation.mFilename != ശൂന്യ) {
		sbi_printf(", assumption made in %s:%u:%u",
			   pData->mAssumptionLocation.mFilename,
			   pData->mAssumptionLocation.m_വരി,
			   pData->mAssumptionLocation.mColumn);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
ശൂന്യം __ubsan_handle_add_overflow(ഘടന COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "+");
}

ശൂന്യം __ubsan_handle_add_overflow_abort(ഘടന COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "+");
}

ശൂന്യം __ubsan_handle_sub_overflow(ഘടന COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "-");
}

ശൂന്യം __ubsan_handle_sub_overflow_abort(ഘടന COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "-");
}

ശൂന്യം __ubsan_handle_mul_overflow(ഘടന COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "*");
}

ശൂന്യം __ubsan_handle_mul_overflow_abort(ഘടന COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "*");
}

ശൂന്യം __ubsan_handle_negate_overflow(ഘടന COverflowData *pData,
				    unsigned long ul_Old_മൂല്യം)
{
	HandleNegateOverflow(false, pData, ul_Old_മൂല്യം);
}

ശൂന്യം __ubsan_handle_negate_overflow_abort(ഘടന COverflowData *pData,
					  unsigned long ul_Old_മൂല്യം)
{
	HandleNegateOverflow(true, pData, ul_Old_മൂല്യം);
}

ശൂന്യം __ubsan_handle_divrem_overflow(ഘടന COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "divrem");
}

ശൂന്യം __ubsan_handle_divrem_overflow_abort(ഘടന COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "divrem");
}

ശൂന്യം __ubsan_handle_type_mismatch_v1(ഘടന CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer)
{
	HandleTypeMismatch(false, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

ശൂന്യം __ubsan_handle_type_mismatch_v1_abort(ഘടന CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer)
{
	HandleTypeMismatch(true, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

ശൂന്യം __ubsan_handle_out_of_bounds(ഘടന COutOfBoundsData *pData,
				  unsigned long ulIndex)
{
	HandleOutOfBounds(false, pData, ulIndex);
}

ശൂന്യം __ubsan_handle_out_of_bounds_abort(ഘടന COutOfBoundsData *pData,
					unsigned long ulIndex)
{
	HandleOutOfBounds(true, pData, ulIndex);
}

ശൂന്യം __ubsan_handle_shift_out_of_bounds(ഘടന CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS)
{
	HandleShiftOutOfBounds(false, pData, ulLHS, ulRHS);
}

ശൂന്യം __ubsan_handle_shift_out_of_bounds_abort(
	ഘടന CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS)
{
	HandleShiftOutOfBounds(true, pData, ulLHS, ulRHS);
}

ശൂന്യം __ubsan_handle_pointer_overflow(ഘടന CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult)
{
	HandlePointerOverflow(false, pData, ulBase, ulResult);
}

ശൂന്യം __ubsan_handle_pointer_overflow_abort(ഘടന CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult)
{
	HandlePointerOverflow(true, pData, ulBase, ulResult);
}

ശൂന്യം __ubsan_handle_alignment_assumption(ഘടന CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset)
{
	HandleAlignmentAssumption(false, pData, ulPointer, ulAlignment,
				  ulOffset);
}

ശൂന്യം __ubsan_handle_alignment_assumption_abort(
	ഘടന CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset)
{
	HandleAlignmentAssumption(true, pData, ulPointer, ulAlignment,
				  ulOffset);
}

ശൂന്യം __ubsan_handle_builtin_unreachable(ഘടന CUnreachableData *pData)
{
	HandleBuiltinUnreachable(true, pData);
}

ശൂന്യം __ubsan_handle_invalid_builtin(ഘടന CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

ശൂന്യം __ubsan_handle_invalid_builtin_abort(ഘടന CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

ശൂന്യം __ubsan_handle_load_invalid_value(ഘടന CInvalid_മൂല്യം_Data *pData,
				       unsigned long ul_മൂല്യം)
{
	Handle_Load_Invalid_മൂല്യം(false, pData, ul_മൂല്യം);
}

ശൂന്യം __ubsan_handle_load_invalid_value_abort(ഘടന CInvalid_മൂല്യം_Data *pData,
					     unsigned long ul_മൂല്യം)
{
	Handle_Load_Invalid_മൂല്യം(true, pData, ul_മൂല്യം);
}

ശൂന്യം __ubsan_handle_missing_return(ഘടന CUnreachableData *pData)
{
	HandleMissingReturn(true, pData);
}

ശൂന്യം __ubsan_handle_vla_bound_not_positive(ഘടന CVLABoundData *pData,
					   unsigned long ulBound)
{
	HandleVlaBoundNotPositive(false, pData, ulBound);
}

ശൂന്യം __ubsan_handle_vla_bound_not_positive_abort(ഘടന CVLABoundData *pData,
						 unsigned long ulBound)
{
	HandleVlaBoundNotPositive(true, pData, ulBound);
}

ശൂന്യം __ubsan_handle_function_type_mismatch(
	ഘടന CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(false, pData, ulFunction);
}

ശൂന്യം __ubsan_handle_function_type_mismatch_abort(
	ഘടന CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(true, pData, ulFunction);
}
#endif
