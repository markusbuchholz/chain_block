cmake_minimum_required(VERSION 3.5)

# Set the project name
project (chain_block)

# Create a sources variable with a link to all cpp files to compile
set(SOURCES
    src/blockchain.cpp
    src/sha256.cpp
)

# Add an executable with the above sources
add_executable(chain_block ${SOURCES})

# Set the directories that should be included in the build command for this target
# when running g++ these will be included as -I/directory/path/
target_include_directories(chain_block
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)
