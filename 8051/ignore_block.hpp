/************************************************************************
 * Copyright (C) 2020-2021 Matthias Rosenthal
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>
 ***********************************************************************/
 
#ifndef IGNORE_BLOCK_8051_HPP_INCLUDED
#define IGNORE_BLOCK_8051_HPP_INCLUDED

namespace disas8051 {

// represents a code block which is ignored by the disassembler
class IgnoreBlock
{
public:
	// maybe insert an optional end address here sometime
};

} // namespace disas8051

#endif // IGNORE_BLOCK_8051_HPP_INCLUDED
