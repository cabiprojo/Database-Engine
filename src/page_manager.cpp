#include "page_manager.h"

using namespace std;

PageManager::PageManager(const string& filename) : file_manager_(filename) {
    uint64_t file_size = file_manager_.size();
    next_page_id_ = file_size / PAGE_SIZE;
}

PageManager::~PageManager() {}

uint32_t PageManager::allocatePage() {
    uint32_t new_page_id = next_page_id_;
    next_page_id_++;
    return new_page_id;
}

void PageManager::readPage(uint32_t page_id, Page& page) {
    file_manager_.readPage(page_id, page);
}

void PageManager::writePage(uint32_t page_id, const Page& page) {
    file_manager_.writePage(page_id, page);
}

void PageManager::freePage(uint32_t page_id) {

}