#include <utility>
#include <cstddef>
#include <initializer_list>
#include <new>
#include <type_traits>

namespace stlish {

    template<class T>
    class vector {

    // Member Types
    public:
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = pointer;
        using const_iterator = const_pointer;

    public:
        // Constructors
        vector() noexcept; // default constructor
        explicit vector(size_type); // size constructor
        vector(size_type , const_reference); // fill constructor
        template<class InputIt>
        requires (!std::is_integral_v<InputIt>)
        vector(InputIt, InputIt); // Range constructor
        vector(std::initializer_list<value_type> init); //Initializer list constructor
        vector(const vector&); // copy constructor
        vector(vector&&) noexcept; // move constructor
      
        ~vector(); // destructor

        // rule of 5
        vector& operator=(const vector&);
        vector& operator=(vector&&) noexcept;
        vector& operator=(std::initializer_list<value_type>);
        void swap(vector&) noexcept;

    private:
        size_type size_;
        size_type capacity_;
        pointer data_;

    private: 
    // helper skeleton
        void check_invariants() const noexcept;

    public:
        size_type size() const noexcept;
        size_type capacity() const noexcept;
        bool empty() const noexcept;
        pointer data() noexcept;
        const_pointer data() const noexcept;
        size_type max_size() const noexcept;

        // Iterators
        iterator begin() noexcept;
        const_iterator begin() const noexcept;
        const_iterator cbegin() const noexcept;
        iterator end() noexcept;
        const_iterator end() const noexcept;
        const_iterator cend() const noexcept;

        // Element access
        reference operator[](size_type) noexcept;
        const_reference operator[](size_type) const noexcept;
        reference at(size_type);
        const_reference at(size_type) const;
        reference front() noexcept;
        const_reference front() const noexcept;
        reference back() noexcept;
        const_reference back() const noexcept;

        // capacity / modifiers
        void reserve(size_type);
        void clear() noexcept;
        void push_back(const_reference);
        void push_back(value_type&&);
        template<class... Args> reference emplace_back(Args&&... args);
        void pop_back() noexcept;


        private:
        // helper skeleton
        static pointer allocate_raw(size_type);
        static void deallocate_raw(pointer, size_type) noexcept;
        static void destroy_range(pointer, pointer) noexcept;
        template<class InputIt> static pointer uninitialized_copy(pointer, InputIt, InputIt);
        template<class InputIt> static pointer uninitialized_move(pointer dst, InputIt first, InputIt last);
        void reallocate(size_type);
        size_type grow_capacity_for(size_type) const;
        static void check_index(size_type, size_type);
    };

    template <class T>
    typename stlish::vector<T>::pointer
    stlish::vector<T>::allocate_raw(size_type cap) {
        if(cap == 0) { return nullptr; }
        return static_cast<pointer>(::operator new(sizeof(value_type) * cap));

    }

    template <class T>
    void stlish::vector<T>::deallocate_raw(pointer p, size_type cap) noexcept {
        if (cap == 0) return;
        ::operator delete(p);
    }

    template <class T>
    void stlish::vector<T>::destroy_range(pointer first, pointer last) noexcept {
        while (last != first) {
            --last;
            last->~value_type();
        }
    }

    template <class T>
    vector<T>::vector() noexcept : 
        size_{0}, capacity_{0}, data_{nullptr} {}

    template <class T>
    vector<T>::~vector() {
        destroy_range(data_, data_ + size_);
        deallocate_raw(data_, capacity_);

        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;
    }

    template <class T>
    void vector<T>::clear() noexcept {
        destroy_range(data_, data_ + size_);
        size_ = 0;
    }

    template <class T>
    vector<T>::vector(size_type n) : 
        size_{0}, capacity_{0}, data_{nullptr} {
        if (n == 0) return;

        pointer p = allocate_raw(n);
        size_type i = 0;

        try {
            for (; i < n; ++i){
                ::new (static_cast<void*>(p+i)) value_type();
            }
        } catch (...) {
            destroy_range(p, p + i);
            deallocate_raw(p, n);
            throw;
        }

        data_ = p;
        size_ = n;
        capacity_ = n;
    }

    template <class T>
    stlish::vector<T>::vector(size_type n, const_reference value) :
        size_{0}, capacity_{0}, data_{nullptr} {
            if (n == 0) return;

            pointer p = allocate_raw(n);
            size_type i = 0;

            try {
                for(; i < n; ++i) {
                    ::new (static_cast<void*>(p + i)) value_type(value);
                }
            } catch (...) {
                destroy_range(p, p + i);
                deallocate_raw(p, n);
                throw;
            }

            data_ = p;
            size_ = n;
            capacity_ = n;
        }

    template <class T> 
    vector<T>::vector(const vector& other) :
        size_{0}, capacity_{0}, data_{nullptr} {
            if (other.size_ == 0) return;

            const size_type n = other.size_;
            pointer p = allocate_raw(n);
            size_type i = 0;
            
            try {
                for (; i < n; ++i){
                    ::new (static_cast<void*>(p + i)) value_type(other.data_[i]);
                }
            }
            catch(...) {
                destroy_range(p, p + i);
                deallocate_raw(p, n);
                throw;
            }

            data_ = p;
            size_ = n;
            capacity_ = n;
            
        }

    template <class T>
    vector<T>::vector(vector&& other) noexcept :
        size_{other.size_}, capacity_{other.capacity_}, data_{other.data_} {
            other.size_ = 0;
            other.capacity_ = 0;
            other.data_ = nullptr;
        }

    template <class T>
    typename stlish::vector<T>::size_type
    stlish::vector<T>::size() const noexcept { return size_; }

    template <class T>
    typename stlish::vector<T>::size_type
    stlish::vector<T>::capacity() const noexcept { return capacity_; }

    template <class T>
    bool stlish::vector<T>::empty() const noexcept { return size_ == 0; }

    template <class T>
    typename stlish::vector<T>::pointer
    stlish::vector<T>::data() noexcept { return data_; }

    template <class T>
    typename stlish::vector<T>::const_pointer
    stlish::vector<T>::data() const noexcept { return data_; }

    template <class T>
    typename stlish::vector<T>::iterator
    stlish::vector<T>::begin() noexcept { return data_; }

    template <class T>
    typename stlish::vector<T>::const_iterator
    stlish::vector<T>::begin() const noexcept { return data_; }

    template <class T>
    typename stlish::vector<T>::const_iterator
    stlish::vector<T>::cbegin() const noexcept { return data_; }

    template <class T>
    typename stlish::vector<T>::iterator
    stlish::vector<T>::end() noexcept { return data_ + size_; }

    template <class T>
    typename stlish::vector<T>::const_iterator
    stlish::vector<T>::end() const noexcept { return data_ + size_; }

    template <class T>
    typename stlish::vector<T>::const_iterator
    stlish::vector<T>::cend() const noexcept { return data_ + size_; }

} // namespace stlish