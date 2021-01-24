// Submitter: haitaoy(Ye, Haitao)
// Partner  : burnscr(Burns, Connor)
// We certify that we worked cooperatively on this programming
//   assignment, according to the rules for pair programming
#ifndef HASH_MAP_HPP_
#define HASH_MAP_HPP_

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
template<class KEY,class T, int (*thash)(const KEY& a) = undefinedhash<KEY>> class HashMap {
  public:
    typedef ics::pair<KEY,T>   Entry;
    typedef int (*hashfunc) (const KEY& a);

    //Destructor/Constructors
    ~HashMap ();

    HashMap          (double the_load_threshold = 1.0, int (*chash)(const KEY& a) = undefinedhash<KEY>);
    explicit HashMap (int initial_bins, double the_load_threshold = 1.0, int (*chash)(const KEY& k) = undefinedhash<KEY>);
    HashMap          (const HashMap<KEY,T,thash>& to_copy, double the_load_threshold = 1.0, int (*chash)(const KEY& a) = undefinedhash<KEY>);
    explicit HashMap (const std::initializer_list<Entry>& il, double the_load_threshold = 1.0, int (*chash)(const KEY& a) = undefinedhash<KEY>);

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    explicit HashMap (const Iterable& i, double the_load_threshold = 1.0, int (*chash)(const KEY& a) = undefinedhash<KEY>);


    //Queries
    bool empty      () const;
    int  size       () const;
    bool has_key    (const KEY& key) const;
    bool has_value  (const T& value) const;
    std::string str () const; //supplies useful debugging information; contrast to operator <<


    //Commands
    T    put   (const KEY& key, const T& value);
    T    erase (const KEY& key);
    void clear ();

    //Iterable class must support "for-each" loop: .begin()/.end() and prefix ++ on returned result
    template <class Iterable>
    int put_all(const Iterable& i);


    //Operators

    T&       operator [] (const KEY&);
    const T& operator [] (const KEY&) const;
    HashMap<KEY,T,thash>& operator = (const HashMap<KEY,T,thash>& rhs);
    bool operator == (const HashMap<KEY,T,thash>& rhs) const;
    bool operator != (const HashMap<KEY,T,thash>& rhs) const;

    template<class KEY2,class T2, int (*hash2)(const KEY2& a)>
    friend std::ostream& operator << (std::ostream& outs, const HashMap<KEY2,T2,hash2>& m);



  private:
    class LN;

  public:
    class Iterator {
      public:
         typedef pair<int,LN*> Cursor;

        //Private constructor called in begin/end, which are friends of HashMap<T>
        ~Iterator();
        Entry       erase();
        std::string str  () const;
        HashMap<KEY,T,thash>::Iterator& operator ++ ();
        HashMap<KEY,T,thash>::Iterator  operator ++ (int);
        bool operator == (const HashMap<KEY,T,thash>::Iterator& rhs) const;
        bool operator != (const HashMap<KEY,T,thash>::Iterator& rhs) const;
        Entry& operator *  () const;
        Entry* operator -> () const;
        friend std::ostream& operator << (std::ostream& outs, const HashMap<KEY,T,thash>::Iterator& i) {
          outs << i.str(); //Use the same meaning as the debugging .str() method
          return outs;
        }
        friend Iterator HashMap<KEY,T,thash>::begin () const;
        friend Iterator HashMap<KEY,T,thash>::end   () const;

      private:
        //If can_erase is false, current indexes the "next" value (must ++ to reach it)
        Cursor                current; //Pair:Bin Index/LN*; stops if LN* == nullptr
        HashMap<KEY,T,thash>* ref_map;
        int                   expected_mod_count;
        bool                  can_erase = true;

        //Helper methods
        void advance_cursors();

        //Called in friends begin/end
        Iterator(HashMap<KEY,T,thash>* iterate_over, bool from_begin);
    };


    Iterator begin () const;
    Iterator end   () const;


  private:
    class LN {
    public:
      LN ()                         : next(nullptr){}
      LN (const LN& ln)             : value(ln.value), next(ln.next){}
      LN (Entry v, LN* n = nullptr) : value(v), next(n){}

      Entry value;
      LN*   next;
  };

  int (*hash)(const KEY& k);  //Hashing function used (from template or constructor)
  LN** map      = nullptr;    //Pointer to array of pointers: each bin stores a list with a trailer node
  double load_threshold;      //used/bins <= load_threshold
  int bins      = 1;         //# bins currently in array (start it >= 1 so no divide by 0 in hash_compress)
  int used      = 0;          //Cache for number of key->value pairs in the hash table
  int mod_count = 0;          //For sensing concurrent modification


  //Helper methods
  int   hash_compress        (const KEY& key)          const;  //hash function ranged to [0,bins-1]
  LN*   find_key             (const KEY& key) const;           //Returns reference to key's node or nullptr
  LN*   copy_list            (LN*   l)                 const;  //Copy the keys/values in a bin (order irrelevant)
  LN**  copy_hash_table      (LN** ht, int bins)       const;  //Copy the bins/keys/values in ht tree (order in bins irrelevant)

  void  ensure_load_threshold(int new_used);                   //Reallocate if load_factor > load_threshold
  void  delete_hash_table    (LN**& ht, int bins);             //Deallocate all LN in ht (and the ht itself; ht == nullptr)
};





////////////////////////////////////////////////////////////////////////////////
//
//HashMap class and related definitions

//Destructor/Constructors

template<class KEY,class T, int (*thash)(const KEY& a)>
HashMap<KEY,T,thash>::~HashMap()
{
    delete_hash_table(map, bins);
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashMap<KEY,T,thash>::HashMap(double the_load_threshold, int (*chash)(const KEY& k))
: hash(thash != (hashfunc)undefinedhash ? thash : chash), load_threshold(the_load_threshold)
{
    if (hash == (hashfunc)undefinedhash)
        throw TemplateFunctionError("hash_map::default constructor: neither specified");
    if (thash != (hashfunc)undefinedhash && chash != (hashfunc)undefinedhash && thash != chash)
        throw TemplateFunctionError("hash_map::default constructor: both specified and different");

    map = new LN*[bins];
    map[0] = new LN();
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashMap<KEY,T,thash>::HashMap(int initial_bins, double the_load_threshold, int (*chash)(const KEY& k))
: hash(thash != (hashfunc)undefinedhash ? thash : chash), bins(initial_bins), load_threshold(the_load_threshold)
{
    if (hash == (hashfunc)undefinedhash)
        throw TemplateFunctionError("hash_map::initial bin constructor: neither specified");
    if (thash != (hashfunc)undefinedhash && chash != (hashfunc)undefinedhash && thash != chash)
        throw TemplateFunctionError("hash_map::initial bin constructor: both specified and different");

    map = new LN*[bins];
    for (int i = 0; i < bins; i++)
    {
        map[i] = new LN();
    }
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashMap<KEY,T,thash>::HashMap(const HashMap<KEY,T,thash>& to_copy, double the_load_threshold, int (*chash)(const KEY& a))
: hash(thash != (hashfunc)undefinedhash ? thash : chash),bins(to_copy.bins), load_threshold(the_load_threshold), used(to_copy.used)
{
    if (hash == (hashfunc)undefinedhash)
        hash = to_copy.hash;
    if (thash != (hashfunc)undefinedhash && chash != (hashfunc)undefinedhash && thash != chash)
        throw TemplateFunctionError("hash_map::initial bin constructor: both specified and different");

    map = new LN*[bins];
    for (int i = 0; i < bins; i++)
    {
        map[i] = new LN();
    }

    if(hash != to_copy.hash)
    {
        hash = to_copy.hash;
        put_all(to_copy);
        used = to_copy.used;
    }
    else
        map = copy_hash_table(to_copy.map, bins);
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashMap<KEY,T,thash>::HashMap(const std::initializer_list<Entry>& il, double the_load_threshold, int (*chash)(const KEY& k))
: hash(thash != (hashfunc)undefinedhash ? thash : chash), load_threshold(the_load_threshold)
{
    if (hash == (hashfunc)undefinedhash)
        throw TemplateFunctionError("hash_map::initializer bin constructor: neither specified");
    if (thash != (hashfunc)undefinedhash && chash != (hashfunc)undefinedhash && thash != chash)
        throw TemplateFunctionError("hash_map::initializer bin constructor: both specified and different");

    map = new LN*[bins];
    map[0] = new LN();

    for (auto& i:il)
    {
        put(i.first, i.second);
    }
}


template<class KEY,class T, int (*thash)(const KEY& a)>
template <class Iterable>
HashMap<KEY,T,thash>::HashMap(const Iterable& i, double the_load_threshold, int (*chash)(const KEY& k))
: hash(thash != (hashfunc)undefinedhash ? thash : chash), load_threshold(the_load_threshold)
{
    if (hash == (hashfunc)undefinedhash)
        throw TemplateFunctionError("hash_map::iterable bin constructor: neither specified");
    if (thash != (hashfunc)undefinedhash && chash != (hashfunc)undefinedhash && thash != chash)
        throw TemplateFunctionError("hash_map::iterable bin constructor: both specified and different");

    map = new LN*[bins];
    map[0] = new LN();

    for (auto& i:i)
    {
        put(i.first, i.second);
    }
}


////////////////////////////////////////////////////////////////////////////////
//
//Queries

template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashMap<KEY,T,thash>::empty() const {
    return used == 0;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
int HashMap<KEY,T,thash>::size() const {
    return used;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashMap<KEY,T,thash>::has_key (const KEY& key) const
{
    return (find_key(key) != nullptr);
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashMap<KEY,T,thash>::has_value (const T& value) const
{
    for(int i = 0; i < bins; i++)
    {
        for(LN* p= map[i]; p->next != nullptr; p = p->next)
        {
            if (p->value.second == value)
                return true;
        }
    }
    return false;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
std::string HashMap<KEY,T,thash>::str() const
{
    std::ostringstream answer;
    answer << "HashMap[\n";

    if (used != 0)
    {
        int i = 0;
        for (int i = 0; i < bins; i++)
        {
            answer << "bin[" << i << "]: ";
            for (LN* p = map[i]; p->next != nullptr; p = p->next)
            {
                if (p->next != nullptr)
                answer << p->value.first << "->" << p->value.second << " -> ";
            }
                answer << "TRAILER\n";
        }
    }

    answer << "](bins=" << bins << ",used=" << used << ",mod_count=" << mod_count << ")";
    return answer.str();
}


////////////////////////////////////////////////////////////////////////////////
//
//Commands

template<class KEY,class T, int (*thash)(const KEY& a)>
T HashMap<KEY,T,thash>::put(const KEY& key, const T& value)
{
    LN* k = find_key(key);
    if (k != nullptr)
    {
        T old = k->value.second;
        k->value.second = value;
        return old;
    }

    ensure_load_threshold(used + 1);
    int num = hash_compress(key);
    LN* list = map[num];
    map[num] = new LN (Entry(key, value), list);
    used++;
    mod_count++;
    return value;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
T HashMap<KEY,T,thash>::erase(const KEY& key)
{
    LN* k = find_key(key);

    if (k == nullptr) {
        std::ostringstream answer;
        answer << "BSTMap::erase: key(" << key << ") not in Map";
        throw KeyError(answer.str());
    }

    mod_count++;
    used--;
    int index = hash_compress(key);
    T to_return = k->value.second;
    if (map[index] == k)
    {
        map[index] = k->next;
        delete k;
        return to_return;
    }
    LN* p;
    for (p = map[index]; p->next != k; p = p->next)
    {}

    p->next = k->next;
    delete k;
    return to_return;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
void HashMap<KEY,T,thash>::clear()
{
    LN** to_delete = map;
    int old_bins = bins;
    used = 0;
    bins = 1;
    map = new LN*[bins];
    map[0] = new LN();
    delete_hash_table(to_delete, old_bins);
    mod_count++;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
template<class Iterable>
int HashMap<KEY,T,thash>::put_all(const Iterable& i)
{
    int count = 0;
    for (auto m_entry : i) {
        ++count;
        put(m_entry.first, m_entry.second);
    }

    return count;
}


////////////////////////////////////////////////////////////////////////////////
//
//Operators

template<class KEY,class T, int (*thash)(const KEY& a)>
T& HashMap<KEY,T,thash>::operator [] (const KEY& key)
{
    LN* k = find_key(key);

    if (k != nullptr)
        return k->value.second;

    ensure_load_threshold(used + 1);
    put(key, T());
    T& to_return = find_key(key)->value.second;
    return to_return;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
const T& HashMap<KEY,T,thash>::operator [] (const KEY& key) const
{
    LN* k = find_key(key);

    if (k != nullptr)
        return k->value.second;

    std::ostringstream answer;
    answer << "hash_Map::operator []: key(" << key << ") not in Map";
    throw KeyError(answer.str());

}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashMap<KEY,T,thash>& HashMap<KEY,T,thash>::operator = (const HashMap<KEY,T,thash>& rhs)
{
    if (hash != rhs.hash)
        hash = rhs.hash;

    clear();
    map = copy_hash_table(rhs.map, rhs.bins);
    mod_count++;
    used = rhs.used;
    load_threshold = rhs.load_threshold;
    bins = rhs.bins;
    return *this;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashMap<KEY,T,thash>::operator == (const HashMap<KEY,T,thash>& rhs) const
{
    if (this == &rhs)
        return true;

    if(used != rhs.used)
        return false;

    if(hash != rhs.hash)
        return false;

    for (int i = 0; i < bins; i++)
    {
        for (LN* p = map[i]; p->next != nullptr; p= p->next)
        {
            LN* check = rhs.find_key(p->value.first);
            if (check == nullptr)
            {
                return false;}
            if (check->value.second != p->value.second)
                return false;
        }
    }

    return true;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashMap<KEY,T,thash>::operator != (const HashMap<KEY,T,thash>& rhs) const
{
    return !(*this == rhs);
}


template<class KEY,class T, int (*thash)(const KEY& a)>
std::ostream& operator << (std::ostream& outs, const HashMap<KEY,T,thash>& m)
{
    outs << "map[";

    if (m.map != nullptr)
    {
        for(int i = 0; i < m.bins; i++)
        {
            if(m.map[i]->next != nullptr)
            {
                typename HashMap<KEY,T,thash>::LN* ptr = m.map[i];
                while(ptr->next != nullptr)
                {
                    outs << ptr->value.first << "->" << ptr->value.second;
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

template<class KEY,class T, int (*thash)(const KEY& a)>
auto HashMap<KEY,T,thash>::begin () const -> HashMap<KEY,T,thash>::Iterator
{
    if (this->empty())
    {
        return Iterator(const_cast<HashMap<KEY,T,thash>*>(this),false);
    }
    return Iterator(const_cast<HashMap<KEY,T,thash>*>(this),true);
}


template<class KEY,class T, int (*thash)(const KEY& a)>
auto HashMap<KEY,T,thash>::end () const -> HashMap<KEY,T,thash>::Iterator
{
    return Iterator(const_cast<HashMap<KEY,T,thash>*>(this),false);
}


///////////////////////////////////////////////////////////////////////////////
//
//Private helper methods

template<class KEY,class T, int (*thash)(const KEY& a)>
int HashMap<KEY,T,thash>::hash_compress (const KEY& key) const
{
    int code = abs(hash(key));
    return (code % bins);
}


template<class KEY,class T, int (*thash)(const KEY& a)>
typename HashMap<KEY,T,thash>::LN* HashMap<KEY,T,thash>::find_key (const KEY& key) const
{
    int index = hash_compress(key);
    for (LN* ptr = map[index]; ptr->next != nullptr; ptr = ptr->next)
    {
        if (key == ptr->value.first)
        {
            return ptr;
        }
    }
    return nullptr;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
typename HashMap<KEY,T,thash>::LN* HashMap<KEY,T,thash>::copy_list (LN* l) const
{
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


template<class KEY,class T, int (*thash)(const KEY& a)>
typename HashMap<KEY,T,thash>::LN** HashMap<KEY,T,thash>::copy_hash_table (LN** ht, int bins) const
{
    LN** table = new LN*[bins];
    for (int i = 0; i < bins; i++)
    {
        table[i] = copy_list(ht[i]);
    }

    return table;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
void HashMap<KEY,T,thash>::ensure_load_threshold(int new_used)
{
    if (double(new_used)/double(bins) > double(1) ||  double(new_used)/double(bins) > load_threshold )
    {
        load_threshold = double(new_used)/double(bins);
        bins = 2*bins;
        LN** new_map = new LN*[bins];
        for(int i = 0; i < bins; ++i){
            new_map[i] = new LN();
        }

        for(int i = 0; i < bins/2; ++i)
        {
            if(map[i]->next != nullptr){
                for (LN* ptr = map[i]; ptr->next != nullptr; )
                {
                    int num = hash_compress(ptr->value.first);
                    LN* p2 = ptr;
                    ptr = ptr->next;
                    LN* p = new_map[num];
                    new_map[num] = p2;
                    p2->next = p;
                }
            }else if (map[i]->next == nullptr){
                delete map[i];
            }
        }
        LN** temp = map;
        map = new_map;
        delete [] temp;
    }
}


template<class KEY,class T, int (*thash)(const KEY& a)>
void HashMap<KEY,T,thash>::delete_hash_table (LN**& ht, int bins)
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
    }
    delete [] ht;
}






////////////////////////////////////////////////////////////////////////////////
//
//Iterator class definitions

template<class KEY,class T, int (*thash)(const KEY& a)>
void HashMap<KEY,T,thash>::Iterator::advance_cursors()
{
    if (current.second->next == nullptr)
    {
        current.first++;
        if(current.first > ref_map->bins-1)
        {
            current.first = -1;
            current.second = nullptr;
            return;
        }
        current.second = ref_map->map[current.first];
        return;
    }

    current.second = current.second->next;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashMap<KEY,T,thash>::Iterator::Iterator(HashMap<KEY,T,thash>* iterate_over, bool from_begin)
: ref_map(iterate_over), expected_mod_count(ref_map->mod_count)
{
    if(from_begin)
    {
        current.second = ref_map->map[0];
        int b = 0;

        while(current.second->next == nullptr)
        {
            b++;
            current.second = ref_map->map[b];
        }
        current.first = b;
    }
    else
    {
        current.first = -1;
        current.second = nullptr;
    }
}


template<class KEY,class T, int (*thash)(const KEY& a)>
HashMap<KEY,T,thash>::Iterator::~Iterator()
{}


template<class KEY,class T, int (*thash)(const KEY& a)>
auto HashMap<KEY,T,thash>::Iterator::erase() -> Entry
{
    if (expected_mod_count != ref_map->mod_count)
        throw ConcurrentModificationError("HashMap::Iterator::erase");
    if (!can_erase)
        throw CannotEraseError("HashMap::Iterator::erase Iterator cursor already erased");
    if (current.first == -1 || current.second == nullptr)
        throw CannotEraseError("HashMap::Iterator::erase Iterator cursor beyond data structure");

    can_erase = false;
    Entry to_return = current.second->value;
    ref_map->erase(current.second->value.first);
    advance_cursors();
    if(current.first != -1) {
        while (current.second->next == nullptr)
        {
            advance_cursors();
            if (current.first == -1)
                break;
        }
    }
    expected_mod_count = ref_map->mod_count;
    return to_return;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
std::string HashMap<KEY,T,thash>::Iterator::str() const
{
    std::ostringstream answer;
    answer << *ref_map << "/expected_mod_count=" << expected_mod_count << "/can_erase=" << can_erase << "/current" << current;
    return answer.str();
}

template<class KEY,class T, int (*thash)(const KEY& a)>
auto  HashMap<KEY,T,thash>::Iterator::operator ++ () -> HashMap<KEY,T,thash>::Iterator&
{
    if (expected_mod_count != ref_map->mod_count)
        throw ConcurrentModificationError("hash_map::Iterator::operator ++");

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

    return *this;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
auto  HashMap<KEY,T,thash>::Iterator::operator ++ (int) -> HashMap<KEY,T,thash>::Iterator {
    if (expected_mod_count != ref_map->mod_count)
        throw ConcurrentModificationError("bst_map::Iterator::operator ++");

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


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashMap<KEY,T,thash>::Iterator::operator == (const HashMap<KEY,T,thash>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (expected_mod_count != ref_map->mod_count)
        throw ConcurrentModificationError("Hashmap::Iterator::operator ==");
    if (ref_map != rhsASI->ref_map)
        throw ComparingDifferentIteratorsError("Hashmap::Iterator::operator ==");

    return current == rhsASI->current;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
bool HashMap<KEY,T,thash>::Iterator::operator != (const HashMap<KEY,T,thash>::Iterator& rhs) const {
    const Iterator* rhsASI = dynamic_cast<const Iterator*>(&rhs);
    if (expected_mod_count != ref_map->mod_count)
        throw ConcurrentModificationError("Hashmap::Iterator::operator !=");
    if (ref_map != rhsASI->ref_map)
        throw ComparingDifferentIteratorsError("Hashmap::Iterator::operator !=");

    return current != rhsASI->current;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
pair<KEY,T>& HashMap<KEY,T,thash>::Iterator::operator *() const
{
    if (expected_mod_count != ref_map->mod_count)
        throw ConcurrentModificationError("hash_map::Iterator::operator *");
    if (!can_erase || current.first == -1 || current.second == nullptr) {
        std::ostringstream where;
        where << " when size = " << ref_map->size();
        throw IteratorPositionIllegal("hash_map::Iterator::operator * Iterator illegal: "+ where.str());
    }

    return current.second->value;
}


template<class KEY,class T, int (*thash)(const KEY& a)>
pair<KEY,T>* HashMap<KEY,T,thash>::Iterator::operator ->() const {
    if (expected_mod_count != ref_map->mod_count)
        throw ConcurrentModificationError("hash_map::Iterator::operator ->");
    if (!can_erase || current.first == -1 || current.second == nullptr) {
        std::ostringstream where;
        where << " when size = " << ref_map->size();
        throw IteratorPositionIllegal("hash_map::Iterator::operator -> Iterator illegal: "+ where.str());
    }

    return &current.second->value;
}


}

#endif /* HASH_MAP_HPP_ */
