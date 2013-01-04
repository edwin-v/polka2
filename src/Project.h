#ifndef _POLKA_PROJECT_H_
#define _POLKA_PROJECT_H_

#include "ProjectTreeStore.h"
#include "UndoHistory.h"
#include "Object.h"

#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/menu.h>
#include <glibmm/ustring.h>
#include <gdkmm/pixbuf.h>
#include <string>

namespace Polka {

extern const std::string MIME_BASE;
extern const std::string MIME_OBJNAME;

class Editor;
class Storage;

class Project : public Gtk::TreeView 
{
public:
	Project( Glib::RefPtr<Gtk::UIManager> ui_manager );
	Project( Glib::RefPtr<Gtk::UIManager> ui_manager, const std::string& filename );
	~Project();
		
	// project storage
	int loadFromFile( const std::string& filename );
	int saveToFile( const std::string& filename );
	int save();
	bool modified() const;
	const std::string& filename() const;
	const Glib::ustring& projectName() const;
	
	// global project
	UndoHistory& undoHistory();
	void projectUndo( const std::string& id, Storage& s );
	void projectRedo( const std::string& id, Storage& s );
	void projectUndoAction( const std::string& action, Storage& s );
	
	// interface
	void startRename();
	void deleteObject();
	Polka::Object *editObject( const Glib::ustring& name );
	Polka::Object *editObject( guint32 funid );
	void setObjectName( Polka::Object& obj, const Glib::ustring& name );
	void setObjectComments( Polka::Object& obj, const Glib::ustring& comments );

	// object access
	Polka::Object *getObject();
	Polka::Object *getObject( const Gtk::TreeModel::Path& path ) const;

	Polka::Object *findObjectOfType( const std::string& id ) const;
	Polka::Object *findObjectOfTypes( const std::string& ids ) const;
	Polka::Object *findObject( const Glib::ustring& name ) const;
	Polka::Object *findObject( guint32 funid ) const;
	void findAllObjectsOfType( const std::string& id, std::vector<Polka::Object*>& vec ) const;

	// object creation
	Glib::ustring createUniqueName( const Glib::ustring& prefix );
	bool checkObjectRequirements( const std::string& id );
	Polka::Object *createNewObject( const std::string& id );
	guint32 getNewFunid() const;

	// Edit signal
	typedef sigc::signal<void, Editor*> SignalEditObject;
	SignalEditObject signalEditObject();
	typedef sigc::signal<void> SignalTreeUpdate;
	SignalTreeUpdate signalTreeUpdate();


protected:
	// handlers for popup menu
	virtual bool on_button_press_event(GdkEventButton *event);
	// drag handler
	virtual void on_drag_data_get( const Glib::RefPtr<Gdk::DragContext>& dc, Gtk::SelectionData& data, guint info, guint time );

private:
	// Tree model columns
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		ModelColumns()
		{	add(m_Name);
			add(m_BaseLocation);
			add(m_pObject);
			add(m_rpIcon);  }

		Gtk::TreeModelColumn<Glib::ustring> m_Name;
		Gtk::TreeModelColumn<std::string> m_BaseLocation;
		Gtk::TreeModelColumn<Polka::Object*> m_pObject;
		Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > m_rpIcon;
  	};

	// basic project vars
	std::string m_Filename;
	Glib::ustring m_ProjectName;
	std::list<Polka::Object*> m_Objects;
	// project undo storage
	UndoHistory m_History;
	UndoAction *m_pImportAction;
	// project tree
	ModelColumns m_Cols;
	Glib::RefPtr<Gtk::TreeStore> m_rpTreeModel;
	Glib::RefPtr<Gtk::TreeView> m_rpTree;
	Gtk::CellRendererText *m_pNameCellRenderer;
	// item popup menu
	Glib::RefPtr<Gtk::UIManager> m_refUIManager;
	Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
	Gtk::Menu m_Popup;
	Gtk::UIManager::ui_merge_id m_CreateMenuId;
	// signals
	SignalEditObject m_SignalEditObject;
	SignalTreeUpdate m_SignalTreeUpdate;
	
	guint32 m_ForceFUNID;
	
	
	// signal handlers
	void onEdit();
	void onCreateFolder();
	void onCreateObject();
	void onNameEdited(const Glib::ustring& path_txt, const Glib::ustring& new_text);
	void onSelectionChanged();

	// tree functions
	void init();

	// object creation
	void renameLocation( const Glib::ustring& from, const Glib::ustring& to );
	void createFolder( const Glib::ustring& location, const Glib::ustring& name );
	Gtk::TreeModel::iterator createFolder( Gtk::TreeModel::iterator location, const Glib::ustring& name );
	Gtk::TreeModel::iterator createNewObject( const Glib::ustring& location, const Glib::ustring& name, const std::string& type, guint32 funid = 0 );
	Gtk::TreeModel::iterator createObjectWithUndoAction( Gtk::TreeModel::iterator location, const std::string& type );
	Gtk::TreeModel::iterator createObject( const Glib::ustring& location, const Glib::ustring& name, const std::string& type, Storage& s );
	Gtk::TreeModel::iterator createObject( Gtk::TreeModel::iterator location, const Glib::ustring& name, const std::string& type );
	void deleteLocation( const Glib::ustring& name );
	void deleteLocation( Gtk::TreeModel::iterator location );
	
	Gtk::TreeModel::Row createLocation( const std::vector<Glib::ustring>& path, const std::string& baseType );
	void getNames( std::vector<Glib::ustring>& list, const Gtk::TreeNodeChildren& items );
	Gtk::TreeNodeChildren::iterator findLocation( const Glib::ustring& name, const Gtk::TreeNodeChildren& items );
	Gtk::TreeNodeChildren::iterator findBaseLocation( const std::string& type );

	int saveTreeRow( Storage& s, const Gtk::TreeModel::Children& c );

};


} // namespace Polka

#endif // _POLKA_PROJECT_H_
