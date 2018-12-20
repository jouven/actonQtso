//because windows sucks...

#ifndef ACTONQTSO_CROSSPLATFORMMACROS_HPP
#define ACTONQTSO_CROSSPLATFORMMACROS_HPP

#include <QtCore/QtGlobal>

//remember to define this variable in the .pro file
#if defined(ACTONQTSO_LIB)
#  define EXPIMP_ACTONQTSO Q_DECL_EXPORT
#else
#  define EXPIMP_ACTONQTSO Q_DECL_IMPORT
#endif

#endif // ACTONQTSO_CROSSPLATFORMMACROS_HPP
