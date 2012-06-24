#include "UndoHistory.h"
#include "UndoAction.h"
#include "Project.h"
#include <cassert>
#include <iostream>


namespace Polka {

/*
 * UndoHistory implementation
 */


UndoHistory::UndoHistory( Project& project, int major, int minor )
	: m_Project(project), m_VersionMajor(major), m_VersionMinor(minor)
{
	m_UndoPointName = "ERROR";
}

UndoHistory::~UndoHistory()
{
	clearHistory();
}

UndoHistory::SignalHistoryChanged UndoHistory::signalHistoryChanged()
{
	return m_SignalHistoryChanged;
}

void UndoHistory::clearHistory()
{
	clearRedoHistory();
	clearUndoHistory();
}

void UndoHistory::clearUndoHistory( unsigned int num_remain )
{
	if( m_UndoActions.size() <= num_remain ) return;
	
	while( m_UndoActions.size() > num_remain ) {
		delete m_UndoActions.back();
		m_UndoActions.pop_back();
	}

	m_SignalHistoryChanged.emit( CHANGE_ALLUNDO );	
}

void UndoHistory::clearRedoHistory( unsigned int num_remain )
{
	if( m_RedoActions.size() <= num_remain ) return;
	
	while( m_RedoActions.size() > num_remain ) {
		delete m_RedoActions.back();
		m_RedoActions.pop_back();
	}

	m_SignalHistoryChanged.emit( CHANGE_ALLREDO );	
}

void UndoHistory::createUndoPoint( const Glib::ustring& name, const Glib::RefPtr<Gdk::Pixbuf>& icon )
{
	assert( !name.empty() );
	
	m_UndoPointName = name;
	m_refUndoPointIcon = icon;
}

UndoAction& UndoHistory::createAction( Object& object )
{
	assert( &object.project() == &m_Project );
	return *(new UndoAction( *this, object.funid() ));
}

UndoAction& UndoHistory::createAction()
{
	return *(new UndoAction( *this, 0 ));
}

void UndoHistory::registerAction( UndoAction* action )
{
	// redo invalid now, clear
	if( m_RedoActions.size() )
		clearRedoHistory();

	// add to undo history
	m_UndoActions.push_back( action );
	if( !m_UndoPointName.empty() ) {
		action->m_UserActionName = m_UndoPointName;
		action->m_refUserActionIcon = m_refUndoPointIcon;
		m_UndoPointName.clear();
		m_SignalHistoryChanged.emit( CHANGE_ADDUNDO );
	}
}

const UndoAction& UndoHistory::lastUndoAction() const
{
	assert( m_UndoActions.size() > 0 );
	auto it = m_UndoActions.end();
	for(;;) {
		it--;
		UndoAction *a = *it;
		if( a->isUserAction() )
			return *a;
	}
}

void UndoHistory::undo()
{
	for(;;) {
		UndoAction& a = *m_UndoActions.back();
		a.undo(m_Project);
		m_RedoActions.push_back( m_UndoActions.back() );
		m_UndoActions.pop_back();
		if( a.isUserAction() ) break;
	}
	m_SignalHistoryChanged.emit( CHANGE_UNDOACTION );	
	m_UndoPointName = "ERROR";
}

void UndoHistory::redo()
{
	if( !m_RedoActions.size() ) return;
	for(;;) {
		UndoAction& a = *m_RedoActions.back();
		a.redo(m_Project);
		m_UndoActions.push_back( m_RedoActions.back() );
		m_RedoActions.pop_back();
		if( m_RedoActions.size() == 0 || m_RedoActions.back()->isUserAction() ) break;
	}
	m_SignalHistoryChanged.emit( CHANGE_REDOACTION );
	m_UndoPointName = "ERROR";
}




} // namespace ...

