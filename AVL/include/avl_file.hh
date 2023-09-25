#pragma once
#include "node.hh"
#include "../utils/load.hh"
#include <vector>
#include <functional>

template<typename TK = char[13], typename RecordType = AudioFeatures,
         typename Extractor = std::function<TK(RecordType &)>, typename Greater = std::greater<TK>>
class AVLFile {
private:
    long height(long position) {
        if (position == NULL_POSITION) return -1;

        file.seekg(position);
        Node<TK> node;
        file >> node;

        return node.height;
    }

    long balancing_factor(long position) {
        if (position == NULL_POSITION) return 0;

        file.seekg(position);
        Node<TK> node;
        file >> node;

        return this->height(node.left) - this->height(node.right);
    }

    void update_height(long position, Node<TK> &node) {
        if (position == NULL_POSITION) return;

        node.height = std::max(this->height(node.left), this->height(node.right)) + 1;

        file.seekp(position);
        file << node;
    }

    long rotate_left(long position) {
        file.seekg(position);
        Node<TK> node;
        file >> node;

        Node<TK> right_node;
        file.seekg(node.right, std::ios::beg);
        file >> right_node;

        long right_position = node.right;

        node.right = right_node.left;
        right_node.left = right_position;

        file.seekp(right_position, std::ios::beg);
        file << node;
        file.seekp(position, std::ios::beg);
        file << right_node;

        this->update_height(right_position, node);
        this->update_height(position, right_node);

        return node.right;
    }

    long rotate_right(long position) {
        file.seekg(position);
        Node<TK> node;
        file >> node;

        Node<TK> left_node;
        file.seekg(node.left, std::ios::beg);
        file >> left_node;

        long left_position = node.left;

        node.left = left_node.right;
        left_node.right = left_position;

        file.seekp(left_position, std::ios::beg);
        file << node;
        file.seekp(position, std::ios::beg);
        file << left_node;

        this->update_height(left_position, node);
        this->update_height(position, left_node);

        return node.left;
    }

    void balance(long position) {
        if (position == NULL_POSITION) return;

        file.seekg(position);
        Node<TK> node;
        file >> node;

        long balancing_factor = this->balancing_factor(position);

        if (balancing_factor >= 2) {
            long left_position = node.left;

            file.seekg(left_position);
            Node<TK> left_node;
            file >> left_node;

            if (this->balancing_factor(left_position) <= -1) this->rotate_left(left_position);
            this->rotate_right(position);
        }

        if (balancing_factor <= -2) {
            long right_position = node.right;

            file.seekg(right_position);
            Node<TK> right_node;
            file >> right_node;

            if (this->balancing_factor(right_position) >= 1) this->rotate_right(right_position);
            this->rotate_left(position);
        }
    }
    
private:
    long insert_database(RecordType record) {
        std::fstream file(database, std::ios::binary | std::ios::app);

        long physical_position = file.tellp();
        file << record;
        
        file.close();

        return physical_position;
    }

    RecordType search_database(long position) {
        std::fstream file(database, std::ios::binary | std::ios::in);

        file.seekg(position, std::ios::beg);
        RecordType record;
        file >> record;

        file.close();

        return record;
    }

    long insert(long position, RecordType record) {
        if (position == NULL_POSITION) {
            long physical_position = insert_database(record);

            file.seekp(0, std::ios::end);
            long insertion_position = file.tellp();

            Node<TK> node(extractor(record), physical_position);
            file << node;

            return insertion_position;
        }

        Node<TK> current;
        file.seekg(position, std::ios::beg);
        file >> current;

        if (greater(current.key, extractor(record))) {
            if (long inserted_position = this->insert(current.left, record)) current.left = inserted_position;
        } else if (greater(extractor(record), current.key)) {
            if (long inserted_position = this->insert(current.right, record)) current.right = inserted_position;
        } else {
            long physical_position = insert_database(record);

            file.seekp(0, std::ios::end);
            long insertion_position = file.tellp();

            Node<TK> next(extractor(record), physical_position);
            next.next = current.next;
            file << next;

            current.next = insertion_position;
            file.seekp(position, std::ios::beg);
            file << current;

            return EXIT_SUCCESS;
        }

        this->update_height(position, current);
        this->balance(position);

        return EXIT_SUCCESS;
    }

    void search(long position, TK key, std::vector<RecordType> &records) {
        if (position == NULL_POSITION) return;

        Node<TK> current;
        file.seekg(position, std::ios::beg);
        file >> current;

        if (greater(current.key, key)) this->search(current.left, key, records);
        else if (greater(key, current.key)) this->search(current.right, key, records);
        else {

            records.push_back(search_database(current.physical_position));

            long iterator_position = current.next;
            while (iterator_position != NULL_POSITION) {
                Node<TK> next;
                file.seekg(iterator_position, std::ios::beg);
                file >> next;

                records.push_back(search_database(next.physical_position));

                iterator_position = next.next;
            }
        }
    }

    void range_search(long position, TK begin, TK end, std::vector<RecordType> &records) {
        if (position == NULL_POSITION) return;

        Node<TK> current;
        file.seekg(position, std::ios::beg);
        file >> current;

        if (greater(current.key, begin)) this->range_search(current.left, begin, end, records);
        if (greater(end, current.key)) this->range_search(current.right, begin, end, records);

        if (greater(current.key, begin) && greater(end, current.key)) {
            records.push_back(search_database(current.physical_position));

            long iterator_position = current.next;
            while (iterator_position != NULL_POSITION) {
                Node<TK> next;
                file.seekg(iterator_position, std::ios::beg);
                file >> next;

                records.push_back(search_database(next.physical_position));

                iterator_position = next.next;
            }
        }
    }

    void print_inorder(long position) {
        if (position == NULL_POSITION) return;

        Node<TK> current;
        file.seekg(position, std::ios::beg);
        file >> current;

        print_inorder(current.left);
        std::cout << current.to_string() << std::endl;

        long iterator_position = current.next;
        while (iterator_position != NULL_POSITION) {
            Node<TK> next;
            file.seekg(iterator_position, std::ios::beg);
            file >> next;

            std::cout << next.to_string() << std::endl;

            iterator_position = next.next;
        }

        print_inorder(current.right);
    }

private:
    long root;
    
    std::string database;
    std::string file_name;
    std::fstream file;

    Greater greater;
    Extractor extractor;

public:
    explicit AVLFile(const std::string& database, Extractor extractor, Greater greater) : 
        database(database + ".bin"), file_name(database + "_avl.bin"), extractor(extractor), 
        greater(greater), root(NULL_POSITION) {
        file.open(file_name, std::ios::binary | std::ios::app);
        if (file.tellp() > 0) root = INITIAL_POSITION;
        file.close();
    }

    void insert(RecordType record) {
        file.open(file_name, std::ios::binary | std::ios::out | std::ios::in);

        if (root == NULL_POSITION) root = this->insert(root, record);
        else this->insert(root, record);

        file.close();
    }
    
    std::vector<RecordType> search(TK key) {
        file.open(file_name, std::ios::binary | std::ios::in);
        std::vector<RecordType> records;
        this->search(root, key, records);
        file.close();

        return records;
    }

    std::vector<RecordType> range_search(TK begin, TK end) {
        std::vector<RecordType> records;
        file.open(file_name, std::ios::binary | std::ios::in);
        this->range_search(root, begin, end, records);
        file.close();

        return records;
    }

    void print_inorder() {
        file.open(file_name, std::ios::binary | std::ios::in);
        print_inorder(root);
        file.close();
    }

    void load(const std::string& csv) {
        std::fstream file(csv, std::ios::in);
        std::string line;

        std::getline(file, line);
        while (std::getline(file, line)) {
            std::string isrc, acousticness, danceability, 
                        duration_ms, energy;

            std::stringstream stream(line);

            std::getline(stream, isrc, ',');
            std::getline(stream, acousticness, ',');
            std::getline(stream, danceability, ',');
            std::getline(stream, duration_ms, ',');
            std::getline(stream, energy, ',');

            AudioFeatures r(isrc, string_to_double(acousticness), string_to_double(danceability), 
                        std::stoi(duration_ms), string_to_double(energy));
            this->insert(r);
        }
    }

    // code
};
