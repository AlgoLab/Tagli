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


using namespace std;



TightString::TightString(const std::string& s) {
	TightString::import(s);
}

void TightString::import(const std::string& s) {
	fingerprint = encode(s);
}

std::string TightString::unimport(void) const {
	return (decode(fingerprint, KMER_LENGTH));
}

Fingerprint encode(const std::string& k, const size_t start, const size_t len) {
	const size_t end= std::min(start + len, k.length());
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

LongTightString::LongTightString(const std::string& s) {
	LongTightString::import(s);
}


void LongTightString::import(const std::string& s) {
	length = s.length();
	for (unsigned short int i=0; i<WORDS_IN_LONGSTRING; i++) { kmer[i]=0; }
	for (unsigned short int i=0, chunk= 0;
		  i<length;
		  i+= KMER_LENGTH, ++chunk) {
	  kmer[chunk] = encode(s, i, KMER_LENGTH);
	}
}

std::string LongTightString::unimport(void) {
	std::string s(length, ' ');
	for (unsigned short int i=0, chunk= 0;
		  i<length;
		  i+= KMER_LENGTH, ++chunk) {
		unsigned short int chunk_length = min (length-i, KMER_LENGTH);
		s.replace(i, KMER_LENGTH, decode(kmer[chunk], chunk_length));
	}
	return s;
}



