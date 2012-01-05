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
