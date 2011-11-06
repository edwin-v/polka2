#ifndef _POLKA_TILECANVASEDITOR_H_
#define _POLKA_TILECANVASEDITOR_H_

#include "Editor.h"
#include "ObjectManager.h"
#include "TileImageView.h"
#include "TileEditGrid.h"
#include "ColorSelector.h"
#include <gtkmm/comboboxtext.h>
#include <gtkmm/scale.h>

namespace Polka {

class TileCanvas;

class TileCanvasEditor: public Editor
{
public:
	TileCanvasEditor();
	~TileCanvasEditor();

	virtual const Glib::ustring& menuString();
	void reset();
	
protected:
	virtual void assignObject( Polka::Object *obj );
	virtual void updateTreeNames();

private:
	void fillPaletteSelector();
	void changePalette();
	void tileSelectionChanged( int x, int y );
	void changeEditorZoom();
	void resizeGrid( int h, int v );
	
	TileCanvas *m_pCanvas;

	TileImageView m_CanvasView;
	TileEditGrid m_TileGrid;
	Gtk::ComboBoxText m_PaletteSelector;
	ColorSelector m_ColorSelector;
	Gtk::VScale m_EditorZoom;
	int m_GridSizeH, m_GridSizeV;
	Gtk::Button m_GrowH, m_ShrinkH, m_GrowV, m_ShrinkV;
};



class TileCanvasEditorFactory : public ObjectManager::EditorFactory
{
public:
	TileCanvasEditorFactory();

	Editor *create() const;
};


} // namespace Polka

#endif // _POLKA_TILECANVASEDITOR_H_
