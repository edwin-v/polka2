#ifndef _POLKA_PREFERENCESDIALOG_H_
#define _POLKA_PREFERENCESDIALOG_H_

#include <gtkmm/dialog.h>
#include <gtkmm/notebook.h>
#include <gtkmm/treeview.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include "AcceleratorPrefs.h"

namespace Polka {


class PreferencesDialog : public Gtk::Dialog
{
public:
	PreferencesDialog();
	~PreferencesDialog();
	
protected:
	void on_response( int id );

private:
	// list store model columns
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:

		ModelColumns(){ add(m_ColIcon); add(m_ColName); add(m_PageId); }

		Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > m_ColIcon;
		Gtk::TreeModelColumn< Glib::ustring > m_ColName;
		Gtk::TreeModelColumn< int > m_PageId;
	};

	Glib::RefPtr<Gtk::ListStore> m_refListModel;

	Gtk::HBox m_MainBox;
	Gtk::TreeView m_CategoryList;
	Gtk::Notebook m_PrefsPages;
	ModelColumns m_Columns;

	// pref widgets
	AcceleratorPrefs m_AcceleratorPrefs;

	void addPrefsPage( Glib::RefPtr<Gdk::Pixbuf> icon, const Glib::ustring& name, Gtk::Widget& w );
};

} // namespace Polka

#endif // _POLKA_PREFERENCESDIALOG_H_

