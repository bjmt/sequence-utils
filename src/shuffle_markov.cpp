/*
 * Copyright (C) 2019-2021 Benjamin Jean-Marie Tremblay
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
#include <cmath>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <algorithm>
#include "klets.hpp"
using namespace std;

typedef std::vector<int> vec_int_t;
typedef std::vector<std::vector<int>> list_int_t;

vec_int_t klet_counter(const vec_int_t &single_seq, const int &k,
    const std::size_t &nlets, const std::size_t &alphlen) {

  vec_int_t klet_counts(nlets, 0);
  int l, counter;

  for (std::size_t i = 0; i < single_seq.size() - k + 1; ++i) {
    l = 0; counter = 0;
    for (int j = k - 1; j >= 0; --j) {
      l += pow(alphlen, j) * single_seq[i + counter];
      ++counter;
    }
    ++klet_counts[l];
  }

  return klet_counts;

}

list_int_t get_edgecounts(const vec_int_t &klet_counts, const std::size_t &mlets,
    const std::size_t &alphlen) {

  list_int_t edgecounts(mlets, vec_int_t(alphlen));
  std::size_t counter = 0;

  for (std::size_t i = 0; i < mlets; ++i) {
    for (std::size_t j = 0; j < alphlen; ++j) {
      edgecounts[i][j] = klet_counts[counter];
      ++counter;
    }
  }

  return edgecounts;

}

std::string make_new_seq(const vec_int_t &shuffled_seq_ints,
    const std::string &alph) {

  std::string out;
  out.reserve(shuffled_seq_ints.size());

  for (std::size_t i = 0; i < shuffled_seq_ints.size(); ++i) {
    out += alph[shuffled_seq_ints[i]];
  }

  return out;

}

list_int_t make_klet_lists(const std::size_t &nlets, const int &k,
    const std::size_t &alphlen) {

  list_int_t klet_list(nlets);
  for (std::size_t i = 0; i < nlets; ++i) {
    klet_list[i].reserve(k);
  }
  int counter, let, step;

  for (int i = k - 1; i >= 0; --i) {
    counter = 0;
    let = 0;
    step = pow(alphlen, i);
    while (counter < int(nlets)) {
      for (int j = 0; j < step; ++j) {
        klet_list[counter].push_back(let);
        ++counter;
      }
      if (let == int(alphlen) - 1)
        let = 0;
      else
        ++let;
    }
  }

  return klet_list;

}

vec_int_t markov_generator(const std::size_t &seqsize, const vec_int_t &nlet_counts,
    const list_int_t &transitions, default_random_engine &gen,
    const std::size_t &nlets, const int &k, const std::size_t &alphlen, const bool verbose) {

  vec_int_t out;
  out.reserve(seqsize);

  list_int_t nlet_lists = make_klet_lists(nlets, k, alphlen);

  if (verbose) {
    cerr << "  Generating letters\n";
  }

  std::discrete_distribution<int> first_gen(nlet_counts.begin(), nlet_counts.end());
  int firstletters = first_gen(gen);
  for (int i = 0; i < k; ++i) {
    out.push_back(nlet_lists[firstletters][i]);
  }

  int previous_mlet;
  for (std::size_t i = k - 1; i < seqsize - 1; ++i) {
    previous_mlet = 0;
    for (int j = k - 1; j >= 1; --j) {
      previous_mlet += out[i - j] * pow(alphlen, j - 1);
    }
    std::discrete_distribution<int> next_gen(transitions[previous_mlet].begin(),
        transitions[previous_mlet].end());
    out.push_back(next_gen(gen));
  }

  return out;

}
// string shuffle_markov(const string &letters, default_random_engine &gen,
//     unsigned int k, bool verbose) {

std::string shuffle_markov(const std::string &single_seq,
    default_random_engine &gen, unsigned int k, bool verbose) {

  std::set<int> alph_s;
  for (std::size_t i = 0; i < single_seq.size(); ++i) {
    alph_s.insert(single_seq[i]);
  }
  std::string alph;
  alph.assign(alph_s.begin(), alph_s.end());
  std::size_t alphlen = alph.size();
  std::size_t nlets = pow(alphlen, k);
  std::size_t mlets = pow(alphlen, k - 1);

  vec_int_t seq_ints(single_seq.size());
  for (std::size_t i = 0; i < single_seq.size(); ++i) {
    for (std::size_t a = 0; a < alphlen; ++a) {
      if (single_seq[i] == alph[a]) {
        seq_ints[i] = a;
        break;
      }
    }
  }

  vec_int_t nlet_counts = klet_counter(seq_ints, k, nlets, alphlen);
  list_int_t transitions = get_edgecounts(nlet_counts, mlets, alphlen);
  if (verbose) {
    cerr << "  Generated transitions matrix: " << transitions[0].size() <<
      "x" << transitions.size() << '\n';
  }

  vec_int_t out_ints = markov_generator(seq_ints.size(), nlet_counts, transitions,
      gen, nlets, k, alphlen, verbose);

  if (verbose) {
    cerr << "  Assembling final sequence\n";
  }

  std::string out = make_new_seq(out_ints, alph);

  return out;

}

// string markov_loop(const vector<string> &klets, const vector<string> &kletsm1,
//     const vector<unsigned long> &let_counts, const vector<char> &lets_uniq,
//     default_random_engine &gen, size_t seqlen, unsigned int k, bool verbose) {
//
//   unsigned long rand_i, x, y;
//   size_t alphlen = lets_uniq.size();
//   size_t nlets = klets.size();
//   size_t nletsm1 = kletsm1.size();
//   string tmp_let, out, out_split;
//   out.reserve(seqlen);
//
//   if (let_counts.size() == 0) {
//     cerr << "Error: empty let_counts vector [markov_loop()]" << endl;
//     exit(EXIT_FAILURE);
//   }
//
//   [> initialize sequence <]
//
//   discrete_distribution<unsigned long> first_let_d(let_counts.begin(), let_counts.end());
//   rand_i = first_let_d(gen);
//   tmp_let = klets[rand_i];
//   out_split = tmp_let;
//   out_split.reserve(seqlen);
//
//   [> keep growing <]
//
//   for (size_t i = k + 1; i < seqlen; ++i) {
//
//     tmp_let.clear();
//     for (int j = k - 1; j >= 1; --j) {
//       tmp_let += out_split[i - k];
//     }
//
//     for (size_t j = 0; j < nletsm1; ++j) {
//
      /* Perhaps experiment with using ints instead of chars here. Not convinced
       * it would lead to much speed increase since in the end a new string has
       * to be pasted together regardless. I'm very probably wrong though.
       */
//
//       if (tmp_let.compare(kletsm1[j]) == 0) {
//         x = j * alphlen;
//         y = nlets - x - alphlen;
//         discrete_distribution<unsigned long> next_let(let_counts.begin() + x,
//             let_counts.end() - y);
//         rand_i = next_let(gen);
//         out_split += lets_uniq[rand_i];
//         break;
//       }
//
//     }
//
//   }
//
//   if (verbose) {
//     vector<string> k1lets = make_klets(lets_uniq, 1);
//     vector<unsigned long> k1_counts = count_klets(out_split, lets_uniq, 1, alphlen);
//     size_t alignlen = to_string(max_element(k1_counts.begin(), k1_counts.end())[0]).length();
//     cerr << "  After shuffling:" << endl;
//     for (size_t i = 0; i < alphlen; ++i) {
//       cerr << "    " << k1lets[i] << "  " << setw(alignlen) << k1_counts[i] << "\n";
//     }
//   }
//
//   [> return <]
//
//   for (size_t i = 0; i < out_split.size(); ++i) {
//     out += out_split[i];
//   }
//
//   return out;
//
// }
//
// string shuffle_markov(const string &letters, default_random_engine &gen,
//     unsigned int k, bool verbose) {
//
//   [> variables <]
//
//   size_t seqlen = letters.length();
//   vector<char> lets_uniq;
//   set<unsigned long> lets_set;
//   unsigned long nlets;
//   size_t alphlen;
//   vector<string> klets, kletsm1;
//   vector<unsigned long> let_counts;
//   string let_j, out;
//
//   [> get unique letters <]
//
//   for (size_t i = 0; i < seqlen; ++i) {
//     lets_set.insert(letters[i]);
//   }
//   lets_uniq.assign(lets_set.begin(), lets_set.end());
//
//   [> make k-lets <]
//
//   alphlen = lets_uniq.size();
//   nlets = pow(alphlen, k);
//
//   klets = make_klets(lets_uniq, k);
//   kletsm1 = make_klets(lets_uniq, k - 1);
//
//   [> count k-lets <]
//
//   let_counts = count_klets(letters, lets_uniq, k, alphlen);
//
//   if (verbose) {
//     vector<string> k1lets = make_klets(lets_uniq, 1);
//     vector<unsigned long> k1_counts = count_klets(letters, lets_uniq, 1, alphlen);
//     int alignlen = to_string(max_element(k1_counts.begin(), k1_counts.end())[0]).length();
//     cerr << "Letter counts:" << endl;
//     cerr << "  Before shuffling:" << endl;
//     for (size_t i = 0; i < alphlen; ++i) {
//       cerr << "    " << k1lets[i] << "  " << setw(alignlen) << k1_counts[i] << "\n";
//     }
//   }
//
//   [> generate shuffled sequence <]
//
//   out = markov_loop(klets, kletsm1, let_counts, lets_uniq, gen, seqlen, k, verbose);
//
//   [> return <]
//
//   return out;
//
// }
