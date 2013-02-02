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

#ifndef _POLKA_CANVASEDITOR_H_
#define _POLKA_CANVASEDITOR_H_

#include "Editor.h"
#include "ObjectManager.h"
#include "BitmapCanvasEditor.h"
#include "ToolButtonWindow.h"
#include "ColorChooser.h"
#include "GridSelector.h"
#include <gtkmm/comboboxtext.h>
#include <gtkmm/scale.h>

namespace Polka {

class Canvas;

class CanvasEditor: public Editor
{
public:
	CanvasEditor();
	~CanvasEditor();

	virtual const Glib::ustring& menuString();
	void reset();
	
	virtual void objectUpdated( bool full = true );

protected:
	virtual void on_hide();
	virtual void assignObject( Polka::Object *obj );

	virtual void on_drag_data_received( const Glib::RefPtr<Gdk::DragContext>& dc, int x, int y, const Gtk::SelectionData& data, guint info, guint time);

private:
	Canvas *m_pCanvas;

	BitmapCanvasEditor m_CanvasView;
	ToolButtonWindow m_ToolWindow;
	ColorChooser m_ColorChooser;
};



class CanvasEditorFactory : public ObjectManager::EditorFactory
{
public:
	CanvasEditorFactory();

	Editor *create() const;
};


} // namespace Polka

#endif // _POLKA_CANVASEDITOR_H_
