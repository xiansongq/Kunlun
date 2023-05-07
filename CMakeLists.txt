# Specify the minimum version for CMake
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

# Project's name 
PROJECT(Kunlun)
# On some machines, the path of OpenSSL is  /usr/local/lib64/openssl/libcrypto.a
IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    SET(OPENSSL_LIBRARIES /usr/local/lib64/libcrypto.a /usr/local/lib64/libssl.a)
    SET(OPENSSL_INCLUDE_DIR /usr/local/include)
    SET(OPENSSL_DIR /usr/local/lib64)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    SET(OPENSSL_LIBRARIES /usr/local/lib/libcrypto.a /usr/local/lib/libssl.a)
    SET(OPENSSL_INCLUDE_DIR /usr/local/include)
    SET(OPENSSL_DIR /usr/local/lib)
ENDIF()
FIND_PACKAGE(OpenMP REQUIRED)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fpic -msse4.2 -Wno-deprecated-declarations -Wno-unused-result -Xpreprocessor -O3")

#SET(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")

INCLUDE_DIRECTORIES(
    ${OPENSSL_INCLUDE_DIR}
    "/usr/local/include"
)

LINK_DIRECTORIES(
    /usr/local/lib
)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build) 

include(CMakePrintHelpers)

foreach(key
  IN ITEMS
    NUMBER_OF_LOGICAL_CORES
    NUMBER_OF_PHYSICAL_CORES
    IS_64BIT
    HAS_SSE2
    OS_NAME
    OS_RELEASE
  )
  # query the item ${key} and save its value in the variable _${key}
  cmake_host_system_information(RESULT _${key} QUERY ${key})
  cmake_print_variables(_${key})
endforeach()

# let the preprocessor know about the system name
if(CMAKE_SYSTEM_NAME  STREQUAL "Linux")
  SET(IS_LINUX 1)
endif()
if(CMAKE_SYSTEM_NAME  STREQUAL "Darwin")
  SET(IS_MACOS 1)
endif()
if(CMAKE_SYSTEM_NAME  STREQUAL "Windows")
  SET(IS_WINDOWS 1)
endif()

configure_file(${PROJECT_SOURCE_DIR}/config/config.h.in ${PROJECT_SOURCE_DIR}/config/config.h @ONLY)


# netio
ADD_EXECUTABLE(test_netio test/test_netio.cpp)
TARGET_LINK_LIBRARIES(test_netio ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

# filter
ADD_EXECUTABLE(test_bloom_filter test/test_bloom_filter.cpp)
TARGET_LINK_LIBRARIES(test_bloom_filter ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_cuckoo_filter test/test_cuckoo_filter.cpp)
TARGET_LINK_LIBRARIES(test_cuckoo_filter ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

# ot
ADD_EXECUTABLE(test_naor_pinkas_ot test/test_naor_pinkas_ot.cpp)
TARGET_LINK_LIBRARIES(test_naor_pinkas_ot ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_iknp_ote test/test_iknp_ote.cpp)
TARGET_LINK_LIBRARIES(test_iknp_ote ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_alsz_ote test/test_alsz_ote.cpp)
TARGET_LINK_LIBRARIES(test_alsz_ote ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

# ske  
ADD_EXECUTABLE(test_aes test/test_aes.cpp)
TARGET_LINK_LIBRARIES(test_aes ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

# psi
ADD_EXECUTABLE(test_cwprf_psi test/test_cwprf_psi.cpp)
TARGET_LINK_LIBRARIES(test_cwprf_psi ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

# pso
ADD_EXECUTABLE(test_mqrpmt test/test_mqrpmt.cpp)
TARGET_LINK_LIBRARIES(test_mqrpmt ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_mqrpmt_psi test/test_mqrpmt_psi.cpp)
TARGET_LINK_LIBRARIES(test_mqrpmt_psi ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_mqrpmt_psi_card test/test_mqrpmt_psi_card.cpp)
TARGET_LINK_LIBRARIES(test_mqrpmt_psi_card ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_mqrpmt_psi_card_sum test/test_mqrpmt_psi_card_sum.cpp)
TARGET_LINK_LIBRARIES(test_mqrpmt_psi_card_sum ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_mqrpmt_psu test/test_mqrpmt_psu.cpp)
TARGET_LINK_LIBRARIES(test_mqrpmt_psu ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_mqrpmt_private_id test/test_mqrpmt_private_id.cpp)
TARGET_LINK_LIBRARIES(test_mqrpmt_private_id ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

#mypso
ADD_EXECUTABLE(mytest test/test_pso.cpp)
TARGET_LINK_LIBRARIES(mytest ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

#mypso
ADD_EXECUTABLE(mypsu test/mypsuTest.cpp)
TARGET_LINK_LIBRARIES(mypsu ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

#cwPRF psi
ADD_EXECUTABLE(test_psi_from_cwprf test/mytest/test_psi_from_cwprf.cpp )
TARGET_LINK_LIBRARIES(test_psi_from_cwprf ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

# oprf
ADD_EXECUTABLE(test_oteoprf test/test_oteoprf.cpp)
TARGET_LINK_LIBRARIES(test_oteoprf ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

# peqt
ADD_EXECUTABLE(test_peqt test/test_peqt.cpp)
TARGET_LINK_LIBRARIES(test_peqt ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

# misc
ADD_EXECUTABLE(test_misc test/test_misc.cpp)
TARGET_LINK_LIBRARIES(test_misc ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})
  
# pke  
ADD_EXECUTABLE(test_twisted_elgamal test/test_twisted_elgamal.cpp)
TARGET_LINK_LIBRARIES(test_twisted_elgamal ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_elgamal test/test_elgamal.cpp)
TARGET_LINK_LIBRARIES(test_elgamal ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_calculate_dlog test/test_calculate_dlog.cpp)
TARGET_LINK_LIBRARIES(test_calculate_dlog ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

# signature
ADD_EXECUTABLE(test_accountable_ring_sig test/test_accountable_ring_sig.cpp)
TARGET_LINK_LIBRARIES(test_accountable_ring_sig ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_schnorr_sig test/test_schnorr_sig.cpp)
TARGET_LINK_LIBRARIES(test_schnorr_sig ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

# nike
ADD_EXECUTABLE(test_nizk_enc_relation test/test_nizk_enc_relation.cpp)
TARGET_LINK_LIBRARIES(test_nizk_enc_relation ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_nizk_dlog_knowledge test/test_nizk_dlog_knowledge.cpp)
TARGET_LINK_LIBRARIES(test_nizk_dlog_knowledge ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_nizk_plaintext_equality test/test_nizk_plaintext_equality.cpp)
TARGET_LINK_LIBRARIES(test_nizk_plaintext_equality ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_nizk_plaintext_knowledge test/test_nizk_plaintext_knowledge.cpp)
TARGET_LINK_LIBRARIES(test_nizk_plaintext_knowledge ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_nizk_dlog_equality test/test_nizk_dlog_equality.cpp)
TARGET_LINK_LIBRARIES(test_nizk_dlog_equality ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

# bulletproof
ADD_EXECUTABLE(test_innerproduct_proof test/test_innerproduct_proof.cpp)
TARGET_LINK_LIBRARIES(test_innerproduct_proof ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

ADD_EXECUTABLE(test_bullet_proof test/test_bullet_proof.cpp)
TARGET_LINK_LIBRARIES(test_bullet_proof ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

# gadget
ADD_EXECUTABLE(test_range_proof test/test_range_proof.cpp)
TARGET_LINK_LIBRARIES(test_range_proof ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

# cryptocurrency
ADD_EXECUTABLE(test_adcp test/test_adcp.cpp)
TARGET_LINK_LIBRARIES(test_adcp ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

# mcl
# add_executable(test_mcl test/test_mcl.cpp)
# target_link_libraries(test_mcl libmcl.a libgmp.a)



#[[!!!有时会遇到 seal/seal.h 找不到的错误]]
find_package(SEAL 4.0  REQUIRED)

if(NOT SEAL_FOUND)
    message(FATAL_ERROR "Microsoft SEAL: not found")
else()
    message(STATUS "Microsoft SEAL: ${SEAL_DIR}")
endif()


add_library(sealpir mpc/pir/pir.hpp mpc/pir/pir.cpp mpc/pir/pir_client.hpp mpc/pir/pir_client.cpp mpc/pir/pir_server.hpp
        mpc/pir/pir_server.cpp  )


target_link_libraries(sealpir SEAL::seal)

#add_executable(pirtest test/mytest/PirTest.cpp )
#target_link_libraries(pirtest sealpir ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

add_executable(test_pirfile test/mytest/sealpirTest.cpp)
target_link_libraries(test_pirfile sealpir ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

add_executable(test_cope test/mytest/test_cope.cpp)
target_link_libraries(test_cope  ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

add_executable(test_oprf_psi test/mytest/test_psi_from_oprf.cpp)
target_link_libraries(test_oprf_psi ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

#ecdh_psi
add_executable(test_ecdh_psi test/mytest/test_ecdh_psi.cpp)
target_link_libraries(test_ecdh_psi ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

#hash function
add_executable(test_hash test/mytest/test_hash.cpp)
target_link_libraries(test_hash ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})

#multi thread
add_executable(test_thread test/mytest/test_multi_thread.cpp)
target_link_libraries(test_thread ${OPENSSL_LIBRARIES} OpenMP::OpenMP_CXX ${CMAKE_DL_LIBS})


