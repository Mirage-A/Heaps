
#ifndef HEAP_VECTOR_H
#define HEAP_VECTOR_H


#include <stdexcept>

template<typename T>
class Vector {
public:
    Vector() {
        size_ = 0;
        capacity_ = 0;
        arr_ = nullptr;
        capacity_locked_ = false;
    }

    ~Vector() {
        Clear();
    }

    void PushBack(T item) {
        if (size_ == capacity_) {
            IncreaseCapacity();
            capacity_locked_ = false;
        }
        arr_[size_] = item;
        ++size_;
    }

    T PopBack() {
        if (size_ < 1) {
            throw std::logic_error("Vector is empty");
        }
        T result = Get(size_ - 1);
        --size_;
        if (size_ <= GetLesserSize(capacity_) && !capacity_locked_) {
            DecreaseCapacityTo(GetLesserCapacity(capacity_));
            capacity_locked_ = false;
        }
        return result;
    }

    T Get(size_t index) const {
        if (index + 1 > size_) {
            throw std::out_of_range("gIndex out of bounds");
        }
        return arr_[index];
    }

    void Set(T value, int index) {
        if (index + 1 > size_) {
            throw std::out_of_range("Index out of bounds");
        }
        arr_[index] = value;
    }

    void Clear() {
        delete [] arr_;
        size_ = 0;
        capacity_ = 0;
        capacity_locked_ = false;
    }

    size_t GetSize() const {
        return size_;
    }

    bool IsEmpty() const {
        return (size_ == 0);
    }

    size_t GetCapacity() const {
        return capacity_;
    }

    void Swap(size_t first_index, size_t second_index) {
        if (first_index + 1 > size_ || second_index + 1 > size_) {
            throw std::out_of_range("Index out of bounds");
        }
        T tmp = Get(first_index);
        Set(Get(second_index), first_index);
        Set(tmp, second_index);
    }

    void IncreaseCapacityTo(size_t new_capacity) {
        if (new_capacity < capacity_) {
            throw std::invalid_argument("New capacity must be higher than the current one");
        }
        T* new_arr = new T[new_capacity];
        for (size_t i = 0; i < capacity_; i++) {
            new_arr[i] = arr_[i];
        }
        size_t new_size = size_;
        Clear();
        arr_ = new_arr;
        size_ = new_size;
        capacity_ = new_capacity;
        capacity_locked_ = true;
    }

    void IncreaseCapacity() {
        IncreaseCapacityTo(GetLargerCapacity(capacity_));
    }

    void IncreaseCapacityFor(size_t additional_capacity) {
        IncreaseCapacityTo(capacity_ + additional_capacity);
    }

    void DecreaseCapacityTo(size_t new_capacity) {
        if (new_capacity < size_) {
            throw std::invalid_argument("New capacity must be higher than the current size");
        }
        if (new_capacity > capacity_) {
            throw std::invalid_argument("New capacity must be lesser than the current one");
        }
        T* new_arr = new T[new_capacity];
        for (size_t i = 0; i < new_capacity; i++) {
            new_arr[i] = arr_[i];
        }
        size_t new_size = size_;
        Clear();
        arr_ = new_arr;
        size_ = new_size;
        capacity_ = new_capacity;
        capacity_locked_ = true;
    }

    void ShrinkToFit() {
        DecreaseCapacityTo(size_);
        capacity_locked_ = false;
    }

private:
    T* arr_;
    size_t size_;
    size_t capacity_;
    bool capacity_locked_;  //true, если capacity была увеличена извне - тогда она не будет уменьшаться при pop_back()

    size_t GetLargerCapacity(size_t cur_capacity) const {
        return (size_t)(cur_capacity * 1.5 + 1);
    }

    // Этот метод определяет процент заполнения массива, после которого массив сжимается
    size_t GetLesserSize(size_t cur_capacity) const {
        return (size_t)(cur_capacity * 0.5 + 1);
    }

    // А этот метод определяет размер массива после сжимания
    // Объединять их в один метод неправильно, т.к. алгоритмы определения этих двух размеров,
    // вообще говоря, могут быть принципиально разными.
    size_t GetLesserCapacity(size_t cur_capacity) const {
        return (size_t)(cur_capacity * 0.66666667 + 1);
    }

};

#endif //HEAP_VECTOR_H
