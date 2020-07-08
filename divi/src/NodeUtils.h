#ifndef NODE_UTILS_H
#define NODE_UTILS_H

#ifndef BITCOIN_UTIL_H
#include "util.h"
#endif

#ifndef BITCOIN_INIT_H
#include "init.h"
#endif

#include <string>

/** Abort with a message */
bool AbortNode(const std::string& msg, const std::string& userMessage = "");

#endif // BLOCK_UTILS_H
