#pragma once

#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <cassert>
#include <iterator>

#include "array_ptr.h"

using namespace std::literals;

class ReserveProxyObj {
    public:
        explicit ReserveProxyObj (size_t reserve)
            : capacity_(reserve) {
        }

        size_t GetCapacity() {
            return capacity_;
        }

    private:
        size_t capacity_;
};

ReserveProxyObj Reserve(size_t reserve) {
    return ReserveProxyObj(reserve);
}

template <typename Type>
class SimpleVector {
    public:
        using Iterator = Type*;
        using ConstIterator = const Type*;

        SimpleVector() noexcept = default;

        explicit SimpleVector(size_t size)
            : SimpleVector(size, std::move(Type{})) {
        }

        SimpleVector(size_t size, const Type &value)
            :   data_(size),
                size_(size),
                capacity_(size) {

            std::fill(begin(), end(), value);
        }

        SimpleVector(std::initializer_list<Type> initlist)
            :   data_(initlist.size()),
                size_(initlist.size()),
                capacity_(initlist.size()) {

            std::copy(std::make_move_iterator(initlist.begin()), std::make_move_iterator(initlist.end()), begin());
        }

        SimpleVector(ReserveProxyObj capacity_to_reserve) {
            Reserve(capacity_to_reserve.GetCapacity());
        }

                SimpleVector(const SimpleVector& other) {
            Reserve(other.capacity_);
            size_ = other.size_;

            std::copy(other.begin(), other.end(), begin());
        }

        SimpleVector(SimpleVector&& other)  {
            data_.swap(other.data_);
            size_ = other.size_;
            capacity_ = size_;
            other.Clear();
        }

        Iterator begin() noexcept {
            return Iterator(&data_[0]);
        }

        Iterator end() noexcept {
            return Iterator(&data_[size_]);
        }

        ConstIterator begin() const noexcept {
            return cbegin();
        }

        ConstIterator end() const noexcept {
            return cend();
        }

        ConstIterator cbegin() const noexcept {
            return ConstIterator(&data_[0]);
        }

        ConstIterator cend() const noexcept {
            return ConstIterator(&data_[size_]);
        }

        size_t GetSize() const noexcept {
            return size_;
        }

        size_t GetCapacity() const noexcept {
            return capacity_;
        }

        bool IsEmpty() const noexcept {
            return !size_;
        }

        Type &operator[](size_t index) noexcept {
            assert(index < size_);
            return data_[index];
        }

        const Type &operator[](size_t index) const noexcept {
            assert(index < size_);
            return data_[index];
        }

        Type &At(size_t index) {
            if (index < size_) {
                return data_[index];
            }

            throw std::out_of_range("Invalid index");
        }

        const Type &At(size_t index) const {
            if (index < size_) {
                return data_[index];
            }

            throw std::invalid_argument("Invalid argument"s);
        }

        void Clear() noexcept {
            size_ = 0;
        }

        void Resize(size_t new_size) {
            if (size_ >= new_size) {
                size_ = new_size;
                return;
            }
            if (capacity_ < new_size) {
                Reserve(new_size * 2);
            }
            std::generate(begin() + size_, begin() + new_size, []() { return Type{}; });
            size_ = new_size;
        }

        SimpleVector& operator=(const SimpleVector& rhs) {
            if (this != &rhs) {
                SimpleVector helper(rhs);
                swap(helper);
            }

            return *this;
        }

        void PushBack(const Type& item) {
            if (capacity_ == size_) {
                Reserve(std::max(capacity_ * 2, size_t(1)));
            }

            data_[size_++] = item;
        }

        void PushBack(Type&& item) {
            if (capacity_ == size_) {
                Reserve(std::max(capacity_ * 2, size_t(1)));
            }

            data_[size_++] = std::forward <Type> (item);
        }

        Iterator Insert(ConstIterator pos, const Type& value) {
            if (begin() <= pos && end() >= pos) {
                auto index = std::distance(cbegin(), pos);

                if (size_ == capacity_) {
                    Reserve(std::max(capacity_ * 2, size_t(1)));
                }

                std::move_backward(begin() + index, end(), end() + 1);

                data_[index] = value;
                ++size_;

                return begin() + index;
            }

            throw std::invalid_argument("Invalid argument"s);
        }

        Iterator Insert(ConstIterator pos, Type&& value) {
            if (begin() <= pos && end() >= pos) {
                auto index = std::distance(cbegin(), pos);

                if (size_ == capacity_) {
                    Reserve(std::max(capacity_ * 2, size_t(1)));
                }

                std::move_backward(begin() + index, end(), end() + 1);

                data_[index] = std::forward<Type>(value);
                ++size_;

                return begin() + index;
            }

            throw std::invalid_argument("Invalid argument"s);
        }

        void PopBack() noexcept {
            assert(size_!=0);
            --size_;
        }

        Iterator Erase(ConstIterator pos) {
            if (begin() <= pos && end() >= pos) {
                auto index = std::distance(cbegin(), pos);

                std::move(&data_[index + 1], end(), const_cast <Iterator> (pos));
                --size_;

                return const_cast <Iterator> (pos);
            }

            throw std::invalid_argument("Invalid argument"s);
        }

        void swap(SimpleVector& other) noexcept {
            data_.swap(other.data_);

            std::swap(size_, other.size_);
            std::swap(capacity_, other.capacity_);
        }

        void Reserve(size_t new_capacity){
            if (new_capacity > capacity_) {
                ArrayPtr <Type> helper(new_capacity);
                std::move(begin(), end(), &helper[0]);

                data_.swap(helper);
                capacity_ = new_capacity;
            }
        }

    private:
        //DATA//
        ArrayPtr <Type> data_;
        size_t size_ = 0;
        size_t capacity_ = 0;
};

template <typename Type>
inline bool operator== (const SimpleVector <Type>& lhs, const SimpleVector <Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!= (const SimpleVector <Type>& lhs, const SimpleVector <Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator< (const SimpleVector <Type>& lhs, const SimpleVector <Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<= (const SimpleVector <Type>& lhs, const SimpleVector <Type>& rhs) {
    return lhs < rhs || lhs == rhs;
}

template <typename Type>
inline bool operator> (const SimpleVector <Type>& lhs, const SimpleVector <Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>= (const SimpleVector <Type>& lhs, const SimpleVector <Type>& rhs) {
    return !(lhs < rhs);
}
