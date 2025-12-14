// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The YukiCoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <primitives/block.h>

#include <crypto/scrypt.h>
#include <hash.h>
#include <streams.h>
#include <tinyformat.h>

uint256 CBlockHeader::GetHash() const
{
    return (HashWriter{} << *this).GetHash();
}

uint256 CBlockHeader::GetPoWHash() const
{
    uint256 thash;
    // Serialize the block header (80 bytes) for Scrypt hashing
    std::vector<unsigned char> vch;
    VectorWriter{vch, 0, *this};
    scrypt_hash((const char*)vch.data(), (char*)thash.begin());
    return thash;
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=0x%08x, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u)\n",
        GetHash().ToString(),
        nVersion,
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        vtx.size());
    for (const auto& tx : vtx) {
        s << "  " << tx->ToString() << "\n";
    }
    return s.str();
}
