/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2018 The NetBSD Foundation, Inc.
 *
 * Author: Marcos Oduardo <marcos.oduardo@gmail.com>
 */

#ifdef UBSAN_ENABLED

#अन्तर्भाव <sbi/sbi_types.h>
#अन्तर्भाव <sbi/sbi_console.h>

/* Undefined Behavior specific defines and structures defined by the compiler ABI */

#define KIND_INTEGER 0
#define KIND_FLOAT 1
#define KIND_UNKNOWN UINT16_MAX

चञ्चल unsigned long sbi_ubsan_report_गणनम्;

संरचना CSourceLocation {
	वर्ण *mFilename;
	uint32_t m_पङ्क्तिः;
	uint32_t mColumn;
};

संरचना CTypeDescriptor {
	uint16_t mTypeKind;
	uint16_t mTypeInfo;
	uint8_t mTypeName[1];
};

संरचना COverflowData {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
};

संरचना CUnreachableData {
	संरचना CSourceLocation mLocation;
};

संरचना CCFICheckFailData {
	uint8_t mCheckKind;
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
};

संरचना CDynamicTypeCacheMissData {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
	शून्य *mTypeInfo;
	uint8_t mTypeCheckKind;
};

संरचना CFunctionTypeMismatchData {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
};

संरचना CInvalidBuiltinData {
	संरचना CSourceLocation mLocation;
	uint8_t mKind;
};

संरचना CInvalid_मानम्_Data {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
};

संरचना CNonNullArgData {
	संरचना CSourceLocation mLocation;
	संरचना CSourceLocation mAttributeLocation;
	पूर्णाङ्क mArgIndex;
};

संरचना CNonNullReturnData {
	संरचना CSourceLocation mAttributeLocation;
};

संरचना COutOfBoundsData {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mArrayType;
	संरचना CTypeDescriptor *mIndexType;
};

संरचना CPointerOverflowData {
	संरचना CSourceLocation mLocation;
};

संरचना CShiftOutOfBoundsData {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mLHSType;
	संरचना CTypeDescriptor *mRHSType;
};

संरचना CTypeMismatchData {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
	unsigned long mLogAlignment;
	uint8_t mTypeCheckKind;
};

संरचना CTypeMismatchData_v1 {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
	uint8_t mLogAlignment;
	uint8_t mTypeCheckKind;
};

संरचना CVLABoundData {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mType;
};

संरचना CFloatCastOverflowData {
	संरचना CSourceLocation
		mLocation; /* This field exists in this struct since 2015 August 11th */
	संरचना CTypeDescriptor *mFromType;
	संरचना CTypeDescriptor *mToType;
};

संरचना CImplicitConversionData {
	संरचना CSourceLocation mLocation;
	संरचना CTypeDescriptor *mFromType;
	संरचना CTypeDescriptor *mToType;
	uint8_t mKind;
};

संरचना CAlignmentAssumptionData {
	संरचना CSourceLocation mLocation;
	संरचना CSourceLocation mAssumptionLocation;
	संरचना CTypeDescriptor *mType;
};

/* Public symbols used in the instrumentation of the code generation part */
शून्य __ubsan_handle_add_overflow(संरचना COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
शून्य __ubsan_handle_add_overflow_abort(संरचना COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
शून्य __ubsan_handle_alignment_assumption(संरचना CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset);
शून्य __ubsan_handle_alignment_assumption_abort(
	संरचना CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset);
शून्य __ubsan_handle_builtin_unreachable(संरचना CUnreachableData *pData);
शून्य __ubsan_handle_divrem_overflow(संरचना COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS);
शून्य __ubsan_handle_divrem_overflow_abort(संरचना COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS);
शून्य __ubsan_handle_function_type_mismatch(
	संरचना CFunctionTypeMismatchData *pData, unsigned long ulFunction);
शून्य __ubsan_handle_function_type_mismatch_abort(
	संरचना CFunctionTypeMismatchData *pData, unsigned long ulFunction);
शून्य __ubsan_handle_function_type_mismatch_v1(
	संरचना CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
शून्य __ubsan_handle_function_type_mismatch_v1_abort(
	संरचना CFunctionTypeMismatchData *pData, unsigned long ulFunction,
	unsigned long ulCalleeRTTI, unsigned long ulFnRTTI);
शून्य __ubsan_handle_invalid_builtin(संरचना CInvalidBuiltinData *pData);
शून्य __ubsan_handle_invalid_builtin_abort(संरचना CInvalidBuiltinData *pData);
शून्य __ubsan_handle_load_invalid_value(संरचना CInvalid_मानम्_Data *pData,
				       unsigned long ulVal);
शून्य __ubsan_handle_load_invalid_value_abort(संरचना CInvalid_मानम्_Data *pData,
					     unsigned long ulVal);
शून्य __ubsan_handle_missing_return(संरचना CUnreachableData *pData);
शून्य __ubsan_handle_mul_overflow(संरचना COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
शून्य __ubsan_handle_mul_overflow_abort(संरचना COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
शून्य __ubsan_handle_negate_overflow(संरचना COverflowData *pData,
				    unsigned long ulOldVal);
शून्य __ubsan_handle_negate_overflow_abort(संरचना COverflowData *pData,
					  unsigned long ulOldVal);
शून्य __ubsan_handle_nullability_arg(संरचना CNonNullArgData *pData);
शून्य __ubsan_handle_nullability_arg_abort(संरचना CNonNullArgData *pData);
शून्य __ubsan_handle_nullability_return_v1(
	संरचना CNonNullReturnData *pData,
	संरचना CSourceLocation *pLocationPointer);
शून्य __ubsan_handle_nullability_return_v1_abort(
	संरचना CNonNullReturnData *pData,
	संरचना CSourceLocation *pLocationPointer);
शून्य __ubsan_handle_out_of_bounds(संरचना COutOfBoundsData *pData,
				  unsigned long ulIndex);
शून्य __ubsan_handle_out_of_bounds_abort(संरचना COutOfBoundsData *pData,
					unsigned long ulIndex);
शून्य __ubsan_handle_pointer_overflow(संरचना CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult);
शून्य __ubsan_handle_pointer_overflow_abort(संरचना CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult);
शून्य __ubsan_handle_shift_out_of_bounds(संरचना CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS);
शून्य __ubsan_handle_shift_out_of_bounds_abort(
	संरचना CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS);
शून्य __ubsan_handle_sub_overflow(संरचना COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS);
शून्य __ubsan_handle_sub_overflow_abort(संरचना COverflowData *pData,
				       unsigned long ulLHS,
				       unsigned long ulRHS);
शून्य __ubsan_handle_type_mismatch(संरचना CTypeMismatchData *pData,
				  unsigned long ulPointer);
शून्य __ubsan_handle_type_mismatch_abort(संरचना CTypeMismatchData *pData,
					unsigned long ulPointer);
शून्य __ubsan_handle_type_mismatch_v1(संरचना CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer);
शून्य __ubsan_handle_type_mismatch_v1_abort(संरचना CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer);
शून्य __ubsan_handle_vla_bound_not_positive(संरचना CVLABoundData *pData,
					   unsigned long ulBound);
शून्य __ubsan_handle_vla_bound_not_positive_abort(संरचना CVLABoundData *pData,
						 unsigned long ulBound);
शून्य __ubsan_get_current_report_data(ध्रुव वर्ण **ppOutIssueKind,
				     ध्रुव वर्ण **ppOutMessage,
				     ध्रुव वर्ण **ppOutFilename,
				     uint32_t *p_Out_पङ्क्तिः, uint32_t *pOutCol,
				     वर्ण **ppOutMemoryAddr);
स्थिर शून्य HandleOverflow(bool isFatal, संरचना COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   ध्रुव वर्ण *szOperation);
स्थिर शून्य HandleNegateOverflow(bool isFatal, संरचना COverflowData *pData,
				 unsigned long ul_Old_मानम्);
स्थिर शून्य HandleBuiltinUnreachable(bool isFatal,
				     संरचना CUnreachableData *pData);
स्थिर शून्य HandleTypeMismatch(bool isFatal, संरचना CSourceLocation *mLocation,
			       संरचना CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer);
स्थिर शून्य HandleVlaBoundNotPositive(bool isFatal, संरचना CVLABoundData *pData,
				      unsigned long ulBound);
स्थिर शून्य HandleOutOfBounds(bool isFatal, संरचना COutOfBoundsData *pData,
			      unsigned long ulIndex);
स्थिर शून्य HandleShiftOutOfBounds(bool isFatal,
				   संरचना CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS);
स्थिर शून्य Handle_Load_Invalid_मानम्(bool isFatal,
				   संरचना CInvalid_मानम्_Data *pData,
				   unsigned long ul_मानम्);
स्थिर शून्य HandleInvalidBuiltin(bool isFatal,
				 संरचना CInvalidBuiltinData *pData);
स्थिर शून्य HandleFunctionTypeMismatch(bool isFatal,
				       संरचना CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction);
स्थिर शून्य HandleMissingReturn(bool isFatal, संरचना CUnreachableData *pData);
स्थिर शून्य HandlePointerOverflow(bool isFatal,
				  संरचना CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult);
स्थिर शून्य HandleAlignmentAssumption(bool isFatal,
				      संरचना CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset);

#define सङ्ख्या_SIGNED_BIT 1
#define सङ्ख्या_MAXLEN 128
#define __arraycount(__a) (परिमाण(__a) / परिमाण(__a[0]))
#define __BIT(__n) (1UL << (__n))
#define SEPARATOR sbi_printf("===========================================\n")
#define ACK_REPORTED (1U << 31)

स्थिर bool isAlreadyReported(संरचना CSourceLocation *pLocation)
{
	uint32_t si_Old_मानम्;
	चञ्चल uint32_t *p_पङ्क्तिः;

	सचे (!pLocation)
		पटिनिवत्त false;

	p_पङ्क्तिः = &pLocation->m_पङ्क्तिः;

	करोहि {
		si_Old_मानम् = *p_पङ्क्तिः;
	} याव (__sync_val_compare_and_swap(p_पङ्क्तिः, si_Old_मानम्,
					     si_Old_मानम् | ACK_REPORTED) !=
		 si_Old_मानम्);

	सचे (!(si_Old_मानम् & ACK_REPORTED)) {

		sbi_ubsan_report_गणनम्++;

		पटिनिवत्त false;
	}

	पटिनिवत्त true;
}

स्थिर शून्य HandleOverflow(bool isFatal, संरचना COverflowData *pData,
			   unsigned long ulLHS, unsigned long ulRHS,
			   ध्रुव वर्ण *szOperation)
{
	सचे (!pData) {
		पटिनिवत्त;
	}

	सचे (isAlreadyReported(&pData->mLocation)) {
		पटिनिवत्त;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & सङ्ख्या_SIGNED_BIT;

	sbi_printf("UBSan: %s integer overflow: ",
		   is_signed ? "signed" : "unsigned");

	सचे (is_signed) {
		sbi_printf("%ld %s %ld ", (long)ulLHS, szOperation,
			   (long)ulRHS);
	} अञ्ञथा {
		sbi_printf("%lu %s %lu ", ulLHS, szOperation, ulRHS);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

स्थिर शून्य HandleNegateOverflow(bool isFatal, संरचना COverflowData *pData,
				 unsigned long ul_Old_मानम्)
{
	सचे (!pData) {
		पटिनिवत्त;
	}
	सचे (isAlreadyReported(&pData->mLocation)) {
		पटिनिवत्त;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & सङ्ख्या_SIGNED_BIT;

	sbi_printf("UBSan: Negation of ");

	सचे (is_signed) {
		sbi_printf("%ld", (long)ul_Old_मानम्);
	} अञ्ञथा {
		sbi_printf("%lu", ul_Old_मानम्);
	}

	sbi_printf("cannot be represented in type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

स्थिर शून्य HandleBuiltinUnreachable(bool isFatal,
				     संरचना CUnreachableData *pData)
{
	सचे (!pData) {
		पटिनिवत्त;
	}
	सचे (isAlreadyReported(&pData->mLocation)) {
		पटिनिवत्त;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	SEPARATOR;
}

ध्रुव वर्ण *rgczTypeCheckKinds[] = { "load of",
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

स्थिर शून्य HandleTypeMismatch(bool isFatal, संरचना CSourceLocation *mLocation,
			       संरचना CTypeDescriptor *mType,
			       unsigned long mLogAlignment,
			       uint8_t mTypeCheckKind, unsigned long ulPointer)
{

	सचे ((!mLocation) || (!mType)) {
		पटिनिवत्त;
	}

	सचे (isAlreadyReported(mLocation)) {
		पटिनिवत्त;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   mLocation->mFilename, mLocation->m_पङ्क्तिः & ~ACK_REPORTED,
		   mLocation->mColumn);

	ध्रुव वर्ण *kind = (mTypeCheckKind < __arraycount(rgczTypeCheckKinds))
				   ? rgczTypeCheckKinds[mTypeCheckKind]
				   : "access to";

	सचे (ulPointer == 0) {
		sbi_printf("%s null pointer of type %s\n", kind,
			   mType->mTypeName);
	} अञ्ञथा सचे (
		(mLogAlignment - 1) &
		ulPointer) { //mLogAlignment is converted on the wrapper function call
		sbi_printf(
			"%s misaligned address %p for type %s which requires %ld byte alignment\n",
			kind, (शून्य *)ulPointer, mType->mTypeName,
			mLogAlignment);
	} अञ्ञथा {
		sbi_printf(
			"%s address %p with insufficient space for an object of type %s\n",
			kind, (शून्य *)ulPointer, mType->mTypeName);
	}
	SEPARATOR;
}

स्थिर शून्य HandleVlaBoundNotPositive(bool isFatal, संरचना CVLABoundData *pData,
				      unsigned long ulBound)
{
	सचे (!pData) {
		पटिनिवत्त;
	}
	सचे (isAlreadyReported(&pData->mLocation)) {
		पटिनिवत्त;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("variable length array bound value ");

	bool is_signed = pData->mType->mTypeInfo & सङ्ख्या_SIGNED_BIT;

	सचे (is_signed) {
		sbi_printf("%ld", (long)ulBound);
	} अञ्ञथा {
		sbi_printf("%lu", ulBound);
	}

	sbi_printf(" <= 0\n");

	SEPARATOR;
}

स्थिर शून्य HandleOutOfBounds(bool isFatal, संरचना COutOfBoundsData *pData,
			      unsigned long ulIndex)
{
	सचे (!pData) {
		पटिनिवत्त;
	}
	सचे (isAlreadyReported(&pData->mLocation)) {
		पटिनिवत्त;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mIndexType->mTypeInfo & सङ्ख्या_SIGNED_BIT;

	सचे (is_signed) {
		sbi_printf("index %ld", (long)ulIndex);
	} अञ्ञथा {
		sbi_printf("index %lu", ulIndex);
	}

	sbi_printf(" is out of range for type %s\n",
		   pData->mArrayType->mTypeName);

	SEPARATOR;
}

स्थिर bool is_Negative_सङ्ख्या(संरचना CTypeDescriptor *pType, unsigned long ulVal)
{
	सचे (!(pType->mTypeInfo & सङ्ख्या_SIGNED_BIT)) {
		पटिनिवत्त false;
	}

	पटिनिवत्त (long)ulVal < 0;
}

स्थिर size_t type_width(संरचना CTypeDescriptor *pType)
{
	पटिनिवत्त 1UL << (pType->mTypeInfo >> 1);
}

स्थिर शून्य HandleShiftOutOfBounds(bool isFatal,
				   संरचना CShiftOutOfBoundsData *pData,
				   unsigned long ulLHS, unsigned long ulRHS)
{
	सचे (!pData) {
		पटिनिवत्त;
	}

	सचे (isAlreadyReported(&pData->mLocation)) {
		पटिनिवत्त;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	सचे (is_Negative_सङ्ख्या(pData->mRHSType, ulRHS)) {
		sbi_printf("shift exponent %ld is negative\n", (long)ulRHS);
	} अञ्ञथा सचे (ulRHS >= type_width(pData->mLHSType)) {
		sbi_printf(
			"shift exponent %lu is too large for %lu-bit type %s\n",
			ulRHS, (unsigned long)type_width(pData->mLHSType),
			pData->mLHSType->mTypeName);

	} अञ्ञथा सचे (is_Negative_सङ्ख्या(pData->mLHSType, ulLHS)) {
		sbi_printf("left shift of negative value %ld\n", (long)ulLHS);
	} अञ्ञथा {
		sbi_printf(
			"left shift of %lu by %lu places cannot be represented in type %s\n",
			ulLHS, ulRHS, pData->mLHSType->mTypeName);
	}

	SEPARATOR;
}

स्थिर शून्य Handle_Load_Invalid_मानम्(bool isFatal,
				   संरचना CInvalid_मानम्_Data *pData,
				   unsigned long ul_मानम्)
{
	सचे (!pData) {
		पटिनिवत्त;
	}

	सचे (isAlreadyReported(&pData->mLocation)) {
		पटिनिवत्त;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	bool is_signed = pData->mType->mTypeInfo & सङ्ख्या_SIGNED_BIT;

	sbi_printf("load of value ");

	सचे (is_signed) {
		sbi_printf("%ld ", (long)ul_मानम्);
	} अञ्ञथा {
		sbi_printf("%lu ", ul_मानम्);
	}

	sbi_printf("is not a valid value for type %s\n",
		   pData->mType->mTypeName);

	SEPARATOR;
}

ध्रुव वर्ण *rgczBuiltinCheckKinds[] = { "ctz()", "clz()" };

स्थिर शून्य HandleInvalidBuiltin(bool isFatal,
				 संरचना CInvalidBuiltinData *pData)
{
	सचे (!pData) {
		पटिनिवत्त;
	}

	सचे (isAlreadyReported(&pData->mLocation)) {
		पटिनिवत्त;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	ध्रुव वर्ण *builtin =
		(pData->mKind < __arraycount(rgczBuiltinCheckKinds))
			? rgczBuiltinCheckKinds[pData->mKind]
			: "unknown builtin";

	sbi_printf("passing zero to %s, which is not a valid argument\n",
		   builtin);

	SEPARATOR;
}

स्थिर शून्य HandleFunctionTypeMismatch(bool isFatal,
				       संरचना CFunctionTypeMismatchData *pData,
				       unsigned long ulFunction)
{
	सचे (!pData) {
		पटिनिवत्त;
	}

	सचे (isAlreadyReported(&pData->mLocation)) {
		पटिनिवत्त;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"call to function %#lx through pointer to incorrect function type %s\n",
		ulFunction, pData->mType->mTypeName);

	SEPARATOR;
}

स्थिर शून्य HandleMissingReturn(bool isFatal, संरचना CUnreachableData *pData)
{
	सचे (!pData) {
		पटिनिवत्त;
	}

	सचे (isAlreadyReported(&pData->mLocation)) {
		पटिनिवत्त;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf(
		"execution reached the end of a value-returning function without returning a value\n");

	SEPARATOR;
}

स्थिर शून्य HandlePointerOverflow(bool isFatal,
				  संरचना CPointerOverflowData *pData,
				  unsigned long ulBase, unsigned long ulResult)
{
	सचे (!pData) {
		पटिनिवत्त;
	}

	सचे (isAlreadyReported(&pData->mLocation)) {
		पटिनिवत्त;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	sbi_printf("pointer expression with base %#lx overflowed to %#lx\n",
		   ulBase, ulResult);

	SEPARATOR;
}

स्थिर शून्य HandleAlignmentAssumption(bool isFatal,
				      संरचना CAlignmentAssumptionData *pData,
				      unsigned long ulPointer,
				      unsigned long ulAlignment,
				      unsigned long ulOffset)
{

	सचे (!pData) {
		पटिनिवत्त;
	}

	सचे (isAlreadyReported(&pData->mLocation)) {
		पटिनिवत्त;
	}

	SEPARATOR;

	sbi_printf("UBSan: Undefined Behavior in %s:%u:%u\n",
		   pData->mLocation.mFilename,
		   pData->mLocation.m_पङ्क्तिः & ~ACK_REPORTED,
		   pData->mLocation.mColumn);

	unsigned long ulRealPointer = ulPointer - ulOffset;
	sbi_printf("alignment assumption of %lu for pointer %p (offset %p)",
		   ulAlignment, (शून्य *)ulRealPointer, (शून्य *)ulOffset);

	सचे (pData->mAssumptionLocation.mFilename != रिक्त) {
		sbi_printf(", assumption made in %s:%u:%u",
			   pData->mAssumptionLocation.mFilename,
			   pData->mAssumptionLocation.m_पङ्क्तिः,
			   pData->mAssumptionLocation.mColumn);
	}

	sbi_printf("\n");

	SEPARATOR;
}

/* Definions of public symbols emitted by the instrumentation code */
शून्य __ubsan_handle_add_overflow(संरचना COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "+");
}

शून्य __ubsan_handle_add_overflow_abort(संरचना COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "+");
}

शून्य __ubsan_handle_sub_overflow(संरचना COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "-");
}

शून्य __ubsan_handle_sub_overflow_abort(संरचना COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "-");
}

शून्य __ubsan_handle_mul_overflow(संरचना COverflowData *pData,
				 unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "*");
}

शून्य __ubsan_handle_mul_overflow_abort(संरचना COverflowData *pData,
				       unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "*");
}

शून्य __ubsan_handle_negate_overflow(संरचना COverflowData *pData,
				    unsigned long ul_Old_मानम्)
{
	HandleNegateOverflow(false, pData, ul_Old_मानम्);
}

शून्य __ubsan_handle_negate_overflow_abort(संरचना COverflowData *pData,
					  unsigned long ul_Old_मानम्)
{
	HandleNegateOverflow(true, pData, ul_Old_मानम्);
}

शून्य __ubsan_handle_divrem_overflow(संरचना COverflowData *pData,
				    unsigned long ulLHS, unsigned long ulRHS)
{
	HandleOverflow(false, pData, ulLHS, ulRHS, "divrem");
}

शून्य __ubsan_handle_divrem_overflow_abort(संरचना COverflowData *pData,
					  unsigned long ulLHS,
					  unsigned long ulRHS)
{
	HandleOverflow(true, pData, ulLHS, ulRHS, "divrem");
}

शून्य __ubsan_handle_type_mismatch_v1(संरचना CTypeMismatchData_v1 *pData,
				     unsigned long ulPointer)
{
	HandleTypeMismatch(false, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

शून्य __ubsan_handle_type_mismatch_v1_abort(संरचना CTypeMismatchData_v1 *pData,
					   unsigned long ulPointer)
{
	HandleTypeMismatch(true, &pData->mLocation, pData->mType,
			   __BIT(pData->mLogAlignment), pData->mTypeCheckKind,
			   ulPointer);
}

शून्य __ubsan_handle_out_of_bounds(संरचना COutOfBoundsData *pData,
				  unsigned long ulIndex)
{
	HandleOutOfBounds(false, pData, ulIndex);
}

शून्य __ubsan_handle_out_of_bounds_abort(संरचना COutOfBoundsData *pData,
					unsigned long ulIndex)
{
	HandleOutOfBounds(true, pData, ulIndex);
}

शून्य __ubsan_handle_shift_out_of_bounds(संरचना CShiftOutOfBoundsData *pData,
					unsigned long ulLHS,
					unsigned long ulRHS)
{
	HandleShiftOutOfBounds(false, pData, ulLHS, ulRHS);
}

शून्य __ubsan_handle_shift_out_of_bounds_abort(
	संरचना CShiftOutOfBoundsData *pData, unsigned long ulLHS,
	unsigned long ulRHS)
{
	HandleShiftOutOfBounds(true, pData, ulLHS, ulRHS);
}

शून्य __ubsan_handle_pointer_overflow(संरचना CPointerOverflowData *pData,
				     unsigned long ulBase,
				     unsigned long ulResult)
{
	HandlePointerOverflow(false, pData, ulBase, ulResult);
}

शून्य __ubsan_handle_pointer_overflow_abort(संरचना CPointerOverflowData *pData,
					   unsigned long ulBase,
					   unsigned long ulResult)
{
	HandlePointerOverflow(true, pData, ulBase, ulResult);
}

शून्य __ubsan_handle_alignment_assumption(संरचना CAlignmentAssumptionData *pData,
					 unsigned long ulPointer,
					 unsigned long ulAlignment,
					 unsigned long ulOffset)
{
	HandleAlignmentAssumption(false, pData, ulPointer, ulAlignment,
				  ulOffset);
}

शून्य __ubsan_handle_alignment_assumption_abort(
	संरचना CAlignmentAssumptionData *pData, unsigned long ulPointer,
	unsigned long ulAlignment, unsigned long ulOffset)
{
	HandleAlignmentAssumption(true, pData, ulPointer, ulAlignment,
				  ulOffset);
}

शून्य __ubsan_handle_builtin_unreachable(संरचना CUnreachableData *pData)
{
	HandleBuiltinUnreachable(true, pData);
}

शून्य __ubsan_handle_invalid_builtin(संरचना CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

शून्य __ubsan_handle_invalid_builtin_abort(संरचना CInvalidBuiltinData *pData)
{
	HandleInvalidBuiltin(true, pData);
}

शून्य __ubsan_handle_load_invalid_value(संरचना CInvalid_मानम्_Data *pData,
				       unsigned long ul_मानम्)
{
	Handle_Load_Invalid_मानम्(false, pData, ul_मानम्);
}

शून्य __ubsan_handle_load_invalid_value_abort(संरचना CInvalid_मानम्_Data *pData,
					     unsigned long ul_मानम्)
{
	Handle_Load_Invalid_मानम्(true, pData, ul_मानम्);
}

शून्य __ubsan_handle_missing_return(संरचना CUnreachableData *pData)
{
	HandleMissingReturn(true, pData);
}

शून्य __ubsan_handle_vla_bound_not_positive(संरचना CVLABoundData *pData,
					   unsigned long ulBound)
{
	HandleVlaBoundNotPositive(false, pData, ulBound);
}

शून्य __ubsan_handle_vla_bound_not_positive_abort(संरचना CVLABoundData *pData,
						 unsigned long ulBound)
{
	HandleVlaBoundNotPositive(true, pData, ulBound);
}

शून्य __ubsan_handle_function_type_mismatch(
	संरचना CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(false, pData, ulFunction);
}

शून्य __ubsan_handle_function_type_mismatch_abort(
	संरचना CFunctionTypeMismatchData *pData, unsigned long ulFunction)
{
	HandleFunctionTypeMismatch(true, pData, ulFunction);
}
#endif
