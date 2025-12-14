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
 *   Year 1: 500 YUKI/block  (~105 million YUKI, 42% of supply)
 *   Year 2: 200 YUKI/block  (~42 million YUKI, 17%)
 *   Year 3: 100 YUKI/block  (~21 million YUKI, 8%)
 *   Year 4+: 50 YUKI/block with halving every 840,000 blocks (~84 million YUKI, 33%)
 * Total maximum supply: ~252 million YUKI
 * */
static constexpr CAmount MAX_MONEY = 260000000 * COIN;
inline bool MoneyRange(const CAmount& nValue) { return (nValue >= 0 && nValue <= MAX_MONEY); }

#endif // YukiCoin_CONSENSUS_AMOUNT_H
