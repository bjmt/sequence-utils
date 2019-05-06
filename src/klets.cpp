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
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <unordered_map>
using namespace std;

#ifdef ADD_TIMERS
#include <chrono>
using Clock = chrono::high_resolution_clock;
#endif

vector<string> make_klets(vector<char> lets_uniq, unsigned int k) {

  size_t alphlen = lets_uniq.size();
  unsigned long nlets = pow(alphlen, k);
  unsigned long let_i, counter, step;
  vector<string> klets(nlets, "");

  /* perhaps a bit primitive, but it works */

  for (int i = k - 1; i >= 0; --i) {

    counter = 0;
    let_i = 0;
    step = pow(alphlen, i);

    while (counter < nlets) {

      for (unsigned long j = 0; j < step; ++j) {
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

vector<unsigned long> count_klets(string letters, vector<char> lets_uniq,
    unsigned int k, size_t alphlen) {

  /* Scales very well with increasing k, but requires having the entire
   * sequence in memory.
   */

  #ifdef ADD_TIMERS
  auto t0 = Clock::now();
  cerr << ">BEGIN count_klets()" << endl;
  #endif

  size_t seqlen = letters.length();
  unsigned long nlets = pow(alphlen, k);
  unsigned long l, counter;
  vector<unsigned long> let_counts(nlets, 0);
  vector<unsigned int> intletters;
  intletters.reserve(seqlen);
  unordered_map<char, unsigned int> let2int;
  let2int.reserve(lets_uniq.size());

  for (size_t i = 0; i < lets_uniq.size(); ++i) {
    let2int[lets_uniq[i]] = (unsigned int)i;
  }

  for (size_t i = 0; i < seqlen; ++i) {
    intletters.push_back(let2int[letters[i]]);
  }

  #ifdef ADD_TIMERS
  auto t1 = Clock::now();
  cerr << " lets->ints\t"
    << chrono::duration_cast<chrono::microseconds>(t1 - t0).count()
    << " us" << endl;
  #endif

  for (size_t i = 0; i < seqlen - k + 1; ++i) {

    l = 0; counter = 0;
    for (int j = k - 1; j >= 0; --j) {
      l += pow(alphlen, j) * intletters[i + counter];
      ++counter;
    }
    ++let_counts[l];

  }

  #ifdef ADD_TIMERS
  auto t2 = Clock::now();
  cerr << " count loop\t"
    << chrono::duration_cast<chrono::microseconds>(t2 - t1).count()
    << " us" << endl;
  cerr << " ---\n fun total\t"
    << chrono::duration_cast<chrono::microseconds>(t2 - t0).count()
    << " us" << endl;
  cerr << ">END count_klets()" << endl;
  #endif

  return let_counts;

}
