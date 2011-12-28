#include "AccelManager.h"
#include <glibmm/miscutils.h>
#include <iostream>
#include <cassert>


namespace Polka {


AccelManager::AccelManager()
{
	// store in system config dir
	std::string settings_file = Glib::get_user_config_dir();
	// with app name
	settings_file = Glib::build_filename(settings_file, "Polka2");
	// force create directory / error checking ????
	g_mkdir_with_parents( settings_file.c_str(), 0755 );
	// and filename
	settings_file = Glib::build_filename(settings_file, "accel.settings");
	
	m_Storage.setFilename( settings_file );
	
	int err = m_Storage.load();
	std::cout << "Accels load err = " << err << std::endl;
}

AccelManager::~AccelManager()
{
}

AccelManager& AccelManager::get()
{
	static AccelManager instance;
	return instance;
}

void AccelManager::addAccelMap( const std::string& context, const DefinitionMap& map )
{
	// add new context
	if( m_Definitions.find(context) == m_Definitions.end() ) {
		
		// new accelmap
		m_Definitions[context] = &map;
		AssignmentMap& am = m_Assignments[context];
		assert( am.size() == 0 ); // should be new

		// find storage container for context
		bool stored = m_Storage.findObject(context);
		if( stored ) {
			stored = m_Storage.object().findItem(context);
			if( stored ) stored = m_Storage.object().checkFormat("[SSIII]");
		};

		// init assigned accels
		guint i = 0;
		for( auto it = map.begin(); it != map.end(); ++it ) {
			// verify numbering
			assert( it->nr == i++ );
			// is linked?
			if( !it->link.empty() ) {
				// verify if link is allowed
				auto lit = find( it->allowed_links.begin(), it->allowed_links.end(), it->link );
				assert( lit != it->allowed_links.end() );
			}
			// fill assigned
			am.push_back( { it->link, it->default_button, it->default_key, it->default_modifiers } );

			if( stored ) {
				// find in storage
				for( int n = 0; n < m_Storage.object().arraySize(); n++ )
					if( m_Storage.object().stringField(n, 0) == it->id ) {
						am.back().link = m_Storage.object().stringField(n, 1);
						am.back().button = m_Storage.object().integerField(n, 2);
						am.back().key = m_Storage.object().integerField(n, 3);
						am.back().modifiers = Gdk::ModifierType(m_Storage.object().integerField(n, 4));
						am.back().updated = am.back().link.empty();
						break;
					}
			}		
		}
	}
}

const AccelManager::AssignmentMap& AccelManager::getAccelContext( const std::string& context )
{
	static const AssignmentMap empty;
	
	auto it = m_Assignments.find(context);
	if( it == m_Assignments.end() ) return empty;
	
	return it->second;
}

void AccelManager::updateLink( const std::string& context, guint accel )
{
	auto defit = m_Definitions.find(context);
	if( defit == m_Definitions.end() ) return;
	if( defit->second->size() <= accel ) return;

	Definition accdef = (*defit->second)[accel];
	Assignment accass = m_Assignments[context][accel];
}

} // namespace ...
