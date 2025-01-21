# Copyright (c) 2023-present The YukiCoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or https://opensource.org/license/mit/.

function(generate_setup_nsi)
  set(abs_top_srcdir ${PROJECT_SOURCE_DIR})
  set(abs_top_builddir ${PROJECT_BINARY_DIR})
  set(CLIENT_URL ${PROJECT_HOMEPAGE_URL})
  set(CLIENT_TARNAME "YukiCoin")
  set(YukiCoin_GUI_NAME "YukiCoin-qt")
  set(YukiCoin_DAEMON_NAME "YukiCoind")
  set(YukiCoin_CLI_NAME "YukiCoin-cli")
  set(YukiCoin_TX_NAME "YukiCoin-tx")
  set(YukiCoin_WALLET_TOOL_NAME "YukiCoin-wallet")
  set(YukiCoin_TEST_NAME "test_YukiCoin")
  set(EXEEXT ${CMAKE_EXECUTABLE_SUFFIX})
  configure_file(${PROJECT_SOURCE_DIR}/share/setup.nsi.in ${PROJECT_BINARY_DIR}/YukiCoin-win64-setup.nsi USE_SOURCE_PERMISSIONS @ONLY)
endfunction()
