#include <LotteryWinnersCalculator.h>

#include <SuperblockHelpers.h>
#include <hash.h>
#include <uint256.h>
#include <primitives/transaction.h>
#include <primitives/block.h>
#include <chain.h>
#include <chainparams.h>
#include <timedata.h>
#include <numeric>
#include <spork.h>
#include <BlockDiskAccessor.h>
#include <I_SuperblockHeightValidator.h>

LotteryWinnersCalculator::LotteryWinnersCalculator(
    const CChainParams& chainParameters,
    CChain& activeChain,
    CSporkManager& sporkManager,
    const I_SuperblockHeightValidator& superblockHeightValidator
    ): chainParameters_(chainParameters)
    , activeChain_(activeChain)
    , sporkManager_(sporkManager)
    , superblockHeightValidator_(superblockHeightValidator)
{
}

int LotteryWinnersCalculator::minimumCoinstakeForTicket(int nHeight) const
{
    int nMinStakeValue = 10000; // default is 10k

    if(sporkManager_.IsSporkActive(SPORK_16_LOTTERY_TICKET_MIN_VALUE)) {
        MultiValueSporkList<LotteryTicketMinValueSporkValue> vValues;
        CSporkManager::ConvertMultiValueSporkVector(sporkManager_.GetMultiValueSpork(SPORK_16_LOTTERY_TICKET_MIN_VALUE), vValues);
        auto nBlockTime = activeChain_[nHeight] ? activeChain_[nHeight]->nTime : GetAdjustedTime();
        LotteryTicketMinValueSporkValue activeSpork = CSporkManager::GetActiveMultiValueSpork(vValues, nHeight, nBlockTime);

        if(activeSpork.IsValid()) {
            // we expect that this value is in coins, not in satoshis
            nMinStakeValue = activeSpork.nEntryTicketValue;
        }
    }

    return nMinStakeValue;
}

uint256 LotteryWinnersCalculator::CalculateLotteryScore(const uint256 &hashCoinbaseTx, const uint256 &hashLastLotteryBlock) const
{
    // Deterministically calculate a "score" for a Masternode based on any given (block)hash
    CHashWriter ss(SER_GETHASH, PROTOCOL_VERSION);
    ss << hashCoinbaseTx << hashLastLotteryBlock;
    return ss.GetHash();
}

bool LotteryWinnersCalculator::IsCoinstakeValidForLottery(const CTransaction &tx, int nHeight) const
{
    CAmount nAmount = 0;
    if(tx.IsCoinBase()) {
        nAmount = tx.vout[0].nValue;
    }
    else {
        auto payee = tx.vout[1].scriptPubKey;
        nAmount = std::accumulate(std::begin(tx.vout), std::end(tx.vout), CAmount(0), [payee](CAmount accum, const CTxOut &out) {
                return out.scriptPubKey == payee ? accum + out.nValue : accum;
    });
    }

    return nAmount > minimumCoinstakeForTicket(nHeight) * COIN; // only if stake is more than 10k
}

LotteryCoinstakes LotteryWinnersCalculator::CalculateLotteryWinners(const CBlock &block, const CBlockIndex *prevBlockIndex, int nHeight) const
{
    LotteryCoinstakes result;
    // if that's a block when lottery happens, reset score for whole cycle
    if(superblockHeightValidator_.IsValidLotteryBlockHeight(nHeight))
        return result;

    if(!prevBlockIndex)
        return result;

    const int lotteryBlockPaymentCycle = superblockHeightValidator_.GetLotteryBlockPaymentCycle(nHeight);
    int nLastLotteryHeight = std::max(chainParameters_.GetLotteryBlockStartBlock(),  lotteryBlockPaymentCycle* ((nHeight - 1) / lotteryBlockPaymentCycle) );

    if(nHeight <= nLastLotteryHeight) {
        return result;
    }

    const auto& coinbaseTx = (nHeight > chainParameters_.LAST_POW_BLOCK() ? block.vtx[1] : block.vtx[0]);

    if(!IsCoinstakeValidForLottery(coinbaseTx, nHeight)) {
        return prevBlockIndex->vLotteryWinnersCoinstakes; // return last if we have no lotter participant in this block
    }

    CBlockIndex* pblockindex = activeChain_[nLastLotteryHeight];
    auto hashLastLotteryBlock = pblockindex->GetBlockHash();
    // lotteryWinnersCoinstakes has hashes of coinstakes, let calculate old scores + new score
    using LotteryScore = uint256;
    using LotteryCoinstake = std::pair<uint256, CScript>;
    std::vector<std::pair<LotteryScore, LotteryCoinstake>> scores;

    for(auto &&lotteryCoinstake : prevBlockIndex->vLotteryWinnersCoinstakes) {
        scores.emplace_back(CalculateLotteryScore(lotteryCoinstake.first, hashLastLotteryBlock), lotteryCoinstake);
    }

    auto newScore = CalculateLotteryScore(coinbaseTx.GetHash(), hashLastLotteryBlock);
    scores.emplace_back(newScore, std::make_pair(coinbaseTx.GetHash(),coinbaseTx.vout[1].scriptPubKey) );

    // biggest entry at the begining
    if(scores.size() > 1)
    {
        std::sort(std::begin(scores), std::end(scores), 
            [](const std::pair<LotteryScore, LotteryCoinstake> &lhs, const std::pair<LotteryScore, LotteryCoinstake> &rhs) 
            {
                return lhs.first > rhs.first;
            }
        );
    }

    scores.resize(std::min<size_t>(scores.size(), 11)); // don't go over 11 entries, since we will have only 11 winners

    // prepare new coinstakes vector
    for(auto &&score : scores) {
        result.push_back(score.second);
    }

    return result;
}