import hashlib

def sha256(data):
    """Perform a single SHA-256 hash."""
    return hashlib.sha256(data).digest()

def double_sha256(data):
    """Perform a double SHA-256 hash."""
    return sha256(sha256(data))

def calculate_merkle_root(tx_hashes):
    """Calculate the Merkle root from a list of transaction hashes."""
    if not tx_hashes:
        raise ValueError("No transaction hashes provided")

    while len(tx_hashes) > 1:
        # If the number of hashes is odd, duplicate the last hash
        if len(tx_hashes) % 2 != 0:
            tx_hashes.append(tx_hashes[-1])

        # Hash pairs of transaction hashes
        tx_hashes = [
            double_sha256(tx_hashes[i] + tx_hashes[i + 1])
            for i in range(0, len(tx_hashes), 2)
        ]

    # The last remaining hash is the Merkle root
    return tx_hashes[0]

# Coinbase transaction hash (example)
coinbase_tx_hash = bytes.fromhex("4e9b0c198d9e82e62c7dbe7c2e2d7c2c8c19c33b1b4b2b7c8d4b9b2d9c19e82e")

# Calculate Merkle root for the genesis block
merkle_root = calculate_merkle_root([coinbase_tx_hash])
print("Merkle Root:", merkle_root.hex())
