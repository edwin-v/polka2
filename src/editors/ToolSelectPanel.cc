#include "ToolSelectPanel.h"
#include <glibmm/i18n.h>

namespace Polka {

ToolSelectPanel::ToolSelectPanel()
	: m_ToBrush(_("To Brush"))
{
	pack_start( m_ToBrush );

	show_all_children();
}

ToolSelectPanel::~ToolSelectPanel()
{
}

Glib::SignalProxy0< void > ToolSelectPanel::toBrushClicked()
{
	return m_ToBrush.signal_clicked();
}

} // namespace Polka 
