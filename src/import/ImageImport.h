#ifndef _POLKA_IMAGEIMPORT_H_
#define _POLKA_IMAGEIMPORT_H_

#include "Importer.h"
#include "ImportManager.h"
#include "HIGFrame.h"
#include "ColorReducer.h"
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/grid.h>
#include <gtkmm/notebook.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/spinbutton.h>
#include <fstream>

namespace Polka {

class ImageImporter : public Importer
{
public:
	ImageImporter();
	~ImageImporter();

	virtual bool initImport( const std::string& filename );
	virtual void cancelImport();

	virtual Gtk::Widget& getPreviewWidget();

	virtual bool importToProject( Project& project );

protected:

private:
	
	void cleanUp();

	std::string m_FileName;
	int m_FileSize;

	// image surface
	Cairo::RefPtr<Cairo::ImageSurface> m_refImage;
	
	// image formats
	bool tryLoadPNG();
	
	class ImagePreviewWidget : public Gtk::VBox
	{
	public:
		ImagePreviewWidget( ImageImporter& importer );

		void reset();

		void setImage( const Glib::ustring& type, int hres, int vres, const Glib::ustring& colors );

		std::string getPaletteType() const;
		ColorReducer::QuantizationMethod getQuantMethod() const;
		ColorReducer::ColorErrorMethod getErrorColorSpace() const;
		ColorReducer::DitherType getDitherType() const;
		int getOrderedDitherWidth() const;
		int getOrderedDitherHeight() const;
		int getOrderedDitherOffsetX() const;
		int getOrderedDitherOffsetY() const;

	private:
		ImageImporter& m_Importer;
		Gtk::Label m_ImportLabel, m_ImageLabel, 
		           m_PaletteLabel, m_QuantLabel, m_DitherLabel,
		           m_DitherErrorLabel, m_DitherOrderedLabel,
		           m_DitherOrderedOffsetLabel, m_ColorSpaceLabel;
		HIGFrame m_OptionsFrame;
		Gtk::ComboBoxText m_PaletteCombo, m_QuantCombo, m_DitherCombo,
		                  m_DitherErrorCombo;
		Gtk::RadioButton m_RGBRadio, m_PerceptualRadio;
		Gtk::Notebook m_DitherMethodSelect;
		Gtk::Grid m_OptionsGrid, m_ErrorDiffOptions, m_OrderedOptions;
		Gtk::SpinButton m_OrderedWidthSpin, m_OrderedHeightSpin,
		                m_OrderedOffsetXSpin, m_OrderedOffsetYSpin;

		void paletteChanged();
		void ditherGroupChanged();
		bool orderedSizeChanged( Gtk::SpinButton& sb );
	};
	
	ImagePreviewWidget m_Preview;

};


} // namespace Polka

#endif // _POLKA_IMAGEIMPORT_H_
