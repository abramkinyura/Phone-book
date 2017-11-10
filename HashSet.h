//
// class HashSet and auxiliary classes
//
#ifndef HASH_SET_H
#define HASH_SET_H

// An ABSTRACT class representing a key in HashSet
class HashSetKey {
public:
    HashSetKey() {}
    virtual ~HashSetKey() {}    // virtual destructor

    // Abstract class has virtual methods unimplemented

    virtual bool operator==(const HashSetKey&) const = 0;

    virtual int hashValue() const = 0;  // Abstract class

    // The virtual method "clone" must allocate a copy of the object
    // in the dynamic memory. In any derived class Foo it must
    // be always implemented in the following way:
    // virtual Foo* clone() const { return new Foo(*this); }
    //
    virtual HashSetKey* clone() const = 0;

    bool operator!=(const HashSetKey& k) const {
        return !operator==(k);
    }
};

// An ABSTRACT class representing a value of a key in HashSet
class HashSetValue {
public:
    HashSetValue() {}
    virtual ~HashSetValue() {}
    virtual HashSetValue* clone() const = 0;
};

//
// class HashSet implements "Map" or "Dictionary".
// It stores the set of pairs: (key, value).
// All keys are unique (different pairs have different keys).
//
class HashSet {
public:
    class Pair {
    public:
        const HashSetKey* key;
        HashSetValue* value;
        Pair():
            key(0),
            value(0)
        {}
        Pair(const HashSetKey* k, HashSetValue* v):
            key(k),
            value(v)
        {}
    };

private:
    class L1ListHeader {
    public:
        L1ListHeader* next;
        L1ListHeader(): next(0) {}
        L1ListHeader(const L1ListHeader& h): next(h.next) {}
        void link(const L1ListHeader* h) { next = (L1ListHeader*) h; }
    };

    class L1ListElement: public L1ListHeader {
    public:
        Pair pair;
        L1ListElement():
            L1ListHeader(),
            pair()
        {}
        L1ListElement(HashSetKey* k, HashSetValue* v):
            L1ListHeader(),
            pair(k, v)
        {}
        ~L1ListElement() {
            delete pair.key;
            delete pair.value;
        }
    };

    int hashTableSize;
    L1ListHeader* hashTable;
    int numElements;

public:
    HashSet():
        hashTableSize(1021),    // Prime number
        hashTable(new L1ListHeader[hashTableSize]),
        numElements(0)
    {}

    HashSet(int tableSize):
        hashTableSize(tableSize),
        hashTable(new L1ListHeader[hashTableSize]),
        numElements(0)
    {}

    int size() const { return numElements; }

    // Add a pair (key, value) to the set
    void add(const HashSetKey* k, const HashSetValue* v = 0);

    void remove(const HashSetKey* key);

    // Return a value of a key
    HashSetValue* value(const HashSetKey* k) const;
    HashSetValue* operator[](const HashSetKey* k) const {
        return value(k);
    }
    bool contains(const HashSetKey* k) const;

public:
    class iterator {
    private:
        HashSet* set;
        int hash;
        L1ListElement* element;
    public:
        iterator(): set(0), hash(0), element(0) {}
        iterator(HashSet* s, int h);
        iterator& operator++();
        iterator operator++(int) { // Postfix increment operator
            iterator tmp = *this;
            operator++();          // Apply the prefix increment operator
            return tmp;
        }
        iterator& operator--();
        iterator operator--(int) { // Postfix decrement operator
            iterator tmp = *this;
            operator--();          // Apply the prefix decrement operator
            return tmp;
        }
        const Pair& operator*() const {
            return element->pair;
        }
        const Pair* operator->() const {
            return &(operator*());
        }
        bool operator==(const iterator& i) const {
            return (
                set == i.set &&
                hash == i.hash &&
                element == i.element
            );
        }
        bool operator!=(const iterator& i) const {
            return !operator==(i);
        }
    };

    class const_iterator: public iterator {
    public:
        const_iterator():
            iterator()
        {
        }

        const_iterator(const HashSet* s, int h):
            iterator((HashSet*) s, h)
        {
        }

        const_iterator(const iterator& i):
            iterator(i)
        {
        }
    };

public:
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

private:
    // Calculate an index in hash table
    int hashIndex(const HashSetKey* key) const;

    // Find the PREVIOUS list element to element that contains the key.
    // Returns zero if element is not found.
    L1ListHeader* search(const HashSetKey* key) const;
};

#endif /* HASH_SET_H */
