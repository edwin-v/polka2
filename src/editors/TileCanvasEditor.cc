#include "TileCanvasEditor.h"
#include "TileCanvas.h"
#include "Palette.h"
#include "Project.h"
#include <gtkmm/alignment.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/box.h>
#include <gtkmm/stock.h>
#include <glibmm/i18n.h>
#include <iostream>
#include <gtkmm/handlebox.h>

namespace Polka {


TileCanvasEditorFactory::TileCanvasEditorFactory()
	: ObjectManager::EditorFactory( "TILECANVASEDIT", _("Tile canvas editor") )
{
}

Editor *TileCanvasEditorFactory::create() const
{
	return new TileCanvasEditor; 
}



TileCanvasEditor::TileCanvasEditor()
	: Editor("TILECANVASEDIT"), m_pCanvas(0), m_GridSizeH(0), m_GridSizeV(0)
{
	// Label
	Gtk::Label *l = manage( new Gtk::Label( _("Tile canvas editor"), Gtk::ALIGN_START ));
	pack_start( *l, Gtk::PACK_SHRINK );

	// attach canvas viewer
	Gtk::ScrolledWindow *sw = manage( new Gtk::ScrolledWindow );
	sw->set_policy( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC );
	Gtk::Frame *f = manage( new Gtk::Frame );
	sw->add( m_CanvasView );
	f->set_shadow_type( Gtk::SHADOW_IN );
	f->add( *sw );
	pack_start( *f, Gtk::PACK_EXPAND_WIDGET );
	
	// attach palette selector
	Gtk::HBox *hbox = manage( new Gtk::HBox );
	l = manage( new Gtk::Label( _("Current palette:"), Gtk::ALIGN_START ));
	hbox->pack_start( *l, Gtk::PACK_SHRINK );
	hbox->pack_start( m_PaletteSelector, Gtk::PACK_SHRINK );
	pack_start( *hbox, Gtk::PACK_SHRINK );

	// main edit hbox
	hbox = manage( new Gtk::HBox );
	pack_start( *hbox, Gtk::PACK_SHRINK );
	
	// zoom slider
	auto a = m_EditorZoom.get_adjustment();
	a->set_lower(4.0);
	a->set_upper(32.0);
	a->set_step_increment(1.0);
	a->set_page_increment(4.0);
	a->signal_value_changed().connect( sigc::mem_fun(*this,
			&TileCanvasEditor::changeEditorZoom) );
	m_EditorZoom.set_size_request( -1, 16*8 );

	Gtk::VBox *leftBox = manage( new Gtk::VBox );
	hbox->pack_start( *leftBox, Gtk::PACK_SHRINK );
	leftBox->pack_start( m_EditorZoom, Gtk::PACK_SHRINK );
	
	Gtk::HandleBox *hb = manage( new Gtk::HandleBox );
	Gtk::Table *sizeTable = manage( new Gtk::Table(3, 3, true ) );
	sizeTable->attach( m_ShrinkV, 1, 2, 0, 1 );
	sizeTable->attach( m_ShrinkH, 0, 1, 1, 2 );
	sizeTable->attach( m_GrowH, 2, 3, 1, 2 );
	sizeTable->attach( m_GrowV, 1, 2, 2, 3 );
	//leftBox->pack_end( *sizeTable, Gtk::PACK_SHRINK );
	hb->add( *sizeTable );
	leftBox->pack_end( *hb, Gtk::PACK_SHRINK );
	
	m_ShrinkV.set_use_stock(true);
	m_ShrinkV.set_label("gtk-go-up");
	m_ShrinkH.set_use_stock(true);
	m_ShrinkH.set_label("gtk-go-back");
	m_GrowH.set_use_stock(true);
	m_GrowH.set_label("gtk-go-forward");
	m_GrowV.set_use_stock(true);
	m_GrowV.set_label("gtk-go-down");
	
	hbox->pack_start( m_TileGrid, Gtk::PACK_SHRINK );
	m_ColorSelector.setSize( 2, 8 );
	m_ColorSelector.set_size_request( 20*2+4, 20*8+4 );
	hbox->pack_start( m_ColorSelector, Gtk::PACK_SHRINK );

	//m_ColorSelector.signalSelectionChanged().connect( sigc::mem_fun(m_TileGrid,
	//		&TileEditGrid::setSelectColor) );
	m_PaletteSelector.signal_changed().connect( sigc::mem_fun(*this,
			&TileCanvasEditor::changePalette) );
	m_CanvasView.signalSelectionChanged().connect( sigc::mem_fun(*this,
			&TileCanvasEditor::tileSelectionChanged) );
	m_TileGrid.signalTileChanged().connect( sigc::mem_fun(m_CanvasView,
			&TileImageView::tileUpdated) );
	//m_TileGrid.signalColorSelected().connect( sigc::mem_fun(m_ColorSelector,
	//		&ColorSelector::setSelected) );


	m_ShrinkV.signal_clicked().connect(
				sigc::bind<int, int>( sigc::mem_fun( *this,
						&TileCanvasEditor::resizeGrid), 0, -1 ) );
	m_GrowV.signal_clicked().connect(
				sigc::bind<int, int>( sigc::mem_fun( *this,
						&TileCanvasEditor::resizeGrid), 0, 1 ) );
	m_ShrinkH.signal_clicked().connect(
				sigc::bind<int, int>( sigc::mem_fun( *this,
						&TileCanvasEditor::resizeGrid), -1, 0 ) );
	m_GrowH.signal_clicked().connect(
				sigc::bind<int, int>( sigc::mem_fun( *this,
						&TileCanvasEditor::resizeGrid), 1, 0 ) );


	// set up palette model
	fillPaletteSelector();
	// set default editor zoom
	a->set_value(16);
	resizeGrid(1,1);
	
	reset();
}

TileCanvasEditor::~TileCanvasEditor()
{
}

void TileCanvasEditor::assignObject( Polka::Object *obj )
{
	// set palette
	m_pCanvas = dynamic_cast<TileCanvas*>(obj);
	// set interface
	if( m_pCanvas ) {
		//m_Updating = true;
		m_CanvasView.setImage( m_pCanvas->getImage() );
		m_ColorSelector.setPalette( const_cast<Palette*>(m_pCanvas->getPalette()) );
		m_TileGrid.setPalette( m_pCanvas->getPalette() );
		m_PaletteSelector.set_active_text( m_pCanvas->getPalette()->name() );
		m_CanvasView.setSelection(0, 0);
		queue_draw();
		//m_Updating = false;
	} else {
		reset();
	}
}

void TileCanvasEditor::updateTreeNames()
{
	fillPaletteSelector();
}

const Glib::ustring& TileCanvasEditor::menuString()
{
	static Glib::ustring editor_menu =
		"<ui>"
		"  <menubar name='MenuBar'>"
		"    <placeholder name='EditorMenu'>"
		"      <menu action='Tile Canvas'>"
		"        <menu action='Grid size'>"
		"          <menu action='Width'>"
		"            <menuitem action='TileCanvasIncreaseWidth'/>"
		"            <menuitem action='TileCanvasDecreaseWidth'/>"
		"            <separator/>"
		"            <menuitem action='TileCanvasWidth1'/>"
		"            <menuitem action='TileCanvasWidth2'/>"
		"            <menuitem action='TileCanvasWidth3'/>"
		"            <menuitem action='TileCanvasWidth4'/>"
		"            <menuitem action='TileCanvasWidth5'/>"
		"            <menuitem action='TileCanvasWidth6'/>"
		"      <menuitem action='EditProperties'/>"
		"            <menuitem action='TileCanvasWidth7'/>"
		"            <menuitem action='TileCanvasWidth8'/>"
		"          </menu>"
		"          <menu action='Height'>"
		"            <menuitem action='TileCanvasIncreaseHeight'/>"
		"            <menuitem action='TileCanvasDecreaseHeight'/>"
		"            <separator/>"
		"            <menuitem action='TileCanvasHeight1'/>"
		"            <menuitem action='TileCanvasHeight2'/>"
		"            <menuitem action='TileCanvasHeight3'/>"
		"            <menuitem action='TileCanvasHeight4'/>"
		"            <menuitem action='TileCanvasHeight5'/>"
		"            <menuitem action='TileCanvasHeight6'/>"
		"            <menuitem action='TileCanvasHeight7'/>"
		"            <menuitem action='TileCanvasHeight8'/>"
		"          </menu>"
		"        </menu>"
		"        <menuitem action='EditCut'/>"
		"        <menuitem action='EditCopy'/>"
		"        <menuitem action='EditPaste'/>"
		"        <menuitem action='EditDelete'/>"
		"        <separator/>"
		"        <menuitem action='EditRename'/>"
		"        <menuitem action='EditProperties'/>"
		"      </menu>"
		"    </placeholder>"
		"  </menubar>"
		"</ui>";
	return editor_menu;
}

void TileCanvasEditor::reset()
{
	m_pCanvas = 0;
	m_CanvasView.setImage( Cairo::RefPtr<Cairo::ImageSurface>(0) );
	m_ColorSelector.setPalette(0);
	m_TileGrid.reset();
	queue_draw();
}

void TileCanvasEditor::fillPaletteSelector()
{
	m_PaletteSelector.remove_all();
	if( m_pCanvas ) {
		std::vector<Polka::Object*> objects;
		m_pCanvas->project().findAllObjectsOfType( "PAL1", objects );
		m_pCanvas->project().findAllObjectsOfType( "PAL2", objects );
		std::vector<Polka::Object*>::iterator it = objects.begin();
		while( it != objects.end() ) {
			m_PaletteSelector.append( (*it)->name() );
			if( (*it) == m_pCanvas->getPalette() )
				m_PaletteSelector.set_active_text( (*it)->name() );
			it++;
		}
	}
}

void TileCanvasEditor::changePalette()
{
	Polka::Object *obj = m_pCanvas->project().findObject(
		m_PaletteSelector.get_active_text() );
	if( obj ) {
		Palette *pal = dynamic_cast<Palette*>(obj);
		m_pCanvas->setPalette( *pal );
		m_CanvasView.queue_draw();
		m_ColorSelector.setPalette( pal );
		m_TileGrid.setPalette( pal );
	}
}

void TileCanvasEditor::tileSelectionChanged( int x, int y )
{
	m_TileGrid.setGrid( m_GridSizeH, m_GridSizeV, x, y, m_pCanvas->getImage()->get_data(), 
			m_pCanvas->getImage()->get_stride() );
}

void TileCanvasEditor::changeEditorZoom()
{
	m_TileGrid.setZoomSize( int(m_EditorZoom.get_adjustment()->get_value()) );
}

void TileCanvasEditor::resizeGrid( int h, int v )
{
	// check bounds
	if( h<0 && m_GridSizeH-h < 1 ) h = 0;
	if( h>0 && m_GridSizeH+h > 8 ) h = 0;
	if( v<0 && m_GridSizeV-v < 1 ) v = 0;
	if( v>0 && m_GridSizeV+v > 8 ) v = 0;
	if( h == 0 && v == 0 ) return;
	// set new size
	m_GridSizeH += h;
	m_GridSizeV += v;
	// set select rectangle
	m_CanvasView.setSelector( m_GridSizeH*8, m_GridSizeV*8 );
}

} // namespace Polka 
