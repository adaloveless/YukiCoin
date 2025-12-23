#ifndef YukiCoin_CHAINPARAMSSEEDS_H
#define YukiCoin_CHAINPARAMSSEEDS_H
/**
 * List of fixed seed nodes for the YukiCoin network
 * 
 * Each line contains a BIP155 serialized (networkID, addr, port) tuple.
 * Format for IPv4: 0x01, 0x04, <4 bytes IP>, <2 bytes port big-endian>
 */

// Fixed seed: 63.231.178.217:8834 (trick.social)
// IPv4 format: 0x01 (network), 0x04 (addr len), IP bytes, port (8834 = 0x2282)
static const uint8_t chainparams_seed_main[] = {
    0x01,0x04,0x3f,0xe7,0xb2,0xd9,0x22,0x82,
};

static const uint8_t chainparams_seed_test[] = {
    0x00,
};

static const uint8_t chainparams_seed_testnet4[] = {
    0x00,
};

#endif // YukiCoin_CHAINPARAMSSEEDS_H
