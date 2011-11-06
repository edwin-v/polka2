#include "UndoHistory.h"
#include "UndoAction.h"
#include "Project.h"
#include <cassert>
#include <iostream>
namespace Polka {

UndoHistory::UndoHistory( Project& project )
	: m_Project(project)
{
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

UndoAction& UndoHistory::createAction( Object& object )
{
	return *(new UndoAction( *this, object ));
}

void UndoHistory::registerAction( UndoAction* action )
{
	// add to undo history
	m_UndoActions.push_back( action );
	// redo invalid now, clear
	if( m_RedoActions.size() )
		clearRedoHistory();

	m_SignalHistoryChanged.emit( CHANGE_ADDUNDO );	
}

void UndoHistory::displayChange( UndoAction *action )
{
	if( action == m_UndoActions.back() )
		m_SignalHistoryChanged.emit( CHANGE_NEWDISPLAY );
	else
		m_SignalHistoryChanged.emit( CHANGE_ALL );
}

const UndoAction& UndoHistory::lastUndoAction() const
{
	assert( m_UndoActions.size() > 0 );
	return *m_UndoActions.back();
}

void UndoHistory::undo()
{
	if( !m_UndoActions.size() ) return;

 std::cout << "undo:" << m_UndoActions.back()->source() << std::endl;
	Object *obj = m_Project.editObject( m_UndoActions.back()->source() );
	assert( obj );
	UndoAction& a = *m_UndoActions.back();
	obj->undo( a.undoId(), a.undoData() );
	m_RedoActions.push_back( m_UndoActions.back() );
	m_UndoActions.pop_back();

	m_SignalHistoryChanged.emit( CHANGE_UNDOACTION );	
}

void UndoHistory::redo()
{
	if( !m_RedoActions.size() ) return;

 std::cout << "redo:" << m_RedoActions.back()->source() << std::endl;
	Object *obj = m_Project.editObject( m_RedoActions.back()->source() );
	assert( obj );
	UndoAction& a = *m_RedoActions.back();
	obj->redo( a.redoId(), a.redoData() );
	m_UndoActions.push_back( m_RedoActions.back() );
	m_RedoActions.pop_back();

	m_SignalHistoryChanged.emit( CHANGE_REDOACTION );	
}


} // namespace ...

