//
// Test of class HashSet:
// Calculate the set of all words in a text,
// and for every word calculate a number of its inclusions
// in the text.
//
// The program reads a text either from a file
// or from standard input stream, depending on how it is called:
//
// 1) the command
//        wordfreq input_file
//    reads the text from "input_file";
//
// 2) the command
//        wordfreq
//    without arguments reads the text from standard input.
//    It allows, for instance, the following possibility (in Unix):
//    Define the set of words in all Unix standard include files.
//    It can be done by the command
//        cat /usr/include/*.h | ./wordfreq
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
    void initialize();

    // Add a character to the end of word
    Word& operator+=(int c);

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
    assert(str != 0);
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

void Word::initialize() {
    len = 0;
    if (capacity > 0)
        str[0] = 0;
}

// Add a character to the end of word
Word& Word::operator+=(int c) {
    if (capacity <= len+1) {
        // Allocate additional space
        int extent = capacity / 8;
        if (extent < 16)
            extent = 16;
        char* new_str = new char[capacity + extent];
        memmove(new_str, str, len);
        delete[] str;
        capacity += extent;
        str = new_str;
    }
    str[len] = (char) c;
    ++len;
    str[len] = 0;
    return *this;
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

int main(int argc, char *argv[]) {
    HashSet set(5009);  // 5009 is a prime number (the hashtable size)
    FILE* input;
    if (argc > 1) {
        if (*argv[1] == '-') {
            printHelp();
            return 0;
        }
        input = fopen(argv[1], "r");
        if (input == 0) {
            perror("Cannot open an input file");
            return 1;
        }
    } else {
        input = stdin;  // Standard input
    }

    Word currentWord;
    bool wasLetter = false;
    bool endOfFileDetected = false;
    while (!endOfFileDetected) {
        int c = fgetc(input);
        if (isalpha(c)) {       // If this is an English letter
            currentWord += c;   // Add a letter to a word
            wasLetter = true;
        } else {
            if (wasLetter) {
                // The end of current word detected
                if (set.contains(&currentWord)) {
                    Integer* val = (Integer*) set.value(&currentWord);
                    ++(val->number);    // Increment a number of inclusions
                } else {
                    Integer unit(1);
                    set.add(&currentWord, &unit);
                }
                currentWord.initialize();
            }
            wasLetter = false;
            endOfFileDetected = (c == EOF);
        }
    }

    // Print the set of words in the text and
    // define the most frequent word
    printf("The text contains the following words:\n");

    HashSet::const_iterator i = set.begin();
    HashSet::const_iterator e = set.end();
    Word mostFrequentWord;
    int numberOfMostFrequentWords = 0;
    while (i != e) {
        const HashSet::Pair& pair = *i;
        const Word* w = (const Word *) pair.key;
        int number = ((const Integer *) pair.value)->number;
        printf(
            "%d\t%s\n", number, w->getString()
        );

        // Define the most frequent word
        if (number > numberOfMostFrequentWords) {
            numberOfMostFrequentWords = number;
            mostFrequentWord = *w;
        }
        ++i;
    }

    printf(
        "----\n"
        "Number of different words in the text = %d\n",
        set.size()
    );

    printf(
        "The most frequent word is \"%s\", included %d times.\n",
        mostFrequentWord.getString(), numberOfMostFrequentWords
    );

    return 0;
}

static void printHelp() {
    printf(
        "Calculate the set of all words in a text,\n"
        "and for every word calculate a number of its inclusions\n"
        "in the text.\n"
        "Usage:\n"
        "    wordfreq [input_file]\n"
        "The program reads a text either from a file\n"
        "or from standard input stream, depending on how it is called.\n\n"
        "EXAMPLES:\n"
        "1) the command\n"
        "       ./wordfreq input_file\n"
        "   reads the text from \"input_file\";\n"
        "\n"
        "2) the Unix-command\n"
        "       cat /usr/include/*.h | ./wordfreq\n"
        "   defines the set of words in all standard include files.\n"
    );
}
