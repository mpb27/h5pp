#pragma once
#include "h5ppOptional.h"
#include "h5ppTypeSfinae.h"
#include <cstring>
#include <numeric>

/*!
 * \brief A collection of functions to get information about C++ types passed by the user
 */
namespace h5pp::utils {

    template<typename DataType>
    [[nodiscard]] hid::h5t getH5Type(const std::optional<hid::h5t> &h5type = std::nullopt) {
        if(h5type.has_value()) return h5type.value();
        namespace tc = h5pp::type::sfinae;
        /* clang-format off */
        using DecayType    = typename std::decay<DataType>::type;
        if constexpr (std::is_pointer_v<DecayType>)                                                     return getH5Type<typename std::remove_pointer<DecayType>::type>();
        if constexpr (std::is_reference_v<DecayType>)                                                   return getH5Type<typename std::remove_reference<DecayType>::type>();
        if constexpr (std::is_array_v<DecayType>)                                                       return getH5Type<typename std::remove_all_extents<DecayType>::type>();
        if constexpr (tc::is_std_vector_v<DecayType>)                                                   return getH5Type<typename DecayType::value_type>();

        if constexpr (std::is_same_v<DecayType, short>)                                                 return H5Tcopy(H5T_NATIVE_SHORT);
        if constexpr (std::is_same_v<DecayType, int>)                                                   return H5Tcopy(H5T_NATIVE_INT);
        if constexpr (std::is_same_v<DecayType, long>)                                                  return H5Tcopy(H5T_NATIVE_LONG);
        if constexpr (std::is_same_v<DecayType, long long>)                                             return H5Tcopy(H5T_NATIVE_LLONG);
        if constexpr (std::is_same_v<DecayType, unsigned short>)                                        return H5Tcopy(H5T_NATIVE_USHORT);
        if constexpr (std::is_same_v<DecayType, unsigned int>)                                          return H5Tcopy(H5T_NATIVE_UINT);
        if constexpr (std::is_same_v<DecayType, unsigned long>)                                         return H5Tcopy(H5T_NATIVE_ULONG);
        if constexpr (std::is_same_v<DecayType, unsigned long long >)                                   return H5Tcopy(H5T_NATIVE_ULLONG);
        if constexpr (std::is_same_v<DecayType, double>)                                                return H5Tcopy(H5T_NATIVE_DOUBLE);
        if constexpr (std::is_same_v<DecayType, long double>)                                           return H5Tcopy(H5T_NATIVE_LDOUBLE);
        if constexpr (std::is_same_v<DecayType, float>)                                                 return H5Tcopy(H5T_NATIVE_FLOAT);
        if constexpr (std::is_same_v<DecayType, bool>)                                                  return H5Tcopy(H5T_NATIVE_HBOOL);
        if constexpr (std::is_same_v<DecayType, std::string>)                                           return H5Tcopy(H5T_C_S1);
        if constexpr (std::is_same_v<DecayType, char>)                                                  return H5Tcopy(H5T_C_S1);
        if constexpr (std::is_same_v<DecayType, std::complex<short>>)                                   return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_SHORT);
        if constexpr (std::is_same_v<DecayType, std::complex<int>>)                                     return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_INT);
        if constexpr (std::is_same_v<DecayType, std::complex<long>>)                                    return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_LONG);
        if constexpr (std::is_same_v<DecayType, std::complex<long long>>)                               return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_LLONG);
        if constexpr (std::is_same_v<DecayType, std::complex<unsigned short>>)                          return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_USHORT);
        if constexpr (std::is_same_v<DecayType, std::complex<unsigned int>>)                            return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_UINT);
        if constexpr (std::is_same_v<DecayType, std::complex<unsigned long>>)                           return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_ULONG);
        if constexpr (std::is_same_v<DecayType, std::complex<unsigned long long>>)                      return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_ULLONG);
        if constexpr (std::is_same_v<DecayType, std::complex<double>>)                                  return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_DOUBLE);
        if constexpr (std::is_same_v<DecayType, std::complex<long double>>)                             return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_LDOUBLE);
        if constexpr (std::is_same_v<DecayType, std::complex<float>>)                                   return H5Tcopy(h5pp::type::compound::H5T_COMPLEX_FLOAT);
        if constexpr (tc::is_Scalar2_of_type<DecayType, short>())                                       return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_SHORT);
        if constexpr (tc::is_Scalar2_of_type<DecayType, int>())                                         return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_INT);
        if constexpr (tc::is_Scalar2_of_type<DecayType, long>())                                        return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_LONG);
        if constexpr (tc::is_Scalar2_of_type<DecayType, long long>())                                   return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_LLONG);
        if constexpr (tc::is_Scalar2_of_type<DecayType, unsigned short>())                              return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_USHORT);
        if constexpr (tc::is_Scalar2_of_type<DecayType, unsigned int>())                                return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_UINT);
        if constexpr (tc::is_Scalar2_of_type<DecayType, unsigned long>())                               return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_ULONG);
        if constexpr (tc::is_Scalar2_of_type<DecayType, unsigned long long>())                          return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_ULLONG);
        if constexpr (tc::is_Scalar2_of_type<DecayType, double>())                                      return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_DOUBLE);
        if constexpr (tc::is_Scalar2_of_type<DecayType, long double>())                                 return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_LDOUBLE);
        if constexpr (tc::is_Scalar2_of_type<DecayType, float>())                                       return H5Tcopy(h5pp::type::compound::H5T_SCALAR2_FLOAT);
        if constexpr (tc::is_Scalar3_of_type<DecayType, short>())                                       return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_SHORT);
        if constexpr (tc::is_Scalar3_of_type<DecayType, int>())                                         return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_INT);
        if constexpr (tc::is_Scalar3_of_type<DecayType, long>())                                        return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_LONG);
        if constexpr (tc::is_Scalar3_of_type<DecayType, long long>())                                   return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_LLONG);
        if constexpr (tc::is_Scalar3_of_type<DecayType, unsigned short>())                              return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_USHORT);
        if constexpr (tc::is_Scalar3_of_type<DecayType, unsigned int>())                                return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_UINT);
        if constexpr (tc::is_Scalar3_of_type<DecayType, unsigned long>())                               return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_ULONG);
        if constexpr (tc::is_Scalar3_of_type<DecayType, unsigned long long>())                          return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_ULLONG);
        if constexpr (tc::is_Scalar3_of_type<DecayType, double>())                                      return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_DOUBLE);
        if constexpr (tc::is_Scalar3_of_type<DecayType, long double>())                                 return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_LDOUBLE);
        if constexpr (tc::is_Scalar3_of_type<DecayType, float>())                                       return H5Tcopy(h5pp::type::compound::H5T_SCALAR3_FLOAT);
        if constexpr (tc::has_Scalar_v <DecayType>)                                                     return getH5Type<typename DecayType::Scalar>();
        if constexpr (tc::has_value_type_v <DecayType>)                                                 return getH5Type<typename DataType::value_type>();

        /* clang-format on */
        h5pp::logger::log->critical("getH5Type could not match the type provided: {}", type::sfinae::type_name<DecayType>());
        throw std::logic_error("getH5Type could not match the type provided: " + std::string(type::sfinae::type_name<DecayType>()));
        return hid_t(0);
    }

    template<typename DataType, size_t size>
    [[nodiscard]] constexpr size_t getArraySize([[maybe_unused]] const DataType (&arr)[size]) {
        if constexpr(h5pp::type::sfinae::is_text_v<DataType>)
            return strlen(arr);
        else
            return size;
    }

    template<typename DataType, typename = std::enable_if_t<not std::is_base_of_v<hid::hid_base<DataType>, DataType>>>
    [[nodiscard]] hsize_t getSize(const DataType &data) {
        namespace sfn = h5pp::type::sfinae;
        if constexpr(sfn::has_size_v<DataType>) return data.size();
        if constexpr(std::is_array_v<DataType>) return getArraySize(data);

        // Add more checks here. As it is, these two checks above handle all cases I have encountered.
        return 1; // All others should be "atomic" of size 1.
    }

    template<typename DataType, typename = std::enable_if_t<not std::is_base_of_v<hid::hid_base<DataType>, DataType>>>
    [[nodiscard]] constexpr int getRank() {
        namespace sfn = h5pp::type::sfinae;
        if constexpr(sfn::has_NumIndices_v<DataType>) return (int) DataType::NumIndices;
#ifdef H5PP_EIGEN3
        if constexpr(sfn::is_eigen_tensor_v<DataType>) return (int) DataType::NumIndices;
        if constexpr(sfn::is_eigen_1d_v<DataType>) return 1;
        if constexpr(sfn::is_eigen_dense_v<DataType>) return 2;
#endif
        return 1;
    }

    template<typename DataType, typename = std::enable_if_t<not std::is_base_of_v<hid::hid_base<DataType>, DataType>>>
    [[nodiscard]] std::vector<hsize_t> getDimensions(const DataType &data) {
        constexpr int ndims = getRank<DataType>();
        if constexpr(h5pp::type::sfinae::has_dimensions_v<DataType>) {
            std::vector<hsize_t> dims(data.dimensions().begin(),
                                      data.dimensions().end()); // We copy because the vectors may not be assignable or may not be implicitly convertible to hsize_t.
            assert(data.dimensions().size() == ndims and "given dimensions do not match detected rank");
            assert(dims.size() == ndims and "copied dimensions do not match detected rank");
            return dims;
        } else if constexpr(h5pp::type::sfinae::is_text_v<DataType>) {
            // Read more about this step here
            // http://www.astro.sunysb.edu/mzingale/io_tutorial/HDF5_simple/hdf5_simple.c
            return {(hsize_t) 1};
        } else if constexpr(h5pp::type::sfinae::has_size_v<DataType> and ndims == 1) {
            return {(hsize_t) data.size()};
        }
#ifdef H5PP_EIGEN3
        else if constexpr(h5pp::type::sfinae::is_eigen_tensor_v<DataType>) {
            assert(data.dimensions().size() == ndims and "given dimensions do not match detected rank");
            std::vector<hsize_t> dims(data.dimensions().begin(),
                                      data.dimensions().end()); // We copy because the vectors may not be assignable or may not be implicitly convertible to hsize_t.
            return dims;
        } else if constexpr(h5pp::type::sfinae::is_eigen_dense_v<DataType>) {
            std::vector<hsize_t> dims(ndims);
            dims[0] = (hsize_t) data.rows();
            dims[1] = (hsize_t) data.cols();
            return dims;
        }
#endif
        else if constexpr(h5pp::type::sfinae::is_ScalarN<DataType>()) {
            return {1};
        } else if constexpr(std::is_array_v<DataType>) {
            return {getArraySize(data)};
        } else if constexpr(std::is_arithmetic_v<DataType> or h5pp::type::sfinae::is_std_complex_v<DataType>) {
            return {1};
        } else {
            h5pp::type::sfinae::print_type_and_exit_compile_time<DataType>();
            std::string error = "getDimensions can't match the type provided: " + h5pp::type::sfinae::type_name<DataType>();
            h5pp::logger::log->critical(error);
            throw std::logic_error(error);
        }
    }

    [[nodiscard]] inline hid::h5s getDataSpace(const hsize_t size, const int ndims, const std::vector<hsize_t> &dims, const H5D_layout_t layout) {
        assert((size_t) ndims == dims.size() and "Dimension mismatch");
        if(size == 0) return H5Screate(H5S_NULL);
        if(layout == H5D_CHUNKED and ndims > 0) {
            // Chunked layout datasets can be extended, which is why their max extent in any dimension is unlimited.
            std::vector<hsize_t> maxDims(ndims, H5S_UNLIMITED);
            return H5Screate_simple(ndims, dims.data(), maxDims.data());
        } else {
            if(ndims == 0)
                return H5Screate(H5S_SCALAR);
            else
                return H5Screate_simple(ndims, dims.data(), nullptr); // nullptr ->  maxDims same as dims
        }
    }

    [[nodiscard]] inline hid::h5s getMemSpace(const hsize_t size, const int ndims, const std::vector<hsize_t> &dims) {
        assert((size_t) ndims == dims.size() and "Dimension mismatch");
        if(size == 0) return H5Screate(H5S_NULL);
        if(ndims == 0)
            return H5Screate(H5S_SCALAR);
        else
            return H5Screate_simple(ndims, dims.data(), nullptr);
    }

    template<typename DataType, typename = std::enable_if_t<not std::is_base_of_v<hid::hid_base<DataType>, DataType>>>
    [[nodiscard]] size_t getBytesPerElem() {
        namespace sfn   = h5pp::type::sfinae;
        using DecayType = typename std::decay<DataType>::type;
        if constexpr(std::is_pointer_v<DecayType>) return getBytesPerElem<typename std::remove_pointer<DecayType>::type>();
        if constexpr(std::is_reference_v<DecayType>) return getBytesPerElem<typename std::remove_reference<DecayType>::type>();
        if constexpr(std::is_array_v<DecayType>) return getBytesPerElem<typename std::remove_all_extents<DecayType>::type>();
        if constexpr(sfn::is_std_complex<DecayType>()) return sizeof(DecayType);
        if constexpr(sfn::is_ScalarN<DecayType>()) return sizeof(DecayType);
        if constexpr(std::is_arithmetic_v<DecayType>) return sizeof(DecayType);
        if constexpr(sfn::has_Scalar_v<DecayType>) return sizeof(typename DecayType::Scalar);
        if constexpr(sfn::has_value_type_v<DecayType>) return sizeof(typename DecayType::value_type);
        return sizeof(std::remove_all_extents_t<DecayType>);
    }

    template<typename DataType, typename = std::enable_if_t<not std::is_base_of_v<hid::hid_base<DataType>, DataType>>>
    [[nodiscard]] size_t getBytesTotal(const DataType &data) {
        return h5pp::utils::getBytesPerElem<DataType>() * h5pp::utils::getSize(data);
    }

    [[nodiscard]] inline H5D_layout_t decideLayout(const size_t bytes, std::optional<H5D_layout_t> desiredLayout = std::nullopt) {
        /*! Depending on the size of this dataset we may benefint from using either
            a contiguous layout (for big non-extendable non-compressible datasets),
            a chunked layout (for extendable and compressible datasets)
            or a compact layout (for tiny datasets).

            Contiguous
            For big non-extendable non-compressible datasets

            Chunked
            Chunking is required for enabling compression and other filters, as well as for
            creating extendible or unlimited dimension datasets. Note that a chunk always has
            the same rank as the dataset and the chunk's dimensions do not need to be factors
            of the dataset dimensions.

            Compact
            A compact dataset is one in which the raw data is stored in the object header of the dataset.
            This layout is for very small datasets that can easily fit in the object header.
            The compact layout can improve storage and access performance for files that have many very
            tiny datasets. With one I/O access both the header and data values can be read.
            The compact layout reduces the size of a file, as the data is stored with the header which
            will always be allocated for a dataset. However, the object header is 64 KB in size,
            so this layout can only be used for very small datasets.
         */

        // First, we check if the user explicitly asked for an extendable dataset.
        if(desiredLayout) return desiredLayout.value();
        // Otherwise we decide based on size
        if(bytes < h5pp::constants::maxSizeCompact)
            return H5D_COMPACT;
        else if(bytes < h5pp::constants::maxSizeContiguous)
            return H5D_CONTIGUOUS;
        else
            return H5D_CHUNKED;
    }

    [[nodiscard]] inline std::vector<hsize_t>
        getDefaultChunkDimensions(const size_t size, const std::vector<hsize_t> &dims, const std::optional<std::vector<hsize_t>> &desiredChunkDims = std::nullopt) {
        // Chunk dimensions are always the same rank as the dataset dimensions.
        // The simplest way to get a reasonable chunk dimension is to pick a slice, by setting one of
        // the dimensions to 1.

        // HDF5 is row-major, meaning that on a rank 3 tensor with indices i,j,k, we get contiguous slices
        // when i is fixed (i.e. chunk dims: 1 x size(j) x size(k)). However since the i,j, and k dim
        // sizes may be significantly different, we may have to reduce the chunk dimension even more.

        // Here is a simple heuristic to find a chunk size:
        // For a square matrix with N elements (N = n x n) reasonable chunk dimensions could be (1 x n),
        // so that each row is a single chunk, and where n = sqrt(N) is the size of a single slice.
        // When generalizing this to other dimensions, we can use sqrt(N) as an estimator of the slice size.

        // For example, a rank 3 tensor with dimensions (100 x 20 x 5) has size N = 10000,
        // with slice size = sqrt(N) = 100. To get reasonable chunk dimensions we reduce dataset
        // dimensions by one starting from the left and stop right before becomming smaller than sqrt(N)
        // In this case we get (1 x 20 x 5)

        // A less trivial example is a tensor with dimensions (20 x 15 x 3 x 8), with N = 7200 and
        // slice size sqrt(N) = 84.8528... The same procedure as before gives us chunk
        // dimensions (1 x 4 x 3 x 8). Note that 1 x 4 x 3 x 8 = 96 which is just slightly bigger
        // than 84.8528...
        if(desiredChunkDims.has_value()) {
            // Check that the desired rank matches dims
            if(dims.size() != desiredChunkDims.value().size()) throw std::runtime_error("Mismatch in rank: desired chunk dimensions and data dimensions");
            return desiredChunkDims.value();
        }
        auto                 slice_size = static_cast<size_t>(std::ceil(std::sqrt(size)));
        std::vector<hsize_t> chunkDims  = dims;
        for(size_t dim = 0; dim < dims.size(); dim++) {
            while(chunkDims[dim] > 1) {
                size_t chunkSize = std::accumulate(chunkDims.begin(), chunkDims.end(), 1, std::multiplies<>());
                if(chunkSize < slice_size)
                    break;
                else
                    chunkDims[dim]--;
            }
        }
        return chunkDims;
    }

}
