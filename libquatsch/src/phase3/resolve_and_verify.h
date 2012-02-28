#ifndef RESOLVE_AND_VERIFY_H_INCLUDED_20120130
#define RESOLVE_AND_VERIFY_H_INCLUDED_20120130

#include "phase2/Program.h"
#include "phase3/Program.h"

namespace quatsch { namespace compiler { namespace phase3 {
        ProgramPtr resolve_and_verify (phase2::Program const &);
} } }
#endif // RESOLVE_AND_VERIFY_H_INCLUDED_20120130