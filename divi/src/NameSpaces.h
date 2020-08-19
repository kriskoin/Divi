// Copyright (c) 2017-2020 The DIVI Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef NAME_SPACES_H
#define NAME_SPACES_H


namespace BlockIndexComparator {
    
    // Internal stuff
    struct CBlockIndexWorkComparator {
        bool operator()(CBlockIndex* pa, CBlockIndex* pb) const
        {
            // First sort by most total work, ...
            if (pa->nChainWork > pb->nChainWork) return false;
            if (pa->nChainWork < pb->nChainWork) return true;

            // ... then by earliest time received, ...
            if (pa->nSequenceId < pb->nSequenceId) return false;
            if (pa->nSequenceId > pb->nSequenceId) return true;

            // Use pointer address as tie breaker (should only happen with blocks
            // loaded from disk, as those all have id 0).
            if (pa < pb) return false;
            if (pa > pb) return true;

            // Identical blocks.
            return false;
        }
    };

    CBlockIndex* pindexBestInvalid;

    /**
        * The set of all CBlockIndex entries with BLOCK_VALID_TRANSACTIONS (for itself and all ancestors) and
        * as good as our current tip or better. Entries may be failed, though.
        */
    std::set<CBlockIndex*, CBlockIndexWorkComparator> setBlockIndexCandidates;
    /** Number of nodes with fSyncStarted. */
    int nSyncStarted = 0;
    /** All pairs A->B, where A (or one if its ancestors) misses transactions, but B has transactions. */
    std::multimap<CBlockIndex*, CBlockIndex*> mapBlocksUnlinked;

    CCriticalSection cs_LastBlockFile;
    std::vector<CBlockFileInfo> vinfoBlockFile;
    int nLastBlockFile = 0;

    /**
        * Every received block is assigned a unique and increasing identifier, so we
        * know which one to give priority in case of a fork.
        */
    CCriticalSection cs_nBlockSequenceId;
    /** Blocks loaded from disk are assigned id 0, so start the counter at 1. */
    uint32_t nBlockSequenceId = 1;

    /**
        * Sources of received blocks, to be able to send them reject messages or ban
        * them, if processing happens afterwards. Protected by cs_main.
        */
    std::map<uint256, NodeId> mapBlockSource;

    /** Blocks that are in flight, and that are in the queue to be downloaded. Protected by cs_main. */
    struct QueuedBlock {
        uint256 hash;
        CBlockIndex* pindex;        //! Optional.
        int64_t nTime;              //! Time of "getdata" request in microseconds.
        int nValidatedQueuedBefore; //! Number of blocks queued with validated headers (globally) at the time this one is requested.
        bool fValidatedHeaders;     //! Whether this block has validated headers at the time of request.
    };
    std::map<uint256, std::pair<NodeId, std::list<QueuedBlock>::iterator> > mapBlocksInFlight;

    /** Number of blocks in flight with validated headers. */
    int nQueuedValidatedHeaders = 0;

    /** Number of preferable block download peers. */
    int nPreferredDownload = 0;

    /** Dirty block index entries. */
    std::set<CBlockIndex*> setDirtyBlockIndex;

    /** Dirty block file entries. */
    std::set<int> setDirtyFileInfo;

}

#endif