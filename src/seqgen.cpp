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
#include <fstream>
#include <random>
#include <unistd.h>
using namespace std;

void usage() {
  printf(
    "seqgen v1.0  Copyright (C) 2019  Benjamin Jean-Marie Tremblay                   \n"
    "                                                                                \n"
    "Usage:  seqgen [options] -a [letters] -l [length] -o [outfile]                  \n"
    "        seqgen [options] -a [letters] -l [length] > [outfile]                   \n"
    "                                                                                \n"
    " -o <str>    Output filename. Alternatively, prints to stdout.                  \n"
    " -a <str>    Comma-seperated sequence letters. Each letter can be made of any   \n"
    "             number of characters.                                              \n"
    " -w <str>    Comma-seperated letter weights. Order matches that of letters. If  \n"
    "             missing, assumes equal likelihood for all letters.                 \n"
    " -l <int>    Final sequence length. This length is the number of characters in  \n"
    "             output string.                                                     \n"
    " -s <int>    RNG seed number. Defaults to time in seconds.                      \n"
    " -h          Print usage and exit.                                              \n"
  );
}

int main(int argc, char **argv) {

  unsigned int seqlen{0};
  int alphlen, opt;
  ofstream outfile;
  bool has_out{false}, has_freqs{false};
  default_random_engine gen;
  vector<string> lets;
  vector<double> freqs;
  string outletters, all_lets, all_freqs, final_freq, final_let;
  string comma = ",";
  unsigned int iseed = time(0);
  size_t last, next;

  if (argc == 1) {
    usage();
    return 0;
  }

  while ((opt = getopt(argc, argv, "a:o:l:s:w:h")) != -1) {
    switch (opt) {
      case 'o': if (optarg) {
                  outfile.open(optarg);
                  if (outfile.bad()) {
                    cerr << "Error: could not create outfile" << endl;
                    exit(EXIT_FAILURE);
                  }
                  has_out = true;
                }
                break;
      case 'l': if (optarg) seqlen = atoi(optarg);
                break;
      case 's': if (optarg) iseed = atoi(optarg);
                break;
      case 'a': if (optarg) all_lets = string(optarg);
                break;
      case 'w': if (optarg) {
                  all_freqs = string(optarg);
                  has_freqs = true;
                }
                break;
      case 'h': usage();
                return 0;
      default: usage();
               return 0;
    }
  }

  if (seqlen < 1) {
    cerr << "Error: please input a desired sequence length above 0" << endl;;
    exit(EXIT_FAILURE);
  }

  /* split up letters */

  last = 0;
  next = 0;
  while ((next = all_lets.find(comma, last)) != string::npos) {
    lets.push_back(all_lets.substr(last, next - last));
    last = next + 1;
  }
  final_let = all_lets.substr(last);
  if (final_let.length() > 0) lets.push_back(final_let);

  alphlen = lets.size();

  if (alphlen < 1) {
    cerr << "Error: could not parse sequence alphabet" << endl;
    exit(EXIT_FAILURE);
  }

  /* split up freqs */

  if (has_freqs) {

    last = 0;
    next = 0;
    while ((next = all_freqs.find(comma, last)) != string::npos) {
      freqs.push_back(stod(all_freqs.substr(last, next - last)));
      last = next + 1;
    }
    final_freq = all_freqs.substr(last);
    if (final_freq.length() > 0) freqs.push_back(stod(final_freq));

    if (lets.size() != freqs.size()) {
      cerr << "Error: mismatching number of letters [" << lets.size()
        << "] and frequencies [" << freqs.size() << "]" << endl;
      exit(EXIT_FAILURE);
    }

  }

  /* main seq generation loop */

  gen = default_random_engine(iseed);
  outletters = "";

  if (!has_freqs) {

    while (outletters.length() < seqlen) {
      outletters += lets[gen() % alphlen];
    }

  } else {

    discrete_distribution<int> next_let(freqs.begin(), freqs.end());
    while (outletters.length() < seqlen) {
      outletters += lets[next_let(gen)];
    }

  }

  if (outletters.length() > seqlen)
     outletters = outletters.substr(0, seqlen);

  /* return */

  if (has_out) {
    outfile << outletters << endl;
  } else {
    cout << outletters << endl;
  }

  return 1;

}
