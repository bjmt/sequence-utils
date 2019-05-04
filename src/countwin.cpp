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

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <unistd.h>
#include "klets.hpp"
using namespace std;

void usage() {
  printf(
    "countwin v1.0  Copyright (C) 2019  Benjamin Jean-Marie Tremblay                 \n"
    "                                                                                \n"
    "Usage:   countwin [options] -a [alphabet] -i [filename] -o [filename]           \n"
    "         echo [string] | countwin [options] -a [alphabet] > [filename]          \n"
    "                                                                                \n"
    " -i <str>   Input filename. All white space will be removed. Alternatively, can \n"
    "            take string input from a pipe.                                      \n"
    " -o <str>   Output filename. Alternatively, prints to stdout. Output is printed \n"
    "            in tsv format.                                                      \n"
    " -a <str>   A string containing all of the alphabet letters present in the      \n"
    "            sequence.                                                           \n"
    " -k <int>   K-let size. Defaults to 1.                                          \n"
    " -w <int>   Window size. Defaults to K.                                         \n"
    " -s <int>   Step size. Must be equal to or less than window size. Defaults to   \n"
    "            window size.                                                        \n"
    " -n         Don't print rows where the COUNT column is 0.                       \n"
    " -h         Show usage.                                                         \n"
  );
}

string make_row(string START, string STOP, vector<int> counts,
    vector<string> klets, bool nozero) {

  string out;

  for (int i = 0; i < counts.size(); ++i) {
    if (counts[i] > 0 || !nozero)
      out += START + "\t" + STOP + "\t" + klets[i] + "\t" + to_string(counts[i]) + "\n";
  }

  return out;

}

string extract_window(istream &input, int window) {

  string out;
  char l;
  int counter{0};
  out.reserve(window);

  while (input >> l) {
    if (l != ' ' && l != '\n') {
      out += l;
      ++counter;
    }
    if (counter == window) break;
  }

  return out;

}

int main(int argc, char **argv) {

  int k{1}, START{1};
  int opt, window, alphlen, STOP, step;
  string alph, seq;
  ifstream infile;
  ofstream outfile;
  bool has_file{false}, has_out{false}, has_win{false}, nozero{false}, has_step{false};
  vector<string> klets;
  set<int> lets_set;
  vector<char> lets_uniq;
  vector<int> counts;

  while ((opt = getopt(argc, argv, "i:o:a:k:w:s:nh")) != -1) {
    switch (opt) {

      case 'i': if (optarg) {
                  infile.open(optarg);
                  if (infile.bad()) {
                    cerr << "Error: file not found" << endl;
                    cerr << "Run countwin -h to see usage." << endl;
                    exit(EXIT_FAILURE);
                  }
                  has_file = true;
                }
                break;

      case 'o': if (optarg) {
                  outfile.open(optarg);
                  if (outfile.bad()) {
                    cerr << "Error: could not create outfile" << endl;
                    cerr << "Run countwin -h to see usage." << endl;
                    exit(EXIT_FAILURE);
                  }
                  has_out = true;
                }
                break;

      case 'a': if (optarg) alph = optarg;
                break;

      case 'k': if (optarg) k = atoi(optarg);
                break;

      case 'w': if (optarg) {
                  window = atoi(optarg);
                  has_win = true;
                }
                break;

      case 's': if (optarg) {
                  step = atoi(optarg);
                  has_step = true;
                }
                break;

      case 'n': nozero = true;
                break;

      case 'h': usage();
                return 0;

    }
  }

  if (k < 1) {
    cerr << "Error: k must be greater than 0" << endl;
    cerr << "Run countwin -h to see usage." << endl;
    exit(EXIT_FAILURE);
  }

  if (!has_file) {
    if (isatty(STDIN_FILENO)) {
      cerr << "Error: missing input" << endl;
      cerr << "Run countwin -h to see usage." << endl;
      exit(EXIT_FAILURE);
    }
  }

  if (alph.length() < 1) {
    cerr << "Error: missing alphabet" << endl;
    exit(EXIT_FAILURE);
  }

  if (!has_win) {
    window = k;
  } else {
    if (window < k) {
      cerr << "Error: window size must be equal to or greater than k" << endl;
      cerr << "Run countwin -h to see usage." << endl;
      exit(EXIT_FAILURE);
    }
  }

  if (!has_step) step = window;

  if (step < 1 || step > window) {
    cerr << "Error: step size must be between 1 and window size" << endl;
    cerr << "Run countwin -h to see usage." << endl;
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < alph.length(); ++i) {
    lets_set.insert(alph[i]);
  }
  lets_uniq.assign(lets_set.begin(), lets_set.end());
  alphlen = lets_uniq.size();
  klets = make_klets(lets_uniq, k);

  if (has_file) {
    outfile << "START\tSTOP\tLET\tCOUNT" << endl;
  } else {
    cout << "START\tSTOP\tLET\tCOUNT" << endl;
  }

  /* initialise */

  if (has_file) {
    seq = extract_window(infile, window);
  } else {
    seq = extract_window(cin, window);
  }
  STOP = START + seq.length() - 1;
  if (seq.length() < k) {
    cerr << "Error: sequence cannot be smaller than k" << endl;
    cerr << "Run countwin -h to see usage." << endl;
    exit(EXIT_FAILURE);
  }
  counts = count_klets2(vector<char>(seq.begin(), seq.end()), lets_uniq, k, alphlen);
  if (has_out) {
    outfile << make_row(to_string(START), to_string(STOP), counts, klets, nozero);
  } else {
    cout << make_row(to_string(START), to_string(STOP), counts, klets, nozero);
  }
  START += step;

  /* loop the rest */

  while (true) {

    seq.erase(0, step);
    if (has_file) {
      seq += extract_window(infile, step);
    } else {
      seq += extract_window(cin, step);
    }

    if (seq.length() < k) break;

    counts = count_klets2(vector<char>(seq.begin(), seq.end()), lets_uniq, k, alphlen);

    STOP = START + seq.length() - 1;

    if (has_out) {
      outfile << make_row(to_string(START), to_string(STOP), counts, klets, nozero);
    } else {
      cout << make_row(to_string(START), to_string(STOP), counts, klets, nozero);
    }

    START += step;

  }

  if (has_file) infile.close();
  if (has_out) outfile.close();

  return 0;

}
