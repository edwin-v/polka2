#ifndef _POLKA_EDITOR_H_
#define _POLKA_EDITOR_H_

#include <gtkmm/grid.h>
#include <gtkmm/label.h>

#include "EditorWindow.h"

namespace Polka {

class Object;


class Editor : public Gtk::Grid
{
public:
	Editor( const std::string& _id );
	virtual ~Editor();

	void setObject( Polka::Object *obj );
	void treeUpdated();
	virtual void objectUpdated( bool full = true );
	virtual const Glib::ustring& menuString();	
	
	bool isMainEditor() const;
	void activate();
	virtual void hide();

	Glib::ustring title() const;

	// signals
	//typedef sigc::signal<void, const Glib::ustring> SignalTitleChanged;
	//SignalTitleChanged signalTitleChanged();

protected:
	void setMainEditor( bool value );
	virtual void assignObject( Polka::Object *obj ) = 0;
	virtual void updateTreeNames();
	
private:
	std::string m_Id;
	bool m_MainEditor;
	EditorWindow m_Window;
	Polka::Object *m_pObject;
	
	//SignalTitleChanged m_SignalTitleChanged;
	
	void updateTitle();
};

} // namespace Polka

#endif // _POLKA_EDITOR_H_
