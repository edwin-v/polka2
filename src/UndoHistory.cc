#include "UndoHistory.h"
#include "UndoAction.h"
#include "Project.h"
#include <cassert>
#include <iostream>


namespace Polka {

/*
 * additional class to group undo actions
 */

class UndoActionGroup : public UndoAction
{
public:
	UndoActionGroup( UndoHistory& hist );
	~UndoActionGroup();

	void add( UndoAction *action );

protected:
	virtual void undo( Project& project );
	virtual void redo( Project& project );

private:
	std::vector<UndoAction*> m_Actions;
};


UndoHistory::UndoHistory( Project& project )
	: m_Project(project), m_Grouped(false)
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
	assert( &object.project() == &m_Project );
	return *(new UndoAction( *this, object ));
}

void UndoHistory::openActionGroup( Glib::ustring name, Glib::RefPtr<Gdk::Pixbuf> icon )
{
	if( m_Grouped ) closeActionGroup();
	
	UndoAction *a = new UndoActionGroup(*this);
	a->setName(name);
	a->setIcon(icon);
	
	m_Grouped = true;
}

void UndoHistory::closeActionGroup()
{
	m_Grouped = false;
}


void UndoHistory::registerAction( UndoAction* action )
{
	// add to undo history
	if( m_Grouped ) {
		UndoActionGroup *ag = dynamic_cast<UndoActionGroup *>(m_UndoActions.back());
		assert(ag);
		ag->add( action );
	} else {
		m_UndoActions.push_back( action );
		m_SignalHistoryChanged.emit( CHANGE_ADDUNDO );	
	}
	
	// redo invalid now, clear
	if( m_RedoActions.size() )
		clearRedoHistory();
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
	UndoAction& a = *m_UndoActions.back();
	a.undo(m_Project);
	m_RedoActions.push_back( m_UndoActions.back() );
	m_UndoActions.pop_back();

	m_SignalHistoryChanged.emit( CHANGE_UNDOACTION );	
}

void UndoHistory::redo()
{
	if( !m_RedoActions.size() ) return;
	UndoAction& a = *m_RedoActions.back();
	a.redo(m_Project);
	m_UndoActions.push_back( m_RedoActions.back() );
	m_RedoActions.pop_back();

	m_SignalHistoryChanged.emit( CHANGE_REDOACTION );	
}






UndoActionGroup::UndoActionGroup( UndoHistory& hist )
	: UndoAction( hist )
{
}

UndoActionGroup::~UndoActionGroup()
{
	while( m_Actions.size() ) {
		delete m_Actions.back();
		m_Actions.pop_back();
	}
}

void UndoActionGroup::add( UndoAction *action )
{
	m_Actions.push_back(action);
}

void UndoActionGroup::undo( Project& project )
{
	for( unsigned int i = 0; i < m_Actions.size(); i++ )
		m_Actions[i]->undo( project );
}

void UndoActionGroup::redo( Project& project )
{
	for( unsigned int i = 0; i < m_Actions.size(); i++ )
		m_Actions[i]->redo( project );
}


} // namespace ...

