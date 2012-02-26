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
	m_History.displayChange(this);
}

void UndoAction::setIcon( Glib::RefPtr<Gdk::Pixbuf> icon )
{
	m_refIcon = icon;
	m_History.displayChange(this);
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

