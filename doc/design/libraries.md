# Libraries

| Name                     | Description |
|--------------------------|-------------|
| *libYukiCoin_cli*         | RPC client functionality used by *YukiCoin-cli* executable |
| *libYukiCoin_common*      | Home for common functionality shared by different executables and libraries. Similar to *libYukiCoin_util*, but higher-level (see [Dependencies](#dependencies)). |
| *libYukiCoin_consensus*   | Consensus functionality used by *libYukiCoin_node* and *libYukiCoin_wallet*. |
| *libYukiCoin_crypto*      | Hardware-optimized functions for data encryption, hashing, message authentication, and key derivation. |
| *libYukiCoin_kernel*      | Consensus engine and support library used for validation by *libYukiCoin_node*. |
| *libYukiCoinqt*           | GUI functionality used by *YukiCoin-qt* and *YukiCoin-gui* executables. |
| *libYukiCoin_ipc*         | IPC functionality used by *YukiCoin-node*, *YukiCoin-wallet*, *YukiCoin-gui* executables to communicate when [`-DWITH_MULTIPROCESS=ON`](multiprocess.md) is used. |
| *libYukiCoin_node*        | P2P and RPC server functionality used by *YukiCoind* and *YukiCoin-qt* executables. |
| *libYukiCoin_util*        | Home for common functionality shared by different executables and libraries. Similar to *libYukiCoin_common*, but lower-level (see [Dependencies](#dependencies)). |
| *libYukiCoin_wallet*      | Wallet functionality used by *YukiCoind* and *YukiCoin-wallet* executables. |
| *libYukiCoin_wallet_tool* | Lower-level wallet functionality used by *YukiCoin-wallet* executable. |
| *libYukiCoin_zmq*         | [ZeroMQ](../zmq.md) functionality used by *YukiCoind* and *YukiCoin-qt* executables. |

## Conventions

- Most libraries are internal libraries and have APIs which are completely unstable! There are few or no restrictions on backwards compatibility or rules about external dependencies. An exception is *libYukiCoin_kernel*, which, at some future point, will have a documented external interface.

- Generally each library should have a corresponding source directory and namespace. Source code organization is a work in progress, so it is true that some namespaces are applied inconsistently, and if you look at [`add_library(YukiCoin_* ...)`](../../src/CMakeLists.txt) lists you can see that many libraries pull in files from outside their source directory. But when working with libraries, it is good to follow a consistent pattern like:

  - *libYukiCoin_node* code lives in `src/node/` in the `node::` namespace
  - *libYukiCoin_wallet* code lives in `src/wallet/` in the `wallet::` namespace
  - *libYukiCoin_ipc* code lives in `src/ipc/` in the `ipc::` namespace
  - *libYukiCoin_util* code lives in `src/util/` in the `util::` namespace
  - *libYukiCoin_consensus* code lives in `src/consensus/` in the `Consensus::` namespace

## Dependencies

- Libraries should minimize what other libraries they depend on, and only reference symbols following the arrows shown in the dependency graph below:

<table><tr><td>

```mermaid

%%{ init : { "flowchart" : { "curve" : "basis" }}}%%

graph TD;

YukiCoin-cli[YukiCoin-cli]-->libYukiCoin_cli;

YukiCoind[YukiCoind]-->libYukiCoin_node;
YukiCoind[YukiCoind]-->libYukiCoin_wallet;

YukiCoin-qt[YukiCoin-qt]-->libYukiCoin_node;
YukiCoin-qt[YukiCoin-qt]-->libYukiCoinqt;
YukiCoin-qt[YukiCoin-qt]-->libYukiCoin_wallet;

YukiCoin-wallet[YukiCoin-wallet]-->libYukiCoin_wallet;
YukiCoin-wallet[YukiCoin-wallet]-->libYukiCoin_wallet_tool;

libYukiCoin_cli-->libYukiCoin_util;
libYukiCoin_cli-->libYukiCoin_common;

libYukiCoin_consensus-->libYukiCoin_crypto;

libYukiCoin_common-->libYukiCoin_consensus;
libYukiCoin_common-->libYukiCoin_crypto;
libYukiCoin_common-->libYukiCoin_util;

libYukiCoin_kernel-->libYukiCoin_consensus;
libYukiCoin_kernel-->libYukiCoin_crypto;
libYukiCoin_kernel-->libYukiCoin_util;

libYukiCoin_node-->libYukiCoin_consensus;
libYukiCoin_node-->libYukiCoin_crypto;
libYukiCoin_node-->libYukiCoin_kernel;
libYukiCoin_node-->libYukiCoin_common;
libYukiCoin_node-->libYukiCoin_util;

libYukiCoinqt-->libYukiCoin_common;
libYukiCoinqt-->libYukiCoin_util;

libYukiCoin_util-->libYukiCoin_crypto;

libYukiCoin_wallet-->libYukiCoin_common;
libYukiCoin_wallet-->libYukiCoin_crypto;
libYukiCoin_wallet-->libYukiCoin_util;

libYukiCoin_wallet_tool-->libYukiCoin_wallet;
libYukiCoin_wallet_tool-->libYukiCoin_util;

classDef bold stroke-width:2px, font-weight:bold, font-size: smaller;
class YukiCoin-qt,YukiCoind,YukiCoin-cli,YukiCoin-wallet bold
```
</td></tr><tr><td>

**Dependency graph**. Arrows show linker symbol dependencies. *Crypto* lib depends on nothing. *Util* lib is depended on by everything. *Kernel* lib depends only on consensus, crypto, and util.

</td></tr></table>

- The graph shows what _linker symbols_ (functions and variables) from each library other libraries can call and reference directly, but it is not a call graph. For example, there is no arrow connecting *libYukiCoin_wallet* and *libYukiCoin_node* libraries, because these libraries are intended to be modular and not depend on each other's internal implementation details. But wallet code is still able to call node code indirectly through the `interfaces::Chain` abstract class in [`interfaces/chain.h`](../../src/interfaces/chain.h) and node code calls wallet code through the `interfaces::ChainClient` and `interfaces::Chain::Notifications` abstract classes in the same file. In general, defining abstract classes in [`src/interfaces/`](../../src/interfaces/) can be a convenient way of avoiding unwanted direct dependencies or circular dependencies between libraries.

- *libYukiCoin_crypto* should be a standalone dependency that any library can depend on, and it should not depend on any other libraries itself.

- *libYukiCoin_consensus* should only depend on *libYukiCoin_crypto*, and all other libraries besides *libYukiCoin_crypto* should be allowed to depend on it.

- *libYukiCoin_util* should be a standalone dependency that any library can depend on, and it should not depend on other libraries except *libYukiCoin_crypto*. It provides basic utilities that fill in gaps in the C++ standard library and provide lightweight abstractions over platform-specific features. Since the util library is distributed with the kernel and is usable by kernel applications, it shouldn't contain functions that external code shouldn't call, like higher level code targeted at the node or wallet. (*libYukiCoin_common* is a better place for higher level code, or code that is meant to be used by internal applications only.)

- *libYukiCoin_common* is a home for miscellaneous shared code used by different YukiCoin Core applications. It should not depend on anything other than *libYukiCoin_util*, *libYukiCoin_consensus*, and *libYukiCoin_crypto*.

- *libYukiCoin_kernel* should only depend on *libYukiCoin_util*, *libYukiCoin_consensus*, and *libYukiCoin_crypto*.

- The only thing that should depend on *libYukiCoin_kernel* internally should be *libYukiCoin_node*. GUI and wallet libraries *libYukiCoinqt* and *libYukiCoin_wallet* in particular should not depend on *libYukiCoin_kernel* and the unneeded functionality it would pull in, like block validation. To the extent that GUI and wallet code need scripting and signing functionality, they should be get able it from *libYukiCoin_consensus*, *libYukiCoin_common*, *libYukiCoin_crypto*, and *libYukiCoin_util*, instead of *libYukiCoin_kernel*.

- GUI, node, and wallet code internal implementations should all be independent of each other, and the *libYukiCoinqt*, *libYukiCoin_node*, *libYukiCoin_wallet* libraries should never reference each other's symbols. They should only call each other through [`src/interfaces/`](../../src/interfaces/) abstract interfaces.

## Work in progress

- Validation code is moving from *libYukiCoin_node* to *libYukiCoin_kernel* as part of [The libYukiCoinkernel Project #27587](https://github.com/YukiCoin/YukiCoin/issues/27587)
