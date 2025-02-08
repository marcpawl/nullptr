#pragma once

// Legacy code generated with Chat GPT.

#include <iostream>

class SortedDoublyLinkedList {
private:
    struct Node {
        int data;
        Node* prev;
        Node* next;
        
        Node(int value) : data(value), prev(nullptr), next(nullptr) {}
    };
    
    Node* head;
    Node* tail;

public:
    SortedDoublyLinkedList() : head(nullptr), tail(nullptr) {}

    ~SortedDoublyLinkedList() {
        clear();
    }

    void insert(int value) {
        Node* newNode = new Node(value);
        
        if (!head) {
            head = tail = newNode;
            return;
        }

        Node* current = head;
        while (current && current->data < value) {
            current = current->next;
        }

        if (current == head) { // Insert before head
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        } else if (current == nullptr) { // Insert at the end
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        } else { // Insert in the middle
            newNode->prev = current->prev;
            newNode->next = current;
            current->prev->next = newNode;
            current->prev = newNode;
        }
    }

    void clear() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
    }

    void print() const {
        Node* current = head;
        while (current) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }

    void replaceWithMean() {
        Node* current = head;
        
        while (current && current->next && current->next->next) {
            int mean = (current->data + current->next->data + current->next->next->data) / 3;
            current->next->data = mean;
            current = current->next; // Move to the next group of three
        }
    }

void popHead() {
    if (!head) return; // List is empty

    Node* temp = head;
    head = head->next;

    if (head) {
        head->prev = nullptr; // Update the new head's previous pointer
    } else {
        tail = nullptr; // List is now empty
    }

    delete temp; // Free the old head node
}

void popTail() {
    if (!tail) return; // List is empty

    Node* temp = tail;
    tail = tail->prev;

    if (tail) {
        tail->next = nullptr; // Update the new tail's next pointer
    } else {
        head = nullptr; // List is now empty
    }

    delete temp; // Free the old tail node
}
}
