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
#include <iostream>
using namespace std;

string shuffle_linear(string letters, default_random_engine gen,
    unsigned int k, bool verbose) {

  /* variables */

  unsigned long seqlen1 = letters.length();
  unsigned long seqlen2{seqlen1 / k};
  unsigned long seqrem{seqlen1 % k};
  unsigned long seqremlen{seqlen1 - seqrem};

  if (verbose) {
    cerr << "  Times split: " << seqlen2 << endl;
    cerr << "  Remainder: " << seqrem << endl;
  }

  string out;
  out.reserve(seqlen1);

  vector<unsigned long> seqindex;
  seqindex.reserve(seqlen2);

  /* shuffle index */

  for (unsigned long i = 0; i < seqlen2; ++i) {
    seqindex.push_back(i * k);
  }

  shuffle(seqindex.begin(), seqindex.end(), gen);

  /* build output string from shuffled index */

  for (unsigned long i = 0; i < seqlen2; ++i) {
    for (unsigned int j = 0; j < k; ++j) {
      out += letters[seqindex[i] + j];
    }
  }

  /* add leftover letters */

  if (seqrem > 0) {
    for (unsigned long i = seqremlen; i < seqlen1; ++i) {
      out += letters[i];
    }
  }

  /* return */

  return out;

}
