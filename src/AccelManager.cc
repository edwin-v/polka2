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

std::vector<std::string> AccelManager::getContexts() const
{
	std::vector<std::string> vec;
	auto dit = m_Definitions.begin();
	while( dit != m_Definitions.end() ) {
		vec.push_back( dit->first );
		++dit;
	}
	return vec;
}

const AccelManager::DefinitionMap& AccelManager::getAccelDefinitons( const std::string& context ) const
{
	auto it = m_Definitions.find(context);
	assert( it != m_Definitions.end() );
	return *it->second;
}

const AccelManager::Definition& AccelManager::getAccelDefiniton( const std::string& context, const std::string& id ) const
{
	auto it = m_Definitions.find(context);
	assert( it != m_Definitions.end() );
	auto dit = it->second->begin();
	while( dit != it->second->end() ) {
		if( dit->id == id )
			break;
		++dit;
	}
	assert( dit != it->second->end() );
	return *dit;
}

const AccelManager::AssignmentMap& AccelManager::getAccelContext( const std::string& context ) const
{
	static const AssignmentMap empty;
	
	auto it = m_Assignments.find(context);
	if( it == m_Assignments.end() ) return empty;
	
	return it->second;
}

AccelManager::Assignment& AccelManager::getAccelAssignment( const std::string& context, const std::string& id )
{
	// get context
	auto dcit = m_Definitions.find(context);
	assert( dcit != m_Definitions.end() );
	// find definition
	auto dit = dcit->second->begin();
	int n = 0;
	while( dit != dcit->second->end() ) {
		if( dit->id == id ) break;
		++dit;
		++n;
	}
	assert( dit != dcit->second->end() );
	// return matching assignment
	Assignment& a = m_Assignments[context][n];
	if( !a.updated ) updateLink( context, n );
	return a;
}

void AccelManager::updateLink( const std::string& context, guint accel )
{
	auto dcit = m_Definitions.find(context);
	if( dcit == m_Definitions.end() ) return;
	if( dcit->second->size() <= accel ) return;

	Assignment& a = m_Assignments[context][accel];
	
	if( a.link.size() ) {
		// determine target context/id
		std::string cx = context, l;
		size_t p = a.link.find(':');
		if( p != std::string::npos ) {
			// get context from link
			cx = a.link.substr(0, p);
			l = a.link.substr(p+1);
		} else
			l = a.link;

		// get target context
		auto tcit = m_Definitions.find(context);
		if( tcit == m_Definitions.end() ) return;
		// find target id
		auto tdit = tcit->second->begin();
		int n = 0;
		while( tdit != tcit->second->end() ) {
			if( tdit->id == l ) break;
			++tdit;
			n++;
		}
		if( tdit == tcit->second->end() ) return;
		
		Assignment& ta = m_Assignments[cx][n];
		if( !ta.updated ) updateLink( cx, n );
		
		a.button = ta.button;
		a.key = ta.key;
		a.modifiers = ta.modifiers;
	}
	
	a.updated = true;
}

Glib::ustring AccelManager::linkDisplayText( const std::string& context, const std::string& link )
{
	std::string cx = context, l = link;
	size_t p = link.find(':');
	if( p != std::string::npos ) {
		// get context from link
		cx = link.substr(0, p);
		l = link.substr(p+1);
	}
	// get context
	auto dcit = m_Definitions.find(context);
	assert( dcit != m_Definitions.end() );
	// find definition
	auto dit = dcit->second->begin();
	while( dit != dcit->second->end() ) {
		if( dit->id == l ) break;
		++dit;
	}
	assert( dit != dcit->second->end() );
	// create string
	Glib::ustring t;
	if( cx != context )
		t = cx + ": ";
	for( unsigned int i = 0; i < dit->location.size(); i++ ) {
		if(i) t += '/';
		t += dit->location[i];
	}
	
	return t;
}

void AccelManager::changed()
{
	// invalidate all links
	auto cit = m_Assignments.begin();
	while( cit != m_Assignments.end() ) {
		auto ait = cit->second.begin();
		while( ait != cit->second.end() ) {
			if( !ait->link.empty() )
				ait->updated = false;
			++ait;
		}
		++cit;
	}
}

} // namespace ...
