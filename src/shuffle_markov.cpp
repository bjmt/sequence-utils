/*
 * Copyright (C) 2019 Benjamin Jean-Marie Tremblay
 *
 * This file is part of sequence-utils.
 *
 * sequence-utils is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * sequence-utils is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sequence-utils.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <string>
#include <vector>
#include <random>
#include <set>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "klets.hpp"
using namespace std;

string markov_loop(vector<string> klets, vector<string> kletsm1,
    vector<unsigned long> let_counts, vector<char> lets_uniq,
    default_random_engine gen, size_t seqlen, unsigned int k, bool verbose) {

  unsigned long rand_i, x, y;
  size_t alphlen = lets_uniq.size();
  size_t nlets = klets.size();
  size_t nletsm1 = kletsm1.size();
  string tmp_let, out, out_split;
  out.reserve(seqlen);

  if (let_counts.size() == 0) {
    cerr << "Error: empty let_counts vector [markov_loop()]" << endl;
    exit(EXIT_FAILURE);
  }

  /* initialize sequence */

  discrete_distribution<unsigned long> first_let_d(let_counts.begin(), let_counts.end());
  rand_i = first_let_d(gen);
  tmp_let = klets[rand_i];
  out_split = tmp_let;
  out_split.reserve(seqlen);

  /* keep growing */

  for (size_t i = k + 1; i < seqlen; ++i) {

    tmp_let.clear();
    for (int j = k - 1; j >= 1; --j) {
      tmp_let += out_split[i - k];
    }

    for (size_t j = 0; j < nletsm1; ++j) {

      /* Perhaps experiment with using ints instead of chars here. Not convinced
       * it would lead to much speed increase since in the end a new string has
       * to be pasted together regardless. I'm very probably wrong though.
       */

      if (tmp_let.compare(kletsm1[j]) == 0) {
        x = j * alphlen;
        y = nlets - x - alphlen;
        discrete_distribution<unsigned long> next_let(let_counts.begin() + x,
            let_counts.end() - y);
        rand_i = next_let(gen);
        out_split += lets_uniq[rand_i];
        break;
      }

    }

  }

  if (verbose) {
    vector<string> k1lets = make_klets(lets_uniq, 1);
    vector<unsigned long> k1_counts = count_klets(out_split, lets_uniq, 1, alphlen);
    size_t alignlen = to_string(max_element(k1_counts.begin(), k1_counts.end())[0]).length();
    cerr << "  After shuffling:" << endl;
    for (size_t i = 0; i < alphlen; ++i) {
      cerr << "    " << k1lets[i] << "  " << setw(alignlen) << k1_counts[i] << "\n";
    }
  }

  /* return */

  for (size_t i = 0; i < out_split.size(); ++i) {
    out += out_split[i];
  }

  return out;

}

string shuffle_markov(string letters, default_random_engine gen,
    unsigned int k, bool verbose) {

  /* variables */

  size_t seqlen = letters.length();
  vector<char> lets_uniq;
  set<unsigned long> lets_set;
  unsigned long nlets;
  size_t alphlen;
  vector<string> klets, kletsm1;
  vector<unsigned long> let_counts;
  string let_j, out;

  /* get unique letters */

  for (size_t i = 0; i < seqlen; ++i) {
    lets_set.insert(letters[i]);
  }
  lets_uniq.assign(lets_set.begin(), lets_set.end());

  /* make k-lets */

  alphlen = lets_uniq.size();
  nlets = pow(alphlen, k);

  klets = make_klets(lets_uniq, k);
  kletsm1 = make_klets(lets_uniq, k - 1);

  /* count k-lets */

  let_counts = count_klets(letters, lets_uniq, k, alphlen);

  if (verbose) {
    vector<string> k1lets = make_klets(lets_uniq, 1);
    vector<unsigned long> k1_counts = count_klets(letters, lets_uniq, 1, alphlen);
    int alignlen = to_string(max_element(k1_counts.begin(), k1_counts.end())[0]).length();
    cerr << "Letter counts:" << endl;
    cerr << "  Before shuffling:" << endl;
    for (size_t i = 0; i < alphlen; ++i) {
      cerr << "    " << k1lets[i] << "  " << setw(alignlen) << k1_counts[i] << "\n";
    }
  }

  /* generate shuffled sequence */

  out = markov_loop(klets, kletsm1, let_counts, lets_uniq, gen, seqlen, k, verbose);

  /* return */

  return out;

}
