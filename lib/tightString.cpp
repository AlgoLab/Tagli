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


#include "tightString.hpp"
#include <iostream>
#include <fstream>
#include <assert.h>
#include <vector>
#include <algorithm>
#include <bitset>
#include <boost/foreach.hpp>
#include <functional>
#include <boost/bind.hpp>

using namespace std;



void TightString::import(const std::string& s) {
    fingerprint = encode(s);
}

std::string TightString::unimport(void) const {
    return (decode(fingerprint, KMER_LENGTH));
}

Fingerprint encode(const std::string& k, const size_t start, const size_t len) {
    const size_t end= std::min(start + len, k.length());
    if (end - start > KMER_LENGTH ) {
        cout << k << " " << k.length() << " " << start << " " << end << " " << len << "\n";
    }
    assert(end - start <= KMER_LENGTH );
    Fingerprint fingerprint = 0;
    for (size_t i= start; i<end; i++) {
        fingerprint = fingerprint << 2;
        fingerprint += encodeNucleotide(k[i]);
    }
    return fingerprint;
}



std::string decode(const Fingerprint f, const unsigned short int length) {
    string s(length, ' ');
    Fingerprint _f(f);
    for (unsigned short int i= length; i>0;) {
        --i; // 'i' is unsigned !!
        s[i]= decodeNucleotide(_f & 0x3);
        _f = _f >> 2;
    }
    return s;
}



Nucleotide decodeNucleotide(NucleotideBits nucleotide) {
    const static char map[]= {'A', 'C', 'G', 'T'};
    return map[0x3 & nucleotide];
}

NucleotideBits encodeNucleotide(Nucleotide nucleotide) {
    const static char map[]={0,0,1,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,3};
#ifndef NDEBUG
#warning "The following assertion is quite slow: consider to disable assertions (flag -DNDEBUG) or to comment out the code."
    assert((nucleotide == 'A') || (nucleotide == 'a') ||
           (nucleotide == 'C') || (nucleotide == 'c') ||
           (nucleotide == 'G') || (nucleotide == 'g') ||
           (nucleotide == 'T') || (nucleotide == 't') ||
           (nucleotide == 'N') || (nucleotide == 'n'));
#endif
    return map[(size_t)nucleotide-'A'];
}

/*

  void Kmer::import(std::string k) {
  fingerprint = 0;
  assert(k.length() == KMER_LENGTH );
  for (unsigned int i=0; i<KMER_LENGTH; i++) {
  fingerprint = fingerprint << 2;
  fingerprint += importNucleotide(k[i]);
  }
  }


  std::string Kmer::unimport(void) {
  string s;
  Fingerprint f = fingerprint;
  for (unsigned int i=0; i<KMER_LENGTH; i++) {
  unsigned short int x = (f & _highest_2_bits_bitmask_) >> (TAGLI_WORD_SIZE - 2); // get the highest 2 bits
  s.push_back(exportNucleotide(x));
  f = f << 2;
  }
  return s;
  }


*/


void LongTightString::import(const std::string& s) {
    assert(s.length() <= LONGTIGHTSTRING_LEN);
    _length = s.length();
    _sequence.reset();
    for (size_t i=0; i<_length; i++) {
        NucleotideBits t = encodeNucleotide(s[_length-i-1]);
        _sequence[2*i] = (t & 0x1);
        _sequence[2*i+1] = (t & 0x2);
    }
}

std::string LongTightString::unimport(void) const {
    std::string s(_length, ' ');
    NucleotideBits t;
    for (uint32_t i=0; i < _length; ++i) {
        t= _sequence[2*(_length-i-1)] + (_sequence[2*(_length-i-1)+1]*2);
        s[i]= decodeNucleotide(t & 0x3);
    }
    return s;
}


len_t overlap(Fingerprint f1, Fingerprint f2) {
    for (uint16_t len= KMER_LENGTH, d=2; len>0; d+=2, len-- ) {
        if (f1 == f2)
            return len;
        f1 = (f1 << 2);
        f2 = ((f2 >> d) << d);
    }
    return 0;
}

len_t overlap(const TightString & str1, const TightString & str2) {
    return overlap(str1.fingerprint, str2.fingerprint);
}



len_t overlap(const LongTightString & str1, const LongTightString & str2) {
    len_t len = std::min(str1.length(), str2.length());
    LongTightStringSequence s1 = str1.sequence();
    LongTightStringSequence s2 = str2.sequence();
    s1 <<= (LONGTIGHTSTRING_BITSET_LEN-2*len);
    s1 >>= (LONGTIGHTSTRING_BITSET_LEN-2*len);
    s2 >>= 2*(str2.length() - len);
    for (; len>0; len-- ) {
        if (s1 == s2)
            return len;
        s2 >>= 2;
        s1[(2*len)-1] = false;
        s1[(2*len)-2] = false;
    }
    return 0;
}


// void find_largest_common_substring(Match & m, const LongTightString & s1, const LongTightString & s2) {
//  const LongTightString TightString
// }

Fingerprint reverse_complement(const Fingerprint s) {
    Fingerprint v = ~s;
//  assert(sizeof(v) == 64);
// swap odd and even bits
//  v = ((v >> 1) & 0x5555555555555555) | ((v & 0x5555555555555555) << 1);
// swap consecutive pairs
    v = ((v >> 2) & 0x3333333333333333) | ((v & 0x3333333333333333) << 2);
// swap nibbles ...
    v = ((v >> 4) & 0x0F0F0F0F0F0F0F0F) | ((v & 0x0F0F0F0F0F0F0F0F) << 4);
// swap bytes
    v = ((v >> 8) & 0x00FF00FF00FF00FF) | ((v & 0x00FF00FF00FF00FF) << 8);
// swap 2-byte long pairs
    v = ( v >> 16 & 0x0000FFFF0000FFFF) | ((v & 0x0000FFFF0000FFFF) << 16);
// swap 4-byte long pairs
    v = ( v >> 32             ) | ( v               << 32);
    return v;
}


static char _complement(char ch)
{
    switch(ch) {
    case 'A':
        return 'T';
    case 'T':
        return 'A';
    case 'C':
        return 'G';
    case 'G':
        return 'C';
    }
    return 'A';
}

std::string reverse_complement(const std::string& seq)
{
    std::string r(seq);
    std::reverse(r.begin(), r.end());
    std::transform(r.begin(), r.end(), r.begin(), _complement);

    return r;
}

/*
  Compute the length of the Lcp, as the actual prefix is trivial to compute
*/
unsigned int longest_common_prefix(const vector<LongTightString> & strings) {
    LongTightString old = strings.front();
    unsigned int len = old.length();
    BOOST_FOREACH( LongTightString s, strings ) {
        len = min(len, longest_common_prefix(s, old));
        if (len == 0) { break;}
        old = s;
    }
    return len;
}

unsigned int longest_common_prefix(const LongTightString& s1, const LongTightString& s2) {
    LongTightStringSequence diff = s1.sequence() ^ s2.sequence();
    unsigned short int pos = 0;
    for (; pos < s1.length() && !diff[2*pos] && !diff[2*pos+1]; pos++) {}
    return pos;
}

/*
  merge two LongTightStringSequence
*/
LongTightStringSequence merge(const LongTightStringSequence & s1, const LongTightStringSequence & s2, const len_t ov) {
    return((s1 << (2*ov))|s2);
}

LongTightStringSequence merge(const LongTightString & s1, const LongTightString & s2) {
    len_t ov=overlap(s1, s2);
    return(merge(s1.sequence(),s2.sequence(),ov));
}

/*
  Find the longest suffix of s1 that is a substring of s2
*/
Match find_longest_suffix_substring(const LongTightString & s1, const LongTightString & s2) {
  const std::string ss1(s1.unimport());
  const std::string ss2(s2.unimport());
  const len_t ls1= ss1.length();
  const len_t ls2= ss2.length();
  const len_t max_len = min(ls1, ls2);
  for (len_t len= max_len; len>0; --len) {
	 for (len_t i2= 0; i2<=ls2-len; ++i2) {
		if (ss1.compare(ls1-len, len, ss2, i2, len)==0) {
		  return Match(ls1-len, i2, len);
		}
	 }
  }
  return Match(0, 0, 0);
}

/*
  Find the longest prefix of s1 that is a substring of s2
*/
Match find_longest_prefix_substring(const LongTightString & s1, const LongTightString & s2) {
  const std::string ss1(s1.unimport());
  const std::string ss2(s2.unimport());
  const len_t ls1= ss1.length();
  const len_t ls2= ss2.length();
  const len_t max_len = min(ls1, ls2);
  for (len_t len= max_len; len>0; --len) {
	 for (len_t i2= 0; i2<=ls2-len; ++i2) {
		if (ss1.compare(0, len, ss2, i2, len)==0) {
		  return Match(0, i2, len);
		}
	 }
  }
  return Match(0, 0, 0);
}

Match find_largest_common_substring(const LongTightString & s1, const LongTightString & s2) {
  const std::string ss1(s1.unimport());
  const std::string ss2(s2.unimport());
  const len_t ls1= ss1.length();
  const len_t ls2= ss2.length();
  const len_t max_len = min(ls1, ls2);
  for (len_t len= max_len; len>0; --len) {
	 for (len_t i1= 0; i1<=ls1-len; ++i1) {
		for (len_t i2= 0; i2<=ls2-len; ++i2) {
		  if (ss1.compare(i1, len, ss2, i2, len)==0) {
			 return Match(i1, i2, len);
		  }
		}
	 }
  }
  return Match(0, 0, 0);
}

std::vector<LongTightString> build_suffixes(const LongTightString& s) {
  LongTightString x(s);
    std::vector<LongTightString> sa;
    sa.reserve(x.length()+1);
    const len_t max = x.length();
    for(len_t len = 0; len < max; len++) {
        sa.push_back(x);
        x.shift();
    }
    sa.push_back(x);
    std::reverse(sa.begin(), sa.end());
    return sa;
}
const std::vector<LongTightString> build_prefixes(LongTightString x) {
    std::vector<LongTightString> sa;
    sa.reserve(x.length()+1);
    len_t max = x.length();
    for(len_t len = 0; len <= max; len++) {
        sa.push_back(x);
        x.pop();
    }
    std::reverse(sa.begin(), sa.end());
    return sa;
}

LongTightString LongTightString::prefix(const len_t length) const {
    len_t actual_len = std::min(length, _length);
    len_t movement = _length-actual_len;
    LongTightString result(_sequence>>(2*movement), actual_len);
    return result;
}
LongTightString LongTightString::suffix(const len_t length) const {
    len_t actual_len = std::min(length, _length);
    LongTightString result((_sequence<<(LONGTIGHTSTRING_BITSET_LEN-2*actual_len))>>(LONGTIGHTSTRING_BITSET_LEN-2*actual_len), actual_len);
    return result;
}

bool LongTightString::is_prefix(const LongTightString & s) {
    if (s.length() <= _length) {
        LongTightString my_prefix = this->prefix(s.length());
        return (my_prefix == s);
    }
    return false;
}
bool LongTightString::is_suffix(const LongTightString & s) {
    if (s.length() <= _length) {
        LongTightString my_suffix = this->suffix(s.length());
        return (my_suffix == s);
    }
    return false;
}

NucleotideBits LongTightString::pop() {
    NucleotideBits t= _sequence[0] + (_sequence[1]*2);
    _sequence >>= 2;
    --_length;
    return t;
}
NucleotideBits LongTightString::shift() {
    NucleotideBits x = _sequence[2*(_length-1)] + _sequence[2*(_length-1)+1]*2;
    _sequence[2*(_length-1)] = 0;
    _sequence[2*(_length-1)+1] = 0;
    --_length;
    return x;
}

len_t longest_left_overlap(const LongTightString & s, const std::vector<LongTightString>  & v) {
    len_t max = 0;
    for (auto x : v) {
        len_t actual = overlap(s, x);
        if (actual>max)
            max = actual;
    }
    return max;
    // my_overlap = boost::bind(overlap, s, _1)(x);
    // std::vector<len_t> values = for_each(v.begin(), v.end(), overlap);
    // return *max_element(values.cbegin(), values.cend());;
}
len_t longest_right_overlap(const LongTightString & s, const std::vector<LongTightString>  & v) {
    len_t max = 0;
    for (auto x : v) {
        len_t actual = overlap(x, s);
        if (actual>max)
            max = actual;
    }
    return max;
}

LongTightString LongTightString::pop(len_t length) {
    len_t actual_len = min(length, _length);
    LongTightString result = this->suffix(actual_len);
    _sequence >>= (2*actual_len);
    _length -= actual_len;
    return result;
}
LongTightString LongTightString::shift(len_t length) {
    len_t actual_len = std::min(length, _length);
    LongTightString result = this->prefix(actual_len);
    _sequence <<= (2*(LONGTIGHTSTRING_LEN - _length + actual_len));
    _sequence >>= (2*(LONGTIGHTSTRING_LEN - _length + actual_len));
    _length -= actual_len;
    return result;
}
void LongTightString::push(const LongTightString & s) {
    LongTightStringSequence new_sequence = s.sequence();
    len_t new_length = s.length();
    _sequence <<= (2*new_length);
    _sequence |= new_sequence;
    _length += new_length;
}
void LongTightString::unshift(const LongTightString & s) {
    LongTightStringSequence new_sequence = s.sequence();
    len_t new_length = s.length();
    new_sequence <<= (2*_length);
    _sequence |= new_sequence;
    _length += new_length;
}

bool compare(std::string s1, std::string s2, const Match & m1, const Match & m2, bool debug=false) {
    if ((m1.length == m2.length) &&
        (m1.begin1 == m2.begin1) &&
        (m1.begin2 == m2.begin2))
        return true;
    if (m1.length == m2.length &&
        s1.compare(m1.begin1, m1.length, s2, m1.begin2, m1.length) == 0 &&
        s1.compare(m2.begin1, m2.length, s2, m2.begin2, m2.length) == 0)
        return true;
    int diff = m1.begin2-m2.begin2;
    if (debug) {
        cout << diff << endl;
        cout << "m1:"  << "begin1: " << m1.begin1 << ", begin2: " << m1.begin2 << ", length: " << m1.length << endl;
        cout << "m2:"  << "begin1: " << m2.begin1 << ", begin2: " << m2.begin2 << ", length: " << m2.length << endl ;
        cout << "s1:" << s1 << "(" << s1.size() << ")" << endl;
        cout << "s2:" << s2 << "(" << s2.size() << ")" << endl;
        cout << "s1m1:" << s1.substr(m1.begin1, m1.length) << "/" << endl;
        cout << "s2m1:" << s2.substr(m1.begin2, m1.length) << "/" <<  endl << endl;
        cout << "s1m2:" << s1.substr(m2.begin1, m2.length) << "/" <<  endl;
        cout << "s2m2:" << s2.substr(m2.begin2, m2.length) << "/" <<  endl;
        cout << (m1.length == m2.length);
        cout << (m1.begin1 == m2.begin1);
        cout << (m1.begin2 == m2.begin2);
        cout <<         s1.compare(m1.begin1, m1.length, s1, m2.begin1, m2.length);
        cout <<         s2.compare(m1.begin2, m1.length, s2, m2.begin2, m2.length);
        cout << "\n";


    }
    return false;
}
