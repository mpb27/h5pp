//
// Created by david on 2019-03-01.
//

#ifndef H5PP_FILE_H
#define H5PP_FILE_H
#include <hdf5.h>
#include <hdf5_hl.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <experimental/filesystem>
#include <experimental/type_traits>
#include "h5ppFileCounter.h"
#include "h5ppTypeCheck.h"
#include "h5ppTextra.h"
#include "h5ppLogger.h"
#include "h5ppUtils.h"
#include "h5ppType.h"
#include "h5ppTypeCheck.h"
#include "h5ppDatasetProperties.h"
#include "h5ppAttributeProperties.h"
#include "h5ppHdf5.h"



namespace h5pp{

    namespace fs = std::experimental::filesystem;
    namespace tc = h5pp::Type::Check;
/*!
 \brief Writes and reads data to a binary hdf5-file.
*/
    enum class CreateMode {OPEN,TRUNCATE,RENAME};
    enum class AccessMode {READONLY,READWRITE};

    class File {
    private:
        herr_t      retval;
        fs::path    FileName;       /*!< Filename (possibly relative) and extension, e.g. ../files/output.h5 */
        fs::path    FilePath;       /*!< Full path to the file */
        bool        createDir = true;

        AccessMode accessMode;
        CreateMode createMode;


        size_t      logLevel  = 0;

        //Mpi related constants
        hid_t plist_facc;
        hid_t plist_xfer;
        hid_t plist_lncr;
        hid_t plist_lapl;

    public:

        bool        hasInitialized = false;
        File()=default;

        explicit File(const File & other){
            *this = other;
        }



        File(const std::string FileName_,
                AccessMode accessMode_ = AccessMode::READWRITE,
                CreateMode createMode_ = CreateMode::RENAME,
                size_t logLevel_ = 3)
            :
            FileName(FileName_),
            accessMode(accessMode_),
            createMode(createMode_),
            logLevel(logLevel_)
            {
            h5pp::Logger::setLogger("h5pp",logLevel,false);
            if (accessMode_ == AccessMode::READONLY and createMode_ == CreateMode::TRUNCATE){
                Logger::log->error("Options READONLY and TRUNCATE are incompatible.");
                return;
            }
            setCompression();
            initialize();
        }


        ~File(){
            h5pp::Logger::setLogger("h5pp-exit",logLevel,false);
            try{
                if(h5pp::Counter::ActiveFileCounter::getCount() == 1){
                    H5Pclose(plist_facc);
                    H5Pclose(plist_xfer);
                    H5Pclose(plist_lncr);
                    h5pp::Type::Complex::closeTypes();
                }
                h5pp::Counter::ActiveFileCounter::decrementCounter(FileName.string());
                h5pp::Logger::log->debug("Closing file: {}. Remaining {} are: {}", FileName.string(), h5pp::Counter::ActiveFileCounter::getCount(), h5pp::Counter::ActiveFileCounter::OpenFileNames());
            }
            catch (...){
                h5pp::Logger::log->warn("Failed to properly close file: ", get_file_path());
            }
        }

        File & operator= (const File & rhs) {
            if (&rhs != this){
                if(this->hasInitialized){
                    this->~File();
                }
                if(rhs.hasInitialized){
                    this->logLevel            = rhs.logLevel;
                    this->accessMode          = rhs.getAccessMode();
                    this->createMode          = CreateMode::OPEN;
                    this->FileName            = rhs.FilePath;
                    this->createDir           = rhs.createDir;
                    this->setCompression();
                    this->initialize();
                }
            }
            return *this;
        }

        hid_t openFileHandle(){
            switch(accessMode){
                case(AccessMode::READONLY)  : return H5Fopen(FilePath.c_str(), H5F_ACC_RDONLY, plist_facc);
                case(AccessMode::READWRITE) : return H5Fopen(FilePath.c_str(), H5F_ACC_RDWR  , plist_facc);
                default: throw std::runtime_error("Invalid access mode");
            }
        }
        herr_t closeFileHandle(hid_t file){return H5Fclose(file);}


        void setCompression(){
            /*
            * Check if zlib compression is available and can be used for both
            * compression and decompression.  Normally we do not perform error
            * checking in these examples for the sake of clarity, but in this
            * case we will make an exception because this filter is an
            * optional part of the hdf5 library.
            */
            [[maybe_unused]] herr_t          status;
            [[maybe_unused]] htri_t          avail;
            [[maybe_unused]] H5Z_filter_t    filter_type;
            [[maybe_unused]] unsigned int    flags, filter_info;

            avail = H5Zfilter_avail(H5Z_FILTER_DEFLATE);
            if (!avail) {
                h5pp::Logger::log->warn("zlib filter not available");
            }
            status = H5Zget_filter_info (H5Z_FILTER_DEFLATE, &filter_info);
            if ( !(filter_info & H5Z_FILTER_CONFIG_ENCODE_ENABLED) ||
                 !(filter_info & H5Z_FILTER_CONFIG_DECODE_ENABLED) ) {
                h5pp::Logger::log->warn("zlib filter not available for encoding and decoding");
            }
        }

        void setCreateMode(CreateMode createMode_){createMode = createMode_;}
        void setAccessMode(AccessMode accessMode_){accessMode = accessMode_;}
        CreateMode getCreateMode()const{return createMode;}
        AccessMode getAccessMode()const{return accessMode;}



        void setLogLevel(size_t logLevelZeroToSix){
            logLevel = logLevelZeroToSix;
            h5pp::Logger::setLogLevel(logLevelZeroToSix);
        };



        std::string get_file_name()const{return FileName.string();}
        std::string get_file_path()const{return FilePath.string();}


        template <typename DataType>
        void readDataset(DataType &data, const std::string &datasetRelativeName);

        template <typename DataType>
        void writeDataset(const DataType &data, const std::string &datasetRelativeName);

        template <typename DataType>
        void writeDataset(const DataType &data, const DatasetProperties &props);

        template <typename AttrType>
        void writeAttributeToLink(const AttrType &attribute, const AttributeProperties &aprops);

        template <typename AttrType>
        void writeAttributeToLink(const AttrType &attribute, const std::string &attributeName,
                                  const std::string &linkName);

        template <typename AttrType>
        void writeAttributeToFile(const AttrType &attribute, const std::string attributeName);



        inline void create_group_link(const std::string &group_relative_name){
            hid_t file = openFileHandle();
            h5pp::Hdf5::create_group_link(file,plist_lncr,group_relative_name);
            closeFileHandle(file);
        }

//        void create_group_link(const std::string &group_relative_name);
        inline void write_symbolic_link(const std::string &src_path, const std::string &tgt_path){
            hid_t file = openFileHandle();
            h5pp::Hdf5::write_symbolic_link(file,src_path, tgt_path);
            closeFileHandle(file);
        }


        bool linkExists(std::string link){
            hid_t file = openFileHandle();
            bool exists = h5pp::Hdf5::checkIfLinkExistsRecursively(file, link);
            closeFileHandle(file);
            return exists;
        }



//
//        template <typename AttrType>
//        void write_attribute_to_group(const AttrType &attribute, const std::string &attribute_name, const std::string &linkName);
//


        std::vector<std::string> getContentsOfGroup(std::string groupName){
            hid_t file = openFileHandle();
            auto foundLinks = h5pp::Hdf5::getContentsOfGroup(file,groupName);
            closeFileHandle(file);
            return foundLinks;
        }



    private:

        bool fileIsValid(){
            return fileIsValid(FilePath);
        }

        bool fileIsValid(fs::path fileName) {
            if (fs::exists(fileName)){
                if (H5Fis_hdf5(FilePath.c_str()) > 0) {
                    return true;
                } else {
                    return false;
                }
            }else{
                return false;
            }
        }



        fs::path getNewFileName(fs::path fileName){
            int i=1;
            fs::path newFileName = fileName;
            while (fs::exists(newFileName)){
                newFileName.replace_filename(fileName.stem().string() + "-" + std::to_string(i++) + fileName.extension().string() );
            }
            return newFileName;
        }


        void createDatasetLink(hid_t file, const DatasetProperties &props){
            h5pp::Hdf5::createDatasetLink(file, plist_lncr, props);
        }

        void selectHyperslab(const hid_t &filespace, const hid_t &memspace){
            h5pp::Hdf5::select_hyperslab(filespace,memspace);
        }



//
//        template <typename AttrType>
//        void write_attribute_to_group(const AttrType &attribute, const AttributeProperties &aprops);
//
//

        void initialize(){
            h5pp::Logger::setLogger("h5pp-init",logLevel,false);
            plist_facc = H5Pcreate(H5P_FILE_ACCESS);
            plist_lncr = H5Pcreate(H5P_LINK_CREATE);   //Create missing intermediate group if they don't exist
            plist_xfer = H5Pcreate(H5P_DATASET_XFER);
            plist_lapl = H5Pcreate(H5P_LINK_ACCESS);
            H5Pset_create_intermediate_group(plist_lncr, 1);
            setOutputFilePath();
            h5pp::Type::Complex::initTypes();
            hasInitialized = true;
//            fileCount++;
            h5pp::Counter::ActiveFileCounter::incrementCounter(FileName.string());
        }



        void setOutputFilePath() {
//            if (FileName.is_relative()) {
//                FileDir = fs::current_path();
//            } else if (FileName.is_absolute()) {
//                FileDir = FileDir.stem();
//                FileName = FileName.filename();
//            }


            fs::path FileDirAbs;
            if (FileName.is_relative()) {
                FileDirAbs = fs::current_path() / FileName.parent_path();
            } else if (FileName.is_absolute()) {
                FileDirAbs = FileName.parent_path();
            }

            FileName = FileName.filename();
            FilePath = fs::system_complete(FileDirAbs / FileName);


            fs::path currentDir           = fs::current_path();
            h5pp::Logger::log->debug("currentDir   : {}", currentDir.string() );
            h5pp::Logger::log->debug("FileDirAbs   : {}", FileDirAbs.string() );
            h5pp::Logger::log->debug("FileName     : {}", FileName.string() );

            try{
                if(createDir){
                    if (fs::create_directories(FileDirAbs)){
                        FileDirAbs = fs::canonical(FileDirAbs);
                        h5pp::Logger::log->debug("Created directory: {}",FileDirAbs.string());
                    }else{
                        h5pp::Logger::log->debug("Directory already exists: {}",FileDirAbs.string());
                    }
                }else{
                    h5pp::Logger::log->critical("Target folder does not exist and creation of directory is disabled in settings");
                }

            }




            catch (std::exception &ex){
                throw std::runtime_error("Failed to create directory: " + FileDirAbs.string() + "\n" + ex.what());
            }
            switch (createMode){
                case CreateMode::OPEN: {
                    h5pp::Logger::log->debug("File mode OPEN: {}", FilePath.string());
                    try{
                        if(fileIsValid(FilePath)){
                            hid_t file = openFileHandle();
                            closeFileHandle(file);
                        }
                    }catch(std::exception &ex){
                        throw std::runtime_error("Failed to open hdf5 file :" + FilePath.string() );

                    }
                    break;
                }
                case CreateMode::TRUNCATE: {
                    h5pp::Logger::log->debug("File mode TRUNCATE: {}", FilePath.string());
                    try{
                        hid_t file = H5Fcreate(FilePath.c_str(), H5F_ACC_TRUNC,  H5P_DEFAULT, plist_facc);
                        H5Fclose(file);
                        file = openFileHandle();
                        closeFileHandle(file);
                    }catch(std::exception &ex){
                        throw std::runtime_error("Failed to create hdf5 file :" + FilePath.string() );
                    }
                    break;
                }
                case CreateMode::RENAME: {
                    try{
                        h5pp::Logger::log->debug("File mode RENAME: {}", FilePath.string());
                        if(fileIsValid(FilePath)) {
                            FilePath = getNewFileName(FilePath);
                            h5pp::Logger::log->info("Renamed output file: {} ---> {}", FileName.string(),FilePath.filename().string());
                            h5pp::Logger::log->info("File mode RENAME: {}", FilePath.string());
                            FileName = FilePath.filename();
                        }
                        hid_t file = H5Fcreate(FilePath.c_str(), H5F_ACC_TRUNC,  H5P_DEFAULT, plist_facc);
                        H5Fclose(file);
                        file = openFileHandle();
                        closeFileHandle(file);
                    }catch(std::exception &ex){
                        throw std::runtime_error("Failed to create renamed hdf5 file :" + FilePath.string() );
                    }
                    break;
                }
                default:{
                    h5pp::Logger::log->error("File Mode not set. Choose  CreateMode:: |OPEN|TRUNCATE|RENAME|");
                    throw std::runtime_error("File Mode not set. Choose  CreateMode::  |OPEN|TRUNCATE|RENAME|");
                }
            }

        }



    };
    
}



template <typename DataType>
void h5pp::File::writeDataset(const DataType &data, const DatasetProperties &props){
    hid_t file = openFileHandle();
    createDatasetLink(file, props);
    h5pp::Hdf5::setExtentDataset(file, props);
    hid_t dataset   = H5Dopen(file,props.dsetName.c_str(), H5P_DEFAULT);
    hid_t filespace = H5Dget_space(dataset);
    selectHyperslab(filespace, props.memSpace);
    if constexpr (tc::hasMember_c_str<DataType>::value){
//        retval = H5Dwrite(dataset, props.dataType, H5S_ALL, filespace,
//                       H5P_DEFAULT, data.c_str());
        retval = H5Dwrite(dataset, props.dataType, props.memSpace, filespace, H5P_DEFAULT, data.c_str());
        if(retval < 0) throw std::runtime_error("Failed to write text to file");
    }
    else if constexpr(tc::hasMember_data<DataType>::value){
        retval = H5Dwrite(dataset, props.dataType, props.memSpace, filespace, H5P_DEFAULT, data.data());
        if(retval < 0) throw std::runtime_error("Failed to write data to file");
    }
    else{
        retval = H5Dwrite(dataset, props.dataType, props.memSpace, filespace, H5P_DEFAULT, &data);
        if(retval < 0) throw std::runtime_error("Failed to write number to file");
    }
    H5Dclose(dataset);
    H5Fflush(file,H5F_SCOPE_GLOBAL);
    H5Fclose(file);
}

template <typename DataType>
void h5pp::File::writeDataset(const DataType &data, const std::string &datasetRelativeName){
    DatasetProperties props;
    props.dataType   = h5pp::Type::getDataType<DataType>();
    props.memSpace   = h5pp::Utils::getMemSpace(data);
    props.size       = h5pp::Utils::getSize<DataType>(data);
    props.ndims      = h5pp::Utils::getRank<DataType>();
    props.dims       = h5pp::Utils::getDimensions<DataType>(data);
    props.chunkSize = props.dims;
    props.dsetName  = datasetRelativeName;



    if constexpr(h5pp::Type::Check::hasStdComplex<DataType>() or h5pp::Type::Check::is_StdComplex<DataType>()) {
        if constexpr(tc::is_eigen_type<DataType>::value) {
            auto temp_rowm = Textra::to_RowMajor(data); //Convert to Row Major first;
            auto temp_cplx = h5pp::Utils::convertComplexDataToH5T(temp_rowm); // Convert to vector<H5T_COMPLEX_STRUCT<>>
            writeDataset(temp_cplx, props);
        } else {
            auto temp_cplx = h5pp::Utils::convertComplexDataToH5T(data);
            writeDataset(temp_cplx, props);
        }
    }

    else{
        if constexpr(tc::is_eigen_type<DataType>::value) {
            auto tempRowm = Textra::to_RowMajor(data); //Convert to Row Major first;
            writeDataset(tempRowm, props);
        } else {
            if(H5Tequal(props.dataType, H5T_C_S1)){
                // Read more about this step here
                //http://www.astro.sunysb.edu/mzingale/io_tutorial/HDF5_simple/hdf5_simple.c
                retval = H5Tset_size  (props.dataType, props.size);
                retval = H5Tset_strpad(props.dataType,H5T_STR_NULLPAD);
                writeDataset(data, props);
            }else{
                writeDataset(data, props);
            }
        }
    }

}


template <typename DataType>
void h5pp::File::readDataset(DataType &data, const std::string &datasetRelativeName){
    hid_t file = openFileHandle();
    if (h5pp::Hdf5::checkIfLinkExistsRecursively(file, datasetRelativeName)) {
        try{
            hid_t dataset   = H5Dopen(file, datasetRelativeName.c_str(), H5P_DEFAULT);
            hid_t memspace  = H5Dget_space(dataset);
            hid_t datatype  = H5Dget_type(dataset);
            int ndims       = H5Sget_simple_extent_ndims(memspace);
            std::vector<hsize_t> dims(ndims);
            H5Sget_simple_extent_dims(memspace, dims.data(), NULL);
            if constexpr(tc::is_eigen_core<DataType>::value) {
                Eigen::Matrix<typename DataType::Scalar, Eigen::Dynamic, Eigen::Dynamic,Eigen::RowMajor> matrixRowmajor;
                matrixRowmajor.resize(dims[0], dims[1]); // Data is transposed in HDF5!
                H5LTread_dataset(file, datasetRelativeName.c_str(), datatype, matrixRowmajor.data());
                data = matrixRowmajor;
            }
            else if constexpr(tc::is_eigen_tensor<DataType>()){
                Eigen::DSizes<long, DataType::NumDimensions> test;
                // Data is rowmajor in HDF5, so we need to convert back to ColMajor.
                Eigen::Tensor<typename DataType::Scalar,DataType::NumIndices, Eigen::RowMajor> tensorRowmajor;
                std::copy(dims.begin(),dims.end(),test.begin());
                tensorRowmajor.resize(test);
                H5LTread_dataset(file, datasetRelativeName.c_str(), datatype, tensorRowmajor.data());
                data = Textra::to_ColMajor(tensorRowmajor);
            }

            else if constexpr(tc::is_vector<DataType>::value) {
                assert(ndims == 1 and "Vector cannot take 2D datasets");
                data.resize(dims[0]);
                H5LTread_dataset(file, datasetRelativeName.c_str(), datatype, data.data());
            }
            else if constexpr(std::is_same<std::string,DataType>::value) {
                assert(ndims == 1 and "std string needs to have 1 dimension");
                hsize_t stringsize  = H5Dget_storage_size(dataset);
                data.resize(stringsize);
                H5LTread_dataset(file, datasetRelativeName.c_str(), datatype, data.data());
            }
            else if constexpr(std::is_arithmetic<DataType>::value){
                H5LTread_dataset(file, datasetRelativeName.c_str(), datatype, &data);
            }else{
                std::cerr << "Attempted to read dataset of unknown type: " << datasetRelativeName << "[" << typeid(data).name() << "]" << std::endl;
                exit(1);
            }
            H5Dclose(dataset);
            H5Sclose(memspace);
            H5Tclose(datatype);
        }catch(std::exception &ex){
            throw std::runtime_error("Failed to read dataset [ " + datasetRelativeName + " ] :" + std::string(ex.what()) );
        }
    }
    else{
        std::cerr << "Attempted to read dataset that doesn't exist: " << datasetRelativeName << std::endl;
    }
    H5Fclose(file);
}





template <typename AttrType>
void h5pp::File::writeAttributeToFile(const AttrType &attribute, const std::string attributeName){
    hid_t file = openFileHandle();
    hid_t datatype          = h5pp::Type::getDataType<AttrType>();
    hid_t memspace          = h5pp::Utils::getMemSpace(attribute);
    auto size               = h5pp::Utils::getSize(attribute);
    if constexpr (tc::hasMember_c_str<AttrType>::value){
        retval                  = H5Tset_size(datatype, size);
        retval                  = H5Tset_strpad(datatype,H5T_STR_NULLTERM);
    }

    hid_t attributeId      = H5Acreate(file, attributeName.c_str(), datatype, memspace, H5P_DEFAULT, H5P_DEFAULT );
    if constexpr (tc::hasMember_c_str<AttrType>::value){
        retval                  = H5Awrite(attributeId, datatype, attribute.c_str());
    }
    else if constexpr (tc::hasMember_data<AttrType>::value){
        retval                  = H5Awrite(attributeId, datatype, attribute.data());
    }
    else{
        retval                  = H5Awrite(attributeId, datatype, &attribute);
    }

    H5Sclose(memspace);
    H5Tclose(datatype);
    H5Aclose(attributeId);
    H5Fclose(file);
}



template <typename AttrType>
void h5pp::File::writeAttributeToLink(const AttrType &attribute, const AttributeProperties &aprops){
    hid_t file = openFileHandle();
    if (h5pp::Hdf5::checkIfLinkExistsRecursively(file, aprops.linkName) ) {
        if (not h5pp::Hdf5::checkIfAttributeExists(file, aprops.linkName, aprops.attrName)) {
//            hid_t linkObject = H5Dopen(file, aprops.linkName.c_str(), H5P_DEFAULT);
            hid_t linkObject = H5Oopen(file, aprops.linkName.c_str(), H5P_DEFAULT);
            hid_t attributeId = H5Acreate(linkObject, aprops.attrName.c_str(), aprops.dataType, aprops.memSpace,
                                           H5P_DEFAULT, H5P_DEFAULT);

            if constexpr (tc::hasMember_c_str<AttrType>::value) {
                retval = H5Awrite(attributeId, aprops.dataType, attribute.c_str());
            } else if constexpr (tc::hasMember_data<AttrType>::value) {
                retval = H5Awrite(attributeId, aprops.dataType, attribute.data());
            } else {
                retval = H5Awrite(attributeId, aprops.dataType, &attribute);
            }

            H5Dclose(linkObject);
            H5Aclose(attributeId);
            H5Fflush(file, H5F_SCOPE_GLOBAL);
            H5Fclose(file);
        }
    }
    else{
        H5Fclose(file);
        std::string error = "Link " + aprops.linkName + " does not exist, yet attribute is being written.";
        h5pp::Logger::log->critical(error);
        throw(std::logic_error(error));
    }
}


template <typename AttrType>
void h5pp::File::writeAttributeToLink(const AttrType &attribute, const std::string &attributeName,
                                      const std::string &linkName){
    AttributeProperties aprops;
    aprops.dataType  = h5pp::Type::getDataType<AttrType>();
    aprops.memSpace  = h5pp::Utils::getMemSpace(attribute);
    aprops.size      = h5pp::Utils::getSize(attribute);
    aprops.ndims     = h5pp::Utils::getRank<AttrType>();
    aprops.dims      = h5pp::Utils::getDimensions(attribute);
    aprops.attrName = attributeName;
    aprops.linkName = linkName;
    if constexpr (tc::hasMember_c_str<AttrType>::value
                  or std::is_same<char * , typename std::decay<AttrType>::type>::value
    ){
        retval                  = H5Tset_size(aprops.dataType, aprops.size);
        retval                  = H5Tset_strpad(aprops.dataType,H5T_STR_NULLTERM);
    }
    writeAttributeToLink(attribute, aprops);
}











#endif //H5PP_FILE_H
