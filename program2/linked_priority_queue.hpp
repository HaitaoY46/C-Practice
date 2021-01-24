// Submitter: haitaoy(Ye, Haitao)
// Partner  : burnscr(Burns, Connor)
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming
#ifndef LINKED_PRIORITY_QUEUE_HPP_
#define LINKED_PRIORITY_QUEUE_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "array_stack.hpp"      //See operator <<


namespace ics {


#ifndef undefinedgtdefined
#define undefinedgtdefined
template<class T>
bool undefinedgt (const T& a, const T& b) {return false;}
#endif /* undefinedgtdefined */

//Instantiate the templated class supplying tgt(a,b): true, iff a has higher priority than b.
//If tgt is defaulted to undefinedgt in the template, then a constructor must supply cgt.
//If both tgt and cgt are supplied, then they must be the same (by ==) function.
//If neither is supplied, or both are supplied but different, TemplateFunctionError is raised.
//The (unique) non-undefinedgt value supplied by tgt/cgt is stored in the instance variable gt.
template<class T, bool (*tgt)(const T& a, const T& b) = undefinedgt<T>> class LinkedPriorityQueue {
  public:
    //Destructor/Constructors
    ~LinkedPriorityQueue();

    LinkedPriorityQueue          (bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);
    LinkedPriorityQueue          (const LinkedPriorityQueue<T,tgt>& to_copy, bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);
    explicit LinkedPriorityQueue (const std::initializer_list<T>& il, bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    explicit LinkedPriorityQueue (const Iterable& i, bool (*cgt)(const T& a, const T& b) = undefinedgt<T>);


    //Queries
    bool empty      () const;
    int  size       () const;
    T&   peek       () const;
    std::string str () const; //supplies useful debugging information; contrast to operator <<


    //Commands
    int  enqueue (const T& element);
    T    dequeue ();
    void clear   ();

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    int enqueue_all (const Iterable& i);


    //Operators
    LinkedPriorityQueue<T,tgt>& operator = (const LinkedPriorityQueue<T,tgt>& rhs);
    bool operator == (const LinkedPriorityQueue<T,tgt>& rhs) const;
    bool operator != (const LinkedPriorityQueue<T,tgt>& rhs) const;

    template<class T2, bool (*gt2)(const T2& a, const T2& b)>
    friend std::ostream& operator << (std::ostream& outs, const LinkedPriorityQueue<T2,gt2>& pq);



  private:
    class LN;

  public:
    class Iterator {
      public:
        //Private constructor called in begin/end, which are friends of LinkedPriorityQueue<T,tgt>
        ~Iterator();
        T           erase();
        std::string str  () const;
        LinkedPriorityQueue<T,tgt>::Iterator& operator ++ ();
        LinkedPriorityQueue<T,tgt>::Iterator  operator ++ (int);
        bool operator == (const LinkedPriorityQueue<T,tgt>::Iterator& rhs) const;
        bool operator != (const LinkedPriorityQueue<T,tgt>::Iterator& rhs) const;
        T& operator *  () const;
        T* operator -> () const;
        friend std::ostream& operator << (std::ostream& outs, const LinkedPriorityQueue<T,tgt>::Iterator& i) {
          outs << i.str(); //Use the same meaning as the debugging .str() method
          return outs;
        }
        friend Iterator LinkedPriorityQueue<T,tgt>::begin () const;
        friend Iterator LinkedPriorityQueue<T,tgt>::end   () const;

      private:
        //If can_erase is false, current indexes the "next" value (must ++ to reach it)
        LN*             prev;            //initialize this instance to header node
        LN*             current;         //current == prev->next
        LinkedPriorityQueue<T,tgt>* ref_pq;
        int             expected_mod_count;
        bool            can_erase = true;

        //Called in friends begin/end
        Iterator(LinkedPriorityQueue<T,tgt>* iterate_over, LN* initial);
    };


    Iterator begin () const;
    Iterator end   () const;


  private:
    class LN {
      public:
        LN ()                      {}
        LN (const LN& ln)          : value(ln.value), next(ln.next){}
        LN (T v,  LN* n = nullptr) : value(v), next(n){}

        T   value;
        LN* next = nullptr;
    };


    bool (*gt) (const T& a, const T& b); // The gt used by enqueue (from template or constructor)
    LN* front     =  new LN();
    int used      =  0;                  //Cache the count of nodes in the linked list
    int mod_count =  0;                  //Allows the sensing of concurrent modification

    //Helper methods
    void delete_list(LN*& front);        //Deallocate all LNs, and set front's argument to nullptr;
};





////////////////////////////////////////////////////////////////////////////////
//
//LinkedPriorityQueue class and related definitions

//Destructor/Constructors

template<class T, bool (*tgt)(const T& a, const T& b)>
LinkedPriorityQueue<T,tgt>::~LinkedPriorityQueue()
{
    delete_list(front);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
LinkedPriorityQueue<T,tgt>::LinkedPriorityQueue(bool (*cgt)(const T& a, const T& b))
:gt(tgt != undefinedgt<T> ? tgt : cgt)
{
    if (gt == undefinedgt<T>)
        throw TemplateFunctionError("LinkedPriorityQueue::default constructor: neither specified");
    if (tgt != undefinedgt<T> && cgt != undefinedgt<T> && tgt != cgt)
        throw TemplateFunctionError("LinkedPriorityQueue::default constructor: both specified and different");
}


template<class T, bool (*tgt)(const T& a, const T& b)>
LinkedPriorityQueue<T,tgt>::LinkedPriorityQueue(const LinkedPriorityQueue<T,tgt>& to_copy, bool (*cgt)(const T& a, const T& b))
:gt(tgt != undefinedgt<T> ? tgt : cgt)
{
    if (gt == undefinedgt<T>)
    {
        gt = to_copy.gt;
    }
    if (tgt != undefinedgt<T> && cgt != undefinedgt<T> && tgt != cgt)
        throw TemplateFunctionError("LinkedPriorityQueue::copy constructor: both specified and different");
    if (gt == to_copy.gt){
        LN* f = new LN();
        front = f;
        for(LN* n = to_copy.front->next; n != nullptr; n = n->next){
            LN* nw = new LN();
            nw->value = n->value;
            f->next = nw;
            f = f->next;
        }
        used = to_copy.used;

    }else{
        enqueue_all(to_copy);
    }
}


template<class T, bool (*tgt)(const T& a, const T& b)>
LinkedPriorityQueue<T,tgt>::LinkedPriorityQueue(const std::initializer_list<T>& il, bool (*cgt)(const T& a, const T& b))
:gt(tgt != undefinedgt<T> ? tgt : cgt)
{
    if (gt == undefinedgt<T>)
        throw TemplateFunctionError("LinkedPriorityQueue::initializer list constructor: neither specified");
    if (tgt != undefinedgt<T> && cgt != undefinedgt<T> && tgt != cgt)
        throw TemplateFunctionError("LinkedPriorityQueue::initializer list constructor: both specified and different");

    for (auto& elem: il)
    {
        enqueue(elem);
    }
}


template<class T, bool (*tgt)(const T& a, const T& b)>
template<class Iterable>
LinkedPriorityQueue<T,tgt>::LinkedPriorityQueue(const Iterable& i, bool (*cgt)(const T& a, const T& b))
:gt(tgt != undefinedgt<T> ? tgt : cgt)
{
    if (gt == undefinedgt<T>)
        throw TemplateFunctionError("LinkedPriorityQueue::iterable constructor: neither specified");
    if (tgt != undefinedgt<T> && cgt != undefinedgt<T> && tgt != cgt)
        throw TemplateFunctionError("LinkedPriorityQueue::iterable constructor: both specified and different");

    for (auto& elem: i)
    {
        enqueue(elem);
    }
}


////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class T, bool (*tgt)(const T& a, const T& b)>
bool LinkedPriorityQueue<T,tgt>::empty() const
{
    return used == 0;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
int LinkedPriorityQueue<T,tgt>::size() const
{
    return this->used;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T& LinkedPriorityQueue<T,tgt>::peek () const
{
    if (empty())
        throw EmptyError("LinkedPriorityQueue::peek");

    return front->next->value;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
std::string LinkedPriorityQueue<T,tgt>::str() const
{
    std::ostringstream answer;
    LN* p = front->next;
    answer << "LinkedPriorityQueue[HEADER";


    for (; p->next != nullptr; p = p->next)
    {
        answer << p->value << "->";
    }

    answer << p->value << "]" ;
    answer << "(used=" << used << ",front=" << front << "mod_count=" << mod_count << ")";
    return answer.str();
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands

template<class T, bool (*tgt)(const T& a, const T& b)>
int LinkedPriorityQueue<T,tgt>::enqueue(const T& element)
{
    LN* n = new LN();
    front->value = element;
    LN* i = front->next;
    LN* p = front;

    while(i != nullptr && !gt(p->value, i->value))
    {
            T v = p->value;
            p->value = i->value;
            i->value = v;

            i=i->next;
            p=p->next;
    }

    n->next = front;
    front = n;
    used++;
    mod_count++;
    return 1;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
T LinkedPriorityQueue<T,tgt>::dequeue()
{
    if (this->empty())
        throw EmptyError("LinkedPriorityQueue::dequeue");

    LN* p = front->next;
    T to_return  = p->value;
    front->next = p->next;
    delete(p);
    used--;
    ++mod_count;
    return to_return;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
void LinkedPriorityQueue<T,tgt>::clear()
{
    delete_list(front->next);
    front->next = nullptr;
    used = 0;
    ++mod_count;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
template <class Iterable>
int LinkedPriorityQueue<T,tgt>::enqueue_all (const Iterable& i)
{
    int count = 0;
    for (const T& v : i) {
        count += enqueue(v);
    }
    return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class T, bool (*tgt)(const T& a, const T& b)>
LinkedPriorityQueue<T,tgt>& LinkedPriorityQueue<T,tgt>::operator = (const LinkedPriorityQueue<T,tgt>& rhs)
{
    if(rhs.gt != gt) {
        gt = rhs.gt;
    }

    LN* p1 = front->next;
    LN* p2 = rhs.front->next;
    if(used == rhs.used)
    {
        while(p1!= nullptr)
        {
            p1->value = p2->value;
            p1 = p1->next;
            p2 = p2->next;
        }
    }

    else if (rhs.size() > this->size())
    {
        for(int i = 0; i < used; ++i){
            p1->value = p2->value;
            p1 = p1->next;
            p2 = p2->next;
        }
        while(p2 != nullptr)
        {
            enqueue(p2->value);
            p2 = p2->next;
        }
    }


    else if (rhs.size() < this->size())
    {
        LN* p1prev;
        while (p2 != nullptr)
        {
            p1->value = p2->value;
            p1prev = p1;
            p1 = p1->next;
            p2 = p2->next;
        }
        delete_list(p1);
        p1prev->next = nullptr;
    }

    this->used = rhs.used;
    ++mod_count;
    return *this;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool LinkedPriorityQueue<T,tgt>::operator == (const LinkedPriorityQueue<T,tgt>& rhs) const {
    int used = this->size();
    if (used != rhs.size())
        return false;

    if (gt != rhs.gt)
        return true;

    Iterator itor = this->begin();
    Iterator ritor = rhs.begin();
    while (itor != this->end())
    {
        if(*itor != *ritor)
            return false;
        itor++;
        ritor++;
    }
    return true;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool LinkedPriorityQueue<T,tgt>::operator != (const LinkedPriorityQueue<T,tgt>& rhs) const
{
    if (gt != rhs.gt)
        return true;

    int used = this->size();
    if (used != rhs.size())
        return true;

    Iterator itor = this->begin();
    Iterator ritor = rhs.begin();
    while (itor != this->end())
    {
        if(*itor != *ritor)
            return true;
        itor++;
        ritor++;
    }
    return false;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
std::ostream& operator << (std::ostream& outs, const LinkedPriorityQueue<T,tgt>& pq)
{
    outs << "priority_queue[";

    if (!pq.empty()) {
        int count = pq.used;
        for (; count > 0; --count)
        {
            typename LinkedPriorityQueue<T, tgt>::LN* p = pq.front;
            for (int i = 0; i < count  ;++i)
            { p = p->next;}
            if(count == pq.used){
                outs << p->value;
            }else {
                outs << "," << p->value;
            }
        }

    }

    outs << "]:highest";
    return outs;
}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors


template<class T, bool (*tgt)(const T& a, const T& b)>
auto LinkedPriorityQueue<T,tgt>::begin () const -> LinkedPriorityQueue<T,tgt>::Iterator
{
    return Iterator(const_cast<LinkedPriorityQueue<T, tgt>*>(this), front);
}


template<class T, bool (*tgt)(const T& a, const T& b)>
auto LinkedPriorityQueue<T,tgt>::end () const -> LinkedPriorityQueue<T,tgt>::Iterator
{
    LN* n = new LN();
    return Iterator(const_cast<LinkedPriorityQueue<T, tgt>*>(this), n);
}


////////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class T, bool (*tgt)(const T& a, const T& b)>
void LinkedPriorityQueue<T,tgt>::delete_list(LN*& front)
{
    while(front != nullptr)
    {
        LN* d = front;
        front = front->next;
        delete(d);
    }
}





////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class T, bool (*tgt)(const T& a, const T& b)>
LinkedPriorityQueue<T,tgt>::Iterator::Iterator(LinkedPriorityQueue<T,tgt>* iterate_over, LN* initial)
:ref_pq(iterate_over), prev(initial), current(prev->next), expected_mod_count(ref_pq->mod_count)
{
}


template<class T, bool (*tgt)(const T& a, const T& b)>
LinkedPriorityQueue<T,tgt>::Iterator::~Iterator()
{}


template<class T, bool (*tgt)(const T& a, const T& b)>
T LinkedPriorityQueue<T,tgt>::Iterator::erase()
{
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::erase");
    if (!can_erase)
        throw CannotEraseError("LinkedPriorityQueue::Iterator::erase Iterator cursor already erased");
    if (current == nullptr)
    {
        throw CannotEraseError("LinkedPriorityQueue::Iterator::erase Iterator cursor already erased");
    }

    can_erase = false;
    prev->next = current->next;
    T to_return = current->value;
    LN* p = current;
    current = prev->next;
    delete(p);

    --ref_pq->used;
    ++ref_pq->mod_count;
    expected_mod_count = ref_pq->mod_count;
    return to_return;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
std::string LinkedPriorityQueue<T,tgt>::Iterator::str() const {
    std::ostringstream answer;
    answer << ref_pq->str() << "/current=" << current << "/expected_mod_count=" << expected_mod_count << "/can_erase=" << can_erase;
    return answer.str();
}


template<class T, bool (*tgt)(const T& a, const T& b)>
auto LinkedPriorityQueue<T,tgt>::Iterator::operator ++ () -> LinkedPriorityQueue<T,tgt>::Iterator&
{
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator ++");

    if(can_erase && current != nullptr)
    {
        if (current->next == nullptr)
        {
            prev = current;
            current = nullptr;
            return *this;
        }
            prev = prev->next;
            current = current ->next;
    }
    else
    {
        can_erase = true;
    }
    return *this;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
auto LinkedPriorityQueue<T,tgt>::Iterator::operator ++ (int) -> LinkedPriorityQueue<T,tgt>::Iterator
{
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator ++");

    Iterator to_return(*this);
    if(can_erase && current != nullptr)
    {
        if (current->next == nullptr)
        {
            prev = current;
            current = nullptr;
            return to_return;
        }
        prev = prev->next;
        current = current ->next;
    }
    else
    {
        can_erase = true;
    }
    return to_return;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool LinkedPriorityQueue<T,tgt>::Iterator::operator == (const LinkedPriorityQueue<T,tgt>::Iterator& rhs) const
{
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (rhsASI == nullptr)
        throw IteratorTypeError("LinkedPriorityQueue::Iterator::operator ==");
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator ==");
    if (ref_pq != rhsASI->ref_pq)
        throw ComparingDifferentIteratorsError("LinkedPriorityQueue::Iterator::operator ==");

    return current == rhsASI->current;
}


template<class T, bool (*tgt)(const T& a, const T& b)>
bool LinkedPriorityQueue<T,tgt>::Iterator::operator != (const LinkedPriorityQueue<T,tgt>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (rhsASI == nullptr)
        throw IteratorTypeError("LinkedPriorityQueue::Iterator::operator !=");
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator !=");
    if (ref_pq != rhsASI->ref_pq)
        throw ComparingDifferentIteratorsError("LinkedPriorityQueue::Iterator::operator !=");

    return current != rhsASI->current;
}

template<class T, bool (*tgt)(const T& a, const T& b)>
T& LinkedPriorityQueue<T,tgt>::Iterator::operator *() const
{
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator *");
    if (!can_erase || current == nullptr) {
        std::ostringstream where;
        where << current << " when size = " << ref_pq->size();
        throw IteratorPositionIllegal("LinkedPriorityQueue::Iterator::operator * Iterator illegal: "+where.str());
    }

    return current->value;

}

template<class T, bool (*tgt)(const T& a, const T& b)>
T* LinkedPriorityQueue<T,tgt>::Iterator::operator ->() const
{
    if (expected_mod_count != ref_pq->mod_count)
        throw ConcurrentModificationError("LinkedPriorityQueue::Iterator::operator ->");
    if (!can_erase || current == nullptr) {
        std::ostringstream where;
        where << current << " when size = " << ref_pq->size();
        throw IteratorPositionIllegal("LinkedPriorityQueue::Iterator::operator -> Iterator illegal: "+where.str());
    }

    return &current->value;
}


}

#endif /* LINKED_PRIORITY_QUEUE_HPP_ */
