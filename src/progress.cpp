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
using namespace std;

void progress_meter(int i, int stop) {

  if (i == 0) {
    cerr << "  0%";
    return;
  }

  if (i == stop) {
    cerr << "\b\b\b\b100%" << endl;
    return;
  }

  i *= 100;
  i /= stop;

  if (i != ((i - 1) * 100) / stop) {
    if (i < 10)
      cerr << "\b\b\b\b  " << i << "%";
    else
      cerr << "\b\b\b\b " << i << "%";
  }

  return;

}
