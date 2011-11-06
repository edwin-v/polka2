#include "ToolGridPanel.h"

namespace Polka {

ToolGridPanel::ToolGridPanel()
	: m_GridStyleNone("N"), m_GridStyleLines("L"), m_GridStyleDots("D")
{
	
	m_GridStyleBox.pack_start( m_GridStyleNone );
	m_GridStyleBox.pack_start( m_GridStyleLines );
	m_GridStyleBox.pack_start( m_GridStyleDots );

	m_GridStyleNone.signal_toggled().connect( sigc::bind<int>( sigc::mem_fun(*this, &ToolGridPanel::gridHandler), 0 ) );
	m_GridStyleLines.signal_toggled().connect( sigc::bind<int>( sigc::mem_fun(*this, &ToolGridPanel::gridHandler), 1 ) );
	m_GridStyleDots.signal_toggled().connect( sigc::bind<int>( sigc::mem_fun(*this, &ToolGridPanel::gridHandler), 2 ) );
	
	pack_start( m_GridStyleBox );

	show_all_children();
}

ToolGridPanel::~ToolGridPanel()
{
}

ToolGridPanel::SignalGridTypeChanged ToolGridPanel::signalGridTypeChanged()
{
	return m_SignalGridTypeChanged;
}


void ToolGridPanel::gridHandler( int type )
{
	if( type == 0 ) {
		m_GridStyleLines.set_active(false);
		m_GridStyleDots.set_active(false);
	} if( type == 1 ) {
		m_GridStyleNone.set_active(false);
		m_GridStyleDots.set_active(false);
	} if( type == 2 ) {
		m_GridStyleNone.set_active(false);
		m_GridStyleLines.set_active(false);
	}
	m_SignalGridTypeChanged.emit(type);
}

} // namespace Polka 
