#include "CanvasEditor.h"
#include "Canvas.h"
#include "Palette.h"
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

CanvasEditorFactory::CanvasEditorFactory()
	: ObjectManager::EditorFactory( ID, _("Canvas editor") )
{
}

Editor *CanvasEditorFactory::create() const
{
	return new CanvasEditor; 
}



CanvasEditor::CanvasEditor()
	: Editor(ID), m_pCanvas(0), m_CanvasView(ID)
{
	Gtk::HBox *hbox = manage( new Gtk::HBox );
	pack_start( *hbox, Gtk::PACK_EXPAND_WIDGET );
	// left column
	Gtk::VBox *vbox = manage( new Gtk::VBox );
	hbox->pack_start( *vbox, Gtk::PACK_SHRINK );
	
	// mid column
	vbox = manage( new Gtk::VBox );
	hbox->pack_start( *vbox, Gtk::PACK_EXPAND_WIDGET );
	// main view
	Gtk::Frame *f = manage( new Gtk::Frame );
	f->set_shadow_type( Gtk::SHADOW_IN );
	f->add( m_CanvasView );
	vbox->pack_start( *f, Gtk::PACK_EXPAND_WIDGET );

	// lower widgets
	Gtk::HBox *lbox = manage( new Gtk::HBox );
	vbox->pack_start( *lbox, Gtk::PACK_SHRINK );
	
	lbox->set_border_width(2);
	lbox->pack_start( m_ColorChooser, Gtk::PACK_SHRINK );
	lbox->pack_end( m_CanvasView.gridSelector(), Gtk::PACK_SHRINK );
	
	// right column
	vbox = manage( new Gtk::VBox );
	hbox->pack_start( *vbox, Gtk::PACK_SHRINK );
	vbox->pack_start( m_ToolWindow, Gtk::PACK_SHRINK );
	
	m_CanvasView.createTools(m_ToolWindow);

	// connect component signals
	m_ColorChooser.signalFGChanged().connect( sigc::mem_fun( m_CanvasView, &BitmapCanvasEditor::setFGColor ) );
	m_ColorChooser.signalBGChanged().connect( sigc::mem_fun( m_CanvasView, &BitmapCanvasEditor::setBGColor ) );
	m_CanvasView.signalChangeFGColor().connect( sigc::mem_fun( m_ColorChooser, &ColorChooser::setFGColor ) );
	m_CanvasView.signalChangeBGColor().connect( sigc::mem_fun( m_ColorChooser, &ColorChooser::setBGColor ) );

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
	queue_draw();
}


void CanvasEditor::objectUpdated( bool full )
{
	if( full ) {
		// redraw everything
		queue_draw();
	} else {
		// redraw palette
		m_CanvasView.canvasChanged( m_pCanvas->lastUpdate() );
	}
}

} // namespace Polka 
