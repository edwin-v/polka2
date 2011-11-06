#ifndef _POLKA_EDITORWINDOW_H_
#define _POLKA_EDITORWINDOW_H_

#include <gtkmm/window.h>
#include <gtkmm/box.h>


namespace Polka {

class Editor;

class EditorWindow : public Gtk::Window 
{
public:
	EditorWindow( const std::string& _id );
	~EditorWindow();

	void setEditor( Editor *edt );
	Editor *editor();

protected:
	void on_show();
	void on_hide();
	bool on_delete_event( GdkEventAny *event );

private:
	std::string m_Id;
	// layout main window
	Gtk::VBox m_MainBox;
	Editor *m_Editor;
};

} // namespace Polka 

#endif // _POLKA_EDITORWINDOW_H_

