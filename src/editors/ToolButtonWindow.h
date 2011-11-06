#ifndef _POLKA_TOOLBUTTONWINDOW_H_
#define _POLKA_TOOLBUTTONWINDOW_H_

#include <EditorWindow.h>
#include <gtkmm/table.h>
#include <gtkmm/box.h>
#include <gtkmm/expander.h>
#include <gtkmm/notebook.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/handlebox.h>


namespace Polka {
	
class ToolButtonWindow : public Gtk::HandleBox //public EditorWindow
{
public:
	ToolButtonWindow( int width = 2 );
	~ToolButtonWindow();
	
	void addTool( Glib::RefPtr<Gdk::Pixbuf> icon, Gtk::Widget *w = 0 );
	void activateTool( int id );
	
	// signals
	typedef sigc::signal<void, int> SignalSelectTool;
	SignalSelectTool signalSelectTool();

private:
	int m_Width;
	int m_CurrentTool;
	std::vector<Gtk::ToggleButton*> m_Buttons;
	std::vector<Gtk::Widget*> m_PageWidgets;
	
	Gtk::VBox m_MainBox;
	Gtk::Table m_ButtonGrid;
	Gtk::Expander m_Expand;
	Gtk::Notebook m_Pages;
	Gtk::VBox m_Empty;

	void toolButtonPressed( int id );

	// signal objects
	SignalSelectTool m_SignalSelectTool;

};


} // namespace Polka

#endif // _POLKA_TOOLBUTTONWINDOW_H_
