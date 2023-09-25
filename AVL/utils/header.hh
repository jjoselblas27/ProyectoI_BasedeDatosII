#include <vector>
#include <cstring>
#include <fstream>
#include <functional>
#include <string>
#include <iostream>
#include <sstream>
#include "../utils/load.hh"

/*void insert(Record record) {
        if (this->root == NULL_POSITION) {
            this->file.seekp(0, std::ios::end);
            this->root = this->file.tellp();

            Node<TK> node(record);
            this->file << node;

            return;
        }

        this->file.seekg(this->root);
        Node<TK> node;
        this->file >> node;

        if (this->greater(record.key, node.key)) {
            node.right = this->insert(node.right, record);
        } else {
            node.left = this->insert(node.left, record);
        }

        this->file.seekp(this->root);
        this->file << node;

        return;
    }

    void remove(RecordKeyType key) {
        if (this->root == NULL_POSITION) return;

        this->file.seekg(this->root);
        Node<RecordKeyType> node;
        this->file >> node;

        if (this->greater(key, node.key)) {
            node.right = this->remove(node.right, key);
        } else if (this->greater(node.key, key)) {
            node.left = this->remove(node.left, key);
        } else {
            if (node.left == NULL_POSITION || node.right == NULL_POSITION) {
                long left = node.left;
                long right = node.right;

                if (left != NULL_POSITION) {
                    this->file.seekg(left);
                    Node<RecordKeyType> left_node;
                    this->file >> left_node;

                    left_node.right = right;

                    this->file.seekp(left);
                    this->file << left_node;
                } else if (right != NULL_POSITION) {
                    this->file.seekg(right);
                    Node<RecordKeyType> right_node;
                    this->file >> right_node;

                    right_node.left = left;

                    this->file.seekp(right);
                    this->file << right_node;
                }

                this->root = left != NULL_POSITION ? left : right;
            } else {
                long left = node.left;
                long right = node.right;

                this->file.seekg(right);
                Node<RecordKeyType> right_node;
                this->file >> right_node;

                while (right_node.left != NULL_POSITION) {
                    this->file.seekg(right_node.left);
                    this->file >> right_node;
                }

                node.key = right_node.key;
                node.record = right_node.record;

                node.right = this->remove(node.right, node.key);

                this->file.seekp(this->root);
                this->file << node;
            }
        }

        this->file.seekp(this->root);
        this->file << node;

        return;
    }

    Record search(RecordKeyType key) {
        if (this->root == NULL_POSITION) return Record();

        this->file.seekg(this->root);
        Node<RecordKeyType> node;
        this->file >> node;

        if (this->greater(key, node.key)) {
            return this->search(node.right, key);
        } else if (this->greater(node.key, key)) {
            return this->search(node.left, key);
        } else {
            return node.record;
        }
    }

    void print() {
        if (this->root == NULL_POSITION) return;

        this->file.seekg(this->root);
        Node<RecordKeyType> node;
        this->file >> node;

        std::cout << node.key << std::endl;

        this->print(node.left);
        this->print(node.right);
    }



    /*long insert(long position, RecordKeyType key, long record) {
        if (position == NULL_POSITION) {
            file.seekp(0, std::ios::end);
            position = file.tellp();

            Node<RecordKeyType> node(key, record);
            file << node;

            return position;
        }

        file.seekg(position);
        Node<RecordKeyType> node;
        file >> node;

        if (this->greater(key, node.key)) {
            node.right = this->insert(node.right, key, record);
        } else {
            node.left = this->insert(node.left, key, record);
        }

        file.seekp(position);
        file << node;

        return this->balance(position);
    }

    long remove(long position, RecordKeyType key) {
        if (position == NULL_POSITION) return NULL_POSITION;

        file.seekg(position);
        Node<RecordKeyType> node;
        file >> node;

        if (this->greater(key, node.key)) {
            node.right = this->remove(node.right, key);
        } else if (this->greater(node.key, key)) {
            node.left = this->remove(node.left, key);
        } else {
            if (node.left == NULL_POSITION || node.right == NULL_POSITION) {
                long left = node.left;
                long right = node.right;

                if (left != NULL_POSITION) {
                    file.seekg(left);
                    Node<RecordKeyType> left_node;
                    file >> left_node;

                    left_node.right = right;

                    file.seekp(left);
                    file << left_node;
                } else if (right != NULL_POSITION) {
                    file.seekg(right);
                    Node<RecordKeyType> right_node;
                    file >> right_node;

                    right_node.left = left;

                    file.seekp(right);
                    file << right_node;
                }

                return left != NULL_POSITION ? left : right;
            } else {
                long left = node.left;
                long right = node.right;

                file.seekg(right);
                Node<RecordKeyType> right_node;
                file >> right_node;

                while (right_node.left != NULL_POSITION) {
                    file.seekg(right_node.left);
                    file >> right_node;
                }

                node.key = right_node.key;
                node.record = right_node.record;

                node.right = this->remove(node.right, node.key);

                file.seekp(position);
                file << node;
            }
        }

        file.seekp(position);
        file << node;

        return this->balance(position);
    }

    long search(long position, RecordKeyType key) {
        if (position == NULL_POSITION) return NULL_POSITION;

        file.seekg(position);
        Node<RecordKeyType> node;
        file >> node;

        if (this->greater(key, node.key)) {
            return this->search(node.right, key);
        } else if (this->greater(node.key, key)) {
            return this->search(node.left, key);
        } else {
            return position;
        }
    }

    void print(long position) {
        if (position == NULL_POSITION) return;

        file.seekg(position);
        Node<RecordKeyType> node;
        file >> node;

        std::cout << node.key << std::endl;

        this->print(node.left);
        this->print(node.right);
    }*/

/*auto extractor = [](AudioFeatures &af) { return af.isrc; };
    auto greater = [](char a[13], char b[13]) { return std::strcmp(a, b) > 0; };

    auto extractor = [](AudioFeatures &af) { return af.duration_ms; };
    auto greater = [](int a, int b) { return a > b; };

    
    AVLFile<int, AudioFeatures, decltype(extractor), decltype(greater)> avl("database", extractor, greater);

    //AVLFile<char [13], AudioFeatures, decltype(extractor), decltype(greater)> avl("database", extractor, greater);

    AudioFeatures a1("AD4X65752184",0.906,0.65,296733,0.35);
    AudioFeatures a2("AEA0D1991170",0.00095,0.621,191989,0.639);
    AudioFeatures a3("AEA0Q2004008",0.0312,0.692,282904,0.98);

    /*avl.insert(a1);
    avl.insert(a2);
    avl.insert(a3);*/

    /*avl.load("../database/database_20.csv");
    avl.print_inorder();

    auto v = avl.range_search(197904, 207490);

    for (auto &i : v) {
        std::cout << i.to_string() << std::endl;
    }*/