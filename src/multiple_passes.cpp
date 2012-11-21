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
#include "junctions.hpp"
#include <boost/foreach.hpp>
#include "cxxmph/MurmurHash3.h"
#include "cxxmph/mph_index.h"
#include <set>

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
    std::string inputfile("reads.fastq.gz");
    Fingerprint seeds[2*Kf];
    /* seeds[] is an array of 2*Kf fingerprints extracted from a sequence s.
           Even-indexed elements refer to substrings of s, while odd-indexed elements refer to substrings of the
           reverse-and-complement of s
    */
//  std::vector<Junction> junctions;
    /* junctions[] is an array of all interesting splicing junctions.
           It is mainly an intermediate array required by the use of cxxmph
    */

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
    std::set<Fingerprint> good_fingerprints;
    vector<bool> bloom_table;
    bloom_table.resize(BLOOM_FILTER_SIZE, false);

    while (kseq_read(seq) >= 0) {
        extract_seeds(seq->seq.s, seeds);
        BOOST_FOREACH(Fingerprint f, seeds) {
            unsigned int pos=0;
            MurmurHash3_x86_32(&f, BLOOM_FILTER_SIZE_BITS, 0xB0F57EE3, &pos);
            pos &= (BLOOM_FILTER_SIZE - 1);
            if (bloom_table[pos]) {
			    cout << hex << f  << "\n";
                good_fingerprints.insert(f);
			}
            else
                bloom_table[pos] = true;
        }
    }
    bloom_table.resize(0); // reclaim space
    cout << "good_fingerprints size: " << good_fingerprints.size() << "\n";
    kseq_destroy(seq);
    gzclose(fp);

/*
  Build a perfect hash storing only the Fingerprints occurring in at least two reads.
  We use the cmph library (cmph.sf.net) and the BDZ algorithm.
  Each entry of the hash points to a position into the junctions array.
  Each element of junctions is a putative junction site.
*/
    cxxmph::SimpleMPHIndex<Fingerprint> junction_index;
    if (!junction_index.Reset(good_fingerprints.begin(), good_fingerprints.end(), good_fingerprints.size())) { exit(-1); }
    Junction *junctions = new Junction[junction_index.size()];
/*
  Pass #2

  Since cxxmph::SimpleMPHIndex returns an integer, we need an intermediate array of pointers to the putative junctions.
  Such array is called junction_pointers
*/

    fp = gzopen(inputfile.c_str(), "r");
    seq = kseq_init(fp);
    while (kseq_read(seq) >= 0) {
        for (unsigned int i=0; i<Kf; i++) {
			string s = seq->seq.s;
			string rs = reverse_complement(seq->seq.s);
			LongTightString lts_revcom(rs);
			for (unsigned int j=0; j<2; j++) {
				LongTightString lts_straigth(seq->seq.s);
				extract_seeds(seq->seq.s, seeds);
				Fingerprint f = seeds[2*i+j];
				if (good_fingerprints.count(f)>0) {
					// We map the fingerprint to a putative junction
					cout << hex << f << ":" << j << endl;
					uint64_t index = junction_index.index(f);
					LongTightString lts("");
					if (j==0)
						lts = lts_straigth;
					else
						lts = lts_revcom;

					if (junctions[index].single_side.length() == 0) {
						// first time we visit the junction
						junctions[index].single_side = lts;
					} else {
						LongTightString single_side = junctions[index].single_side;
						len_t lts_left = overlap(lts, single_side);
						len_t lts_lright = overlap(single_side, lts);
					}
				}
			}
        }
    }
    kseq_destroy(seq);
    gzclose(fp);

	for (unsigned int _i=0; _i<junction_index.size(); ++_i) {
		junctions[_i].dump();
	}


    return 0;
}
