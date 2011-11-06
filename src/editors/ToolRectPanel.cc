#include "ToolRectPanel.h"
#include <glibmm/i18n.h>

namespace Polka {

ToolRectPanel::ToolRectPanel()
	: m_FillCheck(_("Fill"))
{
	pack_start( m_FillCheck );

	show_all_children();
}

ToolRectPanel::~ToolRectPanel()
{
}

bool ToolRectPanel::filledRectangle() const
{
	return m_FillCheck.get_active();
}

} // namespace Polka 
