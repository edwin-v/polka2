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
