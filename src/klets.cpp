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
#include <iostream>
#include <cmath>
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
