
#ifndef HEAP_HEAP_H
#define HEAP_HEAP_H

#include <memory>
#include "Vector.h"

using std::weak_ptr;
using std::shared_ptr;
using std::unique_ptr;

template<typename Key>
class Heap {
private:
    struct Node{
    public:
        Key key;
        size_t index;
        Node(Key key, size_t index){
            this->key = key;
            this->index = index;
        }
    };
    Vector<shared_ptr<Node>> nodes_;
    size_t optimized_operations_;
    size_t min_index_; //  Индекс минимального элемента во время оптимизации
    void Repair(){
        if (!IsEmpty()) {
            for (size_t i = GetSize() - 1; i < GetSize(); --i) {
                SiftDown(i);
            }
        }
    }
    void Swap(size_t node1, size_t node2){
        nodes_.Swap(node1, node2);
        nodes_.Get(node1)->index = node1;
        nodes_.Get(node2)->index = node2;
    }
    void SiftUp(size_t node){
        if (node + 1 > GetSize()){
            throw std::out_of_range("Index out of bounds");
        }
        if (nodes_.Get(node)->key < nodes_.Get(Par(node))->key){
            Swap(node, Par(node));
            SiftUp(Par(node));
        }
    }
    void SiftDown(size_t node){
        if (node + 1 > GetSize()){
            throw std::out_of_range("Index out of bounds");
        }
        size_t left_child = Left(node);
        size_t right_child = Right(node);
        size_t smallest = node;
        if (left_child < GetSize() && nodes_.Get(left_child)->key < nodes_.Get(smallest)->key){
            smallest = left_child;
        }
        if (right_child < GetSize() && nodes_.Get(right_child)->key < nodes_.Get(smallest)->key){
            smallest = right_child;
        }
        if (smallest != node){
            Swap(smallest, node);
            SiftDown(smallest);
        }
    }
    size_t Par(size_t node) const{
        if (node == 0) return 0;
        return (node - 1) / 2;
    }
    size_t Left(size_t node) const{
        return node * 2 + 1;
    }
    size_t Right(size_t node) const{
        return node * 2 + 2;
    }
public:
    class Pointer{
    private:
        weak_ptr<Node> ptr_;
    public:
        Pointer(shared_ptr<Node> ptr){
            this->ptr_ = ptr;
        }
        weak_ptr<Node> GetPointer() const{
            return ptr_;
        }
    };
    Heap(){
        optimized_operations_ = 0;
        min_index_ = 0;
    }
    template<class Iterator>
    Heap(Iterator begin, Iterator end){
        size_t new_size = 0;
        for (Iterator i = begin; i != end; ++i){
            ++new_size;
    }
        nodes_.IncreaseCapacityTo(new_size);
        size_t cur_index = 0;
        for (Iterator i = begin; i != end; ++i){
            Node* node_ptr = new Node(*i, cur_index);
            shared_ptr<Node> node(node_ptr);
            nodes_.PushBack(node);
            ++cur_index;
        }
        optimized_operations_ = 0;
        min_index_ = 0;
        Repair();
    }
    bool IsEmpty() const{
        return nodes_.IsEmpty();
    }
    size_t GetSize() const{
        return nodes_.GetSize();
    }
    bool IsValid() const{
        for (size_t i = 0; i < GetSize(); ++i) {
            if (Left(i) < GetSize() && nodes_.Get(i)->key > nodes_.Get(Left(i))->key) {
                return false;
            }
            if (Right(i) < GetSize() && nodes_.Get(i)->key > nodes_.Get(Right(i))->key) {
                return false;
            }
        }
        return true;
    }
    Key GetMin() const{
        if (IsEmpty()) {
            throw std::logic_error("Heap is empty");
        }
        if (optimized_operations_ == 0) {
            return nodes_.Get(0)->key;
        }
        else {
            return nodes_.Get(min_index_)->key;
        }
    }
    Key ExtractMin(){
        if (IsEmpty()) {
            throw std::logic_error("Heap is empty");
        }
        if (optimized_operations_ != 0) {
            --optimized_operations_;
            Repair();
            if (optimized_operations_ == 0) {
                nodes_.ShrinkToFit();
            }
        }
        Key min = nodes_.Get(0)->key;
        Swap(0, GetSize() - 1);
        nodes_.PopBack();
        if (GetSize() > 0) {
            SiftDown(0);
        }
        return min;
    }
    Pointer Insert(Key key){
        Node* node_ptr = new Node(key, GetSize());
        shared_ptr<Node> node(node_ptr);
        nodes_.PushBack(node);
        Pointer ptr(node);
        if (optimized_operations_ == 0) {
            SiftUp(GetSize() - 1);
        }
        else {
            --optimized_operations_;
            if (node->key < nodes_.Get(min_index_)->key) {
                min_index_ = GetSize() - 1;
            }
            if (optimized_operations_ == 0) {
                min_index_ = 0;
                Repair();
                nodes_.ShrinkToFit();
            }
        }
        return ptr;
    }
    void Remove(Pointer ptr){
        if (ptr.GetPointer().expired()) {
            throw std::logic_error("Element already deleted");
        }
        if (optimized_operations_!=0) {
            throw std::logic_error("You can only insert and extract_min from optimized heap");
        }
        size_t index = ptr.GetPointer().lock()->index;
        if (nodes_.Get(index)->key <= nodes_.Get(GetSize() - 1)->key) {
            Swap(index, GetSize() - 1);
            nodes_.PopBack();
            if (GetSize() > 0) {
                SiftDown(index);
            }
        }
        else if (nodes_.Get(index)->key > nodes_.Get(GetSize() - 1)->key) {
            Swap(index, GetSize() - 1);
            nodes_.PopBack();
            if (GetSize() > 0) {
                SiftUp(index);
            }
        }
    }
    void Change(Pointer ptr, Key key){
        if (ptr.GetPointer().expired()) {
            throw std::logic_error("Element already deleted");
        }
        if (optimized_operations_ != 0) {
            throw std::logic_error("You can only insert and extract_min from optimized heap");
        }
        size_t index = ptr.GetPointer().lock()->index;
        if (key < nodes_.Get(index)->key) {
            nodes_.Get(index)->key = key;
            SiftUp(index);
        }
        else if (key > nodes_.Get(index)->key) {
            nodes_.Get(index)->key = key;
            SiftDown(index);
        }
    }
    //Можно отключить свойство кучи на время оптимизации
    //Тогда insert выполняется за O(1), а extract_min - за O(n), после всех операции нужен вызов repair()
    //Если (insertCount + extractCount) * logn >> insertCount + extractCount * n + n,
    //Где n = insertCount,
    //То такая оптимизация позволит получить линейное время работы на все запросы
    //Формально, такое решение удовлетворяет всем условиям в задании
    //Также можно увеличить capacity вектора, чтобы не тратить время на копирование данных
    void Optimize(size_t insertCount, size_t extractCount){
        if (!IsEmpty()) {
            throw std::logic_error("Heap should be empty");
        }
        optimized_operations_ = 0;
        if (insertCount != 0) {
            size_t n = insertCount;
            size_t log_n = (size_t)log(n);
            if ((insertCount + extractCount) * log_n > insertCount + extractCount * n + n) {
                if (nodes_.GetCapacity() < GetSize() + insertCount) {
                    nodes_.IncreaseCapacityTo(n);
                }
                optimized_operations_ = insertCount + extractCount;
            }
        }
    }
    bool IsOptimized() const{
        return (optimized_operations_ != 0);
    }
};


#endif //HEAPS_HEAP_H

