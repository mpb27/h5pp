
#include <iostream>
#include <complex>
#include <h5pp/h5pp.h>

/*! \brief Prints the content of a vector nicely */
template<typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v) {
    if (!v.empty()) {
        out << "[ ";
        std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, " "));
        out << "]";
    }
    return out;
}

using namespace std::complex_literals;



// Store some dummy data to an hdf5 file



int main()
{
    using cplx = std::complex<double>;

    static_assert(h5pp::Type::Check::hasMember_data<std::vector<double>>() and "Compile time type-checker failed. Could not properly detect class member data. Check that you are using a supported compiler!");

    std::string outputFilenameA      = "outputA/copySwapA.h5";
    std::string outputFilenameB      = "outputB/copySwapB.h5";

    size_t      logLevel  = 0;
    h5pp::File fileA(outputFilenameA,h5pp::AccessMode::READWRITE,h5pp::CreateMode::TRUNCATE,logLevel);
    h5pp::File fileB(outputFilenameB,h5pp::AccessMode::READWRITE,h5pp::CreateMode::TRUNCATE,logLevel);

    fileA.writeDataset("A", "groupA/A");
    fileB.writeDataset("B", "groupB/B");

    h5pp::File fileC;
    fileC = fileB;
    fileC.writeDataset("C", "groupC/C");
    std::cerr << "Now begins the copy constructor " << std::endl << std::flush;
    h5pp::File fileD(fileC);
    fileD.writeDataset("D", "groupD/D");

    std::cerr << "Now begins the weird part " << std::endl << std::flush;
    h5pp::File fileE(h5pp::File("copySwapE.h5"));
    fileE.writeDataset("E", "groupE/E");
    std::cerr << "Weird part over" << std::endl << std::flush;

    fileD = fileB;


    return 0;
}