#ifndef _POLKA_IMPORTER_H_
#define _POLKA_IMPORTER_H_

#include <string>

namespace Gtk { class Widget; }

namespace Polka {

class Project;

class Importer
{
public:
	Importer( const std::string& _id );
	virtual ~Importer();

	virtual bool initImport( const std::string& filename ) = 0;
	virtual void cancelImport() = 0;

	virtual Gtk::Widget& getPreviewWidget() = 0;

	virtual bool importToProject( Project& project ) = 0;

protected:
	
	
private:
	std::string m_Id;
};

} // namespace Polka

#endif // _POLKA_IMPORTER_H_
