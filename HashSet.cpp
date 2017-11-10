#include "HashSet.h"

int HashSet::hashIndex(const HashSetKey* key) const {
    int h = key->hashValue();   // Calculate the hash function
    h &= 0x7fffffff;            // Make it positive
    h %= hashTableSize;         // Restrict it to 0..hashTableSize-1
    return h;
}

void HashSet::add(
    const HashSetKey* key, const HashSetValue* value /* = 0 */
) {
    L1ListHeader* p = search(key);
    if (p != 0) {
        // The key belongs to set, change its value
        L1ListElement* e = (L1ListElement*) p->next;
        delete e->pair.value;
        // Write the new value
        if (value == 0)
            e->pair.value = 0;
        else
            e->pair.value = value->clone();
    } else {
        // Add new element
        int h = hashIndex(key);
        HashSetKey* k = key->clone();
        HashSetValue* v = 0;
        if (value != 0)
            v = value->clone();
        L1ListElement* element = new L1ListElement(k, v);

        // Include the new element in the head of the chain with index h
        element->link(hashTable[h].next);
        hashTable[h].link(element);
        ++numElements;
    }
}

void HashSet::remove(const HashSetKey* key) {
    L1ListHeader* p = search(key);
    if (p != 0) {
        // Element belongs to set
        L1ListElement* e = (L1ListElement*) p->next;
        p->link(e->next);       // Exclude an element from a chain
        delete e;
        --numElements;
    }
}

HashSet::L1ListHeader* HashSet::search(const HashSetKey* key) const {
    int h = hashIndex(key);
    L1ListHeader* p = &(hashTable[h]); // The head of the chain with index h
    L1ListElement* e = (L1ListElement*) p->next; // First element in the chain
    while (e != 0) {
        if (*(e->pair.key) == *key)
            return p;                           // The key is found
        // Go to the next element in chain
        p = e; 
        e = (L1ListElement*) p->next;
    }
    return 0;   // Not found
}

bool HashSet::contains(const HashSetKey* key) const {
    return (search(key) != 0);
}

HashSetValue* HashSet::value(const HashSetKey* k) const {
    const L1ListHeader* h = search(k);
    if (h == 0)
        return 0;
    else
        return ((const L1ListElement*) h->next)->pair.value;
}

HashSet::iterator::iterator(HashSet* s, int h):
    set(s),
    hash(h),
    element(0)
{
    if (set != 0 && 0 <= h && h < set->hashTableSize)
        element = (L1ListElement*) set->hashTable[h].next;
}

HashSet::iterator& HashSet::iterator::operator++() {
    if (element != 0) {
        // Go to the next element in this chain
        element = (L1ListElement*) (element->next);
    }
    if (element == 0) {
        // Go to the next chain
        ++hash;

        // Find out nonempty chain
        while (
            hash < set->hashTableSize &&
            set->hashTable[hash].next == 0
        )
            ++hash;
        if (hash < set->hashTableSize)
            element = (L1ListElement*) (set->hashTable[hash].next);
    }
    return *this;
}

HashSet::iterator HashSet::begin() {
    int h = 0;
    while (h < hashTableSize && hashTable[h].next == 0)
        ++h;
    return iterator(this, h);
}

HashSet::iterator HashSet::end() {
    return iterator(this, hashTableSize);
}
