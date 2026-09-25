# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_executable(xapp_positioning 
  xapp_positioning.c
  ../../../../src/util/alg_ds/alg/defer.c
  ../../../../src/util/alg_ds/alg/murmur_hash_32.c
  ../../../../src/util/alg_ds/ds/assoc_container/assoc_ht_open_address.c
  )
 
target_link_libraries(xapp_positioning
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )
target_compile_definitions(xapp_positioning PRIVATE KPM_MEAS_LIST="${KPM_MEAS_LIST}")
