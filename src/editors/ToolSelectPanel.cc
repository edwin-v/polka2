#include "ToolSelectPanel.h"
#include "ResourceManager.h"
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <glibmm/i18n.h>

namespace Polka {

ToolSelectPanel::ToolSelectPanel()
{
	ResourceManager& rm = ResourceManager::get();
	
	// cut/copy radiobuttons
	m_Copy.add( *manage( new Gtk::Image(rm.getIcon("canvasedit_tool_selectcopy")) ) );
	m_Copy.set_relief( Gtk::RELIEF_NONE );
	m_Copy.set_mode(false);
	m_Copy.set_tooltip_text( _("Copy mode: image data is copied into the selection.") );
	//m_TBPixelNone.get_style_context()->add_provider( rbStyle, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );
	m_Cut.add( *manage( new Gtk::Image(rm.getIcon("canvasedit_tool_selectcut")) ) );
	m_Cut.set_relief( Gtk::RELIEF_NONE );
	m_Cut.set_mode(false);
	m_Cut.join_group( m_Copy );
	m_Cut.set_tooltip_text( _("Cut mode: image data is copied into the selection and replaced by the background color.") );

	// solid/transparent radiobuttons
	m_Solid.add( *manage( new Gtk::Image(rm.getIcon("canvasedit_tool_selectsolid")) ) );
	m_Solid.set_relief( Gtk::RELIEF_NONE );
	m_Solid.set_mode(false);
	m_Solid.set_tooltip_text( _("The brush is solid graphics.") );
	//m_TBPixelNone.get_style_context()->add_provider( rbStyle, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION );
	m_Transparent.add( *manage( new Gtk::Image(rm.getIcon("canvasedit_tool_selecttransparent")) ) );
	m_Transparent.set_relief( Gtk::RELIEF_NONE );
	m_Transparent.set_mode(false);
	m_Transparent.join_group( m_Solid );
	m_Transparent.set_tooltip_text( _("Brush areas with the background color are made transparent.") );

	m_ToBrush.add( *manage( new Gtk::Image(rm.getIcon("canvasedit_tool_selecttobrush")) ) );
	m_ToBrush.set_tooltip_text( _("Covent the floating selection into a brush.") );
	m_ToBrush.set_sensitive(false);

	pack_start(m_Copy, Gtk::PACK_SHRINK);
	pack_start(m_Cut, Gtk::PACK_SHRINK);
	pack_start( *manage( new Gtk::Label ), Gtk::PACK_SHRINK );
	pack_start(m_Solid, Gtk::PACK_SHRINK);
	pack_start(m_Transparent, Gtk::PACK_SHRINK);
	pack_end( m_ToBrush, Gtk::PACK_SHRINK, 16 );

	show_all_children();
}

ToolSelectPanel::~ToolSelectPanel()
{
}

Glib::SignalProxy0<void> ToolSelectPanel::toBrushClicked()
{
	return m_ToBrush.signal_clicked();
}

Glib::SignalProxy0<void> ToolSelectPanel::floatModeChanged()
{
	return m_Solid.signal_toggled();
}

bool ToolSelectPanel::copyMode() const
{
	return m_Copy.get_active();
}

bool ToolSelectPanel::solidMode() const
{
	return m_Solid.get_active();
}

void ToolSelectPanel::setHasFloating( bool value )
{
	m_ToBrush.set_sensitive(value);
}

} // namespace Polka 
