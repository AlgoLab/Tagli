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


#include <iostream>
#include <fstream>
#include <assert.h>
#include <zlib.h>
#include "kseq.h"
#include "tightString.hpp"
#include <boost/foreach.hpp>
#include <list>
#include "MurmurHash3.h"
#include "cxxmph/mph_index.h"


#define Kf 4

#define BLOOM_FILTER_SIZE_BITS 28
#define BLOOM_FILTER_SIZE (1 << BLOOM_FILTER_SIZE_BITS)
KSEQ_INIT(gzFile, gzread)

using namespace std;

/*
  From each string (which is usually a read) we extract Kf equally spaced substrings of length KMER_LENGTH
*/
void extract_seeds(std::string s, Fingerprint seeds[]) {
	unsigned int l = s.length() - KMER_LENGTH;
	for (unsigned int i=0; i < 2*Kf -2 ; i+=2) {
		seeds[i] = encode(s, (size_t) (l * i)/(Kf - 1), (size_t) KMER_LENGTH);
		seeds[i+1] = reverse_complement(seeds[i]);
	}
	seeds[2*Kf - 2] = encode(s, (size_t) l, (size_t) KMER_LENGTH);
	seeds[2*Kf - 1] = reverse_complement(seeds[2*Kf - 2]);
}


int main(void)
{
	gzFile fp;
	kseq_t *seq;
	std::string inputfile("reads.fastq");
	Fingerprint seeds[2*Kf];

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
	
	fp = gzopen(inputfile.c_str(), "r");
	seq = kseq_init(fp);
	list<Fingerprint> good_fingerprints;  
	vector<bool> bloom_table;
	bloom_table.resize(BLOOM_FILTER_SIZE, false);
	
	while (kseq_read(seq) >= 0) {
		extract_seeds(seq->seq.s, seeds);
		BOOST_FOREACH( Fingerprint f, seeds) {
			unsigned int pos=0;
			MurmurHash3_x86_32(&f, BLOOM_FILTER_SIZE_BITS, 0xB0F57EE3, &pos);
			pos &= (BLOOM_FILTER_SIZE - 1);
			if (bloom_table[pos])
				good_fingerprints.push_front(f);		
			else	
				bloom_table[pos] = true;
		}		
	}
	bloom_table.resize(0); // reclaim space
	cout << good_fingerprints.size() << "\n";
	kseq_destroy(seq);
	gzclose(fp);

/*
  Build a perfect hash storing only the Fingerprints occurring in at least two reads.
  We use the cmph library (cmph.sf.net) and the BDZ algorithm.
  Each entry of the hash points to a putative junction site.
*/
	cxxmph::SimpleMPHIndex<Fingerprint> junction_index;
	if (!junction_index.Reset(good_fingerprints.begin(), good_fingerprints.end(), good_fingerprints.size())) { exit(-1); }

/*
  Pass #2

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
*/

	return 0;
}
