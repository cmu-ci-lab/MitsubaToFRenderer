/*
 * mex_utils.h
 *
 *  Created on: Jan 30, 2013
 *      Author: igkiou
 */

#ifndef MEX_UTILS_H_
#define MEX_UTILS_H_

#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

/*
 * TODO: Replace mex includes with extern declarations, to avoid namespace
 * contamination.
 * TODO: Add a nargin/out checker.
 */
#include "mex.h"
#include "matrix.h"

/*
 * TODO: Add struct array support.
 * TODO: Add support for initialization by const mxArray*. Probably will need to
 * create const_MxArray class and class hierarchy.
 * TODO: Is giving access to a data element with [] in MxCell and MxStruct
 * equivalent to using mxSetCell and mxSetField, respectively?
 * TODO: Add general matrix views to avoid permute when possible?
 * TODO: Maybe add support for mxSetData? Would be unsafe.
 * TODO: Find way to do safe resource management when a malloc occurs
 * (construction is not done by an already existing PMxArrayNative). Maybe using
 * some smart (unique) pointer, or using ownwer member.
 * TODO: Update to take advantage of C++11 move semantics, especially for better
 * safety.
 * TODO: Provide iterators for easy use with STL and new C++11 loops.
 * TODO: Maybe provide StringCell specialization.
 * TODO: When creating from raw mxArray*, add assert to check that data is not
 * null.
 * TODO: Find way to remove call to get_class, or change its return type to
 * MxClass.
 */
namespace mex {

#ifdef NDEBUG
#define mexAssert(cond) ((void) 0)
#define mexAssertEx(cond, explanation) ((void) 0)
#else
#define mexAssert(cond) do { \
		if (!(cond)) \
		mexErrMsgIdAndTxt("MATLAB:mex", "Assertion \"%s\" failed in %s:%i\n", \
		#cond, __FILE__, __LINE__); \
	} while (0)

#define mexAssertEx(cond, explanation) do { \
		if (!(cond)) \
		mexErrMsgIdAndTxt(\
		"MATLAB:mex", "Assertion \"%s\" failed in %s:%i (" explanation ")\n", \
		#cond, __FILE__, __LINE__); \
	} while (0)
#endif

struct MxClass {
	static constexpr mxClassID m_classId = mxUNKNOWN_CLASS;
};


/*
 * Specialization for MATLAB declared types. On Linux x86_64, the
 * correspondences are:
 * 		UINT8_T		-	unsigned char
 * 		INT8_T		-	char
 * 		INT16_T		-	short
 * 		UINT16_T	-	unsigned short
 * 		INT32_T		-	int
 * 		UINT32_T	-	unsigned int
 * 		INT64_T		-	long
 * 		UINT64_T	-	unsigned long
 * 		single		-	float
 * 		double		-	double
 * 		mxLogical	-	bool
 */

template <typename NumericType>
struct MxNumericClass;

template <> struct MxNumericClass<UINT8_T> : public MxClass {
	static constexpr mxClassID m_classId = mxUINT8_CLASS;
};
template <> struct MxNumericClass<INT8_T> : public MxClass {
	static constexpr mxClassID m_classId = mxINT8_CLASS;
};
template <> struct MxNumericClass<UINT16_T> : public MxClass {
	static constexpr mxClassID m_classId = mxUINT16_CLASS;
};
template <> struct MxNumericClass<INT16_T> : public MxClass {
	static constexpr mxClassID m_classId = mxINT16_CLASS;
};
template <> struct MxNumericClass<UINT32_T> : public MxClass {
	static constexpr mxClassID m_classId = mxUINT32_CLASS;
};
template <> struct MxNumericClass<INT32_T> : public MxClass {
	static constexpr mxClassID m_classId = mxINT32_CLASS;
};
template <> struct MxNumericClass<UINT64_T> {
	static constexpr mxClassID m_classId = mxUINT64_CLASS;
};
template <> struct MxNumericClass<INT64_T> : public MxClass {
	static constexpr mxClassID m_classId = mxINT64_CLASS;
};
template <> struct MxNumericClass<float> : public MxClass {
	static constexpr mxClassID m_classId = mxSINGLE_CLASS;
};
template <> struct MxNumericClass<double> : public MxClass {
	static constexpr mxClassID m_classId = mxDOUBLE_CLASS;
};
template <> struct MxNumericClass<mxLogical> : public MxClass {
	static constexpr mxClassID m_classId = mxLOGICAL_CLASS;
};

struct MxStringClass : public MxClass {
	static constexpr mxClassID m_classId = mxCHAR_CLASS;
};

struct MxCellClass : public MxClass {
	static constexpr mxClassID m_classId = mxCELL_CLASS;
};

struct MxStructClass : public MxClass {
	static constexpr mxClassID m_classId = mxSTRUCT_CLASS;
};

namespace detail {
using PMxArrayNative = mxArray*;
}  // namespace detail

class MxArray {
public:
	MxArray() = default;

	/*
	 * The following only copy pointer, and do not perform "deep copy". Also,
	 * care must be taken when using copy constructor and assignment, to avoid
	 * slicing of derived classes.
	 */
	MxArray(const MxArray& other) = default;
	MxArray& operator=(const MxArray& other) = default;

	MxArray(MxArray&& other) = default;
	MxArray& operator=(MxArray&& other) = default;

	/*
	 * Only copies pointer, does not do "deep copy".
	 */
	explicit MxArray(const detail::PMxArrayNative array) :
			m_array(array) {}

	inline detail::PMxArrayNative get_array() const {
		return m_array;
	}

	inline detail::PMxArrayNative get_array() {
		return m_array;
	}

	inline mxClassID getClass() const {
		return mxGetClassID(m_array);
	}

	template <typename IndexType>
	inline IndexType getNumberOfElements() const {
		return static_cast<IndexType>(mxGetNumberOfElements(get_array()));
	}

	inline int getNumberOfElements() const {
		return getNumberOfElements<int>();
	}

	template <typename IndexType>
	inline IndexType size() const {
		return getNumberOfElements<IndexType>();
	}

	inline int size() const {
		return size<int>();
	}

	template <typename IndexType>
	inline IndexType getNumberOfRows() const {
		return static_cast<IndexType>(mxGetM(get_array()));
	}

	inline int getNumberOfRows() const {
		return getNumberOfRows<int>();
	}

	template <typename IndexType>
	inline IndexType getNumberOfColumns() const {
		return static_cast<IndexType>(mxGetN(get_array()));
	}

	inline int getNumberOfColumns() const {
		return getNumberOfColumns<int>();
	}

	template <typename IndexType>
	inline IndexType getNumberOfDimensions() const {
		return static_cast<IndexType>(mxGetNumberOfDimensions(get_array()));
	}

	inline int getNumberOfDimensions() const {
		return getNumberOfDimensions<int>();
	}

	template <typename IndexType>
	inline std::vector<IndexType> getDimensions() const {
		const size_t numberOfDimensions = getNumberOfDimensions<size_t>();
		const mwSize* tempDims = mxGetDimensions(get_array());
		return std::vector<IndexType>(tempDims, tempDims + numberOfDimensions);
	}

	inline std::vector<int> getDimensions() const {
		return getDimensions<int>();
	}

	inline bool isEmpty() const {
		return mxIsEmpty(get_array());
	}

	template <typename NumericType>
	inline bool isNumeric() const {
		return (getClass() == MxNumericClass<NumericType>::m_classId);
	}

	inline bool isString() const {
		return (getClass() == MxStringClass::m_classId);
	}

	inline bool isCell() const {
		return (getClass() == MxCellClass::m_classId);
	}

	inline bool isStruct() const {
		return (getClass() == MxStructClass::m_classId);
	}

	inline void destroy() {
		mxDestroyArray(get_array());
	}

	virtual ~MxArray() = default;

private:
	detail::PMxArrayNative m_array;
};

namespace detail {
using PMxArray = MxArray*;
using array2D = std::array<mwSize, 2>;
}  // namespace detail

template <typename NumericType>
class MxNumeric : public MxArray {
public:
	MxNumeric() = default;
	MxNumeric(const MxNumeric<NumericType>& other) = default;
	MxNumeric<NumericType>& operator=(const MxNumeric<NumericType>& other)
																	= default;
	MxNumeric(MxNumeric<NumericType>&& other) = default;
	MxNumeric<NumericType>& operator=(MxNumeric<NumericType>&& other) = default;

	explicit MxNumeric(const detail::PMxArrayNative array) :
			MxArray(array) {
		mexAssert(MxNumericClass<NumericType>::m_classId == mxGetClassID(array));
	}

	/*
	 * TODO: Find way to avoid the dims cast when mwSize IndexType is used.
	 */
	template <typename IndexType>
	MxNumeric(const NumericType* arrVar,
			const IndexType numDims,
			const IndexType *dims) :
			MxNumeric(mxCreateNumericArray(static_cast<mwSize>(numDims),
										std::vector<mwSize>(dims,
															dims + numDims)
															.data(),
										MxNumericClass<NumericType>::m_classId,
										mxREAL)) {
		if (arrVar != nullptr) {
			NumericType *val = static_cast<NumericType*>(mxGetData(
																get_array()));
			std::memcpy(static_cast<void*>(val),
						static_cast<const void*>(arrVar),
						getNumberOfElements<size_t>() * sizeof(NumericType));
		}
	}

	template <typename IndexType>
	MxNumeric(const IndexType numDims, const IndexType *dims) :
			MxNumeric(nullptr, numDims, dims) {}

	template <typename IndexType>
	MxNumeric(const IndexType numRows, const IndexType numColumns) :
			MxNumeric(nullptr, static_cast<mwSize>(2),
					detail::array2D{numRows, numColumns}.data()) {}

	template <typename IndexType>
	MxNumeric(const NumericType* arrVar, const IndexType numRows,
			const IndexType numColumns) :
			MxNumeric(arrVar, static_cast<mwSize>(2),
					detail::array2D{numRows, numColumns}.data()) {}

	explicit MxNumeric(const std::vector<NumericType>& vecVar) :
			MxNumeric(vecVar.data(), static_cast<mwSize>(2),
					detail::array2D{vecVar.size(), 1}.data()) {}

	template <std::size_t ArraySize>
	explicit MxNumeric(const std::array<NumericType, ArraySize>& arrVar) :
			MxNumeric(arrVar.data(), static_cast<mwSize>(2),
					detail::array2D{ArraySize, 1}.data()) {}

	explicit MxNumeric(const NumericType scalarVar) :
			MxNumeric(&scalarVar, static_cast<mwSize>(2),
					detail::array2D{1, 1}.data()) {}

	template <typename IndexType>
	MxNumeric(const NumericType* arrVar, const std::vector<IndexType>& dims) :
			MxNumeric(arrVar, dims.size(), dims.data()) {}

	template <typename IndexType>
	inline NumericType& operator[](IndexType i) {
		mexAssert(i < getNumberOfElements<IndexType>());
		NumericType* temp = static_cast<NumericType*>(mxGetData(get_array()));
		return temp[i];
	}

	template <typename IndexType>
	inline const NumericType& operator[](IndexType i) const {
		mexAssert(i < getNumberOfElements<IndexType>());
		NumericType* temp = static_cast<NumericType*>(mxGetData(get_array()));
		return temp[i];
	}

	inline const NumericType* getData() const {
		return static_cast<const NumericType*>(mxGetData(get_array()));
	}

	inline NumericType* getData() {
		return static_cast<NumericType*>(mxGetData(get_array()));
	}

	inline std::vector<NumericType> vectorize() const {
		int numel = getNumberOfElements();
		const NumericType* pData = getData();
		return std::vector<NumericType>(pData, pData + numel);
	}

	template <typename IndexType>
	inline std::vector<IndexType> ind2sub(IndexType index) const {
		mexAssert(index < getNumberOfElements<IndexType>());
		std::vector<IndexType> dimensions = getDimensions<IndexType>();
		std::vector<IndexType> subscript(getNumberOfDimensions<size_t>());
		IndexType sliceSize = getNumberOfElements<IndexType>();
		for (int iter = 0, end = getNumberOfDimensions(); iter < end; ++iter) {
			sliceSize /= dimensions[end - 1 - iter];
			subscript[end - 1 - iter] = index / sliceSize;
			index %= sliceSize;
		}
		return subscript;
	}

	template <typename IndexType>
	inline IndexType sub2ind(const std::vector<IndexType>& subscript) const {
		std::vector<IndexType> dimensions = getDimensions<IndexType>();
		mexAssert(subscript.size() == getNumberOfDimensions<size_t>());
		for (int iter = 0, end = getNumberOfDimensions();
			iter < end;
			++iter) {
			mexAssert(subscript[iter] < dimensions[iter]);
		}
		IndexType index = 0;
		IndexType sliceSize = getNumberOfElements<IndexType>();
		for (int iter = 0, end = getNumberOfDimensions(); iter < end; ++iter) {
			sliceSize /= dimensions[end - 1 - iter];
			index += subscript[end - 1 - iter] * sliceSize;
		}
		return index;
	}

	/*
	 * TODO: Very cache-unfriendly and out-of-place multidimensional permute.
	 */
	template <typename IndexType>
	MxNumeric<NumericType> permute(
								const std::vector<IndexType>& indexPermutation)
								const {
		mexAssert(isIndexPermutation(indexPermutation));
		const std::vector<IndexType> dimensions = getDimensions<IndexType>();
		const std::vector<IndexType> permutedDimensions = permuteIndexVector(
															dimensions,
															indexPermutation);
		MxNumeric<NumericType> retArg(static_cast<IndexType>(
													permutedDimensions.size()),
									&permutedDimensions[0]);
		const NumericType* thisData = getData();
		NumericType* otherData = retArg.getData();
		for (IndexType iter = 0,
			end = static_cast<IndexType>(getNumberOfElements());
			iter < end;
			++iter) {
			otherData[retArg.sub2ind(permuteIndexVector(ind2sub(iter),
														indexPermutation))]
			          	  	  	  	  	  	  	  	  	= thisData[iter];
		}
		return retArg;
	}

	virtual ~MxNumeric() = default;

private:
	template <typename IndexType>
	inline bool isIndexPermutation(
								const std::vector<IndexType>& indexPermutation)
								const {
		if (indexPermutation.size() != getNumberOfDimensions<size_t>()) {
			return false;
		}
		std::vector<IndexType> indexVector;
		for (IndexType iter = 0,
			end = getNumberOfDimensions<IndexType>();
			iter < end;
			++iter) {
			indexVector.push_back(iter + 1);
		}
		typename std::vector<IndexType>::const_iterator first(
													indexPermutation.begin());
		typename std::vector<IndexType>::const_iterator last(
													indexPermutation.end());
		typename std::vector<IndexType>::iterator d_first(indexVector.begin());
		if (first != last) {
			typename std::vector<IndexType>::iterator d_last = d_first;
			std::advance(d_last, std::distance(first, last));
			for (typename std::vector<IndexType>::const_iterator i = first;
				i != last;
				++i) {
				if (i != std::find(first, i, *i)) {
					continue;  // already counted this *i
				}

				IndexType m = std::count(d_first, d_last, *i);
				if (m == 0 || std::count(i, last, *i) != m) {
					return false;
				}
			}
		}
		return true;
	}

	template <typename IndexType>
	inline std::vector<IndexType> permuteIndexVector(
							const std::vector<IndexType>& indexVector,
							const std::vector<IndexType>& indexPermutation)
							const {
		std::vector<IndexType> permutedIndexVector(indexVector.size(), 0);
		for (IndexType iter = 0, end = indexVector.size(); iter < end; ++iter) {
			permutedIndexVector[iter] = indexVector[indexPermutation[iter] - 1];
		}
		return permutedIndexVector;
	}
};

template <>
inline MxNumeric<bool>::MxNumeric(const std::vector<bool>& vecVar) :
		MxNumeric(static_cast<mwSize>(2),
				detail::array2D{vecVar.size(), 1}.data()) {
	bool* dataArray = getData();
	int index = 0;
	for (auto iter = std::begin(vecVar); iter < std::end(vecVar); ++iter) {
		dataArray[index++] = *iter;
	}
}

template <> template <std::size_t ArraySize>
inline MxNumeric<bool>::MxNumeric(const std::array<bool, ArraySize>& arrVar) :
		MxNumeric(static_cast<mwSize>(2),
				detail::array2D{ArraySize, 1}.data()) {
	bool* dataArray = getData();
	int index = 0;
	for (auto iter = std::begin(arrVar); iter < std::end(arrVar); ++iter) {
		dataArray[index++] = *iter;
	}
}

/*
 * TODO: For memory safety reasons, and because of MATLAB's internal
 * representation for "string-arrays", this is relatively memory inefficient.
 */
class MxString: public MxArray {
public:

	MxString() = default;
	MxString(const MxString& other) = default;
	MxString& operator=(const MxString& other) = default;
	MxString(MxString&& other) = default;
	MxString& operator=(MxString&& other) = default;

	friend void swap(MxString& first, MxString& second) {
		using std::swap;
		swap(static_cast<MxArray&>(first), static_cast<MxArray&>(second));
		swap(first.m_string, second.m_string);
	}

	explicit MxString(const detail::PMxArrayNative array) :
			MxArray(array), m_string() {
		mexAssert(MxStringClass::m_classId == mxGetClassID(array));
		char* temp = mxArrayToString(get_array());
		m_string = std::string(temp);
		mxFree(temp);
	}

	explicit MxString(const char* cString) :
			MxArray(mxCreateString(cString)), m_string(cString) {}

	explicit MxString(const std::string& string) :
			MxString(string.c_str()) {}

	inline void clone(const MxString& other) {
		mexAssert(getDimensions() == other.getDimensions());
		mxChar *destination = static_cast<mxChar*>(mxGetData(get_array()));
		const mxChar *origin = static_cast<const mxChar*>(
											mxGetData(other.get_array()));
		std::memcpy(static_cast<void*>(destination),
			static_cast<const void*>(origin),
			getNumberOfElements<size_t>() * sizeof(mxChar));
		m_string = other.get_string();
	}

	inline const std::string& get_string() const {
		return m_string;
	}

	inline const char* c_str() const {
		return m_string.c_str();
	}

	template <typename IndexType>
	inline char& operator[](IndexType i) {
		mexAssert(i < length<IndexType>());
		return m_string[i];
	}

	template <typename IndexType>
	inline const char* operator[](IndexType i) const {
		mexAssert(i < length<IndexType>());
		return m_string[i];
	}

	template <typename IndexType>
	inline IndexType length() const {
		return getNumberOfElements<IndexType>();
	}

	inline int length() const {
		return getNumberOfElements<int>();
	}

	virtual ~MxString() = default;

private:
	std::string m_string;
};

class MxCell : public MxArray {
public:

	MxCell() = default;
	MxCell(const MxCell& other) = default;
	MxCell& operator=(const MxCell& other) = default;
	MxCell(MxCell&& other) = default;
	MxCell& operator=(MxCell&& other) = default;

	/*
	 * TODO: Because of this, initialization from scalar PMxArrayNative is
	 * disabled.
	 */
	explicit MxCell(const detail::PMxArrayNative array) :
			MxArray(array) {
		mexAssert(MxCellClass::m_classId == mxGetClassID(array));
	}

	template <typename IndexType>
	MxCell(const detail::PMxArrayNative* arrVar, const IndexType numDims,
			const IndexType *dims) :
			MxCell(mxCreateCellArray(static_cast<mwSize>(numDims),
									std::vector<mwSize>(dims, dims + numDims)
														.data())) {
		if (arrVar != nullptr) {
			for (int iter = 0; iter < getNumberOfElements(); ++iter) {
				mxSetCell(get_array(), iter, arrVar[iter]);
			}
		}
	}

	template <typename IndexType>
	MxCell(const detail::PMxArray* arrVar, const IndexType numDims,
			const IndexType *dims) :
			MxCell(mxCreateCellArray(static_cast<mwSize>(numDims),
									std::vector<mwSize>(dims, dims + numDims)
														.data())) {
		if (arrVar != nullptr) {
			for (int iter = 0; iter < getNumberOfElements(); ++iter) {
				mxSetCell(get_array(), iter, arrVar[iter]->get_array());
			}
		}
	}

	template <typename IndexType>
	MxCell(const IndexType numDims, const IndexType *dims) :
			MxCell(nullptr, numDims, dims) {}

	template <typename IndexType>
	MxCell(const IndexType numRows, const IndexType numColumns) :
			MxCell(nullptr, static_cast<mwSize>(2),
				detail::array2D{numRows, numColumns}.data()) {}

	template <typename IndexType>
	MxCell(const detail::PMxArrayNative* arrVar, const IndexType numRows,
			const IndexType numColumns) :
			MxCell(arrVar, static_cast<mwSize>(2),
				detail::array2D{numRows, numColumns}.data()) {}

	template <typename IndexType>
	MxCell(const detail::PMxArray* arrVar, const IndexType numRows,
			const IndexType numColumns) :
			MxCell(arrVar, static_cast<mwSize>(2),
				detail::array2D{numRows, numColumns}.data()) {}

	explicit MxCell(const std::vector<detail::PMxArray>& vecVar) :
			MxCell(vecVar.data(), static_cast<mwSize>(2),
				detail::array2D{vecVar.size(), 1}.data()) {}

	template <std::size_t ArraySize>
	explicit MxCell(const std::array<detail::PMxArray, ArraySize>& arrVar) :
			MxCell(arrVar.data(), static_cast<mwSize>(2),
				detail::array2D{ArraySize, 1}.data()) {}

	/*
	 * TODO: Remember to change this if I add constructor from MxArray to all
	 * derived classes of MxArray.
	 */
	explicit MxCell(const detail::PMxArray scalarVar) :
			MxCell(&scalarVar, static_cast<mwSize>(2),
				detail::array2D{1, 1}.data()) {}

	template <typename IndexType>
	MxCell(const detail::PMxArrayNative* arrVar,
			const std::vector<IndexType>& dims) :
			MxCell(arrVar, dims.size(), dims.data()) {}

	template <typename IndexType>
	MxCell(const detail::PMxArray* arrVar, const std::vector<IndexType>& dims) :
			MxCell(arrVar, dims.size(), dims.data()) {}

	/*
	 * TODO: The following field access operators (getData, [], and vectorize)
	 * require some thought for the const case. Should I make the pointers const
	 * too?
	 */
	template <typename IndexType>
	inline detail::PMxArrayNative operator[](IndexType i) {
		mexAssert(i < getNumberOfElements<IndexType>());
		return mxGetCell(get_array(), i);
	}

	template <typename IndexType>
	inline detail::PMxArrayNative operator[](IndexType i) const {
		mexAssert(i < getNumberOfElements<IndexType>());
		return mxGetCell(get_array(), i);
	}

	inline const detail::PMxArrayNative* getData() const {
		return static_cast<const detail::PMxArrayNative*>(mxGetData(
																get_array()));
	}

	inline detail::PMxArrayNative* getData() {
		return static_cast<detail::PMxArrayNative*>(mxGetData(get_array()));
	}

	inline const std::vector<detail::PMxArrayNative> vectorize() const {
		std::vector<detail::PMxArrayNative> retArg;
		for (int iter = 0, end = getNumberOfElements(); iter < end; ++iter) {
			retArg.push_back((mxGetCell(get_array(), iter)));
		}
		return retArg;
	}

	inline std::vector<detail::PMxArrayNative> vectorize() {
		std::vector<detail::PMxArrayNative> retArg;
		for (int iter = 0, end = getNumberOfElements(); iter < end; ++iter) {
			retArg.push_back((mxGetCell(get_array(), iter)));
		}
		return retArg;
	}

	virtual ~MxCell() = default;
};

class MxStruct : public MxArray {
public:
	MxStruct() = default;
	MxStruct(const MxStruct& other) = default;
	MxStruct& operator=(const MxStruct& other) = default;
	MxStruct(MxStruct&& other) = default;
	MxStruct& operator=(MxStruct&& other) = default;

	explicit MxStruct(const detail::PMxArrayNative array) :
			MxArray(array) {
		mexAssert(MxStructClass::m_classId == mxGetClassID(array));
	}

	MxStruct(const std::vector<std::string>& vecName,
			const std::vector<detail::PMxArray>& vecVar) :
			MxStruct((vecName.size() == vecVar.size())
					?(mxCreateStructMatrix(static_cast<mwSize>(1),
										static_cast<mwSize>(1),
										static_cast<mwSize>(0),
										nullptr))
					:(nullptr)) {
		mexAssert(get_array() != nullptr);
		addField_sub(vecName.data(), vecVar.data(),
					static_cast<mwSize>(vecName.size()));
	}

//	MxStruct(const std::string& scalarName,
//			const detail::PMxArrayNative scalarVar) :
//			MxStruct(std::vector<std::string>(1, scalarName),
//					std::vector<detail::PMxArrayNative>(1, scalarVar)) {}

	MxStruct(const std::string& scalarName, const detail::PMxArray scalarVar) :
			MxStruct(std::vector<std::string>(1, scalarName),
					std::vector<detail::PMxArray>(1, scalarVar)) {}

	void addField(const std::string& scalarName,
				const detail::PMxArrayNative scalarVar) {
		addField_sub(&scalarName, &scalarVar, static_cast<mwSize>(1));
	}

	void addField(const std::string& scalarName,
				const detail::PMxArray scalarVar) {
		addField_sub(&scalarName, &scalarVar, static_cast<mwSize>(1));
	}

	void addField(const std::vector<std::string>& vecName,
				const std::vector<detail::PMxArrayNative>& vecVar) {
		mexAssert(vecVar.size() == vecName.size());
		addField_sub(&vecName[0], &vecVar[0], vecVar.size());
	}

	void addField(const std::vector<std::string>& vecName,
				const std::vector<detail::PMxArray>& vecVar) {
		mexAssert(vecVar.size() == vecName.size());
		addField_sub(&vecName[0], &vecVar[0], vecVar.size());
	}

	inline bool isField(const std::string& name) const {
		return (mxGetFieldNumber(get_array(), name.c_str()) != -1);
	}

	template <typename IndexType>
	inline IndexType getNumberOfFields() const {
		return static_cast<IndexType>(mxGetNumberOfFields(get_array()));
	}

	inline int getNumberOfFields() const {
		return getNumberOfFields<int>();
	}

	template <typename IndexType>
	inline IndexType getFieldNumber(const std::string& name) const {
		return static_cast<IndexType>(mxGetFieldNumber(get_array(),
													name.c_str()));
	}

	inline int getFieldNumber(const std::string& name) const {
		return getFieldNumber<int>(name);
	}

	template <typename IndexType>
	inline std::string getFieldName(const IndexType i) const {
		return std::string(mxGetFieldNameByNumber(get_array(),
												static_cast<int>(i)));
	}

	inline std::vector<std::string> getFieldNames() const {
		std::vector<std::string> retArg(0);
		for (int iter = 0, end = getNumberOfFields(); iter < end; ++iter) {
			retArg.push_back(std::string(mxGetFieldNameByNumber(get_array(),
															iter)));
		}
		return retArg;
	}

	/*
	 * TODO: The following field access operators (getData, [], and vectorize)
	 * require some thought for the const case. Should I make the pointers const
	 * too?
	 */
	template <typename IndexType>
	inline detail::PMxArrayNative operator[](IndexType i) {
		mexAssert(i < getNumberOfElements());
		return mxGetFieldByNumber(get_array(), 0, static_cast<int>(i));
	}

	template <typename IndexType>
	inline detail::PMxArrayNative operator[](IndexType i) const {
		return mxGetFieldByNumber(get_array(), 0, static_cast<int>(i));
	}

	inline detail::PMxArrayNative operator[](const std::string& name) {
		return mxGetField(get_array(), 0, name.c_str());
	}

	inline detail::PMxArrayNative operator[](const std::string& name) const {
		return mxGetField(get_array(), 0, name.c_str());
	}

	inline const detail::PMxArrayNative* getData() const {
		return static_cast<detail::PMxArrayNative*>(mxGetData(get_array()));
	}

	inline detail::PMxArrayNative* getData() {
		return static_cast<detail::PMxArrayNative*>(mxGetData(get_array()));
	}

	inline void vectorize(std::vector<std::string>& vecName,
						std::vector<detail::PMxArrayNative>& vecVar) const {
		vecName.resize(0);
		vecVar.resize(0);
		for (int iter = 0, end = getNumberOfFields(); iter < end; ++iter) {
			const char* tempName = mxGetFieldNameByNumber(get_array(), iter);
			vecName.push_back(std::string(tempName));
			detail::PMxArrayNative tempVar = mxGetFieldByNumber(get_array(), 0,
																iter);
			vecVar.push_back(tempVar);
		}
	}

	virtual ~MxStruct() = default;
private:
	static const size_t kMxMaxNameLength = mxMAXNAM - 1;

	template <typename IndexType>
	void addField_sub(const std::string* arrName,
					const detail::PMxArrayNative* arrVar,
					IndexType numFields) {
		for (IndexType iter = 0; iter < numFields; ++iter) {
			mexAssert(arrName[iter].size() <= kMxMaxNameLength);
			mxAddField(get_array(), arrName[iter].c_str());
			mxSetField(get_array(), 0, arrName[iter].c_str(), arrVar[iter]);
		}
	}

	template <typename IndexType>
	void addField_sub(const std::string* arrName,
					const detail::PMxArray* arrVar,
					IndexType numFields) {
		for (IndexType iter = 0; iter < numFields; ++iter) {
			mexAssert(arrName[iter].size() <= kMxMaxNameLength);
			mxAddField(get_array(), arrName[iter].c_str());
			mxSetField(get_array(), 0, arrName[iter].c_str(),
					arrVar[iter]->get_array());
		}
	}
};

template <typename T, typename U>
class ConstMap {
public:
	/// Constructor with a single key-value pair
	ConstMap(const T& key, const U& value) :
		m_map() {
		m_map[key] = value;
	}

	/// Consecutive insertion operator
	ConstMap<T, U>& operator()(const T& key, const U& value) {
		m_map[key] = value;
		return *this;
	}

	inline const std::map<T, U>& get_map() const {
		return m_map;
	}

	/// Lookup operator; fail if not found
	inline U operator[](const T& key) const {
		typename std::map<T, U>::const_iterator iter = m_map.find(key);
		mexAssertEx(iter != m_map.end(), "Value not found");
		return (*iter).second;
	}

private:
	std::map<T, U> m_map;
};


template <typename T, typename U>
class ConstBiMap {
public:
	/// Constructor with a single key-value pair
	ConstBiMap(const T& key, const U& value) :
		m_mapLeftToRight(),
		m_mapRightToLeft() {
		m_mapLeftToRight[key] = value;
		m_mapRightToLeft[value] = key;
	}

	/// Consecutive insertion operator
	ConstBiMap<T, U>& operator()(const T& key, const U& value) {
		m_mapLeftToRight[key] = value;
		m_mapRightToLeft[value] = key;
		return *this;
	}

	inline const std::map<T, U>& get_mapLeftToRight() const {
		return m_mapLeftToRight;
	}

	inline const std::map<U, T>& get_mapRightToLeft() const {
		return m_mapRightToLeft;
	}

	/// Lookup operator; fail if not found
	inline U operator[](const T& key) const {
		typename std::map<T, U>::const_iterator iter = m_mapLeftToRight.find(key);
		mexAssertEx(iter != m_mapLeftToRight.end(), "Value not found");
		return (*iter).second;
	}

	inline T find(const U& value) const {
		typename std::map<U, T>::const_iterator iter = m_mapRightToLeft.find(value);
		mexAssertEx(iter != m_mapRightToLeft.end(), "Key not found");
		return (*iter).second;
	}

private:
	std::map<T, U> m_mapLeftToRight;
	std::map<U, T> m_mapRightToLeft;
};

class MxArrayHeader {
public:
	explicit MxArrayHeader(const MxArray& mxArray) :
			m_size(mxArray.size()),
			m_dimensions(mxArray.getDimensions()),
			m_class(mxArray.getClass()) {}

	explicit MxArrayHeader(const detail::PMxArrayNative array) :
			MxArrayHeader(MxArray(array)) {}

	MxArrayHeader() = default;
	MxArrayHeader(const MxArrayHeader& other) = default;
	MxArrayHeader& operator=(const MxArrayHeader& other) = default;
	MxArrayHeader(MxArrayHeader&& other) = default;
	MxArrayHeader& operator=(MxArrayHeader&& other) = default;

	inline std::vector<int> getDimensions() const {
		return m_dimensions;
	}

	inline int getNumberOfDimensions() const {
		return m_dimensions.size();
	}

	inline mxClassID getClass() const {
		return m_class;
	}

	template <typename NumericType>
	inline bool isNumeric() const {
		return (getClass() == MxNumericClass<NumericType>::m_classId);
	}

	inline bool isString() const {
		return (getClass() == MxStringClass::m_classId);
	}

	inline bool isCell() const {
		return (getClass() == MxCellClass::m_classId);
	}

	inline bool isStruct() const {
		return (getClass() == MxStructClass::m_classId);
	}
private:
	const int m_size;
	const std::vector<int> m_dimensions;
	const mxClassID m_class;
};

/*
 * TODO: Better to use copied member or pointer?
 * TODO: Is it OK to have const members here?
 */
struct MxVariableHeader {
	const std::string m_name;
	const MxArrayHeader m_header;
};

struct MxVariable {
	const std::string m_name;
	const MxArray* m_array;
};

//class MxAttributeInterface {
//public:
//
//	virtual MxArray getValue() const = 0;
//	virtual void setValue(const MxArray& value) const = 0;
//	virtual MxString getName() const = 0;
//
//	virtual ~MxAttributeInterface() {};
//
//};

//class MxObjectInterface {
//public:
//
//	virtual MxArray getAttribute(const mex::MxString& attributeName) const = 0;
//	virtual MxArray getAttribute() const const = 0;
//
//	virtual void setAttribute(const mex::MxString& attributeName,
//					const mex::MxArray& attribute) = 0;
//	virtual void setAttribute(const mex::MxStruct& attributes) = 0;
//
//	virtual ~MxObjectInterface() {};
//};

}  // namespace mex

#endif  // MEX_UTILS_H_
