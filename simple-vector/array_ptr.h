#pragma once

#include <stdexcept>
#include <algorithm>
#include <cassert>
#include <cstdlib>

template <typename Type>
class ArrayPtr {
    public:
        ArrayPtr() = default;
        ArrayPtr(const ArrayPtr&) = delete;
        ArrayPtr& operator= (const ArrayPtr&) = delete;

        explicit ArrayPtr(size_t array_size) {
            if (array_size != 0) {
                raw_ptr_ = new Type[array_size];
            }
        }

        //FIX_1_1
        ArrayPtr(ArrayPtr&& other) {
			std::swap(raw_ptr_, other.raw_ptr_);
			other.raw_ptr_ = nullptr;
		}

        ~ArrayPtr() {
            delete[] raw_ptr_;
        }

        //FIX_1_2
        ArrayPtr& operator= (ArrayPtr&& other) {
			if (this != &other) {
				std::swap(other.raw_ptr_, raw_ptr_);
			}

			return *this;
		}

        [[nodiscard]] Type* Release() noexcept {
            Type* return_ptr = raw_ptr_;
            raw_ptr_ = nullptr;
            return return_ptr;
        }

        Type& operator[](size_t index) noexcept {
            Type& element_ref = *(raw_ptr_ + index);
            return element_ref;
        }

        const Type& operator[](size_t index) const noexcept {
            const Type& element_ref = *(raw_ptr_ + index);
            return element_ref;
        }

        explicit operator bool() const {
            return Get() != nullptr;
        }

        Type* Get() const noexcept {
            return raw_ptr_;
        }

        //FIX_2
        void swap(ArrayPtr& other) noexcept {
            if (this != &other) {
                std::swap(other.raw_ptr_, raw_ptr_);
            }
        }

    private:
        Type* raw_ptr_ = nullptr;
};
