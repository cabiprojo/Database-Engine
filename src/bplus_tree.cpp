#include "bplus_tree.h"
#include "buffer_pool.h"
#include "page_manager.h"
#include <iostream>

// create empty tree
BPlusTree::BPlusTree(BufferPool* buffer_pool, PageManager* page_manager, 
    int order) : buffer_pool_(buffer_pool), page_manager_(page_manager), order_(order), root_page_id_(0) {
}

bool BPlusTree::insert(const string& key, uint64_t value) {
    // handle empty tree case, create new root leaf node
    if (root_page_id_ == 0) {
        // allocate new page for root (allocateNode will skip page 0 for metadata)
        root_page_id_ = allocateNode(true);
        
        // create the node directly (don't load - we just created an empty node)
        Node node(true);  // create leaf node
        node.page_id = root_page_id_;
        
        // add the key-value pair
        node.keys.push_back(key);
        node.values.push_back(value);

        // save to page
        saveNode(root_page_id_, node);

        return true;
    }

    // load root node
    Node root;
    loadNode(root_page_id_, root);

    // call recursive insert helper function to do actual insertion
    string promotedKey;
    uint32_t newRightChildPageId;
    uint32_t resultPageId = insertHelper(root_page_id_, key, value, promotedKey, newRightChildPageId);

    // if result from insertHelper is not nullptr, split root and create new root
    if (resultPageId != 0) {
        uint32_t oldRootPageId = root_page_id_;

        root_page_id_ = allocateNode(false); 
        Node newRoot(false);  // create internal node directly, don't load
        newRoot.page_id = root_page_id_;
        newRoot.keys.push_back(promotedKey);
        newRoot.children_page_ids.push_back(oldRootPageId);
        newRoot.children_page_ids.push_back(newRightChildPageId);
        saveNode(root_page_id_, newRoot);
    }

    return true;
}

// returns nullptr if no split, returns new right child if split
uint32_t BPlusTree::insertHelper(uint32_t page_id, const string& key, uint64_t value, string& promotedKey, uint32_t& newRightChildPage) {
    // load node from page
    Node node;
    loadNode(page_id, node);
    
    // base case: leaf node
    if (node.is_leaf) {
        // insert key-value into leaf
        int insertPos = node.keys.size();
        for (int i = 0; i < node.keys.size(); i++) {
            if (node.keys[i] > key) {
                insertPos = i;
                break;
            }
        }
        node.keys.insert(node.keys.begin() + insertPos, key);
        node.values.insert(node.values.begin() + insertPos, value);

        // if leaf not full, return 0
        if (node.keys.size() < order_) {
            saveNode(page_id, node);
            return 0;
        }

        // if leaf is full:
        // -split leaf into two
        int split_index = order_ / 2;

        // -set promotedKey (middle key)
        promotedKey = node.keys[split_index];

        // -set new right child
        newRightChildPage = allocateNode(true);
        Node newRightChild;
        loadNode(newRightChildPage, newRightChild);
        newRightChild.keys.assign(node.keys.begin() + split_index, node.keys.end()); // .assign() expects iterators to start and end
        newRightChild.values.assign(node.values.begin() + split_index, node.values.end());
        saveNode(newRightChildPage, newRightChild);

        // -remove right half from node
        node.keys.erase(node.keys.begin() + split_index, node.keys.end());
        node.values.erase(node.values.begin() + split_index, node.values.end());

        // -link leaves
        uint32_t oldNext = node.next_page_id;
        node.next_page_id = newRightChildPage;
        newRightChild.next_page_id = oldNext;

        saveNode(page_id, node);

        // -return new right child
        return newRightChildPage;

    }

    // recursive case: internal node
    else {
        // find which child to follow (compare key)
        // call insertHelper recursively on the child node

        // find the correct child to follow
        uint32_t resultPageId = 0;
        int childIndex = node.keys.size(); // default to last child
        
        for (int i = 0; i < node.keys.size(); i++) {
            if (key < node.keys[i]) {
                childIndex = i;
                break;
            }
        }
        // if key >= all keys, childIndex stays as last child (keys.size())
        
        resultPageId = insertHelper(node.children_page_ids[childIndex], key, value, promotedKey, newRightChildPage);
        
        // if child didn't split (returned nullptr) return 0
        if (resultPageId == 0) {
            saveNode(page_id, node);
            return 0;
        }
         
        // if child split (returned new node)
        // -insert promoted key at correct position
        int insertPos = node.keys.size();
        for (int i = 0; i < node.keys.size(); i++) {
            if (node.keys[i] > promotedKey) {
                insertPos = i;
                break;
            }
        }
        node.keys.insert(node.keys.begin() + insertPos, promotedKey);
        node.children_page_ids.insert(node.children_page_ids.begin() + insertPos + 1, newRightChildPage);

        // if not full, return nullptr
        if (node.keys.size() < order_) {
            saveNode(page_id, node);
            return 0;
        }
        // else it is full,split internal node
        int split_index = order_ / 2;
        promotedKey = node.keys[split_index];
        newRightChildPage = allocateNode(false);
        Node newRightChild;
        loadNode(newRightChildPage, newRightChild);
        newRightChild.keys.assign(node.keys.begin() + split_index + 1, node.keys.end());
        newRightChild.children_page_ids.assign(node.children_page_ids.begin() + split_index + 1, node.children_page_ids.end());
        node.keys.erase(node.keys.begin() + split_index, node.keys.end());
        node.children_page_ids.erase(node.children_page_ids.begin() + split_index + 1, node.children_page_ids.end());

        saveNode(page_id, node);

        return newRightChildPage;
    }
}


// search for key in tree. if found, return value, else return 0
uint64_t BPlusTree::search(const string& key) {
    if (root_page_id_ == 0) {
        return 0;
    }

    // find the leaf node that contains the key
    uint32_t leaf_page_id = findLeaf(key);
    if (leaf_page_id == 0) {
        return 0;  // not found
    }
    Node leaf;
    loadNode(leaf_page_id, leaf);

    // search through leaf's keys for a match
    // if found, return corresponding value (file position), else 0
    for (int i = 0; i < leaf.keys.size(); i++) {
        if (leaf.keys[i] == key) {
            return leaf.values[i];
        }
    }

    return 0;
}

// find the leaf node that contains the key
uint32_t BPlusTree::findLeaf(const string& key) {
    if (root_page_id_ == 0) {
        return 0;
    }

    // start at root
    Node current;
    loadNode(root_page_id_, current);

    while (!current.is_leaf) {
        bool found = false;
        // go through keys in current node to find child node to follow next
        // if key is less than current key, follow child node at current index
        for (int i = 0; i < current.keys.size(); i++) {
            if (key < current.keys[i]) {
                loadNode(current.children_page_ids[i], current);
                found = true;
                break;
            }
        }
        // if key is greater than all keys in current node, follow last child node (last index in children vector)
        if (!found) {
            loadNode(current.children_page_ids.back(), current);
        }
    }
    return current.page_id;
}


void Node::serializeToPage(Page& page) const { // write node data to page
    uint32_t offset = 0;

    // write is_leaf (1 byte)
    page.writeBytes(offset, (char*)&is_leaf, 1);
    offset += 4;

    // write num_keys (4 bytes)
    uint32_t num_keys = keys.size();
    page.writeUint32(offset, num_keys);
    offset += 4;

    // write next_page_id (4 bytes)
    page.writeUint32(offset, next_page_id);
    offset += 4;

    const uint32_t key_size = 64;
    const uint32_t entry_size = key_size + 8;

    // write keys
    for (uint32_t i = 0; i < keys.size(); i++) {
        uint32_t entry_offset = 12 + (i * entry_size);
        uint32_t value_offset = entry_offset + key_size;

        // write key
        char key_buffer[key_size] = {0}; // create a 64 byte buffer
        uint32_t key_len = keys[i].size();
        if (key_len > 0 && key_len <= key_size) { // if key is not too long, copy it to the buffer
            memcpy(key_buffer, keys[i].c_str(), key_len);
        }
        page.writeBytes(entry_offset, key_buffer, key_size); // write the buffer to the page

        // write value
        if (is_leaf) {
            page.writeUint64(value_offset, values[i]);
        } else {
        // for internal nodes, write the left child for this key
        // (children_page_ids[i] is the left child of key[i])
            page.writeUint64(value_offset, children_page_ids[i]);
        }
    }
    
    // for internal nodes, we need to write the last (rightmost) child
    // internal nodes have num_keys + 1 children
    if (!is_leaf && keys.size() > 0) {
        uint32_t last_entry_offset = 12 + (keys.size() * entry_size);
        uint32_t last_value_offset = last_entry_offset + key_size;
        page.writeUint64(last_value_offset, children_page_ids.back());
    }
}

void Node::deserializeFromPage(const Page& page) {
    uint32_t offset = 0;

    // read is_leaf (1 byte)
    page.readBytes(offset, (char*)&is_leaf, 1);
    offset += 4;

    // read num_keys (4 bytes)
    uint32_t num_keys = page.readUint32(offset);
    offset += 4;

    // read next_page_id
    next_page_id = page.readUint32(offset);
    offset += 4;

    // clear vectors
    keys.clear();
    values.clear();
    children_page_ids.clear();

    const uint32_t key_size = 64;
    const uint32_t entry_size = key_size + 8; // 72 bytes per entry

    // loop through entries and read the key and value
    for (uint32_t i = 0; i < num_keys; i++) {
        uint32_t entry_offset = 12 + (i * entry_size);
        uint32_t value_offset = entry_offset + key_size;

        // read key
        char key_buffer[key_size] = {0};
        page.readBytes(entry_offset, key_buffer, key_size);
        string key_str(key_buffer);

        // remove any null characters from the key
        size_t null_pos = key_str.find('\0');
        if (null_pos != string::npos) {
            key_str = key_str.substr(0, null_pos);
        }
        keys.push_back(key_str);

        // read value
        if (is_leaf) {
            values.push_back(page.readUint64(value_offset));
        } else {
            // for internal nodes, read the left child for this key
            children_page_ids.push_back(page.readUint64(value_offset));
        }
    }
    
    // for internal nodes, read the last (rightmost) child
    if (!is_leaf && num_keys > 0) {
        uint32_t last_entry_offset = 12 + (num_keys * entry_size);
        uint32_t last_value_offset = last_entry_offset + key_size;
        children_page_ids.push_back(page.readUint64(last_value_offset));
    }
}

void BPlusTree::loadNode(uint32_t page_id, Node& node) {
    // load page from BufferPool
    Page page;
    buffer_pool_->getPage(page_id, page);

    // deserialize node from page
    node.deserializeFromPage(page);

    // set node.page_id
    node.page_id = page_id;

}

void BPlusTree::saveNode(uint32_t page_id, const Node& node) {
    // serialize node to page
    Page page;
    node.serializeToPage(page);

    // save page to BufferPool
    buffer_pool_->savePage(page_id, page);

}

uint32_t BPlusTree::allocateNode(bool is_leaf) {
    // allocate new page from PageManager
    uint32_t new_page_id = page_manager_->allocatePage();
    
    // reserve page 0 for metadata - if we got page 0, allocate again to get page 1
    if (new_page_id == 0) {
        new_page_id = page_manager_->allocatePage();
    }

    // create an empty node
    Node node(is_leaf);
    
    // set node page_id
    node.page_id = new_page_id;

    // save node to the page
    saveNode(new_page_id, node);

    // return the page_id
    return new_page_id;
}

// load root_page_id_ from metadata page (page 0)
void BPlusTree::loadRootPageId() {
    // initialize to 0 (empty tree) - default for new files
    root_page_id_ = 0;
    
    // try to load root_page_id_ from metadata page (page 0)
    // if file is new/empty, getPage will return an empty page, and readUint32(0) will return 0
    Page metadata_page(true);  // initialize with cleared page (all zeros)
    buffer_pool_->getPage(0, metadata_page);
    
    // read root_page_id_ from offset 0 (first 4 bytes of page 0)
    // if page is empty (new file), this will be 0, which is correct
    root_page_id_ = metadata_page.readUint32(0);
}

// save root_page_id_ to metadata page (page 0)
void BPlusTree::saveRootPageId() {
    // ensure page 0 exists (getPage will create it if needed)
    Page metadata_page(true);  // initialize with cleared page
    buffer_pool_->getPage(0, metadata_page);
    
    // write root_page_id_ to offset 0 (first 4 bytes of page 0)
    metadata_page.writeUint32(0, root_page_id_);
    
    // mark as dirty and save
    buffer_pool_->savePage(0, metadata_page);
}
