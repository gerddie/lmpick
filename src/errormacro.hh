/* -*- mia-c++  -*-
 *
 * This file is part of qtlmpick- a tool for landmark picking and
 * visualization in volume data
 * Copyright (c) Genoa 2017,  Gert Wollny
 *
 * qtlmpick is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MIA; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ERRORMACRO_HH
#define ERRORMACRO_HH

#include <QTextStream>


template <typename V>
void __append_message(QTextStream& os, const V& v)
{
        os << v;
}

inline void __append_message(QTextStream& os __attribute__((unused)))
{
}

template <typename V, typename... T>
void __append_message(QTextStream& os, const V& v, T ...t)
{
        os << v;
        ::__append_message(os, t...);
}

template <typename... T>
const QString __create_message(T ...t)
{
        QString msg;
        QTextStream msg_stream(&msg);
        ::__append_message(msg_stream, t...);
        return msg;
}

/**
    \ingroup misc
    \brief  helper template to create exceptions with complex messages

    This function template creates an exception of the given type. The
    exception message is created by feeding the arguments from left to right
    into a stringstream. An abitrary number of arguments can be given, but they all
    must support the operator << for writing to an output stream

    \tparam E the exception to be created
    \tparam T... the variadic list of arguments
    \param ...t the arguments used to create the exception message
*/
template <typename E, typename... T>
E create_exception( T ...t )
{
        return E(::__create_message(t...).toStdString());
}

#endif // ERRORMACRO_HH
