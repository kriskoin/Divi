#ifndef NODE_UTILS_H
#include "NodeUtils.h"
#endif
bool AbortNode(const std::string& strMessage, const std::string& userMessage)
{
    strMiscWarning = strMessage;
    LogPrintf("*** %s\n", strMessage);
    uiInterface.ThreadSafeMessageBox(
                userMessage.empty() ? translate("Error: A fatal internal error occured, see debug.log for details") : userMessage,
                "", CClientUIInterface::MSG_ERROR);
    StartShutdown();
    return false;
}
