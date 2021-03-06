/*
	Copyright (C) 2013 Edwin Velds

    This file is part of Polka 2.

    Polka 2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Polka 2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Polka 2.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "BitmapCanvasEditor.h"
#include "ToolButtonWindow.h"
#include "Canvas.h"
#include "Project.h"
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
	TOOL_FILL,
	TOOL_FLIP,
	TOOL_ROTATE
};

const int TOOLMARKER_ID = 9999;

enum { SELECT_MODE_NONE, SELECT_MODE_MOVE, SELECT_MODE_SCALEUP,
       SELECT_MODE_SCALEDOWN, SELECT_MODE_SCALELEFT, SELECT_MODE_SCALERIGHT,
       SELECT_MODE_SCALEUPLEFT, SELECT_MODE_SCALEDOWNLEFT,
       SELECT_MODE_SCALEUPRIGHT, SELECT_MODE_SCALEDOWNRIGHT,
       SELECT_MODE_MOVEFLOATING, SELECT_MODE_APPLYFLOATING };

enum { FLIP_MODE_NONE, FLIP_MODE_FLOAT, FLIP_MODE_SELECT, FLIP_MODE_ALL };


BitmapCanvasEditor::BitmapCanvasEditor( const std::string& _id )
	: CanvasView(_id),
	  m_DragTool(false), m_ZoomMode(false),
	  m_CurrentTool(-1), m_ActiveTool(-1)
{
	add_events(Gdk::POINTER_MOTION_MASK | Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);
	
	set_can_focus(true);
	set_hexpand();
	set_vexpand();

	m_pBrush = 0;
	m_pTempBrush = 0;
	m_pSelectionBrush = 0;
	
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
	m_ToolSelectPanel.floatModeChanged().connect( sigc::mem_fun(*this, &BitmapCanvasEditor::rectSelectModeChanged) );
	m_ToolSelectPanel.toBrushClicked().connect( sigc::mem_fun(*this, &BitmapCanvasEditor::rectSelectToBrush) );
	m_rSelectionRect = RectangleShape::create(RectangleShape::LINE_OUTSIDE);
	add( 10, m_rSelectionRect );
	m_rSelectionMarker = BrushShape::create();
	add( 11, m_rSelectionMarker );
	
	// TOOL_EYEDROPPER: pick color from canvas
	tw.addTool( rm.getIcon("canvasedit_tool_eyedropper") );

	// 1: create grid set tool
	//tw.addTool( rm.getIcon("canvasedit_tool_grid"), &m_ToolGridPanel );
	// grid selection crosshairs and texts
	//m_Overlay.add( 20, new OverlayCross( 0, 0, true, true ) );
	//m_Overlay.shape(20).setPrimaryPen( 1, 1, 1, 1 );
	//m_Overlay.shape(20).setSecondaryPen( 3, 0, 0, 0, 0.5 );
	//m_Overlay.add( 21, new OverlayCross( 0, 0, false, false ) );
	//m_Overlay.shape(21).setPrimaryPen( 1, 1, 1, 1 );
	//m_Overlay.shape(21).setSecondaryPen( 3, 0, 0, 0, 0.5 );
	//m_Overlay.add( 22, new OverlayText( 0, 0, 18, OverlayText::RIGHT, OverlayText::BOTTOM, 8 ) );
	//m_Overlay.shape(22).setPrimaryPen( 1, 1, 1, 1 );
	//m_Overlay.shape(22).setSecondaryPen( 3, 0, 0, 0 );
	//m_Overlay.add( 23, new OverlayText( 0, 0, 18 ) );
	//m_Overlay.shape(23).setPrimaryPen( 1, 1, 1, 1 );
	//m_Overlay.shape(23).setSecondaryPen( 3, 0, 0, 0 );

	// TOOL_PEN: create pen tool
	tw.addTool( rm.getIcon("canvasedit_tool_draw") );

	// TOOL_BRUSH: create brush tool
	tw.addTool( rm.getIcon("canvasedit_tool_brush"), &m_ToolBrushPanel );
	m_ToolBrushPanel.signalBrushSelected().connect( sigc::mem_fun(*this, &BitmapCanvasEditor::changeBrush) );
	m_rBrushMarker = BrushShape::create( true, true, BrushShape::OUTLINE_SHAPED );
	m_rBrushMarker->setLineDashSize(2);
	add( 15, m_rBrushMarker );

	// TOOL_CHANGECOLOR: create color replace tool
	tw.addTool( rm.getIcon("canvasedit_tool_changecolor"), &m_ToolBrushPanel );

	// TOOL_LINE: create line tool
	tw.addTool( rm.getIcon("canvasedit_tool_drawline") );

	// TOOL_RECT: create rect tool
	tw.addTool( rm.getIcon("canvasedit_tool_drawrect"), &m_ToolRectPanel );

	// TOOL_FILL: create fill tool
	tw.addTool( rm.getIcon("canvasedit_tool_fill") );

	// TOOL_FILL: create fill tool
	tw.addTool( rm.getIcon("canvasedit_tool_flip") );

	// TOOL_FILL: create fill tool
	tw.addTool( rm.getIcon("canvasedit_tool_rotate") );

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
	m_rSelectionMarker->setVisible(false);
	// basic settings
	removeToolMarker();
	m_DragTool = false;
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
	if( m_CurrentTool == TOOL_BRUSH ) m_pBrush->setColor(col);
}

void BitmapCanvasEditor::setBGColor( int col )
{
	m_BGColor = col;
	if( m_CurrentTool == TOOL_SELECT ) rectSelectModeChanged();
}

IntSignal BitmapCanvasEditor::signalChangeFGColor()
{
	return m_SignalChangeFGColor;
}

IntSignal BitmapCanvasEditor::signalChangeBGColor()
{
	return m_SignalChangeBGColor;
}

IntSignal BitmapCanvasEditor::signalChangeTool()
{
	return m_SignalChangeTool;
}

void BitmapCanvasEditor::changeBrush( int id )
{
	if( id >= 0 && id < int(m_Brushes.size()) ) {
		m_pBrush = m_Brushes[id];
		m_pBrush->setColor( m_FGColor );
		m_rBrushMarker->setBrush( *m_pBrush, canvas().palette() );
	} else {
		m_pBrush = 0;
		m_rBrushMarker->unsetBrush();
	}
}

void BitmapCanvasEditor::changeCursor( Glib::RefPtr<Gdk::Cursor> cursor )
{
	m_refToolCursor = cursor;
	CanvasView::changeCursor(cursor);
}

void BitmapCanvasEditor::restoreCursor()
{
	get_window()->set_cursor( m_refToolCursor );
}

void BitmapCanvasEditor::createUndo( const Glib::ustring& text, const Glib::RefPtr<Gdk::Pixbuf>& icon )
{
	canvas().project().undoHistory().createUndoPoint(text, icon);
	canvas().startAction(text, icon);
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

void BitmapCanvasEditor::setShapeLine( const Cairo::RefPtr<LineShapeBase>& shape, bool interactive )
{
	if( interactive ) {
		// use a dashed line for interactive shapes
		shape->setBaseWidth(1);
		shape->setBaseColor(0, 0, 0);
		shape->setBaseDashSize(0);
		shape->setLineWidth(1);
		shape->setLineColor(1, 1, 1);
		shape->setLineDashSize(4);
	} else {
		// use a thicker line for semi permanent objects (like selections)
		shape->setBaseWidth(3);
		shape->setBaseColor(0, 0, 0, 0.8);
		shape->setBaseDashSize(0);
		shape->setLineWidth(1);
		shape->setLineColor(1, 1, 1);
		shape->setLineDashSize(0);
	}
}

void BitmapCanvasEditor::changeTool( int id )
{
	if( m_CurrentTool == id ) return;

	switch( m_CurrentTool ) {
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
		case TOOL_FLIP:
			flipClean();
			break;
		case TOOL_ROTATE:
			rotateClean();
			break;
		default:
			break;
	}
	m_DragTool = false;

	m_CurrentTool = id;
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
		case TOOL_FLIP:
			flipInit();
			break;
		case TOOL_ROTATE:
			rotateInit();
			break;
		default:
			changeCursor( Gdk::Cursor::create(Gdk::ARROW) );
	}
}

bool BitmapCanvasEditor::on_button_press_event(GdkEventButton *event)
{
	grab_focus();
	updateCoords( event->x, event->y );
	
	if( toolActivate(accelEventButton(event), 0, event->state) )
		return true;
	
	return CanvasView::on_button_press_event(event);
}

bool BitmapCanvasEditor::on_motion_notify_event(GdkEventMotion* event)
{
	// keep track of mouse
	updateCoords( event->x, event->y );

	if( toolUpdate(event->state) )
		return true;

	return CanvasView::on_motion_notify_event(event);
}

bool BitmapCanvasEditor::on_button_release_event(GdkEventButton *event)
{
	updateCoords( event->x, event->y );

	// exit if used
	if( toolRelease(event->button, 0, event->state) )
		return true;

	return CanvasView::on_button_release_event(event);
}

bool BitmapCanvasEditor::on_key_press_event( GdkEventKey *event )
{
	if( m_ZoomMode ) {
		if(  event->keyval == GDK_KEY_F12 || event->keyval == GDK_KEY_Escape ) {
			m_ZoomMode = false;
			unlockView();
			canvas().setClipRectangle();
			return true;
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
			
			// verify selection
			if( m_rSelectionRect->x() < datx || m_rSelectionRect->y() < daty ||
			    m_rSelectionRect->x()+m_rSelectionRect->width() > datx+gw ||
			    m_rSelectionRect->y()+m_rSelectionRect->height() > daty+gh ) m_rSelectionRect->setVisible(false);
			    
			return true;
		}
	}
	
	if( toolActivate( 0, event->keyval, event->state ) )
		return true;

	if( keyIsMod(event->keyval) )
		if( toolUpdate( event->state ^ keyToMod(event->keyval) ) )
			return true;
	
	return CanvasView::on_key_press_event(event);
}

bool BitmapCanvasEditor::on_key_release_event( GdkEventKey *event )
{
	if( toolRelease(0, event->keyval, event->state) )
		return true;
		
	if( keyIsMod(event->keyval) )
		if( toolUpdate( event->state ^ keyToMod(event->keyval) ) )
			return true;

	return CanvasView::on_key_release_event(event);
}

bool BitmapCanvasEditor::toolActivate( guint button, guint key, guint mods )
{
	// activate tool
	bool res = false;
	switch( m_CurrentTool ) {
		case TOOL_SELECT:
			res = rectSelectActivate(button, key, mods);
			break;
		case TOOL_EYEDROPPER:
			res = eyeDropperActivate(button, key, mods);
			break;
		case TOOL_PEN:
			res = penActivate(button, key, mods);
			break;
		case TOOL_BRUSH:
			res = brushActivate(button, key, mods);
			break;
		case TOOL_CHANGECOLOR:
			res = chgColorActivate(button, key, mods);
			break;
		case TOOL_LINE:
			res = lineActivate(button, key, mods);
			break;
		case TOOL_RECT:
			res = rectActivate(button, key, mods);
			break;
		case TOOL_FILL:
			res = fillActivate(button, key, mods);
			break;
		case TOOL_FLIP:
			res = flipActivate(button, key, mods);
			break;
		case TOOL_ROTATE:
			res = rotateActivate(button, key, mods);
			break;
		default:
			break;
	}

	// exit if used
	if( res ) return true;
	
	// no tool used, secondary functions
	int acc = isAccel( {ACC_QUICKPICK_FG, ACC_QUICKPICK_BG}, button, key, mods );
	if( acc == ACC_QUICKPICK_FG ) {
		bool temp = m_DragTool;
		m_DragTool = true;
		m_PickFG = true;
		eyeDropperUpdate( mods );
		m_DragTool = temp;
		return true;
	} else if( acc == ACC_QUICKPICK_BG ) {
		bool temp = m_DragTool;
		m_DragTool = true;
		m_PickFG = false;
		eyeDropperUpdate( mods );
		m_DragTool = temp;
		return true;
	}
	return false;
}

bool BitmapCanvasEditor::toolUpdate( guint mods )
{
	// notify movement to tool
	bool res = false;
	switch( m_CurrentTool ) {
		case TOOL_SELECT:
			res = rectSelectUpdate( mods );
			break;
		case TOOL_EYEDROPPER:
			res = eyeDropperUpdate( mods );
			break;
		case TOOL_PEN:
			res = penUpdate( mods );
			break;
		case TOOL_BRUSH:
			res = brushUpdate( mods );
			break;
		case TOOL_CHANGECOLOR:
			res = chgColorUpdate( mods );
			break;
		case TOOL_LINE:
			res = lineUpdate( mods );
			break;
		case TOOL_RECT:
			res = rectUpdate( mods );
			break;
		case TOOL_FLIP:
			res = flipUpdate( mods );
			break;
		case TOOL_ROTATE:
			res = rotateUpdate( mods );
			break;
		default:
			break;
	}
	return res;
}

bool BitmapCanvasEditor::toolRelease( guint button, guint key, guint mods )
{
	// notify button release to tool
	bool res = false;
	switch( m_CurrentTool ) {
		case TOOL_SELECT:
			res = rectSelectRelease( button, key );
			break;
		case TOOL_EYEDROPPER:
			res = eyeDropperRelease( button, key );
			break;
		case TOOL_PEN:
			res = penRelease( button, key );
			break;
		case TOOL_BRUSH:
			res = brushRelease( button, key );
			break;
		case TOOL_CHANGECOLOR:
			res = chgColorRelease( button, key );
			break;
		case TOOL_LINE:
			res = lineRelease( button, key, mods );
			break;
		case TOOL_RECT:
			res = rectRelease( button, key, mods );
			break;
		default:
			break;
	}
	return res;
}

bool BitmapCanvasEditor::on_draw( const Cairo::RefPtr<Cairo::Context>& cr )
{
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
	}

	// draw normal stuff
	CanvasView::on_draw(cr);
	
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

void BitmapCanvasEditor::clipPixelArea( int& x, int& y, int w, int h )
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

		if( w > 0 && x+w > tx+gw ) x = tx+gw - w;
		if( h > 0 && y+h > ty+gh ) y = ty+gh - h;
			
	} else {
		if( x < 0 )
			x = 0;
		else if( x >= canvas().width() )
			x = canvas().width() - 1;

		if( y < 0 )
			y = 0;
		else if( y >= canvas().height() )
			y = canvas().height() - 1;
			
		if( w > 0 && x+w > canvas().width() ) x = canvas().width() - w;
		if( h > 0 && y+h > canvas().height() ) y = canvas().height() - h;
	}
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
	changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_selectrect") );
	// init floating mode if a selection exists
	if( m_pSelectionBrush ) {
		m_ToolMode = SELECT_MODE_MOVEFLOATING;
		m_rSelectionMarker->setVisible();
		rectSelectModeChanged();
		rectSelectUpdate(0);
	} else
		m_ToolMode = SELECT_MODE_NONE;
}

/**
 * Start dragging a rectangle or rectangle modification. When inside
 * a selection, a selection can be made floating.
 */
bool BitmapCanvasEditor::rectSelectActivate( guint button, guint key, guint mods )
{
	int f;
	if( m_ToolMode == SELECT_MODE_MOVE && isAccel( ACC_SELECT_FLOAT, button, key, mods ) ) {
		// change to floating selection
		// create new brush from selection
		if( m_pSelectionBrush ) delete m_pSelectionBrush;
		m_pSelectionBrush = createBrushFromSelection();
		if( !m_ToolSelectPanel.solidMode() )
			m_pSelectionBrush->setTransparentColor(m_BGColor);
		// attach to marker
		m_rSelectionMarker->setBrush( *m_pSelectionBrush, canvas().palette() );
		m_rSelectionMarker->setLocation( m_rSelectionRect->x() + m_pSelectionBrush->offsetX(),
		                                 m_rSelectionRect->y() + m_pSelectionBrush->offsetY() );
		m_rSelectionMarker->setVisible();
		setShapeLine(m_rSelectionMarker, true);
		m_rSelectionMarker->setLineDashSize(2);
		// remove background if needed
		if( !m_ToolSelectPanel.copyMode() ) {
			// draw rectangle
			createUndo( _("Clear selection background"), ResourceManager::get().getIcon("canvasedit_tool_select") );
			m_Pen.setColor( m_BGColor );
			canvas().drawRect( m_rSelectionRect->x(), 
			                   m_rSelectionRect->y(), 
			                   m_rSelectionRect->x() + m_pSelectionBrush->width()-1, 
			                   m_rSelectionRect->y() + m_pSelectionBrush->height()-1,
			                   m_Pen, m_Pen );
			canvas().finishAction();
		}
		m_ToolSelectPanel.setHasFloating();
		// remove selection
		m_rSelectionRect->setVisible(false);
		m_rSelectionRect->setSize(0,0);
		
	} else if( (f = isAccel( {ACC_SELECT, ACC_SELECT_TILE}, button, key, mods)) >= 0 ) {
		
		m_PrimaryTool = f == ACC_SELECT;
		
		if( m_ToolMode == SELECT_MODE_APPLYFLOATING ) {

			// remove marker
			m_rSelectionMarker->setVisible(false);
			m_rSelectionMarker->unsetBrush();
			// apply brush at current location
			createUndo( _("Apply selection"), ResourceManager::get().getIcon("canvasedit_tool_select") );
			canvas().draw( m_rSelectionMarker->x(), m_rSelectionMarker->y(), *m_pSelectionBrush );
			canvas().finishAction();
			delete m_pSelectionBrush;
			m_pSelectionBrush = 0;
			rectSelectUpdate( mods );

			m_ToolSelectPanel.setHasFloating(false);
			
		} else if(m_MouseInArea || m_ToolMode == SELECT_MODE_MOVEFLOATING) {
			// start drag
			m_DragTool = true;
			m_DragStartX = m_PixX;
			m_DragStartY = m_PixY;

			// turn off slow drawing features
			setFastUpdate();
			queue_draw();
			
			if( m_TileSelect && updateTileCoords() ) {
				// select initial tile
				if( m_InFullTile ) 
					rectSelectUpdate( mods );
			}
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
	if( m_DragTool ) {
		// check for flip function
		m_TileSelect = !updateAccel(ACC_SELECT, ACC_SELECT_TILE, m_PrimaryTool, mods, ACC_MOD_SELECT_TILE);

		// select whole tiles?
		bool modTile = m_ZoomMode ? false : m_TileSelect;

		if( m_ToolMode == SELECT_MODE_NONE ) {
			int x1, x2, y1, y2;
			if( modTile && updateTileCoords() ) {
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
			m_rSelectionRect->setLocation(x1, y1);
			m_rSelectionRect->setSize( 1+x2-x1, 1+y2-y1 );
			m_rSelectionRect->setVisible( x1 != x2 || y1 != y2 );
			setShapeLine( m_rSelectionRect, true );
			queue_draw();
			return true;
		} else if( m_ToolMode == SELECT_MODE_MOVEFLOATING ) {
			// selection modification, 
			int sx = m_PixX-m_DragOffsetX, sw = m_pSelectionBrush->width();
			int sy = m_PixY-m_DragOffsetY, sh = m_pSelectionBrush->height();
			// move floating selection
			if( sw%canvas().tileGridWidth() || sh%canvas().tileGridHeight() ) modTile = false;
			if( modTile ) {
				clipPixelArea(sx, sy, sw, sh);
				tileCoords( sx, sy, sx, sy );
				sx = sx * canvas().tileGridWidth() + canvas().tileGridHorOffset();
				sy = sy * canvas().tileGridHeight() + canvas().tileGridVerOffset();
			}
			m_rSelectionMarker->setLocation(sx + m_pSelectionBrush->offsetX(), sy + m_pSelectionBrush->offsetY());
			setShapeLine( m_rSelectionRect, true );
			return true;
		} else {
			// selection modification, 
			int sx = m_rSelectionRect->x(), sw = m_rSelectionRect->width();
			int sy = m_rSelectionRect->y(), sh = m_rSelectionRect->height();
			// clip target
			int tx = m_PixX, ty = m_PixY;
			clipPixelArea(tx, ty);
			// move
			if( m_ToolMode == SELECT_MODE_MOVE ) {
				if( sw%canvas().tileGridWidth() || sh%canvas().tileGridHeight() ) modTile = false;
				sx = tx-m_DragOffsetX;
				sy = ty-m_DragOffsetY;
				clipPixelArea(sx, sy, sw, sh);
				if( modTile ) {
					tileCoords( sx, sy, sx, sy );
					sx = sx * canvas().tileGridWidth() + canvas().tileGridHorOffset();
					sy = sy * canvas().tileGridHeight() + canvas().tileGridVerOffset();
				}
				m_rSelectionRect->setLocation(sx, sy);
				setShapeLine( m_rSelectionRect, true );
				return true;
			}
			// modify up
			if( m_ToolMode == SELECT_MODE_SCALEUP || m_ToolMode == SELECT_MODE_SCALEUPLEFT || m_ToolMode == SELECT_MODE_SCALEUPRIGHT ) {
				if( modTile ) {
					// snap to tile
					if( ty > canvas().tileGridVerOffset() )
						ty -= (ty - canvas().tileGridVerOffset()) % canvas().tileGridHeight();
					else
						ty = canvas().tileGridVerOffset();
				}
				if( ty > sy+sh ) {
					sy += sh-1;
					sh = 1;
				} else {
					sh += sy-ty;
					sy = ty;
				}
			}
			// modify down
			if( m_ToolMode == SELECT_MODE_SCALEDOWN || m_ToolMode == SELECT_MODE_SCALEDOWNLEFT || m_ToolMode == SELECT_MODE_SCALEDOWNRIGHT ) {
				if( modTile ) {
					// snap to tile
					ty += canvas().tileGridHeight()-1 - (ty - canvas().tileGridVerOffset()) % canvas().tileGridHeight();
					if( ty >= canvas().height() )
						ty -= canvas().tileGridHeight();
				}
				if( ty < sy ) {
					sh = 1;
				} else {
					sh = ty-sy+1;
				}
			}
			// modify left
			if( m_ToolMode == SELECT_MODE_SCALELEFT || m_ToolMode == SELECT_MODE_SCALEUPLEFT || m_ToolMode == SELECT_MODE_SCALEDOWNLEFT ) {
				if( modTile ) {
					// snap to tile
					if( tx > canvas().tileGridHorOffset() )
						tx -= (tx - canvas().tileGridHorOffset()) % canvas().tileGridWidth();
					else
						tx = canvas().tileGridHorOffset();
				}
				if( tx > sx+sw ) {
					sx += sw-1;
					sw = 1;
				} else {
					sw += sx-tx;
					sx = tx;
				}
			}
			// modify right
			if( m_ToolMode == SELECT_MODE_SCALERIGHT || m_ToolMode == SELECT_MODE_SCALEUPRIGHT || m_ToolMode == SELECT_MODE_SCALEDOWNRIGHT ) {
				if( modTile ) {
					// snap to tile
					tx += canvas().tileGridWidth()-1 - (tx - canvas().tileGridHorOffset()) % canvas().tileGridWidth();
					if( tx >= canvas().width() )
						tx -= canvas().tileGridWidth();
				}
				if( tx < sx ) {
					sw = 1;
				} else {
					sw = tx-sx+1;
				}
			}
			// update rect
			m_rSelectionRect->setLocation(sx, sy);
			m_rSelectionRect->setSize(sw, sh);
			setShapeLine( m_rSelectionRect, true );
			return true;
		}			
	} else if( m_rSelectionRect->isVisible() ) {
		// check for rectangle modifiers
		// first calc selection in rectangle coords
		int x1 = m_rSelectionRect->x() * hscale() - dx();
		int y1 = m_rSelectionRect->y() * vscale() - dy();
		int x2 = x1-1 + m_rSelectionRect->width() * hscale();
		int y2 = y1-1 + m_rSelectionRect->height() * vscale();
		if( m_MouseX <= x1-6 || m_MouseX >= x2+6 || m_MouseY <= y1-6 || m_MouseY >= y2+6 ) {
			// outside selection
			changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_selectrect") );
			m_ToolMode = SELECT_MODE_NONE;
		} else if( m_MouseY < y1+3 ) {
			// top edge
			if( m_MouseX < x1+3 ) {
				m_ToolMode = SELECT_MODE_SCALEUPLEFT;
				changeCursor( Gdk::Cursor::create(Gdk::TOP_LEFT_CORNER) );
			} else if( m_MouseX > x2-3 ) {
				m_ToolMode = SELECT_MODE_SCALEUPRIGHT;
				changeCursor( Gdk::Cursor::create(Gdk::TOP_RIGHT_CORNER) );
			} else {
				m_ToolMode = SELECT_MODE_SCALEUP;
				changeCursor( Gdk::Cursor::create(Gdk::TOP_SIDE) );
			}
		} else if( m_MouseY > y2-3 ) {
			// bottom edge
			if( m_MouseX < x1+3 ) {
				m_ToolMode = SELECT_MODE_SCALEDOWNLEFT;
				changeCursor( Gdk::Cursor::create(Gdk::BOTTOM_LEFT_CORNER) );
			} else if( m_MouseX > x2-3 ) {
				m_ToolMode = SELECT_MODE_SCALEDOWNRIGHT;
				changeCursor( Gdk::Cursor::create(Gdk::BOTTOM_RIGHT_CORNER) );
			} else {
				m_ToolMode = SELECT_MODE_SCALEDOWN;
				changeCursor( Gdk::Cursor::create(Gdk::BOTTOM_SIDE) );
			}
		} else if( m_MouseX < x1+3 ) {
			// left edge
			m_ToolMode = SELECT_MODE_SCALELEFT;
			changeCursor( Gdk::Cursor::create(Gdk::LEFT_SIDE) );
		} else if( m_MouseX > x2-3 ) {
			// right edge
			m_ToolMode = SELECT_MODE_SCALERIGHT;
			changeCursor( Gdk::Cursor::create(Gdk::RIGHT_SIDE) );
		} else {
			// inside
			m_ToolMode = SELECT_MODE_MOVE;
			changeCursor( Gdk::Cursor::create(Gdk::FLEUR) );
			m_DragOffsetX = (m_MouseX - x1) / hscale();
			m_DragOffsetY = (m_MouseY - y1) / vscale();
		}
	} else if( m_rSelectionMarker->isVisible() ) {
		int x1 = (m_rSelectionMarker->x() - m_pSelectionBrush->offsetX()) * hscale() - dx();
		int y1 = (m_rSelectionMarker->y() - m_pSelectionBrush->offsetY()) * vscale() - dy();
		int x2 = x1-1 + m_pSelectionBrush->width() * hscale();
		int y2 = y1-1 + m_pSelectionBrush->height() * vscale();
		if( m_MouseX >= x1 && m_MouseX <= x2 && m_MouseY >= y1 && m_MouseY <= y2 ) {
			// inside floating selection
			m_ToolMode = SELECT_MODE_MOVEFLOATING;
			changeCursor( Gdk::Cursor::create(Gdk::FLEUR) );
			m_DragOffsetX = (m_MouseX - x1) / hscale();
			m_DragOffsetY = (m_MouseY - y1) / vscale();
		} else {
			m_ToolMode = SELECT_MODE_APPLYFLOATING;
			changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_selectrectlock") );
		}
	} else {
		// reset mode if selection was removed
		changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_selectrect") );
		m_ToolMode = SELECT_MODE_NONE;
	}
	
	return false;
}

/**
 * End dragging of selection rectangle
 */
bool BitmapCanvasEditor::rectSelectRelease( guint button, guint key )
{
	if( isAccel( {ACC_SELECT, ACC_SELECT_TILE}, button, key) >= 0 ) {
		setShapeLine( m_rSelectionRect, false );
		// restore slow drawing features
		setFastUpdate(false);
		m_DragTool = false;
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
	m_rSelectionRect->setVisible(false);
	m_rSelectionMarker->setVisible(false);
	queue_draw();
}

/**
 * Handle changes in mode and background color
 */
void BitmapCanvasEditor::rectSelectModeChanged()
{
	if( m_rSelectionMarker->isVisible() ) {
		if( m_ToolSelectPanel.solidMode() )
			m_pSelectionBrush->setTransparentColor(-1);
		else
			m_pSelectionBrush->setTransparentColor(m_BGColor);
		// force update screen area
		m_rBrushMarker->redraw();
	}
}

void BitmapCanvasEditor::rectSelectToBrush()
{
	if( m_pTempBrush ) delete m_pTempBrush;
	// try conversion to shape
	m_pTempBrush = m_pSelectionBrush->convertToShape();
	// assign to current draw brush
	if( m_pTempBrush ) {
		m_pTempBrush->setColor(m_FGColor);
		m_pBrush = m_pTempBrush;
	} else
		m_pBrush = m_pSelectionBrush;
	// update brush marker
	m_rBrushMarker->setBrush( *m_pBrush, canvas().palette() );
	// switch to brush tool
	m_SignalChangeTool.emit( TOOL_BRUSH );
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
	changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_eyedropper_fg") );
	m_PickFG = true;
}

bool BitmapCanvasEditor::eyeDropperActivate( guint button, guint key, guint mods )
{
	int f = isAccel( {ACC_COLORPICK_FG, ACC_COLORPICK_BG}, button, key, mods);

	if( f>=0 && m_MouseInArea && !m_DragTool ) {
		m_DragTool = true;
		m_PrimaryTool = f == ACC_COLORPICK_FG;
		eyeDropperUpdate(mods);
		return true;
	}
	return false;
}

bool BitmapCanvasEditor::eyeDropperUpdate( guint mods )
{
	// pick foreground or background?
	if( m_DragTool ) {
		// switch is necessary
		m_PickFG = updateAccel( ACC_COLORPICK_FG, ACC_COLORPICK_BG, m_PrimaryTool, mods );
	} else {
		// check if mods determine color
		m_PickFG = guessAccel( ACC_COLORPICK_FG, ACC_COLORPICK_BG, mods );
	}

	// adjust cursor
	if( m_PickFG ) {
		changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_eyedropper_fg") );
	} else {
		changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_eyedropper_bg") );
	}
	// pick color if down
	if( m_DragTool ) {
		if( m_PixX >= 0 && m_PixY >= 0 && m_PixX < canvas().width() && m_PixY < canvas().height() ) {
			if( m_PickFG )
				m_SignalChangeFGColor.emit( canvas().data( m_PixX, m_PixY ) );
			else
				m_SignalChangeBGColor.emit( canvas().data( m_PixX, m_PixY ) );
		}
		return true;
	}
	return false;
}

bool BitmapCanvasEditor::eyeDropperRelease( guint button, guint key )
{
	if( ( m_PickFG && isAccel( ACC_COLORPICK_FG, button, key )) ||
		(!m_PickFG && isAccel( ACC_COLORPICK_BG, button, key )) )
	{
		m_DragTool = false;
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
	changeCursor( Gdk::Cursor::create(Gdk::PENCIL) );
}

/**
 * Start drawing, intialize pen color and undo action
 */
bool BitmapCanvasEditor::penActivate( guint button, guint key, guint mods )
{
	int f = isAccel( {ACC_DRAW_FG, ACC_DRAW_BG}, button, key, mods);
	if( f>=0 && m_MouseInArea ) {

		// initial draw, reset color
		m_PenColor = -1;
		m_PrimaryTool = f == ACC_DRAW_FG;
		// start action
		createUndo( _("Draw pencil"), ResourceManager::get().getIcon("canvasedit_tool_draw") );
		m_DragTool = true;

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
	if( m_DragTool ) {
		// flip color
		bool fg = updateAccel( ACC_DRAW_FG, ACC_DRAW_BG, m_PrimaryTool, mods, ACC_MOD_DRAW_COL );
		// color changed?
		if( fg ) {
			if( m_PenColor != m_FGColor ) {
				m_PenColor = m_FGColor;
				m_Pen.setColor( m_FGColor );
			}				
		} else {
			if( m_PenColor != m_BGColor ) {
				m_PenColor = m_BGColor;
				m_Pen.setColor( m_BGColor );
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
bool BitmapCanvasEditor::penRelease( guint button, guint key )
{
	if( m_DragTool && isAccel( {ACC_DRAW_FG, ACC_DRAW_BG}, button, key) >= 0 ) {
		canvas().finishAction();
		m_DragTool = false;
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
	changeCursor( Gdk::Cursor::create(Gdk::TCROSS) );
	// default brush
	if( !m_pBrush )
		m_pBrush = m_Brushes[0];
	// show marker
	m_rBrushMarker->setLocation( m_PixX, m_PixY );
	m_rBrushMarker->setVisible();
}

/**
 * Start drawing, intialize brush color and undo action
 */
bool BitmapCanvasEditor::brushActivate( guint button, guint key, guint mods )
{
	int f = isAccel( {ACC_DRAW_FG, ACC_DRAW_BG}, button, key, mods);
	if( f>=0 && m_MouseInArea ) {

		// initial draw, reset color
		m_PenColor = -1;
		m_PrimaryTool = f == ACC_DRAW_FG;
		// start action
		createUndo( _("Draw brush"), ResourceManager::get().getIcon("canvasedit_tool_brush") );
		m_DragTool = true;
		
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
	// determine brush color
	bool fg = true;
	if( m_DragTool ) {
		// switch is necessary
		fg = updateAccel( ACC_DRAW_FG, ACC_DRAW_BG, m_PrimaryTool, mods, ACC_MOD_DRAW_COL );
	} else {
		// check if mods determine color
		fg = guessAccel( ACC_DRAW_FG, ACC_DRAW_BG, mods, ACC_MOD_DRAW_COL );
	}

	// always update brush color
	if( fg ) {
		if( m_PenColor != m_FGColor ) {
			m_PenColor = m_FGColor;
			m_pBrush->setColor( m_FGColor );
		}				
	} else {
		if( m_PenColor != m_BGColor ) {
			m_PenColor = m_BGColor;
			m_pBrush->setColor( m_BGColor );
		}				
	}
	
	if( m_DragTool ) {
		// draw
		canvas().draw( m_PixX, m_PixY, *m_pBrush );
		m_rBrushMarker->setLocation( m_PixX, m_PixY );
		return true;
	} else {
		m_rBrushMarker->setLocation( m_PixX, m_PixY );
	}
	return false;
}

/**
 * Release brush activation and finalize undo action
 */
bool BitmapCanvasEditor::brushRelease( guint button, guint key )
{
	if( m_DragTool && isAccel( {ACC_DRAW_FG, ACC_DRAW_BG}, button, key) >= 0 ) {
		canvas().finishAction();
		m_DragTool = false;
		return true;
	}
	return false;
}

/**
 * Disable brush
 */
void BitmapCanvasEditor::brushClean()
{
	if( m_pBrush && m_rBrushMarker->isVisible() ) {
		m_rBrushMarker->setVisible(false);
	}
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
	changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_changecolor") );
	// default brush
	if( !m_pBrush )
		m_pBrush = m_Brushes[0];
	// show marker
	m_rBrushMarker->setLocation( m_PixX, m_PixY );
	m_rBrushMarker->setVisible();
}

/**
 * Start drawing, intialize brush color and undo action
 */
bool BitmapCanvasEditor::chgColorActivate( guint button, guint key, guint mods )
{
	int f = isAccel( {ACC_DRAW_FG, ACC_DRAW_BG}, button, key, mods);
	if( f>=0 && m_MouseInArea ) {

		// initial draw, reset color
		m_PenColor = -1;
		m_PrimaryTool = f == ACC_DRAW_FG;
		// start action
		createUndo( _("Change color"), ResourceManager::get().getIcon("canvasedit_tool_changecolor") );
		m_DragTool = true;
		
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
	// determine brush color
	bool fg = true;
	if( m_DragTool ) {
		// switch is necessary
		fg = updateAccel( ACC_DRAW_FG, ACC_DRAW_BG, m_PrimaryTool, mods, ACC_MOD_DRAW_COL );
	} else {
		// check if mods determine color
		fg = guessAccel( ACC_DRAW_FG, ACC_DRAW_BG, mods, ACC_MOD_DRAW_COL );
	}

	// always update brush color
	if( fg ) {
		if( m_PenColor != m_FGColor ) {
			m_PenColor = m_FGColor;
			m_pBrush->setColor( m_FGColor );
		}				
	} else {
		if( m_PenColor != m_BGColor ) {
			m_PenColor = m_BGColor;
			m_pBrush->setColor( m_BGColor );
		}				
	}

	if( m_DragTool ) {
		// draw
		canvas().changeColorDraw( m_PixX, m_PixY, *m_pBrush, m_PenColor==m_FGColor?m_BGColor:m_FGColor );
		m_rBrushMarker->setLocation( m_PixX, m_PixY );
		return true;
	} else {
		m_rBrushMarker->setLocation( m_PixX, m_PixY );
	}

	return false;
}

/**
 * Release brush activation and finalize undo action
 */
bool BitmapCanvasEditor::chgColorRelease( guint button, guint key )
{
	if( m_DragTool && isAccel( {ACC_DRAW_FG, ACC_DRAW_BG}, button, key) >= 0 ) {
		canvas().finishAction();
		m_DragTool = false;
		return true;
	}
	return false;
}

/**
 * Disable brush
 */
void BitmapCanvasEditor::chgColorClean()
{
	if( m_pBrush && m_rBrushMarker->isVisible() ) {
		m_rBrushMarker->setVisible(false);
	}
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
	changeCursor( Gdk::Cursor::create(Gdk::CROSSHAIR) );
}

/**
 * Start drawing, intialize pen color and undo action
 */
bool BitmapCanvasEditor::lineActivate( guint button, guint key, guint mods )
{
	if( isAccel( {ACC_LINE_FG, ACC_LINE_BG}, button, key, mods ) >= 0 ) {
		// only start if primary button not previously pressed
		if( !m_DragTool ) {
			// only mark start
			m_DragTool = true;
			m_DragStartX = m_PixX;
			m_DragStartY = m_PixY;
			// create marker object
			m_rToolMarker = LineShape::create();
			m_rToolMarker->setLocation( m_PixX, m_PixY );
			m_rToolMarker->setSize( 0, 0 );
			m_rToolMarker->setVisible(false);
			add( TOOLMARKER_ID, m_rToolMarker );
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
	if( m_DragTool ) {
		// calc end coord
		m_DragEndX = m_PixX; m_DragEndY = m_PixY;
		int dx = m_DragEndX - m_DragStartX, dy = m_DragEndY - m_DragStartY;
		if( dx && dy ) {
			int sx = dx>0?1:-1, sy = dy>0?1:-1;
			if( isAccelMod( ACC_MOD_LINE_ANGLE, mods ) ) {
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
		m_rToolMarker->setSize( m_DragEndX-m_DragStartX, m_DragEndY-m_DragStartY );
		m_rToolMarker->setVisible();
	}
	// allow other actions
	return false;
}

/**
 * Release and draw line or do nothing if still on start pixel and draw
 * on second click.
 */
bool BitmapCanvasEditor::lineRelease( guint button, guint key, guint mods )
{
	int f = isAccel( {ACC_LINE_FG, ACC_LINE_BG}, button, key, mods);
	if( f >= 0 ) {
		// if on same pixel, allow release
		if( m_PixX == m_DragStartX && m_PixY == m_DragStartY ) {
			return true;
		}
		// set pen color
		if( f == ACC_LINE_BG )
			m_Pen.setColor( m_BGColor );
		else
			m_Pen.setColor( m_FGColor );
		// draw line
		createUndo( _("Line"), ResourceManager::get().getIcon("canvasedit_tool_drawline") );
		canvas().drawLine( m_DragStartX, m_DragStartY, m_DragEndX, m_DragEndY, m_Pen );
		canvas().finishAction();
		m_DragTool = false;
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
	if( m_rToolMarker ) {
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
	changeCursor( Gdk::Cursor::create(Gdk::CROSSHAIR) );
}

/**
 * Start drawing, intialize pen color and undo action
 */
bool BitmapCanvasEditor::rectActivate( guint button, guint key, guint mods )
{
	if( isAccel( {ACC_LINE_FG, ACC_LINE_BG}, button, key, mods ) >= 0 ) {
		// only start if primary button not previously pressed
		if( !m_DragTool ) {
			// only mark start
			m_DragTool = true;
			m_DragStartX = m_PixX;
			m_DragStartY = m_PixY;
			// create marker object
			m_rToolMarker = RectangleShape::create();
			m_rToolMarker->setLocation( m_PixX, m_PixY );
			m_rToolMarker->setSize( 0, 0 );
			m_rToolMarker->setVisible(false);
			Cairo::RefPtr<RectangleShape>::cast_dynamic(m_rToolMarker)->setLineDashSize(3);
			add( TOOLMARKER_ID, m_rToolMarker );
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
	if( m_DragTool ) {
		// calc end coord
		m_DragEndX = m_PixX; m_DragEndY = m_PixY;
		int dx = m_DragEndX - m_DragStartX, dy = m_DragEndY - m_DragStartY;
		if( dx && dy ) {
			if( isAccelMod( ACC_MOD_LINE_ANGLE, mods ) ) {
				// modify to square
				if( abs(dx) > abs(dy) ) {
					dy = std::copysign(dx, dy);
					m_DragEndY = m_DragStartY + dy;
				} else {
					dx = std::copysign(dy, dx);
					m_DragEndX = m_DragStartX + dx;
				}
			}
			int sx = m_DragStartX + (dx>0 ? 0 : dx);
			int sy = m_DragStartY + (dy>0 ? 0 : dy);
			dx = abs(dx);
			dy = abs(dy);
			m_rToolMarker->setLocation( sx, sy );
			m_rToolMarker->setSize( dx+1, dy+1 );
			Cairo::RefPtr<RectangleShape>::cast_dynamic(m_rToolMarker)->setType( m_ToolRectPanel.filledRectangle() ? RectangleShape::LINE_OUTSIDE : RectangleShape::NORMAL);
		}
		m_rToolMarker->setVisible( dx && dy );
	}
	// allow other actions
	return false;
}

/**
 * Release and draw rectangle or do nothing if still on start pixel and 
 * draw on second click.
 */
bool BitmapCanvasEditor::rectRelease( guint button, guint key, guint mods )
{
	int f = isAccel( {ACC_LINE_FG, ACC_LINE_BG}, button, key, mods);
	if( f >= 0 ) {
		// if on same pixel, allow release
		if( m_PixX == m_DragStartX && m_PixY == m_DragStartY ) {
			return true;
		}
		// set pen colors
		Pen fillPen;
		if( f == ACC_LINE_BG ) {
			m_Pen.setColor( m_BGColor );
			fillPen.setColor( m_FGColor );
		} else {
			m_Pen.setColor( m_FGColor );
			fillPen.setColor( m_BGColor );
		}
		if( !m_ToolRectPanel.filledRectangle() ) fillPen.setColor(-1);
		// draw rectangle
		createUndo( _("Rectangle"), ResourceManager::get().getIcon("canvasedit_tool_drawrect") );
		canvas().drawRect( m_DragStartX, m_DragStartY, m_DragEndX, m_DragEndY, m_Pen, fillPen );
		canvas().finishAction();
		m_DragTool = false;
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
	if( m_rToolMarker ) {
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
	changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_fill") );
}

/**
 * Fill at cursor, color determined by modifier
 */
bool BitmapCanvasEditor::fillActivate( guint button, guint key, guint mods )
{
	int f = isAccel( {ACC_FILL_FG, ACC_FILL_BG}, button, key, mods);
	if( f >= 0 ) {
		// only start if primary button not previously pressed
		if( m_MouseInArea ) {
			if( f == ACC_FILL_BG )
				m_Pen.setColor( m_BGColor );
			else
				m_Pen.setColor( m_FGColor );
			createUndo( _("Bucket fill"), ResourceManager::get().getIcon("canvasedit_tool_fill") );
			canvas().bucketFill( m_PixX, m_PixY, m_Pen );
			canvas().finishAction();
		}
		return true;
	}
	return false;
}


/*
 *--------------
 * Flip
 *--------------
 */ 

void BitmapCanvasEditor::flipInit()
{
	changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_horflip") );
	if( m_pSelectionBrush )
		m_rSelectionMarker->setVisible();
	else if( m_rSelectionRect->width() )
		m_rSelectionRect->setVisible();
	m_ToolMode = FLIP_MODE_NONE;
}

bool BitmapCanvasEditor::flipActivate( guint button, guint key, guint mods )
{
	int f = isAccel( {ACC_FLIP_HOR, ACC_FLIP_VER}, button, key, mods);
	if( m_ToolMode != FLIP_MODE_NONE && f >= 0 ) {
		bool sec = f == ACC_FLIP_VER;
		if( m_ToolMode == FLIP_MODE_FLOAT ) {
			// flip floating selection
			m_pSelectionBrush->flip( sec );
			m_rSelectionMarker->setBrush( *m_pSelectionBrush, canvas().palette() );
		} else if( m_ToolMode == FLIP_MODE_SELECT ) {
			// flip static selection	
			createUndo( _("Flip selection"), ResourceManager::get().getIcon("canvasedit_tool_flip") );
			int x = m_rSelectionRect->x(), y = m_rSelectionRect->y();
			int w = m_rSelectionRect->width(), h = m_rSelectionRect->height();
			canvas().flip(x, y, w, h, sec );
			canvas().finishAction();
			m_rSelectionMarker->redraw();
		} else {
			// flip whole canvas
			createUndo( _("Flip"), ResourceManager::get().getIcon("canvasedit_tool_flip") );
			canvas().flip( 0, 0, canvas().width(), canvas().height(), sec );
			canvas().finishAction();
			queue_draw();
		}
		return true;
	}
	return false;
}

bool BitmapCanvasEditor::flipUpdate( guint mods )
{
	// check if inside selection
	int x1, x2, y1, y2;
	if( m_rSelectionMarker->isVisible() ) {
		x1 = (m_rSelectionMarker->x() - m_pSelectionBrush->offsetX()) * hscale() - dx();
		y1 = (m_rSelectionMarker->y() - m_pSelectionBrush->offsetY()) * vscale() - dy();
		x2 = x1-1 + m_pSelectionBrush->width() * hscale();
		y2 = y1-1 + m_pSelectionBrush->height() * vscale();
		m_ToolMode = FLIP_MODE_FLOAT;
	} else if( m_rSelectionRect->isVisible() ) {
		x1 = m_rSelectionRect->x() * hscale() - dx();
		y1 = m_rSelectionRect->y() * vscale() - dy();
		x2 = x1-1 + m_rSelectionRect->width() * hscale();
		y2 = y1-1 + m_rSelectionRect->height() * vscale();
		m_ToolMode = FLIP_MODE_SELECT;
	}
	bool in_selection = m_MouseX >= x1 && m_MouseX <= x2 &&
	                    m_MouseY >= y1 && m_MouseY <= y2;
	if( !in_selection) m_ToolMode = FLIP_MODE_ALL;

	if( guessAccel( ACC_FLIP_HOR, ACC_FLIP_VER, mods ) ) {
		if( in_selection )
			changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_horselflip") );
		else
			changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_horflip") );
	} else {
		if( in_selection )
			changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_verselflip") );
		else
			changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_verflip") );
	}
	return false;
}

void BitmapCanvasEditor::flipClean()
{
	m_rSelectionMarker->setVisible(false);
	m_rSelectionRect->setVisible(false);
}



/*
 *--------------
 * Rotate
 *--------------
 */ 

void BitmapCanvasEditor::rotateInit()
{
	changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_cwrotate") );
	if( m_pSelectionBrush )
		m_rSelectionMarker->setVisible();
	else if( m_rSelectionRect->width() ) {
		// only square non-floating selections
		if( m_rSelectionRect->width() == m_rSelectionRect->height() )
			m_rSelectionRect->setVisible();
	}
	m_ToolMode = FLIP_MODE_NONE;
}	

bool BitmapCanvasEditor::rotateActivate( guint button, guint key, guint mods )
{
	int f = isAccel( {ACC_ROTATE_CW, ACC_ROTATE_CCW}, button, key, mods);
	if( f >= 0 ) {
		bool sec = f == ACC_ROTATE_CCW;
		if( m_ToolMode == FLIP_MODE_FLOAT ) {
			// flip floating selection
			m_pSelectionBrush->rotate( sec );
			m_rSelectionMarker->setBrush( *m_pSelectionBrush, canvas().palette() );
		} else if( m_ToolMode == FLIP_MODE_SELECT ) {
			// rotate static square selection	
			createUndo( _("Rotate selection"), ResourceManager::get().getIcon("canvasedit_tool_rotate") );
			int x = m_rSelectionRect->x(), y = m_rSelectionRect->y();
			int w = m_rSelectionRect->width();
			canvas().rotate(x, y, w, sec );
			canvas().finishAction();
			m_rSelectionRect->redraw();
		}
	}
	return false;
}

bool BitmapCanvasEditor::rotateUpdate( guint mods )
{
	// check if inside selection
	int x1, x2, y1, y2;
	if( m_rSelectionMarker->isVisible() ) {
		x1 = (m_rSelectionMarker->x() - m_pSelectionBrush->offsetX()) * hscale() - dx();
		y1 = (m_rSelectionMarker->y() - m_pSelectionBrush->offsetY()) * vscale() - dy();
		x2 = x1-1 + m_pSelectionBrush->width() * hscale();
		y2 = y1-1 + m_pSelectionBrush->height() * vscale();
		m_ToolMode = FLIP_MODE_FLOAT;
	} else if( m_rSelectionRect->isVisible() ) {
		x1 = m_rSelectionRect->x() * hscale() - dx();
		y1 = m_rSelectionRect->y() * vscale() - dy();
		x2 = x1-1 + m_rSelectionRect->width() * hscale();
		y2 = y1-1 + m_rSelectionRect->height() * vscale();
		m_ToolMode = FLIP_MODE_SELECT;
	}
	bool in_selection = m_MouseX >= x1 && m_MouseX <= x2 &&
	                    m_MouseY >= y1 && m_MouseY <= y2;
	
	if( in_selection ) {
		// set appropriate cursor
		if( guessAccel( ACC_ROTATE_CW, ACC_ROTATE_CCW, mods ) )
			changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_cwrotate") );
		else
			changeCursor( ResourceManager::get().getCursor(get_window(), "canvasedit_ccwrotate") );
	} else {
		changeCursor( Gdk::Cursor::create(Gdk::ARROW) );
		m_ToolMode = FLIP_MODE_NONE;
	}
	
	return false;
}

void BitmapCanvasEditor::rotateClean()
{
	m_rSelectionMarker->setVisible(false);
	m_rSelectionRect->setVisible(false);
}




Brush *BitmapCanvasEditor::createBrushFromSelection()
{
	int sx = m_rSelectionRect->x();
	int sy = m_rSelectionRect->y();
	int sw = m_rSelectionRect->width();
	int sh = m_rSelectionRect->height();
	
	if( sw == 0 || sh == 0 ) return 0;
	
	return canvas().createBrushFromRect( sx, sy, sw, sh, -1 );
}

void BitmapCanvasEditor::removeToolMarker()
{
	if( m_rToolMarker ) {
		remove(TOOLMARKER_ID);
		m_rToolMarker.clear();
	}
}



} // namespace Polka
