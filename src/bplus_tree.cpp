#include "bplus_tree.h"

// constructor, create empty tree
BPlusTree::BPlusTree(int order) : order_(order), root_(nullptr){

}

// destructor, delete all nodes and set root to nullptr
BPlusTree::~BPlusTree() {
    deleteTree(root_);
    root_ = nullptr;
}

bool BPlusTree::insert(const string& key, uint64_t value) {
    // handle empty tree case, create new root leaf node
    if (root_ == nullptr) {
        root_ = new Node(true);
        root_->keys.push_back(key);
        root_->values.push_back(value);
        return true;
    }

    // call recursive insert helper function to do actual insertion
    string promotedKey;
    Node* newRightChild;
    Node* result = insertHelper(root_, key, value, promotedKey, newRightChild);

    
    // if result from insertHelper is not nullptr, split root and create new root
    if (result != nullptr) {
        Node* oldRoot = root_;
        root_ = new Node(false);
        root_->keys.push_back(promotedKey);
        root_->children.push_back(oldRoot);
        root_->children.push_back(newRightChild);
    }

    return true;
}

// returns nullptr if no split, returns new right child if split
Node *BPlusTree::insertHelper(Node* node, const string& key, uint64_t value, string& promotedKey, Node*& newRightChild) {
    // base case: leaf node
    if (node->is_leaf) {
        // insert key-value into leaf
        int insertPos = node->keys.size();
        for (int i = 0; i < node->keys.size(); i++) {
            if (node->keys[i] > key) {
                insertPos = i;
                break;
            }
        }
        node->keys.insert(node->keys.begin() + insertPos, key);
        node->values.insert(node->values.begin() + insertPos, value);

        // if leaf not full, return null
        if (node->keys.size() < order_) {
            return nullptr;
        }

        // if leaf is full:
        // -split leaf into two
        int split_index = order_ / 2;

        // -set promotedKey (middle key)
        promotedKey = node->keys[split_index];

        // -set new right child
        newRightChild = new Node(true);
        newRightChild->keys.assign(node->keys.begin() + split_index, node->keys.end()); // .assign() expects iterators to start and end
        newRightChild->values.assign(node->values.begin() + split_index, node->values.end());

        // -remove right half from node
        node->keys.erase(node->keys.begin() + split_index, node->keys.end());
        node->values.erase(node->values.begin() + split_index, node->values.end());

        // -link leaves
        Node* oldNext = node->next;
        node->next = newRightChild;
        newRightChild->next = oldNext;

        // -return new right child
        return newRightChild;

    }

    // recursive case: internal node
    else {
        // find which child to follow (compare key)
        // call insertHelper recursively on the child node

        // if key is greater than all keys in current node, follow last child node
        Node* result = nullptr;
        if (key > node->keys.back()) {
            result = insertHelper(node->children.back(), key, value, promotedKey, newRightChild);
        }
        for (int i = 0; i < node->keys.size(); i++) {
            if (key < node->keys[i]) {
                result = insertHelper(node->children[i], key, value, promotedKey, newRightChild);
                break;
            }
        }

        
        // if child didn't split (returned nullptr) return nullptr
        if (result == nullptr) {
            return nullptr;
        }
         
        // if child split (returned new node)
        // -insert promoted key at correct position
        int insertPos = node->keys.size();
        for (int i = 0; i < node->keys.size(); i++) {
            if (node->keys[i] > promotedKey) {
                insertPos = i;
                break;
            }
        }
        node->keys.insert(node->keys.begin() + insertPos, promotedKey);
        node->children.insert(node->children.begin() + insertPos + 1, newRightChild);

        // if not full, return nullptr
        if (node->keys.size() < order_) {
            return nullptr;
        }
        // else it is full,split internal node
        int split_index = order_ / 2;
        promotedKey = node->keys[split_index];
        newRightChild = new Node(false);
        newRightChild->keys.assign(node->keys.begin() + split_index + 1, node->keys.end());
        newRightChild->children.assign(node->children.begin() + split_index + 1, node->children.end());
        node->keys.erase(node->keys.begin() + split_index, node->keys.end());
        node->children.erase(node->children.begin() + split_index + 1, node->children.end());

        return newRightChild;
    }
}


// search for key in tree. if found, return value, else return 0
uint64_t BPlusTree::search(const string& key) {
    if (root_ == nullptr) {
        return 0;
    }

    // find the leaf node that contains the key
    Node* leaf = findLeaf(key);
    if (leaf == nullptr) {
        return 0;
    }

    // search through leaf's keys for a match
    // if found, return corresponding value (file position), else 0
    for (int i = 0; i < leaf->keys.size(); i++) {
        if (leaf->keys[i] == key) {
            return leaf->values[i];
        }
    }

    return 0;
}

// find the leaf node that contains the key
Node* BPlusTree::findLeaf(const string& key) {
    if (root_ == nullptr) {
        return nullptr;
    }

    // start at root
    Node* current = root_;

    while (!current->is_leaf) {
        bool found = false;
        // go through keys in current node to find child node to follow next
        // if key is less than current key, follow child node at current index
        for (int i = 0; i < current->keys.size(); i++) {
            if (key < current->keys[i]) {
                current = current->children[i];
                found = true;
                break;
            }
        }
        // if key is greater than all keys in current node, follow last child node (last index in children vector)
        if (!found) {
            current = current->children.back();
        }
    }
    return current;
}

void BPlusTree::deleteTree(Node* node) {
    if (node) {
        // just delete the node. the Node destructor will handle
        // recursively deleting children for internal nodes
        delete node;
    }
}
