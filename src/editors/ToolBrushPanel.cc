#include "ToolBrushPanel.h"
#include <glibmm/i18n.h>

namespace Polka {

ToolBrushPanel::ToolBrushPanel()
{
	pack_start( m_BrushFrame );
	m_BrushFrame.add(m_BrushSelector);
	m_BrushFrame.set_shadow_type(Gtk::SHADOW_IN);
	m_BrushFrame.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);
	m_BrushFrame.set_min_content_height(100);
	
	show_all_children();
}

ToolBrushPanel::~ToolBrushPanel()
{
}

void ToolBrushPanel::setBrushVector( const std::vector<Brush*>& brushvec )
{
	m_BrushSelector.setBrushVector(brushvec);
}

BrushSelector::SignalBrushSelected ToolBrushPanel::signalBrushSelected()
{
	return m_BrushSelector.signalBrushSelected();
}

void ToolBrushPanel::selectBrush( int id )
{
	m_BrushSelector.selectBrush(id);
}

void ToolBrushPanel::regenerate()
{
	m_BrushSelector.regenerate();
}

} // namespace Polka 
