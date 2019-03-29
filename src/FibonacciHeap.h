#ifndef FIBONACCIHEAP_FIBONACCIHEAP_H
#define FIBONACCIHEAP_FIBONACCIHEAP_H

#include <memory>
#include "Vector.h"

template <typename Key>
class FibonacciHeap {
private:
    struct Node;
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
        heap_min_ = std::shared_ptr<Node>(nullptr);
    }

    bool IsEmpty() const {
        return heap_min_ == nullptr;
    }

    Pointer Insert(Key key) {
        Node* node_ptr = new Node(key);
        std::shared_ptr<Node> node(node_ptr);
        if (IsEmpty()) {
            heap_min_ = node;
            heap_min_->left = node;
            heap_min_->right = node;
        }
        else {
            std::weak_ptr<Node> prev_right = heap_min_->right;
            heap_min_->right = node;
            node->left = heap_min_;
            node->right = prev_right;
            prev_right.lock()->left = node;
        }
        if (node->key < heap_min_->key) {
            heap_min_ = node;
        }
        return Pointer(node);
    }

    Key GetMin() const {
        if (IsEmpty()) {
            throw std::logic_error("Heap is empty");
        }
        return heap_min_->key;
    }

    Key ExtractMin() {
        if (IsEmpty()) {
            throw std::logic_error("Heap is empty");
        }
        std::shared_ptr<Node> prev_min = heap_min_;
        UniteLists(heap_min_, heap_min_->child);
        std::shared_ptr<Node> L = heap_min_->left;
        std::shared_ptr<Node> R = heap_min_->right.lock();
        L->right = R;
        R->left = L;
        if (prev_min->right.lock() == prev_min) {
            heap_min_ = std::shared_ptr<Node>(nullptr);
            return prev_min->key;
        }
        heap_min_ = heap_min_->right.lock();
        Consolidate();
        return prev_min->key;
    }

    void Merge(FibonacciHeap& other_heap) {
        if (other_heap.IsEmpty()) {
            return;
        }
        if (IsEmpty()){
            heap_min_ = other_heap.heap_min_;
        }
        else {
            UniteLists(heap_min_, other_heap.heap_min_);
            if (other_heap.heap_min_->key < heap_min_->key) {
                heap_min_ = other_heap.heap_min_;
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
        if (ptr.ptr_.lock()->parent.expired() || ptr.ptr_.lock()->parent.lock()->key < key) {
            ptr.ptr_.lock()->key = key;
            if (ptr.ptr_.lock()->key < heap_min_->key) {
                heap_min_ = ptr.ptr_.lock();
            }
        }
        else {
            ptr.ptr_.lock()->key = key;
            Cut(ptr.ptr_.lock());
            CascadingCut(ptr.ptr_.lock()->parent.lock());
            if (ptr.ptr_.lock()->key < heap_min_->key) {
                heap_min_ = ptr.ptr_.lock();
            }
        }
    }

private:
    struct Node {
        Key key;
        size_t degree;
        bool is_marked;
        std::shared_ptr<Node> child;
        std::weak_ptr<Node> parent;
        std::shared_ptr<Node> left;
        std::weak_ptr<Node> right;

        Node(Key key) :
                key(key), degree(0), is_marked(false) {
            child = std::shared_ptr<Node>();
            parent = std::shared_ptr<Node>();
            left = std::shared_ptr<Node>();
            right = std::shared_ptr<Node>();
        }
    };

    void UniteLists(std::shared_ptr<Node> first, std::shared_ptr<Node> second) {
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
        Vector<std::shared_ptr<Node>> roots_of_consolidated_heap;
        std::shared_ptr<Node> current_node = heap_min_;
        bool finish = false;
        while (!finish) {
            if (current_node->key < heap_min_->key) {
                heap_min_ = current_node;
            }
            if (roots_of_consolidated_heap.GetSize() <= current_node->degree) {
                for (size_t i = roots_of_consolidated_heap.GetSize(); i <= current_node->degree; ++i) {
                    roots_of_consolidated_heap.PushBack(std::shared_ptr<Node>(nullptr));
                }
            }
            if (current_node->left == current_node) {
                finish = true;
            }
            std::shared_ptr<Node> next = current_node->left;
            current_node->left->right = current_node->right;
            current_node->right.lock()->left = current_node->left;
            current_node->left = current_node;
            current_node->right = current_node;
            if (roots_of_consolidated_heap.Get(current_node->degree) == nullptr) {
                roots_of_consolidated_heap.Set(current_node, (int)current_node->degree);
            }
            else {
                std::shared_ptr<Node> conflict = roots_of_consolidated_heap.Get(current_node->degree);
                std::shared_ptr<Node> add_to, adding;
                if (conflict->key < current_node->key) {
                    add_to = conflict;
                    adding = current_node;
                }
                else {
                    add_to = current_node;
                    adding = conflict;
                }
                if (add_to->child == nullptr) {
                    add_to->child = adding;
                }
                else {
                    UniteLists(add_to->child, adding);
                }
                adding->parent = add_to;
                roots_of_consolidated_heap.Set(std::shared_ptr<Node>(nullptr), (int)add_to->degree);
                ++add_to->degree;
                if (finish) {
                    next = add_to;
                    finish = false;
                }
                else {
                    UniteLists(next, add_to);
                }
            }
            current_node = next;
        }
        for (size_t i = 0; i < roots_of_consolidated_heap.GetSize(); ++i) {
            if (roots_of_consolidated_heap.Get(i) != nullptr && roots_of_consolidated_heap.Get(i) != heap_min_) {
                UniteLists(heap_min_, roots_of_consolidated_heap.Get(i));
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
        --node->parent.lock()->degree;
        if (node->parent.lock()->child == node){
            if (node->right.lock() == node){
                node->parent.lock()->child = std::shared_ptr<Node>(nullptr);
            }
            else {
                node->parent.lock()->child = node->right.lock();
            }
        }
        node->parent = std::weak_ptr<Node>();
        UniteLists(heap_min_, node);
    }

    void CascadingCut(std::shared_ptr<Node> node) {
        while (node->is_marked){
            Cut(node);
            node = node->parent.lock();
            node->is_marked = true;
        }
    }

    std::shared_ptr<Node> heap_min_;

};

#endif //FIBONACCIHEAP_FIBONACCIHEAP_H
