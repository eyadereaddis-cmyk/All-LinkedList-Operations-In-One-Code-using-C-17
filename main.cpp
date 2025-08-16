// =================================================================
// The Linked List Playground: A Comprehensive C++ Demo
//
// This program brings together all the linked list variations 
// singly (modern & safe), doubly (manual memory),
// circular (manual memory), and the STL's professional versions.
// It's an interactive way to see the theory in action.
//
// Written by: Eyadere Addis
//
// Date: 2025-7-4
// =================================================================
#include <iostream>
#include <memory>
#include <forward_list>
#include <list>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <vector>

// ====================[ UTILITY FUNCTIONS ]====================
int get_int_input(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        } else {
            std::cout << "Invalid input. Please enter an integer.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

size_t get_index_input(const std::string& prompt, size_t max_index) {
    while (true) {
        int value = get_int_input(prompt);
        if (value >= 0 && static_cast<size_t>(value) <= max_index) {
            return static_cast<size_t>(value);
        }
        std::cout << "Invalid index. Must be between 0 and " << max_index << ".\n";
    }
}

// ====================[ LINKED LIST IMPLEMENTATIONS ]====================

// 1. Singly Linked List (unique_ptr)
class LinkedList {
private:
    struct Node {
        int data;
        std::unique_ptr<Node> next;
        Node(int val) : data(val), next(nullptr) {}
    };

    std::unique_ptr<Node> head;
    Node* tail;
    size_t count;

    Node* getNodeAtIndex(size_t index) const {
        if (index >= count) return nullptr;
        Node* current = head.get();
        for (size_t i = 0; i < index && current; ++i) {
            current = current->next.get();
        }
        return current;
    }

public:
    LinkedList() : head(nullptr), tail(nullptr), count(0) {}
    ~LinkedList() = default;

    bool isEmpty() const { return !head; }
    size_t getSize() const { return count; }
    int front() const { 
        if (isEmpty()) throw std::runtime_error("List empty");
        return head->data; 
    }
    int back() const { 
        if (isEmpty()) throw std::runtime_error("List empty");
        return tail->data; 
    }

    void push_front(int value) {
        auto new_node = std::make_unique<Node>(value);
        if (isEmpty()) {
            tail = new_node.get();
        } else {
            new_node->next = std::move(head);
        }
        head = std::move(new_node);
        count++;
    }

    void push_back(int value) {
        auto new_node = std::make_unique<Node>(value);
        if (isEmpty()) {
            head = std::move(new_node);
            tail = head.get();
        } else {
            tail->next = std::move(new_node);
            tail = tail->next.get();
        }
        count++;
    }

    int pop_front() {
        if (isEmpty()) throw std::runtime_error("List empty");
        int value = head->data;
        head = std::move(head->next);
        count--;
        if (isEmpty()) tail = nullptr;
        return value;
    }

    int pop_back() {
        if (isEmpty()) throw std::runtime_error("List empty");
        if (count == 1) return pop_front();
        
        Node* prev = head.get();
        while (prev->next && prev->next.get() != tail) {
            prev = prev->next.get();
        }
        
        int value = tail->data;
        prev->next = nullptr;
        tail = prev;
        count--;
        return value;
    }

    void insert_after(size_t index, int value) {
        if (index >= count) throw std::out_of_range("Index out of bounds");
        Node* prev = getNodeAtIndex(index);
        if (!prev) throw std::logic_error("Internal error");
        
        if (prev == tail) {
            push_back(value);
        } else {
            auto new_node = std::make_unique<Node>(value);
            new_node->next = std::move(prev->next);
            prev->next = std::move(new_node);
            count++;
        }
    }

    int remove_at(size_t index) {
        if (index >= count) throw std::out_of_range("Index out of bounds");
        if (index == 0) return pop_front();
        
        Node* prev = getNodeAtIndex(index - 1);
        if (!prev || !prev->next) throw std::logic_error("Internal error");
        
        int value = prev->next->data;
        bool removing_tail = (prev->next.get() == tail);
        prev->next = std::move(prev->next->next);
        count--;
        
        if (removing_tail) tail = prev;
        return value;
    }

    void display() const {
        if (isEmpty()) {
            std::cout << "List: [Empty]\n";
            return;
        }
        Node* current = head.get();
        std::cout << "List: ";
        while (current) {
            std::cout << current->data << " -> ";
            current = current->next.get();
        }
        std::cout << "nullptr\n";
    }

    void clear() {
        head = nullptr;
        tail = nullptr;
        count = 0;
    }

    void reverse() {
        if (count <= 1) return;
        tail = head.get();
        std::unique_ptr<Node> prev = nullptr;
        std::unique_ptr<Node> current = std::move(head);
        
        while (current) {
            std::unique_ptr<Node> next = std::move(current->next);
            current->next = std::move(prev);
            prev = std::move(current);
            current = std::move(next);
        }
        head = std::move(prev);
    }

    void sortList() {
        if (count <= 1) return;
        std::vector<int> values;
        values.reserve(count);
        
        Node* current = head.get();
        while (current) {
            values.push_back(current->data);
            current = current->next.get();
        }
        
        std::sort(values.begin(), values.end());
        current = head.get();
        for (int val : values) {
            if (current) {
                current->data = val;
                current = current->next.get();
            } else break;
        }
    }

    std::pair<int, int> findMinMax() const {
        if (isEmpty()) throw std::runtime_error("List empty");
        int min_val = head->data;
        int max_val = head->data;
        
        Node* current = head->next.get();
        while (current) {
            min_val = std::min(min_val, current->data);
            max_val = std::max(max_val, current->data);
            current = current->next.get();
        }
        return std::make_pair(min_val, max_val);  // Changed from structured binding
    }
};

// 2. Doubly Linked List (raw pointers)
class DoublyLinkedList {
private:
    struct DNode {
        int data;
        DNode* prev;
        DNode* next;
        DNode(int val) : data(val), prev(nullptr), next(nullptr) {}
    };

    DNode* head;
    DNode* tail;
    size_t count;

    void linkNodes(DNode* a, DNode* b) {
        if (a) a->next = b;
        if (b) b->prev = a;
    }

    DNode* getNodeAtIndex(size_t index) const {
        if (index >= count) return nullptr;
        
        DNode* current = head;
        for (size_t i = 0; i < index && current; ++i) {
            current = current->next;
        }
        return current;
    }

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), count(0) {}
    ~DoublyLinkedList() { clear(); }

    bool isEmpty() const { return !head; }
    size_t getSize() const { return count; }
    int front() const { 
        if (isEmpty()) throw std::runtime_error("List empty");
        return head->data; 
    }
    int back() const { 
        if (isEmpty()) throw std::runtime_error("List empty");
        return tail->data; 
    }

    void push_front(int value) {
        DNode* new_node = new DNode(value);
        if (isEmpty()) {
            head = tail = new_node;
        } else {
            linkNodes(new_node, head);
            head = new_node;
        }
        count++;
    }

    void push_back(int value) {
        DNode* new_node = new DNode(value);
        if (isEmpty()) {
            head = tail = new_node;
        } else {
            linkNodes(tail, new_node);
            tail = new_node;
        }
        count++;
    }

    int pop_front() {
        if (isEmpty()) throw std::runtime_error("List empty");
        DNode* to_delete = head;
        int value = to_delete->data;
        
        head = head->next;
        if (head) head->prev = nullptr;
        else tail = nullptr;
        
        delete to_delete;
        count--;
        return value;
    }

    int pop_back() {
        if (isEmpty()) throw std::runtime_error("List empty");
        DNode* to_delete = tail;
        int value = to_delete->data;
        
        tail = tail->prev;
        if (tail) tail->next = nullptr;
        else head = nullptr;
        
        delete to_delete;
        count--;
        return value;
    }

    void insert_after(size_t index, int value) {
        if (index >= count) throw std::out_of_range("Index out of bounds");
        DNode* prev = getNodeAtIndex(index);
        if (!prev) throw std::logic_error("Internal error");
        
        if (prev == tail) {
            push_back(value);
        } else {
            DNode* new_node = new DNode(value);
            linkNodes(new_node, prev->next);
            linkNodes(prev, new_node);
            count++;
        }
    }

    int remove_at(size_t index) {
        if (index >= count) throw std::out_of_range("Index out of bounds");
        if (index == 0) return pop_front();
        if (index == count - 1) return pop_back();
        
        DNode* to_delete = getNodeAtIndex(index);
        if (!to_delete) throw std::logic_error("Internal error");
        
        int value = to_delete->data;
        linkNodes(to_delete->prev, to_delete->next);
        
        delete to_delete;
        count--;
        return value;
    }

    void display() const {
        if (isEmpty()) {
            std::cout << "Doubly List: [Empty]\n";
            return;
        }
        DNode* current = head;
        std::cout << "Doubly List: head <=> ";
        while (current) {
            std::cout << current->data << " <=> ";
            current = current->next;
        }
        std::cout << "nullptr\n";
    }

    void clear() {
        while (!isEmpty()) {
            pop_front();
        }
    }

    void reverse() {
        if (count <= 1) return;
        DNode* current = head;
        while (current) {
            std::swap(current->prev, current->next);
            current = current->prev;  // Note: prev and next are swapped
        }
        std::swap(head, tail);
    }

    void sortList() {
        if (count <= 1) return;
        std::vector<int> values;
        values.reserve(count);
        
        DNode* current = head;
        while (current) {
            values.push_back(current->data);
            current = current->next;
        }
        
        std::sort(values.begin(), values.end());
        current = head;
        for (int val : values) {
            if (current) {
                current->data = val;
                current = current->next;
            } else break;
        }
    }

    std::pair<int, int> findMinMax() const {
        if (isEmpty()) throw std::runtime_error("List empty");
        int min_val = head->data;
        int max_val = head->data;
        
        DNode* current = head->next;
        while (current) {
            min_val = std::min(min_val, current->data);
            max_val = std::max(max_val, current->data);
            current = current->next;
        }
        return std::make_pair(min_val, max_val);  // Changed from structured binding
    }
};

// 3. Circular Linked List (raw pointers)
class CircularLinkedList {
private:
    struct CNode {
        int data;
        CNode* next;
        CNode(int val) : data(val), next(nullptr) {}
    };

    CNode* tail_ptr;
    size_t count;

    CNode* getNodeAtIndex(size_t index) const {
        if (index >= count) return nullptr;
        if (!tail_ptr) return nullptr;
        
        CNode* current = tail_ptr->next;
        for (size_t i = 0; i < index; ++i) {
            current = current->next;
        }
        return current;
    }

public:
    CircularLinkedList() : tail_ptr(nullptr), count(0) {}
    ~CircularLinkedList() { clear(); }

    bool isEmpty() const { return !tail_ptr; }
    size_t getSize() const { return count; }
    int front() const { 
        if (isEmpty()) throw std::runtime_error("List empty");
        return tail_ptr->next->data; 
    }
    int back() const { 
        if (isEmpty()) throw std::runtime_error("List empty");
        return tail_ptr->data; 
    }

    void push_front(int value) {
        CNode* new_node = new CNode(value);
        if (isEmpty()) {
            tail_ptr = new_node;
            tail_ptr->next = tail_ptr;
        } else {
            new_node->next = tail_ptr->next;
            tail_ptr->next = new_node;
        }
        count++;
    }

    void push_back(int value) {
        push_front(value);
        tail_ptr = tail_ptr->next;
    }

    int pop_front() {
        if (isEmpty()) throw std::runtime_error("List empty");
        CNode* to_delete = tail_ptr->next;
        int value = to_delete->data;
        
        if (tail_ptr == to_delete) {
            tail_ptr = nullptr;
        } else {
            tail_ptr->next = to_delete->next;
        }
        
        delete to_delete;
        count--;
        return value;
    }

    // Circular lists typically don't have efficient pop_back (O(n) operation)
    int pop_back() {
        if (isEmpty()) throw std::runtime_error("List empty");
        if (count == 1) return pop_front();
        
        // Find the node before tail
        CNode* prev = tail_ptr->next;
        while (prev->next != tail_ptr) {
            prev = prev->next;
        }
        
        int value = tail_ptr->data;
        prev->next = tail_ptr->next;
        delete tail_ptr;
        tail_ptr = prev;
        count--;
        return value;
    }

    void insert_after(size_t index, int value) {
        if (index >= count) throw std::out_of_range("Index out of bounds");
        CNode* prev = getNodeAtIndex(index);
        if (!prev) throw std::logic_error("Internal error");
        
        CNode* new_node = new CNode(value);
        new_node->next = prev->next;
        prev->next = new_node;
        
        if (prev == tail_ptr) {
            tail_ptr = new_node;
        }
        count++;
    }

    int remove_at(size_t index) {
        if (index >= count) throw std::out_of_range("Index out of bounds");
        if (index == 0) return pop_front();
        
        CNode* prev = getNodeAtIndex(index - 1);
        if (!prev || !prev->next) throw std::logic_error("Internal error");
        
        CNode* to_delete = prev->next;
        int value = to_delete->data;
        
        prev->next = to_delete->next;
        if (to_delete == tail_ptr) {
            tail_ptr = prev;
        }
        
        delete to_delete;
        count--;
        return value;
    }

    void display() const {
        if (isEmpty()) {
            std::cout << "Circular List: [Empty]\n";
            return;
        }
        CNode* current = tail_ptr->next;
        std::cout << "Circular List: -> ";
        size_t printed = 0;
        do {
            std::cout << current->data << " -> ";
            current = current->next;
            printed++;
        } while (current != tail_ptr->next && printed <= count);
        
        if (current == tail_ptr->next) {
            std::cout << "(head:" << current->data << ")";
        }
        std::cout << "\n";
    }

    void clear() {
        if (isEmpty()) return;
        
        CNode* current = tail_ptr->next;
        tail_ptr->next = nullptr;
        
        while (current) {
            CNode* next = current->next;
            delete current;
            current = next;
        }
        tail_ptr = nullptr;
        count = 0;
    }

    void reverse() {
        if (count <= 1) return;
        
        CNode* prev = tail_ptr;
        CNode* current = tail_ptr->next;
        CNode* next = nullptr;
        
        do {
            next = current->next;
            current->next = prev;
            prev = current;
            current = next;
        } while (current != tail_ptr->next);
        
        tail_ptr = current->next;
    }

    void sortList() {
        if (count <= 1) return;
        std::vector<int> values;
        values.reserve(count);
        
        CNode* current = tail_ptr->next;
        do {
            values.push_back(current->data);
            current = current->next;
        } while (current != tail_ptr->next);
        
        std::sort(values.begin(), values.end());
        current = tail_ptr->next;
        for (int val : values) {
            if (current) {
                current->data = val;
                current = current->next;
            } else break;
        }
    }

    std::pair<int, int> findMinMax() const {
        if (isEmpty()) throw std::runtime_error("List empty");
        int min_val = tail_ptr->next->data;
        int max_val = tail_ptr->next->data;
        
        CNode* current = tail_ptr->next->next;
        while (current != tail_ptr->next) {
            min_val = std::min(min_val, current->data);
            max_val = std::max(max_val, current->data);
            current = current->next;
        }
        return std::make_pair(min_val, max_val);  // Changed from structured binding
    }
};

// ====================[ MENU FUNCTIONS ]====================
void display_list_menu(const std::string& list_type) {
    std::cout << "\n=== " << list_type << " Linked List Menu ===" << std::endl;
    std::cout << " 1. Add at End (push_back)" << std::endl;
    std::cout << " 2. Add at Start (push_front)" << std::endl;
    std::cout << " 3. Delete at End (pop_back)" << std::endl;
    std::cout << " 4. Delete at Start (pop_front)" << std::endl;
    std::cout << " 5. Insert After Index" << std::endl;
    std::cout << " 6. Delete At Index" << std::endl;
    std::cout << " 7. Display List" << std::endl;
    std::cout << " 8. Get Size" << std::endl;
    std::cout << " 9. Clear List" << std::endl;
    std::cout << "10. Reverse List" << std::endl;
    std::cout << "11. Sort List" << std::endl;
    std::cout << "12. Find Min/Max" << std::endl;
    std::cout << " 0. Back to Main Menu" << std::endl;
    std::cout << "=============================" << std::endl;
}

template <typename ListType>
void run_list_interactive(ListType& list, const std::string& list_name) {
    int choice;
    do {
        display_list_menu(list_name);
        choice = get_int_input("Enter your choice: ");
        
        try {
            switch (choice) {
                case 1: {
                    int value = get_int_input("Enter value to add at end: ");
                    list.push_back(value);
                    std::cout << "Added to end.\n";
                    break;
                }
                case 2: {
                    int value = get_int_input("Enter value to add at start: ");
                    list.push_front(value);
                    std::cout << "Added to start.\n";
                    break;
                }
                case 3: {
                    if (list.isEmpty()) {
                        std::cout << "List empty.\n";
                    } else {
                        std::cout << "Deleted " << list.pop_back() << " from end.\n";
                    }
                    break;
                }
                case 4: {
                    if (list.isEmpty()) {
                        std::cout << "List empty.\n";
                    } else {
                        std::cout << "Deleted " << list.pop_front() << " from start.\n";
                    }
                    break;
                }
                case 5: {
                    if (list.isEmpty()) {
                        std::cout << "List empty.\n";
                        break;
                    }
                    size_t idx = get_index_input("Enter index AFTER: ", list.getSize() - 1);
                    int val = get_int_input("Enter value: ");
                    list.insert_after(idx, val);
                    std::cout << "Inserted " << val << " after index " << idx << ".\n";
                    break;
                }
                case 6: {
                    if (list.isEmpty()) {
                        std::cout << "List empty.\n";
                        break;
                    }
                    size_t idx = get_index_input("Enter index AT: ", list.getSize() - 1);
                    std::cout << "Deleted " << list.remove_at(idx) << " at index " << idx << ".\n";
                    break;
                }
                case 7:
                    list.display();
                    break;
                case 8:
                    std::cout << "Size: " << list.getSize() << "\n";
                    break;
                case 9:
                    list.clear();
                    std::cout << "List cleared.\n";
                    break;
                case 10:
                    list.reverse();
                    std::cout << "List reversed.\n";
                    list.display();
                    break;
                case 11:
                    list.sortList();
                    std::cout << "List sorted.\n";
                    list.display();
                    break;
                case 12: {
                    if (list.isEmpty()) {
                        std::cout << "List empty.\n";
                        break;
                    }
                    std::pair<int, int> result = list.findMinMax();
                    std::cout << "Min: " << result.first << ", Max: " << result.second << "\n";
                    break;
                }
                case 0:
                    std::cout << "Returning to main menu...\n";
                    break;
                default:
                    std::cout << "Invalid choice.\n";
            }
            
            if (choice > 0 && choice != 7 && choice != 8 && choice != 10 && choice != 11 && choice != 12) {
                list.display();
                std::cout << "(Size: " << list.getSize() << ")\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "*** Error: " << e.what() << " ***\n";
        }
    } while (choice != 0);
}

void display_main_menu() {
    std::cout << "\n=========== Main Menu ===========" << std::endl;
    std::cout << "1. Singly Linked List (unique_ptr)" << std::endl;
    std::cout << "2. Doubly Linked List (raw ptr)" << std::endl;
    std::cout << "3. Circular Linked List (raw ptr)" << std::endl;
    std::cout << "4. STL forward_list (Singly)" << std::endl;
    std::cout << "5. STL list (Doubly)" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "================================" << std::endl;
}

void run_stl_forward_list_demo() {
    std::forward_list<int> flist;
    int choice;
    
    do {
        display_list_menu("STL forward_list");
        choice = get_int_input("Enter your choice: ");
        
        try {
            switch (choice) {
                case 1: {
                    int value = get_int_input("Enter value to add at end: ");
                    flist.push_front(value);  // STL forward_list doesn't have push_back
                    std::cout << "Added to front (STL forward_list has no push_back).\n";
                    break;
                }
                case 2: {
                    int value = get_int_input("Enter value to add at start: ");
                    flist.push_front(value);
                    std::cout << "Added to start.\n";
                    break;
                }
                case 3: {
                    if (flist.empty()) {
                        std::cout << "List empty.\n";
                    } else {
                        flist.pop_front();  // STL forward_list doesn't have pop_back
                        std::cout << "Deleted from front (STL forward_list has no pop_back).\n";
                    }
                    break;
                }
                case 4: {
                    if (flist.empty()) {
                        std::cout << "List empty.\n";
                    } else {
                        flist.pop_front();
                        std::cout << "Deleted from start.\n";
                    }
                    break;
                }
                case 7: {
                    std::cout << "STL forward_list: ";
                    for (const auto& val : flist) {
                        std::cout << val << " -> ";
                    }
                    std::cout << "nullptr\n";
                    break;
                }
                case 8: {
                    std::cout << "Size: " << std::distance(flist.begin(), flist.end()) << "\n";
                    break;
                }
                case 9: {
                    flist.clear();
                    std::cout << "List cleared.\n";
                    break;
                }
                case 0:
                    std::cout << "Returning to main menu...\n";
                    break;
                default:
                    std::cout << "Operation not supported by STL forward_list or invalid choice.\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "*** Error: " << e.what() << " ***\n";
        }
    } while (choice != 0);
}

void run_stl_list_demo() {
    std::list<int> dlist;
    int choice;
    
    do {
        display_list_menu("STL list");
        choice = get_int_input("Enter your choice: ");
        
        try {
            switch (choice) {
                case 1: {
                    int value = get_int_input("Enter value to add at end: ");
                    dlist.push_back(value);
                    std::cout << "Added to end.\n";
                    break;
                }
                case 2: {
                    int value = get_int_input("Enter value to add at start: ");
                    dlist.push_front(value);
                    std::cout << "Added to start.\n";
                    break;
                }
                case 3: {
                    if (dlist.empty()) {
                        std::cout << "List empty.\n";
                    } else {
                        dlist.pop_back();
                        std::cout << "Deleted from end.\n";
                    }
                    break;
                }
                case 4: {
                    if (dlist.empty()) {
                        std::cout << "List empty.\n";
                    } else {
                        dlist.pop_front();
                        std::cout << "Deleted from start.\n";
                    }
                    break;
                }
                case 7: {
                    std::cout << "STL list: ";
                    for (const auto& val : dlist) {
                        std::cout << val << " <-> ";
                    }
                    std::cout << "nullptr\n";
                    break;
                }
                case 8: {
                    std::cout << "Size: " << dlist.size() << "\n";
                    break;
                }
                case 9: {
                    dlist.clear();
                    std::cout << "List cleared.\n";
                    break;
                }
                case 10: {
                    dlist.reverse();
                    std::cout << "List reversed.\n";
                    break;
                }
                case 11: {
                    dlist.sort();
                    std::cout << "List sorted.\n";
                    break;
                }
                case 12: {
                    if (dlist.empty()) {
                        std::cout << "List empty.\n";
                        break;
                    }
                    auto min_it = std::min_element(dlist.begin(), dlist.end());
                    auto max_it = std::max_element(dlist.begin(), dlist.end());
                    std::cout << "Min: " << *min_it << ", Max: " << *max_it << "\n";
                    break;
                }
                case 0:
                    std::cout << "Returning to main menu...\n";
                    break;
                default:
                    std::cout << "Invalid choice.\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "*** Error: " << e.what() << " ***\n";
        }
    } while (choice != 0);
}

int main() {
    int choice;
    do {
        display_main_menu();
        choice = get_int_input("Enter your choice: ");
        
        try {
            switch (choice) {
                case 1: {
                    LinkedList sll;
                    run_list_interactive(sll, "Singly");
                    break;
                }
                case 2: {
                    DoublyLinkedList dll;
                    run_list_interactive(dll, "Doubly");
                    break;
                }
                case 3: {
                    CircularLinkedList cll;
                    run_list_interactive(cll, "Circular");
                    break;
                }
                case 4:
                    run_stl_forward_list_demo();
                    break;
                case 5:
                    run_stl_list_demo();
                    break;
                case 0:
                    std::cout << "Exiting program. Goodbye!\n";
                    break;
                default:
                    std::cout << "Invalid choice. Try again.\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "\n*** FATAL ERROR: " << e.what() << " ***\n";
        }
        
        if (choice != 0) {
            std::cout << "\n(Press Enter to continue...)";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
        }
    } while (choice != 0);
    
    return 0;
}
