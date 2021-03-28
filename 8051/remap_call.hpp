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
 
#ifndef REMAP_CALL_HPP_INCLUDED
#define REMAP_CALL_HPP_INCLUDED

namespace disas8051 {

class RemapCall
{
public:
	uint64_t targetAddress; // -1 if invalid remap call
    int posX, posY; // for graph in GUI, currently unused!
};

} // namespace disas8051

#endif // REMAP_CALL_HPP_INCLUDED
