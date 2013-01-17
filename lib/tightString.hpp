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
#include <vector>
#include <functional>
#include <sstream>

typedef unsigned short int NucleotideBits;
// Just a sequence string, but with just two bits per character
typedef unsigned char Nucleotide;
typedef uint64_t Fingerprint;
typedef std::string Read;
#define LONGTIGHTSTRING_LEN 96
#define LONGTIGHTSTRING_BITSET_LEN 192
typedef std::bitset<LONGTIGHTSTRING_BITSET_LEN> LongTightStringSequence;




static const NucleotideBits Adenine = 0;
static const NucleotideBits Cytosine = 1;
static const NucleotideBits Guanine = 2;
static const NucleotideBits Thymine = 3;


static const Fingerprint _highest_2_bits_bitmask_ = ~(~ (Fingerprint)0 >> 2);
#define GET_2BITS(x,pos)   (x << pos) >> (KMER_LENGTH -2 )

Nucleotide decodeNucleotide(const NucleotideBits);
NucleotideBits encodeNucleotide(const Nucleotide);
Fingerprint encode(const std::string& k, const size_t start=0, const size_t len=std::string::npos);
std::string decode(const Fingerprint f, const unsigned short int length = KMER_LENGTH);
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


typedef uint16_t len_t;

/* The LongTightString class is a compact representation of a DNA string no longer than LONGTIGHTSTRING_LEN bps
   using a LONGTIGHTSTRING_BITSET_LEN bits bitset.

   A string shorter than LONGTIGHTSTRING_LEN characters uses the positions with smallest possible index.
   Moreover in position 0 is stored the last character of the string, so that applying the method sequence()
   gives a string that can be printed as an intuitive representation of the bitset.
*/

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

    LongTightString(const LongTightString& lts)
        :_length(lts._length), _sequence(lts._sequence)
        {}

    LongTightString(const LongTightStringSequence& s, const len_t l)
        :_length(l), _sequence(s)
        {}

    len_t length() {
        return _length;
    }

    len_t length() const {
        return _length;
    }

    LongTightStringSequence& sequence() {
        return _sequence;
    }

    const LongTightStringSequence& sequence() const {
        return _sequence;
    }
    bool operator== (const LongTightString &other) {
        return (_length==other.length() && _sequence == other.sequence());
    }

    void update(const LongTightStringSequence& s, const len_t l) {
        // Enforce the maximum length of a string
        if (l <= LONGTIGHTSTRING_LEN) {
            _sequence=s;
            _length=l;
        }
    }


    bool compare(const LongTightString & s) const {
        return (_length == s.length() &&
                _sequence == s.sequence());
    }


    std::string unimport(void) const;
    void import(const std::string&);
    // single character version
    // all update the object
    NucleotideBits pop();
    void push(NucleotideBits);
    NucleotideBits shift();
    void unshift(NucleotideBits);

    // multiple characters version
    LongTightString pop(len_t);
    void push(const LongTightString &);
    LongTightString shift(len_t len);
    void unshift(const LongTightString &);
    LongTightString reverse_complement();

    LongTightString prefix(const len_t length) const;
    LongTightString suffix(const len_t length) const;
    bool is_prefix(const LongTightString & s);
    bool is_suffix(const LongTightString & s);
    LongTightString substring(const len_t begin, const len_t end) {return (this->prefix(end+1)).suffix(end-begin);};

    std::string dump() {
        std::stringstream result;
        result << "sequence: " << this->unimport() << ",\nlength: " << _length << ",\nencoded " ;
        for (int i=LONGTIGHTSTRING_BITSET_LEN-1; i >= 0 ; i--)
            result << _sequence[i];
        return result.str();
    }

};


/* Compute the suffixes or prefixes of a LongThightString
   the suffixes are ordered from shortest to longest,
   so that at position i we can find the i-long suffix
*/
std::vector<LongTightString> build_suffixes(const LongTightString& s);
const std::vector<LongTightString> build_prefixes(LongTightString x);

/*
  A match is a common substring between two LongTightString.
  We store the first position of the substring in each string and the length of the substring
*/
class Match {
public:
    len_t begin1;
    len_t begin2;
    len_t length;

    explicit Match(const len_t p1, const len_t p2, const len_t l)
        :begin1(p1), begin2(p2), length(l)
        {}

    std::string dump() {
        std::stringstream result;
        result << "begin1: " << begin1 << ", begin2: " << begin2 << ", length: " << length ;
        return result.str();
    }

};
bool compare(std::string s1, std::string s2, const Match & m1, const Match & m2, bool debug);


Match find_longest_suffix_substring(const LongTightString & s1, const LongTightString & s2);
Match find_longest_prefix_substring(const LongTightString & s1, const LongTightString & s2);
Match find_largest_common_substring(const LongTightString & s1, const LongTightString & s2);
len_t overlap(const TightString &, const TightString &);
len_t overlap(const LongTightString &, const LongTightString &);
len_t longest_left_overlap(const LongTightString & s, const std::vector<LongTightString>  & v);
len_t longest_right_overlap(const LongTightString & s, const std::vector<LongTightString>  & v);
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

/*
  compute the length of the longest common prefix/suffix of an array of
  LongTightString(s) and of two LongTightStrings
*/
unsigned int longest_common_prefix(const std::vector<LongTightString>& strings);
unsigned int longest_common_suffix(const std::vector<LongTightString>& strings);
unsigned int longest_common_prefix(const LongTightString& s1, const LongTightString& s2);
unsigned int longest_common_suffix(const LongTightString& s1, const LongTightString& s2);
//Match find_longest_common_substring(const LongTightString& s1, const LongTightString& s2);

/*
  merge two LongTightStringSequences
*/

LongTightStringSequence merge(const LongTightStringSequence& s1, const LongTightStringSequence& s2, const len_t ov);
LongTightStringSequence merge(const LongTightString& s1, const LongTightString& s2);

#endif
