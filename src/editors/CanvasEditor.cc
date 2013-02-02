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

#include "CanvasEditor.h"
#include "Canvas.h"
#include "Palette.h"
#include "AccelManager.h"
#include "Project.h"
#include "Functions.h"
#include <gtkmm/alignment.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/stock.h>
#include <glibmm/i18n.h>
#include <iostream>

namespace Polka {

const std::string ID = "CANVASEDIT";

#define AccCls BitmapCanvasEditor

const AccelManager::DefinitionMap ACCELS = {
// GENERIC
// Pan viewport
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_PAN,              "pan",                { _("Generic"), _("Pan") },                         _("Move the canvas around in the viewport."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {},
	  // Link                          Button  Key           Modifiers
	     "",                           2,      0,            MOD_NONE },
// Activate primary function           
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_ACTIVATE_PRI,     "activate_pri",       { _("Generic"), _("Activate primary") },            _("Activate the primary function of the selected drawing tool (unless overriden by a specific tool)."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {},
	  // Link                          Button  Key           Modifiers
	     "",                           1,      0,            MOD_NONE },
// Activate secondary function         
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_ACTIVATE_SEC,     "activate_sec",       { _("Generic"), _("Activate secondary") },          _("Activate the secondary function of the selected drawing tool (unless overriden by a specific tool)."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {},
	  // Link                          Button  Key           Modifiers
	     "",                           1,      0,            MOD_CTRL },
// Activate secondary function         
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_MOD_ACT_FLIP,     "activate_flip",       { _("Generic"), _("Flip between primary and secondary") }, _("Generic function to flip between the primary and secondary function of a tool (if supported)."),
	  // Type                          Allowed links
	     DEF_MODIFIER,                 {},
	  // Link                          Button  Key           Modifiers
	     "",                           0,      0,            MOD_CTRL },
// TOOL SELECT	                       
// Start selection                     
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_SELECT,           "select_rect",         { _("Tools"), _("Select"), _("Start selection") }, _("Start dragging a selection in the canvas. This definition is also used for modifying the selection."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {"activate_pri"},
	  // Link                          Button  Key           Modifiers
	     "activate_pri",               1,      0,            MOD_NONE },
// Start tile selection                
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_SELECT_TILE,      "select_tile",        { _("Tools"), _("Select"), _("Start tile selection") }, _("Start dragging a selection of tiles. This definition is also used for modifying the selection."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {"activate_sec"},
	  // Link                          Button  Key           Modifiers
	     "activate_sec",               1,      0,            MOD_CTRL },
// Select/tile select modifier
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_MOD_SELECT_TILE,  "select_tile_flip",  { _("Tools"), _("Select"), _("Tile select modifier") }, _("Modifier for fliping between normal select and tile select."),
	  // Type                          Allowed links
	     DEF_MODIFIER,                 {"activate_flip"},
	  // Link                          Button  Key           Modifiers
	     "activate_flip",              0,      0,            MOD_CTRL },
// Square modifier
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_MOD_SELECT_SQUARE,"select_square", { _("Tools"), _("Select"), _("Square selection modifier") }, _("Force the dragged selection to be square."),
	  // Type                          Allowed links
	     DEF_MODIFIER,                 {},
	  // Link                          Button  Key           Modifiers
	     "",                           0,      0,            MOD_SHIFT },
// Change selection to floating
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_SELECT_FLOAT,     "select_float",       { _("Tools"), _("Select"), _("Float selection") },  _("This option changes the selection under the pointer to floating."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {},
	  // Link                          Button  Key           Modifiers
	     "",                           1+DBL_CLICK,  0,      MOD_NONE },
// TOOL EYEDROPPER
// Pick FG
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_COLORPICK_FG,     "colorpick_fg",       { _("Tools"), _("Color picker"), _("Pick foreground") }, _("Use the eyedropper tool to pick the foreground color from the canvas."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {"activate_pri"},
	  // Link                          Button  Key           Modifiers
	     "",                           1,      0,            MOD_NONE },
// Pick BG
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_COLORPICK_BG,     "colorpick_bg",       { _("Tools"), _("Color picker"), _("Pick background") }, _("Use the eyedropper tool to pick the background color from the canvas."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {"activate_sec"},
	  // Link                          Button  Key           Modifiers
	     "",                           3,      0,            MOD_NONE },
// Quick pick FG
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_QUICKPICK_FG,     "quickpick_fg",       { _("Tools"), _("Color picker"), _("Quick pick foreground") }, _("This combination picks the foreground color while not using the eyedropper tool. This function only works of the combination is not assigned to another function in the active tool."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {},
	  // Link                          Button  Key           Modifiers
	     "",                           3,      0,            MOD_NONE },
// Quick pick BG
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_QUICKPICK_BG,     "quickpick_bg",       { _("Tools"), _("Color picker"), _("Quick pick background") }, _("This combination picks the background color while not using the eyedropper tool. This function only works of the combination is not assigned to another function in the active tool."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {},
	  // Link                          Button  Key           Modifiers
	     "",                           3,      0,            MOD_CTRL },
// PEN/BRUSH/COLOR TOOL
// Draw FG
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_DRAW_FG,          "draw_fg",            { _("Tools"), _("Draw"), _("Foreground color") },   _("Draw a pixel or brush using the foreground color."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {"activate_pri"},
	  // Link                          Button  Key           Modifiers
	     "activate_pri",               1,      0,            MOD_NONE },
// Draw BG
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_DRAW_BG,          "draw_bg",            { _("Tools"), _("Draw"), _("Background color") },   _("Draw a pixel or brush using the background color."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {"activate_sec"},
	  // Link                          Button  Key           Modifiers
	     "activate_sec",               1,      0,            MOD_CTRL },
// Flip foreground/background color
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_MOD_DRAW_COL, "draw_flip_fgbg",    { _("Tools"), _("Draw"), _("Flip color") },          _("Switch between foreground and background color."),
	  // Type                          Allowed links
	     DEF_MODIFIER,                 {},
	  // Link                          Button  Key           Modifiers
	     "",                           0,      0,            MOD_CTRL },
// LINE/RECT TOOL
// Line FG
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_LINE_FG,          "line_fg",            { _("Tools"), _("Line"), _("Foreground color") },   _("Draw a line using the foreground color."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {"activate_pri", "draw_fg"},
	  // Link                          Button  Key           Modifiers
	     "draw_fg",                    1,      0,            MOD_NONE },
// Line BG
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_LINE_BG,          "line_bg",            { _("Tools"), _("Line"), _("Background color") },   _("Draw a line using the background color."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {"activate_sec", "draw_bg"},
	  // Link                          Button  Key           Modifiers
	     "draw_bg",                    1,      0,            MOD_CTRL },
// Only draw 45 degree angles
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_MOD_LINE_ANGLE,   "line_angle",       { _("Tools"), _("Line"), _("Restrict angles") },    _("Restrict lines angles to fixed ratios."),
	  // Type                          Allowed links
	     DEF_MODIFIER,                 {},
	  // Link                          Button  Key           Modifiers
	     "",                           0,      0,            MOD_SHIFT },
// FILL TOOL
// Fill FG
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_FILL_FG,          "fill_fg",            { _("Tools"), _("Fill"), _("Foreground color") },   _("Fill using the foreground color."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {"activate_pri"},
	  // Link                          Button  Key           Modifiers
	     "activate_pri",               1,      0,            MOD_NONE },
// Fill BG
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_FILL_BG,          "fill_bg",            { _("Tools"), _("Fill"), _("Background color") },   _("Fill using the background color."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {"activate_sec"},
	  // Link                          Button  Key           Modifiers
	     "activate_sec",               1,      0,            MOD_CTRL },
// FLIP TOOL
// Flip Horizontal
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_FLIP_HOR,         "flip_hor",           { _("Tools"), _("Flip"), _("Horizontal") },         _("Flip the canvas or selection horizontally."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {"activate_pri"},
	  // Link                          Button  Key           Modifiers
	     "activate_pri",               1,      0,            MOD_NONE },
// Flip Vertical
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_FLIP_VER,         "flip_ver",           { _("Tools"), _("Flip"), _("Vertical") },           _("Flip the canvas or selection vertically."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {"activate_sec"},
	  // Link                          Button  Key           Modifiers
	     "activate_sec",               1,      0,            MOD_CTRL },
// ROTATE TOOL
// Rotate clockwise
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_ROTATE_CW,        "rotate_cw",          { _("Tools"), _("Rotate"), _("Clockwise") },        _("Rotate the selection 90 degrees clockwise."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {"activate_pri"},
	  // Link                          Button  Key           Modifiers
	     "activate_pri",               1,      0,            MOD_NONE },
// Rotate counter clockwise
	{ // Incremental id                Text id               Pref Gui location                                   Description
	     AccCls::ACC_ROTATE_CCW,       "rotate_ccw",         { _("Tools"), _("Rotate"), _("Counter clockwise") }, _("Rotate the selection 90 degrees counter clockwise."),
	  // Type                          Allowed links
	     DEF_ACTION,                   {"activate_sec"},
	  // Link                          Button  Key           Modifiers
	     "activate_sec",               1,      0,            MOD_CTRL }
};

CanvasEditorFactory::CanvasEditorFactory()
	: ObjectManager::EditorFactory( ID, _("Canvas editor") )
{
	// register accels
	AccelManager::get().addAccelMap( ID, ACCELS );
}

Editor *CanvasEditorFactory::create() const
{
	return new CanvasEditor; 
}

CanvasEditor::CanvasEditor()
	: Editor(ID), m_pCanvas(0), m_CanvasView(ID)
{
	
	// main view
	Gtk::Frame *f = manage( new Gtk::Frame );
	f->set_shadow_type( Gtk::SHADOW_IN );
	f->add( m_CanvasView );
	attach( *f, 0, 0, 1, 1 );

	// lower widgets
	Gtk::HBox *lbox = manage( new Gtk::HBox );
	attach( *lbox, 0, 1, 1, 1 );
	
	lbox->set_border_width(2);
	lbox->pack_start( m_ColorChooser, Gtk::PACK_SHRINK );
	lbox->pack_end( m_CanvasView.gridSelector(), Gtk::PACK_SHRINK );
	
	// right column
	attach( m_ToolWindow, 1, 0, 1, 2 );
	
	m_CanvasView.createTools(m_ToolWindow);

	// connect component signals
	m_ColorChooser.signalFGChanged().connect( sigc::mem_fun( m_CanvasView, &BitmapCanvasEditor::setFGColor ) );
	m_ColorChooser.signalBGChanged().connect( sigc::mem_fun( m_CanvasView, &BitmapCanvasEditor::setBGColor ) );
	m_CanvasView.signalChangeFGColor().connect( sigc::mem_fun( m_ColorChooser, &ColorChooser::setFGColor ) );
	m_CanvasView.signalChangeBGColor().connect( sigc::mem_fun( m_ColorChooser, &ColorChooser::setBGColor ) );
	m_CanvasView.signalChangeTool().connect( sigc::mem_fun( m_ToolWindow, &ToolButtonWindow::activateTool ) );
}


CanvasEditor::~CanvasEditor()
{
}

void CanvasEditor::on_hide()
{
	m_CanvasView.hide();
}

void CanvasEditor::assignObject( Polka::Object *obj )
{
	// set palette
	m_pCanvas = dynamic_cast<Canvas*>(obj);
	// set interface
	if( m_pCanvas ) {
		//m_Updating = true;
		m_CanvasView.setCanvas( m_pCanvas );
		m_ColorChooser.setPalette( &m_pCanvas->palette() );
		set_sensitive();
		// set valid drag destination targets 
		std::vector<Gtk::TargetEntry> dragTargets;
		std::vector<std::string> types = ObjectManager::get().expandTypeIds( obj->dependencyType(DEP_PAL) );
		for( unsigned int i = 0; i < types.size(); i++ )
			dragTargets.push_back( Gtk::TargetEntry( MIME_OBJNAME + types[i]) );
		drag_dest_set( dragTargets, Gtk::DEST_DEFAULT_ALL, Gdk::ACTION_LINK );
		
		queue_draw();
		//m_Updating = false;
	} else {
		reset();
	}
}

const Glib::ustring& CanvasEditor::menuString()
{
	static Glib::ustring editor_menu =
		"<ui>"
		"</ui>";
	return editor_menu;
}

void CanvasEditor::reset()
{
	m_pCanvas = 0;
	m_CanvasView.setCanvas(0);
	m_ColorChooser.setPalette(0);
	set_sensitive(false);
	drag_dest_unset();
	queue_draw();
}


void CanvasEditor::objectUpdated( bool full )
{
	if( full ) {
		// full update includes dependecy changes
		m_ColorChooser.setPalette( &m_pCanvas->palette() );
		// redraw everything
		queue_draw();
	} else {
		// redraw palette
		m_CanvasView.canvasChanged( m_pCanvas->lastUpdate() );
	}
}

void CanvasEditor::on_drag_data_received( const Glib::RefPtr<Gdk::DragContext>& dc, int x, int y, const Gtk::SelectionData& data, guint info, guint time)
{
	if( m_pCanvas ) {
		Glib::ustring name( (const char*)data.get_data(), data.get_length() );
		std::cout << "SetPal: " << name << std::endl;
		Palette *pal = dynamic_cast<Palette*>( m_pCanvas->project().findObject(name) );
		if(pal) m_pCanvas->setPalette(*pal);
	}
}

} // namespace Polka 
