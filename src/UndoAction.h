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

#ifndef _POLKA_UNDOACTION_H_
#define _POLKA_UNDOACTION_H_

#include "Storage.h"
#include <string>
#include <glibmm/refptr.h>
#include <gdkmm/pixbuf.h>


namespace Polka {

class UndoHistory;
class Object;
class Project;
class UndoActionGroup;

class UndoAction
{
public:
	// set data
	void setName( Glib::ustring name );
	void setIcon( const Glib::RefPtr<Gdk::Pixbuf>& icon );
	Storage& setUndoData( std::string id );
	Storage& setRedoData( std::string id );

	// access to identifiers
	guint32 sourceId() const;
	const Glib::ustring& name() const;
	const Glib::RefPtr<Gdk::Pixbuf> icon() const;

	bool isUserAction() const;
	const Glib::ustring& userActionName() const;
	const Glib::RefPtr<Gdk::Pixbuf> userActionIcon() const;

	// access to data
	const std::string& undoId() const;
	const std::string& redoId() const;
	Storage& undoData();
	Storage& redoData();

protected:	
	UndoAction( UndoHistory& hist, guint32 source );
	UndoAction( UndoHistory& hist );
	virtual ~UndoAction();

	virtual void undo( Project& project );
	virtual void redo( Project& project );

private:
	friend class UndoHistory;

	Glib::ustring m_Name, m_UserActionName;
	UndoHistory& m_History;
	guint32 m_SourceId;
	Glib::RefPtr<Gdk::Pixbuf> m_refIcon, m_refUserActionIcon;

	std::string m_UndoId, m_RedoId;
	Storage m_UndoStorage, m_RedoStorage;
};

} // namespace Polka

#endif // _POLKA_UNDOACTION_H_

