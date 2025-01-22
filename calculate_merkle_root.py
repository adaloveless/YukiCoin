import hashlib
import sys

def double_sha256(data):
    """Perform a double SHA-256 hash."""
    return hashlib.sha256(hashlib.sha256(data).digest()).digest()

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

    return tx_hashes[0]

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 calculate_merkle_root.py <coinbase_tx_hash>")
        sys.exit(1)

    try:
        # Read the coinbase transaction hash from the command line
        coinbase_tx_hash = bytes.fromhex(sys.argv[1])
        # Calculate the Merkle root
        merkle_root = calculate_merkle_root([coinbase_tx_hash])
        print("Merkle Root:", merkle_root.hex())
    except ValueError as e:
        print(f"Error: {e}")
        sys.exit(1)
