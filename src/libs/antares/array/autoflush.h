/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_ARRAY_MATRIX_AUTOFLUSHH__
#define __ANTARES_LIBS_ARRAY_MATRIX_AUTOFLUSHH__

#include <yuni/yuni.h>

namespace Antares
{
template<class MatrixT>
class MatrixAutoFlush final
{
public:
    enum
    {
        //! Auto flush
        autoFlushRowCount = 500,
    };

public:
#ifdef ANTARES_SWAP_SUPPORT
    MatrixAutoFlush(const MatrixT& matrix) : pCount(0), pLastFlush(0), pMatrix(matrix)
    {
    }
#else
    MatrixAutoFlush(const MatrixT&)
    {
    }
#endif

    MatrixAutoFlush& operator++()
    {
#ifdef ANTARES_SWAP_SUPPORT
        if (++pCount == autoFlushRowCount)
        {
            if (pLastFlush != 0)
            {
                uint last = pLastFlush + pCount;
                for (uint i = pLastFlush; i != last; ++i)
                    pMatrix[i].flush();
                pLastFlush = last;
                pCount = 0;
            }
            else
            {
                pLastFlush = pCount;
            }
        }
#endif
        return *this;
    }

private:
#ifdef ANTARES_SWAP_SUPPORT
    uint pCount;
    uint pLastFlush;
    const MatrixT& pMatrix;
#endif

}; // class MatrixAutoFlush

} // namespace Antares

#endif // __ANTARES_LIBS_ARRAY_MATRIX_AUTOFLUSHH__