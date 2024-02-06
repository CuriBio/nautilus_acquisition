# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0.
set(AWSSDK_PLATFORM_DEPS_LIBS Userenv;version;ws2_32)
set(AWSSDK_CLIENT_LIBS Wininet;winhttp)
set(AWSSDK_CRYPTO_LIBS Bcrypt)
set(AWSSDK_CRYPTO_IN_SOURCE_BUILD )
set(AWSSDK_COMMON_RUNTIME_LIBS aws-crt-cpp;aws-c-auth;aws-c-cal;aws-c-common;aws-c-compression;aws-c-event-stream;aws-c-http;aws-c-io;aws-c-mqtt;aws-c-s3;aws-checksums;aws-c-sdkutils)
set(AWSSDK_ADDITIONAL_LIBS )
set(AWSSDK_INSTALL_LIBDIR lib)
set(AWSSDK_INSTALL_BINDIR bin)
set(AWSSDK_INSTALL_INCLUDEDIR include)
set(AWSSDK_INSTALL_ARCHIVEDIR bin)
if (NOT LibCrypto_INCLUDE_DIR)
    set(LibCrypto_INCLUDE_DIR  CACHE INTERNAL "The OpenSSL include directory")
endif()
if (NOT LibCrypto_STATIC_LIBRARY)
    set(LibCrypto_STATIC_LIBRARY  CACHE INTERNAL "The OpenSSL crypto static library")
endif()
if (NOT LibCrypto_SHARED_LIBRARY)
    set(LibCrypto_SHARED_LIBRARY  CACHE INTERNAL "The OpenSSL crypto shared library")
endif()
