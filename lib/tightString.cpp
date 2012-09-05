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



TightString::TightString(const std::string *s) {
	TightString::import(*s);
}

void TightString::import(const std::string s) {
	fingerprint = encode(s);
}

std::string TightString::unimport(void) {
	return (decode(fingerprint, KMER_LENGTH));
}

Fingerprint encode(std::string k) {
	assert(k.length() <= KMER_LENGTH );
	Fingerprint fingerprint = 0;
	unsigned short int length = k.length();
	for (unsigned int i=0; i<length; i++) {
		fingerprint = fingerprint << 2;
		fingerprint += encodeNucleotide(k[i]);
	}
	fingerprint = fingerprint << (2 * (KMER_LENGTH - length));
	return fingerprint;
}



std::string decode(Fingerprint f, unsigned short int length) {
	string s;
//	assert(length <= KMER_LENGTH );
	for (unsigned int i=0; i<length; i++) {
		unsigned short int x = (f & _highest_2_bits_bitmask_) >> (TAGLI_WORD_SIZE - 2); // get the highest 2 bits
		// cout <<hex<< f << "-" << _highest_2_bits_bitmask_ << " " << (f & _highest_2_bits_bitmask_)   << " " << ((f & _highest_2_bits_bitmask_) >> (TAGLI_WORD_SIZE - 2)) << " " << TAGLI_WORD_SIZE  << "\n";
		// cout <<hex<< ~(~ (Fingerprint)0 >> 2) << " " << (~ (Fingerprint)0 >> 2) << " " << ~ (Fingerprint)0  << " " << "\n";
		s.push_back(decodeNucleotide(x));
//		cout << x << "-" << f << " " << s << " " << decodeNucleotide(x) << "\n";
		f = f << 2;
	}
//	cout << f << "-" << length << " " << KMER_LENGTH - length << " " <<  "\n";
	assert(s.length() == length);
	return s;
}
	
	
    
Nucleotide decodeNucleotide(NucleotideBits nucleotide) {
	switch (nucleotide) {
	case 0:
		return 'A';
	case 1:
		return 'C';
	case 2:
		return 'G';
	case 3:
		return 'T';
	}
	return '?';
}

NucleotideBits encodeNucleotide(Nucleotide nucleotide) {
	switch (toupper(nucleotide)) {
		case 'A':
		case 'N':
			return Adenine;
		case 'C':
			return Cytosine;
		case 'G':
			return Guanine;
		case 'T':
			return Thymine;
	}
	return Adenine;
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

LongTightString::LongTightString(const std::string *s) {
	LongTightString::import(*s);
}


void LongTightString::import(std::string s) {
	length = s.length();
	for (unsigned short int i=0; i<WORDS_IN_LONGSTRING; i++) { kmer[i]=0; }
	unsigned short int chunk = 0;
	while (s.length()>0) {
		kmer[chunk] = encode(s.substr(0,KMER_LENGTH));
		chunk++;
		s.erase(0,KMER_LENGTH);
	}
}

std::string LongTightString::unimport(void) {
	int l = length;
	unsigned short int chunk = 0;
	std::string s("");
	while (l > 0) {
		unsigned short int chunk_length = min (l, KMER_LENGTH);
		// cout << s << " " << chunk << " " << l << " " << chunk_length << "\n";
		// cout << s << " " << s.length()  << "\n";
		s.append(decode(kmer[chunk],chunk_length));
		chunk++;
		l -= chunk_length;
	}
	// cout << s << s.length() << "\n";
	return s;
}



