#ifndef _POLKA_OBJECT_H_
#define _POLKA_OBJECT_H_

#include <glibmm/ustring.h>
#include <sigc++/signal.h>
#include <string>
#include <list>
#include <map>
#include "Storage.h"

namespace Polka {

class Object;
class Editor;
class Project;
class UndoAction;
class ObjectPropertiesDialog;

struct Dependency {
	std::string type;
	const Object *object;
};

typedef std::map<int, Dependency> DependencyMap;
typedef std::list<const Object*> ConstObjectList;
typedef std::list<const Object*> ObjectList;

class Object
{
public:
	Object( Project& _prj, const std::string& _id, bool delayed_update = false );
	virtual ~Object();

	// object identification
	const std::string& id() const;
	guint32 funid() const;
	const Glib::ustring& name() const;
	const Glib::ustring& comments() const;
	const std::string& dependencyType( int id ) const;
	
	void setName( const Glib::ustring& name );
	void setComments( const Glib::ustring& text );

	// ownership
	Project& project();
	bool canRemove() const;
	Editor *editor();
	
	// storage
	int save( Storage& s );
	int load( Storage& s );

	// undo
	virtual void undo( const std::string& id, Storage& s );
	virtual void redo( const std::string& id, Storage& s );

	void setInitMode( bool val = true );

protected:
	bool registerDependency( int id, const std::string& typespec, const Object *object = 0 );
	bool unregisterDependency( int id );
	bool setDependency( int id, const Object *object );
	const Object *dependency( int id ) const;

	// object update
	void update( bool full = true );
	virtual void onUpdate( bool full );
	
	// storage base for both file and internal storage
	virtual int store( Storage& s );
	virtual int restore( Storage& s );
	
private:
	// object dependencies
	DependencyMap m_Dependencies;
	ConstObjectList m_UsedBy;
	// object identification
	std::string m_Id;
	guint32 m_FUNID;
	Glib::ustring m_Name;
	Glib::ustring m_Comments;
	// object state
	Project& m_Project;
	bool m_InitMode, m_Dirty;
	bool m_AllowUpdateDelay;
	// object editor
	Editor *m_pEditor;
	
	// signal dependency selection
	void setDependencyOf( const Object *object );
	bool unsetDependencyOf( const Object *object );

	// object updates
	void forceUpdate();
	void setDirty();
	bool canDelayUpdate() const;

	void attachEditor( Editor* editor );
	void detachEditor( Editor* editor );
	
	friend class Editor;
	// friend ObjectPropertiesDialog so it can display internal
	friend class ObjectPropertiesDialog;
	
	// private undo entries
	friend class UndoAction;
	void objectUndo( const std::string& id, Storage& s );
	void objectRedo( const std::string& id, Storage& s );
	void objectStorageAction( Storage& s );
};

} // namespace Polka

#endif // _POLKAOBJECT_H_
