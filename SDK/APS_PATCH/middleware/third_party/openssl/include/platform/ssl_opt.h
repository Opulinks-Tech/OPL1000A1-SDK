// Copyright 2017 - 2018, Opulinks Technology Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _SSL_OPT_H_
#define _SSL_OPT_H_


/**
 * \def OPENSSL_DEBUG
 *
 * Enable OpenSSL debugging function
 * If the option is enabled, "SSL_DEBUG" works.
 *
 */
#define CONFIG_OPENSSL_DEBUG


/**
 * \def OPENSSL_DEBUG_LEVEL
 *
 * OpenSSL debugging level. (default 0, range 0~255)
 * Only function whose debugging level is higher than "OPENSSL_DEBUG_LEVEL" works.
 * For example:
 * If OPENSSL_DEBUG_LEVEL = 2, you use function "SSL_DEBUG(1, "malloc failed")". Because 1 < 2, it will not print.
 */
#define CONFIG_OPENSSL_DEBUG_LEVEL 1

/**
 * \def OPENSSL_LOWLEVEL_DEBUG, depends on OPENSSL_DEBUG
 *
 * Enable OpenSSL low-level module debugging
 * If the option is enabled, low-level module debugging function of OpenSSL is enabled,
 * e.g. mbedtls internal debugging function.
 */
#define CONFIG_OPENSSL_LOWLEVEL_DEBUG

/**
 * \def OPENSSL_ASSERT
 *
 * Select OpenSSL assert function
 * If you want to use assert debugging function, "OPENSSL_DEBUG" should be enabled
 */

#define OPENSSL_ASSERT_DO_NOTHING

    //CONFIG_OPENSSL_ASSERT_EXIT
    //OPENSSL_ASSERT_DO_NOTHING  // Do nothing and "SSL_ASSERT" does not work.
    //OPENSSL_ASSERT_EXIT        // Enable assert exiting, it will check and return error code.
    //OPENSSL_ASSERT_DEBUG       // Enable assert debugging, it will check and show debugging message.
    //OPENSSL_ASSERT_DEBUG_EXIT  // Enable assert debugging and exiting, it will check, show debugging message and return error code.
    //OPENSSL_ASSERT_DEBUG_BLOCK // Enable assert debugging and blocking, it will check, show debugging message and block by "while (1);".

#endif
