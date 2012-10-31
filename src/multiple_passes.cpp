/*
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


#include <tightString.hpp>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <zlib.h>
#include "lib/kseq.h"
#include "lib/tightString.hpp"
#include <boost/foreach.hpp>
#include <list>

#define Kf 4

#define BLOOM_FILTER_SIZE_BITS 28
#define BLOOM_FILTER_SIZE 2**BLOOM_FILTER_SIZE_BITS

using namespace std;



Fingerprint[Kf] extract_seeds(std::string s) {
	unsigned int l = s.length() - KMER_LENGTH;
	Fingerprint seeds[Kf];
	for (unsigned int i=0; i < Kf - 1; ++i) {
		seeds[i] = Fingerprint encode(s, (size_t) (l * i)/(Kf - 1), (size_t) KMER_LENGTH);
	}
	seeds[Kf - 1] = Fingerprint encode(s, (size_t) l, (size_t) KMER_LENGTH);
	return seeds[];
}


int main(void)
{
	gzFile fp;
	kseq_t *seq;
	std::string inputfile("reads.fastq");
	Fingerprint seeds[Kf];

/*
  Pass #1

  The goal is to build the list of fingerprints appearing at least twice.
  
  Build a Bloom filter storing the seeds, so that we can determine if a certain fingerprint appears at least twice.
  A Bloom filter is, in our implementation, an array of BLOOM_FILTER_SIZE positions.
  Since we only want to throw away some fingerprints that are the result of errors, we can use a fast strategy,
  evene though it gives a higher false positive rate.

  We use an array of two bits, which can store up to 4 occurrences.

  We use the Murmur3 hash to associate to index inside the array to a Fingerprint.
  Only the most significant BLOOM_FILTER_SIZE_BITS bits of the hash are used, as the array size is less than 2^{32}
  */	
	
	fp = gzdopen(inputfile.c_str(), "r");
	seq = kseq_init(fp);
	list<Fingerprint> good_fingerprints;  

	{boolean bloom_table[BLOOM_FILTER_SIZE] {false};
	while (kseq_read(seq) >= 0) {
		std::string all_seq[2] = { seq->seq.s, reverse_complement(seq->seq.s) };
		BOOST_FOREACH( std::string x, all_seq ) {
			BOOST_FOREACH( Fingerprint f, extract_seeds(x)) {
				unsigned int pos=0;
				MurmurHash3_x86_32(&f, BLOOM_FILTER_SIZE_BITS, 0xB0F57EE3, &pos);
				if (bloom_table[pos][0])
					good_fingerprints.push_front(f);		
				else	
					bloom_table[pos][0] = true;
			}		
		}
	}}
	kseq_destroy(seq);
	gzclose(fp);

/*
  Pass #2
  
  Build a perfect hash storing only the Fingerprints occurring in at least two reads.

  We use the cmph library (cmph.sf.net) and the BDZ algorithm.
*/
	fp = gzdopen(inputfile.c_str(), "r");
	seq = kseq_init(fp);
	while (kseq_read(seq) >= 0) {
		std::string all_seq[2] = { seq->seq.s, reverse_complement(seq->seq.s) };
		BOOST_FOREACH( std::string x, all_seq ) {
			LongTightString t(x);
			for (unsigned int _i=0; _i < Kf; _i++ ) {
				populate_junctions(t, seeds(_i));
			}	
		}	
	}	
	printf("%d\t%d\t%d\n", n, slen, qlen);
	kseq_destroy(seq);
	gzclose(fp);


	return 0;
}
