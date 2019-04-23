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
#include <set>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <random>
#include "klets.hpp"
using namespace std;

vector<string> find_euler(vector<string> edgelist, string last, vector<string> kletsm1,
    default_random_engine gen, vector<char> lets_uniq) {

  /* cycle-popping algorithm */

  /* variables */

  int n = kletsm1.size();
  int n2 = kletsm1[0].length() - 1;
  int u, rand_i;
  vector<string> lastlets;
  vector<bool> vertices;
  string l;
  bool success {false};

  for (int i = 0; i < n; ++i) {
    vertices.push_back(false);
  }
  for (int i = 0; i < n; ++i) {
    lastlets.push_back("");
  }

  /* set tree root */

  for (int i = 0; i < n; ++i) {
    if (last.compare(kletsm1[i]) == 0) {
      vertices[i] = true;
      success = true;
      break;
    }
  }

  if (!success) {
    vertices.push_back(true);
    lastlets.push_back("");
    kletsm1.push_back(last);
    ++n;
  }
  for (int i = 0; i < kletsm1.size(); ++i) {
    cout << "kletsm1: " << i << " " << kletsm1[i] << " " << vertices[i] <<
      " " << edgelist[i] << endl;
  }

  cout << last << endl;
  // abort();

  /* go */

  for (int i = 0; i < edgelist.size(); ++i) {


    u = i;
    cout << "vertices[" << u << "]=" << vertices[u] << endl;
    while (!vertices[u]) {

      cout << u << endl;
      l.clear();
      cout << kletsm1[u] << " " << edgelist[u].length() << endl;

      rand_i = gen() % edgelist[u].length();
      lastlets[u] = edgelist[u].substr(rand_i, 1);

      cout << "currentl " << lastlets[u] << endl;

      l = kletsm1[u].substr(1, n2) + lastlets[u];
      cout << "nextl " << l << endl;

      for (int j = 0; j < n; ++j) {
        if (l.compare(kletsm1[j]) == 0) {
          u = j;
          cout << "hello" << endl;
          break;
        }
      }

      cout << u << endl;
      // abort();

    }

    cout << endl;
    cout << "lastlets[" << u << "]=" << lastlets[u] << endl;

    u = i;
    while (!vertices[u]) {

      l.clear();

      vertices[u] = true;

      l = kletsm1[u].substr(1, n2) + lastlets[u];
      cout << l << endl;
      // abort();

      for (int j = 0; j < n; ++j) {
        if (l.compare(kletsm1[j]) == 0) {
          u = j;
          break;
        }
      }

    }
    // abort();

  }

  cout << "--LASTLETS START" << endl;
  for (int i = 0; i < n; ++i)
    cout << lastlets[i] << endl;
  cout << "--LASTLETS STOP" << endl;

  return lastlets;

};

vector<string> shuffle_edgelist(vector<string> edgelist, vector<string> lastlets,
    default_random_engine gen) {

  int n = edgelist.size();

  cout << "--SHUFFLED EDGELIST" << endl;
  for (int i = 0; i < n; ++i) {

    for (int j = 0; j < edgelist[i].length(); ++j) {
      if (lastlets[i].compare(edgelist[i].substr(j, 1)) == 0) {
        edgelist[i].erase(j, 1);
        break;
      }
    }

    shuffle(edgelist[i].begin(), edgelist[i].end(), gen);

    edgelist[i] += lastlets[i];
    cout << edgelist[i] << endl;

  }
  cout << "--" << endl;

  return edgelist;

}

string walk_euler(vector<string> edgelist, string firstlet, string last,
    int k, int seqlen, vector<string> kletsm1) {

  string out, currentl, nextl;
  int nexti;

  last = last.substr(last.length() - 1, 1);
  out += firstlet;

  for (int i = k - 2; i < seqlen - 2; ++i) {

    currentl = out.substr(i - k + 2, k - 1);

    for (int j = 0; j < kletsm1.size(); ++j) {
      if (currentl.compare(kletsm1[j]) == 0) {
        nexti = j;
        break;
      }
    }

    nextl = edgelist[nexti].substr(0, 1);
    edgelist[nexti].erase(0, 1);

    out += nextl;

  }

  out += last;

  cout << out.length() << "/" << seqlen << endl;

  return out;

};

vector<string> make_edgelist(vector<int> let_counts, int k, vector<string> kletsm1,
    vector<char> lets_uniq, int alphlen) {

  int counter1 {0}, counter2 {0};
  int j;
  int n1 = kletsm1.size();
  int n2 = let_counts.size();
  vector<string> edgelist;
  for (int i = 0; i < n1; ++i) {
    edgelist.push_back("");
  }

  for (int i = 0; i < n2; ++i) {

    j = let_counts[i];

    for (int b = 0; b < j; ++b) {
      edgelist[counter1] += lets_uniq[counter2];
    }

    ++counter2;

    if (counter2 == alphlen) {
      counter2 = 0;
      ++counter1;
    }

  }

  return edgelist;

}

string shuffle_euler(vector<char> letters, default_random_engine gen, int k,
    bool verbose) {

  /* variables */

  int seqlen = letters.size();
  vector<char> lets_uniq;
  set<int> lets_set;
  int alphlen, nlets;
  vector<string> klets, kletsm1, edgelist, out_split, lastlets;
  vector<int> let_counts;
  string firstlet, last, out;

  for (int i = 0; i < k - 1; ++i) {
    firstlet += letters[i];
  }

  for (int i = seqlen - k + 1; i < seqlen; ++i) {
    last += letters[i];
  }

  for (int i = 0; i < seqlen; ++i) {
    lets_set.insert(letters[i]);
  }
  lets_uniq.assign(lets_set.begin(), lets_set.end());

  alphlen = lets_uniq.size();
  nlets = pow(alphlen, k);

  /* make and count klets */

  klets = make_klets(lets_uniq, k);
  kletsm1 = make_klets(lets_uniq, k - 1);

  let_counts = count_klets(letters, klets, k, alphlen);

  if (verbose) {
    vector<string> k1lets = make_klets(lets_uniq, 1);
    vector<int> k1_counts = count_klets(letters, k1lets, 1, alphlen);
    int alignlen = to_string(max_element(k1_counts.begin(), k1_counts.end())[0]).length();
    cerr << "Letter counts:" << endl;
    for (int i = 0; i < alphlen; ++i) {
      cerr << "  " << k1lets[i] << "  " << setw(alignlen) << k1_counts[i] << "\n";
    }
  }

  /* create edgelist */

  edgelist = make_edgelist(let_counts, k, kletsm1, lets_uniq, alphlen);

  vector<string> edgelist_2, kletsm1_2;
  for (int i = 0; i < edgelist.size(); ++i) {
    if (edgelist[i].length() > 0) {
      edgelist_2.push_back(edgelist[i]);
      kletsm1_2.push_back(kletsm1[i]);
    }
  }

  /* find a random eulerian path from last letter of each vertex */

  lastlets = find_euler(edgelist_2, last, kletsm1_2, gen, lets_uniq);

  /* shuffle edgelist */

  edgelist = shuffle_edgelist(edgelist_2, lastlets, gen);

  /* do euler walk and create new sequence */

  out = walk_euler(edgelist_2, firstlet, last, k, seqlen, kletsm1_2);

  return out;

}
