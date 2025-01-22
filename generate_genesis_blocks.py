import hashlib
import struct

def calculate_hash(version, prev_block, merkle_root, timestamp, bits, nonce):
    """
    Calculate the SHA-256 double-hash of the block header.
    """
    header = (
        struct.pack("<L", version) +
        bytes.fromhex(prev_block)[::-1] +
        bytes.fromhex(merkle_root)[::-1] +
        struct.pack("<L", timestamp) +
        struct.pack("<L", bits) +
        struct.pack("<L", nonce)
    )
    hash1 = hashlib.sha256(header).digest()
    hash2 = hashlib.sha256(hash1).digest()
    return hash2[::-1].hex()

def mine_genesis_block(version, prev_block, merkle_root, timestamp, bits, target_difficulty):
    """
    Mine a genesis block by iterating the nonce to find a valid hash.
    """
    nonce = 0
    target = (1 << (256 - target_difficulty)) - 1
    while True:
        hash = calculate_hash(version, prev_block, merkle_root, timestamp, bits, nonce)
        if int(hash, 16) < target:
            return hash, nonce
        nonce += 1

# Parameters for YukiCoin genesis blocks
genesis_blocks = [
    {
        "name": "Mainnet",
        "version": 1,
        "prev_block": "0" * 64,        # Previous block hash (all zeros for genesis block)
        "merkle_root": "0" * 64,      # Replace with actual merkle root if available
        "timestamp": 1231006505,      # Timestamp
        "bits": 0x1d00ffff,           # Difficulty bits
        "target_difficulty": 32       # Adjust for faster mining during testing
    },
    {
        "name": "Testnet3",
        "version": 1,
        "prev_block": "0" * 64,
        "merkle_root": "0" * 64,
        "timestamp": 1296688602,
        "bits": 0x1d00ffff,
        "target_difficulty": 32
    },
    {
        "name": "Testnet4",
        "version": 1,
        "prev_block": "0" * 64,
        "merkle_root": "0" * 64,
        "timestamp": 1714777860,
        "bits": 0x1d00ffff,
        "target_difficulty": 32
    },
    {
        "name": "Regtest",
        "version": 1,
        "prev_block": "0" * 64,
        "merkle_root": "0" * 64,
        "timestamp": 1598918400,
        "bits": 0x1e03ffff,
        "target_difficulty": 32
    }
]

# Mine each genesis block and print results
for block in genesis_blocks:
    print(f"Mining {block['name']} Genesis Block...")
    genesis_hash, nonce = mine_genesis_block(
        block['version'],
        block['prev_block'],
        block['merkle_root'],
        block['timestamp'],
        block['bits'],
        block['target_difficulty']
    )
    print(f"{block['name']} Genesis Hash: {genesis_hash}")
    print(f"{block['name']} Nonce: {nonce}")
    print()
