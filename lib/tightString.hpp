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

#include <iostream>
#include <string>
#include "globals.hpp"
#include <stdint.h>
#include <cstdint>

typedef unsigned short int NucleotideBits;
// Just a sequence string, but with just two bits per character
typedef unsigned char Nucleotide;
typedef uint64_t Fingerprint;
typedef std::string Read;

static const NucleotideBits Adenine = 0;
static const NucleotideBits Cytosine = 1;
static const NucleotideBits Guanine = 2;
static const NucleotideBits Thymine = 3;


static const Fingerprint _highest_2_bits_bitmask_ = ~(~ (Fingerprint)0 >> 2);

class TightString {
public:
    Fingerprint fingerprint;

    TightString(std::string);
    
    std::string unimport(void);

	void import(const std::string);


private:
    Nucleotide exportNucleotide(NucleotideBits);
    NucleotideBits importNucleotide(Nucleotide);
};


#endif
