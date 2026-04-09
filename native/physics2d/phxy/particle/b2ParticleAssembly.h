
#pragma once

#include "b2GrowableBuffer.h"
#include "../common/math/SqVec2.h"
#include "SqParticleContact.h"

namespace phxy
{


  struct FindContactCheck
  {
    unsigned short particleIndex;
    unsigned short comparatorIndex;
  };

  struct FindContactInput
  {
    unsigned int proxyIndex;
    SqVec2 position;
  };

  enum
  {
    NUM_V32_SLOTS = 4
  };

#ifdef __cplusplus
  extern "C"
  {
#endif

    extern int CalculateTags_Simd(const SqVec2 *positions,
                                  int count,
                                  const float &inverseDiameter,
                                  unsigned int *outTags);

    extern void FindContactsFromChecks_Simd(
        const FindContactInput *reordered,
        const FindContactCheck *checks,
        int numChecks,
        const float &particleDiameterSq,
        const float &particleDiameterInv,
        const unsigned int *flags,
        b2GrowableBuffer<phxy::SqParticleContact> &contacts);

#ifdef __cplusplus
  } // extern "C"
#endif
}