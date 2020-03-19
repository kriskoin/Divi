#ifndef WALLET_BACKUP_CREATOR_H
#define WALLET_BACKUP_CREATOR_H
#include <string>
#include <boost/filesystem.hpp>
#include <i_filesystem.h>

class I_FormattedTimestampProvider;

class I_DatabaseWrapper;

class WalletIntegrityVerifier
{
private:
    I_FileSystem& fileSystem_;
    I_DatabaseWrapper& dbInterface_;
public:
    WalletIntegrityVerifier(I_FileSystem& fileSystem, I_DatabaseWrapper& database);
    bool CheckWalletIntegrity();
};

class WalletBackupCreator
{
private:
    int nWalletBackups = 0;
    I_FileSystem& fileSystem_;
    I_FormattedTimestampProvider& formattedTimestampProvider_;
    std::string dataDirectory_;
    std::string walletFilename_;
    void ClearFoldersForResync();
    bool BackupDatabaseInCaseOfError();
    bool VerifyWallet(std::string strWalletFile);
    bool BackupFile(PathType& sourceFile, PathType& backupFile);
    bool BackupWalletFile(std::string strWalletFile, PathType backupDir);
    void PruneOldBackups(std::string strWalletFile, PathType backupDir);

public:
    WalletBackupCreator (
        int numberOfBackups,
        I_FileSystem& fileSystem,
        I_FormattedTimestampProvider& formattedTimestampProvider,
        std::string dataDirectory,
        std::string walletFilename);
    bool BackupWallet();
    bool CheckWalletIntegrity(bool resync = false);
};



#endif //WALLET_BACKUP_CREATOR_H