#!/usr/bin/env python3
# Copyright (c) 2017-2021 The YukiCoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test various command line arguments and configuration file parameters."""

import json


from test_framework.test_framework import YukiCoinTestFramework
from test_framework.test_node import ErrorMatch
from test_framework.util import assert_equal


class SettingsTest(YukiCoinTestFramework):
    def add_options(self, parser):
        self.add_wallet_options(parser)

    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 1
        self.wallet_names = []

    def test_wallet_settings(self, settings_path):
        if not self.is_wallet_compiled():
            return

        self.log.info("Testing wallet settings..")
        node = self.nodes[0]
        # Create wallet to use it during tests
        self.start_node(0)
        node.createwallet(wallet_name='w1')
        self.stop_node(0)

        # Verify wallet settings can only be strings. Either names or paths. Not booleans, nums nor anything else.
        for wallets_data in [[10], [True], [[]], [{}], ["w1", 10], ["w1", False]]:
            with settings_path.open("w") as fp:
                json.dump({"wallet": wallets_data}, fp)
            node.assert_start_raises_init_error(expected_msg="Error: Invalid value detected for '-wallet' or '-nowallet'. '-wallet' requires a string value, while '-nowallet' accepts only '1' to disable all wallets",
                                                extra_args=[f'-settings={settings_path}'])

    def run_test(self):
        node, = self.nodes
        settings = node.chain_path / "settings.json"
        conf = node.datadir_path / "YukiCoin.conf"

        # Assert default settings file was created
        self.stop_node(0)
        default_settings = {"_warning_": f"This file is automatically generated and updated by {self.config['environment']['CLIENT_NAME']}. Please do not edit this file while the node is running, as any changes might be ignored or overwritten."}
        with settings.open() as fp:
            assert_equal(json.load(fp), default_settings)

        # Assert settings are parsed and logged
        with settings.open("w") as fp:
            json.dump({"string": "string", "num": 5, "bool": True, "null": None, "list": [6, 7]}, fp)
        with node.assert_debug_log(expected_msgs=[
                'Ignoring unknown rw_settings value bool',
                'Ignoring unknown rw_settings value list',
                'Ignoring unknown rw_settings value null',
                'Ignoring unknown rw_settings value num',
                'Ignoring unknown rw_settings value string',
                'Setting file arg: string = "string"',
                'Setting file arg: num = 5',
                'Setting file arg: bool = true',
                'Setting file arg: null = null',
                'Setting file arg: list = [6,7]',
        ]):
            self.start_node(0)
            self.stop_node(0)

        # Assert settings are unchanged after shutdown
        with settings.open() as fp:
            assert_equal(json.load(fp), {**default_settings, **{"string": "string", "num": 5, "bool": True, "null": None, "list": [6, 7]}})

        # Test invalid json
        with settings.open("w") as fp:
            fp.write("invalid json")
        node.assert_start_raises_init_error(expected_msg='does not contain valid JSON. This is probably caused by disk corruption or a crash', match=ErrorMatch.PARTIAL_REGEX)

        # Test invalid json object
        with settings.open("w") as fp:
            fp.write('"string"')
        node.assert_start_raises_init_error(expected_msg='Found non-object value "string" in settings file', match=ErrorMatch.PARTIAL_REGEX)

        # Test invalid settings file containing duplicate keys
        with settings.open("w") as fp:
            fp.write('{"key": 1, "key": 2}')
        node.assert_start_raises_init_error(expected_msg='Found duplicate key key in settings file', match=ErrorMatch.PARTIAL_REGEX)

        # Test invalid settings file is ignored with command line -nosettings
        with node.assert_debug_log(expected_msgs=['Command-line arg: settings=false']):
            self.start_node(0, extra_args=["-nosettings"])
            self.stop_node(0)

        # Test invalid settings file is ignored with config file -nosettings
        with conf.open('a') as conf:
            conf.write('nosettings=1\n')
        with node.assert_debug_log(expected_msgs=['Config file arg: [regtest] settings=false']):
            self.start_node(0)
            self.stop_node(0)

        # Test alternate settings path
        altsettings = node.datadir_path / "altsettings.json"
        with altsettings.open("w") as fp:
            fp.write('{"key": "value"}')
        with node.assert_debug_log(expected_msgs=['Setting file arg: key = "value"']):
            self.start_node(0, extra_args=[f"-settings={altsettings}"])
            self.stop_node(0)

        self.test_wallet_settings(settings)


if __name__ == '__main__':
    SettingsTest(__file__).main()
