#ifndef _POLKA_RAWIMPORT_H_
#define _POLKA_RAWIMPORT_H_

#include "Importer.h"
#include "ImportManager.h"
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/spinbutton.h>
#include <glibmm/i18n.h>
#include <fstream>

namespace Polka {

class RawImporter : public Importer
{
public:
	RawImporter();
	~RawImporter();

	virtual bool initImport( const std::string& filename );
	virtual void cancelImport();

	virtual Gtk::Widget& getPreviewWidget();

	virtual bool importToProject( Project& project );

	void updateTarget( int id );

protected:

private:
	
	void cleanUp();
	

	std::string m_FileName;
	int m_FileSize;
	
	class RawPreviewWidget : public Gtk::VBox
	{
	public:
		RawPreviewWidget( RawImporter& importer );

		void reset();

		void addType( const Glib::ustring& text, bool active = false );
		void estimateSize( int size );

		int targetId() const;
		int offset() const;
		int width() const;
		int height() const;

	private:
		RawImporter& m_Importer;
		Gtk::Label m_ImportLabel;
		Gtk::Label m_TypeLabel;
		Gtk::ComboBoxText m_ComboTypes;
		Gtk::Label m_OffsetLabel, m_WidthLabel, m_HeightLabel;
		Gtk::SpinButton m_OffsetSpin, m_WidthSpin, m_HeightSpin;

		void changeTarget();
	};
	
	RawPreviewWidget m_Preview;

};


} // namespace Polka

#endif // _POLKA_RAWIMPORT_H_
