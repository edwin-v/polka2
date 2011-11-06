#ifndef _POLKA_OBJECTPROPERTIESDIALOG_H_
#define _POLKA_OBJECTPROPERTIESDIALOG_H_

#include <gtkmm/dialog.h>
#include <gtkmm/notebook.h>
#include <gtkmm/frame.h>
#include <gtkmm/entry.h>
#include <gtkmm/textview.h>


namespace Polka {

class Object;
class ObjectPropertySheet;

class ObjectPropertiesDialog : public Gtk::Dialog
{
public:
	ObjectPropertiesDialog( Polka::Object& obj_ );
	~ObjectPropertiesDialog();

protected:
	void on_response( int id );

private:
	Polka::Object& m_Object;
	
	// tabs
	Gtk::Notebook m_Tabs;
	// info tab
	Gtk::VBox m_MainBox;
	Gtk::Label m_NameLabel;
	Gtk::Entry m_NameEntry;
	Gtk::Label m_CommentsLabel;
	Gtk::TextView m_CommentsText;
	// properties tab widget
	ObjectPropertySheet *m_pProperties;
	// links
	Gtk::VBox m_LinksBox;
	Gtk::Label    m_UsesLabel;
	Gtk::TextView m_UsesText;
	Gtk::Label    m_UsedByLabel;
	Gtk::TextView m_UsedByText;
	
	bool m_InfoChanged, m_PropertiesChanged;
	
	void setInfoChanged();
	void setPropertiesChanged( bool value );
	void updateSensitivity();
	
};

} // namespace Polka 

#endif // _POLKA_OBJECTPROPERTIESDIALOG_H_
