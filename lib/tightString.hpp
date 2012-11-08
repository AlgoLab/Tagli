/*
Copyright 2007, 2008 Daniel Zerbino (zerbino@ebi.ac.uk)
Copyright 2012-2012 Gianluca Della Vedova (http://gianluca.dellavedova.org)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see http://www.gnu.org/licenses/
*/


#ifndef _TIGHTSTRING_HH_
#define _TIGHTSTRING_HH_

#ifdef __clang__
namespace std { class type_info; }
#endif

#include <iostream>
#include <string>
#include <bitset>
#include "globals.hpp"
#include <stdint.h>
#include <cstdint>
#include <algorithm>



typedef unsigned short int NucleotideBits;
// Just a sequence string, but with just two bits per character
typedef unsigned char Nucleotide;
typedef uint64_t Fingerprint;
typedef std::string Read;
#define LONGTIGHTSTRING_LEN 192
typedef std::bitset<LONGTIGHTSTRING_LEN> LongTightStringSequence;

static const NucleotideBits Adenine = 0;
static const NucleotideBits Cytosine = 1;
static const NucleotideBits Guanine = 2;
static const NucleotideBits Thymine = 3;


static const Fingerprint _highest_2_bits_bitmask_ = ~(~ (Fingerprint)0 >> 2);
#define GET_2BITS(x,pos)   (x << pos) >> (KMER_LENGTH -2 )

Nucleotide decodeNucleotide(const NucleotideBits);
NucleotideBits encodeNucleotide(const Nucleotide);
Fingerprint encode(const std::string& k, const size_t start=0, const size_t len=std::string::npos);
std::string decode(const Fingerprint, const unsigned short int);
Fingerprint reverse_complement(const Fingerprint f);
//std::string reverse_complement(const std::string& s);
std::string reverse_complement(const std::string& seq);


/* The TightString class is a compact representation of a string no longer than KMER_LENGTH bps as a 2*KMER_LENGTH-bit
   unsigned integer, therefore using 2 bits for each character. With currently available architectures, that means that
   we store a 32bp string with a word.

   Notice that we need to also store the length of the encoded string, and that only the least significant bits are
   used, that is a string that is actually shorter than KMER_LENGTH characters will have leading zeroes
*/

class TightString {
public:
    Fingerprint fingerprint;

    TightString(const std::string& s)
                  :fingerprint(encode(s))
    {}


    std::string unimport(void) const;
    void import(const std::string&);


private:
};

/*
class Kmer: public TigthString {
    // length == KMER_LENGTH

};
*/


typedef unsigned short int len_t;

class LongTightString {
    // bit fields to store longer strings
private:
    len_t _length;
    LongTightStringSequence _sequence;
public:

    explicit LongTightString(const std::string& s)
        :_length(0), _sequence()
    {
        this->import(s);
    }

    explicit LongTightString(const LongTightString& lts)
        :_length(lts._length), _sequence(lts._sequence)
    {}

    len_t& length() {
        return _length;
    }

    const len_t& length() const {
        return _length;
    }

    LongTightStringSequence& sequence() {
        return _sequence;
    }

    const LongTightStringSequence& sequence() const {
        return _sequence;
    }

    std::string unimport(void);
    void import(const std::string&);
    // single character version
    NucleotideBits pop();
    void push(NucleotideBits);
    NucleotideBits shift();
    void unshift(NucleotideBits);
    // multiple characters version
    LongTightString pop(len_t);
    void push(LongTightString);
    LongTightString shift(len_t);
    void unshift(LongTightString);
	LongTightString reverse_complement();

};


/*
  A match is a common substring between two LongTightString.
  We store the first position of the substring in each string and the length of the substring
*/
typedef struct {
    len_t position1;
    len_t position2;
    len_t length;
} Match;

Match find_longest_suffix_substring(const LongTightString &, const LongTightString &);
Match find_longest_prefix_substring(const LongTightString &, const LongTightString &);
uint16_t overlap(const TightString &, const TightString &);
uint16_t overlap(const LongTightString &, const LongTightString &);

/*
  The suffix array struct is actually a generalized suffix array, where we store explictly the sequence of each suffix
typedef struct {
    LongTightString suffix;
    uint8_t original_sequence;
} SuffixArrayElement;

class SuffixArray {
public:
    array vector<SuffixArrayElement>;

    SuffixArray(const std::string&);
    SuffixArray(const LongTightString&);


}
*/

#endif
