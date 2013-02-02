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

#include "ProjectTreeStore.h"
#include <iostream>

namespace Polka {


ProjectTreeStore::ProjectTreeStore(const Gtk::TreeModelColumnRecord& columns)
{
	set_column_types(columns);
}

Glib::RefPtr<ProjectTreeStore> ProjectTreeStore::create(const Gtk::TreeModelColumnRecord& columns)
{
	return Glib::RefPtr<ProjectTreeStore>( new ProjectTreeStore(columns) );
}

bool ProjectTreeStore::row_draggable_vfunc(const Gtk::TreeModel::Path& path) const
{
	return path.size() > 2;
}

bool ProjectTreeStore::row_drop_possible_vfunc(const Gtk::TreeModel::Path& dest,
        const Gtk::SelectionData& selection_data) const
{
	// must be at least inside a top folder
	if( dest.size() < 3 ) return false;
	
	// get the container of the destination location
	Gtk::TreeModel::Path containerPath = dest;
	containerPath.up();
	
	ProjectTreeStore* mdl = const_cast<ProjectTreeStore*>(this);
	const_iterator containerIter = mdl->get_iter(containerPath);

	if( !containerIter ) return false;

	// get base location data
	Glib::ustring destBase;
	containerIter->get_value( 1, destBase );

	if( destBase.empty() ) return false;

	// get the row of the dragged data
	Glib::RefPtr<Gtk::TreeModel> refThis = Glib::RefPtr<Gtk::TreeModel>(mdl);
	refThis->reference();
	Gtk::TreeModel::Path draggedPath;
	Gtk::TreeModel::Path::get_from_selection_data(selection_data, refThis,
	                                                  draggedPath);

	draggedPath.up();
	const_iterator draggedIter = mdl->get_iter(draggedPath);

	if( !draggedIter ) return false;

	// get base of dragged data
	Glib::ustring draggedBase;
	draggedIter->get_value( 1, draggedBase );

	if( destBase == draggedBase ) return true;
	
	return false;
}


} // namespace Polka
