#ifndef FIBONACCIHEAP_FIBONACCIHEAP_H
#define FIBONACCIHEAP_FIBONACCIHEAP_H

#include <memory>
#include "Vector.h"

template <typename Key>
class FibonacciHeap {
private:
    struct Node {
        Key key;
        size_t degree;
        bool mark;
        std::shared_ptr<Node> child;
        std::weak_ptr<Node> par;
        std::shared_ptr<Node> left;
        std::weak_ptr<Node> right;
        Node(Key key) :
            key(key), degree(0), mark(false) {
            child = std::shared_ptr<Node>();
            par = std::shared_ptr<Node>();
            left = std::shared_ptr<Node>();
            right = std::shared_ptr<Node>();
        }
    };
    std::shared_ptr<Node> min_;
    void UnionLists(std::shared_ptr<Node> first, std::shared_ptr<Node> second) {
        if (second == nullptr) {
            return;
        }
        std::shared_ptr<Node> L = first->left;
        std::shared_ptr<Node> R = second->right.lock();
        second->right = first;
        first->left = second;
        L->right = R;
        R->left = L;
    }
    void Consolidate() {
        Vector<std::shared_ptr<Node>> arr;
        std::shared_ptr<Node> current = min_;
        bool finish = false;
        while (!finish) {
            if (current->key < min_->key) {
                min_ = current;
            }
            if (arr.GetSize() <= current->degree) {
                for (size_t i = arr.GetSize(); i <= current->degree; ++i) {
                    arr.PushBack(std::shared_ptr<Node>(nullptr));
                }
            }
            if (current->left == current) {
                finish = true;
            }
            std::shared_ptr<Node> next = current->left;
            current->left->right = current->right;
            current->right.lock()->left = current->left;
            current->left = current;
            current->right = current;
            if (arr.Get(current->degree) == nullptr) {
                arr.Set(current, (int)current->degree);
            }
            else {
                std::shared_ptr<Node> conflict = arr.Get(current->degree);
                std::shared_ptr<Node> add_to, adding;
                if (conflict->key < current->key) {
                    add_to = conflict;
                    adding = current;
                }
                else {
                    add_to = current;
                    adding = conflict;
                }
                if (add_to->child == nullptr) {
                    add_to->child = adding;
                }
                else {
                    UnionLists(add_to->child, adding);
                }
                adding->par = add_to;
                arr.Set(std::shared_ptr<Node>(nullptr), (int)add_to->degree);
                ++add_to->degree;
                if (finish) {
                    next = add_to;
                    finish = false;
                }
                else {
                    UnionLists(next, add_to);
                }
            }
            current = next;
        }
        for (size_t i = 0; i < arr.GetSize(); ++i) {
            if (arr.Get(i) != nullptr && arr.Get(i) != min_) {
                UnionLists(min_, arr.Get(i));
            }
        }
    }
    void Cut(std::shared_ptr<Node> node) {
        std::shared_ptr<Node> L = node->left;
        std::shared_ptr<Node> R = node->right.lock();
        R->left = L;
        L->right = R;
        node->right = node;
        node->left = node;
        --node->par.lock()->degree;
        if (node->par.lock()->child == node){
            if (node->right.lock() == node){
                node->par.lock()->child = std::shared_ptr<Node>(nullptr);
            }
            else {
                node->par.lock()->child = node->right.lock();
            }
        }
        node->par = std::weak_ptr<Node>();
        UnionLists(min_, node);
    }
    void CascadingCut(std::shared_ptr<Node> node) {
        while (node->mark){
            Cut(node);
            node = node->par.lock();
            node->mark = true;
        }
    }
public:
    class Pointer {
    private:
        std::weak_ptr<Node> ptr_;
        Pointer(std::weak_ptr<Node> ptr) {
            ptr_ = ptr;
        }
    public:
        friend class FibonacciHeap;
        Pointer() {}
    };
    FibonacciHeap() {
        min_ = std::shared_ptr<Node>(nullptr);
    }
    bool IsEmpty() const {
        return min_ == nullptr;
    }
    Pointer Insert(Key key) {
        Node* node_ptr = new Node(key);
        std::shared_ptr<Node> node(node_ptr);
        if (IsEmpty()) {
            min_ = node;
            min_->left = node;
            min_->right = node;
        }
        else {
            std::weak_ptr<Node> prev_right = min_->right;
            min_->right = node;
            node->left = min_;
            node->right = prev_right;
            prev_right.lock()->left = node;
        }
        if (node->key < min_->key) {
            min_ = node;
        }
        return Pointer(node);
    }
    Key GetMin() const {
        if (IsEmpty()) {
            throw std::logic_error("Heap is empty");
        }
        return min_->key;
    }
    Key ExtractMin() {
        if (IsEmpty()) {
            throw std::logic_error("Heap is empty");
        }
        std::shared_ptr<Node> prev_min = min_;
        UnionLists(min_, min_->child);
        std::shared_ptr<Node> L = min_->left;
        std::shared_ptr<Node> R = min_->right.lock();
        L->right = R;
        R->left = L;
        if (prev_min->right.lock() == prev_min) {
            min_ = std::shared_ptr<Node>(nullptr);
            return prev_min->key;
        }
        min_ = min_->right.lock();
        Consolidate();
        return prev_min->key;
    }
    void Merge(FibonacciHeap& other_heap) {
        if (other_heap.IsEmpty()) {
            return;
        }
        if (IsEmpty()){
            min_ = other_heap.min_;
        }
        else {
            UnionLists(min_, other_heap.min_);
            if (other_heap.min_->key < min_->key) {
                min_ = other_heap.min_;
            }
        }
    }
    void Decrease(Pointer ptr, Key key) {
        if (ptr.ptr_.expired()) {
            throw std::invalid_argument("Element does not exist");
        }
        if (ptr.ptr_.lock()->key < key) {
            throw std::invalid_argument("New key should be lesser than the current one");
        }
        if (ptr.ptr_.lock()->par.expired() || ptr.ptr_.lock()->par.lock()->key < key) {
            ptr.ptr_.lock()->key = key;
            if (ptr.ptr_.lock()->key < min_->key) {
                min_ = ptr.ptr_.lock();
            }
        }
        else {
            ptr.ptr_.lock()->key = key;
            Cut(ptr.ptr_.lock());
            CascadingCut(ptr.ptr_.lock()->par.lock());
            if (ptr.ptr_.lock()->key < min_->key) {
                min_ = ptr.ptr_.lock();
            }
        }
    }

};

#endif //FIBONACCIHEAP_FIBONACCIHEAP_H
