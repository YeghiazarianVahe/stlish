#include <new>

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
        vector(size_type , const reference); // fill constructor
        template<class InputIt> vector(InputIt, InputIt); // Range constructor
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
        size_type size_ = 0;
        size_type capacity_ = 0;
        pointer data_ = nullptr;

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
        while (last != true) {
            --last;
            last->~value_type();
        }
    }

    template <class T>
    stlish::vector<T>::vector() noexcept : 
        data_{nullptr}, size_{0}, capacity_{0} {};

    template <class T>
    stlish::vector<T>::~vector() {
        if (data_ != nullptr) destroy_range(data_, data_ + size);
        deallocate_raw(data_, capacity_);
    }

    template <class T>
    void stlish::vector<T>::clear() noexcept {
        destroy_range(data_, data_ + size_);
        size_ = 0;
    }
} // namespace stlish