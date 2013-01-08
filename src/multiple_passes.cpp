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
#include "log.hpp"
#include <sstream>
#include "prettyprint.hpp"

// _KF_ is the number of seeds extracted from each read
#define _KF_ 4

#define BLOOM_FILTER_SIZE_BITS 28
#define BLOOM_FILTER_SIZE (1 << BLOOM_FILTER_SIZE_BITS)
KSEQ_INIT(gzFile, gzread)

using namespace std;

/*
  From each string (which is usually a read) we extract Kf equally spaced substrings of length KMER_LENGTH
*/
void extract_seeds(std::string s, Fingerprint seeds[]) {
    unsigned int l = s.length() - KMER_LENGTH;
    for (unsigned int i=0; i < 2*_KF_ -2 ; i+=2) {
        seeds[i] = encode(s, (size_t) (l * i)/(_KF_ - 1), (size_t) KMER_LENGTH);
        seeds[i+1] = reverse_complement(seeds[i]);
    }
    seeds[2*_KF_ - 2] = encode(s, (size_t) l, (size_t) KMER_LENGTH);
    seeds[2*_KF_ - 1] = reverse_complement(seeds[2*_KF_ - 2]);
}
/*
  From each string (which is usually a read) we extract all substrings of length KMER_LENGTH
*/
std::vector<Fingerprint> extract_fingerprints(std::string s) {
    unsigned int l = s.length() - KMER_LENGTH + 1;
    std::vector<Fingerprint> result(l-1);
    for (unsigned int i=0; i < l ; i++) {
        result.push_back(encode(s, i, (size_t) KMER_LENGTH));
    }
    return result;
}


int main(void)
{
    initialize_logger();
    ROOT_INFO("Tagli started");
    gzFile fp;
    kseq_t *seq;
    std::string inputfile("reads.fastq.gz");
    Fingerprint seeds[2*_KF_];
    uint64_t read_counter = 0;
    /* seeds[] is an array of 2*_KF_ fingerprints extracted from a sequence s.
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
        read_counter++;
        ROOT_TRACE("Read " << read_counter << ": " << seq->seq.s);
        extract_seeds(seq->seq.s, seeds);
        BOOST_FOREACH(Fingerprint f, seeds) {
            unsigned int pos=0;
            ROOT_TRACE("checking fingerprint: " << f);
            MurmurHash3_x86_32(&f, BLOOM_FILTER_SIZE_BITS, 0xB0F57EE3, &pos);
            pos &= (BLOOM_FILTER_SIZE - 1);
            if (bloom_table[pos]) {
                ROOT_TRACE("inserted fingerprint: " << f);
                good_fingerprints.insert(f);
            }
            else
                bloom_table[pos] = true;
        }
    }
    bloom_table.resize(0); // reclaim space
    ROOT_TRACE("good_fingerprints dump");
    ROOT_TRACE(good_fingerprints);
    ROOT_DEBUG("good_fingerprints size: " << good_fingerprints.size());
    kseq_destroy(seq);
    gzclose(fp);

/*
  Build a perfect hash storing only the Fingerprints occurring in at least two reads.
  We use the cmph library (cmph.sf.net) and the BDZ algorithm.
  Each entry of the hash points to a position into the junctions array.
  Each element of junctions is a putative junction site.
*/
    ROOT_INFO("End pass #1");
    cxxmph::SimpleMPHIndex<Fingerprint> junction_index;
    while (good_fingerprints.size() < 10) {
        ROOT_TRACE("Adding " << 10 - good_fingerprints.size() << " random junctions");
        good_fingerprints.insert(rand() % BLOOM_FILTER_SIZE);
    }
    if (!junction_index.Reset(good_fingerprints.begin(), good_fingerprints.end(), good_fingerprints.size())) {
        ROOT_TRACE("Cannot create junction_index");
        exit(-1);
    }
    Junction *junctions = new Junction[junction_index.size()];
/*
  Pass #2

  Since cxxmph::SimpleMPHIndex returns an integer, we need an intermediate array of pointers to the putative junctions.
  Such array is called junction_pointers
*/

    ROOT_INFO("Begin pass #2");


    fp = gzopen(inputfile.c_str(), "r");
    seq = kseq_init(fp);
    read_counter = 0;
    while (kseq_read(seq) >= 0) {
        read_counter++;
        ROOT_TRACE("Read " << read_counter << ": " << seq->seq.s);
        extract_seeds(seq->seq.s, seeds);
        string s = seq->seq.s;
        string rs = reverse_complement(seq->seq.s);
        LongTightString lts_revcom(rs);
        LongTightString lts_straigth(s);
        unsigned int kinds[2] = {0, 1};

        ROOT_TRACE("Read: " << s);
        for (unsigned int i=0; i<_KF_; i++) {
            for (auto j : kinds) {
                Fingerprint f = seeds[2*i+j];
                if (good_fingerprints.count(f) > 0) {
                    // We map the fingerprint to a putative junction
                    ROOT_DEBUG(f << ":" << j);
                    uint64_t index = junction_index.index(f);
                    ROOT_DEBUG("Index = " << index << ", Strand:" << j << ", Junction: " << junctions[index].dump());
                    ROOT_TRACE("Read 0");
                    ROOT_TRACE(lts_straigth.dump());
                    ROOT_TRACE("Read 1");
                    ROOT_TRACE(lts_revcom.dump());
                    (j==0) ? junctions[index].add_read(lts_straigth) : junctions[index].add_read(lts_revcom);
                    ROOT_TRACE("Resulting junction");
                    ROOT_TRACE(junctions[index].dump());
                }
            }
        }
    }
    kseq_destroy(seq);
    gzclose(fp);
    ROOT_DEBUG("End pass #2");

/*
  Pass #3:
  Remove from junctions all invalid junctions, i.e. those without a multiple_side
  At the same time keep a vector valid_indices of the indices of correct junctions

  We estimate an initial size of valid_indices equal to half of the current number of junctions. The correctness isn't
  affected by the choice.
  At the end we resize the vector to avoid wasting space.

  TODO: determine if it is beneficial to copy the valid entries of junction to a new vector.
*/
    ROOT_DEBUG("Begin pass #3");
    ROOT_DEBUG("Junctions");
    std::vector<unsigned int> valid_indices(junction_index.size()/2);
    for (unsigned int i=0; i<junction_index.size(); i++)
        if (junctions[i].multiple_side_length > 0) {
            ROOT_DEBUG(junctions[i].dump());
            valid_indices.push_back(i);
        }
    valid_indices.shrink_to_fit();


/*
  Pass #4:

  Refine the valid junctions by using all possible fingerprints from the read.
  In this pass no new junction can be created.
*/

    ROOT_DEBUG("Begin pass #4");
    fp = gzopen(inputfile.c_str(), "r");
    seq = kseq_init(fp);
    read_counter = 0;
    while (kseq_read(seq) >= 0) {
        read_counter++;
        ROOT_TRACE("Read " << read_counter << ": " << seq->seq.s);
        std::vector<Fingerprint> fingerprints = extract_fingerprints(seq->seq.s);
        string s = seq->seq.s;
        string rs = reverse_complement(seq->seq.s);
        LongTightString lts_revcom(rs);
        LongTightString lts_straigth(s);
        unsigned int kinds[2] = {0, 1};

        for (auto f : fingerprints)
            for (auto j : kinds) {
                if (j==0 && good_fingerprints.count(f) > 0) {
                    ROOT_DEBUG(lts_straigth.dump());
                    junctions[junction_index.index(f)].add_read(lts_straigth);
                }
                if (j==1 && good_fingerprints.count(~f) > 0) {
                    ROOT_DEBUG(lts_revcom.dump());
                    junctions[junction_index.index(~f)].add_read(lts_revcom);
                }
            }
    }
    kseq_destroy(seq);
    gzclose(fp);
/*
  Pass:

  Clean the junctions by merging together similar multiple_side components.
*/


/*
  Finally output the junctions
*/
    ROOT_INFO("Begin Output");
    for (auto i : valid_indices)
        cout << junctions[i].simpledump() << endl;
}
