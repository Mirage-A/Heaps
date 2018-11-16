#ifndef BINOMIALHEAP_BINOMIALHEAP_H
#define BINOMIALHEAP_BINOMIALHEAP_H

#include <memory>

using std::weak_ptr;
using std::shared_ptr;
using std::unique_ptr;

template <typename Key>
class BinomialHeap{
private:
    struct Node{
    private:
        Node(){}
    public:
        Key key;
        shared_ptr<Node> right;
        shared_ptr<Node> left;
        shared_ptr<Node> left_child;
        weak_ptr<Node> par;
        size_t degree;
        bool is_negative_infinity = false;
        Node(Key key){
            this->key = key;
            left = nullptr;
            right = nullptr;
            left_child = nullptr;
            par = weak_ptr<Node>();
            is_negative_infinity = false;
            degree = 0;
        }
        void AddLeftChild(shared_ptr<Node> new_child) {
            if (left_child != nullptr) {
                new_child->right = left_child;
                left_child->left = new_child;
            }
            else {
                new_child->right = nullptr;
            }
            left_child = new_child;
            new_child->left = nullptr;
            new_child->par = weak_ptr<Node>();
            ++degree;
        }
    };
    shared_ptr<Node> right_;
    shared_ptr<Node> min_;
    BinomialHeap(shared_ptr<Node> node){
        right_ = node;
        min_ = node;
    }
    void ValidateMin(){
        shared_ptr<Node> current = right_;
        min_ = right_;
        while (current != nullptr){
            if (current->key < min_->key){
                min_ = current;
            }
            current = current->left;
        }
    }
public:
    class Pointer{
    private:
        weak_ptr<Node> ptr_;
    public:
        Pointer() {}
        Pointer(shared_ptr<Node> ptr){
            this->ptr_ = ptr;
        }
        weak_ptr<Node> GetPtr() const{
            return ptr_;
        }
        friend class BinomialHeap;
    };
    BinomialHeap(){
        right_ = nullptr;
        min_ = nullptr;
    }
    bool IsEmpty() const{
        return (right_ == nullptr);
    }
    void Clear(){
        right_ = nullptr;
        min_ = nullptr;
    }
    Key GetMin() const{
        if(IsEmpty()){
            throw std::logic_error("Heap is empty");
        }
        return min_->key;
    }
    Key ExtractMin(){
        Key res = GetMin();
        BinomialHeap children;
        shared_ptr<Node> ptr = min_->left_child;
        if (ptr != nullptr) {
            while (ptr->right != nullptr) {
                ptr->par = ptr;
                ptr = ptr->right;
            }
            ptr->par = ptr;
        }
        children.right_ = ptr;
        if (min_->left != nullptr) {
            min_->left->right = min_->right;
        }
        if (min_->right != nullptr) {
            min_->right->left = min_->left;
        }
        else {
            right_ = min_->left;
        }
        Merge(children);
        ValidateMin();
        return res;
    }
    void Merge(BinomialHeap &other_heap){
        if (IsEmpty()) {
            right_ = other_heap.right_;
            min_ = other_heap.min_;
            other_heap.Clear();
            ValidateMin();
        }
        else if (!other_heap.IsEmpty()) {
            shared_ptr<Node> ptr1 = right_;
            shared_ptr<Node> ptr2 = other_heap.right_;
            if (ptr2->degree < ptr1->degree || (ptr2->degree == ptr1->degree && ptr2->key < ptr1->key)){
                right_ = ptr2;
            }
            while (ptr1 != nullptr && ptr2 != nullptr) {
                if (ptr1->degree < ptr2->degree) {
                    if (ptr1->left != nullptr) {
                        ptr1 = ptr1->left;
                    }
                    else {
                        ptr1->left = ptr2;
                        ptr2->right = ptr1;
                        ptr1 = nullptr;
                    }
                }
                else {
                    shared_ptr<Node> tmp = ptr2->left;
                    ptr2->right = ptr1->right;
                    ptr2->left = ptr1;
                    if (ptr1->right != nullptr) {
                        ptr1->right->left = ptr2;
                    }
                    else {
                        right_ = ptr2;
                    }
                    ptr1->right = ptr2;
                    ptr2 = tmp;
                }
            }
            shared_ptr<Node> ptr = right_;
            while (ptr->left != nullptr) {
                if (ptr->degree == ptr->left->degree) {
                    if (ptr->key < ptr->left->key) {
                        shared_ptr<Node> tmp = ptr->left->left;
                        ptr->AddLeftChild(ptr->left);
                        if (tmp != nullptr) {
                            tmp->right = ptr;
                        }
                        ptr->left = tmp;
                    }
                    else {
                        shared_ptr<Node> new_ptr = ptr->left;
                        if (ptr->right != nullptr) {
                            ptr->right->left = new_ptr;
                        }
                        else {
                            right_ = new_ptr;
                        }
                        new_ptr->right = ptr->right;
                        new_ptr->AddLeftChild(ptr);
                        ptr = new_ptr;
                    }
                }
                else {
                    ptr = ptr->left;
                }
            }
            ValidateMin();
            other_heap.Clear();
        }
    }
    Pointer Insert(Key key){
        Node* node_ptr = new Node(key);
        shared_ptr<Node> ptr(node_ptr);
        Pointer res(ptr);
        BinomialHeap<Key> heap(ptr);
        Merge(heap);
        return res;
    }
    void Delete(Pointer ptr){
        if (ptr.GetPtr().expired()) {
            throw std::invalid_argument("Element does not exist");
        }

    }
    void Change(Pointer ptr, Key key){

    }
};

#endif //BINOMIALHEAP_BINOMIALHEAP_H
