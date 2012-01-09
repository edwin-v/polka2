#ifndef _POLKA_UNDOHISTORY_H_
#define _POLKA_UNDOHISTORY_H_

#include <sigc++/sigc++.h>
#include <vector>
#include "UndoAction.h"


namespace Polka {
	
class Project;
	
class UndoHistory
{
public:
	UndoHistory( Project& project );
	~UndoHistory();
	
	UndoAction& createAction( Object& object );
	void openActionGroup();
	void closeActionGroup();

	void clearHistory();
	void clearUndoHistory( unsigned int num_remain = 0 );
	void clearRedoHistory( unsigned int num_remain = 0 );

	const UndoAction& lastUndoAction() const;

	void undo();
	void redo();

	enum ChangeType { CHANGE_UNDOACTION, CHANGE_REDOACTION, CHANGE_ADDUNDO, CHANGE_NEWDISPLAY,
	                  CHANGE_ALLUNDO, CHANGE_ALLREDO, CHANGE_ALL };

	typedef sigc::signal<void, ChangeType> SignalHistoryChanged;
	SignalHistoryChanged signalHistoryChanged();
	
private:
	SignalHistoryChanged m_SignalHistoryChanged;
	std::vector<UndoAction*> m_UndoActions;
	std::vector<UndoAction*> m_RedoActions;
	Project& m_Project;
	bool m_Grouped;

	friend class UndoAction;
	
	void registerAction( UndoAction* action );
	void displayChange( UndoAction *action );
};

} // namespace Polka

#endif // _POLKA_UNDOHISTORY_H_

