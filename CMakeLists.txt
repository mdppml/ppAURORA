cmake_minimum_required(VERSION 3.9)
project(CECILIA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_DEBUG "-O3")
set(CMAKE_CXX_FLAGS_RELEASE  "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Libraries:
find_package(Threads REQUIRED)

# Helper
add_executable(helper apps/helper.cpp)
target_link_libraries(helper PRIVATE Threads::Threads)

# ppAURORA
# AUROC no-tie
add_executable(proxy_auroc apps/auroc/proxy.cpp core/Party.h utils/constant.h utils/auc_utils.h utils/parse_options.cpp utils/parse_options.h utils/connection.h utils/flib.h utils/llib.h core/auc.h ${ENC_FILES})
target_link_libraries(proxy_auroc PRIVATE Threads::Threads)
# AUROC with-tie
add_executable(proxy_auroc_tie apps/auroctie/proxy.cpp core/Party.h utils/constant.h utils/auc_utils.h utils/parse_options.cpp utils/parse_options.h utils/connection.h utils/flib.h core/auc.h ${ENC_FILES})
target_link_libraries(proxy_auroc_tie PRIVATE Threads::Threads)
# AUPR
add_executable(proxy_aupr apps/aupr/proxy.cpp core/Party.h utils/constant.h utils/parse_options.cpp utils/parse_options.h utils/connection.h utils/flib.h utils/llib.h core/auc.h ${ENC_FILES})
target_link_libraries(proxy_aupr PRIVATE Threads::Threads)
