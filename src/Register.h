#ifndef _POLKA_REGISTER_H_
#define _POLKA_REGISTER_H_


namespace Polka {

class ObjectManager;
class ImportManager;

void registerObjects( ObjectManager& manager );
void registerImporters( ImportManager& manager );

} // namespace Polka

#endif // _POLKA_REGISTER_H_
