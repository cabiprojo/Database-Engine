#include "file_manager.h" 

// constructor, :: prefix means it's member of the FileManager class
FileManager::FileManager(const string& filename) : filename_(filename) {

    // open file for reading and writing at specific offsets
    // ios::binary = binary mode
    // ios::in = input (read)
    // ios::out = output (write)
    // first try to open existing file, if fails create new one
    file_.open(filename_, ios::binary | ios::in | ios::out);
    if (!file_.is_open()) {
        // file doesn't exist, create it
        file_.open(filename_, ios::binary | ios::out);
        file_.close();
        file_.open(filename_, ios::binary | ios::in | ios::out);
    }

}

// destructor
FileManager::~FileManager() {
    if (file_.is_open()) {
        file_.close();
    }
}

string FileManager::read(size_t position, size_t size) {
    // move to the specified position
    file_.seekg(position, ios::beg); // ios::beg means beginning of the file
    
    // check if seek was successful
    if (!file_.good()) {
        return string(size, '\0'); // return empty string if file operation failed
    }

    // create buffer to hold data
    string result(size, '\0'); // create string with size and fill with null characters

    // read data into the buffer
    file_.read(&result[0], size); // read 'size' bytes into the string
    
    // if read failed (e.g., end of file), return what we got (will be zeros)
    // gcount() returns number of characters read
    streamsize bytes_read = file_.gcount();
    if (bytes_read < static_cast<streamsize>(size)) {
        // partial read - fill rest with zeros (already done by string constructor)
        // the string was initialized with zeros, so we're good
    }

    return result;
}


uint64_t FileManager::write(const string& data) {
    // move to the end of the file (to append)
    file_.seekp(0, ios::end); // seekp is seek put pointer, move the put pointer where next write will happen

    // get current position (wheere we're about to write)
    uint64_t position = file_.tellp();

    // write the data to the file
    file_.write(data.c_str(), data.size()); // c_str gives pointer to char array, data.size() ensures write entire string

    // flush to ensure it's written to disk
    file_.flush(); 

    // return the position where data was written
    return position;
}

uint64_t FileManager::size() {
    // check if file is open
    if (!file_.is_open()) {
        return 0;  // file not open, return 0 size
    }
    
    file_.seekg(0, ios::end);
    if (!file_.good()) {
        return 0;  // seek failed, return 0 size
    }
    
    uint64_t size = file_.tellg();
    if (size == static_cast<uint64_t>(-1)) {  // tellg returns -1 on error
        return 0;
    }
    return size;
}

void FileManager::readPage(uint32_t page_id, Page& page) {
    // check if file is open
    if (!file_.is_open()) {
        page.clear();
        return;
    }
    
    uint64_t page_offset = page_id * PAGE_SIZE;
    uint64_t file_size = size();
    // if offset beyond file size, page is empty, clear and return
    if (page_offset >= file_size) {
        page.clear();
        return;
    }

    uint64_t bytes_to_read;
    if (page_offset + PAGE_SIZE > file_size) {
        bytes_to_read = file_size - page_offset;
    } else {
        bytes_to_read = PAGE_SIZE;
    }

    string page_data = read(page_offset, bytes_to_read);
    page.clear();
    page.writeBytes(0, page_data.c_str(), bytes_to_read);

}

void FileManager::writePage(uint32_t page_id, const Page& page) {
    // check if file is open
    if (!file_.is_open()) {
        return;  // can't write if file not open
    }
    
    uint64_t page_offset = page_id * PAGE_SIZE;
    uint64_t required_size = page_offset + PAGE_SIZE;
    uint64_t current_size = size();
    
    // extend file if necessary (fill with zeros)
    if (required_size > current_size) {
        file_.seekp(0, ios::end);
        if (!file_.good()) {
            return;  // seek failed
        }
        uint64_t bytes_to_add = required_size - current_size;
        string zeros(bytes_to_add, '\0');
        file_.write(zeros.c_str(), bytes_to_add);
        if (!file_.good()) {
            return;  // write failed
        }
        file_.flush();
    }
    
    // now write the page at the correct offset
    file_.seekp(page_offset, ios::beg);
    if (!file_.good()) {
        return;  // seek failed
    }
    file_.write(page.data, PAGE_SIZE);
    if (!file_.good()) {
        return;  // write failed
    }
    file_.flush();
}