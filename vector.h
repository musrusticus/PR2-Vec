//  fuer strongly secure: eine Instanzvariable auf den vector und pruefen, ob vector noch das ist, was erwartet wird. So wird er strongly secure.
//
//
//
//  Created by Tamara on 07.08.25.
//
#ifndef VECTOR_H
#define VECTOR_H
#include <iostream>

template<typename T>
class Vector {
public:
    class ConstIterator;
    class Iterator;
    using value_type = T;
    using size_type = std::size_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = Vector::Iterator;
    using const_iterator = Vector::ConstIterator;
    using difference_type = std::ptrdiff_t;

private:
    size_type sz;
    size_type max_sz;
    pointer values;
    
    size_type larger() {
        if (!max_sz) {
            max_sz += 5;
        }
        return max_sz*2;
    }
    
    void realloc(size_type bigger) {
        pointer new_values = new value_type[bigger];
        for (size_type i = 0; i < sz; i++) {
            new_values[i] = values[i];
        }
        delete[] values;
        max_sz = bigger;
        values = new_values;
    }
    
public:
    Vector(): sz{0}, max_sz{0}, values{nullptr} {}
    
    Vector(size_type max_sz): sz{0}, max_sz{max_sz}, values{new value_type[max_sz]} {
        if (max_sz < 0)
            max_sz = 0;
        }
    
    Vector(std::initializer_list<value_type> ilist): Vector(ilist.size()) {
        for(const auto& value: ilist) {
            values[sz] = value;
            ++sz;
        }
        
    }
    
    Vector(const Vector& v): Vector(v.sz) {
        for (sz = 0; sz < v.sz; ++sz) {
            values[sz] = v.values[sz];
        }
    }
    
    ~Vector() {
        delete[] values;
        sz = 0;
    }
    
    Vector& operator=(Vector v) {
        using std::swap;
        swap(max_sz, v.max_sz);
        swap(sz, v.sz);
        swap(values, v.values);
        return *this;
    }
    
    void clear() {
        sz = 0;
    }
    
    void push_back(const_reference value) {
        if (sz >= max_sz) {
            realloc(larger());
        }
        values[sz] = value;
        sz++;
    }
    
    reference operator[](size_type idx) {
        if (idx >= sz)
            throw std::out_of_range("Idx out of range");
        return values[idx];
    }
    
    const_reference operator[](size_type idx) const {
        if (idx >= sz)
            throw std::out_of_range("Idx out of range");
        return values[idx];
    }
    
    size_type size() const {
        return sz;
    }
    
    size_type capacity() const {
        return max_sz;
    }
    
    bool empty() const {
        return !sz;
    }
    
    void shrink_to_fit() {
        realloc(sz);
    }
    
    void reserve(size_type bigger) {
        if(bigger > max_sz) {
            realloc(bigger);
        }
    }
    
    void pop_back() {
        if (!sz) {
            throw std::out_of_range("Vector empy");
        }
        --sz;
    }
    
    std::ostream& print(std::ostream& o) const {
        o << "[";
        for (size_type i = 0; i < sz; i++) {
            o << values [i];
            if (i != sz-1)
                o << ", ";
        }
        o << "]";
        return o;
    }
    

    
// iterator shenenigans
    iterator begin() {
        if (!values) {
            return iterator{nullptr};
        }
        return iterator{values, values + sz, this};
    }
    
    iterator end() {
        if (!values) {
            return iterator{nullptr};
        }
        return iterator{values+sz, values+sz, this};
    }
    
    const_iterator begin() const {
        if (!values) {
            return const_iterator{nullptr, nullptr};
        }
        return const_iterator{values, values+sz, this};
    }
    
    const_iterator end() const {
        if (!values) {
            return const_iterator{nullptr, nullptr};
        }
        return const_iterator{values+sz, values+sz, this};
    }
    
    iterator insert(const_iterator pos, const_reference val) {
        auto diff = pos - begin();
        if (diff < 0 || static_cast<size_type>(diff) > sz)
            throw std::out_of_range("Iterator out of bounds");
        size_type current{static_cast<size_type>(diff)};
        if (sz >= max_sz)
            reserve(larger());
        for (auto i{sz}; i--> current;)
            values[i + 1] = values[i];
        values[current] = val;
        ++sz;
        return iterator{values + current, values + sz};
    }
    
    iterator erase(const_iterator pos) {
        auto diff = pos - begin();
        if (diff < 0 || static_cast<size_type>(diff) >= sz)
            throw std::out_of_range("Iterator out of bounds");
        size_type current{static_cast<size_type>(diff)};
        for (auto i{current}; i < sz - 1; ++i)
            values[i] = values[i + 1];
        --sz;
        return iterator{values + current, values + sz};
    }
    
    // class in class starts now
    
    class Iterator {
    public:
        using value_type = Vector::value_type;
        using reference = Vector::reference;
        using pointer = Vector::pointer;
        using difference_type = Vector::difference_type;
        using iterator_category = std::forward_iterator_tag;
    private:
        pointer ptr;
        pointer endptr;
        const Vector* owner;
    public:
        Iterator(): ptr{nullptr}, endptr{nullptr}, owner{nullptr} {}
        
        Iterator(pointer ptr): ptr{ptr}, endptr{nullptr}, owner{nullptr} {}
        
        Iterator(pointer ptr, pointer endptr): ptr{ptr}, endptr{endptr}, owner{nullptr} {}
        Iterator(pointer ptr, pointer endptr, const Vector* owner): ptr{ptr}, endptr{endptr}, owner{owner} {}
        
       /* reference operator*() const {
            if (ptr == endptr) {
                throw std::out_of_range("This is the end. Nothing to dereference here.");
            }
            return *ptr;
        }
        
        pointer operator->() const {
            if (ptr == endptr) {
                throw std::out_of_range("This is the end. Nothing to reference here.");
            }
            return ptr;
        } */
        
        
        
        
        reference operator*() const {
            if (!owner || !owner->values ||
                ptr < owner->values || ptr >= owner->values + owner->sz)
                throw std::runtime_error("Invalid iterator dereference");
            if (ptr == endptr)
                throw std::out_of_range("This is the end. Nothing to dereference here.");
            return *ptr;
        }

        pointer operator->() const {
            if (!owner || !owner->values ||
                ptr < owner->values || ptr >= owner->values + owner->sz)
                throw std::runtime_error("Invalid iterator reference");
            if (ptr == endptr)
                throw std::out_of_range("This is the end. Nothing to reference here.");
            return ptr;
        }
        
        
        
        
        bool operator==(const const_iterator& it) const {
            return static_cast<const_iterator>(*this) == it;
        }
        
        bool operator!=(const const_iterator& it) const {
            return static_cast<const_iterator>(*this) != it;
        }
        
        
        iterator& operator++() {
            if (ptr != endptr) {
                ++ptr;
            }
            return *this;
        }
        
        iterator operator++(int) {
            auto old_ref = *this;
            ++(*this);
            return old_ref;
        }
        
        operator const_iterator() const {
            return const_iterator(ptr, endptr, owner);
        }
        
    }; // end class Iterator
    
    
    
    class ConstIterator {
    public:
        using value_type = Vector::value_type;
        using reference = Vector::const_reference;
        using pointer = Vector::const_pointer;
        using difference_type = Vector::difference_type;
        using iterator_category = std::forward_iterator_tag;
    private:
        pointer ptr;
        pointer endptr;
        const Vector* owner;
    public:
        ConstIterator(): ptr{nullptr}, endptr{nullptr}, owner{nullptr} {}
        
        ConstIterator(pointer ptr): ptr{ptr}, endptr{nullptr}, owner{nullptr} {}
        
        ConstIterator(pointer ptr, pointer endptr): ptr{ptr}, endptr{endptr}, owner{nullptr} {}
        ConstIterator(pointer ptr, pointer endptr, const Vector* owner): ptr{ptr}, endptr{endptr}, owner{owner} {}
        
       /* const_reference operator*() const {
            if (ptr == endptr) {
                throw std::runtime_error{"This is the end. Nothing to dereference here."};
            }
            return *ptr;
        }
        
        pointer operator->() const {
            if (ptr == endptr) {
                throw std::runtime_error{"This is the end. Nothing to reference here."};
            }
            return ptr;
        } */
        
        const_reference operator*() const {
            if (!owner || !owner->values ||
                ptr < owner->values || ptr >= owner->values + owner->sz)
                throw std::runtime_error("Invalid iterator dereference");
            if (ptr == endptr)
                throw std::runtime_error("This is the end. Nothing to dereference here.");
            return *ptr;
        }

        pointer operator->() const {
            if (!owner || !owner->values ||
                ptr < owner->values || ptr >= owner->values + owner->sz)
                throw std::runtime_error("Invalid iterator reference");
            if (ptr == endptr)
                throw std::runtime_error("This is the end. Nothing to reference here.");
            return ptr;
        }
        
        
        
        
        bool operator==(const ConstIterator& it) const {
            return ptr == it.ptr && owner == it.owner;
        }

        bool operator!=(const ConstIterator& it) const {
            return !(*this == it);
        }
        
        const_iterator& operator++() {
            if (ptr != endptr) {
                ++ptr;
            }
            return *this;
        }
        
        const_iterator operator++(int) {
            auto old_ref = (*this);
            ++(*this);
            return old_ref;
        }
        
        //Damit insert and erase so funktionieren, muss auch die folgende Methode implementiert werden:
        
        friend Vector:: difference_type operator-( const Vector::ConstIterator& lop, const Vector::ConstIterator& rop ) {
            return lop.ptr-rop.ptr;
        }
    }; // end class ConstIterator
}; // end class Vector
    
template<typename T>
std::ostream& operator<<(std::ostream& o, const Vector<T>& v) {
    return v.print(o);
}

#endif
