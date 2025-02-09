#pragma once


// https://chat.deepseek.com/a/chat/s/f7dc8532-0691-4243-a3c8-0f9390e2b26b
//
// create a C++ class for a doublely linked list holding integers 
// in sorted order, with iterators.
//
// add pop methods for head and tail
//
// include a method to transform the data of all nodes to be the mean
// of the previous node and the sucessor node.

#include <iostream>
#include <stdexcept>

class SortedDoublyLinkedList {
private:
    struct Node {
        int data;
        Node* prev;
        Node* next;

        explicit Node(int val) : data(val), prev(nullptr), next(nullptr) {}
    };

    Node* head;
    Node* tail;

public:
    // Iterator class
    class Iterator {
    private:
        Node* current;

    public:
        explicit Iterator(Node* node) : current(node) {}

        // Dereference operator
        int& operator*() const {
            return current->data;
        }

        // Pre-increment operator
        Iterator& operator++() {
            if (current) {
                current = current->next;
            }
            return *this;
        }

        // Post-increment operator
        Iterator operator++(int) 
	{
            Iterator temp = *this;
            current = current->next;
            return temp;
        }

        // Pre-decrement operator
        Iterator& operator--()  {
            if (current) {
                current = current->prev;
            }
            return *this;
        }

        // Post-decrement operator
        Iterator operator--(int) {
            Iterator temp = *this;
            current = current->prev;
            return temp;
        }

        // Equality operator
        bool operator==(const Iterator& other) const {
            return current == other.current;
        }

        // Inequality operator
        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }
    };

    // Constructor
    SortedDoublyLinkedList() : head(nullptr), tail(nullptr) {}

    // Destructor
    ~SortedDoublyLinkedList() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Insert a value into the sorted list
    void insert(int value) {
        Node* newNode = new Node(value);

        if (!head) {
            // List is empty
            head = tail = newNode;
        } else if (value <= head->data) {
            // Insert at the beginning
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        } else if (value >= tail->data) {
            // Insert at the end
            newNode->prev = tail;
            tail->next = newNode;
            tail = newNode;
        } else {
            // Insert in the middle
            Node* current = head;
            while (current->next->data < value) {
                current = current->next;
            }
            newNode->next = current->next;
            newNode->prev = current;
            current->next->prev = newNode;
            current->next = newNode;
        }
    }

    // Remove a value from the list
    void remove(int value) {
        Node* current = head;

        while (current) {
            if (current->data == value) {
                if (current->prev) {
                    current->prev->next = current->next;
                } else {
                    head = current->next;
                }

                if (current->next) {
                    current->next->prev = current->prev;
                } else {
                    tail = current->prev;
                }

                delete current;
                return;
            }
            current = current->next;
        }

        throw std::runtime_error("Value not found in the list.");
    }

    // Remove and return the value at the head of the list
    int pop_head() {
        if (!head) {
            throw std::runtime_error("List is empty.");
        }

        Node* temp = head;
        int value = temp->data;

        head = head->next;
        if (head) {
            head->prev = nullptr;
        } else {
            tail = nullptr; // List is now empty
        }

        delete temp;
        return value;
    }

    // Remove and return the value at the tail of the list
    int pop_tail() {
        if (!tail) {
            throw std::runtime_error("List is empty.");
        }

        Node* temp = tail;
        int value = temp->data;

        tail = tail->prev;
        if (tail) {
            tail->next = nullptr;
        } else {
            head = nullptr; // List is now empty
        }

        delete temp;
        return value;
    }

    // Transform the data of each node to be the mean of its 
    // previous and successor nodes
    void transform_to_mean() const // NOLINT
    {
        Node* current = head;

        while (current) {
            if (current->prev && current->next) {
                // Calculate the mean of the previous and next node's data
                current->data = (current->prev->data + current->next->data) / 2;
            }
            current = current->next;
        }
    }

    // Iterator to the beginning of the list
    Iterator begin() const {
        return Iterator(head);
    }

    // Iterator to the end of the list (one past the last element)
    static Iterator end()  {
        return Iterator(nullptr);
    }

    // Print the list (for debugging purposes)
    void print() const {
        Node* current = head;
        while (current) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }
};

#if 0
int main() {
    SortedDoublyLinkedList list;

    list.insert(5);
    list.insert(3);
    list.insert(8);
    list.insert(1);
    list.insert(6);

    std::cout << "List after insertions: ";
    list.print();

    list.transform_to_mean();
    std::cout << "List after transforming to mean: ";
    list.print();

    return 0;
}
#endif
