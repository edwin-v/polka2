#ifndef _POLKA_OBJECTMANAGER_H_
#define _POLKA_OBJECTMANAGER_H_

#include <glibmm/ustring.h>
#include <gdkmm/pixbuf.h>
#include <gtkmm/widget.h>
#include <sigc++/sigc++.h>
#include <string>
#include <vector>
#include <map>

#include "Object.h"
#include "Editor.h"

namespace Polka { 

class EditorWindow;
class Project;
class ObjectPropertySheet;

class ObjectManager
{
public:
	static ObjectManager& instance();

	// ***************************
	//  object factory base class
	// ***************************
	class ObjectFactory {
	public:
		ObjectFactory( const Glib::ustring& _name,
		               const Glib::ustring& _location,
		               const std::string& _locId,
		               const Glib::ustring& _description,
		               const std::string& _classId,
		               const std::string& _editorId,
		               const std::string& _iconObj,
		               const std::string& _iconLoc = "",
		               const std::string& _iconSubLoc = "" );
		virtual ~ObjectFactory();
		virtual bool canCreate( const Project& project ) const;
		virtual Object *create( Project& _prj ) const = 0;
		virtual ObjectPropertySheet *createPropertySheet( Polka::Object& _obj ) const;
		virtual const Glib::ustring& name() const;
		virtual const Glib::ustring& location() const;
		virtual const std::string& locationId() const;
		virtual const std::string& classId() const;
		virtual const std::string& editorId() const;
		// icons
		virtual const Glib::RefPtr<Gdk::Pixbuf> locationIcon() const;
		virtual const Glib::RefPtr<Gdk::Pixbuf> subLocationIcon() const;
		virtual const Glib::RefPtr<Gdk::Pixbuf> objectIcon() const;
	private:
		Glib::ustring m_Name, m_Location, m_Description;
		std::string m_LocationId, m_ClassId, m_EditorId;
		std::string m_LocationIcon, m_SubLocationIcon, m_ObjectIcon;
	};

	// ***************************
	//  editor factory base class
	// ***************************
	class EditorFactory {
	public:
		EditorFactory( const std::string& _classId, const Glib::ustring& _name );
		virtual ~EditorFactory();
		virtual Editor *create() const = 0;
		virtual const std::string& classId() const;
		virtual const Glib::ustring& name() const;
	private:
		std::string m_ClassId;
		Glib::ustring m_Name;
	};
	// ***************************


	// object registration
	void registerObject( ObjectFactory *obj_factory );
	void registerEditor( EditorFactory *edt_factory );

	// object access
	const std::map<std::string, Glib::ustring>& containers() const;
	bool checkContainerId( const std::string& locId ) const;
	unsigned int numObjectTypes() const;
	const std::string& objectId( unsigned int nr ) const;
	const Glib::ustring& objectName( unsigned int nr ) const;
	const std::string& objectLocationId( unsigned int nr ) const;
	const std::string& objectLocationId( const std::string& id ) const;
	const Glib::ustring& objectLocation( const std::string& id ) const;

	const Glib::RefPtr<Gdk::Pixbuf> locationIcon( const std::string& locId ) const;
	const Glib::RefPtr<Gdk::Pixbuf> subLocationIcon( const std::string& locId ) const;
	
	const Glib::ustring& nameFromId( const std::string& id ) const;
	const Glib::ustring& editorNameFromId( const std::string& id ) const;
	const Glib::RefPtr<Gdk::Pixbuf> iconFromId( const std::string& id ) const;
	Polka::Object *createObject( Project& _prj, const std::string& id );
	
	const std::string& getObjectEditorId( const std::string& id ) const;
	Editor *getObjectEditor( const std::string& id );
	Editor *getEditor( const std::string& id );
	void hideEditors();
	bool canCreateObject( const std::string& id, const Project& prj ) const;

	ObjectPropertySheet *createObjectPropertySheet( Polka::Object& obj );

private:
	ObjectManager();
	~ObjectManager();

	void initPlugIns();
	const ObjectFactory *objectFactoryFromId( const std::string& id ) const;
	
	std::vector<ObjectFactory*> m_ObjectFactories;
	std::map<std::string, Glib::ustring> m_ContainerNames;
	std::map<std::string, EditorFactory*> m_EditorFactories;
	
	std::map<std::string, Editor*> m_Editors;
};


} // namespace Polka 

#endif // _POLKA_OBJECTMANAGER_H_
