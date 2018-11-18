#include "codegen.h"

RegisterAllocator::RegisterAllocator(const llvm::BasicBlock *bb) {
    basicBlock = bb;
}
