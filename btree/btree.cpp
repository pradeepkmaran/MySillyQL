#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdexcept>
#include <Windows.h>

using namespace std;

// write_node
// 1. Seek: Move the file cursor to block 0 (self.seek(0)).
// 2. Prepare Buffer: Allocate 4096 bytes (block size), initialize with zeros.
// 3. Serialize Metadata:
//    - Number of keys (KL = 3): First 2 bytes `03 00`.
//    - Number of children (CL = 2): Next 2 bytes `02 00`.
// 4. Serialize Keys:
//    - Key 1: `00 00 00 01` (4 bytes).
//    - Key 2: `00 00 00 02` (4 bytes).
//    - Key 3: `00 00 00 03` (4 bytes).
//    - Place keys starting at byte 4.  
// 5. Serialize Children:
//    - Child 4: `00 00 00 04` (4 bytes).
//    - Child 5: `00 00 00 05` (4 bytes).
//    - Place children after keys in the buffer.
// 6. Write Data: Write buffer to file at block 0.

// read_node
// 1. Seek: Move the file cursor to block 0 (self.seek(0)).
// 2. Read Data: Read 4096 bytes into the buffer.
// 3. Deserialize Metadata:
//    - First 2 bytes `03 00` → KL = 3 keys.
//    - Next 2 bytes `02 00` → CL = 2 children.
// 4. Deserialize Keys:
//    - `00 00 00 01` → Key 1.
//    - `00 00 00 02` → Key 2.
//    - `00 00 00 03` → Key 3.
// 5. Deserialize Children:
//    - `00 00 00 04` → Child 4.
//    - `00 00 00 05` → Child 5.
// 6. Return Node: Return keys [1, 2, 3] and children [4, 5] as Node object.

class Node { // Actual node of the BTree
public:
    uint32_t block;
    vector<uint32_t> keys;
    vector<uint32_t> children;
    Node() :block(0), keys(), children() {}
    Node(vector<uint32_t> keys) :block(0), keys(keys), children() {}
};

size_t get_block_size(const string& file_name) { // set of system commands to get the block size suitable for the particular OS
    #if defined(_WIN32) || defined(_WIN64) // for windows OS
        DWORD sectors_per_cluster, bytes_per_sector, number_of_free_clusters, total_number_of_clusters;
        char fullPath[MAX_PATH];
        GetFullPathName(file_name.c_str(), MAX_PATH, fullPath, NULL);
        
        char rootPath[4] = {fullPath[0], ':', '\\', '\0'};
        
        if (!GetDiskFreeSpace(rootPath, &sectors_per_cluster, &bytes_per_sector, 
                              &number_of_free_clusters, &total_number_of_clusters)) {
            return 4096; 
        }

        return static_cast<size_t>(bytes_per_sector);
    #else // for POSIX based OS
        struct stat file_info;
        if (stat(file_name.c_str(), &file_info) != 0) {
            throw runtime_error("Failed to get file metadata");
        }
        return static_cast<size_t>(file_info.st_blksize); 
    #endif
}

class BTree {
public:
    unique_ptr<Node> root;
    fstream file;
    size_t block_size;
    size_t num_of_blocks;
    size_t capacity;
    size_t max_num_of_keys;
    size_t degree;

    BTree(string file_name = "default_file") { // setting up the B-Tree for writing it in the file
        file.open(file_name, ios::in | ios::out | ios::binary); // opening the file and setting file operations
        if (!file.is_open()) {
            throw runtime_error("Failed to open the file: " + file_name);
        }

        file.write("\0\0\0\0", 4);
        
        file.seekg(0, ios::end);
        streampos fileSize = file.tellg();
        file.seekg(0, ios::beg);
        
        block_size = get_block_size(file_name);
        capacity = block_size / 4;

        max_num_of_keys = capacity / 2 - 1;
        degree = (max_num_of_keys + 1) / 2;
        num_of_blocks = static_cast<size_t>(fileSize) / block_size;
    }
    
    bool split_child(Node* parent, size_t index) {
        Node target_node;
        if (!read_node(parent->children[index], &target_node)) {
            return false;
        }

        Node new_node;
        new_node.block = free_block();

        for (size_t i = degree; i < (2 * degree - 1); i++) {
            new_node.keys.push_back(target_node.keys[i]);
        }
        target_node.keys.resize(degree - 1);

        int median_key = target_node.keys[degree - 1];
        target_node.keys.resize(degree - 1); 
        
        parent->keys.insert(parent->keys.begin() + index, median_key);

        if (!target_node.children.empty()) {
            for (size_t i = degree; i <= 2 * degree; i++) {
                new_node.children.push_back(target_node.children[i]);
            }
            target_node.children.resize(degree);
        }

        parent->children.insert(parent->children.begin() + index + 1, new_node.block);

        if (!write_node(parent) || 
            !write_node(&target_node) || 
            !write_node(&new_node)) {
            return false;
        }

        return true;
    }

    bool insert_into(Node* node, size_t key) {
        if (node->children.empty()) {
            auto pos = lower_bound(node->keys.begin(), node->keys.end(), key);
            node->keys.insert(pos, key);
            return write_node(node);
        }

        auto pos = lower_bound(node->keys.begin(), node->keys.end(), key);
        size_t index = pos - node->keys.begin();
        
        Node next_node;
        if (!read_node(node->children[index], &next_node)) {
            return false;
        }
        
        if (next_node.keys.size() == (2 * degree) - 1) {
            if (!split_child(node, index)) {
                return false;
            }
            
            if (key > node->keys[index]) {
                if (!read_node(node->children[index + 1], &next_node)) {
                    return false;
                }
            } else {
                if (!read_node(node->children[index], &next_node)) {
                    return false;
                }
            }
        }
        
        return insert_into(&next_node, key);
    }

    size_t free_block() {
        return num_of_blocks;
    }

    bool seek(size_t block) {
        size_t offset = block_size * block;
        file.seekp(offset);
        file.seekg(offset);
        return !file.fail();
    }

    bool read_node(size_t block, Node* node) { // reading a B-Tree node from the block
        if (!seek(block)) {
            return false;
        }
        
        vector<uint8_t> buf(block_size, 0);
        if (!file.read(reinterpret_cast<char*>(buf.data()), block_size)) {
            return false;
        }
        
        node->block = block;
        
        uint16_t keys_len = (static_cast<uint16_t>(buf[0]) << 8) | buf[1]; // first 2 bytes are taken as key_len
        uint16_t children_len = (static_cast<uint16_t>(buf[2]) << 8) | buf[3]; // next 2 bytes are taken as children_len
        
        node->keys.clear();
        node->children.clear();
        
        size_t i = 4;
        for (size_t j = 0; j < keys_len; j++) { // Convert 4 bytes to uint32_t in big-endian order
            uint32_t key = (static_cast<uint32_t>(buf[i]) << 24) |
                            (static_cast<uint32_t>(buf[i+1]) << 16) |
                            (static_cast<uint32_t>(buf[i+2]) << 8) |
                            (static_cast<uint32_t>(buf[i+3] << 0));
            node->keys.push_back(key);
            i += 4;
        }
        
        i = block_size / 2;
        for (size_t j = 0; j < children_len; j++) { // Convert 4 bytes to uint32_t in big-endian order
            uint32_t child = (static_cast<uint32_t>(buf[i]) << 24) |
                            (static_cast<uint32_t>(buf[i+1]) << 16) |
                            (static_cast<uint32_t>(buf[i+2]) << 8) |
                            (static_cast<uint32_t>(buf[i+3] << 0));
            node->children.push_back(child);
            i += 4;
        }
        
        return true;
    }

    bool write_node(Node* node) { // writing a B-Tree node to the block memory
        if (!seek(node->block)) {
            return false;
        }
        
        vector<uint8_t> block(block_size, 0);
        uint16_t keys_len = static_cast<uint16_t>(node->keys.size()); // Store keys.len() as big-endian u16
        block[0] = (keys_len >> 8) & 0xFF;  // Most significant byte first (big-endian)
        block[1] = keys_len & 0xFF;
        
        uint16_t children_len = static_cast<uint16_t>(node->children.size()); // Store children.len() as big-endian u16
        block[2] = (children_len >> 8) & 0xFF;  // Most significant byte first
        block[3] = children_len & 0xFF;
        
        size_t i = 4;
        for (size_t key : node->keys) { // Convert key to big-endian u32 bytes
            block[i]   = (key >> 24) & 0xFF;  
            block[i+1] = (key >> 16) & 0xFF;
            block[i+2] = (key >> 8) & 0xFF;
            block[i+3] = key & 0xFF;         
            i += 4;
        }
        
        i = block_size / 2;
        for (size_t child : node->children) { // Convert child to big-endian u32 bytes
            block[i]   = (child >> 24) & 0xFF;
            block[i+1] = (child >> 16) & 0xFF;
            block[i+2] = (child >> 8) & 0xFF;
            block[i+3] = child & 0xFF;
            i += 4;
        }
                
        if (!file.write(reinterpret_cast<char*>(block.data()), block_size)) {
            return false;
        }
        if (node->block == num_of_blocks) {
            num_of_blocks++;
        }
        
        return true;
    }

    bool insert(size_t key) {
        if (!root) {
            root = make_unique<Node> ();
            root->keys.push_back(key);
            root->block = 0;
            if (!write_node(root.get())) {
                return false;
            }
            return true;
        }
        
        if (root->keys.size() == (2 * degree) - 1) {
            Node old_root;
            old_root.keys = root->keys;
            old_root.children = root->children;
            old_root.block = free_block();
            
            root->keys.clear();
            root->children.clear();
            root->children.push_back(old_root.block);
            
            if (!write_node(&old_root) || !split_child(root.get(), 0)) {
                return false;
            }
        }
        
        return insert_into(root.get(), key);
    }

    string json(const Node* node) { // To display the B-Tree in Json format
        ostringstream ss;
        ss << "{\"block\":" << node->block << ",\"keys\":[";

        if (!node->keys.empty()) {
            ss << node->keys[0];   
            if (node->keys.size() > 1) {
                ss << ", " << node->keys.back();
            }
        }
        ss << "],\"children\":[";
        if (!node->children.empty()) {
            Node subtree;
            if (read_node(node->children[0], &subtree)) {
                ss << json(&subtree);
            }            
            for (size_t i = 1; i < node->children.size(); i++) {
                ss << ',';
                if (read_node(node->children[i], &subtree)) {
                    ss << json(&subtree);
                }
            }
        }
        ss << "]}";
        return ss.str();
    }
};

int main() {
    try {
        BTree btree = BTree ("btree.bin"); // Creating our B-Tree
        
        for (int i = 1; i <= 8192; i++) {
            if (!btree.insert(i)) {
                cerr << "Failed to insert value: " << i << endl;
                return 1;
            }
        }
        
        Node root;
        if (btree.read_node(0, &root)) {
            cout << btree.json(&root) << endl;
        } else {
            cerr << "Failed to read root node" << endl;
            return 1;
        }
        
        return 0;
    } catch (const exception& e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }
}