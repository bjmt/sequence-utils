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

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <ctime>
#include <random>
#include <unistd.h>
#include <cstdlib>
#include "shuffle_linear.hpp"
#include "shuffle_markov.hpp"
#include "shuffle_euler.hpp"
using namespace std;

void usage() {
  printf(
    "shuffler v1.0 (2019-20-04)                                                      \n"
    "Created by Benjamin Jean-Marie Tremblay <benjmtremblay@gmail.com>               \n"
    "                                                                                \n"
    "Usage:  shuffler [options] -i [filename] -o [filename]                          \n"
    "        echo [string] | shuffler [options] > [filename]                         \n"
    "                                                                                \n"
    " -i <str>   Input filename. All white space will be removed. Alternatively, can \n"
    "            take string input from a pipe.                                      \n"
    " -o <str>   Output filename. Alternatively, prints to stdout.                   \n"
    " -k <int>   K-let size. Defaults to 1.                                          \n"
    " -s <int>   RNG seed number. Defaults to time in seconds.                       \n"
    " -m         Use the markov shuffling method (defaults to euler).                \n"
    " -l         Use the linear shuffling method (defaults to euler).                \n"
    " -v         Verbose mode.                                                       \n"
    " -h         Show usage.                                                         \n"
  );
}

int main(int argc, char **argv) {

  /* variables */

  int k {1};
  int opt;
  ifstream seqfile;
  ofstream outfile;
  bool has_file {false}, has_out {false};
  bool use_linear {false}, use_markov {false};
  bool verbose {false};
  unsigned int iseed = time(0);
  char l;
  vector<char> letters;
  string outletters;
  default_random_engine gen;

  /* arguments */

  while ((opt = getopt(argc, argv, "i:k:s:o:mvhl")) != -1) {
    switch (opt) {

      case 'i': if (optarg) {
                  seqfile.open(optarg);
                  if (seqfile.bad()) {
                    cerr << "Error: file not found" << endl;
                    exit(EXIT_FAILURE);
                  }
                  has_file = true;
                }
                break;

      case 'k': if (optarg) k = atoi(optarg);
                break;

      case 's': if (optarg) iseed = atoi(optarg);
                break;

      case 'o': if (optarg) {
                  outfile.open(optarg);
                  if (outfile.bad()) {
                    cerr << "Error: could not create outfile" << endl;
                    exit(EXIT_FAILURE);
                  }
                  has_out = true;
                }
                break;

      case 'm': use_markov = true;
                break;

      case 'l': use_linear = true;
                break;

      case 'v': verbose = true;
                break;

      case 'h': usage();
                return 0;

      default: usage();
               return 0;

    }
  }

  if (k < 1) {
    cerr << "Error: k must be greater than 0" << endl;
    exit(EXIT_FAILURE);
  }

  if (use_linear && use_markov) {
    cerr << "Error: only use one of -l and -m flags" << endl;
    exit(EXIT_FAILURE);
  }

  /* read letters */

  if (!has_file) {
    if (isatty(STDIN_FILENO)) {
      usage();
      exit(EXIT_FAILURE);
    }
    while (cin >> l) letters.push_back(l);
  } else {
    while (seqfile >> l) letters.push_back(l);
    seqfile.close();
  }

  /* shuffle */

  gen = default_random_engine(iseed);

  if (k >= letters.size()) {
    cerr << "Error: sequence length must be greater than k" << endl;
    exit(EXIT_FAILURE);
  }

  if (verbose) {
    cerr << "Character count: " << letters.size() << endl;
    cerr << "K-let size: " << k << endl;
    cerr << "RNG seed: " << iseed << endl;
    if (k > 1) {
      cerr << "Shuffling method: ";
      if (use_markov) cerr << "markov";
      else if (use_linear) cerr << "linear";
      else cerr << "euler";
      cerr << endl;
    }
  }

  if (k == 1) {
    shuffle(letters.begin(), letters.end(), gen);
    outletters = string(letters.begin(), letters.end());
  } else {
    if (use_markov) outletters = shuffle_markov(letters, gen, k, verbose);
    else if (use_linear) outletters = shuffle_linear(letters, gen, k, verbose);
    else outletters = shuffle_euler(letters, gen, k, verbose);
  }

  /* return */

  if (has_out) {
    outfile << outletters;
    outfile.close();
  } else {
    cout << outletters << endl;
  }

  return 0;

}
