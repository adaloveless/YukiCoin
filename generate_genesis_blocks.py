import hashlib
import struct

def calculate_hash(version, prev_block, merkle_root, timestamp, bits, nonce):
    """
    Calculate the SHA256 double-hash of the block header.
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

def mine_genesis_block(network_name, version, prev_block, merkle_root, timestamp, bits):
    """
    Mine a genesis block by iterating the nonce to find a valid hash.
    """
    nonce = 0
    # Lower the difficulty target significantly
    target = (1 << (256 - 8)) - 1  # Very low difficulty (1 leading zero in hex)
    print(f"Mining {network_name} Genesis Block with reduced difficulty...")

    while True:
        hash = calculate_hash(version, prev_block, merkle_root, timestamp, bits, nonce)
        if int(hash, 16) < target:
            print(f"{network_name} Genesis Hash: {hash}")
            print(f"{network_name} Nonce: {nonce}")
            return hash, nonce
        nonce += 1
        if nonce % 100000 == 0:
            print(f"{network_name}: Still mining... Nonce = {nonce}")

# Parameters for each genesis block
genesis_blocks = [
    {
        "name": "Mainnet",
        "version": 1,
        "prev_block": "0" * 64,        # Previous block hash
        "merkle_root": "0" * 64,      # Replace with actual merkle root if available
        "timestamp": 1231006505,      # Timestamp
        "bits": 0x1f00ffff,           # Reduced difficulty bits
    },
    {
        "name": "Testnet3",
        "version": 1,
        "prev_block": "0" * 64,
        "merkle_root": "0" * 64,
        "timestamp": 1296688602,
        "bits": 0x1f00ffff,
    },
    {
        "name": "Testnet4",
        "version": 1,
        "prev_block": "0" * 64,
        "merkle_root": "0" * 64,
        "timestamp": 1714777860,
        "bits": 0x1f00ffff,
    },
    {
        "name": "Regtest",
        "version": 1,
        "prev_block": "0" * 64,
        "merkle_root": "0" * 64,
        "timestamp": 1598918400,
        "bits": 0x1f00ffff,
    },
]

# Mine each genesis block
for block in genesis_blocks:
    hash, nonce = mine_genesis_block(
        block["name"],
        block["version"],
        block["prev_block"],
        block["merkle_root"],
        block["timestamp"],
        block["bits"],
    )
    print(f"{block['name']} Results:")
    print(f"  Genesis Hash: {hash}")
    print(f"  Nonce: {nonce}")
    print()
