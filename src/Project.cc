/*
	Copyright (C) 2013 Edwin Velds

    This file is part of Polka 2.

    Polka 2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Polka 2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Polka 2.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Project.h"
#include "ObjectManager.h"
#include "Object.h"
#include "Storage.h"
#include "Functions.h"
#include "StorageHelpers.h"
#include "ResourceManager.h"
#include "ExportManager.h"
#include <gtkmm/messagedialog.h>
#include <glibmm/i18n.h>
#include <iostream>
#include <algorithm>
#include <assert.h>

namespace Polka {

static const int FILE_VERSION_MAJOR = 0;
static const int FILE_VERSION_MINOR = 2;
static const char *FILE_ID_STRING = "POLKA2_PROJECT_FILE";

#define _MIME_BASE "application/x-polka2"
const std::string MIME_BASE = _MIME_BASE;
const std::string MIME_OBJNAME = _MIME_BASE"-objectname-";


/*
 * Implementation of Project
 */


Project::Project( Glib::RefPtr<Gtk::UIManager> ui_manager )
	: Gtk::TreeView(), m_History(*this, FILE_VERSION_MAJOR, FILE_VERSION_MINOR),
	  m_pImportAction(0), m_refUIManager( ui_manager ), m_CreateMenuId(0),
	  m_ForceFUNID(0)
{
	init();
}

Project::Project( Glib::RefPtr<Gtk::UIManager> ui_manager, const std::string& filename )
	: Gtk::TreeView(), m_History(*this, FILE_VERSION_MAJOR, FILE_VERSION_MINOR),
	  m_pImportAction(0), m_refUIManager( ui_manager ), m_CreateMenuId(0),
	  m_ForceFUNID(0)
{
	init();
	loadFromFile( filename );
}

Project::~Project()
{
	// delete objects
	while( m_Objects.size() ) {
		std::list<Polka::Object*>::iterator oit = m_Objects.begin();
		while( oit != m_Objects.end() ) {
			if( (*oit)->canRemove() ) {
				// object can be deleted
				delete *oit;
				oit = m_Objects.erase(oit);
			} else {
				// object still is a dependency, skip
				++oit;
			}
		}
	}

	unset_model();
	m_rpTreeModel->clear();
}

void Project::init()
{
	// defaults for empty project
	m_Filename.clear();
	m_ProjectName = _("Unnamed project");
	
	// create the necessary object actions
	m_refActionGroup = Gtk::ActionGroup::create("TreeContext");

	m_refActionGroup->add(Gtk::Action::create("ContextEdit", _("_Edit ...")),
	                      sigc::mem_fun(*this, &Project::onEdit));
	m_refActionGroup->add(Gtk::Action::create("ContextCreateFolder", _("Create _Folder")),
	                      sigc::mem_fun(*this, &Project::onCreateFolder));
	m_refActionGroup->add(Gtk::Action::create("ContextExport", _("E_xport")),
	                      sigc::mem_fun(*this, &Project::onExport));
	ObjectManager& om = ObjectManager::get();
	for( unsigned int i = 0; i < om.numObjectTypes(); i++ )
		m_refActionGroup->add(Gtk::Action::create(
				Glib::ustring("ContextCreateObject") + om.objectId(i),
				_("Create ") + om.objectName(i) ),
				sigc::mem_fun(*this, &Project::onCreateObject));
	
	m_refUIManager->insert_action_group(m_refActionGroup);
	
	// create base context menu
	Glib::ustring popup_text = 
		"<ui>"
		"  <popup name='TreePopup'>"
		"    <menuitem action='ContextEdit'/>"
		"    <separator/>"
		"    <menuitem action='ContextCreateFolder'/>"
		"    <placeholder name='EditAdditions'/>"
		"    <separator/>"
		"    <menuitem action='EditCut'/>"
		"    <menuitem action='EditCopy'/>"
		"    <menuitem action='EditPaste'/>"
		"    <menuitem action='EditDelete'/>"
		"    <separator/>"
		"    <menuitem action='ContextExport'/>"
		"    <separator/>"
		"    <menuitem action='EditRename'/>"
		"    <menuitem action='EditProperties'/>"
		"  </popup>"
		"</ui>";	

	m_refUIManager->add_ui_from_string(popup_text);

	
	// Create the Tree model:
	m_rpTreeModel = ProjectTreeStore::create(m_Cols);

	// project name at top level
	Gtk::TreeModel::Row topRow = *(m_rpTreeModel->append());
	topRow[m_Cols.m_Name] = m_ProjectName;
	topRow[m_Cols.m_pObject] = 0;

	// Fill the TreeView's model with existing containers
	const std::map<std::string, Glib::ustring>& containers = ObjectManager::get().containers();
	std::map<std::string, Glib::ustring>::const_iterator it = containers.begin();
	while( it != containers.end() ) {
		Gtk::TreeModel::Row row = *(m_rpTreeModel->append(topRow.children()));
		row[m_Cols.m_Name] = it->second;
		row[m_Cols.m_pObject] = 0;
		row[m_Cols.m_BaseLocation] = it->first;
		row[m_Cols.m_rpIcon] = ObjectManager::get().locationIcon( it->first );
		it++;
	}
	
	// init the tree
	set_model( m_rpTreeModel );
	Gtk::TreeView::Column* pColumn =
		Gtk::manage( new Gtk::TreeView::Column(_("Project objects")) );
	pColumn->pack_start(m_Cols.m_rpIcon, false); //false = don't expand.
	pColumn->pack_start(m_Cols.m_Name);
	append_column(*pColumn);
	property_headers_visible() = true;
	expand_all();
	
	// attach a name edit checker
	auto crList = get_column(0)->get_cells();
	auto crit = crList.begin();
	crit++;
	assert( crit != crList.end() );
	m_pNameCellRenderer = dynamic_cast<Gtk::CellRendererText*>(*crit);
	assert(m_pNameCellRenderer);
	m_pNameCellRenderer->signal_edited().connect(
		sigc::mem_fun(*this, &Project::onNameEdited) );
	
	
	onSelectionChanged();
	// attach selection signal
	get_selection()->signal_changed().connect(
    		sigc::mem_fun(*this, &Project::onSelectionChanged) );

	set_reorderable();
}

UndoHistory& Project::undoHistory()
{
	return m_History;
}

void Project::onSelectionChanged()
{
	// possible menu's
	//   Top           Folder         Object
        //
	//                 Create Folder
	//                 Create ....
	//                 Create ....    Edit
	//                 ------         ------
	//   Cut           Cut            Cut
	//   Copy          Copy           Copy
	//   Paste         Paste          Paste
	//   Delete        Delete         Delete
	//                                ------
	//                                Export
	//   ------        ------         ------
	//   Rename        Rename         Rename
	//   Properties    Properties     Properties

	// remove existing 
	if( m_CreateMenuId ) m_refUIManager->remove_ui( m_CreateMenuId );

	// allowed options
	bool edit = false, folder = false,
	     cut = false, copy = false, paste = false, del = false,
	     exportobj = false, rename = false, props = false;
	
	Gtk::TreeModel::iterator rit = get_selection()->get_selected();
	
	Polka::Object *obj = 0;//(*rit)[m_Cols.m_pObject];
	if( rit ) {
		Gtk::TreeModel::Row row = *rit;
		
		if( !row.parent() ) {
			// top level
			m_refUIManager->get_action("/ui/TreePopup/ContextEdit")->set_visible(false);
			m_refUIManager->get_action("/ui/TreePopup/ContextCreateFolder")->set_visible(false);
		} else if( row[m_Cols.m_pObject] ) {
			// row is an object
			cut = copy = rename = props = true;
			/*Polka::Object **/obj = row[m_Cols.m_pObject];
			const std::string& editId = ObjectManager::get().getObjectEditorId( obj->id() );
			edit = !editId.empty();
			// delete if nothing depends on it
			if( obj->canRemove() )
				del = true;
			// can this object be exported?
			exportobj = ExportManager::get().canExport( *obj );
		} else {
			// row is an object folder
			folder = props = true;
			// add actions for possible objects to placeholder menu
			Glib::ustring popup_text = 
				"<ui>"
				"  <popup name='TreePopup'>"
				"    <placeholder name='EditAdditions'>";
			
			// allow delete if empty
			if( row.children().size() == 0 ) 
				if( (*row.parent()).parent() ) del = true;
			// add objects allowed in this location
			ObjectManager& om = ObjectManager::get();
			const std::string& locId = row[m_Cols.m_BaseLocation];
			for( unsigned int i = 0; i < om.numObjectTypes(); i++ )
				if( om.objectLocationId(i) == locId )
					popup_text += "      <menuitem action='ContextCreateObject" + om.objectId(i) + "'/>";

			popup_text +=
				"    </placeholder>"
				"  </popup>"
				"</ui>";
			m_CreateMenuId = m_refUIManager->add_ui_from_string(popup_text);
			rename = row.parent()->parent() != 0;
		}
	}
		
	m_refUIManager->get_action("/ui/TreePopup/ContextEdit")->set_visible(edit);
	m_refUIManager->get_action("/ui/TreePopup/ContextCreateFolder")->set_visible(folder);
	m_refUIManager->get_action("/ui/TreePopup/EditCut")->set_sensitive(cut);
	m_refUIManager->get_action("/ui/TreePopup/EditCopy")->set_sensitive(copy);
	m_refUIManager->get_action("/ui/TreePopup/EditPaste")->set_sensitive(paste);
	m_refUIManager->get_action("/ui/TreePopup/EditDelete")->set_sensitive(del);
	m_refUIManager->get_action("/ui/TreePopup/ContextExport")->set_visible(exportobj);
	m_refUIManager->get_action("/ui/TreePopup/EditRename")->set_sensitive(rename);
	m_refUIManager->get_action("/ui/TreePopup/EditProperties")->set_sensitive(props);
	m_pNameCellRenderer->set_property("editable", rename);

	// init target types for dragging
	std::vector<Gtk::TargetEntry> dragTargets;
	dragTargets.push_back( Gtk::TargetEntry("GTK_TREE_MODEL_ROW") ); // necessary for row ordering
	// targets for name strings
	dragTargets.push_back( Gtk::TargetEntry("STRING") );
	dragTargets.push_back( Gtk::TargetEntry("text/plain") );
	// support others for data objects
	if( obj ) {
		dragTargets.push_back( Gtk::TargetEntry( MIME_OBJNAME + obj->id() ) );
	}
	// set source
	enable_model_drag_source(dragTargets, Gdk::MODIFIER_MASK, Gdk::ACTION_COPY | Gdk::ACTION_LINK);
}

void Project::onNameEdited(const Glib::ustring& path_txt, const Glib::ustring& new_text)
{
	// get list of existing names
	std::vector<Glib::ustring> names;
	getNames(names, m_rpTreeModel->children());
	
	Gtk::TreePath path(path_txt);

	//Get the row from the path
	Gtk::TreeModel::iterator it = m_rpTreeModel->get_iter(path);
	if(it) {
		Gtk::TreeModel::Row row = *it;
		if( row[m_Cols.m_Name] == new_text || new_text.empty() ) {
			// same, do nothing
		} else if( std::find( names.begin(), names.end(), new_text ) == names.end() ) {
			// create undo information
			Glib::ustring title = _("Renamed '") + row[m_Cols.m_Name] + _("' to '") + new_text + _("'");
			m_History.createUndoPoint( title, row[m_Cols.m_rpIcon] );
			UndoAction& action = m_History.createAction();
			action.setIcon( row[m_Cols.m_rpIcon] );
			action.setName( title );
			storageRename( action.setUndoData( ACTION_RENAME ), new_text, row[m_Cols.m_Name] );
			storageRename( action.setRedoData( ACTION_RENAME ), row[m_Cols.m_Name], new_text );
			// change the name
			row[m_Cols.m_Name] = new_text;
			Polka::Object *obj = row[m_Cols.m_pObject];
			if(obj) obj->setName( new_text );
			// signal update to tree
			m_SignalTreeUpdate.emit();
		} else {
			// error dialog for duplicate name
			Gtk::MessageDialog dialog(
        			"Duplicate name. Please use a unique name for everything.",
	        		false, Gtk::MESSAGE_ERROR);
			dialog.run();

			// restart editing
			set_cursor(path, *get_column(0), *m_pNameCellRenderer, true );
		}
	}
}

void Project::setObjectName( Polka::Object& obj, const Glib::ustring& name )
{
	if( obj.name() != name ) {
		// create undo
		UndoAction& action = m_History.createAction();
		action.setIcon( ObjectManager::get().iconFromId( obj.id() ) );
		action.setName( _("Renamed '") + obj.name() + _("' to '") + name + _("'") );
		storageRename( action.setUndoData( ACTION_RENAME ), obj.funid(), obj.name() );
		storageRename( action.setRedoData( ACTION_RENAME ), obj.funid(), name );
		// rename object
		renameLocation( obj.name(), name );
	}
}

void Project::setObjectComments( Polka::Object& obj, const Glib::ustring& comments )
{
	if( obj.comments() != comments ) {
		// create undo
		UndoAction& action = m_History.createAction();
		action.setIcon( ObjectManager::get().iconFromId( obj.id() ) );
		action.setName( _("Changed '") + obj.name() + _("' comments") );
		storageComments( action.setUndoData( ACTION_COMMENTS ), obj.funid(), obj.comments() );
		storageComments( action.setRedoData( ACTION_COMMENTS ), obj.funid(), comments );
		// change comments
		obj.setComments( comments );
	}
}

bool Project::on_button_press_event(GdkEventButton *event)
{
	// call base class, to allow normal handling,
	// such as allowing the row to be selected by the right-click
	bool ret = TreeView::on_button_press_event(event);
	// was this a right click
	if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ) {
		// then update and show the tree popup
		Gtk::Menu *popup = dynamic_cast<Gtk::Menu*>(
        		m_refUIManager->get_widget("/TreePopup"));
		popup->popup(event->button, event->time);
	}
	return ret;
}

void Project::on_drag_data_get( const Glib::RefPtr<Gdk::DragContext>& dc, Gtk::SelectionData& data, guint info, guint time )
{
	if( data.get_target() == "STRING" || data.get_target() == "text/plain" ||
		data.get_target().substr(0, MIME_OBJNAME.size()) == MIME_OBJNAME )
	{
		Gtk::TreeModel::iterator rit = get_selection()->get_selected();
		if( rit ) {
			Gtk::TreeModel::Row row = *rit;
			const Glib::ustring& name = row[m_Cols.m_Name];
			data.set( data.get_target(), 8, (const guint8*)name.data(), name.bytes() );
		}
	} else
		Gtk::TreeView::on_drag_data_get(dc, data, info, time);
}


Project::SignalEditObject Project::signalEditObject()
{
	return m_SignalEditObject;
}

Project::SignalTreeUpdate Project::signalTreeUpdate()
{
	return m_SignalTreeUpdate;
}

int Project::loadFromFile( const std::string& filename )
{
	Storage s( filename );
	int err = s.load();
 std::cout << err << std::endl;
	if( err ) {
		// handle load error
		return err;
	};
	// file loaded, check id
	s.getFileIdentification(FILE_ID_STRING);
	if( s.versionMajor() == -1 ) return 1;
	// get name
	if( !s.findItem("PROJECT_NAME") ) return 1;
	if( !s.checkFormat("S") ) return 1;
	m_ProjectName.assign( s.stringField(0) );
	
	ObjectManager& om = ObjectManager::get();
 std::cout << "obj" << std::endl;
	
	// edited object list
	std::vector<Glib::ustring> editedObjs;
	
	// read objects
	bool readObjs = s.findObject();
	while( readObjs ) {
		const std::string& type = s.objectType();
		Storage& objS = s.object();
		// all objects have a name and location
		if( !objS.findItem("OBJECT_NAME") ) return 1;
		if( !objS.checkFormat("S") ) return 1;
		std::string nameField = objS.stringField(0);

		// object tree location
		if( !objS.findItem("LOCATION") ) return 1;
		if( !objS.checkFormat("[S]") ) return 1;
		std::vector<Glib::ustring> locationField;
		for( int i = 0; i < objS.arraySize(); i++ )
			locationField.push_back( Glib::ustring(objS.stringField( i, 0 ) ) );
		
		// check for standard names
		Gtk::TreeModel::Row newRow;
		if( type == "EMPTY_LOCATION" ) {
			// if location id exists
			if( !om.checkContainerId( nameField ) ) return 1;
			// create the location
			newRow = createLocation( locationField, nameField );
		} else {
			// find location for type
			const std::string& locId = om.objectLocationId(type);
			if( locId.empty() ) return 1;
			// create location
			Gtk::TreeModel::Row row = createLocation( locationField, locId );
			// load and add object of type
       		Polka::Object *newObj = om.createObject( *this, type );
       		m_Objects.push_back( newObj );
			// create a row
			Gtk::TreeModel::iterator newIt = m_rpTreeModel->append(row.children());
			newRow = *newIt;
			newRow[m_Cols.m_Name] = createUniqueName( nameField );
			newRow[m_Cols.m_pObject] = newObj;
			newRow[m_Cols.m_BaseLocation] = Glib::ustring();
			newRow[m_Cols.m_rpIcon] = om.iconFromId( type );
			newObj->setName( newRow[m_Cols.m_Name] );
			// let object load
			newObj->load( objS );
			newObj->setInitMode(false);
			// edited object?
			if( objS.findItem("IN_EDITOR") ) {
				if( objS.checkFormat("I") ) {
					editedObjs.push_back( newObj->name() );
				}
			}
		}
		// expand rows
		if( objS.findItem("CONTAINERS_EXPANDED") ) {
			if( objS.checkFormat("I") ) {
				Gtk::TreeModel::iterator expit = newRow.parent();
				for( int i = 0; i < objS.integerField(0); i++ ) {
					if( !expit ) break;
					expit = expit->parent();
				}
				if( expit ) {
					Gtk::TreeModel::Path p = m_rpTreeModel->get_path(expit);
					expand_to_path(p);
				}
			}
		}
		// next object
		readObjs = s.findNextObject();		
	}
	
	m_Filename = filename;
	
	// with everything ok, open object editors
	for( guint i = 0; i < editedObjs.size(); i++ )
		editObject( editedObjs[i] );
	
	return 0;
}

int Project::saveToFile( const std::string& filename )
{
	m_Filename = filename;
	int r = save();
	
	if( r ) m_Filename.clear();

	return r;
}

int Project::save()
{
	Storage s( m_Filename );
	// set file id
 	s.setFileIdentification(FILE_ID_STRING, FILE_VERSION_MAJOR, FILE_VERSION_MINOR );
 	s.createItem("PROJECT_NAME", "S");
 	s.setField(0, m_ProjectName.raw() );
 	// store objects
	saveTreeRow( s, m_rpTreeModel->children()[0].children() );
	// save to file
	int r = s.save();
	return r;
}

int Project::saveTreeRow( Storage& s, const Gtk::TreeModel::Children& c )
{
	// loop over children
	Gtk::TreeModel::Children::iterator it = c.begin();
	while( it != c.end() ) {
		// get row
		Gtk::TreeModel::Row row = *it;
		// get path
		std::vector<Glib::ustring> path;
		Gtk::TreeModel::Row srow = *it;
		while(true) {
			if( !srow.parent() ) break;
			srow = *srow.parent();
			path.push_back( srow[m_Cols.m_Name] );
		}

		// get object
		Polka::Object *obj = /*(Polka::Object*)*/row[m_Cols.m_pObject];
		bool hasChildren = row.children().size();
		if( !obj && hasChildren  ) {
			// container has childern, store them
			saveTreeRow( s, row.children() );
		} else if( obj || (!hasChildren && path.size() > 1 ) ) {
			// row is object or empty location create object
			Storage& subS = s.createObject( obj?obj->id():"EMPTY_LOCATION" );
			// write location
			subS.createItem("LOCATION", "[S]");
			int f = 0;
			for( int i = path.size()-3; i >= 0; i-- )
				subS.setField( f++, 0, path[i].raw() );
			if( obj ) {
				// mark if object in editor
				if( obj->editor() ) {
					subS.createItem("IN_EDITOR", "I");
					subS.setField(0, 1);
				}
				// write object
				obj->save( subS );
			} else {
				// location name added to location
				Glib::ustring locName( row[m_Cols.m_Name] );
				subS.setField( f, 0, locName.raw() );
				// store location id in name
				subS.createItem("OBJECT_NAME", "S");
				subS.setField( 0, row[m_Cols.m_BaseLocation] );
			}
			// write if unfolded
			if( it == c.begin() ) {
				subS.createItem("CONTAINERS_EXPANDED", "I");
				f = 0;
				Gtk::TreeModel::iterator expit = row.parent();
				while( expit ) {
					Gtk::TreeModel::Path p = m_rpTreeModel->get_path(expit);
					if( row_expanded(p) ) break;
					f++;
					expit = expit->parent();
				}				
				subS.setField( 0, f );
			}
			// edited
		}
		++it;
	}
	return 0;
}

const std::string& Project::filename() const
{
	return m_Filename;
}

const Glib::ustring& Project::projectName() const
{
	return m_ProjectName;
}

void Project::onEdit()
{
	Gtk::TreeModel::iterator rit = get_selection()->get_selected();
	
	if( rit ) {
		Gtk::TreeModel::Row row = *rit;
		Polka::Object *obj = (Polka::Object*)row[m_Cols.m_pObject];
		if( !obj ) return;
		Editor *objEdt = ObjectManager::get().getObjectEditor( obj->id() );
		if( !objEdt ) return;
		objEdt->setObject(obj);
		m_SignalEditObject.emit( objEdt );
	}
}

void Project::onExport()
{
	Gtk::TreeModel::iterator rit = get_selection()->get_selected();
	
	if( rit ) {
		Gtk::TreeModel::Row row = *rit;
		Polka::Object *obj = (Polka::Object*)row[m_Cols.m_pObject];
		if( !obj ) return;
		ExportManager::get().executeFileExport( /**(get_transient_for()),*/ *obj );
	}
}

Polka::Object *Project::editObject( const Glib::ustring& name )
{
	Polka::Object *obj = findObject( name );
	if( obj ) {
		Editor *objEdt = ObjectManager::get().getObjectEditor( obj->id() );
		if( objEdt ) {
			objEdt->setObject(obj);
			m_SignalEditObject.emit( objEdt );
		}
	}	
	return obj;	
}

Polka::Object *Project::editObject( guint32 funid )
{
	Polka::Object *obj = findObject( funid );
	if( obj ) {
		Editor *objEdt = ObjectManager::get().getObjectEditor( obj->id() );
		if( objEdt ) {
			objEdt->setObject(obj);
			m_SignalEditObject.emit( objEdt );
		}
	}	
	return obj;	
}

void Project::onCreateFolder()
{
	Gtk::TreeModel::iterator rit = get_selection()->get_selected();
	
	if( rit ) {
		// create the folder
		Gtk::TreeModel::iterator it = createFolder( rit, createUniqueName(_("New Folder") ) );
		Gtk::TreeModel::Row row = *it;
		// select row and edit name
		Gtk::TreeModel::Path path(it);
		expand_to_path(path);
		set_cursor(path, *get_column(0), *m_pNameCellRenderer, true);

		Glib::ustring name = row[m_Cols.m_Name];
		// create undo information
		Glib::ustring newfolder = _("Create new folder");
		m_History.createUndoPoint( newfolder, row[m_Cols.m_rpIcon] );
		UndoAction& action = m_History.createAction();
		action.setName( newfolder );
		action.setIcon( row[m_Cols.m_rpIcon] );
		Storage& su = action.setUndoData( ACTION_DELETE );
		su.createItem("DELETE_NAME", "S");
		su.setField( 0, name );
		Storage& sr = action.setRedoData( ACTION_CREATEFOLDER );
		sr.createItem("CREATE_FOLDER", "SS");
		sr.setField( 0, Glib::ustring( (*rit)[m_Cols.m_Name] ) );
		sr.setField( 1, name );
	}
}

void Project::onCreateObject()
{
	// get menu
	Gtk::Menu *popup = dynamic_cast<Gtk::Menu*>(
       		m_refUIManager->get_widget("/TreePopup"));
	Gtk::MenuItem *item = popup->get_active();
       	
	// get name
	if( !item ) return;
	std::string name = item->get_name().substr(19);

	// check requirements
	if( !checkObjectRequirements( name ) ) {
		// error
		return;
	}

	// get container
	Gtk::TreeModel::iterator rit = get_selection()->get_selected();
	
	if( rit ) {
		// interactive undo  point
		ObjectManager& om = ObjectManager::get();
		Glib::ustring undotext = _("Create new ") + om.nameFromId( name );
		m_History.createUndoPoint( undotext, om.iconFromId( name) );
		// create object of type "name"
		Gtk::TreeModel::iterator newIt = createObjectWithUndoAction( rit, name );
		// start row name editor
		Gtk::TreeModel::Path path(newIt);
		expand_to_path(path);
		set_cursor(path, *get_column(0), *m_pNameCellRenderer, true);
	}
}

Polka::Object *Project::createNewObject( const std::string& id )
{
	Gtk::TreeModel::iterator location = findBaseLocation(id);
	if( !location ) return 0;
	location = createObjectWithUndoAction( location, id );
	if( !location ) return 0;
	return (*location)[m_Cols.m_pObject];
}

Gtk::TreeModel::iterator Project::createObjectWithUndoAction( Gtk::TreeModel::iterator location, const std::string& id )
{
	ObjectManager& om = ObjectManager::get();
	// create object of type "name"
	Glib::ustring typeName = om.nameFromId( id );
	Gtk::TreeModel::iterator newIt = createObject( location, createUniqueName( typeName ), id );
	Gtk::TreeModel::Row row = *newIt;
	// create undo information
	Glib::ustring newname = _("Create new ") + typeName;
	UndoAction& action = m_History.createAction();
	action.setName( newname );
	action.setIcon( row[m_Cols.m_rpIcon] );
	// set undo delete
	Storage& su = action.setUndoData( ACTION_DELETE );
	su.createItem("DELETE_NAME", "S");
	su.setField( 0, Glib::ustring( row[m_Cols.m_Name] ) );
	// set redo create empty
	Storage& sr = action.setRedoData( ACTION_CREATE );
	sr.createItem("CREATE_EMPTY", "SSSI");
	sr.setField( 0, id ); // object type
	sr.setField( 1, Glib::ustring( (*location)[m_Cols.m_Name] ) ); // location
	sr.setField( 2, Glib::ustring( row[m_Cols.m_Name] ) ); // create name
	Polka::Object *obj = row[m_Cols.m_pObject];
	sr.setField( 3, int(obj->funid()) ); // create funid
	
	return newIt;
}

Gtk::TreeModel::Row Project::createLocation( const std::vector<Glib::ustring>& path, const std::string& baseType )
{
	// start with children of base item (= project name)
	const Gtk::TreeNodeChildren& items = m_rpTreeModel->children()[0].children();

	// find container of correct type (must exist)
	auto it = items.begin();
	while( it != items.end() ) {
		const std::string& type = (*it)[m_Cols.m_BaseLocation];
		if( type == baseType ) break;
		++it;
	}
	// no fallback
	assert( it != items.end() );
	Gtk::TreeModel::Row row = *it;
	
	// find/create
	for( unsigned int i = 0; i < path.size(); i++ ) {
		bool exist = false;
		// check existing map
		for(auto it = row.children().begin(); it != row.children().end(); it++) {
			if( (*it)[m_Cols.m_Name] == path[i] ) {
				row = *it;
				exist = true;
				break;
			}
		}
		if( !exist ) {
			// recreate path remainder
			while( i < path.size() ) {
				Gtk::TreeModel::iterator newIt = m_rpTreeModel->append(row.children());
				row = *newIt;
				row[m_Cols.m_Name] = path[i];
				row[m_Cols.m_pObject] = 0;
				row[m_Cols.m_BaseLocation] = baseType;
				row[m_Cols.m_rpIcon] = ObjectManager::get().subLocationIcon( baseType );
				i++;
			}
			break;
		}
	}
	
	return row;
}

void Project::startRename()
{
	Gtk::TreeModel::iterator rit = get_selection()->get_selected();
	
	if( rit ) {
		Gtk::TreeModel::Path path(rit);
		set_cursor(path, *get_column(0), *m_pNameCellRenderer, true );
	}
}

void Project::deleteObject()
{
	Gtk::TreeModel::iterator rit = get_selection()->get_selected();
	if( rit ) {
		Gtk::TreeModel::Row row = *rit;
		// create undo information
		Glib::ustring title = _("Deleted '") + row[m_Cols.m_Name] + "'";
		m_History.createUndoPoint( title, row[m_Cols.m_rpIcon] );
		UndoAction& action = m_History.createAction();
		action.setName( title );
		action.setIcon( row[m_Cols.m_rpIcon] );
		// create location string
		Glib::ustring folder( (*row.parent())[m_Cols.m_Name] );
		Glib::ustring name( row[m_Cols.m_Name] );
		Storage& s = action.setRedoData( ACTION_DELETE );
		s.createItem("DELETE_NAME", "S");
		s.setField( 0, name );
		
		if( row[m_Cols.m_pObject] == 0 ) {
			// deleting folder, create action wit hname
			Storage& s = action.setUndoData( ACTION_CREATEFOLDER );
			s.createItem("CREATE_FOLDER", "SS");
			s.setField( 0, folder );
			s.setField( 1, name );
		} else {
			Storage& s = action.setUndoData( ACTION_OBJECTS );
			Polka::Object *obj = row[m_Cols.m_pObject];
			Storage& subS = s.createObject( obj->id() );
			subS.createItem("LOCATION", "S");
			subS.setField( 0, folder );
			obj->save( subS );
		}
		// delete
		deleteLocation( rit );
	}
}

Object* Project::getObject( const Gtk::TreeModel::Path& path ) const
{
	Gtk::TreeModel::iterator iter = m_rpTreeModel->get_iter(path);
	
	if( !iter ) return 0;

	Gtk::TreeModel::Row row = *iter;
	if( row[m_Cols.m_pObject] ) 
		return row[m_Cols.m_pObject];
	else
		return 0;
}

Object* Project::getObject()
{
	Gtk::TreeModel::iterator rit = get_selection()->get_selected();
	
	if( !rit ) return 0;

	Gtk::TreeModel::Row row = *rit;
	if( row[m_Cols.m_pObject] ) 
		return row[m_Cols.m_pObject];
	else
		return 0;
}

Glib::ustring Project::createUniqueName( const Glib::ustring& prefix )
{
	// get list of existing names
	std::vector<Glib::ustring> names;
	getNames(names, m_rpTreeModel->children());
	int id = 0;
	while(true) {
		// create name
		Glib::ustring name = prefix;
		if(id) name += Glib::ustring::compose(" %1", id);
		// check name
		if( std::find(names.begin(), names.end(), name) == names.end() )
			return name;
		id++;
	}
}

void Project::getNames( std::vector<Glib::ustring>& list, const Gtk::TreeNodeChildren& items )
{
	for(Gtk::TreeNodeChildren::iterator it = items.begin(); it != items.end(); it++) {
		list.push_back( (*it)[m_Cols.m_Name] );
		getNames( list, (*it).children() );
	}
}

Gtk::TreeNodeChildren::iterator Project::findLocation( const Glib::ustring& name, const Gtk::TreeNodeChildren& items )
{
	for(Gtk::TreeNodeChildren::iterator it = items.begin(); it != items.end(); it++) {
		if( name == Glib::ustring((*it)[m_Cols.m_Name]) ) return it;
		Gtk::TreeNodeChildren::iterator res = findLocation( name, (*it).children() );
		if( res != (*it).children().end() ) return res;
	}
	return items.end();
}

Gtk::TreeNodeChildren::iterator Project::findBaseLocation( const std::string& type )
{
	const std::string& baseid = ObjectManager::get().objectLocationId(type);
	
	// start with children of base item (= project name)
	const Gtk::TreeNodeChildren& items = m_rpTreeModel->children()[0].children();

	if( baseid.empty() ) return items.end();

	// find container of correct type (must exist)
	for(Gtk::TreeNodeChildren::iterator it = items.begin(); it != items.end(); it++) {
		const std::string& id = (*it)[m_Cols.m_BaseLocation];
		if( id == baseid ) {
			return it;
		}
	}

	return items.end();
}

Polka::Object *Project::findObjectOfTypes( const std::string& ids ) const
{
	std::vector<std::string> reqs = split( ids, ',' );
	for( std::vector<std::string>::iterator it = reqs.begin(); it != reqs.end(); ++it ) {
		Polka::Object *obj = findObjectOfType( *it );
		if( obj ) return obj;
	}
	return 0;
}

Polka::Object *Project::findObjectOfType( const std::string& id ) const
{
	// full object category?
	int sz = id.size();
	bool cat = id.at(sz-1) == '/';
	// check for an existing object
	std::list<Polka::Object*>::const_iterator it = m_Objects.begin();
	while( it != m_Objects.end() ) {
		if( cat ) {
			if( (*it)->id().compare(0, sz, id) == 0 ) return *it;
		} else {
			if( (*it)->id() == id ) return *it;
		}
		it++;
	}
	return 0;
}

Polka::Object *Project::findObject( const Glib::ustring& name ) const
{
	std::list<Polka::Object*>::const_iterator it = m_Objects.begin();
	while( it != m_Objects.end() ) {
		if( (*it)->name() == name ) return *it;
		it++;
	}
	return 0;
}

Polka::Object *Project::findObject( guint32 funid ) const
{
	std::list<Polka::Object*>::const_iterator it = m_Objects.begin();
	while( it != m_Objects.end() ) {
		if( (*it)->funid() == funid ) return *it;
		it++;
	}
	return 0;
}

void Project::findAllObjectsOfType( const std::string& id,
                                          std::vector<Polka::Object*>& vec ) const
{
	// full object category?
	int sz = id.size();
	bool cat = id.at(sz-1) == '/';
	//std::list<Polka::Object*>::const_iterator it = m_Objects.begin();
	auto it = m_Objects.begin();
	while( it != m_Objects.end() ) {
		if( cat ) {
			if( (*it)->id().compare(0, sz, id) == 0 ) vec.push_back(*it);
		} else {
			if( (*it)->id() == id ) vec.push_back(*it);
		}
		it++;
	}
}

bool Project::checkObjectRequirements( const std::string& id )
{
	return ObjectManager::get().canCreateObject( id, *this );
}

void Project::renameLocation( const Glib::ustring& from, const Glib::ustring& to )
{
	Gtk::TreeModel::iterator it = findLocation( from, m_rpTreeModel->children() );
	assert( it ); // must exist if data is uncorrupted
	// rename tree ....
	Gtk::TreeModel::Row row = *it;
	row[m_Cols.m_Name] = to;
	// ... and object if object row
	Polka::Object *obj = row[m_Cols.m_pObject];
	if(obj) {
		obj->setName( to );
		m_SignalTreeUpdate.emit();
	}

}

void Project::createFolder( const Glib::ustring& location, const Glib::ustring& name )
{
	Gtk::TreeModel::iterator it = findLocation( location, m_rpTreeModel->children() );
	assert( it ); // must exist if data is uncorrupted
	Gtk::TreeModel::iterator newIt = createFolder( it, name );

	// select row 
	Gtk::TreeModel::Path path(newIt);
	expand_to_path(path);
	set_cursor(path, *get_column(0), *m_pNameCellRenderer);
}

Gtk::TreeModel::iterator Project::createFolder( Gtk::TreeModel::iterator location, const Glib::ustring& name )
{
	Gtk::TreeModel::Row row = *location;
	Gtk::TreeModel::iterator newIt = m_rpTreeModel->append(row.children());
	Gtk::TreeModel::Row newRow = *newIt;
	newRow[m_Cols.m_Name] = name;
	newRow[m_Cols.m_pObject] = 0;
	newRow[m_Cols.m_BaseLocation] = Glib::ustring(row[m_Cols.m_BaseLocation]);
	newRow[m_Cols.m_rpIcon] = ObjectManager::get().subLocationIcon( newRow[m_Cols.m_BaseLocation] );
	// signal update to tree
	m_SignalTreeUpdate.emit();
	
	return newIt;
}

Gtk::TreeModel::iterator Project::createNewObject( const Glib::ustring& location, const Glib::ustring& name, const std::string& type, guint32 funid )
{
	Gtk::TreeModel::iterator it = findLocation( location, m_rpTreeModel->children() );
	assert( it ); // must exist if data is uncorrupted
	m_ForceFUNID = funid;
	Gtk::TreeModel::iterator newIt = createObject( it, name, type );
	m_ForceFUNID = 0;

	// select row 
	Gtk::TreeModel::Path path(newIt);
	expand_to_path(path);
	set_cursor(path, *get_column(0), *m_pNameCellRenderer);
	
	return newIt;
}

Gtk::TreeModel::iterator Project::createObject( const Glib::ustring& location, const Glib::ustring& name, const std::string& type, Storage& s )
{
	Gtk::TreeModel::iterator it = findLocation( location, m_rpTreeModel->children() );
	assert( it ); // must exist if data is uncorrupted
	Gtk::TreeModel::iterator newIt = createObject( it, name, type );

	// load data
	Polka::Object *obj = (*newIt)[m_Cols.m_pObject];
	obj->setInitMode();
	obj->load( s );
	obj->setInitMode(false);

	// select row 
	Gtk::TreeModel::Path path(newIt);
	expand_to_path(path);
	set_cursor(path, *get_column(0), *m_pNameCellRenderer);
	
	return newIt;
}

Gtk::TreeModel::iterator Project::createObject( Gtk::TreeModel::iterator location, const Glib::ustring& name, const std::string& type )
{
	ObjectManager& om = ObjectManager::get();
	// create object
	Polka::Object *newObj = om.createObject( *this, type );
	m_Objects.push_back( newObj );
	// object must exist
	assert( newObj );
	
	Gtk::TreeModel::Row row = *location;
	// create a row
	Gtk::TreeModel::iterator newIt = m_rpTreeModel->append(row.children());
	Gtk::TreeModel::Row newRow = *newIt;
	newRow[m_Cols.m_Name] = createUniqueName(name);
	newRow[m_Cols.m_pObject] = newObj;
	newRow[m_Cols.m_BaseLocation] = Glib::ustring();
	newRow[m_Cols.m_rpIcon] = om.iconFromId( type );
	newObj->setName( newRow[m_Cols.m_Name] );
	// signal update to tree
	m_SignalTreeUpdate.emit();
	
	newObj->setInitMode(false);
	
	return newIt;
}

guint32 Project::getNewFunid() const
{
	// force a particular id?
	if( m_ForceFUNID ) return m_ForceFUNID;
	// generate a random id
	guint32 uid;
	while(true) {
		uid = g_random_int();
		if( uid == 0 ) continue; // zero is reserved for project
		std::list<Polka::Object*>::const_iterator it = m_Objects.begin();
		while( it != m_Objects.end() ) { 
			if( (*it)->funid() == uid ) break;
			it++;
		}
		if( it == m_Objects.end() ) break;
	}
	return uid;
}

void Project::deleteLocation( const Glib::ustring& name )
{
	Gtk::TreeModel::iterator it = findLocation( name, m_rpTreeModel->children() );
	assert( it ); // must exist if data is uncorrupted
	deleteLocation( it );
}

void Project::deleteLocation( Gtk::TreeModel::iterator location )
{
	Gtk::TreeModel::Row row = *location;
	// remove object first
	Polka::Object *obj = row[m_Cols.m_pObject];
	if( obj ) {
		std::list<Polka::Object*>::iterator it = std::find( m_Objects.begin(), m_Objects.end(), obj );
		assert( it != m_Objects.end() );
		m_Objects.erase(it);
		delete obj;
	}
	// delete tree row
	m_rpTreeModel->erase(location);
}

void Project::projectUndo( const std::string& id, Storage& s )
{
	projectUndoAction( id, s );
}

void Project::projectRedo( const std::string& id, Storage& s )
{
	projectUndoAction( id, s );
}

void Project::projectUndoAction( const std::string& action, Storage& s )
{
	if( action == ACTION_RENAME ) {

		// get names
		Glib::ustring fromName, toName;
		if( s.findItem("NAMES") ) {
			fromName = s.stringField(0);
		} else if( s.findItem("NAME_ID") ) {
			Polka::Object *obj = findObject( s.integerField(0) );
			if(!obj) return;
			fromName = obj->name();
		} else {
			return;
		}
		toName = s.stringField(1);
		// rename the object
		renameLocation( fromName, toName );

	} else if( action == ACTION_COMMENTS ) {

		// get names
		if( s.findItem("COMMENTS") ) {
			// find the object
			Polka::Object *obj = findObject( s.integerField(0) );
			if( obj ) {
				Glib::ustring comments( s.stringField(1) );
				obj->setComments( comments );
			}
		}

	} else if( action == ACTION_CREATEFOLDER ) {

		// get location and name
		if( s.findItem("CREATE_FOLDER") ) {
			Glib::ustring location( s.stringField(0) );
			Glib::ustring name( s.stringField(1) );
			// let project create named folder
			createFolder( location, name );
		}

	} else if( action == ACTION_DELETE ) {

		if( s.findItem("DELETE_NAME") ) {
			Glib::ustring name( s.stringField(0) );
			deleteLocation( name );
		}

	} else if( action  == ACTION_CREATE ) {

		if( s.findItem("CREATE_EMPTY") ) {
			std::string objtype( s.stringField(0) );
			std::string location( s.stringField(1) );
			std::string name( s.stringField(2) );
			guint32 funid = s.integerField(3);
			// let project create named folder
			createNewObject( location, name, objtype, funid );
		}
		
	} else if( action  == ACTION_OBJECTS ) {

		// read objects
		bool readObjs = s.findObject();
		while( readObjs ) {
			const std::string& objtype = s.objectType();
			Storage& objS = s.object();
			// next object (do it now, so continue will work)
			readObjs = s.findNextObject();		

			// all objects have a name and location
			if( !objS.findItem("OBJECT_NAME") ) continue;
			Glib::ustring name = objS.stringField(0);

			// object container
			if( !objS.findItem("LOCATION") ) continue;
			Glib::ustring location = objS.stringField(0);

			createObject( location, name, objtype, objS );
		}
		
	} else if( action == ACTION_MULTIPLE ) {

		// read objects
		bool readObjs = s.findObject();
		while( readObjs ) {
			const std::string& objtype = s.objectType();
			Storage& objS = s.object();
			
			projectUndoAction( objtype, objS );

			// next object
			readObjs = s.findNextObject();		
		}
		
	}
}



} // namespace Polka
