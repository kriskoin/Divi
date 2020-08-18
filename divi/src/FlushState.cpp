// Copyright (c) 2017-2020 The DIVI Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <FlushState.h>
#include <blockFileInfo.h>
#include <BlockFileOpener.h>
#include <sync.h>
#include <chain.h>
#include <util.h>

extern CCriticalSection cs_LastBlockFile;
extern int nLastBlockFile ;
extern std::vector<CBlockFileInfo> vinfoBlockFile;


void  FlushBlockFile(bool fFinalize  )
{
    LOCK(cs_LastBlockFile);

    CDiskBlockPos posOld(nLastBlockFile, 0);

    FILE* fileOld = OpenBlockFile(posOld);
    if (fileOld) {
        if (fFinalize)
            TruncateFile(fileOld, vinfoBlockFile[nLastBlockFile].nSize);
        FileCommit(fileOld);
        fclose(fileOld);
    }

    fileOld = OpenUndoFile(posOld);
    if (fileOld) {
        if (fFinalize)
            TruncateFile(fileOld, vinfoBlockFile[nLastBlockFile].nUndoSize);
        FileCommit(fileOld);
        fclose(fileOld);
    }
}