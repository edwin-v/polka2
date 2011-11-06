#include "PaletteEditor.h"
#include "Palette.h"
#include <gtkmm/alignment.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/table.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <glibmm/i18n.h>
#include <iostream>

namespace Polka {

PaletteEditorFactory::PaletteEditorFactory()
	: ObjectManager::EditorFactory( "PALEDIT", _("Palette editor") )
{
}

Editor *PaletteEditorFactory::create() const
{
	return new PaletteEditor; 
}



PaletteEditor::PaletteEditor()
	: Editor("PALEDIT"), m_CopyButton(_("Copy")), m_SwapButton(_("Swap")), m_GradientButton(_("Gradient")), 
	  m_pPalette(0)

{
	// separate window
	setMainEditor(false);

	set_border_width(4);
	
	// color selector on top
	Gtk::Frame *f = manage( new Gtk::Frame );
	f->set_shadow_type( Gtk::SHADOW_IN );
	f->add( m_Selector );
	pack_start( *f, Gtk::PACK_EXPAND_WIDGET );

	// table for controls
	Gtk::Table *table = manage( new Gtk::Table );
	pack_start( *table, Gtk::PACK_SHRINK  );
	
	// attach spinbuttons i
	auto adjR = m_RedSlider.getAdjustment();
	auto adjG = m_GreenSlider.getAdjustment();
	auto adjB = m_BlueSlider.getAdjustment();

	m_RedSpin.set_adjustment( adjR );
	m_GreenSpin.set_adjustment( adjG );
	m_BlueSpin.set_adjustment( adjB );
	
	table->attach( m_RedSpin, 0, 1, 0, 1, Gtk::SHRINK, Gtk::SHRINK );
	table->attach( m_GreenSpin, 0, 1, 1, 2, Gtk::SHRINK, Gtk::SHRINK );
	table->attach( m_BlueSpin, 0, 1, 2, 3, Gtk::SHRINK, Gtk::SHRINK );
	
	// setup and attach the sliders
	m_RedSlider.setLowColor( 0.3, 0.0, 0.0 );
	m_RedSlider.setHighColor( 0.8, 0.0, 0.0 );
	m_GreenSlider.setLowColor( 0.0, 0.3, 0.0 );
	m_GreenSlider.setHighColor( 0.0, 0.8, 0.0 );
	m_BlueSlider.setLowColor( 0.0, 0.0, 0.3 );
	m_BlueSlider.setHighColor( 0.0, 0.0, 0.8 );
	
	m_RedSlider.signalChanged().connect( sigc::mem_fun(*this,
	                                     &PaletteEditor::onChanged) );
	m_RedSlider.signalChanging().connect( sigc::mem_fun(*this,
	                                     &PaletteEditor::onChanging) );
	m_GreenSlider.signalChanged().connect( sigc::mem_fun(*this,
	                                     &PaletteEditor::onChanged) );
	m_GreenSlider.signalChanging().connect( sigc::mem_fun(*this,
	                                     &PaletteEditor::onChanging) );
	m_BlueSlider.signalChanged().connect( sigc::mem_fun(*this,
	                                     &PaletteEditor::onChanged) );
	m_BlueSlider.signalChanging().connect( sigc::mem_fun(*this,
	                                     &PaletteEditor::onChanging) );
	
	m_RedSlider.set_size_request( 8*16, 26 );
	m_GreenSlider.set_size_request( 8*16, 26 );
	m_BlueSlider.set_size_request( 8*16, 26 );

	table->attach( m_RedSlider, 1, 2, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::FILL );
	table->attach( m_GreenSlider, 1, 2, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::FILL );
	table->attach( m_BlueSlider, 1, 2, 2, 3, Gtk::FILL | Gtk::EXPAND, Gtk::FILL );
	
	// color preview
	m_Preview.set_size_request( 64 ,-1 );
	table->attach( m_Preview, 2, 3, 0, 3, Gtk::FILL | Gtk::EXPAND, Gtk::FILL );
					
	// buttons
	table->attach( m_CopyButton, 3, 4, 0, 1, Gtk::SHRINK, Gtk::SHRINK );
	table->attach( m_SwapButton, 3, 4, 1, 2, Gtk::SHRINK, Gtk::SHRINK );
	table->attach( m_GradientButton, 3, 4, 2, 3, Gtk::SHRINK, Gtk::SHRINK );
	
	m_CopyButton.signal_clicked().connect( sigc::mem_fun(*this, &PaletteEditor::copyColor ) );
	m_SwapButton.signal_clicked().connect( sigc::mem_fun(*this, &PaletteEditor::swapColor ) );
	m_GradientButton.signal_clicked().connect( sigc::mem_fun(*this, &PaletteEditor::createGradient ) );

	m_Selector.signalColorClicked().connect( sigc::mem_fun(*this, &PaletteEditor::onClick ) );
	
	table->set_row_spacings( 4 );
	table->set_col_spacings( 4 );
	
	reset();
	
}

PaletteEditor::~PaletteEditor()
{
	assignObject(0);
}

void PaletteEditor::assignObject( Polka::Object *obj )
{
	if( m_pPalette == obj ) return;

	reset();

	if( obj ) {
		// set palette
		m_pPalette = dynamic_cast<Palette*>(obj);
		// set interface
		m_Updating = true;
		m_Selector.setPalette( m_pPalette );
		int hi = (1 << m_pPalette->depth()) - 1;
		m_RedSlider.getAdjustment()->set_upper( hi );
		m_GreenSlider.getAdjustment()->set_upper( hi );
		m_BlueSlider.getAdjustment()->set_upper( hi );
		m_Updating = false;
		
		int c = m_Selector.primaryColor();
		if( c < 0 ) c = 0;
		selectColor(c);
		set_sensitive(true);
	}
}

void PaletteEditor::reset()
{
	m_Updating = true;
	m_Selector.reset();
	m_pPalette = 0;
	m_Preview.setColor( .22, .22, .22 );
	m_RedSlider.getAdjustment()->set_value(0);
	m_GreenSlider.getAdjustment()->set_value(0);
	m_BlueSlider.getAdjustment()->set_value(0);
	m_RedSpin.set_sensitive( false );
	m_GreenSpin.set_sensitive( false );
	m_BlueSpin.set_sensitive( false );
	m_RedSlider.set_sensitive( false );
	m_GreenSlider.set_sensitive( false );
	m_BlueSlider.set_sensitive( false );
	m_Updating = false;
	
	set_sensitive(false);
	queue_draw();
}

void PaletteEditor::onChanged()
{
	if( !m_pPalette ) return;
	if( m_Updating ) return;

	m_pPalette->setColor( m_Selector.primaryColor(), 
	      double(m_RedSlider.value())/m_RedSlider.range(),
	      double(m_GreenSlider.value())/m_GreenSlider.range(),
	      double(m_BlueSlider.value())/m_BlueSlider.range() );
	m_Selector.queue_draw();
}

void PaletteEditor::onChanging()
{
	if( !m_pPalette ) return;
	if( m_Updating ) return;

	m_Preview.setColor(
	      double(m_RedSlider.value())/m_RedSlider.range(),
	      double(m_GreenSlider.value())/m_GreenSlider.range(),
	      double(m_BlueSlider.value())/m_BlueSlider.range() );
}

void PaletteEditor::selectColor( int c )
{
	m_Selector.setSelection( c, -1 );
	onSelect(c);
}

void PaletteEditor::onSelect( int c )
{
	// activate the color silders
	m_RedSpin.set_sensitive( true );
	m_GreenSpin.set_sensitive( true );
	m_BlueSpin.set_sensitive( true );
	m_RedSlider.set_sensitive( true );
	m_GreenSlider.set_sensitive( true );
	m_BlueSlider.set_sensitive( true );
	// set components
	m_Updating = true;
	m_RedSlider.getAdjustment()->set_value( m_pPalette->r(c) * m_RedSlider.range() );
	m_GreenSlider.getAdjustment()->set_value( m_pPalette->g(c) * m_GreenSlider.range() );
	m_BlueSlider.getAdjustment()->set_value( m_pPalette->b(c) * m_BlueSlider.range() );
	m_Updating = false;
	onChanging();
}

void PaletteEditor::onClick( int b )
{
	if( b == 1 ) 
		onSelect( m_Selector.primaryColor() );
}

void PaletteEditor::copyColor()
{
	if( m_pPalette ) m_pPalette->copyColor( m_Selector.secondaryColor(), m_Selector.primaryColor() );
}

void PaletteEditor::swapColor()
{
	if( m_pPalette ) m_pPalette->swapColor( m_Selector.primaryColor(), m_Selector.secondaryColor() );
}

void PaletteEditor::createGradient()
{
	if( m_pPalette ) m_pPalette->createGradient( m_Selector.primaryColor(), m_Selector.secondaryColor() );
}



} // namespace Polka 
