#pragma once // header guard to prevent multiple includes in compilation

#include <string>
#include <fstream>
#include <cstdint>
using namespace std;

class FileManager {
public:
    // constructor, opens the file
    FileManager(const string& filename);

    // destructor, closes the file
    ~FileManager();

    // read data from specific position in the file
    // position: where to start reading (byte offset)
    // size: how many bytes to read
    // returns: the data read as a string
    string read(size_t position, size_t size);

    // write data to end of the file
    // data: the data to write
    // returns: the position where data was written
    uint64_t write(const string& data);

    // get file size
    // returns: size of file in bytes
    uint64_t size();

private:
    string filename_; // store the filename
    fstream file_; // the file stream object
};

