#ifndef _POLKA_IMPORTMANAGER_H_
#define _POLKA_IMPORTMANAGER_H_

#include <gtkmm/window.h>
#include <gtkmm/filechooserdialog.h>
#include <glibmm/ustring.h>
#include <vector>

namespace Polka { 

class Importer;
class Project;

class ImportManager
{
public:
	static ImportManager& get();


	// object registration
	void registerImporter( Importer *imp );

	// main interface
	void executeFileImport( Gtk::Window& parent, Project& project );

private:
	ImportManager();
	~ImportManager();

	void updatePreview();
	
	Gtk::FileChooserDialog *m_pFileChooser;
	std::vector<Importer*> m_Importers;
	Importer *m_pCurrentImporter;
};


} // namespace Polka 

#endif // _POLKA_IMPORTMANAGER_H_
