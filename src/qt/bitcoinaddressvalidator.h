// Copyright (c) 2011-2020 The YukiCoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef YukiCoin_QT_YukiCoinADDRESSVALIDATOR_H
#define YukiCoin_QT_YukiCoinADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class YukiCoinAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit YukiCoinAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

/** YukiCoin address widget validator, checks for a valid YukiCoin address.
 */
class YukiCoinAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit YukiCoinAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

#endif // YukiCoin_QT_YukiCoinADDRESSVALIDATOR_H
