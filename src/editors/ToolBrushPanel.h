#ifndef _POLKA_TOOLBRUSHPANEL_H_
#define _POLKA_TOOLBRUSHPANEL_H_

#include <gtkmm/box.h>
#include <gtkmm/scrolledwindow.h>
#include "BrushSelector.h"

namespace Polka {

class ToolBrushPanel: public Gtk::VBox
{
public:
	ToolBrushPanel();
	~ToolBrushPanel();

	void setBrushVector( const std::vector<Brush*>& brushvec );
	BrushSelector::SignalBrushSelected signalBrushSelected();
	
	void selectBrush( int id );
	void regenerate();

private:
	Gtk::ScrolledWindow m_BrushFrame;
	BrushSelector m_BrushSelector;
};

} // namespace Polka

#endif // _POLKA_TOOLRECTPANEL_H_
