// Submitter: haitaoy(Ye, Haitao)
// Partner  : burnscr(Burns, Connor)
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming
#ifndef HASH_SET_HPP_
#define HASH_SET_HPP_

#include <string>
#include <iostream>
#include <sstream>
#include <initializer_list>
#include "ics_exceptions.hpp"
#include "pair.hpp"


namespace ics {


#ifndef undefinedhashdefined
#define undefinedhashdefined
template<class T>
int undefinedhash (const T& a) {return 0;}
#endif /* undefinedhashdefined */

//Instantiate the templated class supplying thash(a): produces a hash value for a.
//If thash is defaulted to undefinedhash in the template, then a constructor must supply chash.
//If both thash and chash are supplied, then they must be the same (by ==) function.
//If neither is supplied, or both are supplied but different, TemplateFunctionError is raised.
//The (unique) non-undefinedhash value supplied by thash/chash is stored in the instance variable hash.
template<class T, int (*thash)(const T& a) = undefinedhash<T>> class HashSet {
  public:
    typedef int (*hashfunc) (const T& a);

    //Destructor/Constructors
    ~HashSet ();

    HashSet (double the_load_threshold = 1.0, int (*chash)(const T& a) = undefinedhash<T>);
    explicit HashSet (int initial_bins, double the_load_threshold = 1.0, int (*chash)(const T& k) = undefinedhash<T>);
    HashSet (const HashSet<T,thash>& to_copy, double the_load_threshold = 1.0, int (*chash)(const T& a) = undefinedhash<T>);
    explicit HashSet (const std::initializer_list<T>& il, double the_load_threshold = 1.0, int (*chash)(const T& a) = undefinedhash<T>);

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    explicit HashSet (const Iterable& i, double the_load_threshold = 1.0, int (*chash)(const T& a) = undefinedhash<T>);


    //Queries
    bool empty      () const;
    int  size       () const;
    bool contains   (const T& element) const;
    std::string str () const; //supplies useful debugging information; contrast to operator <<

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    bool contains_all (const Iterable& i) const;


    //Commands
    int  insert (const T& element);
    int  erase  (const T& element);
    void clear  ();

    //Iterable class must support "for" loop: .begin()/.end() and prefix ++ on returned result

    template <class Iterable>
    int insert_all(const Iterable& i);

    template <class Iterable>
    int erase_all(const Iterable& i);

    template<class Iterable>
    int retain_all(const Iterable& i);


    //Operators
    HashSet<T,thash>& operator = (const HashSet<T,thash>& rhs);
    bool operator == (const HashSet<T,thash>& rhs) const;
    bool operator != (const HashSet<T,thash>& rhs) const;
    bool operator <= (const HashSet<T,thash>& rhs) const;
    bool operator <  (const HashSet<T,thash>& rhs) const;
    bool operator >= (const HashSet<T,thash>& rhs) const;
    bool operator >  (const HashSet<T,thash>& rhs) const;

    template<class T2, int (*hash2)(const T2& a)>
    friend std::ostream& operator << (std::ostream& outs, const HashSet<T2,hash2>& s);



  private:
    class LN;

  public:
    class Iterator {
      public:
        typedef pair<int,LN*> Cursor;

        //Private constructor called in begin/end, which are friends of HashSet<T,thash>
        ~Iterator();
        T           erase();
        std::string str  () const;
        HashSet<T,thash>::Iterator& operator ++ ();
        HashSet<T,thash>::Iterator  operator ++ (int);
        bool operator == (const HashSet<T,thash>::Iterator& rhs) const;
        bool operator != (const HashSet<T,thash>::Iterator& rhs) const;
        T& operator *  () const;
        T* operator -> () const;
        friend std::ostream& operator << (std::ostream& outs, const HashSet<T,thash>::Iterator& i) {
          outs << i.str(); //Use the same meaning as the debugging .str() method
          return outs;
        }
        friend Iterator HashSet<T,thash>::begin () const;
        friend Iterator HashSet<T,thash>::end   () const;

      private:
        //If can_erase is false, current indexes the "next" value (must ++ to reach it)
        Cursor              current; //Pair:Bin Index/LN*; stops if LN* == nullptr
        HashSet<T,thash>*   ref_set;
        int                 expected_mod_count;
        bool                can_erase = true;

        //Helper methods
        void advance_cursors();

        //Called in friends begin/end
        Iterator(HashSet<T,thash>* iterate_over, bool from_begin);
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
        LN* next   = nullptr;
    };

public:
  int (*hash)(const T& k);   //Hashing function used (from template or constructor)
private:
  LN** set      = nullptr;   //Pointer to array of pointers: each bin stores a list with a trailer node
  double load_threshold;     //used/bins <= load_threshold
  int bins      = 1;         //# bins currently in array (start it >= 1 so no divide by 0 in hash_compress)
  int used      = 0;         //Cache for number of elements in the hash table
  int mod_count = 0;         //For sensing concurrent modification


  //Helper methods
  int   hash_compress        (const T& element)          const;  //hash function ranged to [0,bins-1]
  LN*   find_element         (const T& element)          const;  //Returns reference to element's node or nullptr
  LN*   copy_list            (LN*   l)                   const;  //Copy the elements in a bin (order irrelevant)
  LN**  copy_hash_table      (LN** ht, int bins)         const;  //Copy the bins/keys/values in ht (order in bins irrelevant)

  void  ensure_load_threshold(int new_used);                     //Reallocate if load_threshold > load_threshold
  void  delete_hash_table    (LN**& ht, int bins);               //Deallocate all LN in ht (and the ht itself; ht == nullptr)
};





//HashSet class and related definitions

////////////////////////////////////////////////////////////////////////////////
//
//Destructor/Constructors

template<class T, int (*thash)(const T& a)>
HashSet<T,thash>::~HashSet()
{
    delete_hash_table(set, bins);
}


template<class T, int (*thash)(const T& a)>
HashSet<T,thash>::HashSet(double the_load_threshold, int (*chash)(const T& element))
: hash(thash != (hashfunc)undefinedhash ? thash : chash), load_threshold(the_load_threshold)
{
    if (hash == (hashfunc)undefinedhash)
        throw TemplateFunctionError("hash_set::default constructor: neither specified");
    if (thash != (hashfunc)undefinedhash && chash != (hashfunc)undefinedhash && thash != chash)
        throw TemplateFunctionError("hash_set::default constructor: both specified and different");

    set = new LN*[bins];
    set[0] = new LN();
}


template<class T, int (*thash)(const T& a)>
HashSet<T,thash>::HashSet(int initial_bins, double the_load_threshold, int (*chash)(const T& element))
: hash(thash != (hashfunc)undefinedhash ? thash : chash), bins(initial_bins), load_threshold(the_load_threshold)
{
    if (hash == (hashfunc)undefinedhash)
        throw TemplateFunctionError("hash_set::initial bin constructor: neither specified");
    if (thash != (hashfunc)undefinedhash && chash != (hashfunc)undefinedhash && thash != chash)
        throw TemplateFunctionError("hash_set::initial bin constructor: both specified and different");

    set = new LN*[bins];
    for (int i = 0; i < bins; i++)
        set[i] = new LN();
}


template<class T, int (*thash)(const T& a)>
HashSet<T,thash>::HashSet(const HashSet<T,thash>& to_copy, double the_load_threshold, int (*chash)(const T& element))
: hash(thash != (hashfunc)undefinedhash ? thash : chash),bins(to_copy.bins), load_threshold(the_load_threshold), used(to_copy.used)
{
    if (hash == (hashfunc)undefinedhash)
        hash = to_copy.hash;
    if (thash != (hashfunc)undefinedhash && chash != (hashfunc)undefinedhash && thash != chash)
        throw TemplateFunctionError("hash_set::copy constructor: both specified and different");

    set = new LN*[bins];
    for (int i = 0; i < bins; i++)
        set[i] = new LN();

    if(hash != to_copy.hash)
    {
        hash = to_copy.hash;
        insert_all(to_copy);
        used = to_copy.used;
    }
    else
        set = copy_hash_table(to_copy.set, bins);
}


template<class T, int (*thash)(const T& a)>
HashSet<T,thash>::HashSet(const std::initializer_list<T>& il, double the_load_threshold, int (*chash)(const T& element))
: hash(thash != (hashfunc)undefinedhash ? thash : chash), load_threshold(the_load_threshold)
{
    if (hash == (hashfunc)undefinedhash)
        throw TemplateFunctionError("hash_set::initializer list constructor: neither specified");
    if (thash != (hashfunc)undefinedhash && chash != (hashfunc)undefinedhash && thash != chash)
        throw TemplateFunctionError("hash_set::initializer list constructor: both specified and different");

    set = new LN*[bins];
    set[0] = new LN();

    for (auto& i:il)
        insert(i);
}


template<class T, int (*thash)(const T& a)>
template<class Iterable>
HashSet<T,thash>::HashSet(const Iterable& i, double the_load_threshold, int (*chash)(const T& a))
: hash(thash != (hashfunc)undefinedhash ? thash : chash), load_threshold(the_load_threshold)
{
    if (hash == (hashfunc)undefinedhash)
        throw TemplateFunctionError("hash_set::iterable constructor: neither specified");
    if (thash != (hashfunc)undefinedhash && chash != (hashfunc)undefinedhash && thash != chash)
        throw TemplateFunctionError("hash_set::iterable constructor: both specified and different");

    set = new LN*[bins];
    set[0] = new LN();

    for (auto& items:i)
        insert(items);
}


////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::empty() const
{
    return used == 0;
}


template<class T, int (*thash)(const T& a)>
int HashSet<T,thash>::size() const
{
    return used;
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::contains (const T& element) const
{
    return (find_element(element) != nullptr);
}


template<class T, int (*thash)(const T& a)>
std::string HashSet<T,thash>::str() const
{
    std::ostringstream answer;
    answer << "HashSet[\n";

    if (used != 0)
    {
        int i = 0;
        for (i = 0; i < bins; i++)
        {
            answer << "bin[" << i << "]: ";
            for (LN* p = set[i]; p->next != nullptr; p = p->next)
            {
                if (p->next != nullptr)
                    answer << p->value << " -> ";
            }
            answer << "TRAILER\n";
        }
    }

    answer << "](bins=" << bins << ",used=" << used << ",mod_count=" << mod_count << ")";
    return answer.str();
}


template<class T, int (*thash)(const T& a)>
template <class Iterable>
bool HashSet<T,thash>::contains_all(const Iterable& i) const
{
    for (const T& v : i)
        if (!contains(v))
            return false;

    return true;
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands

template<class T, int (*thash)(const T& a)>
int HashSet<T,thash>::insert(const T& element)
{
    LN* k = find_element(element);
    if (k != nullptr)
        return 0;

    ensure_load_threshold(used + 1);
    int num = hash_compress(element);
    LN* list = set[num];
    set[num] = new LN (element, list);
    used++;
    mod_count++;
    return 1;
}


template<class T, int (*thash)(const T& a)>
int HashSet<T,thash>::erase(const T& element)
{
    LN* k = find_element(element);

    if (k == nullptr)
        return 0;

    mod_count++;
    used--;
    int index = hash_compress(element);
    if (set[index] == k)
    {
        set[index] = k->next;
        delete k;
        return 1;
    }
    LN* p;
    for (p = set[index]; p->next != k; p = p->next)
    {}

    p->next = k->next;
    delete k;
    return 1;
}


template<class T, int (*thash)(const T& a)>
void HashSet<T,thash>::clear()
{
    LN** to_delete = set;
    int old_bins = bins;
    used = 0;
    bins = 1;
    set = new LN*[bins];
    set[0] = new LN();
    delete_hash_table(to_delete, old_bins);
    mod_count++;
}


template<class T, int (*thash)(const T& a)>
template<class Iterable>
int HashSet<T,thash>::insert_all(const Iterable& i)
{
    int count = 0;
    for (auto element : i)
        count += insert(element);

    return count;
}


template<class T, int (*thash)(const T& a)>
template<class Iterable>
int HashSet<T,thash>::erase_all(const Iterable& i)
{
    int count = 0;
    for (const T& v : i)
        count += erase(v);

    return count;
}


template<class T, int (*thash)(const T& a)>
template<class Iterable>
int HashSet<T,thash>::retain_all(const Iterable& i)
{
    HashSet s(i);
    int count = 0;
    HashSet new_set;

    for(auto ele : *this){
        if (s.contains(ele)){
            count += new_set.insert(ele);
        }
    }
    *this = new_set;
    return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class T, int (*thash)(const T& a)>
HashSet<T,thash>& HashSet<T,thash>::operator = (const HashSet<T,thash>& rhs)
{
    if (hash != rhs.hash)
        hash = rhs.hash;

    clear();
    set = copy_hash_table(rhs.set, rhs.bins);
    mod_count++;
    used = rhs.used;
    load_threshold = rhs.load_threshold;
    bins = rhs.bins;
    return *this;
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::operator == (const HashSet<T,thash>& rhs) const
{
    if (this == &rhs)
        return true;

    if(used != rhs.used)
        return false;

    if(hash != rhs.hash)
        return false;

    for (int i = 0; i < bins; i++)
    {
        for (LN* p = set[i]; p->next != nullptr; p= p->next)
        {
            LN* check = rhs.find_element(p->value);
            if (check == nullptr)
                return false;
            if (check->value != p->value)
                return false;
        }
    }

    return true;
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::operator != (const HashSet<T,thash>& rhs) const
{
    return !(*this == rhs);
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::operator <= (const HashSet<T,thash>& rhs) const
{
    if (this == &rhs)
        return true;

    if (used > rhs.size())
        return false;

    for (auto& element: rhs)
        if (!rhs.contains(element))
            return false;

    return true;
}

template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::operator < (const HashSet<T,thash>& rhs) const
{
    if (this == &rhs)
        return false;

    if (used >= rhs.size())
        return false;

    for (auto& element: rhs)
        if (!rhs.contains(element))
            return false;

    return true;
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::operator >= (const HashSet<T,thash>& rhs) const
{
    return rhs <= *this;
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::operator > (const HashSet<T,thash>& rhs) const
{
    return rhs < *this;
}


template<class T, int (*thash)(const T& a)>
std::ostream& operator << (std::ostream& outs, const HashSet<T,thash>& s)
{
    outs << "set[";

    if (s.set != nullptr)
    {
        for(int i = 0; i < s.bins; i++)
        {
            if(s.set[i]->next != nullptr)
            {
                typename HashSet<T,thash>::LN* ptr = s.set[i];
                while(ptr->next != nullptr)
                {
                    outs << ptr->value;
                    ptr = ptr->next;
                }
            }
        }
    }

    return outs << "]";
}


////////////////////////////////////////////////////////////////////////////////
//
//Iterator constructors

template<class T, int (*thash)(const T& a)>
auto HashSet<T,thash>::begin () const -> HashSet<T,thash>::Iterator
{
    if (this->empty())
    {
        return Iterator(const_cast<HashSet<T,thash>*>(this),false);
    }
    return Iterator(const_cast<HashSet<T,thash>*>(this),true);
}


template<class T, int (*thash)(const T& a)>
auto HashSet<T,thash>::end () const -> HashSet<T,thash>::Iterator
{
    return Iterator(const_cast<HashSet<T,thash>*>(this),false);
}


///////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class T, int (*thash)(const T& a)>
int HashSet<T,thash>::hash_compress (const T& element) const {
    int code = abs(hash(element));
    return (code % bins);
}


template<class T, int (*thash)(const T& a)>
typename HashSet<T,thash>::LN* HashSet<T,thash>::find_element (const T& element) const {
    int index = hash_compress(element);
    for (LN* ptr = set[index]; ptr->next != nullptr; ptr = ptr->next)
    {
        if (element == ptr->value)
        {
            return ptr;
        }
    }
    return nullptr;
}

template<class T, int (*thash)(const T& a)>
typename HashSet<T,thash>::LN* HashSet<T,thash>::copy_list (LN* l) const {
    LN* copy = new LN(l->value);
    LN* c_ptr = copy;
    LN* ptr = l->next;

    while(ptr != nullptr)
    {
        LN* n = new LN(ptr->value);
        c_ptr->next = n;
        c_ptr = n;
        ptr = ptr->next;
    }

    return copy;
}


template<class T, int (*thash)(const T& a)>
typename HashSet<T,thash>::LN** HashSet<T,thash>::copy_hash_table (LN** ht, int bins) const {
    LN** table = new LN*[bins];
    for (int i = 0; i < bins; i++)
    {
        table[i] = copy_list(ht[i]);
    }

    return table;
}


template<class T, int (*thash)(const T& a)>
void HashSet<T,thash>::ensure_load_threshold(int new_used)
{    if (double(new_used)/double(bins) > double(1) ||  double(new_used)/double(bins) > load_threshold )
    {
        load_threshold = double(new_used)/double(bins);
        bins = 2*bins;
        LN** new_map = new LN*[bins];
        for(int i = 0; i < bins; ++i){
            new_map[i] = new LN();
        }

        for(int i = 0; i < bins/2; ++i)
        {
            if(set[i]->next != nullptr){
                for (LN* ptr = set[i]; ptr->next != nullptr; )
                {
                    int num = hash_compress(ptr->value);
                    LN* p2 = ptr;
                    ptr = ptr->next;
                    LN* p = new_map[num];
                    new_map[num] = p2;
                    p2->next = p;
                }
            }else if (set[i]->next == nullptr){
                delete set[i];
            }
        }
        LN** temp = set;
        set = new_map;
        delete [] temp;
    }
}


template<class T, int (*thash)(const T& a)>
void HashSet<T,thash>::delete_hash_table (LN**& ht, int bins)
{
    for (int i  = 0; i < bins; ++i)
    {
        LN* temp = ht[i];
        while(temp != nullptr)
        {
            LN* d = temp;
            temp = temp->next;
            delete d;
        }
        ht[i] = nullptr;
    }
    delete [] ht;
    ht = nullptr;
}






////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class T, int (*thash)(const T& a)>
void HashSet<T,thash>::Iterator::advance_cursors()
{
    if (current.second->next == nullptr)
    {
        current.first++;
        if(current.first > ref_set->bins-1)
        {
            current.first = -1;
            current.second = nullptr;
            return;
        }
        current.second = ref_set->set[current.first];

        return;
    }

    current.second = current.second->next;
}


template<class T, int (*thash)(const T& a)>
HashSet<T,thash>::Iterator::Iterator(HashSet<T,thash>* iterate_over, bool begin)
: ref_set(iterate_over), expected_mod_count(ref_set->mod_count)
{
    if(begin)
    {
        current.second = ref_set->set[0];
        int b = 0;

        while(current.second->next == nullptr)
        {
            b++;
            current.second = ref_set->set[b];
        }
        current.first = b;
    }
    else
    {
        current.first = -1;
        current.second = nullptr;
    }
}


template<class T, int (*thash)(const T& a)>
HashSet<T,thash>::Iterator::~Iterator()
{}


template<class T, int (*thash)(const T& a)>
T HashSet<T,thash>::Iterator::erase()
{
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("HashSet::Iterator::erase");
    if (!can_erase)
        throw CannotEraseError("HashSet::Iterator::erase Iterator cursor already erased");
    if (current.first == -1 || current.second == nullptr)
        throw CannotEraseError("HashSet::Iterator::erase Iterator cursor beyond data structure");

    can_erase = false;
    T to_return = current.second->value;
    ref_set->erase(current.second->value);

    advance_cursors();
    if(current.first != -1)
    {
        while (current.second->next == nullptr)
        {
            advance_cursors();
            if (current.first == -1)
                break;
        }
    }
    expected_mod_count = ref_set->mod_count;
    return to_return;
}


template<class T, int (*thash)(const T& a)>
std::string HashSet<T,thash>::Iterator::str() const
{
    std::ostringstream answer;
    answer << *ref_set << "/expected_mod_count=" << expected_mod_count << "/can_erase=" << can_erase << "/current" << current;
    return answer.str();
}


template<class T, int (*thash)(const T& a)>
auto  HashSet<T,thash>::Iterator::operator ++ () -> HashSet<T,thash>::Iterator&
{
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("hash_set::Iterator::operator ++");

    if (current.first != -1 || current.second != nullptr)
    {
        if (can_erase)
        {
            advance_cursors();
            if(current.first != -1)
            {
                while (current.second->next == nullptr)
                {
                    advance_cursors();
                    if (current.first == -1)
                        break;
                }
            }
        }

        else{can_erase = true;}
    }

    return *this;
}


template<class T, int (*thash)(const T& a)>
auto  HashSet<T,thash>::Iterator::operator ++ (int) -> HashSet<T,thash>::Iterator
{
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("hash_set::Iterator::operator ++");

    Iterator to_return(*this);
    if (current.first != -1 || current.second != nullptr)
    {
        if (can_erase)
        {
            advance_cursors();
            if(current.first != -1) {
                while (current.second->next == nullptr)
                {
                    advance_cursors();
                    if (current.first == -1)
                        break;
                }
            }
        }

        else{can_erase = true;}
    }

    return to_return;
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::Iterator::operator == (const HashSet<T,thash>::Iterator& rhs) const
{
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("HashSet::Iterator::operator ==");
    if (ref_set != rhsASI->ref_set)
        throw ComparingDifferentIteratorsError("HashSet::Iterator::operator ==");

    return current == rhsASI->current;
}


template<class T, int (*thash)(const T& a)>
bool HashSet<T,thash>::Iterator::operator != (const HashSet<T,thash>::Iterator& rhs) const
{
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("HashSet::Iterator::operator ==");
    if (ref_set != rhsASI->ref_set)
        throw ComparingDifferentIteratorsError("HashSet::Iterator::operator ==");

    return current != rhsASI->current;
}

template<class T, int (*thash)(const T& a)>
T& HashSet<T,thash>::Iterator::operator *() const
{
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("hash_set::Iterator::operator *");
    if (!can_erase || current.first == -1 || current.second == nullptr) {
        std::ostringstream where;
        where << " when size = " << ref_set->size();
        throw IteratorPositionIllegal("hash_set::Iterator::operator * Iterator illegal: "+ where.str());
    }

    return current.second->value;
}

template<class T, int (*thash)(const T& a)>
T* HashSet<T,thash>::Iterator::operator ->() const
{
    if (expected_mod_count != ref_set->mod_count)
        throw ConcurrentModificationError("hash_set::Iterator::operator ->");
    if (!can_erase || current.first == -1 || current.second == nullptr) {
        std::ostringstream where;
        where << " when size = " << ref_set->size();
        throw IteratorPositionIllegal("hash_set::Iterator::operator -> Iterator illegal: "+ where.str());
    }

    return &current.second->value;
}

}

#endif /* HASH_SET_HPP_ */
