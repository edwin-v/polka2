#include "ObjectPropertySheet.h"

namespace Polka {

ObjectPropertySheet::ObjectPropertySheet()
{
}

ObjectPropertySheet::~ObjectPropertySheet()
{
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
