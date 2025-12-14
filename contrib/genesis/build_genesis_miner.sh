#!/bin/bash
# Build script for YukiCoin genesis block miner

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC_DIR="$SCRIPT_DIR/../../src"

echo "Building YukiCoin Genesis Block Miner..."
echo "Source directory: $SRC_DIR"

# Compile with the crypto sources
g++ -O3 -std=c++17 \
    -I"$SRC_DIR" \
    -I"$SRC_DIR/crypto" \
    -DHAVE_CONFIG_H=0 \
    "$SCRIPT_DIR/genesis_miner.cpp" \
    "$SRC_DIR/crypto/scrypt.cpp" \
    "$SRC_DIR/crypto/sha256.cpp" \
    -o "$SCRIPT_DIR/genesis_miner"

if [ $? -eq 0 ]; then
    echo "Build successful! Run ./genesis_miner to mine the genesis block."
else
    echo "Build failed!"
    exit 1
fi

