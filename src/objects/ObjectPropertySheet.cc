#include "ObjectPropertySheet.h"

namespace Polka {

ObjectPropertySheet::ObjectPropertySheet( const Glib::ustring& _name )
	: m_Name(_name)
{
}

ObjectPropertySheet::~ObjectPropertySheet()
{
}

const Glib::ustring& ObjectPropertySheet::pageName() const
{
	return m_Name;
}

ObjectPropertySheet::SignalSetModified ObjectPropertySheet::signalSetModified()
{
	return m_SignalSetModified;
}

void ObjectPropertySheet::setModified( bool value )
{
	m_SignalSetModified.emit(value);
}

} // namespace Polka 
