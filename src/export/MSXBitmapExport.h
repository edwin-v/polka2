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

#ifndef _POLKA_MSXBITMAPEXPORTER_H_
#define _POLKA_MSXBITMAPEXPORTER_H_

#include "Exporter.h"
#include "ExportManager.h"
#include "gtkmm/dialog.h"

namespace Polka {

class MSXBitmapExporter : public Exporter
{
public:
	MSXBitmapExporter();
	~MSXBitmapExporter();

	virtual std::vector< Glib::RefPtr<Gtk::FileFilter> > fileFilters( const Object& obj ) const;
	virtual void setActiveFilter( Glib::RefPtr<Gtk::FileFilter> filter );

	virtual bool showExportOptions( Gtk::Window& parent );

	virtual bool exportObject( const std::string& filename );

protected:
	virtual void initObject();

private:
	std::vector< Glib::RefPtr<Gtk::FileFilter> > m_FileFilters;
	Gtk::Dialog *m_pOptionsDialog;
	
	int m_Format;
	bool m_IncludePalette;
	int m_CropLeft, m_CropRight, m_CropTop, m_CropBottom;
	int m_OffsetX, m_OffsetY;
};


} // namespace Polka

#endif // _POLKA_MSXBITMAPEXPORTER_H_
