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
	UndoHistory( Project& project, int major, int minor );
	~UndoHistory();
	
	void createUndoPoint( const Glib::ustring& name, const Glib::RefPtr<Gdk::Pixbuf>& icon );
	UndoAction& createAction( Object& object );
	UndoAction& createAction();

	void clearHistory();
	void clearUndoHistory( unsigned int num_remain = 0 );
	void clearRedoHistory( unsigned int num_remain = 0 );

	const UndoAction& lastUndoAction() const;

	void undo();
	void redo();

	enum ChangeType { CHANGE_UNDOACTION, CHANGE_REDOACTION, CHANGE_ADDUNDO,
	                  CHANGE_ALLUNDO, CHANGE_ALLREDO };

	typedef sigc::signal<void, ChangeType> SignalHistoryChanged;
	SignalHistoryChanged signalHistoryChanged();
	
private:
	SignalHistoryChanged m_SignalHistoryChanged;
	std::vector<UndoAction*> m_UndoActions;
	std::vector<UndoAction*> m_RedoActions;
	Project& m_Project;
	int m_VersionMajor, m_VersionMinor;
	Glib::ustring m_UndoPointName;
	Glib::RefPtr<Gdk::Pixbuf> m_refUndoPointIcon;

	friend class UndoAction;
	
	void registerAction( UndoAction* action );
};

} // namespace Polka

#endif // _POLKA_UNDOHISTORY_H_

