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
	IntSignal signalChangeTool();

	// action/accelerator IDs continued from CanvasView 
	enum { ACC_ACTIVATE_PRI = 1, ACC_ACTIVATE_SEC, ACC_MOD_ACT_FLIP, ACC_SELECT, ACC_SELECT_TILE,
		   ACC_MOD_SELECT_TILE, ACC_MOD_SELECT_SQUARE, ACC_SELECT_FLOAT, ACC_COLORPICK_FG, ACC_COLORPICK_BG,
		   ACC_QUICKPICK_FG, ACC_QUICKPICK_BG, ACC_DRAW_FG, ACC_DRAW_BG, ACC_MOD_DRAW_COL,
		   ACC_LINE_FG, ACC_LINE_BG, ACC_MOD_LINE_ANGLE, ACC_FILL_FG, ACC_FILL_BG,
		   ACC_FLIP_HOR, ACC_FLIP_VER, ACC_ROTATE_CW, ACC_ROTATE_CCW };

protected:
	//virtual bool on_scroll_event(GdkEventScroll* event);
	virtual bool on_button_press_event(GdkEventButton *event);
	virtual bool on_button_release_event(GdkEventButton *event);
	virtual bool on_motion_notify_event(GdkEventMotion* event);
	
	virtual bool on_key_press_event (GdkEventKey* event);
	virtual bool on_key_release_event (GdkEventKey* event);

	virtual bool on_draw( const Cairo::RefPtr<Cairo::Context>& cr );

	virtual void on_hide();
	virtual void on_show();

	virtual void changeCursor( Glib::RefPtr<Gdk::Cursor> cursor = Glib::RefPtr<Gdk::Cursor>() );
	virtual void restoreCursor();
	
	void createUndo( const Glib::ustring& text, const Glib::RefPtr<Gdk::Pixbuf>& icon );

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
	IntSignal m_SignalChangeTool;
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
	bool m_TileSelect, m_UseFGColor;
	int m_FGColor, m_BGColor;
	Pen m_Pen;
	int m_PenColor;
	bool m_PickFG;
	std::vector<Brush*> m_Brushes;
	Glib::RefPtr<Gdk::Cursor> m_refToolCursor;
	Brush *m_pBrush, *m_pTempBrush, *m_pSelectionBrush;

	bool toolActivate( guint button, guint key, guint mods );
	bool toolRelease( guint button, guint key, guint mods );

	void screenDraw( int x, int y, bool use_brush = false );
	
	void changeTool( int id );
	void changeColor( int col, int seccol );
	void changeBrush( int id );
	bool isInPixelArea( int x, int y );
	void clipPixelArea( int& x, int& y, int w = -1, int h = -1 );
	void removeToolMarker();
	void updateCoords( int x, int y );
	bool updateTileCoords();
	bool tileCoords( int x, int y, int& tx, int& ty );
	
	// interactive functions
	Brush *createBrushFromSelection();
	
	// tool handlers
	void rectSelectInit();
	bool rectSelectActivate( guint button, guint key, guint mods );
	bool rectSelectUpdate( guint mods );
	bool rectSelectRelease( guint button, guint key );
	void rectSelectClean();
	void rectSelectModeChanged();
	void rectSelectToBrush();

	void eyeDropperInit();
	bool eyeDropperActivate( guint button, guint key, guint mods );
	bool eyeDropperUpdate( guint mods );
	bool eyeDropperRelease( guint button, guint key );
	void eyeDropperClean();
	
	void penInit();
	bool penActivate( guint button, guint key, guint mods );
	bool penUpdate( guint mods );
	bool penRelease( guint button, guint key );
	void penClean();
	
	void brushInit();
	bool brushActivate( guint button, guint key, guint mods );
	bool brushUpdate( guint mods );
	bool brushRelease( guint button, guint key );
	void brushClean();
	
	void chgColorInit();
	bool chgColorActivate( guint button, guint key, guint mods );
	bool chgColorUpdate( guint mods );
	bool chgColorRelease( guint button, guint key );
	void chgColorClean();

	void lineInit();
	bool lineActivate( guint button, guint key, guint mods );
	bool lineUpdate( guint mods );
	bool lineRelease( guint button, guint key, guint mods );
	void lineClean();

	void rectInit();
	bool rectActivate( guint button, guint key, guint mods );
	bool rectUpdate( guint mods );
	bool rectRelease( guint button, guint key, guint mods );
	void rectClean();

	void fillInit();
	bool fillActivate( guint button, guint key, guint mods );
	//bool fillUpdate( guint mods );
	//bool fillRelease( guint button, guint key );
	//void fillClean();

	void flipInit();
	bool flipActivate( guint button, guint key, guint mods );
	bool flipUpdate( guint mods );
	//bool flipRelease( guint button, guint key );
	void flipClean();

	void rotateInit();
	bool rotateActivate( guint button, guint key, guint mods );
	bool rotateUpdate( guint mods );
	//bool rotateRelease( guint button, guint key );
	void rotateClean();

};

} // namespace Polka

#endif // _POLKA_BITMAPCANVASEDITOR_H_
