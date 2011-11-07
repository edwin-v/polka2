#include "BitmapCanvasEditor.h"
#include "ToolButtonWindow.h"
#include "Canvas.h"
#include "Palette.h"
#include "ResourceManager.h"
#include <gtkmm/image.h>
#include <gdk/gdkkeysyms.h>
#include <iostream>

using namespace std;
namespace Polka {

void createBrushList( std::vector<Brush*>& bl )
{
	const int s1[] = { 1 };
	const int s2[] = { 1, 1, 1,   1, 1, 1,   1, 1, 1 };
	const int s3[] = { -1, 1, -1,   1, 1, 1,   -1, 1, -1 };
	const int s4[] = { -1, 1, 1, 1, -1,   1, 1, 1, 1, 1,   1, 1, 1, 1, 1,   1, 1, 1, 1, 1,   -1, 1, 1, 1, -1 };
	const int s5[] = { -1, -1, 1, -1, -1,   -1, 1, 1, 1, -1,   1, 1, 1, 1, 1,   -1, 1, 1, 1, -1,   -1, -1, 1, -1, -1 };
	Shape *s = new Shape(1, 1);
	bl.push_back(s);
	s->setData(s1);
	s = new Shape(3, 3);
	bl.push_back(s);
	s->setData(s2);
	s = new Shape(3, 3);
	bl.push_back(s);
	s->setData(s3);
	s = new Shape(5, 5);
	bl.push_back(s);
	s->setData(s4);
	s = new Shape(5, 5);
	bl.push_back(s);
	s->setData(s5);
}

/* list of tools
 * 
 * All tools can have the following implemented
 * 
 *  toolInit     - Initialize the tool (button clicked)
 *  toolActivate - Start/Continue using the tool (mouse button clicked)
 *  toolUpdate   - Process (on mouse move if a tool is active)
 *  toolRelease  - End tool or move to next step
 *  toolClean    - Clean up before a different tool is selected
 */

enum {
	TOOL_SELECT = 0,
	TOOL_EYEDROPPER,
	TOOL_PEN,
	TOOL_BRUSH,
	TOOL_CHANGECOLOR,
	TOOL_LINE,
	TOOL_RECT,
	TOOL_FILL
};

const int TOOLMARKER_ID = 9999;


enum { ACC_ACTIVATE = 1, ACC_SELECT_TILE, 
       ACC_COLORPICK_FG, ACC_COLORPICK_BG, ACC_COLORPICK_QUICK, ACC_SECCOL,
       ACC_MOD_SQUARE };


BitmapCanvasEditor::BitmapCanvasEditor( const std::string& _id )
	: CanvasView(_id),
	  m_DragPrimary(false), m_ZoomMode(false),
	  m_CurrentTool(-1), m_ActiveTool(-1), m_pToolMarker(0)
{
	add_events(Gdk::POINTER_MOTION_MASK | Gdk::KEY_PRESS_MASK);
	
	set_can_focus(true);

	// add accelerators
	accAdd( ACC_ACTIVATE          , "activate"         , 1 );
	accAdd( ACC_SELECT_TILE       , "select/tile"      , 0, 0, MOD_CTRL );
	accAdd( ACC_COLORPICK_FG      , "colorpick/fg"     , 1 );
	accAdd( ACC_COLORPICK_BG      , "colorpick/bg"     , 3 );
	accAdd( ACC_COLORPICK_QUICK   , "colorpick/quick"  , 3 );
	accAdd( ACC_SECCOL            , "secondary_color"  , 0, 0, MOD_CTRL );
	accAdd( ACC_MOD_SQUARE        , "square"           , 0, 0, MOD_SHIFT );

	m_pBrush = 0;

	createBrushList(m_Brushes);
	m_ToolBrushPanel.setBrushVector(m_Brushes);
}

BitmapCanvasEditor::~BitmapCanvasEditor()
{
	setCanvas(0);
}

void BitmapCanvasEditor::createTools( ToolButtonWindow& tw )
{
	ResourceManager& rm = ResourceManager::get();
	// TOOL_SELECT: create rectange select tool
	tw.addTool( rm.getIcon("canvasedit_tool_select"), &m_ToolSelectPanel );
	m_ToolSelectPanel.toBrushClicked().connect( sigc::mem_fun(*this, &BitmapCanvasEditor::createBrushFromSelection) );
	m_Overlay.add( 10, new OverlayRectangle( 10, 10, 5, 5 ) );
	
	// TOOL_EYEDROPPER: pick color from canvas
	tw.addTool( rm.getIcon("canvasedit_tool_eyedropper") );

	// 1: create grid set tool
	//tw.addTool( rm.getIcon("canvasedit_tool_grid"), &m_ToolGridPanel );
	// grid selection crosshairs and texts
	m_Overlay.add( 20, new OverlayCross( 0, 0, true, true ) );
	m_Overlay.shape(20).setPrimaryPen( 1, 1, 1, 1 );
	m_Overlay.shape(20).setSecondaryPen( 3, 0, 0, 0, 0.5 );
	m_Overlay.add( 21, new OverlayCross( 0, 0, false, false ) );
	m_Overlay.shape(21).setPrimaryPen( 1, 1, 1, 1 );
	m_Overlay.shape(21).setSecondaryPen( 3, 0, 0, 0, 0.5 );
	m_Overlay.add( 22, new OverlayText( 0, 0, 18, OverlayText::RIGHT, OverlayText::BOTTOM, 8 ) );
	m_Overlay.shape(22).setPrimaryPen( 1, 1, 1, 1 );
	m_Overlay.shape(22).setSecondaryPen( 3, 0, 0, 0 );
	m_Overlay.add( 23, new OverlayText( 0, 0, 18 ) );
	m_Overlay.shape(23).setPrimaryPen( 1, 1, 1, 1 );
	m_Overlay.shape(23).setSecondaryPen( 3, 0, 0, 0 );
	
	// TOOL_PEN: create pen tool
	tw.addTool( rm.getIcon("canvasedit_tool_draw") );
	
	// TOOL_BRUSH: create brush tool
	tw.addTool( rm.getIcon("canvasedit_tool_brush"), &m_ToolBrushPanel );
	m_ToolBrushPanel.signalBrushSelected().connect( sigc::mem_fun(*this, &BitmapCanvasEditor::changeBrush) );
	
	// TOOL_CHANGECOLOR: create color replace tool
	tw.addTool( rm.getIcon("canvasedit_tool_changecolor"), &m_ToolBrushPanel );

	// TOOL_LINE: create line tool
	tw.addTool( rm.getIcon("canvasedit_tool_drawline") );
	
	// TOOL_RECT: create rect tool
	tw.addTool( rm.getIcon("canvasedit_tool_drawrect"), &m_ToolRectPanel );
	
	// TOOL_FILL: create fill tool
	tw.addTool( rm.getIcon("canvasedit_tool_fill") );

	// connect tool window
	tw.signalSelectTool().connect( sigc::mem_fun(*this, &BitmapCanvasEditor::changeTool) );
	
	//tw.present();
}

void BitmapCanvasEditor::setCanvas( Canvas *_canvas )
{
	// unselect canvas
	if( m_ZoomMode ) {
		m_ZoomMode = false;
		unlockView();
	}
	// remove selection
	m_Overlay.shape(10).setVisible(false);
	// basic settings
	removeToolMarker();
	m_DragPrimary = false;
	m_ActiveTool = -1;

	// set canvas
	CanvasView::setCanvas( _canvas );
	
	if( hasCanvas() ) {
		canvas().setClipRectangle();
	}
	grab_focus();
}

void BitmapCanvasEditor::on_hide()
{
	CanvasView::on_hide();
	//m_ToolWindow.hide();
}

void BitmapCanvasEditor::on_show()
{
	CanvasView::on_show();
	//m_ToolWindow.show();
}

void BitmapCanvasEditor::setFGColor( int col )
{
	m_FGColor = col;
}

void BitmapCanvasEditor::setBGColor( int col )
{
	m_BGColor = col;
}

IntSignal BitmapCanvasEditor::signalChangeFGColor()
{
	return m_SignalChangeFGColor;
}

IntSignal BitmapCanvasEditor::signalChangeBGColor()
{
	return m_SignalChangeBGColor;
}

void BitmapCanvasEditor::changeBrush( int id )
{
	if( id >= 0 && id < int(m_Brushes.size()) ) {
		m_pBrush = m_Brushes[id];
		m_pBrush->setColor( m_FGColor );
	} else
		m_pBrush = 0;
}
	
void BitmapCanvasEditor::restoreCursor()
{
	get_window()->set_cursor( m_refToolCursor );
}

void BitmapCanvasEditor::updateCoords( int x, int y )
{
	// store last
	m_LastMouseX = m_MouseX;
	m_LastMouseY = m_MouseY;
	m_LastPixX = m_PixX;
	m_LastPixY = m_PixY;
	// store new coords
	m_MouseX = x;
	m_MouseY = y;
	m_PixX = (dx() + x) / hscale();
	m_PixY = (dy() + y) / vscale();
	m_MouseInArea = m_PixX >= 0 && m_PixY >= 0 &&
	                m_PixX < canvas().width() && m_PixY < canvas().height();
}

bool BitmapCanvasEditor::updateTileCoords()
{
	if( !tileCoords( m_PixX, m_PixY, m_TileX, m_TileY ) ) return false;
	
	m_InFullTile = m_TileX >= 0 && canvas().tileGridHorOffset()+(m_TileX+1)*canvas().tileGridWidth()  < canvas().width() &&
	               m_TileY >= 0 && canvas().tileGridVerOffset()+(m_TileY+1)*canvas().tileGridHeight() < canvas().height();
	return true;
}

bool BitmapCanvasEditor::tileCoords( int x, int y, int& tx, int& ty )
{
	// tile grid
	int gw = canvas().tileGridWidth();
	int gh = canvas().tileGridHeight();
	// is there a full tile grid
	if( gw == 0 || gh == 0 ) return false;

	int gx = canvas().tileGridHorOffset();
	int gy = canvas().tileGridVerOffset();

	// calc tile number
	tx = (x-gx)/gw;
	ty = (y-gy)/gh;
	return true;
}	

void BitmapCanvasEditor::changeTool( int id )
{
	if( m_CurrentTool == id ) return;

	switch( id ) {
		case TOOL_SELECT:
			rectSelectClean();
			break;
		case TOOL_EYEDROPPER:
			eyeDropperClean();
			break;
		case TOOL_PEN: 
			penClean();
			break;
		case TOOL_BRUSH:
			brushClean();
			break;
		case TOOL_CHANGECOLOR:
			chgColorClean();
			break;
		case TOOL_LINE:
			lineClean();
			break;
		case TOOL_RECT:
			rectClean();
			break;
		case TOOL_FILL:
			//fillClean();
			break;
		default:
			break;
	}
	m_DragPrimary = false;

	m_CurrentTool = id;
	resetActiveTool();
	grab_focus();

	switch( id ) {
		case TOOL_SELECT:
			rectSelectInit();
			break;
		case TOOL_EYEDROPPER:
			eyeDropperInit();
			break;
		case TOOL_PEN: 
			penInit();
			break;
		case TOOL_BRUSH:
			brushInit();
			break;
		case TOOL_CHANGECOLOR:
			chgColorInit();
			break;
		case TOOL_LINE:
			lineInit();
			break;
		case TOOL_RECT:
			rectInit();
			break;
		case TOOL_FILL:
			fillInit();
			break;
		default:
			m_refToolCursor = Gdk::Cursor::create(Gdk::ARROW);
	}
	changeCursor( m_refToolCursor );
}

bool BitmapCanvasEditor::on_button_press_event(GdkEventButton *event)
{
	grab_focus();
	updateCoords( event->x, event->y );
	// activate tool
	bool res = false;
	switch( m_CurrentTool ) {
		case TOOL_SELECT:
			res = rectSelectActivate(event->button, event->state);
			break;
		case TOOL_EYEDROPPER:
			res = eyeDropperActivate(event->button, event->state);
			break;
		case TOOL_PEN:
			res = penActivate(event->button, event->state);
			break;
		case TOOL_BRUSH:
			res = brushActivate(event->button, event->state);
			break;
		case TOOL_CHANGECOLOR:
			res = chgColorActivate(event->button, event->state);
			break;
		case TOOL_LINE:
			res = lineActivate(event->button, event->state);
			break;
		case TOOL_RECT:
			res = rectActivate(event->button, event->state);
			break;
		case TOOL_FILL:
			res = fillActivate(event->button, event->state);
			break;
		default:
			break;
	}

	// exit if used
	if( res ) return true;
	
	// no tool used, secondary functions
	if( checkAccButton( ACC_COLORPICK_QUICK, event->button, event->state ) ) {
		// quick colorpick, temp store current
		bool temp = m_DragPrimary;
		Glib::RefPtr<Gdk::Cursor> tempCursor = m_refToolCursor;
		m_DragPrimary = true;
		eyeDropperUpdate( event->state );
		m_DragPrimary = temp;
		m_refToolCursor = tempCursor;
		changeCursor(m_refToolCursor);
	}
	
	
	
	return CanvasView::on_button_press_event(event);
}

bool BitmapCanvasEditor::on_motion_notify_event(GdkEventMotion* event)
{
	// keep track of mouse
	updateCoords( event->x, event->y );
	// notify movement to tool
	bool res = false;
	switch( m_CurrentTool ) {
		case TOOL_SELECT:
			res = rectSelectUpdate( event->state );
			break;
		case TOOL_EYEDROPPER:
			res = eyeDropperUpdate( event->state );
			break;
		case TOOL_PEN:
			res = penUpdate( event->state );
			break;
		case TOOL_BRUSH:
			res = brushUpdate( event->state );
			break;
		case TOOL_CHANGECOLOR:
			res = chgColorUpdate( event->state );
			break;
		case TOOL_LINE:
			res = lineUpdate( event->state );
			break;
		case TOOL_RECT:
			res = rectUpdate( event->state );
			break;
		default:
			break;
	}
	
	// exit if used
	if( res ) return true;

	return CanvasView::on_motion_notify_event(event);
}

bool BitmapCanvasEditor::on_button_release_event(GdkEventButton *event)
{
	updateCoords( event->x, event->y );
	// notify button release to tool
	bool res = false;
	switch( m_CurrentTool ) {
		case TOOL_SELECT:
			res = rectSelectRelease( event->button );
			break;
		case TOOL_EYEDROPPER:
			res = eyeDropperRelease( event->button );
			break;
		case TOOL_PEN:
			res = penRelease( event->button );
			break;
		case TOOL_BRUSH:
			res = brushRelease( event->button );
			break;
		case TOOL_CHANGECOLOR:
			res = chgColorRelease( event->button );
			break;
		case TOOL_LINE:
			res = lineRelease( event->button, event->state );
			break;
		case TOOL_RECT:
			res = rectRelease( event->button, event->state );
			break;
		default:
			break;
	}

	// exit if used
	if( res ) return true;

	return CanvasView::on_button_release_event(event);
}

bool BitmapCanvasEditor::on_key_press_event( GdkEventKey *event )
{
	if( m_ZoomMode ) {
		if(  event->keyval == GDK_KEY_F12 || event->keyval == GDK_KEY_Escape ) {
			m_ZoomMode = false;
			unlockView();
			canvas().setClipRectangle();
		}
	} else {
		if( event->keyval == GDK_KEY_F12 ) {
			// calc tile and zoom
			int hsc = hscale(), vsc = vscale();
			int datx = (dx() + m_MouseX) / hsc;
			int daty = (dy() + m_MouseY) / vsc;
			// tile grid
			int gw = canvas().tileGridWidth();
			int gh = canvas().tileGridHeight();
			// does it exist?
			if( gw == 0 || gh == 0 ) return true;
			
			int gx = canvas().tileGridHorOffset()%gw;
			int gy = canvas().tileGridVerOffset()%gh;

			// skip half start tiles
			if( datx < gx || daty < gy ) return true;

			int sc = 32;
			while( (75+sc*canvas().pixelScaleHor()*gh) > get_height() || (200+sc*canvas().pixelScaleVer()*gw) > get_width() ) sc >>= 1;
			// minimum scale for zoom mode is 2
			if( sc < 2 ) return true; 

			m_TileX = (datx-gx)/gw;
			m_TileY = (daty-gy)/gh;

			// center tile
            datx = m_TileX*gw+gx;
            daty = m_TileY*gh+gy;

            // skip half end tiles
			if( datx+gw > canvas().width() || daty+gh > canvas().height() ) return true;
            
			// lock to zoom mode
			hsc = sc*canvas().pixelScaleHor();
			vsc = sc*canvas().pixelScaleVer();
			lockView( hsc*(datx+gw/2)-get_width()/2, vsc*(daty+gh/2)-get_height()/2, sc );
			
			// clip to tile
			canvas().setClipRectangle( datx, daty, gw, gh );
			
			m_ZoomMode = true;
		}
	}
	return CanvasView::on_key_press_event(event);
}

bool BitmapCanvasEditor::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
	// draw normal stuff
	CanvasView::on_draw(cr);
	
	if( hasCanvas() ) {

		if( m_ZoomMode ) {
			int hsc = hscale(), vsc = vscale();
			int gw = canvas().tileGridWidth();
			int gh = canvas().tileGridHeight();
			int gx = canvas().tileGridHorOffset()%gw;
			int gy = canvas().tileGridVerOffset()%gh;
			
			int tx = hsc*(m_TileX*gw+gx) - dx();
			int ty = vsc*(m_TileY*gh+gy) - dy();
			
			// draw frame
			cr->rectangle( tx-25.5, ty-25.5, gw*hsc+150, gh*vsc+50 );
			cr->begin_new_sub_path();
			cr->rectangle( tx-0.5, ty-0.5, gw*hsc+2, gh*vsc+2 );
			cr->set_fill_rule( Cairo::FILL_RULE_EVEN_ODD );
			cr->set_source_rgba( 0, 0, 0, 0.8 );
			cr->fill_preserve();
			cr->set_source_rgb( 1, 1, 1 );
			cr->set_line_width(1);
			cr->stroke();
		}
		// paint overlays
		m_Overlay.setImageSize( canvas().width(), canvas().height() );
		m_Overlay.setCoordSpace( dx(), dy(), hscale(), vscale() );
		m_Overlay.paint(cr);
	}

	return true;
}

bool BitmapCanvasEditor::isInPixelArea( int x, int y )
{
	if( m_ZoomMode ) {
		// get tile grid
		int gw = canvas().tileGridWidth();
		int gh = canvas().tileGridHeight();
		int gx = canvas().tileGridHorOffset()%gw;
		int gy = canvas().tileGridVerOffset()%gh;
		
		int tx = m_TileX*gw+gx;
		int ty = m_TileY*gh+gy;
		
		return x >= tx && y >= ty && (x-tx)<gw && (y-ty)<gh;
		
	} else {
		return x >= 0 && y >= 0 && x < canvas().width() && y << canvas().height();
	}
}

void BitmapCanvasEditor::clipPixelArea( int& x, int& y )
{
	if( m_ZoomMode ) {
		// get tile grid
		int gw = canvas().tileGridWidth();
		int gh = canvas().tileGridHeight();
		int gx = canvas().tileGridHorOffset()%gw;
		int gy = canvas().tileGridVerOffset()%gh;
		
		int tx = m_TileX*gw+gx;
		int ty = m_TileY*gh+gy;
		
		if( x < tx )
			x = tx;
		else if( x >= tx+gw )
			x = tx+gw-1;

		if( y < ty )
			y = ty;
		else if( y >= ty+gh )
			y = ty+gh-1;
			
	} else {
		if( x < 0 )
			x = 0;
		else if( x >= canvas().width() )
			x = canvas().width() - 1;

		if( y < 0 )
			y = 0;
		else if( y >= canvas().height() )
			y = canvas().height() - 1;
	}
}

void BitmapCanvasEditor::resetActiveTool()
{
	switch( m_ActiveTool ) {
		case TOOL_LINE:
		case TOOL_RECT:
			removeToolMarker();
			break;
		default:
			break;
	}
	m_ActiveTool = -1;
	m_ToolHold = false;
}

/*
 ***********************************************************************
 * Tool handlers
 ***********************************************************************
 */
 
/*
 *--------------
 * RectSelect
 *--------------
 */ 

/**
 * Initialize the rectangle selection tool 
 */
void BitmapCanvasEditor::rectSelectInit()
{
	m_refToolCursor = ResourceManager::get().getCursor(get_window(), "canvasedit_selectrect");	
}

/**
 * Start dragging a selection rectangle
 */
bool BitmapCanvasEditor::rectSelectActivate( guint button, guint mods )
{
	if( checkAccButton(ACC_ACTIVATE, button) && m_MouseInArea ) {
		// start drag
		m_DragPrimary = true;
		m_DragStartX = m_PixX;
		m_DragStartY = m_PixY;

		// turn off slow drawing features
		setFastUpdate();
		
		if( checkAccMods(ACC_SELECT_TILE, mods) && updateTileCoords() ) {
			// select initial tile
			if( m_InFullTile ) 
				rectSelectUpdate( mods );
		}
		return true;
	}
	return false;
}

/**
 * Start dragging a selection rectangle
 */
bool BitmapCanvasEditor::rectSelectUpdate( guint mods )
{
	if( m_DragPrimary ) {
		int x1, x2, y1, y2;
		if( checkAccMods(ACC_SELECT_TILE, mods) && updateTileCoords() ) {
			// select tiles
			int tsx, tsy, tex, tey;
			tileCoords( m_DragStartX, m_DragStartY, tsx, tsy );
			if( tsx <= m_TileX ) {
				tex = m_TileX;
			} else {
				tex = tsx;
				tsx = m_TileX;
			}
			if( tsy <= m_TileY ) {
				tey = m_TileY;
			} else {
				tey = tsy;
				tsy = m_TileY;
			}
			// clip to whole tiles
			if( tsx < 0 ) tsx = 0;
			if( tsy < 0 ) tsy = 0;
			int tmax = (canvas().width() - canvas().tileGridHorOffset()) / canvas().tileGridWidth();
			if( tex >= tmax ) tex = tmax - 1;
			tmax = (canvas().height() - canvas().tileGridVerOffset()) / canvas().tileGridHeight();
			if( tey >= tmax ) tey = tmax - 1;
			//
			x1 = canvas().tileGridHorOffset() + tsx * canvas().tileGridWidth();
			y1 = canvas().tileGridVerOffset() + tsy * canvas().tileGridHeight();
			x2 = canvas().tileGridHorOffset() + (tex+1) * canvas().tileGridWidth() - 1;
			y2 = canvas().tileGridVerOffset() + (tey+1) * canvas().tileGridHeight() - 1;
		} else {
			// select pixels
			if( m_PixX < m_DragStartX ) {
				x1 = m_PixX;
				x2 = m_DragStartX;
			} else {
				x1 = m_DragStartX;
				x2 = m_PixX;
			}
			if( m_PixY < m_DragStartY ) {
				y1 = m_PixY;
				y2 = m_DragStartY;
			} else {
				y1 = m_DragStartY;
				y2 = m_PixY;
			}
			clipPixelArea(x1, y1);
			clipPixelArea(x2, y2);
		}
		m_Overlay.shape(10).setLocation(x1, y1);
		m_Overlay.shape(10).setSize( 1+x2-x1, 1+y2-y1 );
		m_Overlay.shape(10).setVisible( x1 != x2 || y1 != y2 );
		queue_draw();
		return true;
	}
	
	return false;
}

/**
 * End dragging of selection rectangle
 */
bool BitmapCanvasEditor::rectSelectRelease( guint button )
{
	if( checkAccButton(ACC_ACTIVATE, button) ) {
		// restore slow drawing features
		setFastUpdate(false);
		m_DragPrimary = false;
		queue_draw();
		return true;
	}
	return false;
}

/**
 * Disable selection
 */
void BitmapCanvasEditor::rectSelectClean()
{
	m_Overlay.shape(10).setVisible(false);
}


/*
 *--------------
 * EyeDropper
 *--------------
 */ 

/**
 * Initialize the pen drawing tool 
 */
void BitmapCanvasEditor::eyeDropperInit()
{
	m_refToolCursor = ResourceManager::get().getCursor(get_window(), "canvasedit_eyedropper_fg");
	m_PickFG = true;
}

bool BitmapCanvasEditor::eyeDropperActivate( guint button, guint mods )
{
	if( m_MouseInArea && !m_DragPrimary ) {
		// pick under mouse
		if( checkAccButton(ACC_COLORPICK_FG, button, mods) ) {
			m_DragPrimary = true;
			m_PickFG = true;
			m_refToolCursor = ResourceManager::get().getCursor(get_window(), "canvasedit_eyedropper_fg");	
		} else if( checkAccButton(ACC_COLORPICK_BG, button, mods) ) {
			m_DragPrimary = true;
			m_PickFG = false;
			m_refToolCursor = ResourceManager::get().getCursor(get_window(), "canvasedit_eyedropper_bg");	
		} else {
			return false;
		}
		changeCursor( m_refToolCursor );
		eyeDropperUpdate(mods);
		return true;
	}
	return false;
}

bool BitmapCanvasEditor::eyeDropperUpdate( guint mods )
{
	// pick color if down
	if( m_DragPrimary ) {
		if( m_PickFG )
			m_SignalChangeFGColor.emit( canvas().data( m_PixX, m_PixY ) );
		else
			m_SignalChangeBGColor.emit( canvas().data( m_PixX, m_PixY ) );
		return true;
	}
	return false;
}

bool BitmapCanvasEditor::eyeDropperRelease( guint button )
{
	if( ( m_PickFG && checkAccButton( ACC_COLORPICK_FG, button )) ||
		(!m_PickFG && checkAccButton( ACC_COLORPICK_BG, button )) )
	{
		m_DragPrimary = false;
		return true;
	}
	return false;
}

void BitmapCanvasEditor::eyeDropperClean()
{
}


/*
 *--------------
 * Pen
 *--------------
 */ 

/**
 * Initialize the pen drawing tool 
 */
void BitmapCanvasEditor::penInit()
{
	m_refToolCursor = Gdk::Cursor::create(Gdk::PENCIL);
}

/**
 * Start drawing, intialize pen color and undo action
 */
bool BitmapCanvasEditor::penActivate( guint button, guint mods )
{
	if( checkAccButton(ACC_ACTIVATE, button) && m_MouseInArea ) {

		// initial draw, reset color
		m_PenColor = -1;
		// start action
		canvas().startAction( _("Draw pencil"), ResourceManager::get().getIcon("canvasedit_tool_draw") );
		m_DragPrimary = true;

		// draw initial pixel
		m_LastPixX = m_PixX;
		m_LastPixY = m_PixY;
		penUpdate( mods );

		return true;
		
	}
	return false;
}

/**
 * Actual drawing when activated
 */
bool BitmapCanvasEditor::penUpdate( guint mods )
{
	if( m_DragPrimary ) {
		// color changed?
		if( checkAccMods(ACC_SECCOL, mods) ) {
			if( m_PenColor != m_BGColor ) {
				m_PenColor = m_BGColor;
				m_Pen.setColor( m_BGColor );
			}				
		} else {
			if( m_PenColor != m_FGColor ) {
				m_PenColor = m_FGColor;
				m_Pen.setColor( m_FGColor );
			}				
		}
		// draw
		int w = 1+abs(m_PixX - m_LastPixX), h = 1+abs(m_PixY - m_LastPixY);
		if( w < 2 && h < 2 ) {
			// draw single point
			canvas().draw( m_PixX, m_PixY, m_Pen );
		} else {
			// draw line
			canvas().drawLine( m_LastPixX, m_LastPixY, m_PixX, m_PixY, m_Pen );
		}
		return true;
	}
	return false;
}

/**
 * Release pen activation and finalize undo action
 */
bool BitmapCanvasEditor::penRelease( guint button )
{
	if( checkAccButton(ACC_ACTIVATE, button) ) {
		canvas().finishAction();
		m_DragPrimary = false;
		return true;
	}
	return false;
}

/**
 * Disable pen
 */
void BitmapCanvasEditor::penClean()
{
}


/*
 *--------------
 * Brush
 *--------------
 */ 

/**
 * Initialize the brush drawing tool 
 */
void BitmapCanvasEditor::brushInit()
{
	m_refToolCursor = Gdk::Cursor::create(Gdk::TCROSS);
	// default brush
	if( !m_pBrush )
		m_pBrush = m_Brushes[0];
}

/**
 * Start drawing, intialize brush color and undo action
 */
bool BitmapCanvasEditor::brushActivate( guint button, guint mods )
{
	if( checkAccButton(ACC_ACTIVATE, button) && m_MouseInArea ) {

		// initial draw, reset color
		m_PenColor = -1;
		// start action
		canvas().startAction( _("Draw brush"), ResourceManager::get().getIcon("canvasedit_tool_brush") );
		m_DragPrimary = true;
		
		brushUpdate( mods );

		return true;
		
	}
	return false;
}

/**
 * Actual drawing when activated
 */
bool BitmapCanvasEditor::brushUpdate( guint mods )
{
	if( m_DragPrimary ) {
		// color changed?
		if( checkAccMods(ACC_SECCOL, mods) ) {
			if( m_PenColor != m_BGColor ) {
				m_PenColor = m_BGColor;
				m_pBrush->setColor( m_BGColor );
			}				
		} else {
			if( m_PenColor != m_FGColor ) {
				m_PenColor = m_FGColor;
				m_pBrush->setColor( m_FGColor );
			}				
		}
		// draw
		canvas().draw( m_PixX, m_PixY, *m_pBrush );
		return true;
	}
	return false;
}

/**
 * Release brush activation and finalize undo action
 */
bool BitmapCanvasEditor::brushRelease( guint button )
{
	if( checkAccButton(ACC_ACTIVATE, button) ) {
		canvas().finishAction();
		m_DragPrimary = false;
		return true;
	}
	return false;
}

/**
 * Disable brush
 */
void BitmapCanvasEditor::brushClean()
{
}


/*
 *--------------
 * change color
 *--------------
 */ 

/**
 * Initialize the change color brush drawing tool 
 */
void BitmapCanvasEditor::chgColorInit()
{
	m_refToolCursor = ResourceManager::get().getCursor(get_window(), "canvasedit_changecolor");
	// default brush
	if( !m_pBrush )
		m_pBrush = m_Brushes[0];
}

/**
 * Start drawing, intialize brush color and undo action
 */
bool BitmapCanvasEditor::chgColorActivate( guint button, guint mods )
{
	if( checkAccButton(ACC_ACTIVATE, button) && m_MouseInArea ) {

		// initial draw, reset color
		m_PenColor = -1;
		// start action
		canvas().startAction( _("Change color"), ResourceManager::get().getIcon("canvasedit_tool_changecolor") );
		m_DragPrimary = true;
		
		chgColorUpdate( mods );

		return true;
		
	}
	return false;
}

/**
 * Actual drawing when activated
 */
bool BitmapCanvasEditor::chgColorUpdate( guint mods )
{
	if( m_DragPrimary ) {
		// color changed?
		if( checkAccMods(ACC_SECCOL, mods) ) {
			if( m_PenColor != m_BGColor ) {
				m_PenColor = m_BGColor;
				m_pBrush->setColor( m_BGColor );
			}				
		} else {
			if( m_PenColor != m_FGColor ) {
				m_PenColor = m_FGColor;
				m_pBrush->setColor( m_FGColor );
			}				
		}
		// draw
		canvas().changeColorDraw( m_PixX, m_PixY, *m_pBrush, m_PenColor==m_FGColor?m_BGColor:m_FGColor );
		return true;
	}
	return false;
}

/**
 * Release brush activation and finalize undo action
 */
bool BitmapCanvasEditor::chgColorRelease( guint button )
{
	if( checkAccButton(ACC_ACTIVATE, button) ) {
		canvas().finishAction();
		m_DragPrimary = false;
		return true;
	}
	return false;
}

/**
 * Disable brush
 */
void BitmapCanvasEditor::chgColorClean()
{
}


/*
 *--------------
 * Line
 *--------------
 */ 

/**
 * Initialize the line tool 
 */
void BitmapCanvasEditor::lineInit()
{
	m_refToolCursor = Gdk::Cursor::create(Gdk::CROSSHAIR);
}

/**
 * Start drawing, intialize pen color and undo action
 */
bool BitmapCanvasEditor::lineActivate( guint button, guint mods )
{
	if( checkAccButton( ACC_ACTIVATE, button, mods ) ) {
		// only start if primary button not previously pressed
		if( !m_DragPrimary ) {
			// only mark start
			m_DragPrimary = true;
			m_DragStartX = m_PixX;
			m_DragStartY = m_PixY;
			// create marker object
			m_pToolMarker = new OverlayLine( m_PixX, m_PixY, 0, 0 );
			m_pToolMarker->setPrimaryPen( 1, 1, 1, 1 );
			m_pToolMarker->setSecondaryPen( 1, 0, 0, 0 );
			m_pToolMarker->setVisible(false);
			m_Overlay.add( TOOLMARKER_ID, m_pToolMarker );
		}
		return true;
	}
	return false;
}

/**
 * Draw line marker to target based on modifier. 
 */
bool BitmapCanvasEditor::lineUpdate( guint mods )
{
	if( m_DragPrimary ) {
		// calc end coord
		m_DragEndX = m_PixX; m_DragEndY = m_PixY;
		int dx = m_DragEndX - m_DragStartX, dy = m_DragEndY - m_DragStartY;
		if( dx && dy ) {
			int sx = dx>0?1:-1, sy = dy>0?1:-1;
			if( checkAccMods( ACC_MOD_SQUARE, mods ) ) {
				// modify directions
				if( abs(dx) > abs(dy) ) {
					int f = round( double(dx + sx) / (dy + sy) );
					if( f > 2 || f < -2 ) {
						m_DragEndY = m_DragStartY;
					} else {
						int h = round( double(dx + sx) / f );
						int w = f * h;
						m_DragEndX = m_DragStartX + w-sx;
						m_DragEndY = m_DragStartY + h-sy;
					}
				} else {
					int f = round( double(dy + sy) / (dx + sx) );
					if( f > 2 || f < -2 ) {
						m_DragEndX = m_DragStartX;
					} else {
						int w = round( double(dy + sy) / f );
						int h = f * w;
						m_DragEndX = m_DragStartX + w-sx;
						m_DragEndY = m_DragStartY + h-sy;
					}
				}
			}
		}
		int x = m_pToolMarker->x(), y = m_pToolMarker->y(), w = m_pToolMarker->width(), h = m_pToolMarker->height();
		canvasChanged( Gdk::Rectangle( min(x, x+w), min(y, y+h), 1+abs(w), 1+abs(h) ) );
		m_pToolMarker->setSize( m_DragEndX-m_DragStartX, m_DragEndY-m_DragStartY );
		m_pToolMarker->setVisible();
		canvasChanged( Gdk::Rectangle( min(m_DragEndX, m_DragStartX), min(m_DragEndY, m_DragStartY),
		                               1+abs(m_DragEndX - m_DragStartX), 1+abs(m_DragEndY - m_DragStartY) ) );
	}
	// allow other actions
	return false;
}

/**
 * Release and draw line or do nothing if still on start pixel and draw
 * on second click.
 */
bool BitmapCanvasEditor::lineRelease( guint button, guint mods )
{
	if( checkAccButton(ACC_ACTIVATE, button) ) {
		// if on same pixel, allow release
		if( m_PixX == m_DragStartX && m_PixY == m_DragStartY ) {
			return true;
		}
		// set pen color
		if( checkAccMods( ACC_SECCOL, mods ) )
			m_Pen.setColor( m_BGColor );
		else
			m_Pen.setColor( m_FGColor );
		// prequeue marker removal
		int x = m_pToolMarker->x(), y = m_pToolMarker->y(), w = m_pToolMarker->width(), h = m_pToolMarker->height();
		canvasChanged( Gdk::Rectangle( min(x, x+w), min(y, y+h), 1+abs(w), 1+abs(h) ) );
		// draw line
		canvas().startAction( _("Line"), ResourceManager::get().getIcon("canvasedit_tool_drawline") );
		canvas().drawLine( m_DragStartX, m_DragStartY, m_DragEndX, m_DragEndY, m_Pen );
		canvas().finishAction();
		m_DragPrimary = false;
		// remove marker
		removeToolMarker();
		return true;
	}
	return false;
}

/**
 * Disable line
 */
void BitmapCanvasEditor::lineClean()
{
	if( m_pToolMarker ) {
		removeToolMarker();
		queue_draw();
	}
}


/*
 *--------------
 * Rect
 *--------------
 */ 

/**
 * Initialize the rectangle tool 
 */
void BitmapCanvasEditor::rectInit()
{
	m_refToolCursor = Gdk::Cursor::create(Gdk::CROSSHAIR);
}

/**
 * Start drawing, intialize pen color and undo action
 */
bool BitmapCanvasEditor::rectActivate( guint button, guint mods )
{
	if( checkAccButton( ACC_ACTIVATE, button, mods ) ) {
		// only start if primary button not previously pressed
		if( !m_DragPrimary ) {
			// only mark start
			m_DragPrimary = true;
			m_DragStartX = m_PixX;
			m_DragStartY = m_PixY;
			// create marker object
			m_pToolMarker = new OverlayRectangle( m_PixX, m_PixY, 0, 0 );
			m_pToolMarker->setPrimaryPen( 1, 1, 1, 1 );
			m_pToolMarker->setSecondaryPen( 1, 0, 0, 0 );
			m_pToolMarker->setVisible(false);
			dynamic_cast<OverlayRectangle*>(m_pToolMarker)->setPattern(3);
			m_Overlay.add( TOOLMARKER_ID, m_pToolMarker );
		}
		return true;
	}
	return false;
}

/**
 * Draw rect marker to target based on modifier. 
 */
bool BitmapCanvasEditor::rectUpdate( guint mods )
{
	if( m_DragPrimary ) {
		// calc end coord
		m_DragEndX = m_PixX; m_DragEndY = m_PixY;
		int dx = m_DragEndX - m_DragStartX, dy = m_DragEndY - m_DragStartY;
		if( dx && dy ) {
			int sx = dx>0?1:-1, sy = dy>0?1:-1;
			if( checkAccMods( ACC_MOD_SQUARE, mods ) ) {
				// modify to square
				if( abs(dx) > abs(dy) ) {
					m_DragEndX = m_DragStartX + sx*abs(dy);
				} else {
					m_DragEndY = m_DragStartY + sy*abs(dx);
				}
			}
		}
		int x = m_pToolMarker->x(), y = m_pToolMarker->y(), w = m_pToolMarker->width(), h = m_pToolMarker->height();
		canvasChanged( Gdk::Rectangle( min(x, x+w)-1, min(y, y+h)-1, 3+abs(w), 3+abs(h) ) );
		m_pToolMarker->setSize( m_DragEndX-m_DragStartX, m_DragEndY-m_DragStartY );
		m_pToolMarker->setVisible();
		canvasChanged( Gdk::Rectangle( min(m_DragEndX, m_DragStartX)-1, min(m_DragEndY, m_DragStartY)-1,
		                               3+abs(m_DragEndX - m_DragStartX), 3+abs(m_DragEndY - m_DragStartY) ) );
	}
	// allow other actions
	return false;
}

/**
 * Release and draw rectangle or do nothing if still on start pixel and 
 * draw on second click.
 */
bool BitmapCanvasEditor::rectRelease( guint button, guint mods )
{
	if( checkAccButton(ACC_ACTIVATE, button) ) {
		// if on same pixel, allow release
		if( m_PixX == m_DragStartX && m_PixY == m_DragStartY ) {
			return true;
		}
		// set pen colors
		Pen fillPen;
		if( checkAccMods( ACC_SECCOL, mods ) ) {
			m_Pen.setColor( m_BGColor );
			fillPen.setColor( m_FGColor );
		} else {
			m_Pen.setColor( m_FGColor );
			fillPen.setColor( m_BGColor );
		}
		// prequeue marker removal
		int x = m_pToolMarker->x(), y = m_pToolMarker->y(), w = m_pToolMarker->width(), h = m_pToolMarker->height();
		canvasChanged( Gdk::Rectangle( min(x, x+w)-1, min(y, y+h)-1, 3+abs(w), 3+abs(h) ) );
		// draw line
		canvas().startAction( _("Rectangle"), ResourceManager::get().getIcon("canvasedit_tool_drawrect") );
		canvas().drawRect( m_DragStartX, m_DragStartY, m_DragEndX, m_DragEndY, m_Pen, fillPen );
		canvas().finishAction();
		m_DragPrimary = false;
		// remove marker
		removeToolMarker();
		return true;
	}
	return false;
}

/**
 * Disable rectangle
 */
void BitmapCanvasEditor::rectClean()
{
	if( m_pToolMarker ) {
		removeToolMarker();
		queue_draw();
	}
}

/*
 *--------------
 * Fill
 *--------------
 */ 

/**
 * Initialize the flood fill tool 
 */
void BitmapCanvasEditor::fillInit()
{
	m_refToolCursor = ResourceManager::get().getCursor(get_window(), "canvasedit_fill");
}

/**
 * Fill at cursor, color determined by modifier
 */
bool BitmapCanvasEditor::fillActivate( guint button, guint mods )
{
	if( checkAccButton( ACC_ACTIVATE, button, mods ) ) {
		// only start if primary button not previously pressed
		if( m_MouseInArea ) {
			if( checkAccMods( ACC_SECCOL, mods ) )
				m_Pen.setColor( m_BGColor );
			else
				m_Pen.setColor( m_FGColor );
			canvas().startAction( _("Bucket fill"), ResourceManager::get().getIcon("canvasedit_tool_fill") );
			canvas().bucketFill( m_PixX, m_PixY, m_Pen );
			canvas().finishAction();
		}
		return true;
	}
	return false;
}




void BitmapCanvasEditor::createBrushFromSelection()
{
	if( !m_Overlay.shape(10).isVisible() ) return;
	
	int sx = m_Overlay.shape(10).x();
	int sy = m_Overlay.shape(10).y();
	int sw = m_Overlay.shape(10).width();
	int sh = m_Overlay.shape(10).height();
	
	if( sw == 0 || sh == 0 ) return;
	
	Brush *s = canvas().createBrushFromRect( sx, sy, sw, sh, m_BGColor );
	m_Brushes.push_back(s);
	m_ToolBrushPanel.regenerate();
	m_ToolBrushPanel.selectBrush( m_Brushes.size()-1 );
	//m_ToolWindow.activateTool( TOOL_BRUSH ); XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
}

void BitmapCanvasEditor::removeToolMarker()
{
	if( m_pToolMarker ) {
		m_Overlay.remove(TOOLMARKER_ID);
		delete m_pToolMarker;
		m_pToolMarker = 0;
	}
}



} // namespace Polka
