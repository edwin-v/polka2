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

#include "UndoAction.h"
#include "UndoHistory.h"
#include "Object.h"
#include "Project.h"
#include <cassert>
#include <iostream>

namespace Polka {

UndoAction::UndoAction( UndoHistory& hist, guint32 suid )
	: m_History( hist )
{
	m_SourceId = suid;
	m_UndoStorage.setFileIdentification("", m_History.m_VersionMajor, m_History.m_VersionMinor);
	m_RedoStorage.setFileIdentification("", m_History.m_VersionMajor, m_History.m_VersionMinor);
	m_History.registerAction(this);
}

UndoAction::UndoAction( UndoHistory& hist )
	: m_History( hist )
{
	m_History.registerAction(this);
}

UndoAction::~UndoAction()
{
}

guint32 UndoAction::sourceId() const
{
	return m_SourceId;
}

void UndoAction::setName( Glib::ustring name )
{
	m_Name = name;
}

void UndoAction::setIcon( const Glib::RefPtr<Gdk::Pixbuf>& icon )
{
	m_refIcon = icon;
}

Storage& UndoAction::setUndoData( std::string id )
{
	m_UndoId = id;
	return m_UndoStorage;
}

Storage& UndoAction::setRedoData( std::string id )
{
	m_RedoId = id;
	return m_RedoStorage;
}

const Glib::ustring& UndoAction::name() const
{
	return m_Name;
}

const Glib::RefPtr<Gdk::Pixbuf> UndoAction::icon() const
{
	return m_refIcon;
}

bool UndoAction::isUserAction() const
{
	return !m_UserActionName.empty();
}

const Glib::ustring& UndoAction::userActionName() const
{
	return m_UserActionName;
}

const Glib::RefPtr<Gdk::Pixbuf> UndoAction::userActionIcon() const
{
	return m_refUserActionIcon;
}

const std::string& UndoAction::undoId() const
{
	return m_UndoId;
}

const std::string& UndoAction::redoId() const
{
	return m_RedoId;
}

Storage& UndoAction::undoData()
{
	return m_UndoStorage;
}

Storage& UndoAction::redoData()
{
	return m_RedoStorage;
}

void UndoAction::undo( Project& project )
{
	if( m_SourceId ) {
		Object *obj = project.editObject( m_SourceId );
		assert( obj );
		std::cout << "undo:" << obj->name() << std::endl;
		obj->objectUndo( m_UndoId, m_UndoStorage );
	} else {
		project.projectUndo( m_UndoId, m_UndoStorage );
	}
}

void UndoAction::redo( Project& project )
{
	if( m_SourceId ) {
		Object *obj = project.editObject( m_SourceId );
		assert( obj );
		std::cout << "redo:" << obj->name() << std::endl;
		obj->objectRedo( m_RedoId, m_RedoStorage );
	} else {
		project.projectRedo( m_RedoId, m_RedoStorage );
	}
}


} // namespace ...

