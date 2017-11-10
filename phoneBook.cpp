//
// Test of class HashSet: the Phone Book
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

#include "HashSet.h"

static void printHelp();

//
// class Word represents a word in a text.
// This is the dynamic array of characters.
// The class Word is derived from class HashSetKey,
// so the object of this class can be used
// as keys in HashSet
//
// Interface of class Word:
//
class Word: public HashSetKey {
    char* str;
    int   len;
    int capacity;
public:
    Word();
    Word(const char* s, int l = (-1));  // (-1) means "undefined"
    Word(const Word& w);
    virtual ~Word() { delete[] str; }
    virtual Word* clone() const { return new Word(*this); }

    int length() const { return len; }
    int size() const { return len; }

    // Convertor to C-string
    operator const char *() { return str; }
    const char* getString() const { return str; }

    virtual bool operator==(const HashSetKey& s) const {
        return (strcmp(str, ((Word&) s).str) == 0);
    }

    virtual int hashValue() const;
};

// Implementation of class Word
Word::Word():
    HashSetKey(),
    str(0),
    len(0),
    capacity(0)
{}

Word::Word(const char* s, int l):
    HashSetKey(),
    str(0),
    len(0),
    capacity(0)
{
    assert(s != 0);
    if (l < 0)
        l = strlen(str);
    len = l;
    capacity = len + 1;
    str = new char[capacity];
    memmove(str, s, len);
    str[len] = 0;
}

Word::Word(const Word& w):
    HashSetKey(w),
    str(0),
    len(w.len),
    capacity(w.capacity)
{
    if (w.len > 0) {
        str = new char[w.capacity];
        memmove(str, w.str, capacity);
    }
}

int Word::hashValue() const {
    const int HASH_FACTOR = 1021;   // Prime number

    // Calculate polynomial (...(s0*x + s1)*x + ...)*x + sn
    // where x = HASH_FACTOR
    int hash = 0;
    for (int i = 0; i < len; i++) {
        hash *= HASH_FACTOR;
        hash += (int) str[i];
    }

    // Make the hash value positive: clear the sign bit
    hash &= 0x7fffffff;
    return hash;
}

// class Integer represents the number of inclusions of word in text
class Integer: public HashSetValue {
public:
    int number;
public:
    Integer(): 
        HashSetValue(),
        number(0) 
    {}

    Integer(const Integer& i): 
        HashSetValue(),
        number(i.number) 
    {}

    Integer(int n): 
        HashSetValue(),
        number(n) 
    {}

    virtual ~Integer() {}

    virtual Integer* clone() const {
        return new Integer(*this);
    }
};

int main() {
    char line[256];
    HashSet set(5009);  // 5009 is a prime number (the hashtable size)

    printHelp();

    while (true) {
        printf("Command>");
        fgets(line, 254, stdin);
        int len = strlen(line);
        if (len == 0)
            continue;
        // Remove "\r\n" at the end of line
        if (line[len - 1] == '\n')
            --len;
        if (len > 0 && line[len - 1] == '\r')
            --len;
        if (len == 0)
            continue;

        // Extract a command
        int i = 0;
        while (i < len && isspace(line[i]))
            ++i;        // Skip space at the beginning of line
        if (i >= len)
            continue;

        if (line[i] == 'q') {
            // Quit
            break;      
        } else if (line[i] == 'a') {
            // Add/modify a record

            while (i < len && isalpha(line[i])) ++i; // Skip a command
            if (i >= len-2 || !isspace(line[i]))
                continue;       // Incorrect line
            while (i < len && isspace(line[i]))
                ++i;        // Skip a space after a command
            if (i >= len)
                continue;   // Incorrect command
            int nameBeg = i;

            // Look for the beginning of the phone number
            int number = 0;
            while (i < len && !isdigit(line[i]))
                ++i;
            if (i < len && isdigit(line[i]))
                number = atoi(line + i);

            // Look for the after-end of name
            while (i > nameBeg + 1 && isspace(line[i - 1]))
                --i;
            int nameLen = i - nameBeg;
            if (nameLen <= 0)
                continue;       // Incorrect line

            Word name(line + nameBeg, nameLen);
            Integer phoneNumber(number);
            set.add(&name, &phoneNumber);

        } else if (line[i] == 'r') {
            // Remove a record
            int i = 0;
            while (i < len && isalpha(line[i])) ++i; // Skip a command
            if (i >= len-2 || !isspace(line[i]))
                continue;       // Incorrect line
            while (i < len && isspace(line[i]))
                ++i;    // Skip a space after a command

            if (i >= len)
                continue;   // Incorrect line

            Word name(line + i, len - i);
            set.remove(&name);

        } else if (line[i] == 's') {
            // Show the contents of phone book

            HashSet::const_iterator i = set.begin();
            HashSet::const_iterator e = set.end();
            while (i != e) {
                const HashSet::Pair& pair = *i;
                const Word* name = (const Word *) pair.key;
                int number = ((const Integer *) pair.value)->number;
                printf("\t%s\t%d\n", name->getString(), number);
                ++i;
            }
        } else {
            printHelp();
        } // end if
    } // end while

    return 0;
}

static void printHelp() {
    printf("Telephone Book (based on HashSet)\n");
    printf(
        "Commands:\n"
        "\t\tadd name number,\n"
        "\t\tremove name,\n"
        "\t\tshow,\n"
        "\t\thelp,\n"
        "\t\tquit.\n"
        "\tThe \"add\" command modifies a number,\n"
        "\tif name is already in the phone book.\n"
    );
}
