#ifndef LIST_HPP
#define LIST_HPP

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include "NodeFwdList.hpp"


namespace gavrilova {

  template< class T >
  struct IteratorFwd;
  template< class T >
  struct ConstIteratorFwd;

  template< class T >
  class FwdList
  {
  public:
    using Iterator = gavrilova::IteratorFwd< T >;
    using CIterator = gavrilova::ConstIteratorFwd< T >;

    FwdList();
    FwdList(const FwdList& other);
    FwdList(FwdList&& other) noexcept;
    FwdList(std::initializer_list< T > init);
    ~FwdList();

    FwdList& operator=(const FwdList& other);
    FwdList& operator=(FwdList&& other) noexcept;
    FwdList& operator=(std::initializer_list< T > init);

    bool operator==(const FwdList& other) const noexcept;
    bool operator!=(const FwdList& other) const noexcept;
    bool operator<(const FwdList& other) const noexcept;
    bool operator>(const FwdList& other) const noexcept;
    bool operator<=(const FwdList& other) const noexcept;
    bool operator>=(const FwdList& other) const noexcept;

    Iterator begin() const noexcept;
    Iterator end() const noexcept;

    CIterator cbegin() const noexcept;
    CIterator cend() const noexcept;

    T& front();
    bool empty() const noexcept;
    size_t size() const noexcept;
    void push_front(const T& value);
    void push_front(T&& value);
    void pop_front() noexcept;
    void clear() noexcept;

    void remove(const T& value) noexcept;
    template< class UnaryPredicate >
    void remove_if(UnaryPredicate p) noexcept;

    void splice(CIterator pos, FwdList< T >&) noexcept;
    void splice(CIterator pos, FwdList< T >&&) noexcept;
    void splice(CIterator pos, FwdList< T >&, CIterator it) noexcept;
    void splice(CIterator pos, FwdList< T >&&, CIterator it) noexcept;
    void splice(CIterator pos, FwdList< T >&, CIterator first, CIterator last) noexcept;
    void splice(CIterator pos, FwdList< T >&&, CIterator first, CIterator last) noexcept;

    void reverse();
    void swap(FwdList< T >& other) noexcept;

    void assign(size_t count, const T& value);
    template< class InputIt >
    void assign(InputIt first, InputIt last);
    void assign(std::initializer_list< T > init);

    Iterator insert(CIterator pos, const T& value);
    Iterator insert(CIterator pos, T&& value);
    Iterator insert(CIterator pos, size_t count, const T& value);
    template< class InputIt >
    Iterator insert(CIterator pos, InputIt first, InputIt last);
    Iterator insert(CIterator pos, std::initializer_list< T > init);

    Iterator erase(CIterator pos);
    Iterator erase(CIterator first, CIterator last);

  private:
    NodeFwdList< T >* fake_;
    size_t nodeCount_;
  };

  template< class T >
  FwdList< T >::FwdList() :
    fake_(reinterpret_cast< NodeFwdList< T >* >(new char[sizeof(NodeFwdList< T >)])),
    nodeCount_(0)
  {
    fake_->next = fake_;
  }

  template< class T >
  FwdList< T >::FwdList(const FwdList &other) :
    FwdList()
  {
    assign(other.begin(), other.end());
  }

  template< class T >
  FwdList< T >::FwdList(FwdList &&other) noexcept :
    fake_(other.fake_),
    nodeCount_(other.nodeCount_)
  {
    other.fake_ = nullptr;
    other.nodeCount_ = 0;
  }

  template< class T >
  FwdList< T >::FwdList(std::initializer_list< T > init) :
    FwdList()
  {
    assign(init);
  }

  template< class T >
  FwdList< T >& FwdList< T >::operator=(const FwdList &other)
  {
    if (this != std::addressof(other)) {
      FwdList< T > cpy(other);
      swap(cpy);
    }
    return *this;
  }

  template< class T >
  FwdList< T >& FwdList< T >::operator=(FwdList &&other) noexcept
  {
    if (this == &other) {
      return *this;
    }
    clear();
    delete[] reinterpret_cast< char* >(fake_);
    fake_ = other.fake_;
    nodeCount_ = other.nodeCount_;

    other.fake_ = nullptr;
    other.nodeCount_ = 0;

    return *this;
  }

  template< class T >
  FwdList< T >& FwdList< T >::operator=(std::initializer_list< T > init)
  {
    clear();
    assign(init);
    return *this;
  }

  template< class T >
  FwdList< T >::~FwdList()
  {
    clear();
    delete[] reinterpret_cast< char* >(fake_);
  }

  template< class T >
  typename FwdList< T >::Iterator FwdList< T >::begin() const noexcept
  {
    return Iterator(fake_->next);
  }

  template< class T >
  typename FwdList< T >::Iterator FwdList< T >::end() const noexcept
  {
    return Iterator(fake_);
  }

  template< class T >
  typename FwdList< T >::CIterator FwdList< T >::cbegin() const noexcept
  {
    return CIterator(fake_->next);
  }

  template< class T >
  typename FwdList< T >::CIterator FwdList< T >::cend() const noexcept
  {
    return CIterator(fake_);
  }

  template< class T >
  T & FwdList< T >::front()
  {
    if (empty()) {
      throw std::out_of_range("List is empty");
    }
    return fake_->next->data;
  }

  template< class T >
  bool FwdList< T >::empty() const noexcept
  {
    return nodeCount_ == 0;
  }

  template< class T >
  size_t FwdList< T >::size() const noexcept
  {
    return nodeCount_;
  }

  template< class T >
  void FwdList< T >::push_front(const T &value)
  {
    NodeFwdList<T> *newNode = new NodeFwdList<T>{value, fake_->next};
    fake_->next = newNode;
    ++nodeCount_;
  }

  template< class T >
  void FwdList< T >::push_front(T&& value)
  {
    NodeFwdList<T> *newNode = new NodeFwdList<T>{std::move(value), fake_->next};
    fake_->next = newNode;
    ++nodeCount_;
  }

  template< class T >
  void FwdList< T >::pop_front() noexcept
  {
    NodeFwdList<T> *temp = fake_->next;
    fake_->next = fake_->next->next;
    delete temp;
    --nodeCount_;
  }

  template< class T >
  void FwdList< T >::clear() noexcept
  {
    while (!empty()) {
      pop_front();
    }
  }

  template< class T >
  void FwdList< T >::remove(const T &value) noexcept
  {
    NodeFwdList<T> *current = fake_;
    while (current->next != fake_) {
      if (current->next->data == value) {
        NodeFwdList<T> *temp = current->next;
        current->next = current->next->next;
        delete temp;
        --nodeCount_;
      } else {
        current = current->next;
      }
    }
  }

  template< class T >
  template< class UnaryPredicate >
  void FwdList< T >::remove_if(UnaryPredicate p) noexcept
  {
    NodeFwdList<T> *current = fake_;
    while (current->next != fake_) {
      if (p(current->next->data)) {
        NodeFwdList<T> *temp = current->next;
        current->next = current->next->next;
        delete temp;
        --nodeCount_;
      } else {
        current = current->next;
      }
    }
  }

  template< class T >
  void FwdList< T >::splice(CIterator pos, FwdList< T >& other) noexcept
  {
    if(other.empty()) {
      return;
    }
    NodeFwdList< T >* first_other = other.fake_->next;
    NodeFwdList< T >* last_other = first_other;

    while (last_other->next != other.fake_) {
      last_other = last_other->next;
    }
    other.fake_->next = other.fake_;

    NodeFwdList< T >* node = pos.node_;
    NodeFwdList< T >* node_next = node->next;
    node->next = first_other;
    last_other->next = node_next;

    nodeCount_ += other.nodeCount_;
    other.nodeCount_ = 0;
  }

  template< class T >
  void FwdList< T >::splice(CIterator pos, FwdList< T >&& other) noexcept
  {
    FwdList< T > list_for_splice(std::move(other));
    splice(pos, list_for_splice);
  }

  template< class T >
  void FwdList< T >::splice(CIterator pos, FwdList< T >& other, CIterator it) noexcept
  {
    if (!(*it) || *it == other.fake_) {
      return;
    }
    Iterator it_next = it;
    if (pos == it || pos == ++it_next) {
      return;
    }
    NodeFwdList< T >* node_for_move = it_next.node_;
    if (!node_for_move) {
      return;
    }
    NodeFwdList< T >* node_prev_moved = it.node_;
    NodeFwdList< T >* node_after_moved = it_next.node_;
    node_prev_moved->next = node_after_moved;
    --other.nodeCount_;

    NodeFwdList< T >* node = pos.node_;
    NodeFwdList< T >* node_next = node->next;
    node->next = node_for_move;
    node_for_move->next = node_next;
    ++nodeCount_;
  }

  template< class T >
  void FwdList< T >::splice(CIterator pos, FwdList< T >&& other, CIterator it) noexcept
  {
    FwdList< T > list_for_splice(std::move(other));
    splice(pos, list_for_splice, it);
  }

  template< class T >
  void FwdList< T >::splice(CIterator pos, FwdList< T >& other, CIterator first, CIterator last) noexcept
  {
    if(other.empty()) {
      return;
    }
    NodeFwdList< T >* before_first_other = first.node_;
    ++first;
    NodeFwdList< T >* first_other = first.node_;
    NodeFwdList< T >* last_other = last.node_;

    size_t count_moved = 0;
    for (auto it = first; it != last; ++it) {
      ++count_moved;
    }
    before_first_other->next = last_other->next;
    other.nodeCount_ -= count_moved;

    NodeFwdList< T >* node = pos.node_;
    NodeFwdList< T >* node_next = node->next;
    node->next = first_other;
    last_other->next = node_next;

    nodeCount_ += other.nodeCount_;
  }

  template< class T >
  void FwdList< T >::splice(CIterator pos, FwdList< T >&& other, CIterator first, CIterator last) noexcept
  {
    FwdList< T > list_for_splice(std::move(other));
    splice(pos, list_for_splice, first, last);
  }

  template< class T >
  void FwdList< T >::reverse()
  {
    if (empty()) {
      return;
    }
    NodeFwdList< T >* next = fake_->next;
    NodeFwdList< T >* last = fake_;
    while (next->next != fake_) {
      NodeFwdList< T >* remember = next->next;
      next->next = last;
      last = next;
      next = remember;
    }
    next->next = last;
    fake_->next = next;
  }

  template< class T >
  bool FwdList< T >::operator==(const FwdList<T> &other) const noexcept
  {
    if (nodeCount_ != other.nodeCount_) {
      return false;
    }

    Iterator it1 = begin();
    Iterator it2 = other.begin();
    while (it1 != end()) {
      if (*it1 != *it2)
        return false;
      ++it1;
      ++it2;
    }
    return true;
  }

  template< class T >
  bool FwdList< T >::operator!=(const FwdList<T> &other) const noexcept
  {
    return !(*this == other);
  }

  template< class T >
  bool FwdList< T >::operator<(const FwdList<T> &other) const noexcept
  {
    Iterator it1 = begin();
    Iterator it2 = other.begin();
    while (it1 != end() && it2 != other.end()) {
      if (*it1 < *it2) {
        return true;
      }
      if (*it1 > *it2) {
        return false;
      }
      ++it1;
      ++it2;
    }
    return nodeCount_ < other.nodeCount_;
  }

  template< class T >
  bool FwdList< T >::operator>(const FwdList<T> &other) const noexcept
  {
    return other < *this;
  }

  template< class T >
  bool FwdList< T >::operator>=(const FwdList<T> &other) const noexcept
  {
    return !(other < *this);
  }

  template< class T >
  bool FwdList< T >::operator<=(const FwdList<T> &other) const noexcept
  {
    return !(other > *this);
  }

  template< class T>
  void FwdList< T >::swap(FwdList< T >& other ) noexcept
  {
    std::swap(fake_, other.fake_);
    std::swap(nodeCount_, other.nodeCount_);
  }

  template< class T>
  void FwdList< T >::assign(size_t count, const T& value)
  {
    FwdList< T > temporary;
    for (size_t i = 0; i < count; ++i) {
      temporary.push_front(value);
    }
    swap(temporary);
  }

  template< class T >
  template< class InputIt >
  void FwdList< T >::assign(InputIt first, InputIt last)
  {
    FwdList< T > temporary;
    NodeFwdList< T >* cur_node = temporary.begin().node_;
    for (auto it = first; it != last; ++it) {
      cur_node->next = it.node_;
      cur_node = cur_node->next;
      ++temporary.nodeCount_;
    }
    cur_node->next = temporary.fake_;
    swap(temporary);
  }

  template< class T>
  void FwdList< T >::assign(std::initializer_list< T > init)
  {
    FwdList< T > temporary;
    for (const T& value : init) {
      temporary.push_front(value);
    }
    temporary.reverse();
    swap(temporary);
  }

  template< class T>
  typename FwdList< T >::Iterator FwdList< T >::insert(CIterator pos, const T& value)
  {
    NodeFwdList< T >* node = pos.node_;
    if (node == fake_) {
      push_front(value);
      return begin();
    }
    NodeFwdList< T >* node_next = node->next;
    node->next = new NodeFwdList< T >{value, node_next};
    ++nodeCount_;
    return Iterator(node->next);
  }

  template< class T>
  typename FwdList< T >::Iterator FwdList< T >::insert(CIterator pos, T&& value)
  {
    T value_for_move {std::move(value)};
    return insert (pos, value_for_move);
  }

  template< class T>
  typename FwdList< T >::Iterator FwdList< T >::insert(CIterator pos, size_t count, const T& value)
  {
    if (count == 0) {
      return Iterator(pos.node_);
    }

    NodeFwdList< T >* new_head = nullptr;
    new_head = new NodeFwdList< T >{value, nullptr};
    NodeFwdList< T >* cur_node = new_head;
    NodeFwdList< T >* last_one = cur_node;
    for (size_t i = 1; i < count; ++i) {
      try {
        cur_node->next = new NodeFwdList< T >{value, nullptr};
        last_one = cur_node;
        cur_node = cur_node->next;
      } catch(const std::bad_alloc&) {
        cur_node->next = nullptr;
        while (new_head) {
          NodeFwdList< T >* next = new_head->next;
          delete new_head;
          new_head = next;
        }
        throw;
      }
    }

    NodeFwdList< T >* node = pos.node_;
    if (node == fake_) {
      fake_->next = new_head;
      last_one->next = fake_;
    } else {
      last_one->next = node->next;
      node->next = new_head;
    }
    nodeCount_ += count;
    return Iterator(last_one);
  }

  template< class T >
  template< class InputIt >
  typename FwdList< T >::Iterator FwdList< T >::insert(CIterator pos, InputIt first, InputIt last)
  {
    if (first == last) {
      return Iterator(pos.node_);
    }
    size_t count_inserted = 0;
    auto last_inserted = pos;
    try {
      for (auto it = first; it != last; ++it) {
        last_inserted = insert(pos, *it);
        ++count_inserted;
      }
    } catch (const std::bad_alloc&) {
      while (--count_inserted > 0) {
        erase(pos);
      }
      throw;
    }
    return last_inserted;
  }

  template< class T>
  typename FwdList< T >::Iterator FwdList< T >::insert(CIterator pos, std::initializer_list< T > init)
  {
    if (init.size() == 0) {
      return Iterator(pos.node_);
    }

    NodeFwdList< T >* new_head = nullptr;
    new_head = new NodeFwdList< T >{*init.begin(), nullptr};
    NodeFwdList< T >* cur_node = new_head;
    NodeFwdList< T >* last_one = nullptr;
    for (auto it = std::next(init.begin()); it != init.end(); ++it) {
      try {
        cur_node->next = new NodeFwdList< T >{*it, nullptr};
        last_one = cur_node;
        cur_node = cur_node->next;
      } catch(const std::bad_alloc&) {
        cur_node->next = nullptr;
        while (new_head) {
          NodeFwdList< T >* next = new_head->next;
          delete new_head;
          new_head = next;
        }
        throw;
      }
    }

    NodeFwdList< T >* node = pos.node_;
    if (node == fake_) {
      fake_->next = new_head;
      last_one->next = fake_;
    } else {
      last_one->next = node->next;
      node->next = new_head;
    }
    nodeCount_ += init.size();
    return Iterator(last_one);
  }

  template <class T>
  typename FwdList< T >::Iterator FwdList< T >::erase(CIterator pos)
  {
    if (empty()) {
      return pos;
    }
    NodeFwdList<T>* prevNode = pos.node_;
    NodeFwdList<T>* nodeToDelete = prevNode->next;
    if (nodeToDelete == fake_) {
      return (Iterator{end()});
    }
    prevNode->next = nodeToDelete->next;
    delete nodeToDelete;
    --nodeCount_;
    return Iterator{prevNode->next};
  }

  template <class T>
  typename FwdList< T >::Iterator FwdList< T >::erase(CIterator first, CIterator last)
  {
    if (first == last) {
      return Iterator(last.node_);
    }

    NodeFwdList<T>* prev = first.node_;
    NodeFwdList<T>* current = first.node_->next;
    while (current != last.node_) {
      NodeFwdList<T>* nodeToDelete = current;
      current = current->next;
      delete nodeToDelete;
      --nodeCount_;
    }

    prev->next = last.node_;
    return Iterator(last.node_);
  }

}

#endif
