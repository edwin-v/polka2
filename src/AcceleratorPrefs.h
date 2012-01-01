#ifndef _POLKA_ACCELERATORPREFS_H_
#define _POLKA_ACCELERATORPREFS_H_

#include <gtkmm/grid.h>
#include <gtkmm/paned.h>
#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include "ActionGrabFrame.h"

namespace Polka {


class AcceleratorPrefs : public Gtk::Grid
{
public:
	AcceleratorPrefs();
	~AcceleratorPrefs();
	
protected:

private:
	// simple single column tree
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

		ModelColumns(){ add(m_ColName); add(m_Context); add(m_Id); }

		Gtk::TreeModelColumn< Glib::ustring > m_ColName;
		Gtk::TreeModelColumn< std::string > m_Context;
		Gtk::TreeModelColumn< std::string > m_Id;
	};

	// project tree
	ModelColumns m_Cols;
	Glib::RefPtr<Gtk::TreeStore> m_refTreeModel;

	Gtk::Paned m_AccPane;
	Gtk::TreeView m_AccTree;
	Gtk::RadioButton m_RadioDisable, m_RadioLink, m_RadioSet;
	Gtk::ComboBoxText m_ComboLinks;
	Gtk::Label m_NameLabel, m_TypeLabel, m_AccelLabel;
	Gtk::TextView m_DescText;
	ActionGrabFrame m_GrabFrame;
	
	bool m_Updating;
	
	void fillTree();
	void reset();
	Glib::ustring keyDisplayName( guint key, Gdk::ModifierType mods );
	void onAccSelected();
	void onRadioChange();
	void onLinkChange();
	
	void setButton( int button, int modifiers );
	void setKey( int key, int modifiers );
};

} // namespace Polka

#endif // _POLKA_ACCELERATORPREFS_H_

