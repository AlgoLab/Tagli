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


#include "globals.hpp"
#include "tightString.hpp"
#include <iostream>
#include <fstream>
#include <assert.h>


using namespace std;



TightString::TightString(std::string s) {
	TightString::import(s);
}

void TightString::import(std::string k) {
	fingerprint = 0;
	assert(k.length() == KMER_LENGTH );
	for (unsigned int i=0; i<KMER_LENGTH; i++) {
		fingerprint = fingerprint << 2;
		fingerprint += importNucleotide(k[i]);
	}
}


std::string TightString::unimport(void) {
	string s;
	Fingerprint f = fingerprint;
	for (unsigned int i=0; i<KMER_LENGTH; i++) {
		unsigned short int x = (f & _highest_2_bits_bitmask_) >> (TAGLI_WORD_SIZE - 2); // get the highest 2 bits
		// cout <<hex<< f << "-" << _highest_2_bits_bitmask_ << " " << (f & _highest_2_bits_bitmask_)   << " " << ((f & _highest_2_bits_bitmask_) >> (TAGLI_WORD_SIZE - 2)) << " " << TAGLI_WORD_SIZE  << "\n";
		// cout <<hex<< ~(~ (Fingerprint)0 >> 2) << " " << (~ (Fingerprint)0 >> 2) << " " << ~ (Fingerprint)0  << " " << "\n";
		s.push_back(exportNucleotide(x));
		// cout << x << "-" << f << " " << s << " " << exportNucleotide(x) << "\n";
		f = f << 2;
	}
	return s;
}
	
	
    
Nucleotide TightString::exportNucleotide(NucleotideBits nucleotide) {
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

NucleotideBits TightString::importNucleotide(Nucleotide nucleotide) {
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





