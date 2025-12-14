// Copyright (c) 2024 The YukiCoin Core developers
// Genesis block miner for YukiCoin
// Compile: g++ -O3 -o genesis_miner genesis_miner.cpp -I../../src -I../../src/crypto ../../src/crypto/scrypt.cpp ../../src/crypto/sha256.cpp -std=c++17

#include <cstdint>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

// Minimal SHA256 implementation for genesis mining
#include <crypto/sha256.h>

// Scrypt implementation
extern "C" {
void scrypt_1024_1_1_256(const char* input, size_t inputlen, char* output, size_t outputlen);
}

// Simple uint256 representation
struct uint256 {
    uint8_t data[32];
    
    void SetNull() { memset(data, 0, 32); }
    
    std::string GetHex() const {
        std::stringstream ss;
        for (int i = 31; i >= 0; i--) {
            ss << std::hex << std::setfill('0') << std::setw(2) << (int)data[i];
        }
        return ss.str();
    }
    
    int Compare(const uint256& other) const {
        for (int i = 31; i >= 0; i--) {
            if (data[i] < other.data[i]) return -1;
            if (data[i] > other.data[i]) return 1;
        }
        return 0;
    }
    
    bool operator<=(const uint256& other) const { return Compare(other) <= 0; }
};

// Calculate target from nBits (compact representation)
uint256 CalculateTarget(uint32_t nBits) {
    uint256 target;
    target.SetNull();
    
    int nSize = nBits >> 24;
    uint32_t nWord = nBits & 0x007fffff;
    
    if (nSize <= 3) {
        nWord >>= 8 * (3 - nSize);
        target.data[0] = nWord & 0xff;
        target.data[1] = (nWord >> 8) & 0xff;
        target.data[2] = (nWord >> 16) & 0xff;
    } else {
        int offset = nSize - 3;
        target.data[offset] = nWord & 0xff;
        target.data[offset + 1] = (nWord >> 8) & 0xff;
        target.data[offset + 2] = (nWord >> 16) & 0xff;
    }
    
    return target;
}

// Write little-endian uint32
void WriteLE32(uint8_t* ptr, uint32_t val) {
    ptr[0] = val & 0xff;
    ptr[1] = (val >> 8) & 0xff;
    ptr[2] = (val >> 16) & 0xff;
    ptr[3] = (val >> 24) & 0xff;
}

// Double SHA256
void Hash256(const uint8_t* data, size_t len, uint8_t* out) {
    uint8_t hash1[32];
    CSHA256().Write(data, len).Finalize(hash1);
    CSHA256().Write(hash1, 32).Finalize(out);
}

// Serialize block header (80 bytes)
void SerializeHeader(uint8_t* out, int32_t nVersion, const uint256& hashPrevBlock,
                     const uint256& hashMerkleRoot, uint32_t nTime, uint32_t nBits, uint32_t nNonce) {
    WriteLE32(out, nVersion);
    memcpy(out + 4, hashPrevBlock.data, 32);
    memcpy(out + 36, hashMerkleRoot.data, 32);
    WriteLE32(out + 68, nTime);
    WriteLE32(out + 72, nBits);
    WriteLE32(out + 76, nNonce);
}

// Create coinbase transaction and calculate merkle root
uint256 CreateCoinbaseAndMerkleRoot(const char* pszTimestamp, const uint8_t* pubkey, size_t pubkeyLen, uint64_t reward) {
    std::vector<uint8_t> coinbase;
    
    // Version (4 bytes)
    coinbase.push_back(0x01); coinbase.push_back(0x00);
    coinbase.push_back(0x00); coinbase.push_back(0x00);
    
    // Number of inputs (1)
    coinbase.push_back(0x01);
    
    // Previous output hash (null for coinbase)
    for (int i = 0; i < 32; i++) coinbase.push_back(0x00);
    
    // Previous output index (0xffffffff for coinbase)
    coinbase.push_back(0xff); coinbase.push_back(0xff);
    coinbase.push_back(0xff); coinbase.push_back(0xff);
    
    // ScriptSig: OP_PUSHDATA(4) + nBits + OP_PUSHDATA(1) + 0x04 + timestamp
    std::vector<uint8_t> scriptSig;
    // Push 0x1d00ffff (nBits for Litecoin genesis)
    scriptSig.push_back(0x04);  // Push 4 bytes
    scriptSig.push_back(0xff);
    scriptSig.push_back(0xff);
    scriptSig.push_back(0x00);
    scriptSig.push_back(0x1d);
    // Push 0x04
    scriptSig.push_back(0x01);  // Push 1 byte
    scriptSig.push_back(0x04);
    // Push timestamp string
    size_t tsLen = strlen(pszTimestamp);
    scriptSig.push_back(0x4c);  // OP_PUSHDATA1
    scriptSig.push_back(tsLen);
    for (size_t i = 0; i < tsLen; i++) {
        scriptSig.push_back(pszTimestamp[i]);
    }
    
    // ScriptSig length
    coinbase.push_back(scriptSig.size());
    for (auto b : scriptSig) coinbase.push_back(b);
    
    // Sequence (0xffffffff)
    coinbase.push_back(0xff); coinbase.push_back(0xff);
    coinbase.push_back(0xff); coinbase.push_back(0xff);
    
    // Number of outputs (1)
    coinbase.push_back(0x01);
    
    // Output value (50 coins = 5000000000 satoshis)
    uint64_t val = reward;
    for (int i = 0; i < 8; i++) {
        coinbase.push_back(val & 0xff);
        val >>= 8;
    }
    
    // Output script: pubkey + OP_CHECKSIG
    coinbase.push_back(pubkeyLen + 1);  // Script length
    coinbase.push_back(pubkeyLen);      // Push pubkey length
    for (size_t i = 0; i < pubkeyLen; i++) coinbase.push_back(pubkey[i]);
    coinbase.push_back(0xac);           // OP_CHECKSIG
    
    // Lock time (0)
    coinbase.push_back(0x00); coinbase.push_back(0x00);
    coinbase.push_back(0x00); coinbase.push_back(0x00);
    
    // Calculate txid (double SHA256 of tx)
    uint256 txid;
    Hash256(coinbase.data(), coinbase.size(), txid.data);
    
    // For a single tx, merkle root = txid
    return txid;
}

int main(int argc, char* argv[]) {
    std::cout << "=== YukiCoin Genesis Block Miner ===" << std::endl;
    std::cout << "Using Scrypt (N=1024, r=1, p=1) for PoW" << std::endl << std::endl;
    
    // Genesis block parameters
    const char* pszTimestamp = "YukiCoin Genesis - December 14, 2024";
    uint32_t nTime = (uint32_t)time(nullptr);
    uint32_t nBits = 0x1e0ffff0;  // Litecoin-style initial difficulty
    int32_t nVersion = 1;
    uint64_t genesisReward = 50 * 100000000ULL;  // 50 coins
    
    // Genesis pubkey (same as Litecoin's for now - should be replaced)
    const uint8_t genesisPubkey[] = {
        0x04, 0x67, 0x8a, 0xfd, 0xb0, 0xfe, 0x55, 0x48, 0x27, 0x19, 0x67, 0xf1,
        0xa6, 0x71, 0x30, 0xb7, 0x10, 0x5c, 0xd6, 0xa8, 0x28, 0xe0, 0x39, 0x09,
        0xa6, 0x79, 0x62, 0xe0, 0xea, 0x1f, 0x61, 0xde, 0xb6, 0x49, 0xf6, 0xbc,
        0x3f, 0x4c, 0xef, 0x38, 0xc4, 0xf3, 0x55, 0x04, 0xe5, 0x1e, 0xc1, 0x12,
        0xde, 0x5c, 0x38, 0x4d, 0xf7, 0xba, 0x0b, 0x8d, 0x57, 0x8a, 0x4c, 0x70,
        0x2b, 0x6b, 0xf1, 0x1d, 0x5f
    };
    
    std::cout << "Timestamp: " << pszTimestamp << std::endl;
    std::cout << "nTime: " << nTime << std::endl;
    std::cout << "nBits: 0x" << std::hex << nBits << std::dec << std::endl;
    std::cout << "Block Reward: " << genesisReward / 100000000 << " YUKI" << std::endl;
    std::cout << std::endl;
    
    // Calculate merkle root
    uint256 hashMerkleRoot = CreateCoinbaseAndMerkleRoot(pszTimestamp, genesisPubkey, sizeof(genesisPubkey), genesisReward);
    std::cout << "Merkle Root: " << hashMerkleRoot.GetHex() << std::endl;
    
    // Calculate target
    uint256 target = CalculateTarget(nBits);
    std::cout << "Target: " << target.GetHex() << std::endl;
    std::cout << std::endl;
    
    // Previous block hash (null for genesis)
    uint256 hashPrevBlock;
    hashPrevBlock.SetNull();
    
    // Mining
    std::cout << "Mining genesis block..." << std::endl;
    uint8_t header[80];
    uint256 hash;
    uint32_t nNonce = 0;
    uint64_t hashCount = 0;
    time_t startTime = time(nullptr);
    
    while (true) {
        SerializeHeader(header, nVersion, hashPrevBlock, hashMerkleRoot, nTime, nBits, nNonce);
        
        // Scrypt hash
        scrypt_1024_1_1_256((const char*)header, 80, (char*)hash.data, 32);
        
        hashCount++;
        
        if (hash <= target) {
            std::cout << std::endl << "=== GENESIS BLOCK FOUND ===" << std::endl;
            std::cout << "nNonce: " << nNonce << std::endl;
            std::cout << "Hash: " << hash.GetHex() << std::endl;
            std::cout << std::endl;
            
            // Calculate block hash (SHA256d) for identification
            uint256 blockHash;
            Hash256(header, 80, blockHash.data);
            std::cout << "Block Hash (SHA256d): " << blockHash.GetHex() << std::endl;
            std::cout << std::endl;
            
            std::cout << "=== Add to chainparams.cpp ===" << std::endl;
            std::cout << "genesis = CreateGenesisBlock(" << nTime << ", " << nNonce << ", 0x" 
                      << std::hex << nBits << std::dec << ", " << nVersion << ", " 
                      << genesisReward / 100000000 << " * COIN);" << std::endl;
            std::cout << "consensus.hashGenesisBlock = genesis.GetHash();" << std::endl;
            std::cout << "assert(consensus.hashGenesisBlock == uint256{\"" << blockHash.GetHex() << "\"});" << std::endl;
            std::cout << "assert(genesis.hashMerkleRoot == uint256{\"" << hashMerkleRoot.GetHex() << "\"});" << std::endl;
            
            break;
        }
        
        // Progress update
        if (hashCount % 100000 == 0) {
            time_t now = time(nullptr);
            double elapsed = difftime(now, startTime);
            double hashRate = (elapsed > 0) ? hashCount / elapsed : 0;
            std::cout << "\rNonce: " << nNonce << " | Hash rate: " << std::fixed 
                      << std::setprecision(0) << hashRate << " H/s" << std::flush;
        }
        
        nNonce++;
        if (nNonce == 0) {
            // Overflow - need to change nTime
            nTime++;
            std::cout << "\nNonce overflow, incrementing nTime to " << nTime << std::endl;
        }
    }
    
    return 0;
}

