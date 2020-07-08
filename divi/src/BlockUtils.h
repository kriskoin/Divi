#ifndef BLOCK_UTILS_H
#define BLOCK_UTILS_H

#ifndef BLOCK_MAP_H
#include "blockmap.h"
#endif

#ifndef BITCOIN_CHAIN_H
#include "chain.h"
#endif

#ifndef NODE_UTILS_H
#include "NodeUtils.h"
#endif

#ifndef BITCOIN_MAIN_H
#include "main.h"
#endif

#include <cstddef>


BlockMap mapBlockIndex;

class CValidationState;

class CBlockIndex;

/** Find the last common block between the parameter chain and a locator. */
CBlockIndex* FindForkInGlobalIndex(const CChain& chain, const CBlockLocator& locator);

/** Mark a block as invalid. */
bool InvalidateBlock(CValidationState& state, CBlockIndex* pindex);

/** Remove invalidity status from a block and its descendants. */
bool ReconsiderBlock(CValidationState& state, CBlockIndex* pindex);

#endif // BLOCK_UTILS_H
