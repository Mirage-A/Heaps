#ifndef BINOMIALHEAP_BINOMIALHEAP_H
#define BINOMIALHEAP_BINOMIALHEAP_H

#include <memory>

using std::weak_ptr;
using std::shared_ptr;
using std::unique_ptr;

template <typename Key>
class BinomialHeap{
private:
    struct Node;
    struct PrePointer {
    private:
        PrePointer() {}
    public:
        weak_ptr<Node> node;
        PrePointer(std::weak_ptr<Node> node) : node(node) {};
    };
    struct Node{
    private:
        Node(){}
    public:
        Key key;
        weak_ptr<Node> right;
        shared_ptr<Node> left;
        shared_ptr<Node> right_child;
        weak_ptr<Node> left_child;
        weak_ptr<Node> par;
        shared_ptr<BinomialHeap::PrePointer> pre_ptr;
        size_t degree;
        bool is_negative_infinity = false;
        Node(Key key){
            this->key = key;
            left = nullptr;
            right = weak_ptr<Node>();
            left_child = weak_ptr<Node>();
            right_child = nullptr;
            par = weak_ptr<Node>();
            is_negative_infinity = false;
            degree = 0;
        }
        void AddLeftChild(shared_ptr<Node> new_child) {
            if (!left_child.expired()) {
                new_child->right = left_child;
                left_child.lock()->left = new_child;
            }
            else {
                new_child->right = weak_ptr<Node>();
                right_child = new_child;
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
    void Swap(shared_ptr<Node> node1, shared_ptr<Node> node2) {
        Key tmp = node1->key;
        node1->key = node2->key;
        node2->key = tmp;
        node1->pre_ptr->node = node2;
        node2->pre_ptr->node = node1;
        shared_ptr<PrePointer> pre_tmp = node1->pre_ptr;
        node1->pre_ptr = node2->pre_ptr;
        node2->pre_ptr = pre_tmp;
    }
public:
    class Pointer{
    private:
        weak_ptr<PrePointer> ptr_;
    public:
        Pointer() {
            ptr_ = weak_ptr<PrePointer>();
        }
        Pointer(shared_ptr<PrePointer> ptr){
            this->ptr_ = ptr;
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
        shared_ptr<Node> ptr = min_->left_child.lock();
        if (ptr != nullptr) {
            while (!ptr->right.expired()) {
                ptr->par = ptr;
                ptr = ptr->right.lock();
            }
            ptr->par = ptr;
        }
        children.right_ = ptr;
        if (min_->left != nullptr) {
            min_->left->right = min_->right;
        }
        if (!min_->right.expired()) {
            min_->right.lock()->left = min_->left;
        }
        else {
            right_ = min_->left;
        }
        min_->left = shared_ptr<Node>(nullptr);
        Merge(children);
        return res;
    }
    void Merge(BinomialHeap &other_heap){
        if (IsEmpty()) {
            right_ = other_heap.right_;
            min_ = other_heap.min_;
            other_heap.Clear();
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
                    if (!ptr1->right.expired()) {
                        ptr1->right.lock()->left = ptr2;
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
                        ptr->left->par = ptr;
                        if (tmp != nullptr) {
                            tmp->right = ptr;
                        }
                        ptr->left = tmp;
                    }
                    else {
                        shared_ptr<Node> new_ptr = ptr->left;
                        if (!ptr->right.expired()) {
                            ptr->right.lock()->left = new_ptr;
                        }
                        else {
                            right_ = new_ptr;
                        }
                        new_ptr->right = ptr->right;
                        new_ptr->AddLeftChild(ptr);
                        ptr->par = new_ptr;
                        ptr = new_ptr;
                    }
                }
                else {
                    ptr = ptr->left;
                }
            }
            other_heap.Clear();
        }
        ValidateMin();
    }
    Pointer Insert(Key key){
        Node* node = new Node(key);
        shared_ptr<Node> node_ptr(node);
        PrePointer* pre = new PrePointer(node_ptr);
        shared_ptr<PrePointer> pre_ptr(pre);
        node_ptr->pre_ptr = pre_ptr;
        pre_ptr->node = node_ptr;
        Pointer res(pre_ptr);
        BinomialHeap<Key> heap(node_ptr);
        Merge(heap);
        return res;
    }
    void Delete(Pointer ptr){
        if (ptr.ptr_.expired() || ptr.ptr_.lock()->node.expired()) {
            throw std::invalid_argument("Element does not exist");
        }

    }
    void Change(Pointer ptr, Key key){
        if (ptr.ptr_.expired() || ptr.ptr_.lock()->node.expired()) {
            throw std::invalid_argument("Element does not exist");
        }
        shared_ptr<Node> node = ptr.ptr_.lock()->node.lock();
        if (key < node->key) {
            if (node->par.expired()) {
                node->key = key;
                if (key < min_->key) {
                    min_ = node;
                }
            }
            else if (node->par.lock()->key <= key) {
                node->key = key;
            }
            else {
                Swap(node, node->par.lock());
                Change(ptr, key);
            }
        }
        else if (node->key < key) {
            shared_ptr<Node> min = node->right_child;
            shared_ptr<Node> cur = node->right_child;
            while (cur != nullptr) {
                if (cur->key < min->key) {
                    min = cur;
                }
                cur = cur->left;
            }
            if (min == nullptr || key <= min->key) {
                node->key = key;
            }
            else {
                Swap(node, min);
                Change(ptr, key);
            }
        }
    }
};

#endif //BINOMIALHEAP_BINOMIALHEAP_H
