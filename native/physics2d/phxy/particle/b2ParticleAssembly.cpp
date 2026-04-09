#include "b2ParticleAssembly.h"
// #include "b2ParticleSystem.h"

using namespace phxy;

// // Helper function, called from assembly routine.
// void GrowParticleContactBuffer(b2GrowableBuffer<SqParticleContact> &contacts)
// {
// 	// Set contacts.count = capacity instead of count because there are
// 	// items past the end of the array waiting to be post-processed.
// 	// We must maintain the entire contacts array.
// 	// TODO: It would be better to have the items awaiting post-processing
// 	// in their own array on the stack.
// 	contacts.SetCount(contacts.GetCapacity());
// 	contacts.Grow();
// }
