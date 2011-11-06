#include "UndoAction.h"
#include "UndoHistory.h"
#include "Object.h"


namespace Polka {

UndoAction::UndoAction( UndoHistory& hist, Object& source )
	: m_History( hist )
{
	m_Source = source.name();
	m_History.registerAction(this);
}

UndoAction::~UndoAction()
{
}

const Glib::ustring& UndoAction::source() const
{
	return m_Source;
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


} // namespace ...

