#include "kv_store.h"
#include "page_manager.h"
#include <iostream>

KVStore::KVStore(const string& filename) : page_manager_(filename), current_page_id_(0), current_offset_(0) {
    // create BufferPool with PageManager
    buffer_pool_ = new BufferPool(&page_manager_, 100);
    // rebuild the index from the file
    rebuildIndex();
}

KVStore::~KVStore() {
    // file manager destructor will automatically close the file
    // index will be automatically destroyed
    delete buffer_pool_;
}

bool KVStore::put(const string& key, const string& value) {
    // calculate record size
    uint32_t key_len = key.size();
    uint32_t value_len = value.size();
    uint32_t record_size = 4 + key_len + 4 + value_len;
 
    // check if record fits in current page
    if (current_offset_ + record_size > PAGE_SIZE) {
        // allocate new page
        current_page_id_ = page_manager_.allocatePage();
        current_offset_ = 0;
    }

    // load current page (or create empty if none) using BufferPool cache
    Page page;
    buffer_pool_->getPage(current_page_id_, page);

    // write record to page at current offset
    uint32_t offset = current_offset_;
    page.writeUint32(offset, key_len);
    offset += 4;
    page.writeString(offset, key, key_len);
    offset += key_len;
    page.writeUint32(offset, value_len);
    offset += 4;
    page.writeString(offset, value, value_len);

    // save page back to cache (will write to disk when flushed/evicted)
    buffer_pool_->savePage(current_page_id_, page);

    // update index with (page_id, offset)
    index_[key] = {current_page_id_, current_offset_};

    // update current offset
    current_offset_ += record_size;
 
    return true;
}

string KVStore::get(const string& key) {
    // check if key exists in index, if not, return empty string
    if (index_.find(key) == index_.end()) { 
        return "";
    }

    // look up key in index (page_id, offset)
    uint32_t page_id = index_[key].first;
    uint32_t offset = index_[key].second;

    // load that page from cache or disk using BufferPool cache
    Page page;
    buffer_pool_->getPage(page_id, page);

    // read record from page at that offset
    uint32_t key_len = page.readUint32(offset);
    offset += 4;

    // skip key
    offset += key_len;

    // read value length (4 bytes)
    uint32_t value_len = page.readUint32(offset);
    offset += 4;

    // read value
    string value = page.readString(offset, value_len);

    // return the value
    return value;
}

bool KVStore::remove(const string& key) {
    // check if key exists in index
    if (index_.find(key) == index_.end()) return false;
    
    // remove key from index
    index_.erase(key);

    // return true
    return true;
}


void KVStore::rebuildIndex() {
    // start at beginning of file
    uint32_t page_id = 0;
    current_page_id_ = 0;
    current_offset_ = 0;

    while (true) {
        // load page from cache or disk using BufferPool cache
        Page page;
        buffer_pool_->getPage(page_id, page);

        // check if page is empty (all zeros or past end of file)
        uint32_t offset = 0;
        bool found_any = false; // flag to check if any records were found on this page

        // scan page for records
        while (offset + 4 <= PAGE_SIZE) {
            // save where this record starts
            uint32_t record_start = offset;
            
            uint32_t key_len = page.readUint32(offset);

            if (key_len == 0 || key_len > PAGE_SIZE) break;

            offset += 4;

            if (offset + key_len > PAGE_SIZE) break;

            // read key
            string key = page.readString(offset, key_len);
            offset += key_len;

            if (offset + 4 > PAGE_SIZE) break;

            uint32_t value_len = page.readUint32(offset);
            offset += 4;

            if (offset + value_len > PAGE_SIZE) break;

            // store in index using the saved record_start
            index_[key] = {page_id, record_start};
            found_any = true;

            offset += value_len;

        }

        // update current position
        if (found_any) {
            current_page_id_ = page_id;
            current_offset_ = offset;
        }

        // if page was empty, break
        if (!found_any && page_id > 0) {
            break;
        }

        page_id++;
    }
}
