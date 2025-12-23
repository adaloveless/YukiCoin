// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The YukiCoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef YukiCoin_CONSENSUS_AMOUNT_H
#define YukiCoin_CONSENSUS_AMOUNT_H

#include <cstdint>

/** Amount in yukis (smallest unit, Can be negative) */
typedef int64_t CAmount;

/** The amount of yukis (smallest unit) in one YUKI. */
static constexpr CAmount COIN = 100000000;

/** No amount larger than this (in yuki) is valid.
 *
 * YukiCoin emission schedule:
 *   First month: 25,000 YUKI/block  (~438 million YUKI, 46% - founder allocation)
 *   Rest of Year 1: 1,000 YUKI/block  (~193 million YUKI)
 *   Year 2: 500 YUKI/block  (~105 million YUKI)
 *   Year 3: 250 YUKI/block  (~53 million YUKI)
 *   Year 4+: 100 YUKI/block with halving every 840,000 blocks (~168 million YUKI)
 * Total maximum supply: ~957 million YUKI
 * */
static constexpr CAmount MAX_MONEY = 1000000000LL * COIN;
inline bool MoneyRange(const CAmount& nValue) { return (nValue >= 0 && nValue <= MAX_MONEY); }

#endif // YukiCoin_CONSENSUS_AMOUNT_H
