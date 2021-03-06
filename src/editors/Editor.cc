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

#include "Editor.h"
#include "Object.h"
#include "ObjectManager.h"
#include "Settings.h"
#include <cassert>
#include <iostream>

namespace Polka {

const Gdk::ModifierType ACC_ALLOWED = Gdk::SHIFT_MASK | 
                                      Gdk::CONTROL_MASK | 
                                      Gdk::MOD1_MASK /*ALT*/ |
                                      Gdk::MOD4_MASK /*Left Win*/ |
                                      Gdk::MOD5_MASK /*Right Win*/ ;

Editor::Editor( const std::string& _id )
	: m_Id(_id), m_MainEditor(true), m_Window(_id), m_pObject(0)
{
}

Editor::~Editor()
{
}

void Editor::treeUpdated()
{
	updateTitle();
	updateTreeNames();
}

void Editor::objectUpdated( bool /*full*/ )
{
	queue_draw();
}

void Editor::setObject( Polka::Object *obj )
{
	assignObject(obj);

	// handle core links
	if( m_pObject ) {
		// detach editor
		m_pObject->detachEditor(this);
	}

	if( obj ) {
		// assign editor
		obj->attachEditor(this);
	}

	m_pObject = obj;
	updateTitle();
}

void Editor::updateTreeNames()
{
	// empty default
	// implement in derived classes
}

void Editor::updateDependencies()
{
	// empty default
	// implement in derived classes
}

const Glib::ustring& Editor::menuString()
{
	static Glib::ustring EMPTY_MENU;
	return EMPTY_MENU;
}

bool Editor::isMainEditor() const
{
	return m_MainEditor;
}

void Editor::setMainEditor( bool value )
{
	m_MainEditor = value;
	if( value )
		m_Window.setEditor(0);
	else
		m_Window.setEditor(this);
}

void Editor::activate()
{
	if( !m_MainEditor ) {
		m_Window.show_all_children();
		m_Window.present();
	}
}

void Editor::hide()
{
	if( !m_MainEditor ) {
		m_Window.hide();
	}
	Gtk::Grid::hide();
}

//Editor::SignalTitleChanged Editor::signalTitleChanged()
//{
//	return m_SignalTitleChanged;
//}

void Editor::updateTitle()
{
	m_Window.set_title( title() );
}

Glib::ustring Editor::title() const
{
	Glib::ustring title = ObjectManager::get().editorNameFromId( m_Id );
	// add object name
	if( m_pObject ) {
		title += " - ";
		title += m_pObject->name();
	}
	return title;
}


} // namespace Polka
