/*
 * Copyright (C) 2019 Benjamin Jean-Marie Tremblay
 *
 * This file is part of sequenceshuffler.
 *
 * sequenceshuffler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * sequenceshuffler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sequenceshuffler.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <string>
#include <vector>
#include <random>
#include <set>
#include <iostream>
#include <stdlib.h>
using namespace std;

vector<string> make_klets(vector<char> lets_uniq, int k) {

  int alphlen = lets_uniq.size();
  int nlets = pow(alphlen, k);
  int let_i, counter, step;
  vector<string> klets;
  for (int i = 0; i < nlets; ++i) {
    klets.push_back("");
  }

  for (int i = k - 1; i >= 0; --i) {

    counter = 0;
    let_i = 0;
    step = pow(alphlen, i);

    while (counter < nlets) {

      for (int j = 0; j < step; ++j) {
        klets[counter] += lets_uniq[let_i];
        ++counter;
      }

      if (let_i == alphlen - 1)
        let_i = 0;
      else
        ++let_i;

    }

  }

  return klets;

}

vector<int> count_klets(vector<char> letters, vector<string> klets, int k,
    int alphlen) {

  int seqlen = letters.size();
  int nlets = pow(alphlen, k);
  string let_j;
  vector<int> let_counts;
  for (int i = 0; i < nlets; ++i) {
    let_counts.push_back(0);
  }

  if (klets.size() == 0) {
    cerr << "Error: empty klets vector [count_klets()]" << endl;
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < seqlen - k + 1; ++i) {

    let_j.clear();

    for (int j = 0; j < k; ++j) {
      let_j += letters[i + j];
    }

    for (int j = 0; j < nlets; ++j) {

      if (let_j.compare(klets[j]) == 0) {
        ++let_counts[j];
        break;
      }

    }

  }

  return let_counts;

}

string markov_loop(vector<string> klets, vector<string> kletsm1,
    vector<int> let_counts, vector<char> lets_uniq, default_random_engine gen,
    int seqlen, int k, bool verbose) {

  int rand_i, x, y;
  int alphlen = lets_uniq.size();
  int nlets = klets.size();
  int nletsm1 = kletsm1.size();
  string tmp_let, out;

  if (let_counts.size() == 0) {
    cerr << "Error: empty let_counts vector [markov_loop()]" << endl;
    exit(EXIT_FAILURE);
  }

  /* initialize sequence */

  discrete_distribution<int> first_let_d(let_counts.begin(), let_counts.end());
  rand_i = first_let_d(gen);
  tmp_let = klets[rand_i];
  vector<char> out_split(tmp_let.begin(), tmp_let.end());

  /* keep growing */

  for (int i = k + 1; i < seqlen; ++i) {

    tmp_let.clear();
    for (int j = k - 1; j >= 1; --j) {
      tmp_let += out_split[i - k];
    }

    for (int j = 0; j < nletsm1; ++j) {

      if (tmp_let.compare(kletsm1[j]) == 0) {
        x = j * alphlen;
        y = nlets - x - alphlen;
        discrete_distribution<int> next_let(let_counts.begin() + x, let_counts.end() - y);
        rand_i = next_let(gen);
        out_split.push_back(lets_uniq[rand_i]);
        break;
      }

    }

  }

  if (verbose) {
    vector<string> k1lets = make_klets(lets_uniq, 1);
    vector<int> k1_counts = count_klets(out_split, k1lets, 1, alphlen);
    cerr << "  After shuffling:" << endl;
    for (int i = 0; i < alphlen; ++i) {
      cerr << "    " << k1lets[i] << " " << k1_counts[i] << "\n";
    }
  }

  /* return */

  for (int i = 0; i < out_split.size(); ++i) {
    out += out_split[i];
  }

  return out;

}

string shuffle_markov(vector<char> letters, default_random_engine gen, int k,
    bool verbose) {

  /* variables */

  int seqlen = letters.size();
  vector<char> lets_uniq;
  set<int> lets_set;
  int alphlen, nlets;
  vector<string> klets, kletsm1;
  vector<int> let_counts;
  string let_j, out;

  /* get unique letters */

  for (int i = 0; i < seqlen; ++i) {
    lets_set.insert(letters[i]);
  }
  lets_uniq.assign(lets_set.begin(), lets_set.end());

  /* make k-lets */

  alphlen = lets_uniq.size();
  nlets = pow(alphlen, k);

  klets = make_klets(lets_uniq, k);
  kletsm1 = make_klets(lets_uniq, k - 1);

  /* count k-lets */

  let_counts = count_klets(letters, klets, k, alphlen);

  if (verbose) {
    vector<string> k1lets = make_klets(lets_uniq, 1);
    vector<int> k1_counts = count_klets(letters, k1lets, 1, alphlen);
    cerr << "Letter counts:" << endl;
    cerr << "  Before shuffling:" << endl;
    for (int i = 0; i < alphlen; ++i) {
      cerr << "    " << k1lets[i] << " " << k1_counts[i] << "\n";
    }
  }

  /* generate shuffled sequence */

  out = markov_loop(klets, kletsm1, let_counts, lets_uniq, gen, seqlen, k, verbose);

  /* return */

  return out;

}
