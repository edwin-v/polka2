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

#ifndef _POLKA_PROJECTTREESTORE_H_
#define _POLKA_PROJECTTREESTORE_H_

#include <gtkmm/treestore.h>

namespace Polka {

class ProjectTreeStore : public Gtk::TreeStore
{
public:
	static Glib::RefPtr<ProjectTreeStore> create(const Gtk::TreeModelColumnRecord& columns);

protected:
	ProjectTreeStore(const Gtk::TreeModelColumnRecord& columns);

	//Overridden virtual functions:
	virtual bool row_draggable_vfunc(const Gtk::TreeModel::Path& path) const;
	virtual bool row_drop_possible_vfunc(const Gtk::TreeModel::Path& dest,
	                         const Gtk::SelectionData& selection_data) const;
};


} // namespace Polka

#endif // _POLKA_PROJECTTREESTORE_H_
