/*
	Copyright (C) 2013 Edwin Velds

    This file is part of Polka 2.

    Polka 2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Polka 2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Polka 2.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ObjectPropertySheet.h"

namespace Polka {

ObjectPropertySheet::ObjectPropertySheet( const Glib::ustring& _name )
	: m_Name(_name)
{
}

ObjectPropertySheet::~ObjectPropertySheet()
{
}

const Glib::ustring& ObjectPropertySheet::pageName() const
{
	return m_Name;
}

ObjectPropertySheet::SignalSetModified ObjectPropertySheet::signalSetModified()
{
	return m_SignalSetModified;
}

void ObjectPropertySheet::setModified( bool value )
{
	m_SignalSetModified.emit(value);
}

} // namespace Polka 
