#ifndef _POLKA_BITMAPCANVASEDITOR_H_
#define _POLKA_BITMAPCANVASEDITOR_H_

#include "CanvasView.h"
#include "Palette.h"
// tools
#include "ToolSelectPanel.h"
//#include "ToolGridPanel.h"
#include "ToolBrushPanel.h"
#include "ToolRectPanel.h"
#include "Brush.h"
#include "Defs.h"
#include <vector>

namespace Polka {

class ToolButtonWindow;


class BitmapCanvasEditor : public CanvasView
{
public:
	BitmapCanvasEditor( const std::string& _id );
	~BitmapCanvasEditor();

	virtual void setCanvas( Canvas *canvas );

	void createTools( ToolButtonWindow& tw );

	void setFGColor( int col );
	void setBGColor( int col );
	IntSignal signalChangeFGColor();
	IntSignal signalChangeBGColor();

protected:
	//virtual bool on_scroll_event(GdkEventScroll* event);
	virtual bool on_button_press_event(GdkEventButton *event);
	virtual bool on_button_release_event(GdkEventButton *event);
	virtual bool on_motion_notify_event(GdkEventMotion* event);
	
	virtual bool on_key_press_event (GdkEventKey* event);

	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );

	virtual void on_hide();
	virtual void on_show();

	virtual void changeCursor( Glib::RefPtr<Gdk::Cursor> cursor = Glib::RefPtr<Gdk::Cursor>() );
	virtual void restoreCursor();

	OverlayPainter m_Overlay;

private:
	bool m_DragPrimary;

	// processed coordinates
	int m_MouseX, m_MouseY, m_LastMouseX, m_LastMouseY;
	int m_PixX, m_PixY, m_LastPixX, m_LastPixY;
	bool m_MouseInArea;
	int m_TileX, m_TileY;
	bool m_InFullTile;
	// extra tool coordinates
	int m_DragStartX, m_DragStartY, m_DragEndX, m_DragEndY;
	int m_DragOffsetX, m_DragOffsetY;

	bool m_ZoomMode;

	IntSignal m_SignalChangeFGColor, m_SignalChangeBGColor;
	//Glib::RefPtr<Gtk::ActionGroup> m_refActionGroup;
	
	// tools
	int m_CurrentTool;
	int m_ActiveTool;
	bool m_ToolHold;
	OverlayPainter::Shape *m_pToolMarker;
	OverlayBrush m_BrushMarker, m_SelectionMarker;
	

	ToolSelectPanel m_ToolSelectPanel;
	//ToolGridPanel m_ToolGridPanel;
	ToolBrushPanel m_ToolBrushPanel;
	ToolRectPanel m_ToolRectPanel;

	// tool variables
	int m_ToolMode;
	int m_FGColor, m_BGColor;
	Pen m_Pen;
	int m_PenColor;
	bool m_PickFG;
	std::vector<Brush*> m_Brushes;
	Glib::RefPtr<Gdk::Cursor> m_refToolCursor;
	Brush *m_pBrush, *m_pSelectionBrush;


	void screenDraw( int x, int y, bool use_brush = false );
	
	void changeTool( int id );
	void changeColor( int col, int seccol );
	void changeBrush( int id );
	bool isInPixelArea( int x, int y );
	void clipPixelArea( int& x, int& y, int w = -1, int h = -1 );
	void resetActiveTool();	
	void removeToolMarker();
	void updateCoords( int x, int y );
	bool updateTileCoords();
	bool tileCoords( int x, int y, int& tx, int& ty );
	
	// interactive functions
	Brush *createBrushFromSelection();
	
	// tool handlers
	void rectSelectInit();
	bool rectSelectActivate( guint button, guint mods );
	bool rectSelectUpdate( guint mods );
	bool rectSelectRelease( guint button );
	void rectSelectClean();

	void eyeDropperInit();
	bool eyeDropperActivate( guint button, guint mods );
	bool eyeDropperUpdate( guint mods );
	bool eyeDropperRelease( guint button );
	void eyeDropperClean();
	
	void penInit();
	bool penActivate( guint button, guint mods );
	bool penUpdate( guint mods );
	bool penRelease( guint button );
	void penClean();
	
	void brushInit();
	bool brushActivate( guint button, guint mods );
	bool brushUpdate( guint mods );
	bool brushRelease( guint button );
	void brushClean();
	
	void chgColorInit();
	bool chgColorActivate( guint button, guint mods );
	bool chgColorUpdate( guint mods );
	bool chgColorRelease( guint button );
	void chgColorClean();

	void lineInit();
	bool lineActivate( guint button, guint mods );
	bool lineUpdate( guint mods );
	bool lineRelease( guint button, guint mods );
	void lineClean();

	void rectInit();
	bool rectActivate( guint button, guint mods );
	bool rectUpdate( guint mods );
	bool rectRelease( guint button, guint mods );
	void rectClean();

	void fillInit();
	bool fillActivate( guint button, guint mods );
	//bool fillUpdate( guint mods );
	//bool fillRelease( guint button );
	//void Clean();

};

} // namespace Polka

#endif // _POLKA_BITMAPCANVASEDITOR_H_
