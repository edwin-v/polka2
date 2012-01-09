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
	void setIcon( Glib::RefPtr<Gdk::Pixbuf> icon );
	Storage& setUndoData( std::string id );
	Storage& setRedoData( std::string id );

	// access to identifiers
	const Glib::ustring& source() const;
	const Glib::ustring& name() const;
	const Glib::RefPtr<Gdk::Pixbuf> icon() const;

	// access to data
	const std::string& undoId() const;
	const std::string& redoId() const;
	Storage& undoData();
	Storage& redoData();

protected:	
	UndoAction( UndoHistory& hist, Polka::Object& source );
	UndoAction( UndoHistory& hist );
	~UndoAction();

	virtual void undo( Project& project );
	virtual void redo( Project& project );

private:
	friend class UndoHistory;
	friend class UndoActionGroup;

	Glib::ustring m_Name;
	UndoHistory& m_History;
	Glib::ustring m_Source;
	Glib::RefPtr<Gdk::Pixbuf> m_refIcon;

	std::string m_UndoId, m_RedoId;
	Storage m_UndoStorage, m_RedoStorage;
};

} // namespace Polka

#endif // _POLKA_UNDOACTION_H_

