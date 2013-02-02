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

#ifndef _POLKA_BITMAPSCREENIMPORT_H_
#define _POLKA_BITMAPSCREENIMPORT_H_

#include "Importer.h"
#include "ImportManager.h"
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/comboboxtext.h>
#include <glibmm/i18n.h>
#include <fstream>

namespace Polka {

class BitmapScreenImporter : public Importer
{
public:
	BitmapScreenImporter();
	~BitmapScreenImporter();

	virtual bool initImport( const std::string& filename );
	virtual void cancelImport();

	virtual Gtk::Widget& getPreviewWidget();

	virtual bool importToProject( Project& project );

	void updateTarget( int id );

protected:

private:
	bool tryOpenPalette( std::ifstream& stream );
	bool tryOpenG9kPalette( std::ifstream& stream );
	bool tryOpenBSaved( std::ifstream& stream );
	
	void cleanUp();
	
	bool checkPalette( const char *data ) const;
	bool checkG9kPalette( const char *data ) const;
	
	enum BitmapType { NONE, SC5, SC6, SC7, SC8 };
	
	BitmapType getBitmapTypeByFileName();
	
	char *m_pData;

	std::string m_FileName;
	std::vector<BitmapType> m_PossibleTypes;
	int m_FileSize, m_StartAddress, m_EndAddress;
	int m_DetectedType;
	int m_PaletteOffset;
	bool m_PaletteG9k;
	
	class BitmapPreviewWidget : public Gtk::VBox
	{
	public:
		BitmapPreviewWidget( BitmapScreenImporter& importer );

		void reset();

		void addType( const Glib::ustring& text, bool active = false );
		void setTargetText( const Glib::ustring& text );

		int targetId() const;

	private:
		BitmapScreenImporter& m_Importer;
		Gtk::Label m_ImportLabel;
		Gtk::Label m_TypeLabel;
		Gtk::ComboBoxText m_ComboTypes;
		Gtk::Label m_TargetLabel;

		void changeTarget();
	};
	
	BitmapPreviewWidget m_Preview;

};


} // namespace Polka

#endif // _POLKA_PALETTEEDITOR_H_
