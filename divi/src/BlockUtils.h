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

#ifndef BITCOIN_NET_H
#include "net.h"
#endif

#ifndef BITCOIN_VALIDATIONINTERFACE_H
#include "NotificationInterface.h"
#endif

#ifndef BITCOIN_ALERT_H
#include "alert.h"
#endif

#ifndef BITCOIN_CHAINPARAMS_H
#include "chainparams.h"
#endif // BITCOIN_CHAINPARAMS_H

#ifndef BITCOIN_CHECKPOINTS_H
#include "checkpoints.h"
#endif // BITCOIN_CHECKPOINTS_H

#include <cstddef>

enum FlushStateMode {
    FLUSH_STATE_IF_NEEDED,
    FLUSH_STATE_PERIODIC,
    FLUSH_STATE_ALWAYS
};



static CBlockIndex* pcheckpoint;
static CBlockIndex *pindexBestForkTip = NULL, *pindexBestForkBase = NULL;
static NotificationInterfaceRegistry registry;


class CBlockFileInfo
{
public:
    unsigned int nBlocks;      //! number of blocks stored in file
    unsigned int nSize;        //! number of used bytes of block file
    unsigned int nUndoSize;    //! number of used bytes in the undo file
    unsigned int nHeightFirst; //! lowest height of block in file
    unsigned int nHeightLast;  //! highest height of block in file
    uint64_t nTimeFirst;       //! earliest time of block in file
    uint64_t nTimeLast;        //! latest time of block in file

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        READWRITE(VARINT(nBlocks));
        READWRITE(VARINT(nSize));
        READWRITE(VARINT(nUndoSize));
        READWRITE(VARINT(nHeightFirst));
        READWRITE(VARINT(nHeightLast));
        READWRITE(VARINT(nTimeFirst));
        READWRITE(VARINT(nTimeLast));
    }

    void SetNull()
    {
        nBlocks = 0;
        nSize = 0;
        nUndoSize = 0;
        nHeightFirst = 0;
        nHeightLast = 0;
        nTimeFirst = 0;
        nTimeLast = 0;
    }

    CBlockFileInfo()
    {
        SetNull();
    }

    std::string ToString() const;

    /** update statistics (does not update nSize) */
    void AddBlock(unsigned int nHeightIn, uint64_t nTimeIn)
    {
        if (nBlocks == 0 || nHeightFirst > nHeightIn)
            nHeightFirst = nHeightIn;
        if (nBlocks == 0 || nTimeFirst > nTimeIn)
            nTimeFirst = nTimeIn;
        nBlocks++;
        if (nHeightIn > nHeightLast)
            nHeightLast = nHeightIn;
        if (nTimeIn > nTimeLast)
            nTimeLast = nTimeIn;
    }
};

static std::vector<CBlockFileInfo> vinfoBlockFile;
static int nLastBlockFile = 0;

/** Dirty block file entries. */
static set<int> setDirtyFileInfo;

static BlockMap mapBlockIndex;

class CValidationState;

class CBlockIndex;

class CBlockIndexWorkComparator ;

/** Find the last common block between the parameter chain and a locator. */
CBlockIndex* FindForkInGlobalIndex(const CChain& chain, const CBlockLocator& locator);

/** Mark a block as invalid. */
//bool InvalidateBlock(CValidationState& state, CBlockIndex* pindex);TODO: rid this
/*
bool  InvalidateBlock(CValidationState& state,
                    CBlockIndex* pindex,
                    set<CBlockIndex*> * pSetDirtyBlockIndex,
                    set<CBlockIndex*, CBlockIndexWorkComparator>* pSetBlockIndexCandidates);*/

void static CheckForkWarningConditions();
#endif // BLOCK_UTILS_H
