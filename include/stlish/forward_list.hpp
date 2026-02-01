#pragma once

#include <cstddef>
#include <utility>

namespace stlish {

    template <class T>
    class forward_list {
    public:
        // Member types
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;

    private:
        struct Node {
            T data;
            Node* next;
            
            explicit Node(const T& value, Node* next_ptr = nullptr) :
                data{value}, next{next_ptr} {}
            
            explicit Node(T&& value, Node* next_ptr = nullptr) :
                data{std::move(value)}, next{next_ptr} {}
        };
        
        Node* head_ = nullptr;

    public:
        // ========================================
        // Phase 1: Basic Operations
        // ========================================
        
        // Constructor
        forward_list() = default;
        
        // Destructor
        ~forward_list() {
            clear();
        }
        
        // Element access
        reference front();
        const_reference front() const;
        
        // Capacity
        [[nodiscard]] bool empty() const noexcept;
        
        // Modifiers
        void clear() noexcept;
        void push_front(const T& value);
        void push_front(T&& value);
        void pop_front();
    };

    template <class T>
    typename forward_list<T>::reference
    forward_list<T>::front() {
        return head_->data;
    } 

    template <class T>
    [[nodiscard]] bool forward_list<T>::empty() const noexcept {
        return head_ == nullptr;
    }
      
    template <class T>
    typename forward_list<T>::const_reference
    forward_list<T>::front() const{
        return head_->data;
    } 

    template <class T>
    void forward_list<T>::clear() noexcept {
        Node* current = head_;
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head_ = nullptr;
    } 

    template <class T>
    void forward_list<T>::push_front(const T& value) {
        head_ = new Node(value, head_);
    }

    template <class T>
    void forward_list<T>::push_front(T&& value) {
        head_ = new Node(std::move(value), head_);
    }

    template <class T>
    void forward_list<T>::pop_front() {
        Node* old_head = head_;
        head_ = head_->next;
        delete old_head;
    }
} // namespace stlish