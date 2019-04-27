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
    "shuffler v1.1  Copyright (C) 2019  Benjamin Jean-Marie Tremblay                 \n"
    "                                                                                \n"
    "Usage:  shuffler [options] -i [filename] -o [filename]                          \n"
    "        echo [string] | shuffler [options] > [filename]                         \n"
    "                                                                                \n"
    " -i <str>   Input filename. All white space will be removed. Alternatively, can \n"
    "            take string input from a pipe.                                      \n"
    " -o <str>   Output filename. Alternatively, prints to stdout. For fasta input, a\n"
    "            newline is inserted every 80 characters.                            \n"
    " -k <int>   K-let size. Defaults to 1.                                          \n"
    " -s <int>   RNG seed number. Defaults to time in seconds.                       \n"
    " -m         Use the markov shuffling method. Defaults to euler.                 \n"
    " -l         Use the linear shuffling method. Defaults to euler.                 \n"
    " -f         Indicate the input is fasta formatted. Each sequence will be        \n"
    "            shuffled individually. Text preceding the first sequence entry is   \n"
    "            ignored.                                                            \n"
    " -v         Verbose mode.                                                       \n"
    " -h         Show usage.                                                         \n"
  );
}

string do_shuffle(vector<char> letters, int k, default_random_engine gen,
    bool verbose, int method_i) {

  string outletters;

  switch (method_i) {

    case 1: shuffle(letters.begin(), letters.end(), gen);
            outletters = string(letters.begin(), letters.end());
            break;
    case 2: outletters = shuffle_markov(letters, gen, k, verbose);
            break;
    case 3: outletters = shuffle_linear(letters, gen, k, verbose);
            break;
    case 4: outletters = shuffle_euler(letters, gen, k, verbose);
            break;

  }

  return outletters;

}

void shuffle_and_write(vector<char> letters, int k, default_random_engine gen,
    bool verbose, int method_i, ostream &output, bool is_fasta) {

  string outletters;
  if (letters.size() > k) {
    outletters = do_shuffle(letters, k, gen, verbose, method_i);
  } else {
    outletters = string(letters.begin(), letters.end());
  }

  if (!is_fasta) {

    output << outletters << endl;

  } else {

    for (int i = 0; i < outletters.length(); ++i) {
      if (i % 80 == 0 && i != 0) {
        output << endl;
      }
      output << outletters[i];
    }
    output << endl;

  }

  return;

}

void read_fasta_then_shuffle_and_write(istream &input, ostream &output, int k,
    default_random_engine gen, int method_i, bool verbose) {

  int count_n{0}, count_s{0};
  string line, name, content;

  while (getline(input, line).good()) {

    if (line.empty() || line[0] == '>') {

      if (!name.empty()) {
        ++count_n;
        output << name << endl;
        name.clear();
      }
      if (!line.empty()) {
        name = line;
      }

      if (content.length() > 0) {

        ++count_s;
        if (count_s < count_n) {
          cerr << "Warning: encountered a missing sequence ["
            << count_n - 1 << "]" << endl;
          count_s = count_n;
        }
        if (content.length() <= k) {
          cerr << "Warning: encountered a sequence where k is too big ["
            << count_n << "]" << endl;
        }

        shuffle_and_write(vector<char>(content.begin(), content.end()), k, gen,
            false, method_i, output, true);

      }
      content.clear();

    } else if (!name.empty()) {

      if (line.find(' ') != string::npos) {
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
      }

      if (line.length() == 0) {
        name.clear();
        content.clear();
      } else {
        content += line;
      }

    }

  }

  if (!name.empty()) {
    ++count_n;
    output << name << endl;
    if (content.length() == 0) {
      cerr << "Warning: encountered a missing sequence ["
        << count_n << "]" << endl;
    } else if (content.length() <= k) {
      cerr << "Warning: encountered a sequence where k is too big ["
        << count_n << "]" << endl;
    }
    shuffle_and_write(vector<char>(content.begin(), content.end()), k, gen,
        false, method_i, output, true);
  }

  if (verbose) {
    cerr << "Shuffled " << count_n << " sequences" << endl;
  }

  return;

}

int main(int argc, char **argv) {

  /* variables */

  int k{1}, method_i{1};
  int opt;
  ifstream seqfile;
  ofstream outfile;
  bool has_file{false}, has_out{false}, is_fasta{false};
  bool use_linear{false}, use_markov{false};
  bool verbose{false};
  unsigned int iseed = time(0);
  char l;
  vector<char> letters;
  default_random_engine gen;

  /* arguments */

  while ((opt = getopt(argc, argv, "i:k:s:o:mvhlf")) != -1) {
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

      case 'f': is_fasta = true;
                break;

      case 'h': usage();
                return 0;

      default: usage();
               return 0;

    }
  }

  if (!has_file && isatty(STDIN_FILENO)) {
    usage();
    exit(EXIT_FAILURE);
  }

  if (k < 1) {
    cerr << "Error: k must be greater than 0" << endl;
    exit(EXIT_FAILURE);
  }

  if (use_linear && use_markov) {
    cerr << "Error: only use one of -l and -m flags" << endl;
    exit(EXIT_FAILURE);
  }

  if (k > 1) {
    if (use_markov) method_i = 2;
    else if (use_linear) method_i = 3;
    else method_i = 4;
  }

  gen = default_random_engine(iseed);

  if (verbose) {
    cerr << "K-let size: " << k << endl;
    cerr << "RNG seed: " << iseed << endl;
    if (k > 1) {
      cerr << "Shuffling method: ";
      switch (method_i) {
        case 2: cerr << "markov";
                break;
        case 3: cerr << "linear";
                break;
        case 4: cerr << "euler";
                break;
      }
      cerr << endl;
    }
  }

  /* The code here is a bit spaghetti, but the alternative is to have way more
   * repeate code. Additionally, for fasta this allows reading+shuffling+writting
   * per sequence instead of needing to read the whole file before starting.
   */

  if (!is_fasta) {

    if (!has_file) {

      while (cin >> l) letters.push_back(l);

      if (letters.size() <= k) {
        cerr << "Error: k must be greater than sequence length" << endl;
        exit(EXIT_FAILURE);
      }

      if (has_out) {
        shuffle_and_write(letters, k, gen, verbose, method_i, outfile, false);
        outfile.close();
      } else {
        shuffle_and_write(letters, k, gen, verbose, method_i, cout, false);
      }

      if (verbose) {
        cerr << "Shuffled " << letters.size() << " characters" << endl;
      }

    } else {

      while (seqfile >> l) letters.push_back(l);
      seqfile.close();

      if (letters.size() <= k) {
        cerr << "Error: k must be greater than sequence length" << endl;
        exit(EXIT_FAILURE);
      }

      if (has_out) {
        shuffle_and_write(letters, k, gen, verbose, method_i, outfile, false);
        outfile.close();
      } else {
        shuffle_and_write(letters, k, gen, verbose, method_i, cout, false);
      }

      if (verbose) {
        cerr << "Shuffled " << letters.size() << " characters" << endl;
      }

    }

  } else {

    if (!has_file) {

      if (has_out) {
        read_fasta_then_shuffle_and_write(cin, outfile, k, gen, method_i, verbose);
        outfile.close();
      } else {
        read_fasta_then_shuffle_and_write(cin, cout, k, gen, method_i, verbose);
      }

    } else {

      if (has_out) {
        read_fasta_then_shuffle_and_write(seqfile, outfile, k, gen, method_i, verbose);
        seqfile.close();
        outfile.close();
      } else {
        read_fasta_then_shuffle_and_write(seqfile, cout, k, gen, method_i, verbose);
        seqfile.close();
      }

    }

  }

  return 0;

}
