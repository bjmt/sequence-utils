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

#ifndef _KLETS_
#define _KLETS_

#include <vector>
#include <string>

std::vector<std::string> make_klets(const std::vector<char> &lets_uniq, unsigned int k);

std::vector<unsigned long> count_klets(const std::string &letters,
    const std::vector<char> &lets_uniq, unsigned int k, size_t alphlen);

#endif
