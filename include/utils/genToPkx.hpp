/*
 *   This file is part of PKSM-Core
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
 *       * Requiring preservation of specified reasonable legal notices or
 *         author attributions in that material or in the Appropriate Legal
 *         Notices displayed by works containing it.
 *       * Prohibiting misrepresentation of the origin of that material,
 *         or requiring that modified versions of such material be marked in
 *         reasonable ways as different from the original version.
 */

#ifndef GENTOPKX_HPP
#define GENTOPKX_HPP

#include "utils/generation.hpp"

class PK3;
class PK4;
class PK5;
class PK6;
class PK7;
class PK8;
class PB7;

template <Generation g>
struct GenToPkx
{
};

template <>
struct GenToPkx<Generation::THREE>
{
    using PKX = PK3;
};

template <>
struct GenToPkx<Generation::FOUR>
{
    using PKX = PK4;
};

template <>
struct GenToPkx<Generation::FIVE>
{
    using PKX = PK5;
};

template <>
struct GenToPkx<Generation::SIX>
{
    using PKX = PK6;
};

template <>
struct GenToPkx<Generation::SEVEN>
{
    using PKX = PK7;
};

template <>
struct GenToPkx<Generation::EIGHT>
{
    using PKX = PK8;
};

template <>
struct GenToPkx<Generation::LGPE>
{
    using PKX = PB7;
};

#endif