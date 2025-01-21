22.0 Release Notes
==================

YukiCoin Core version 22.0 is now available from:

  <https://YukiCoincore.org/bin/YukiCoin-core-22.0/>

This release includes new features, various bug fixes and performance
improvements, as well as updated translations.

Please report bugs using the issue tracker at GitHub:

  <https://github.com/YukiCoin/YukiCoin/issues>

To receive security and update notifications, please subscribe to:

  <https://YukiCoincore.org/en/list/announcements/join/>

How to Upgrade
==============

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes in some cases), then run the
installer (on Windows) or just copy over `/Applications/YukiCoin-Qt` (on Mac)
or `YukiCoind`/`YukiCoin-qt` (on Linux).

Upgrading directly from a version of YukiCoin Core that has reached its EOL is
possible, but it might take some time if the data directory needs to be migrated. Old
wallet versions of YukiCoin Core are generally supported.

Compatibility
==============

YukiCoin Core is supported and extensively tested on operating systems
using the Linux kernel, macOS 10.14+, and Windows 7 and newer.  YukiCoin
Core should also work on most other Unix-like systems but is not as
frequently tested on them.  It is not recommended to use YukiCoin Core on
unsupported systems.

From YukiCoin Core 22.0 onwards, macOS versions earlier than 10.14 are no longer supported.

Notable changes
===============

P2P and network changes
-----------------------
- Added support for running YukiCoin Core as an
  [I2P (Invisible Internet Project)](https://en.wikipedia.org/wiki/I2P) service
  and connect to such services. See [i2p.md](https://github.com/YukiCoin/YukiCoin/blob/22.x/doc/i2p.md) for details. (#20685)
- This release removes support for Tor version 2 hidden services in favor of Tor
  v3 only, as the Tor network [dropped support for Tor
  v2](https://blog.torproject.org/v2-deprecation-timeline) with the release of
  Tor version 0.4.6.  Henceforth, YukiCoin Core ignores Tor v2 addresses; it
  neither rumors them over the network to other peers, nor stores them in memory
  or to `peers.dat`.  (#22050)

- Added NAT-PMP port mapping support via
  [`libnatpmp`](https://miniupnp.tuxfamily.org/libnatpmp.html). (#18077)

New and Updated RPCs
--------------------

- Due to [BIP 350](https://github.com/YukiCoin/bips/blob/master/bip-0350.mediawiki)
  being implemented, behavior for all RPCs that accept addresses is changed when
  a native witness version 1 (or higher) is passed. These now require a Bech32m
  encoding instead of a Bech32 one, and Bech32m encoding will be used for such
  addresses in RPC output as well. No version 1 addresses should be created
  for mainnet until consensus rules are adopted that give them meaning
  (as will happen through [BIP 341](https://github.com/YukiCoin/bips/blob/master/bip-0341.mediawiki)).
  Once that happens, Bech32m is expected to be used for them, so this shouldn't
  affect any production systems, but may be observed on other networks where such
  addresses already have meaning (like signet). (#20861)

- The `getpeerinfo` RPC returns two new boolean fields, `bip152_hb_to` and
  `bip152_hb_from`, that respectively indicate whether we selected a peer to be
  in compact blocks high-bandwidth mode or whether a peer selected us as a
  compact blocks high-bandwidth peer. High-bandwidth peers send new block
  announcements via a `cmpctblock` message rather than the usual inv/headers
  announcements. See BIP 152 for more details. (#19776)

- `getpeerinfo` no longer returns the following fields: `addnode`, `banscore`,
  and `whitelisted`, which were previously deprecated in 0.21. Instead of
  `addnode`, the `connection_type` field returns manual. Instead of
  `whitelisted`, the `permissions` field indicates if the peer has special
  privileges. The `banscore` field has simply been removed. (#20755)

- The following RPCs:  `gettxout`, `getrawtransaction`, `decoderawtransaction`,
  `decodescript`, `gettransaction`, and REST endpoints: `/rest/tx`,
  `/rest/getutxos`, `/rest/block` deprecated the following fields (which are no
  longer returned in the responses by default): `addresses`, `reqSigs`.
  The `-deprecatedrpc=addresses` flag must be passed for these fields to be
  included in the RPC response. This flag/option will be available only for this major release, after which
  the deprecation will be removed entirely. Note that these fields are attributes of
  the `scriptPubKey` object returned in the RPC response. However, in the response
  of `decodescript` these fields are top-level attributes, and included again as attributes
  of the `scriptPubKey` object. (#20286)

- When creating a hex-encoded YukiCoin transaction using the `YukiCoin-tx` utility
  with the `-json` option set, the following fields: `addresses`, `reqSigs` are no longer
  returned in the tx output of the response. (#20286)

- The `listbanned` RPC now returns two new numeric fields: `ban_duration` and `time_remaining`.
  Respectively, these new fields indicate the duration of a ban and the time remaining until a ban expires,
  both in seconds. Additionally, the `ban_created` field is repositioned to come before `banned_until`. (#21602)

- The `setban` RPC can ban onion addresses again. This fixes a regression introduced in version 0.21.0. (#20852)

- The `getnodeaddresses` RPC now returns a "network" field indicating the
  network type (ipv4, ipv6, onion, or i2p) for each address.  (#21594)

- `getnodeaddresses` now also accepts a "network" argument (ipv4, ipv6, onion,
  or i2p) to return only addresses of the specified network.  (#21843)

- The `testmempoolaccept` RPC now accepts multiple transactions (still experimental at the moment,
  API may be unstable). This is intended for testing transaction packages with dependency
  relationships; it is not recommended for batch-validating independent transactions. In addition to
  mempool policy, package policies apply: the list cannot contain more than 25 transactions or have a
  total size exceeding 101K virtual bytes, and cannot conflict with (spend the same inputs as) each other or
  the mempool, even if it would be a valid BIP125 replace-by-fee. There are some known limitations to
  the accuracy of the test accept: it's possible for `testmempoolaccept` to return "allowed"=True for a
  group of transactions, but "too-long-mempool-chain" if they are actually submitted. (#20833)

- `addmultisigaddress` and `createmultisig` now support up to 20 keys for
  Segwit addresses. (#20867)

Changes to Wallet or GUI related RPCs can be found in the GUI or Wallet section below.

Build System
------------

- Release binaries are now produced using the new `guix`-based build system.
  The [/doc/release-process.md](/doc/release-process.md) document has been updated accordingly.

Files
-----

- The list of banned hosts and networks (via `setban` RPC) is now saved on disk
  in JSON format in `banlist.json` instead of `banlist.dat`. `banlist.dat` is
  only read on startup if `banlist.json` is not present. Changes are only written to the new
  `banlist.json`. A future version of YukiCoin Core may completely ignore
  `banlist.dat`. (#20966)

New settings
------------

- The `-natpmp` option has been added to use NAT-PMP to map the listening port.
  If both UPnP and NAT-PMP are enabled, a successful allocation from UPnP
  prevails over one from NAT-PMP. (#18077)

Updated settings
----------------

Changes to Wallet or GUI related settings can be found in the GUI or Wallet section below.

- Passing an invalid `-rpcauth` argument now cause YukiCoind to fail to start.  (#20461)

Tools and Utilities
-------------------

- A new CLI `-addrinfo` command returns the number of addresses known to the
  node per network type (including Tor v2 versus v3) and total. This can be
  useful to see if the node knows enough addresses in a network to use options
  like `-onlynet=<network>` or to upgrade to this release of YukiCoin Core 22.0
  that supports Tor v3 only.  (#21595)

- A new `-rpcwaittimeout` argument to `YukiCoin-cli` sets the timeout
  in seconds to use with `-rpcwait`. If the timeout expires,
  `YukiCoin-cli` will report a failure. (#21056)

Wallet
------

- External signers such as hardware wallets can now be used through the new RPC methods `enumeratesigners` and `displayaddress`. Support is also added to the `send` RPC call. This feature is experimental. See [external-signer.md](https://github.com/YukiCoin/YukiCoin/blob/22.x/doc/external-signer.md) for details. (#16546)

- A new `listdescriptors` RPC is available to inspect the contents of descriptor-enabled wallets.
  The RPC returns public versions of all imported descriptors, including their timestamp and flags.
  For ranged descriptors, it also returns the range boundaries and the next index to generate addresses from. (#20226)

- The `bumpfee` RPC is not available with wallets that have private keys
  disabled. `psbtbumpfee` can be used instead. (#20891)

- The `fundrawtransaction`, `send` and `walletcreatefundedpsbt` RPCs now support an `include_unsafe` option
  that when `true` allows using unsafe inputs to fund the transaction.
  Note that the resulting transaction may become invalid if one of the unsafe inputs disappears.
  If that happens, the transaction must be funded with different inputs and republished. (#21359)

- We now support up to 20 keys in `multi()` and `sortedmulti()` descriptors
  under `wsh()`. (#20867)

- Taproot descriptors can be imported into the wallet only after activation has occurred on the network (e.g. mainnet, testnet, signet) in use. See [descriptors.md](https://github.com/YukiCoin/YukiCoin/blob/22.x/doc/descriptors.md) for supported descriptors.

GUI changes
-----------

- External signers such as hardware wallets can now be used. These require an external tool such as [HWI](https://github.com/YukiCoin-core/HWI) to be installed and configured under Options -> Wallet. When creating a new wallet a new option "External signer" will appear in the dialog. If the device is detected, its name is suggested as the wallet name. The watch-only keys are then automatically imported. Receive addresses can be verified on the device. The send dialog will automatically use the connected device. This feature is experimental and the UI may freeze for a few seconds when performing these actions.

Low-level changes
=================

RPC
---

- The RPC server can process a limited number of simultaneous RPC requests.
  Previously, if this limit was exceeded, the RPC server would respond with
  [status code 500 (`HTTP_INTERNAL_SERVER_ERROR`)](https://en.wikipedia.org/wiki/List_of_HTTP_status_codes#5xx_server_errors).
  Now it returns status code 503 (`HTTP_SERVICE_UNAVAILABLE`). (#18335)

- Error codes have been updated to be more accurate for the following error cases (#18466):
  - `signmessage` now returns RPC_INVALID_ADDRESS_OR_KEY (-5) if the
    passed address is invalid. Previously returned RPC_TYPE_ERROR (-3).
  - `verifymessage` now returns RPC_INVALID_ADDRESS_OR_KEY (-5) if the
    passed address is invalid. Previously returned RPC_TYPE_ERROR (-3).
  - `verifymessage` now returns RPC_TYPE_ERROR (-3) if the passed signature
    is malformed. Previously returned RPC_INVALID_ADDRESS_OR_KEY (-5).

Tests
-----

22.0 change log
===============

A detailed list of changes in this version follows. To keep the list to a manageable length, small refactors and typo fixes are not included, and similar changes are sometimes condensed into one line.

### Consensus
- YukiCoin/YukiCoin#19438 Introduce deploymentstatus (ajtowns)
- YukiCoin/YukiCoin#20207 Follow-up extra comments on taproot code and tests (sipa)
- YukiCoin/YukiCoin#21330 Deal with missing data in signature hashes more consistently (sipa)

### Policy
- YukiCoin/YukiCoin#18766 Disable fee estimation in blocksonly mode (by removing the fee estimates global) (darosior)
- YukiCoin/YukiCoin#20497 Add `MAX_STANDARD_SCRIPTSIG_SIZE` to policy (sanket1729)
- YukiCoin/YukiCoin#20611 Move `TX_MAX_STANDARD_VERSION` to policy (MarcoFalke)

### Mining
- YukiCoin/YukiCoin#19937, YukiCoin/YukiCoin#20923 Signet mining utility (ajtowns)

### Block and transaction handling
- YukiCoin/YukiCoin#14501 Fix possible data race when committing block files (luke-jr)
- YukiCoin/YukiCoin#15946 Allow maintaining the blockfilterindex when using prune (jonasschnelli)
- YukiCoin/YukiCoin#18710 Add local thread pool to CCheckQueue (hebasto)
- YukiCoin/YukiCoin#19521 Coinstats Index (fjahr)
- YukiCoin/YukiCoin#19806 UTXO snapshot activation (jamesob)
- YukiCoin/YukiCoin#19905 Remove dead CheckForkWarningConditionsOnNewFork (MarcoFalke)
- YukiCoin/YukiCoin#19935 Move SaltedHashers to separate file and add some new ones (achow101)
- YukiCoin/YukiCoin#20054 Remove confusing and useless "unexpected version" warning (MarcoFalke)
- YukiCoin/YukiCoin#20519 Handle rename failure in `DumpMempool(…)` by using the `RenameOver(…)` return value (practicalswift)
- YukiCoin/YukiCoin#20749, YukiCoin/YukiCoin#20750, YukiCoin/YukiCoin#21055, YukiCoin/YukiCoin#21270, YukiCoin/YukiCoin#21525, YukiCoin/YukiCoin#21391, YukiCoin/YukiCoin#21767, YukiCoin/YukiCoin#21866 Prune `g_chainman` usage (dongcarl)
- YukiCoin/YukiCoin#20833 rpc/validation: enable packages through testmempoolaccept (glozow)
- YukiCoin/YukiCoin#20834 Locks and docs in ATMP and CheckInputsFromMempoolAndCache (glozow)
- YukiCoin/YukiCoin#20854 Remove unnecessary try-block (amitiuttarwar)
- YukiCoin/YukiCoin#20868 Remove redundant check on pindex (jarolrod)
- YukiCoin/YukiCoin#20921 Don't try to invalidate genesis block in CChainState::InvalidateBlock (theStack)
- YukiCoin/YukiCoin#20972 Locks: Annotate CTxMemPool::check to require `cs_main` (dongcarl)
- YukiCoin/YukiCoin#21009 Remove RewindBlockIndex logic (dhruv)
- YukiCoin/YukiCoin#21025 Guard chainman chainstates with `cs_main` (dongcarl)
- YukiCoin/YukiCoin#21202 Two small clang lock annotation improvements (amitiuttarwar)
- YukiCoin/YukiCoin#21523 Run VerifyDB on all chainstates (jamesob)
- YukiCoin/YukiCoin#21573 Update libsecp256k1 subtree to latest master (sipa)
- YukiCoin/YukiCoin#21582, YukiCoin/YukiCoin#21584, YukiCoin/YukiCoin#21585 Fix assumeutxo crashes (MarcoFalke)
- YukiCoin/YukiCoin#21681 Fix ActivateSnapshot to use hardcoded nChainTx (jamesob)
- YukiCoin/YukiCoin#21796 index: Avoid async shutdown on init error (MarcoFalke)
- YukiCoin/YukiCoin#21946 Document and test lack of inherited signaling in RBF policy (ariard)
- YukiCoin/YukiCoin#22084 Package testmempoolaccept followups (glozow)
- YukiCoin/YukiCoin#22102 Remove `Warning:` from warning message printed for unknown new rules (prayank23)
- YukiCoin/YukiCoin#22112 Force port 0 in I2P (vasild)
- YukiCoin/YukiCoin#22135 CRegTestParams: Use `args` instead of `gArgs` (kiminuo)
- YukiCoin/YukiCoin#22146 Reject invalid coin height and output index when loading assumeutxo (MarcoFalke)
- YukiCoin/YukiCoin#22253 Distinguish between same tx and same-nonwitness-data tx in mempool (glozow)
- YukiCoin/YukiCoin#22261 Two small fixes to node broadcast logic (jnewbery)
- YukiCoin/YukiCoin#22415 Make `m_mempool` optional in CChainState (jamesob)
- YukiCoin/YukiCoin#22499 Update assumed chain params (sriramdvt)
- YukiCoin/YukiCoin#22589 net, doc: update I2P hardcoded seeds and docs for 22.0 (jonatack)

### P2P protocol and network code
- YukiCoin/YukiCoin#18077 Add NAT-PMP port forwarding support (hebasto)
- YukiCoin/YukiCoin#18722 addrman: improve performance by using more suitable containers (vasild)
- YukiCoin/YukiCoin#18819 Replace `cs_feeFilter` with simple std::atomic (MarcoFalke)
- YukiCoin/YukiCoin#19203 Add regression fuzz harness for CVE-2017-18350. Add FuzzedSocket (practicalswift)
- YukiCoin/YukiCoin#19288 fuzz: Add fuzzing harness for TorController (practicalswift)
- YukiCoin/YukiCoin#19415 Make DNS lookup mockable, add fuzzing harness (practicalswift)
- YukiCoin/YukiCoin#19509 Per-Peer Message Capture (troygiorshev)
- YukiCoin/YukiCoin#19763 Don't try to relay to the address' originator (vasild)
- YukiCoin/YukiCoin#19771 Replace enum CConnMan::NumConnections with enum class ConnectionDirection (luke-jr)
- YukiCoin/YukiCoin#19776 net, rpc: expose high bandwidth mode state via getpeerinfo (theStack)
- YukiCoin/YukiCoin#19832 Put disconnecting logs into BCLog::NET category (hebasto)
- YukiCoin/YukiCoin#19858 Periodically make block-relay connections and sync headers (sdaftuar)
- YukiCoin/YukiCoin#19884 No delay in adding fixed seeds if -dnsseed=0 and peers.dat is empty (dhruv)
- YukiCoin/YukiCoin#20079 Treat handshake misbehavior like unknown message (MarcoFalke)
- YukiCoin/YukiCoin#20138 Assume that SetCommonVersion is called at most once per peer (MarcoFalke)
- YukiCoin/YukiCoin#20162 p2p: declare Announcement::m_state as uint8_t, add getter/setter (jonatack)
- YukiCoin/YukiCoin#20197 Protect onions in AttemptToEvictConnection(), add eviction protection test coverage (jonatack)
- YukiCoin/YukiCoin#20210 assert `CNode::m_inbound_onion` is inbound in ctor, add getter, unit tests (jonatack)
- YukiCoin/YukiCoin#20228 addrman: Make addrman a top-level component (jnewbery)
- YukiCoin/YukiCoin#20234 Don't bind on 0.0.0.0 if binds are restricted to Tor (vasild)
- YukiCoin/YukiCoin#20477 Add unit testing of node eviction logic (practicalswift)
- YukiCoin/YukiCoin#20516 Well-defined CAddress disk serialization, and addrv2 anchors.dat (sipa)
- YukiCoin/YukiCoin#20557 addrman: Fix new table bucketing during unserialization (jnewbery)
- YukiCoin/YukiCoin#20561 Periodically clear `m_addr_known` (sdaftuar)
- YukiCoin/YukiCoin#20599 net processing: Tolerate sendheaders and sendcmpct messages before verack (jnewbery)
- YukiCoin/YukiCoin#20616 Check CJDNS address is valid (lontivero)
- YukiCoin/YukiCoin#20617 Remove `m_is_manual_connection` from CNodeState (ariard)
- YukiCoin/YukiCoin#20624 net processing: Remove nStartingHeight check from block relay (jnewbery)
- YukiCoin/YukiCoin#20651 Make p2p recv buffer timeout 20 minutes for all peers (jnewbery)
- YukiCoin/YukiCoin#20661 Only select from addrv2-capable peers for torv3 address relay (sipa)
- YukiCoin/YukiCoin#20685 Add I2P support using I2P SAM (vasild)
- YukiCoin/YukiCoin#20690 Clean up logging of outbound connection type (sdaftuar)
- YukiCoin/YukiCoin#20721 Move ping data to `net_processing` (jnewbery)
- YukiCoin/YukiCoin#20724 Cleanup of -debug=net log messages (ajtowns)
- YukiCoin/YukiCoin#20747 net processing: Remove dropmessagestest (jnewbery)
- YukiCoin/YukiCoin#20764 cli -netinfo peer connections dashboard updates 🎄 ✨ (jonatack)
- YukiCoin/YukiCoin#20788 add RAII socket and use it instead of bare SOCKET (vasild)
- YukiCoin/YukiCoin#20791 remove unused legacyWhitelisted in AcceptConnection() (jonatack)
- YukiCoin/YukiCoin#20816 Move RecordBytesSent() call out of `cs_vSend` lock (jnewbery)
- YukiCoin/YukiCoin#20845 Log to net debug in MaybeDiscourageAndDisconnect except for noban and manual peers (MarcoFalke)
- YukiCoin/YukiCoin#20864 Move SocketSendData lock annotation to header (MarcoFalke)
- YukiCoin/YukiCoin#20965 net, rpc:  return `NET_UNROUTABLE` as `not_publicly_routable`, automate helps (jonatack)
- YukiCoin/YukiCoin#20966 banman: save the banlist in a JSON format on disk (vasild)
- YukiCoin/YukiCoin#21015 Make all of `net_processing` (and some of net) use std::chrono types (dhruv)
- YukiCoin/YukiCoin#21029 YukiCoin-cli: Correct docs (no "generatenewaddress" exists) (luke-jr)
- YukiCoin/YukiCoin#21148 Split orphan handling from `net_processing` into txorphanage (ajtowns)
- YukiCoin/YukiCoin#21162 Net Processing: Move RelayTransaction() into PeerManager (jnewbery)
- YukiCoin/YukiCoin#21167 make `CNode::m_inbound_onion` public, initialize explicitly (jonatack)
- YukiCoin/YukiCoin#21186 net/net processing: Move addr data into `net_processing` (jnewbery)
- YukiCoin/YukiCoin#21187 Net processing: Only call PushAddress() from `net_processing` (jnewbery)
- YukiCoin/YukiCoin#21198 Address outstanding review comments from PR20721 (jnewbery)
- YukiCoin/YukiCoin#21222 log: Clarify log message when file does not exist (MarcoFalke)
- YukiCoin/YukiCoin#21235 Clarify disconnect log message in ProcessGetBlockData, remove send bool (MarcoFalke)
- YukiCoin/YukiCoin#21236 Net processing: Extract `addr` send functionality into MaybeSendAddr() (jnewbery)
- YukiCoin/YukiCoin#21261 update inbound eviction protection for multiple networks, add I2P peers (jonatack)
- YukiCoin/YukiCoin#21328 net, refactor: pass uint16 CService::port as uint16 (jonatack)
- YukiCoin/YukiCoin#21387 Refactor sock to add I2P fuzz and unit tests (vasild)
- YukiCoin/YukiCoin#21395 Net processing: Remove unused CNodeState.address member (jnewbery)
- YukiCoin/YukiCoin#21407 i2p: limit the size of incoming messages (vasild)
- YukiCoin/YukiCoin#21506 p2p, refactor: make NetPermissionFlags an enum class (jonatack)
- YukiCoin/YukiCoin#21509 Don't send FEEFILTER in blocksonly mode (mzumsande)
- YukiCoin/YukiCoin#21560 Add Tor v3 hardcoded seeds (laanwj)
- YukiCoin/YukiCoin#21563 Restrict period when `cs_vNodes` mutex is locked (hebasto)
- YukiCoin/YukiCoin#21564 Avoid calling getnameinfo when formatting IPv4 addresses in CNetAddr::ToStringIP (practicalswift)
- YukiCoin/YukiCoin#21631 i2p: always check the return value of Sock::Wait() (vasild)
- YukiCoin/YukiCoin#21644 p2p, bugfix: use NetPermissions::HasFlag() in CConnman::Bind() (jonatack)
- YukiCoin/YukiCoin#21659 flag relevant Sock methods with [[nodiscard]] (vasild)
- YukiCoin/YukiCoin#21750 remove unnecessary check of `CNode::cs_vSend` (vasild)
- YukiCoin/YukiCoin#21756 Avoid calling `getnameinfo` when formatting IPv6 addresses in `CNetAddr::ToStringIP` (practicalswift)
- YukiCoin/YukiCoin#21775 Limit `m_block_inv_mutex` (MarcoFalke)
- YukiCoin/YukiCoin#21825 Add I2P hardcoded seeds (jonatack)
- YukiCoin/YukiCoin#21843 p2p, rpc: enable GetAddr, GetAddresses, and getnodeaddresses by network (jonatack)
- YukiCoin/YukiCoin#21845 net processing: Don't require locking `cs_main` before calling RelayTransactions() (jnewbery)
- YukiCoin/YukiCoin#21872 Sanitize message type for logging (laanwj)
- YukiCoin/YukiCoin#21914 Use stronger AddLocal() for our I2P address (vasild)
- YukiCoin/YukiCoin#21985 Return IPv6 scope id in `CNetAddr::ToStringIP()` (laanwj)
- YukiCoin/YukiCoin#21992 Remove -feefilter option (amadeuszpawlik)
- YukiCoin/YukiCoin#21996 Pass strings to NetPermissions::TryParse functions by const ref (jonatack)
- YukiCoin/YukiCoin#22013 ignore block-relay-only peers when skipping DNS seed (ajtowns)
- YukiCoin/YukiCoin#22050 Remove tor v2 support (jonatack)
- YukiCoin/YukiCoin#22096 AddrFetch - don't disconnect on self-announcements (mzumsande)
- YukiCoin/YukiCoin#22141 net processing: Remove hash and fValidatedHeaders from QueuedBlock (jnewbery)
- YukiCoin/YukiCoin#22144 Randomize message processing peer order (sipa)
- YukiCoin/YukiCoin#22147 Protect last outbound HB compact block peer (sdaftuar)
- YukiCoin/YukiCoin#22179 Torv2 removal followups (vasild)
- YukiCoin/YukiCoin#22211 Relay I2P addresses even if not reachable (by us) (vasild)
- YukiCoin/YukiCoin#22284 Performance improvements to ProtectEvictionCandidatesByRatio() (jonatack)
- YukiCoin/YukiCoin#22387 Rate limit the processing of rumoured addresses (sipa)
- YukiCoin/YukiCoin#22455 addrman: detect on-disk corrupted nNew and nTried during unserialization (vasild)

### Wallet
- YukiCoin/YukiCoin#15710 Catch `ios_base::failure` specifically (Bushstar)
- YukiCoin/YukiCoin#16546 External signer support - Wallet Box edition (Sjors)
- YukiCoin/YukiCoin#17331 Use effective values throughout coin selection (achow101)
- YukiCoin/YukiCoin#18418 Increase `OUTPUT_GROUP_MAX_ENTRIES` to 100 (fjahr)
- YukiCoin/YukiCoin#18842 Mark replaced tx to not be in the mempool anymore (MarcoFalke)
- YukiCoin/YukiCoin#19136 Add `parent_desc` to `getaddressinfo` (achow101)
- YukiCoin/YukiCoin#19137 wallettool: Add dump and createfromdump commands (achow101)
- YukiCoin/YukiCoin#19651 `importdescriptor`s update existing (S3RK)
- YukiCoin/YukiCoin#20040 Refactor OutputGroups to handle fees and spending eligibility on grouping (achow101)
- YukiCoin/YukiCoin#20202 Make BDB support optional (achow101)
- YukiCoin/YukiCoin#20226, YukiCoin/YukiCoin#21277, - YukiCoin/YukiCoin#21063 Add `listdescriptors` command (S3RK)
- YukiCoin/YukiCoin#20267 Disable and fix tests for when BDB is not compiled (achow101)
- YukiCoin/YukiCoin#20275 List all wallets in non-SQLite and non-BDB builds (ryanofsky)
- YukiCoin/YukiCoin#20365 wallettool: Add parameter to create descriptors wallet (S3RK)
- YukiCoin/YukiCoin#20403 `upgradewallet` fixes, improvements, test coverage (jonatack)
- YukiCoin/YukiCoin#20448 `unloadwallet`: Allow specifying `wallet_name` param matching RPC endpoint wallet (luke-jr)
- YukiCoin/YukiCoin#20536 Error with "Transaction too large" if the funded tx will end up being too large after signing (achow101)
- YukiCoin/YukiCoin#20687 Add missing check for -descriptors wallet tool option (MarcoFalke)
- YukiCoin/YukiCoin#20952 Add BerkeleyDB version sanity check at init time (laanwj)
- YukiCoin/YukiCoin#21127 Load flags before everything else (Sjors)
- YukiCoin/YukiCoin#21141 Add new format string placeholders for walletnotify (maayank)
- YukiCoin/YukiCoin#21238 A few descriptor improvements to prepare for Taproot support (sipa)
- YukiCoin/YukiCoin#21302 `createwallet` examples for descriptor wallets (S3RK)
- YukiCoin/YukiCoin#21329 descriptor wallet: Cache last hardened xpub and use in normalized descriptors (achow101)
- YukiCoin/YukiCoin#21365 Basic Taproot signing support for descriptor wallets (sipa)
- YukiCoin/YukiCoin#21417 Misc external signer improvement and HWI 2 support (Sjors)
- YukiCoin/YukiCoin#21467 Move external signer out of wallet module (Sjors)
- YukiCoin/YukiCoin#21572 Fix wrong wallet RPC context set after #21366 (ryanofsky)
- YukiCoin/YukiCoin#21574 Drop JSONRPCRequest constructors after #21366 (ryanofsky)
- YukiCoin/YukiCoin#21666 Miscellaneous external signer changes (fanquake)
- YukiCoin/YukiCoin#21759 Document coin selection code (glozow)
- YukiCoin/YukiCoin#21786 Ensure sat/vB feerates are in range (mantissa of 3) (jonatack)
- YukiCoin/YukiCoin#21944 Fix issues when `walletdir` is root directory (prayank23)
- YukiCoin/YukiCoin#22042 Replace size/weight estimate tuple with struct for named fields (instagibbs)
- YukiCoin/YukiCoin#22051 Basic Taproot derivation support for descriptors (sipa)
- YukiCoin/YukiCoin#22154 Add OutputType::BECH32M and related wallet support for fetching bech32m addresses (achow101)
- YukiCoin/YukiCoin#22156 Allow tr() import only when Taproot is active (achow101)
- YukiCoin/YukiCoin#22166 Add support for inferring tr() descriptors (sipa)
- YukiCoin/YukiCoin#22173 Do not load external signers wallets when unsupported (achow101)
- YukiCoin/YukiCoin#22308 Add missing BlockUntilSyncedToCurrentChain (MarcoFalke)
- YukiCoin/YukiCoin#22334 Do not spam about non-existent spk managers (S3RK)
- YukiCoin/YukiCoin#22379 Erase spkmans rather than setting to nullptr (achow101)
- YukiCoin/YukiCoin#22421 Make IsSegWitOutput return true for taproot outputs (sipa)
- YukiCoin/YukiCoin#22461 Change ScriptPubKeyMan::Upgrade default to True (achow101)
- YukiCoin/YukiCoin#22492 Reorder locks in dumpwallet to avoid lock order assertion (achow101)
- YukiCoin/YukiCoin#22686 Use GetSelectionAmount in ApproximateBestSubset (achow101)

### RPC and other APIs
- YukiCoin/YukiCoin#18335, YukiCoin/YukiCoin#21484 cli: Print useful error if YukiCoind rpc work queue exceeded (LarryRuane)
- YukiCoin/YukiCoin#18466 Fix invalid parameter error codes for `{sign,verify}message` RPCs (theStack)
- YukiCoin/YukiCoin#18772 Calculate fees in `getblock` using BlockUndo data (robot-visions)
- YukiCoin/YukiCoin#19033 http: Release work queue after event base finish (promag)
- YukiCoin/YukiCoin#19055 Add MuHash3072 implementation (fjahr)
- YukiCoin/YukiCoin#19145 Add `hash_type` MUHASH for gettxoutsetinfo (fjahr)
- YukiCoin/YukiCoin#19847 Avoid duplicate set lookup in `gettxoutproof` (promag)
- YukiCoin/YukiCoin#20286 Deprecate `addresses` and `reqSigs` from RPC outputs (mjdietzx)
- YukiCoin/YukiCoin#20459 Fail to return undocumented return values (MarcoFalke)
- YukiCoin/YukiCoin#20461 Validate `-rpcauth` arguments (promag)
- YukiCoin/YukiCoin#20556 Properly document return values (`submitblock`, `gettxout`, `getblocktemplate`, `scantxoutset`) (MarcoFalke)
- YukiCoin/YukiCoin#20755 Remove deprecated fields from `getpeerinfo` (amitiuttarwar)
- YukiCoin/YukiCoin#20832 Better error messages for invalid addresses (eilx2)
- YukiCoin/YukiCoin#20867 Support up to 20 keys for multisig under Segwit context (darosior)
- YukiCoin/YukiCoin#20877 cli: `-netinfo` user help and argument parsing improvements (jonatack)
- YukiCoin/YukiCoin#20891 Remove deprecated bumpfee behavior (achow101)
- YukiCoin/YukiCoin#20916 Return wtxid from `testmempoolaccept` (MarcoFalke)
- YukiCoin/YukiCoin#20917 Add missing signet mentions in network name lists (theStack)
- YukiCoin/YukiCoin#20941 Document `RPC_TRANSACTION_ALREADY_IN_CHAIN` exception (jarolrod)
- YukiCoin/YukiCoin#20944 Return total fee in `getmempoolinfo` (MarcoFalke)
- YukiCoin/YukiCoin#20964 Add specific error code for "wallet already loaded" (laanwj)
- YukiCoin/YukiCoin#21053 Document {previous,next}blockhash as optional (theStack)
- YukiCoin/YukiCoin#21056 Add a `-rpcwaittimeout` parameter to limit time spent waiting (cdecker)
- YukiCoin/YukiCoin#21192 cli: Treat high detail levels as maximum in `-netinfo` (laanwj)
- YukiCoin/YukiCoin#21311 Document optional fields for `getchaintxstats` result (theStack)
- YukiCoin/YukiCoin#21359 `include_unsafe` option for fundrawtransaction (t-bast)
- YukiCoin/YukiCoin#21426 Remove `scantxoutset` EXPERIMENTAL warning (jonatack)
- YukiCoin/YukiCoin#21544 Missing doc updates for bumpfee psbt update (MarcoFalke)
- YukiCoin/YukiCoin#21594 Add `network` field to `getnodeaddresses` (jonatack)
- YukiCoin/YukiCoin#21595, YukiCoin/YukiCoin#21753 cli: Create `-addrinfo` (jonatack)
- YukiCoin/YukiCoin#21602 Add additional ban time fields to `listbanned` (jarolrod)
- YukiCoin/YukiCoin#21679 Keep default argument value in correct type (promag)
- YukiCoin/YukiCoin#21718 Improve error message for `getblock` invalid datatype (klementtan)
- YukiCoin/YukiCoin#21913 RPCHelpMan fixes (kallewoof)
- YukiCoin/YukiCoin#22021 `bumpfee`/`psbtbumpfee` fixes and updates (jonatack)
- YukiCoin/YukiCoin#22043 `addpeeraddress` test coverage, code simplify/constness (jonatack)
- YukiCoin/YukiCoin#22327 cli: Avoid truncating `-rpcwaittimeout` (MarcoFalke)

### GUI
- YukiCoin/YukiCoin#18948 Call setParent() in the parent's context (hebasto)
- YukiCoin/YukiCoin#20482 Add depends qt fix for ARM macs (jonasschnelli)
- YukiCoin/YukiCoin#21836 scripted-diff: Replace three dots with ellipsis in the ui strings (hebasto)
- YukiCoin/YukiCoin#21935 Enable external signer support for GUI builds (Sjors)
- YukiCoin/YukiCoin#22133 Make QWindowsVistaStylePlugin available again (regression) (hebasto)
- YukiCoin-core/gui#4 UI external signer support (e.g. hardware wallet) (Sjors)
- YukiCoin-core/gui#13 Hide peer detail view if multiple are selected (promag)
- YukiCoin-core/gui#18 Add peertablesortproxy module (hebasto)
- YukiCoin-core/gui#21 Improve pruning tooltip (fluffypony, YukiCoinErrorLog)
- YukiCoin-core/gui#72 Log static plugins meta data and used style (hebasto)
- YukiCoin-core/gui#79 Embed monospaced font (hebasto)
- YukiCoin-core/gui#85 Remove unused "What's This" button in dialogs on Windows OS (hebasto)
- YukiCoin-core/gui#115 Replace "Hide tray icon" option with positive "Show tray icon" one (hebasto)
- YukiCoin-core/gui#118 Remove BDB version from the Information tab (hebasto)
- YukiCoin-core/gui#121 Early subscribe core signals in transaction table model (promag)
- YukiCoin-core/gui#123 Do not accept command while executing another one (hebasto)
- YukiCoin-core/gui#125 Enable changing the autoprune block space size in intro dialog (luke-jr)
- YukiCoin-core/gui#138 Unlock encrypted wallet "OK" button bugfix (mjdietzx)
- YukiCoin-core/gui#139 doc: Improve gui/src/qt README.md (jarolrod)
- YukiCoin-core/gui#154 Support macOS Dark mode (goums, Uplab)
- YukiCoin-core/gui#162 Add network to peers window and peer details (jonatack)
- YukiCoin-core/gui#163, YukiCoin-core/gui#180 Peer details: replace Direction with Connection Type (jonatack)
- YukiCoin-core/gui#164 Handle peer addition/removal in a right way (hebasto)
- YukiCoin-core/gui#165 Save QSplitter state in QSettings (hebasto)
- YukiCoin-core/gui#173 Follow Qt docs when implementing rowCount and columnCount (hebasto)
- YukiCoin-core/gui#179 Add Type column to peers window, update peer details name/tooltip (jonatack)
- YukiCoin-core/gui#186 Add information to "Confirm fee bump" window (prayank23)
- YukiCoin-core/gui#189 Drop workaround for QTBUG-42503 which was fixed in Qt 5.5.0 (prusnak)
- YukiCoin-core/gui#194 Save/restore RPCConsole geometry only for window (hebasto)
- YukiCoin-core/gui#202 Fix right panel toggle in peers tab (RandyMcMillan)
- YukiCoin-core/gui#203 Display plain "Inbound" in peer details (jonatack)
- YukiCoin-core/gui#204 Drop buggy TableViewLastColumnResizingFixer class (hebasto)
- YukiCoin-core/gui#205, YukiCoin-core/gui#229 Save/restore TransactionView and recentRequestsView tables column sizes (hebasto)
- YukiCoin-core/gui#206 Display fRelayTxes and `bip152_highbandwidth_{to, from}` in peer details (jonatack)
- YukiCoin-core/gui#213 Add Copy Address Action to Payment Requests (jarolrod)
- YukiCoin-core/gui#214 Disable requests context menu actions when appropriate (jarolrod)
- YukiCoin-core/gui#217 Make warning label look clickable (jarolrod)
- YukiCoin-core/gui#219 Prevent the main window popup menu (hebasto)
- YukiCoin-core/gui#220 Do not translate file extensions (hebasto)
- YukiCoin-core/gui#221 RPCConsole translatable string fixes and improvements (jonatack)
- YukiCoin-core/gui#226 Add "Last Block" and "Last Tx" rows to peer details area (jonatack)
- YukiCoin-core/gui#233 qt test: Don't bind to regtest port (achow101)
- YukiCoin-core/gui#243 Fix issue when disabling the auto-enabled blank wallet checkbox (jarolrod)
- YukiCoin-core/gui#246 Revert "qt: Use "fusion" style on macOS Big Sur with old Qt" (hebasto)
- YukiCoin-core/gui#248 For values of "Bytes transferred" and "Bytes/s" with 1000-based prefix names use 1000-based divisor instead of 1024-based (wodry)
- YukiCoin-core/gui#251 Improve URI/file handling message (hebasto)
- YukiCoin-core/gui#256 Save/restore column sizes of the tables in the Peers tab (hebasto)
- YukiCoin-core/gui#260 Handle exceptions isntead of crash (hebasto)
- YukiCoin-core/gui#263 Revamp context menus (hebasto)
- YukiCoin-core/gui#271 Don't clear console prompt when font resizing (jarolrod)
- YukiCoin-core/gui#275 Support runtime appearance adjustment on macOS (hebasto)
- YukiCoin-core/gui#276 Elide long strings in their middle in the Peers tab (hebasto)
- YukiCoin-core/gui#281 Set shortcuts for console's resize buttons (jarolrod)
- YukiCoin-core/gui#293 Enable wordWrap for Services (RandyMcMillan)
- YukiCoin-core/gui#296 Do not use QObject::tr plural syntax for numbers with a unit symbol (hebasto)
- YukiCoin-core/gui#297 Avoid unnecessary translations (hebasto)
- YukiCoin-core/gui#298 Peertableview alternating row colors (RandyMcMillan)
- YukiCoin-core/gui#300 Remove progress bar on modal overlay (brunoerg)
- YukiCoin-core/gui#309 Add access to the Peers tab from the network icon (hebasto)
- YukiCoin-core/gui#311 Peers Window rename 'Peer id' to 'Peer' (jarolrod)
- YukiCoin-core/gui#313 Optimize string concatenation by default (hebasto)
- YukiCoin-core/gui#325 Align numbers in the "Peer Id" column to the right (hebasto)
- YukiCoin-core/gui#329 Make console buttons look clickable (jarolrod)
- YukiCoin-core/gui#330 Allow prompt icon to be colorized (jarolrod)
- YukiCoin-core/gui#331 Make RPC console welcome message translation-friendly (hebasto)
- YukiCoin-core/gui#332 Replace disambiguation strings with translator comments (hebasto)
- YukiCoin-core/gui#335 test: Use QSignalSpy instead of QEventLoop (jarolrod)
- YukiCoin-core/gui#343 Improve the GUI responsiveness when progress dialogs are used (hebasto)
- YukiCoin-core/gui#361 Fix GUI segfault caused by YukiCoin/YukiCoin#22216 (ryanofsky)
- YukiCoin-core/gui#362 Add keyboard shortcuts to context menus (luke-jr)
- YukiCoin-core/gui#366 Dark Mode fixes/portability (luke-jr)
- YukiCoin-core/gui#375 Emit dataChanged signal to dynamically re-sort Peers table (hebasto)
- YukiCoin-core/gui#393 Fix regression in "Encrypt Wallet" menu item (hebasto)
- YukiCoin-core/gui#396 Ensure external signer option remains disabled without signers (achow101)
- YukiCoin-core/gui#406 Handle new added plurals in `YukiCoin_en.ts` (hebasto)

### Build system
- YukiCoin/YukiCoin#17227 Add Android packaging support (icota)
- YukiCoin/YukiCoin#17920 guix: Build support for macOS (dongcarl)
- YukiCoin/YukiCoin#18298 Fix Qt processing of configure script for depends with DEBUG=1 (hebasto)
- YukiCoin/YukiCoin#19160 multiprocess: Add basic spawn and IPC support (ryanofsky)
- YukiCoin/YukiCoin#19504 Bump minimum python version to 3.6 (ajtowns)
- YukiCoin/YukiCoin#19522 fix building libconsensus with reduced exports for Darwin targets (fanquake)
- YukiCoin/YukiCoin#19683 Pin clang search paths for darwin host (dongcarl)
- YukiCoin/YukiCoin#19764 Split boost into build/host packages + bump + cleanup (dongcarl)
- YukiCoin/YukiCoin#19817 libtapi 1100.0.11 (fanquake)
- YukiCoin/YukiCoin#19846 enable unused member function diagnostic (Zero-1729)
- YukiCoin/YukiCoin#19867 Document and cleanup Qt hacks (fanquake)
- YukiCoin/YukiCoin#20046 Set `CMAKE_INSTALL_RPATH` for native packages (ryanofsky)
- YukiCoin/YukiCoin#20223 Drop the leading 0 from the version number (achow101)
- YukiCoin/YukiCoin#20333 Remove `native_biplist` dependency (fanquake)
- YukiCoin/YukiCoin#20353 configure: Support -fdebug-prefix-map and -fmacro-prefix-map (ajtowns)
- YukiCoin/YukiCoin#20359 Various config.site.in improvements and linting (dongcarl)
- YukiCoin/YukiCoin#20413 Require C++17 compiler (MarcoFalke)
- YukiCoin/YukiCoin#20419 Set minimum supported macOS to 10.14 (fanquake)
- YukiCoin/YukiCoin#20421 miniupnpc 2.2.2 (fanquake)
- YukiCoin/YukiCoin#20422 Mac deployment unification (fanquake)
- YukiCoin/YukiCoin#20424 Update univalue subtree (MarcoFalke)
- YukiCoin/YukiCoin#20449 Fix Windows installer build (achow101)
- YukiCoin/YukiCoin#20468 Warn when generating man pages for binaries built from a dirty branch (tylerchambers)
- YukiCoin/YukiCoin#20469 Avoid secp256k1.h include from system (dergoegge)
- YukiCoin/YukiCoin#20470 Replace genisoimage with xorriso (dongcarl)
- YukiCoin/YukiCoin#20471 Use C++17 in depends (fanquake)
- YukiCoin/YukiCoin#20496 Drop unneeded macOS framework dependencies (hebasto)
- YukiCoin/YukiCoin#20520 Do not force Precompiled Headers (PCH) for building Qt on Linux (hebasto)
- YukiCoin/YukiCoin#20549 Support make src/YukiCoin-node and src/YukiCoin-gui (promag)
- YukiCoin/YukiCoin#20565 Ensure PIC build for bdb on Android (BlockMechanic)
- YukiCoin/YukiCoin#20594 Fix getauxval calls in randomenv.cpp (jonasschnelli)
- YukiCoin/YukiCoin#20603 Update crc32c subtree (MarcoFalke)
- YukiCoin/YukiCoin#20609 configure: output notice that test binary is disabled by fuzzing (apoelstra)
- YukiCoin/YukiCoin#20619 guix: Quality of life improvements (dongcarl)
- YukiCoin/YukiCoin#20629 Improve id string robustness (dongcarl)
- YukiCoin/YukiCoin#20641 Use Qt top-level build facilities (hebasto)
- YukiCoin/YukiCoin#20650 Drop workaround for a fixed bug in Qt build system (hebasto)
- YukiCoin/YukiCoin#20673 Use more legible qmake commands in qt package (hebasto)
- YukiCoin/YukiCoin#20684 Define .INTERMEDIATE target once only (hebasto)
- YukiCoin/YukiCoin#20720 more robustly check for fcf-protection support (fanquake)
- YukiCoin/YukiCoin#20734 Make platform-specific targets available for proper platform builds only (hebasto)
- YukiCoin/YukiCoin#20936 build fuzz tests by default (danben)
- YukiCoin/YukiCoin#20937 guix: Make nsis reproducible by respecting SOURCE-DATE-EPOCH (dongcarl)
- YukiCoin/YukiCoin#20938 fix linking against -latomic when building for riscv (fanquake)
- YukiCoin/YukiCoin#20939 fix `RELOC_SECTION` security check for YukiCoin-util (fanquake)
- YukiCoin/YukiCoin#20963 gitian-linux: Build binaries for 64-bit POWER (continued) (laanwj)
- YukiCoin/YukiCoin#21036 gitian: Bump descriptors to focal for 22.0 (fanquake)
- YukiCoin/YukiCoin#21045 Adds switch to enable/disable randomized base address in MSVC builds (EthanHeilman)
- YukiCoin/YukiCoin#21065 make macOS HOST in download-osx generic (fanquake)
- YukiCoin/YukiCoin#21078 guix: only download sources for hosts being built (fanquake)
- YukiCoin/YukiCoin#21116 Disable --disable-fuzz-binary for gitian/guix builds (hebasto)
- YukiCoin/YukiCoin#21182 remove mostly pointless `BOOST_PROCESS` macro (fanquake)
- YukiCoin/YukiCoin#21205 actually fail when Boost is missing (fanquake)
- YukiCoin/YukiCoin#21209 use newer source for libnatpmp (fanquake)
- YukiCoin/YukiCoin#21226 Fix fuzz binary compilation under windows (danben)
- YukiCoin/YukiCoin#21231 Add /opt/homebrew to path to look for boost libraries (fyquah)
- YukiCoin/YukiCoin#21239 guix: Add codesignature attachment support for osx+win (dongcarl)
- YukiCoin/YukiCoin#21250 Make `HAVE_O_CLOEXEC` available outside LevelDB (bugfix) (theStack)
- YukiCoin/YukiCoin#21272 guix: Passthrough `SDK_PATH` into container (dongcarl)
- YukiCoin/YukiCoin#21274 assumptions:  Assume C++17 (fanquake)
- YukiCoin/YukiCoin#21286 Bump minimum Qt version to 5.9.5 (hebasto)
- YukiCoin/YukiCoin#21298 guix: Bump time-machine, glibc, and linux-headers (dongcarl)
- YukiCoin/YukiCoin#21304 guix: Add guix-clean script + establish gc-root for container profiles (dongcarl)
- YukiCoin/YukiCoin#21320 fix libnatpmp macos cross compile (fanquake)
- YukiCoin/YukiCoin#21321 guix: Add curl to required tool list (hebasto)
- YukiCoin/YukiCoin#21333 set Unicode true for NSIS installer (fanquake)
- YukiCoin/YukiCoin#21339 Make `AM_CONDITIONAL([ENABLE_EXTERNAL_SIGNER])` unconditional (hebasto)
- YukiCoin/YukiCoin#21349 Fix fuzz-cuckoocache cross-compiling with DEBUG=1 (hebasto)
- YukiCoin/YukiCoin#21354 build, doc: Drop no longer required packages from macOS cross-compiling dependencies (hebasto)
- YukiCoin/YukiCoin#21363 build, qt: Improve Qt static plugins/libs check code (hebasto)
- YukiCoin/YukiCoin#21375 guix: Misc feedback-based fixes + hier restructuring (dongcarl)
- YukiCoin/YukiCoin#21376 Qt 5.12.10 (fanquake)
- YukiCoin/YukiCoin#21382 Clean remnants of QTBUG-34748 fix (hebasto)
- YukiCoin/YukiCoin#21400 Fix regression introduced in #21363 (hebasto)
- YukiCoin/YukiCoin#21403 set --build when configuring packages in depends (fanquake)
- YukiCoin/YukiCoin#21421 don't try and use -fstack-clash-protection on Windows (fanquake)
- YukiCoin/YukiCoin#21423 Cleanups and follow ups after bumping Qt to 5.12.10 (hebasto)
- YukiCoin/YukiCoin#21427 Fix `id_string` invocations (dongcarl)
- YukiCoin/YukiCoin#21430 Add -Werror=implicit-fallthrough compile flag (hebasto)
- YukiCoin/YukiCoin#21457 Split libtapi and clang out of `native_cctools` (fanquake)
- YukiCoin/YukiCoin#21462 guix: Add guix-{attest,verify} scripts (dongcarl)
- YukiCoin/YukiCoin#21495 build, qt: Fix static builds on macOS Big Sur (hebasto)
- YukiCoin/YukiCoin#21497 Do not opt-in unused CoreWLAN stuff in depends for macOS (hebasto)
- YukiCoin/YukiCoin#21543 Enable safe warnings for msvc builds (hebasto)
- YukiCoin/YukiCoin#21565 Make `YukiCoin_qt.m4` more generic (fanquake)
- YukiCoin/YukiCoin#21610 remove -Wdeprecated-register from NOWARN flags (fanquake)
- YukiCoin/YukiCoin#21613 enable -Wdocumentation (fanquake)
- YukiCoin/YukiCoin#21629 Fix configuring when building depends with `NO_BDB=1` (fanquake)
- YukiCoin/YukiCoin#21654 build, qt: Make Qt rcc output always deterministic (hebasto)
- YukiCoin/YukiCoin#21655 build, qt: No longer need to set `QT_RCC_TEST=1` for determinism (hebasto)
- YukiCoin/YukiCoin#21658 fix make deploy for arm64-darwin (sgulls)
- YukiCoin/YukiCoin#21694 Use XLIFF file to provide more context to Transifex translators (hebasto)
- YukiCoin/YukiCoin#21708, YukiCoin/YukiCoin#21593 Drop pointless sed commands (hebasto)
- YukiCoin/YukiCoin#21731 Update msvc build to use Qt5.12.10 binaries (sipsorcery)
- YukiCoin/YukiCoin#21733 Re-add command to install vcpkg (dplusplus1024)
- YukiCoin/YukiCoin#21793 Use `-isysroot` over `--sysroot` on macOS (fanquake)
- YukiCoin/YukiCoin#21869 Add missing `-D_LIBCPP_DEBUG=1` to debug flags (MarcoFalke)
- YukiCoin/YukiCoin#21889 macho: check for control flow instrumentation (fanquake)
- YukiCoin/YukiCoin#21920 Improve macro for testing -latomic requirement (MarcoFalke)
- YukiCoin/YukiCoin#21991 libevent 2.1.12-stable (fanquake)
- YukiCoin/YukiCoin#22054 Bump Qt version to 5.12.11 (hebasto)
- YukiCoin/YukiCoin#22063 Use Qt archive of the same version as the compiled binaries (hebasto)
- YukiCoin/YukiCoin#22070 Don't use cf-protection when targeting arm-apple-darwin (fanquake)
- YukiCoin/YukiCoin#22071 Latest config.guess and config.sub (fanquake)
- YukiCoin/YukiCoin#22075 guix: Misc leftover usability improvements (dongcarl)
- YukiCoin/YukiCoin#22123 Fix qt.mk for mac arm64 (promag)
- YukiCoin/YukiCoin#22174 build, qt: Fix libraries linking order for Linux hosts (hebasto)
- YukiCoin/YukiCoin#22182 guix: Overhaul how guix-{attest,verify} works and hierarchy (dongcarl)
- YukiCoin/YukiCoin#22186 build, qt: Fix compiling qt package in depends with GCC 11 (hebasto)
- YukiCoin/YukiCoin#22199 macdeploy: minor fixups and simplifications (fanquake)
- YukiCoin/YukiCoin#22230 Fix MSVC linker /SubSystem option for YukiCoin-qt.exe (hebasto)
- YukiCoin/YukiCoin#22234 Mark print-% target as phony (dgoncharov)
- YukiCoin/YukiCoin#22238 improve detection of eBPF support (fanquake)
- YukiCoin/YukiCoin#22258 Disable deprecated-copy warning only when external warnings are enabled (MarcoFalke)
- YukiCoin/YukiCoin#22320 set minimum required Boost to 1.64.0 (fanquake)
- YukiCoin/YukiCoin#22348 Fix cross build for Windows with Boost Process (hebasto)
- YukiCoin/YukiCoin#22365 guix: Avoid relying on newer symbols by rebasing our cross toolchains on older glibcs (dongcarl)
- YukiCoin/YukiCoin#22381 guix: Test security-check sanity before performing them (with macOS) (fanquake)
- YukiCoin/YukiCoin#22405 Remove --enable-glibc-back-compat from Guix build (fanquake)
- YukiCoin/YukiCoin#22406 Remove --enable-determinism configure option (fanquake)
- YukiCoin/YukiCoin#22410 Avoid GCC 7.1 ABI change warning in guix build (sipa)
- YukiCoin/YukiCoin#22436 use aarch64 Clang if cross-compiling for darwin on aarch64 (fanquake)
- YukiCoin/YukiCoin#22465 guix: Pin kernel-header version, time-machine to upstream 1.3.0 commit (dongcarl)
- YukiCoin/YukiCoin#22511 guix: Silence `getent(1)` invocation, doc fixups (dongcarl)
- YukiCoin/YukiCoin#22531 guix: Fixes to guix-{attest,verify} (achow101)
- YukiCoin/YukiCoin#22642 release: Release with separate sha256sums and sig files (dongcarl)
- YukiCoin/YukiCoin#22685 clientversion: No suffix `#if CLIENT_VERSION_IS_RELEASE` (dongcarl)
- YukiCoin/YukiCoin#22713 Fix build with Boost 1.77.0 (sizeofvoid)

### Tests and QA
- YukiCoin/YukiCoin#14604 Add test and refactor `feature_block.py` (sanket1729)
- YukiCoin/YukiCoin#17556 Change `feature_config_args.py` not to rely on strange regtest=0 behavior (ryanofsky)
- YukiCoin/YukiCoin#18795 wallet issue with orphaned rewards (domob1812)
- YukiCoin/YukiCoin#18847 compressor: Use a prevector in CompressScript serialization (jb55)
- YukiCoin/YukiCoin#19259 fuzz: Add fuzzing harness for LoadMempool(…) and DumpMempool(…) (practicalswift)
- YukiCoin/YukiCoin#19315 Allow outbound & block-relay-only connections in functional tests. (amitiuttarwar)
- YukiCoin/YukiCoin#19698 Apply strict verification flags for transaction tests and assert backwards compatibility (glozow)
- YukiCoin/YukiCoin#19801 Check for all possible `OP_CLTV` fail reasons in `feature_cltv.py` (BIP 65) (theStack)
- YukiCoin/YukiCoin#19893 Remove or explain syncwithvalidationinterfacequeue (MarcoFalke)
- YukiCoin/YukiCoin#19972 fuzz: Add fuzzing harness for node eviction logic (practicalswift)
- YukiCoin/YukiCoin#19982 Fix inconsistent lock order in `wallet_tests/CreateWallet` (hebasto)
- YukiCoin/YukiCoin#20000 Fix creation of "std::string"s with \0s (vasild)
- YukiCoin/YukiCoin#20047 Use `wait_for_{block,header}` helpers in `p2p_fingerprint.py` (theStack)
- YukiCoin/YukiCoin#20171 Add functional test `test_txid_inv_delay` (ariard)
- YukiCoin/YukiCoin#20189 Switch to BIP341's suggested scheme for outputs without script (sipa)
- YukiCoin/YukiCoin#20248 Fix length of R check in `key_signature_tests` (dgpv)
- YukiCoin/YukiCoin#20276, YukiCoin/YukiCoin#20385, YukiCoin/YukiCoin#20688, YukiCoin/YukiCoin#20692 Run various mempool tests even with wallet disabled (mjdietzx)
- YukiCoin/YukiCoin#20323 Create or use existing properly initialized NodeContexts (dongcarl)
- YukiCoin/YukiCoin#20354 Add `feature_taproot.py --previous_release` (MarcoFalke)
- YukiCoin/YukiCoin#20370 fuzz: Version handshake (MarcoFalke)
- YukiCoin/YukiCoin#20377 fuzz: Fill various small fuzzing gaps (practicalswift)
- YukiCoin/YukiCoin#20425 fuzz: Make CAddrMan fuzzing harness deterministic (practicalswift)
- YukiCoin/YukiCoin#20430 Sanitizers: Add suppression for unsigned-integer-overflow in libstdc++ (jonasschnelli)
- YukiCoin/YukiCoin#20437 fuzz: Avoid time-based "non-determinism" in fuzzing harnesses by using mocked GetTime() (practicalswift)
- YukiCoin/YukiCoin#20458 Add `is_bdb_compiled` helper (Sjors)
- YukiCoin/YukiCoin#20466 Fix intermittent `p2p_fingerprint` issue (MarcoFalke)
- YukiCoin/YukiCoin#20472 Add testing of ParseInt/ParseUInt edge cases with leading +/-/0:s (practicalswift)
- YukiCoin/YukiCoin#20507 sync: print proper lock order location when double lock is detected (vasild)
- YukiCoin/YukiCoin#20522 Fix sync issue in `disconnect_p2ps` (amitiuttarwar)
- YukiCoin/YukiCoin#20524 Move `MIN_VERSION_SUPPORTED` to p2p.py (jnewbery)
- YukiCoin/YukiCoin#20540 Fix `wallet_multiwallet` issue on windows (MarcoFalke)
- YukiCoin/YukiCoin#20560 fuzz: Link all targets once (MarcoFalke)
- YukiCoin/YukiCoin#20567 Add option to git-subtree-check to do full check, add help (laanwj)
- YukiCoin/YukiCoin#20569 Fix intermittent `wallet_multiwallet` issue with `got_loading_error` (MarcoFalke)
- YukiCoin/YukiCoin#20613 Use Popen.wait instead of RPC in `assert_start_raises_init_error` (MarcoFalke)
- YukiCoin/YukiCoin#20663 fuzz: Hide `script_assets_test_minimizer` (MarcoFalke)
- YukiCoin/YukiCoin#20674 fuzz: Call SendMessages after ProcessMessage to increase coverage (MarcoFalke)
- YukiCoin/YukiCoin#20683 Fix restart node race (MarcoFalke)
- YukiCoin/YukiCoin#20686 fuzz: replace CNode code with fuzz/util.h::ConsumeNode() (jonatack)
- YukiCoin/YukiCoin#20733 Inline non-member functions with body in fuzzing headers (pstratem)
- YukiCoin/YukiCoin#20737 Add missing assignment in `mempool_resurrect.py` (MarcoFalke)
- YukiCoin/YukiCoin#20745 Correct `epoll_ctl` data race suppression (hebasto)
- YukiCoin/YukiCoin#20748 Add race:SendZmqMessage tsan suppression (MarcoFalke)
- YukiCoin/YukiCoin#20760 Set correct nValue for multi-op-return policy check (MarcoFalke)
- YukiCoin/YukiCoin#20761 fuzz: Check that `NULL_DATA` is unspendable (MarcoFalke)
- YukiCoin/YukiCoin#20765 fuzz: Check that certain script TxoutType are nonstandard (mjdietzx)
- YukiCoin/YukiCoin#20772 fuzz: Bolster ExtractDestination(s) checks (mjdietzx)
- YukiCoin/YukiCoin#20789 fuzz: Rework strong and weak net enum fuzzing (MarcoFalke)
- YukiCoin/YukiCoin#20828 fuzz: Introduce CallOneOf helper to replace switch-case (MarcoFalke)
- YukiCoin/YukiCoin#20839 fuzz: Avoid extraneous copy of input data, using Span<> (MarcoFalke)
- YukiCoin/YukiCoin#20844 Add sanitizer suppressions for AMD EPYC CPUs (MarcoFalke)
- YukiCoin/YukiCoin#20857 Update documentation in `feature_csv_activation.py` (PiRK)
- YukiCoin/YukiCoin#20876 Replace getmempoolentry with testmempoolaccept in MiniWallet (MarcoFalke)
- YukiCoin/YukiCoin#20881 fuzz: net permission flags in net processing (MarcoFalke)
- YukiCoin/YukiCoin#20882 fuzz: Add missing muhash registration (MarcoFalke)
- YukiCoin/YukiCoin#20908 fuzz: Use mocktime in `process_message*` fuzz targets (MarcoFalke)
- YukiCoin/YukiCoin#20915 fuzz: Fail if message type is not fuzzed (MarcoFalke)
- YukiCoin/YukiCoin#20946 fuzz: Consolidate fuzzing TestingSetup initialization (dongcarl)
- YukiCoin/YukiCoin#20954 Declare `nodes` type `in test_framework.py` (kiminuo)
- YukiCoin/YukiCoin#20955 Fix `get_previous_releases.py` for aarch64 (MarcoFalke)
- YukiCoin/YukiCoin#20969 check that getblockfilter RPC fails without block filter index (theStack)
- YukiCoin/YukiCoin#20971 Work around libFuzzer deadlock (MarcoFalke)
- YukiCoin/YukiCoin#20993 Store subversion (user agent) as string in `msg_version` (theStack)
- YukiCoin/YukiCoin#20995 fuzz: Avoid initializing version to less than `MIN_PEER_PROTO_VERSION` (MarcoFalke)
- YukiCoin/YukiCoin#20998 Fix BlockToJsonVerbose benchmark (martinus)
- YukiCoin/YukiCoin#21003 Move MakeNoLogFileContext to `libtest_util`, and use it in bench (MarcoFalke)
- YukiCoin/YukiCoin#21008 Fix zmq test flakiness, improve speed (theStack)
- YukiCoin/YukiCoin#21023 fuzz: Disable shuffle when merge=1 (MarcoFalke)
- YukiCoin/YukiCoin#21037 fuzz: Avoid designated initialization (C++20) in fuzz tests (practicalswift)
- YukiCoin/YukiCoin#21042 doc, test: Improve `setup_clean_chain` documentation (fjahr)
- YukiCoin/YukiCoin#21080 fuzz: Configure check for main function (take 2) (MarcoFalke)
- YukiCoin/YukiCoin#21084 Fix timeout decrease in `feature_assumevalid` (brunoerg)
- YukiCoin/YukiCoin#21096 Re-add dead code detection (flack)
- YukiCoin/YukiCoin#21100 Remove unused function `xor_bytes` (theStack)
- YukiCoin/YukiCoin#21115 Fix Windows cross build (hebasto)
- YukiCoin/YukiCoin#21117 Remove `assert_blockchain_height` (MarcoFalke)
- YukiCoin/YukiCoin#21121 Small unit test improvements, including helper to make mempool transaction (amitiuttarwar)
- YukiCoin/YukiCoin#21124 Remove unnecessary assignment in bdb (brunoerg)
- YukiCoin/YukiCoin#21125 Change `BOOST_CHECK` to `BOOST_CHECK_EQUAL` for paths (kiminuo)
- YukiCoin/YukiCoin#21142, YukiCoin/YukiCoin#21512 fuzz: Add `tx_pool` fuzz target (MarcoFalke)
- YukiCoin/YukiCoin#21165 Use mocktime in `test_seed_peers` (dhruv)
- YukiCoin/YukiCoin#21169 fuzz: Add RPC interface fuzzing. Increase fuzzing coverage from 65% to 70% (practicalswift)
- YukiCoin/YukiCoin#21170 bench: Add benchmark to write json into a string (martinus)
- YukiCoin/YukiCoin#21178 Run `mempool_reorg.py` even with wallet disabled (DariusParvin)
- YukiCoin/YukiCoin#21185 fuzz: Remove expensive and redundant muhash from crypto fuzz target (MarcoFalke)
- YukiCoin/YukiCoin#21200 Speed up `rpc_blockchain.py` by removing miniwallet.generate() (MarcoFalke)
- YukiCoin/YukiCoin#21211 Move `P2WSH_OP_TRUE` to shared test library (MarcoFalke)
- YukiCoin/YukiCoin#21228 Avoid comparision of integers with different signs (jonasschnelli)
- YukiCoin/YukiCoin#21230 Fix `NODE_NETWORK_LIMITED_MIN_BLOCKS` disconnection (MarcoFalke)
- YukiCoin/YukiCoin#21252 Add missing wait for sync to `feature_blockfilterindex_prune` (MarcoFalke)
- YukiCoin/YukiCoin#21254 Avoid connecting to real network when running tests (MarcoFalke)
- YukiCoin/YukiCoin#21264 fuzz: Two scripted diff renames (MarcoFalke)
- YukiCoin/YukiCoin#21280 Bug fix in `transaction_tests` (glozow)
- YukiCoin/YukiCoin#21293 Replace accidentally placed bit-OR with logical-OR (hebasto)
- YukiCoin/YukiCoin#21297 `feature_blockfilterindex_prune.py` improvements (jonatack)
- YukiCoin/YukiCoin#21310 zmq test: fix sync-up by matching notification to generated block (theStack)
- YukiCoin/YukiCoin#21334 Additional BIP9 tests (Sjors)
- YukiCoin/YukiCoin#21338 Add functional test for anchors.dat (brunoerg)
- YukiCoin/YukiCoin#21345 Bring `p2p_leak.py` up to date (mzumsande)
- YukiCoin/YukiCoin#21357 Unconditionally check for fRelay field in test framework (jarolrod)
- YukiCoin/YukiCoin#21358 fuzz: Add missing include (`test/util/setup_common.h`) (MarcoFalke)
- YukiCoin/YukiCoin#21371 fuzz: fix gcc Woverloaded-virtual build warnings (jonatack)
- YukiCoin/YukiCoin#21373 Generate fewer blocks in `feature_nulldummy` to fix timeouts, speed up (jonatack)
- YukiCoin/YukiCoin#21390 Test improvements for UTXO set hash tests (fjahr)
- YukiCoin/YukiCoin#21410 increase `rpc_timeout` for fundrawtx `test_transaction_too_large` (jonatack)
- YukiCoin/YukiCoin#21411 add logging, reduce blocks, move `sync_all` in `wallet_` groups (jonatack)
- YukiCoin/YukiCoin#21438 Add ParseUInt8() test coverage (jonatack)
- YukiCoin/YukiCoin#21443 fuzz: Implement `fuzzed_dns_lookup_function` as a lambda (practicalswift)
- YukiCoin/YukiCoin#21445 cirrus: Use SSD cluster for speedup (MarcoFalke)
- YukiCoin/YukiCoin#21477 Add test for CNetAddr::ToString IPv6 address formatting (RFC 5952) (practicalswift)
- YukiCoin/YukiCoin#21487 fuzz: Use ConsumeWeakEnum in addrman for service flags (MarcoFalke)
- YukiCoin/YukiCoin#21488 Add ParseUInt16() unit test and fuzz coverage (jonatack)
- YukiCoin/YukiCoin#21491 test: remove duplicate assertions in util_tests (jonatack)
- YukiCoin/YukiCoin#21522 fuzz: Use PickValue where possible (MarcoFalke)
- YukiCoin/YukiCoin#21531 remove qt byteswap compattests (fanquake)
- YukiCoin/YukiCoin#21557 small cleanup in RPCNestedTests tests (fanquake)
- YukiCoin/YukiCoin#21586 Add missing suppression for signed-integer-overflow:txmempool.cpp (MarcoFalke)
- YukiCoin/YukiCoin#21592 Remove option to make TestChain100Setup non-deterministic (MarcoFalke)
- YukiCoin/YukiCoin#21597 Document `race:validation_chainstatemanager_tests` suppression (MarcoFalke)
- YukiCoin/YukiCoin#21599 Replace file level integer overflow suppression with function level suppression (practicalswift)
- YukiCoin/YukiCoin#21604 Document why no symbol names can be used for suppressions (MarcoFalke)
- YukiCoin/YukiCoin#21606 fuzz: Extend psbt fuzz target a bit (MarcoFalke)
- YukiCoin/YukiCoin#21617 fuzz: Fix uninitialized read in i2p test (MarcoFalke)
- YukiCoin/YukiCoin#21630 fuzz: split FuzzedSock interface and implementation (vasild)
- YukiCoin/YukiCoin#21634 Skip SQLite fsyncs while testing (achow101)
- YukiCoin/YukiCoin#21669 Remove spurious double lock tsan suppressions by bumping to clang-12 (MarcoFalke)
- YukiCoin/YukiCoin#21676 Use mocktime to avoid intermittent failure in `rpc_tests` (MarcoFalke)
- YukiCoin/YukiCoin#21677 fuzz: Avoid use of low file descriptor ids (which may be in use) in FuzzedSock (practicalswift)
- YukiCoin/YukiCoin#21678 Fix TestPotentialDeadLockDetected suppression (hebasto)
- YukiCoin/YukiCoin#21689 Remove intermittently failing and not very meaningful `BOOST_CHECK` in `cnetaddr_basic` (practicalswift)
- YukiCoin/YukiCoin#21691 Check that no versionbits are re-used (MarcoFalke)
- YukiCoin/YukiCoin#21707 Extend functional tests for addr relay (mzumsande)
- YukiCoin/YukiCoin#21712 Test default `include_mempool` value of gettxout (promag)
- YukiCoin/YukiCoin#21738 Use clang-12 for ASAN, Add missing suppression (MarcoFalke)
- YukiCoin/YukiCoin#21740 add new python linter to check file names and permissions (windsok)
- YukiCoin/YukiCoin#21749 Bump shellcheck version (hebasto)
- YukiCoin/YukiCoin#21754 Run `feature_cltv` with MiniWallet (MarcoFalke)
- YukiCoin/YukiCoin#21762 Speed up `mempool_spend_coinbase.py` (MarcoFalke)
- YukiCoin/YukiCoin#21773 fuzz: Ensure prevout is consensus-valid (MarcoFalke)
- YukiCoin/YukiCoin#21777 Fix `feature_notifications.py` intermittent issue (MarcoFalke)
- YukiCoin/YukiCoin#21785 Fix intermittent issue in `p2p_addr_relay.py` (MarcoFalke)
- YukiCoin/YukiCoin#21787 Fix off-by-ones in `rpc_fundrawtransaction` assertions (jonatack)
- YukiCoin/YukiCoin#21792 Fix intermittent issue in `p2p_segwit.py` (MarcoFalke)
- YukiCoin/YukiCoin#21795 fuzz: Terminate immediately if a fuzzing harness tries to perform a DNS lookup (belt and suspenders) (practicalswift)
- YukiCoin/YukiCoin#21798 fuzz: Create a block template in `tx_pool` targets (MarcoFalke)
- YukiCoin/YukiCoin#21804 Speed up `p2p_segwit.py` (jnewbery)
- YukiCoin/YukiCoin#21810 fuzz: Various RPC fuzzer follow-ups (practicalswift)
- YukiCoin/YukiCoin#21814 Fix `feature_config_args.py` intermittent issue (MarcoFalke)
- YukiCoin/YukiCoin#21821 Add missing test for empty P2WSH redeem (MarcoFalke)
- YukiCoin/YukiCoin#21822 Resolve bug in `interface_YukiCoin_cli.py` (klementtan)
- YukiCoin/YukiCoin#21846 fuzz: Add `-fsanitize=integer` suppression needed for RPC fuzzer (`generateblock`) (practicalswift)
- YukiCoin/YukiCoin#21849 fuzz: Limit toxic test globals to their respective scope (MarcoFalke)
- YukiCoin/YukiCoin#21867 use MiniWallet for `p2p_blocksonly.py` (theStack)
- YukiCoin/YukiCoin#21873 minor fixes & improvements for files linter test (windsok)
- YukiCoin/YukiCoin#21874 fuzz: Add `WRITE_ALL_FUZZ_TARGETS_AND_ABORT` (MarcoFalke)
- YukiCoin/YukiCoin#21884 fuzz: Remove unused --enable-danger-fuzz-link-all option (MarcoFalke)
- YukiCoin/YukiCoin#21890 fuzz: Limit ParseISO8601DateTime fuzzing to 32-bit (MarcoFalke)
- YukiCoin/YukiCoin#21891 fuzz: Remove strprintf test cases that are known to fail (MarcoFalke)
- YukiCoin/YukiCoin#21892 fuzz: Avoid excessively large min fee rate in `tx_pool` (MarcoFalke)
- YukiCoin/YukiCoin#21895 Add TSA annotations to the WorkQueue class members (hebasto)
- YukiCoin/YukiCoin#21900 use MiniWallet for `feature_csv_activation.py` (theStack)
- YukiCoin/YukiCoin#21909 fuzz: Limit max insertions in timedata fuzz test (MarcoFalke)
- YukiCoin/YukiCoin#21922 fuzz: Avoid timeout in EncodeBase58 (MarcoFalke)
- YukiCoin/YukiCoin#21927 fuzz: Run const CScript member functions only once (MarcoFalke)
- YukiCoin/YukiCoin#21929 fuzz: Remove incorrect float round-trip serialization test (MarcoFalke)
- YukiCoin/YukiCoin#21936 fuzz: Terminate immediately if a fuzzing harness tries to create a TCP socket (belt and suspenders) (practicalswift)
- YukiCoin/YukiCoin#21941 fuzz: Call const member functions in addrman fuzz test only once (MarcoFalke)
- YukiCoin/YukiCoin#21945 add P2PK support to MiniWallet (theStack)
- YukiCoin/YukiCoin#21948 Fix off-by-one in mockscheduler test RPC (MarcoFalke)
- YukiCoin/YukiCoin#21953 fuzz: Add `utxo_snapshot` target (MarcoFalke)
- YukiCoin/YukiCoin#21970 fuzz: Add missing CheckTransaction before CheckTxInputs (MarcoFalke)
- YukiCoin/YukiCoin#21989 Use `COINBASE_MATURITY` in functional tests (kiminuo)
- YukiCoin/YukiCoin#22003 Add thread safety annotations (ajtowns)
- YukiCoin/YukiCoin#22004 fuzz: Speed up transaction fuzz target (MarcoFalke)
- YukiCoin/YukiCoin#22005 fuzz: Speed up banman fuzz target (MarcoFalke)
- YukiCoin/YukiCoin#22029 [fuzz] Improve transport deserialization fuzz test coverage (dhruv)
- YukiCoin/YukiCoin#22048 MiniWallet: introduce enum type for output mode (theStack)
- YukiCoin/YukiCoin#22057 use MiniWallet (P2PK mode) for `feature_dersig.py` (theStack)
- YukiCoin/YukiCoin#22065 Mark `CheckTxInputs` `[[nodiscard]]`. Avoid UUM in fuzzing harness `coins_view` (practicalswift)
- YukiCoin/YukiCoin#22069 fuzz: don't try and use fopencookie() when building for Android (fanquake)
- YukiCoin/YukiCoin#22082 update nanobench from release 4.0.0 to 4.3.4 (martinus)
- YukiCoin/YukiCoin#22086 remove BasicTestingSetup from unit tests that don't need it (fanquake)
- YukiCoin/YukiCoin#22089 MiniWallet: fix fee calculation for P2PK and check tx vsize (theStack)
- YukiCoin/YukiCoin#21107, YukiCoin/YukiCoin#22092 Convert documentation into type annotations (fanquake)
- YukiCoin/YukiCoin#22095 Additional BIP32 test vector for hardened derivation with leading zeros (kristapsk)
- YukiCoin/YukiCoin#22103 Fix IPv6 check on BSD systems (n-thumann)
- YukiCoin/YukiCoin#22118 check anchors.dat when node starts for the first time (brunoerg)
- YukiCoin/YukiCoin#22120 `p2p_invalid_block`: Check that a block rejected due to too-new tim… (willcl-ark)
- YukiCoin/YukiCoin#22153 Fix `p2p_leak.py` intermittent failure (mzumsande)
- YukiCoin/YukiCoin#22169 p2p, rpc, fuzz: various tiny follow-ups (jonatack)
- YukiCoin/YukiCoin#22176 Correct outstanding -Werror=sign-compare errors (Empact)
- YukiCoin/YukiCoin#22180 fuzz: Increase branch coverage of the float fuzz target (MarcoFalke)
- YukiCoin/YukiCoin#22187 Add `sync_blocks` in `wallet_orphanedreward.py` (domob1812)
- YukiCoin/YukiCoin#22201 Fix TestShell to allow running in Jupyter Notebook (josibake)
- YukiCoin/YukiCoin#22202 Add temporary coinstats suppressions (MarcoFalke)
- YukiCoin/YukiCoin#22203 Use ConnmanTestMsg from test lib in `denialofservice_tests` (MarcoFalke)
- YukiCoin/YukiCoin#22210 Use MiniWallet in `test_no_inherited_signaling` RBF test (MarcoFalke)
- YukiCoin/YukiCoin#22224 Update msvc and appveyor builds to use Qt5.12.11 binaries (sipsorcery)
- YukiCoin/YukiCoin#22249 Kill process group to avoid dangling processes when using `--failfast` (S3RK)
- YukiCoin/YukiCoin#22267 fuzz: Speed up crypto fuzz target (MarcoFalke)
- YukiCoin/YukiCoin#22270 Add YukiCoin-util tests (+refactors) (MarcoFalke)
- YukiCoin/YukiCoin#22271 fuzz: Assert roundtrip equality for `CPubKey` (theStack)
- YukiCoin/YukiCoin#22279 fuzz: add missing ECCVerifyHandle to `base_encode_decode` (apoelstra)
- YukiCoin/YukiCoin#22292 bench, doc: benchmarking updates and fixups (jonatack)
- YukiCoin/YukiCoin#22306 Improvements to `p2p_addr_relay.py` (amitiuttarwar)
- YukiCoin/YukiCoin#22310 Add functional test for replacement relay fee check (ariard)
- YukiCoin/YukiCoin#22311 Add missing syncwithvalidationinterfacequeue in `p2p_blockfilters` (MarcoFalke)
- YukiCoin/YukiCoin#22313 Add missing `sync_all` to `feature_coinstatsindex` (MarcoFalke)
- YukiCoin/YukiCoin#22322 fuzz: Check banman roundtrip (MarcoFalke)
- YukiCoin/YukiCoin#22363 Use `script_util` helpers for creating P2{PKH,SH,WPKH,WSH} scripts (theStack)
- YukiCoin/YukiCoin#22399 fuzz: Rework CTxDestination fuzzing (MarcoFalke)
- YukiCoin/YukiCoin#22408 add tests for `bad-txns-prevout-null` reject reason (theStack)
- YukiCoin/YukiCoin#22445 fuzz: Move implementations of non-template fuzz helpers from util.h to util.cpp (sriramdvt)
- YukiCoin/YukiCoin#22446 Fix `wallet_listdescriptors.py` if bdb is not compiled (hebasto)
- YukiCoin/YukiCoin#22447 Whitelist `rpc_rawtransaction` peers to speed up tests (jonatack)
- YukiCoin/YukiCoin#22742 Use proper target in `do_fund_send` (S3RK)

### Miscellaneous
- YukiCoin/YukiCoin#19337 sync: Detect double lock from the same thread (vasild)
- YukiCoin/YukiCoin#19809 log: Prefix log messages with function name and source code location if -logsourcelocations is set (practicalswift)
- YukiCoin/YukiCoin#19866 eBPF Linux tracepoints (jb55)
- YukiCoin/YukiCoin#20024 init: Fix incorrect warning "Reducing -maxconnections from N to N-1, because of system limitations" (practicalswift)
- YukiCoin/YukiCoin#20145 contrib: Add getcoins.py script to get coins from (signet) faucet (kallewoof)
- YukiCoin/YukiCoin#20255 util: Add assume() identity function (MarcoFalke)
- YukiCoin/YukiCoin#20288 script, doc: Contrib/seeds updates (jonatack)
- YukiCoin/YukiCoin#20358 src/randomenv.cpp: Fix build on uclibc (ffontaine)
- YukiCoin/YukiCoin#20406 util: Avoid invalid integer negation in formatmoney and valuefromamount (practicalswift)
- YukiCoin/YukiCoin#20434 contrib: Parse elf directly for symbol and security checks (laanwj)
- YukiCoin/YukiCoin#20451 lint: Run mypy over contrib/devtools (fanquake)
- YukiCoin/YukiCoin#20476 contrib: Add test for elf symbol-check (laanwj)
- YukiCoin/YukiCoin#20530 lint: Update cppcheck linter to c++17 and improve explicit usage (fjahr)
- YukiCoin/YukiCoin#20589 log: Clarify that failure to read/write `fee_estimates.dat` is non-fatal (MarcoFalke)
- YukiCoin/YukiCoin#20602 util: Allow use of c++14 chrono literals (MarcoFalke)
- YukiCoin/YukiCoin#20605 init: Signal-safe instant shutdown (laanwj)
- YukiCoin/YukiCoin#20608 contrib: Add symbol check test for PE binaries (fanquake)
- YukiCoin/YukiCoin#20689 contrib: Replace binary verification script verify.sh with python rewrite (theStack)
- YukiCoin/YukiCoin#20715 util: Add argsmanager::getcommand() and use it in YukiCoin-wallet (MarcoFalke)
- YukiCoin/YukiCoin#20735 script: Remove outdated extract-osx-sdk.sh (hebasto)
- YukiCoin/YukiCoin#20817 lint: Update list of spelling linter false positives, bump to codespell 2.0.0 (theStack)
- YukiCoin/YukiCoin#20884 script: Improve robustness of YukiCoind.service on startup (hebasto)
- YukiCoin/YukiCoin#20906 contrib: Embed c++11 patch in `install_db4.sh` (gruve-p)
- YukiCoin/YukiCoin#21004 contrib: Fix docker args conditional in gitian-build (setpill)
- YukiCoin/YukiCoin#21007 YukiCoind: Add -daemonwait option to wait for initialization (laanwj)
- YukiCoin/YukiCoin#21041 log: Move "Pre-allocating up to position 0x[…] in […].dat" log message to debug category (practicalswift)
- YukiCoin/YukiCoin#21059 Drop boost/preprocessor dependencies (hebasto)
- YukiCoin/YukiCoin#21087 guix: Passthrough `BASE_CACHE` into container (dongcarl)
- YukiCoin/YukiCoin#21088 guix: Jump forwards in time-machine and adapt (dongcarl)
- YukiCoin/YukiCoin#21089 guix: Add support for powerpc64{,le} (dongcarl)
- YukiCoin/YukiCoin#21110 util: Remove boost `posix_time` usage from `gettime*` (fanquake)
- YukiCoin/YukiCoin#21111 Improve OpenRC initscript (parazyd)
- YukiCoin/YukiCoin#21123 code style: Add EditorConfig file (kiminuo)
- YukiCoin/YukiCoin#21173 util: Faster hexstr => 13% faster blocktojson (martinus)
- YukiCoin/YukiCoin#21221 tools: Allow argument/parameter bin packing in clang-format (jnewbery)
- YukiCoin/YukiCoin#21244 Move GetDataDir to ArgsManager (kiminuo)
- YukiCoin/YukiCoin#21255 contrib: Run test-symbol-check for risc-v (fanquake)
- YukiCoin/YukiCoin#21271 guix: Explicitly set umask in build container (dongcarl)
- YukiCoin/YukiCoin#21300 script: Add explanatory comment to tc.sh (dscotese)
- YukiCoin/YukiCoin#21317 util: Make assume() usable as unary expression (MarcoFalke)
- YukiCoin/YukiCoin#21336 Make .gitignore ignore src/test/fuzz/fuzz.exe (hebasto)
- YukiCoin/YukiCoin#21337 guix: Update darwin native packages dependencies (hebasto)
- YukiCoin/YukiCoin#21405 compat: remove memcpy -> memmove backwards compatibility alias (fanquake)
- YukiCoin/YukiCoin#21418 contrib: Make systemd invoke dependencies only when ready (laanwj)
- YukiCoin/YukiCoin#21447 Always add -daemonwait to known command line arguments (hebasto)
- YukiCoin/YukiCoin#21471 bugfix: Fix `bech32_encode` calls in `gen_key_io_test_vectors.py` (sipa)
- YukiCoin/YukiCoin#21615 script: Add trusted key for hebasto (hebasto)
- YukiCoin/YukiCoin#21664 contrib: Use lief for macos and windows symbol & security checks (fanquake)
- YukiCoin/YukiCoin#21695 contrib: Remove no longer used contrib/YukiCoin-qt.pro (hebasto)
- YukiCoin/YukiCoin#21711 guix: Add full installation and usage documentation (dongcarl)
- YukiCoin/YukiCoin#21799 guix: Use `gcc-8` across the board (dongcarl)
- YukiCoin/YukiCoin#21802 Avoid UB in util/asmap (advance a dereferenceable iterator outside its valid range) (MarcoFalke)
- YukiCoin/YukiCoin#21823 script: Update reviewers (jonatack)
- YukiCoin/YukiCoin#21850 Remove `GetDataDir(net_specific)` function (kiminuo)
- YukiCoin/YukiCoin#21871 scripts: Add checks for minimum required os versions (fanquake)
- YukiCoin/YukiCoin#21966 Remove double serialization; use software encoder for fee estimation (sipa)
- YukiCoin/YukiCoin#22060 contrib: Add torv3 seed nodes for testnet, drop v2 ones (laanwj)
- YukiCoin/YukiCoin#22244 devtools: Correctly extract symbol versions in symbol-check (laanwj)
- YukiCoin/YukiCoin#22533 guix/build: Remove vestigial SKIPATTEST.TAG (dongcarl)
- YukiCoin/YukiCoin#22643 guix-verify: Non-zero exit code when anything fails (dongcarl)
- YukiCoin/YukiCoin#22654 guix: Don't include directory name in SHA256SUMS (achow101)

### Documentation
- YukiCoin/YukiCoin#15451 clarify getdata limit after #14897 (HashUnlimited)
- YukiCoin/YukiCoin#15545 Explain why CheckBlock() is called before AcceptBlock (Sjors)
- YukiCoin/YukiCoin#17350 Add developer documentation to isminetype (HAOYUatHZ)
- YukiCoin/YukiCoin#17934 Use `CONFIG_SITE` variable instead of --prefix option (hebasto)
- YukiCoin/YukiCoin#18030 Coin::IsSpent() can also mean never existed (Sjors)
- YukiCoin/YukiCoin#18096 IsFinalTx comment about nSequence & `OP_CLTV` (nothingmuch)
- YukiCoin/YukiCoin#18568 Clarify developer notes about constant naming (ryanofsky)
- YukiCoin/YukiCoin#19961 doc: tor.md updates (jonatack)
- YukiCoin/YukiCoin#19968 Clarify CRollingBloomFilter size estimate (robot-dreams)
- YukiCoin/YukiCoin#20200 Rename CODEOWNERS to REVIEWERS (adamjonas)
- YukiCoin/YukiCoin#20329 docs/descriptors.md: Remove hardened marker in the path after xpub (dgpv)
- YukiCoin/YukiCoin#20380 Add instructions on how to fuzz the P2P layer using Honggfuzz NetDriver (practicalswift)
- YukiCoin/YukiCoin#20414 Remove generated manual pages from master branch (laanwj)
- YukiCoin/YukiCoin#20473 Document current boost dependency as 1.71.0 (laanwj)
- YukiCoin/YukiCoin#20512 Add bash as an OpenBSD dependency (emilengler)
- YukiCoin/YukiCoin#20568 Use FeeModes doc helper in estimatesmartfee (MarcoFalke)
- YukiCoin/YukiCoin#20577 libconsensus: add missing error code description, fix NYukiCoin link (theStack)
- YukiCoin/YukiCoin#20587 Tidy up Tor doc (more stringent) (wodry)
- YukiCoin/YukiCoin#20592 Update wtxidrelay documentation per BIP339 (jonatack)
- YukiCoin/YukiCoin#20601 Update for FreeBSD 12.2, add GUI Build Instructions (jarolrod)
- YukiCoin/YukiCoin#20635 fix misleading comment about call to non-existing function (pox)
- YukiCoin/YukiCoin#20646 Refer to BIPs 339/155 in feature negotiation (jonatack)
- YukiCoin/YukiCoin#20653 Move addr relay comment in net to correct place (MarcoFalke)
- YukiCoin/YukiCoin#20677 Remove shouty enums in `net_processing` comments (sdaftuar)
- YukiCoin/YukiCoin#20741 Update 'Secure string handling' (prayank23)
- YukiCoin/YukiCoin#20757 tor.md and -onlynet help updates (jonatack)
- YukiCoin/YukiCoin#20829 Add -netinfo help (jonatack)
- YukiCoin/YukiCoin#20830 Update developer notes with signet (jonatack)
- YukiCoin/YukiCoin#20890 Add explicit macdeployqtplus dependencies install step (hebasto)
- YukiCoin/YukiCoin#20913 Add manual page generation for YukiCoin-util (laanwj)
- YukiCoin/YukiCoin#20985 Add xorriso to macOS depends packages (fanquake)
- YukiCoin/YukiCoin#20986 Update developer notes to discourage very long lines (jnewbery)
- YukiCoin/YukiCoin#20987 Add instructions for generating RPC docs (ben-kaufman)
- YukiCoin/YukiCoin#21026 Document use of make-tag script to make tags (laanwj)
- YukiCoin/YukiCoin#21028 doc/bips: Add BIPs 43, 44, 49, and 84 (luke-jr)
- YukiCoin/YukiCoin#21049 Add release notes for listdescriptors RPC (S3RK)
- YukiCoin/YukiCoin#21060 More precise -debug and -debugexclude doc (wodry)
- YukiCoin/YukiCoin#21077 Clarify -timeout and -peertimeout config options (glozow)
- YukiCoin/YukiCoin#21105 Correctly identify script type (niftynei)
- YukiCoin/YukiCoin#21163 Guix is shipped in Debian and Ubuntu (MarcoFalke)
- YukiCoin/YukiCoin#21210 Rework internal and external links (MarcoFalke)
- YukiCoin/YukiCoin#21246 Correction for VerifyTaprootCommitment comments (roconnor-blockstream)
- YukiCoin/YukiCoin#21263 Clarify that squashing should happen before review (MarcoFalke)
- YukiCoin/YukiCoin#21323 guix, doc: Update default HOSTS value (hebasto)
- YukiCoin/YukiCoin#21324 Update build instructions for Fedora (hebasto)
- YukiCoin/YukiCoin#21343 Revamp macOS build doc (jarolrod)
- YukiCoin/YukiCoin#21346 install qt5 when building on macOS (fanquake)
- YukiCoin/YukiCoin#21384 doc: add signet to YukiCoin.conf documentation (jonatack)
- YukiCoin/YukiCoin#21394 Improve comment about protected peers (amitiuttarwar)
- YukiCoin/YukiCoin#21398 Update fuzzing docs for afl-clang-lto (MarcoFalke)
- YukiCoin/YukiCoin#21444 net, doc: Doxygen updates and fixes in netbase.{h,cpp} (jonatack)
- YukiCoin/YukiCoin#21481 Tell howto install clang-format on Debian/Ubuntu (wodry)
- YukiCoin/YukiCoin#21567 Fix various misleading comments (glozow)
- YukiCoin/YukiCoin#21661 Fix name of script guix-build (Emzy)
- YukiCoin/YukiCoin#21672 Remove boostrap info from `GUIX_COMMON_FLAGS` doc (fanquake)
- YukiCoin/YukiCoin#21688 Note on SDK for macOS depends cross-compile (jarolrod)
- YukiCoin/YukiCoin#21709 Update reduce-memory.md and YukiCoin.conf -maxconnections info (jonatack)
- YukiCoin/YukiCoin#21710 update helps for addnode rpc and -addnode/-maxconnections config options (jonatack)
- YukiCoin/YukiCoin#21752 Clarify that feerates are per virtual size (MarcoFalke)
- YukiCoin/YukiCoin#21811 Remove Visual Studio 2017 reference from readme (sipsorcery)
- YukiCoin/YukiCoin#21818 Fixup -coinstatsindex help, update YukiCoin.conf and files.md (jonatack)
- YukiCoin/YukiCoin#21856 add OSS-Fuzz section to fuzzing.md doc (adamjonas)
- YukiCoin/YukiCoin#21912 Remove mention of priority estimation (MarcoFalke)
- YukiCoin/YukiCoin#21925 Update bips.md for 0.21.1 (MarcoFalke)
- YukiCoin/YukiCoin#21942 improve make with parallel jobs description (klementtan)
- YukiCoin/YukiCoin#21947 Fix OSS-Fuzz links (MarcoFalke)
- YukiCoin/YukiCoin#21988 note that brew installed qt is not supported (jarolrod)
- YukiCoin/YukiCoin#22056 describe in fuzzing.md how to reproduce a CI crash (jonatack)
- YukiCoin/YukiCoin#22080 add maxuploadtarget to YukiCoin.conf example (jarolrod)
- YukiCoin/YukiCoin#22088 Improve note on choosing posix mingw32 (jarolrod)
- YukiCoin/YukiCoin#22109 Fix external links (IRC, …) (MarcoFalke)
- YukiCoin/YukiCoin#22121 Various validation doc fixups (MarcoFalke)
- YukiCoin/YukiCoin#22172 Update tor.md, release notes with removal of tor v2 support (jonatack)
- YukiCoin/YukiCoin#22204 Remove obsolete `okSafeMode` RPC guideline from developer notes (theStack)
- YukiCoin/YukiCoin#22208 Update `REVIEWERS` (practicalswift)
- YukiCoin/YukiCoin#22250 add basic I2P documentation (vasild)
- YukiCoin/YukiCoin#22296 Final merge of release notes snippets, mv to wiki (MarcoFalke)
- YukiCoin/YukiCoin#22335 recommend `--disable-external-signer` in OpenBSD build guide (theStack)
- YukiCoin/YukiCoin#22339 Document minimum required libc++ version (hebasto)
- YukiCoin/YukiCoin#22349 Repository IRC updates (jonatack)
- YukiCoin/YukiCoin#22360 Remove unused section from release process (MarcoFalke)
- YukiCoin/YukiCoin#22369 Add steps for Transifex to release process (jonatack)
- YukiCoin/YukiCoin#22393 Added info to YukiCoin.conf doc (bliotti)
- YukiCoin/YukiCoin#22402 Install Rosetta on M1-macOS for qt in depends (hebasto)
- YukiCoin/YukiCoin#22432 Fix incorrect `testmempoolaccept` doc (glozow)
- YukiCoin/YukiCoin#22648 doc, test: improve i2p/tor docs and i2p reachable unit tests (jonatack)

Credits
=======

Thanks to everyone who directly contributed to this release:

- Aaron Clauson
- Adam Jonas
- amadeuszpawlik
- Amiti Uttarwar
- Andrew Chow
- Andrew Poelstra
- Anthony Towns
- Antoine Poinsot
- Antoine Riard
- apawlik
- apitko
- Ben Carman
- Ben Woosley
- benk10
- Bezdrighin
- Block Mechanic
- Brian Liotti
- Bruno Garcia
- Carl Dong
- Christian Decker
- coinforensics
- Cory Fields
- Dan Benjamin
- Daniel Kraft
- Darius Parvin
- Dhruv Mehta
- Dmitry Goncharov
- Dmitry Petukhov
- dplusplus1024
- dscotese
- Duncan Dean
- Elle Mouton
- Elliott Jin
- Emil Engler
- Ethan Heilman
- eugene
- Evan Klitzke
- Fabian Jahr
- Fabrice Fontaine
- fanquake
- fdov
- flack
- Fotis Koutoupas
- Fu Yong Quah
- fyquah
- glozow
- Gregory Sanders
- Guido Vranken
- Gunar C. Gessner
- h
- HAOYUatHZ
- Hennadii Stepanov
- Igor Cota
- Ikko Ashimine
- Ivan Metlushko
- jackielove4u
- James O'Beirne
- Jarol Rodriguez
- Joel Klabo
- John Newbery
- Jon Atack
- Jonas Schnelli
- João Barbosa
- Josiah Baker
- Karl-Johan Alm
- Kiminuo
- Klement Tan
- Kristaps Kaupe
- Larry Ruane
- lisa neigut
- Lucas Ontivero
- Luke Dashjr
- Maayan Keshet
- MarcoFalke
- Martin Ankerl
- Martin Zumsande
- Michael Dietz
- Michael Polzer
- Michael Tidwell
- Niklas Gögge
- nthumann
- Oliver Gugger
- parazyd
- Patrick Strateman
- Pavol Rusnak
- Peter Bushnell
- Pierre K
- Pieter Wuille
- PiRK
- pox
- practicalswift
- Prayank
- R E Broadley
- Rafael Sadowski
- randymcmillan
- Raul Siles
- Riccardo Spagni
- Russell O'Connor
- Russell Yanofsky
- S3RK
- saibato
- Samuel Dobson
- sanket1729
- Sawyer Billings
- Sebastian Falbesoner
- setpill
- sgulls
- sinetek
- Sjors Provoost
- Sriram
- Stephan Oeste
- Suhas Daftuar
- Sylvain Goumy
- t-bast
- Troy Giorshev
- Tushar Singla
- Tyler Chambers
- Uplab
- Vasil Dimov
- W. J. van der Laan
- willcl-ark
- William Bright
- William Casarin
- windsok
- wodry
- Yerzhan Mazhkenov
- Yuval Kogman
- Zero

As well as to everyone that helped with translations on
[Transifex](https://www.transifex.com/YukiCoin/YukiCoin/).
