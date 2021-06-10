#include <iostream>
#include <string>
#include <random>
#include <algorithm>
#include "sha256.h"
#include <vector>

//--------------------------------------------------------------------------

struct Block
{

    Block *next;
    Block *prev;
    std::string actual_hash;
    std::string prev_hash;
    std::string data;
};

//--------------------------------------------------------------------------

class BlockChain
{

private:
    Block *previous;
    Block *head;

public:
    static int numberOfBlocks;

    BlockChain()
    {

        this->previous = nullptr;
        this->head = nullptr;
        std::cout << __FUNCTION__ << " created " << std::endl;
    };

    //--------------------------------------------------------------------------

    std::string random_string()
    {
        std::string str("0123456789abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz");

        std::random_device rd;
        std::mt19937 generator(rd());

        std::shuffle(str.begin(), str.end(), generator);

        return str.substr(0, 64); // assumes 32 < number of characters in str
    }

    //--------------------------------------------------------------------------

    std::string generateHashSHA256(std::string toHasing)
    {

        return sha256(toHasing);
    }

    //--------------------------------------------------------------------------

    void addNode(std::string new_data)
    {

        Block *new_block = new Block;

        new_block->data = new_data;
        new_block->next = nullptr;

        if (previous != nullptr)
        {

            previous = head;
            head = new_block;
            new_block->actual_hash = generateHashSHA256(new_data + previous->actual_hash);
            new_block->prev_hash = previous->actual_hash;

            std::cout << "actual hash : " << new_block->actual_hash << std::endl;
            std::cout << "prev hash : " << previous->actual_hash << std::endl;
            std::cout << "------------------------------------------------------------------------------" << std::endl;
            new_block->prev = previous;
        }
        if (previous == nullptr)
        {
            head = new_block;
            previous = head;

            new_block->prev_hash = generateHashSHA256(random_string());
            new_block->data = new_data;
            new_block->actual_hash = generateHashSHA256(new_data);
            std::cout << "### Data for new block   : " << new_block->data << std::endl;
        }

        std::cout << previous << " :: " << head << std::endl;

        numberOfBlocks++;
    }

    //--------------------------------------------------------------------------

    void traverseBlokChain(BlockChain &chain)
    {

        Block *actual_node = chain.head;
        int currentBlock = numberOfBlocks;

        while (actual_node->prev != nullptr)
        {
            std::cout << "=========================================================" << std::endl;
            std::cout << "block : " << currentBlock << std::endl;
            std::cout << "data  : " << actual_node->data << std::endl;
            actual_node = actual_node->prev;
            currentBlock--;
        }
        std::cout << "=========================================================" << std::endl;
    }

    //--------------------------------------------------------------------------

    int getNumberOfBlocks()
    {

        return numberOfBlocks;
    }

    //--------------------------------------------------------------------------

    void checkChainConsistency(BlockChain &chain)
    {
        std::cout << "=======================================================" << std::endl;
        Block *actual_node = chain.head;
        int currentBlock = numberOfBlocks;

        while ((actual_node->prev != nullptr) && (currentBlock > 2))
        {

            std::string check_hash;
            std::string check_data;
            std::string check_hashFWD;
            std::string check_dataFWD;
            std::cout << __FUNCTION__ << "for block : " << currentBlock - 1 << std::endl;

            check_data = (actual_node->prev)->data;
            check_hash = (actual_node->prev)->prev_hash;

            check_dataFWD = actual_node->data;
            check_hashFWD = actual_node->prev_hash;

            std::cout << "DATA CHECKED : " << (actual_node->prev)->data << std::endl;

            if (actual_node->prev != nullptr)
            {
                std::string FWDCalculatedHash = generateHashSHA256(check_dataFWD + check_hashFWD);
                std::string injectedCalculatedHash = generateHashSHA256(check_data + check_hash);

                std::string storedHash = (actual_node->prev)->actual_hash;

                if (injectedCalculatedHash == check_hashFWD)
                {
                    std::cout << " HASH OK ==> Block Healthy !! " << std::endl;
                }
                if (injectedCalculatedHash != check_hashFWD)
                {
                    std::cout << " HASH WRONG ==> Block MANIPULATED !! " << std::endl;
                }
            }
            std::cout << "==============================================================================" << std::endl;
            actual_node = actual_node->prev;
            currentBlock--;
        }
    }
    //--------------------------------------------------------------------------
    void recalulcateChain(BlockChain &chain)
    {

        Block *node = chain.head;

        std::vector<std::string> tempChainData;
        std::vector<Block *> ptr_node{node};
        std::vector<Block *> recal_node{numberOfBlocks};

        while (node->prev != nullptr)
        {

            tempChainData.push_back(node->data);
            node = node->prev;
            ptr_node.push_back(node);

            if (node->prev == nullptr)
            {
                tempChainData.push_back(node->data);
            }
        }

        int i = 0;
        for (auto &ii : ptr_node)
        {

            std::cout << " DATA PTR NODE  " << ii->data << std::endl;
            recal_node[numberOfBlocks - 1 - i] = ii;
            i++;
        }

        int j = 0;
        for (auto &ii : recal_node)
        {

            std::cout << " RECAL DATA   " << ii->data << std::endl;

            if (j == 0)
            {
                ii->prev_hash = generateHashSHA256(random_string());
                ii->actual_hash = generateHashSHA256(ii->data);
            }

            if (j > 0)
            {

                ii->actual_hash = generateHashSHA256(ii->data + (ii->prev)->actual_hash);
                ii->prev_hash = (ii->prev)->actual_hash;
            }

            j++;
        }

        Block *node2 = chain.head;
        int currentBlock = numberOfBlocks;

        while (node2->prev != nullptr)
        {
            std::cout << "=========================================================" << std::endl;
            std::cout << "block : " << currentBlock << std::endl;
            std::cout << "data  : " << node2->data << std::endl;
            std::cout << "actual hash : " << node2->actual_hash << std::endl;
            std::cout << "NODE prev hash : " << node2->prev_hash << std::endl;
            std::cout << "prev hash : " << (node2->prev)->actual_hash << std::endl;
            node2 = node2->prev;
            currentBlock--;
        }
        std::cout << "=========================================================" << std::endl;
    }

    //--------------------------------------------------------------------------

    void injectDataInBlock(BlockChain &chain, int injectToBlock, std::string injectData)
    {

        Block *actual_node = chain.head;
        int currentBlock = numberOfBlocks;

        while (actual_node->prev != nullptr)
        {

            if (currentBlock == injectToBlock)
            {

                std::cout << "HASH__1 " << actual_node->actual_hash << std::endl;
                std::cout << "BEFORE INJECTION " << actual_node->data << std::endl;
                actual_node->data = injectData;
                actual_node->actual_hash = generateHashSHA256(injectData + actual_node->prev_hash);
                std::cout << "HASH__2 " << actual_node->actual_hash << std::endl;
            }

            actual_node = actual_node->prev;
            currentBlock--;
        }
        std::cout << "=========================================================" << std::endl;
    }
};

//--------------------------------------------------------------------------

int BlockChain::numberOfBlocks{0};

//--------------------------------------------------------------------------

int main()
{

    BlockChain chain;
    chain.addNode("important info 1");
    chain.addNode("important info 2");
    chain.addNode("important info 3");
    chain.addNode("important info 4");
    chain.addNode("important info 5");
    chain.addNode("important info 6");
    chain.addNode("important info 7");
    chain.addNode("important info 8");
    chain.addNode("important info 9");
    chain.addNode("important info 10");

    chain.traverseBlokChain(chain);
    chain.checkChainConsistency(chain);
    chain.injectDataInBlock(chain, 5, "Superior LINUX");
    chain.traverseBlokChain(chain);
    chain.checkChainConsistency(chain);
    chain.recalulcateChain(chain);
    chain.checkChainConsistency(chain);
}

//EXPECTED OUTPUT

/*-----------------------------------------------------
BlockChain created 
### Data for new block   : important info 1
0x55e0ee4842a0 :: 0x55e0ee4842a0
actual hash : c43b45b6cef1055f8dfa995786279d3f603e4f3dd85e73dfe7f0e21fad362e1d
prev hash : 01a0d24f7eb166a2ca006f23d34c0f13ddc9b25b1ef99e9a01d10f5d4d3917e1
------------------------------------------------------------------------------
0x55e0ee4842a0 :: 0x55e0ee4844e0
actual hash : 64327441085190e7602b1bb76b58151b5fa9a1dc4161aa6f88fd76cd9f5eddff
prev hash : c43b45b6cef1055f8dfa995786279d3f603e4f3dd85e73dfe7f0e21fad362e1d
------------------------------------------------------------------------------
0x55e0ee4844e0 :: 0x55e0ee484640
actual hash : b7098b658bdb4b8e0e871464ba371711c0f885a1f5ed69ca2a63b65c9ef16d78
prev hash : 64327441085190e7602b1bb76b58151b5fa9a1dc4161aa6f88fd76cd9f5eddff
------------------------------------------------------------------------------
0x55e0ee484640 :: 0x55e0ee484790
actual hash : 11fd396bb5302f49685b9c28c0be07719e610c17a86376d532a93b02a763a371
prev hash : b7098b658bdb4b8e0e871464ba371711c0f885a1f5ed69ca2a63b65c9ef16d78
------------------------------------------------------------------------------
0x55e0ee484790 :: 0x55e0ee4848e0
actual hash : 2ecdff189754c79644acda5fd5786c57190be67751dae0b813f1ed3817178ddf
prev hash : 11fd396bb5302f49685b9c28c0be07719e610c17a86376d532a93b02a763a371
------------------------------------------------------------------------------
0x55e0ee4848e0 :: 0x55e0ee484a30
actual hash : a215675d6d82324df8b3c5a3e2618f6d13eb46b30c5f74b5ace809f15beb1992
prev hash : 2ecdff189754c79644acda5fd5786c57190be67751dae0b813f1ed3817178ddf
------------------------------------------------------------------------------
0x55e0ee484a30 :: 0x55e0ee484b80
actual hash : 31a51ce358b931f8353ab0edf62e2128d7f58c68fbceb6442ae91758762cda84
prev hash : a215675d6d82324df8b3c5a3e2618f6d13eb46b30c5f74b5ace809f15beb1992
------------------------------------------------------------------------------
0x55e0ee484b80 :: 0x55e0ee484cd0
actual hash : f14116000783daf8277bdd50dcbeaea697c1c635ae91df64634e833891712dfe
prev hash : 31a51ce358b931f8353ab0edf62e2128d7f58c68fbceb6442ae91758762cda84
------------------------------------------------------------------------------
0x55e0ee484cd0 :: 0x55e0ee484e20
actual hash : f04f42d245e0d5efefa24d1d94fe6ad6a636f6d293f28ff66d76f47bd28cc4d9
prev hash : f14116000783daf8277bdd50dcbeaea697c1c635ae91df64634e833891712dfe
------------------------------------------------------------------------------
0x55e0ee484e20 :: 0x55e0ee484f70
=========================================================
block : 10
data  : important info 10
=========================================================
block : 9
data  : important info 9
=========================================================
block : 8
data  : important info 8
=========================================================
block : 7
data  : important info 7
=========================================================
block : 6
data  : important info 6
=========================================================
block : 5
data  : important info 5
=========================================================
block : 4
data  : important info 4
=========================================================
block : 3
data  : important info 3
=========================================================
block : 2
data  : important info 2
=========================================================
=======================================================
checkChainConsistencyfor block : 9
DATA CHECKED 1: important info 9
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 8
DATA CHECKED 1: important info 8
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 7
DATA CHECKED 1: important info 7
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 6
DATA CHECKED 1: important info 6
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 5
DATA CHECKED 1: important info 5
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 4
DATA CHECKED 1: important info 4
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 3
DATA CHECKED 1: important info 3
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 2
DATA CHECKED 1: important info 2
 HASH OK ==> Block Healthy !! 
==============================================================================
HASH__1 11fd396bb5302f49685b9c28c0be07719e610c17a86376d532a93b02a763a371
BEFORE INJECTION important info 5
HASH__2 70993d6cfa65d73473f9a8d8e91b4bf2831e2445958316b016c0bdb96b3d6a5a
=========================================================
=========================================================
block : 10
data  : important info 10
=========================================================
block : 9
data  : important info 9
=========================================================
block : 8
data  : important info 8
=========================================================
block : 7
data  : important info 7
=========================================================
block : 6
data  : important info 6
=========================================================
block : 5
data  : Superior LINUX
=========================================================
block : 4
data  : important info 4
=========================================================
block : 3
data  : important info 3
=========================================================
block : 2
data  : important info 2
=========================================================
=======================================================
checkChainConsistencyfor block : 9
DATA CHECKED 1: important info 9
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 8
DATA CHECKED 1: important info 8
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 7
DATA CHECKED 1: important info 7
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 6
DATA CHECKED 1: important info 6
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 5
DATA CHECKED 1: Superior LINUX
 HASH WRONG ==> Block MANIPULATED !! 
==============================================================================
checkChainConsistencyfor block : 4
DATA CHECKED 1: important info 4
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 3
DATA CHECKED 1: important info 3
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 2
DATA CHECKED 1: important info 2
 HASH OK ==> Block Healthy !! 
==============================================================================
 DATA PTR NODE  important info 10
 DATA PTR NODE  important info 9
 DATA PTR NODE  important info 8
 DATA PTR NODE  important info 7
 DATA PTR NODE  important info 6
 DATA PTR NODE  Superior LINUX
 DATA PTR NODE  important info 4
 DATA PTR NODE  important info 3
 DATA PTR NODE  important info 2
 DATA PTR NODE  important info 1
 RECAL DATA   important info 1
 RECAL DATA   important info 2
 RECAL DATA   important info 3
 RECAL DATA   important info 4
 RECAL DATA   Superior LINUX
 RECAL DATA   important info 6
 RECAL DATA   important info 7
 RECAL DATA   important info 8
 RECAL DATA   important info 9
 RECAL DATA   important info 10
=========================================================
block : 10
data  : important info 10
actual hash : d7057c1ddfda54988fbc750bfb20f0525dec0b6ae90532621cc4ccda0153be8a
NODE prev hash : add4fc9516c44add941d5ece408fd0947820c68283d30dffc4be37b321a90360
prev hash : add4fc9516c44add941d5ece408fd0947820c68283d30dffc4be37b321a90360
=========================================================
block : 9
data  : important info 9
actual hash : add4fc9516c44add941d5ece408fd0947820c68283d30dffc4be37b321a90360
NODE prev hash : b917213dd781fd9906262514689711f507d67f2a9224f28ac696a4dbea0e1667
prev hash : b917213dd781fd9906262514689711f507d67f2a9224f28ac696a4dbea0e1667
=========================================================
block : 8
data  : important info 8
actual hash : b917213dd781fd9906262514689711f507d67f2a9224f28ac696a4dbea0e1667
NODE prev hash : 43a4d4f176bbc76bb193159266ca8536bf61a18f8876fd7e190bab7d1d3d2f56
prev hash : 43a4d4f176bbc76bb193159266ca8536bf61a18f8876fd7e190bab7d1d3d2f56
=========================================================
block : 7
data  : important info 7
actual hash : 43a4d4f176bbc76bb193159266ca8536bf61a18f8876fd7e190bab7d1d3d2f56
NODE prev hash : c23d85c4db7bef519ca83543653b77c0373abd83505155d12bfd68e31d7a9c9b
prev hash : c23d85c4db7bef519ca83543653b77c0373abd83505155d12bfd68e31d7a9c9b
=========================================================
block : 6
data  : important info 6
actual hash : c23d85c4db7bef519ca83543653b77c0373abd83505155d12bfd68e31d7a9c9b
NODE prev hash : 70993d6cfa65d73473f9a8d8e91b4bf2831e2445958316b016c0bdb96b3d6a5a
prev hash : 70993d6cfa65d73473f9a8d8e91b4bf2831e2445958316b016c0bdb96b3d6a5a
=========================================================
block : 5
data  : Superior LINUX
actual hash : 70993d6cfa65d73473f9a8d8e91b4bf2831e2445958316b016c0bdb96b3d6a5a
NODE prev hash : b7098b658bdb4b8e0e871464ba371711c0f885a1f5ed69ca2a63b65c9ef16d78
prev hash : b7098b658bdb4b8e0e871464ba371711c0f885a1f5ed69ca2a63b65c9ef16d78
=========================================================
block : 4
data  : important info 4
actual hash : b7098b658bdb4b8e0e871464ba371711c0f885a1f5ed69ca2a63b65c9ef16d78
NODE prev hash : 64327441085190e7602b1bb76b58151b5fa9a1dc4161aa6f88fd76cd9f5eddff
prev hash : 64327441085190e7602b1bb76b58151b5fa9a1dc4161aa6f88fd76cd9f5eddff
=========================================================
block : 3
data  : important info 3
actual hash : 64327441085190e7602b1bb76b58151b5fa9a1dc4161aa6f88fd76cd9f5eddff
NODE prev hash : c43b45b6cef1055f8dfa995786279d3f603e4f3dd85e73dfe7f0e21fad362e1d
prev hash : c43b45b6cef1055f8dfa995786279d3f603e4f3dd85e73dfe7f0e21fad362e1d
=========================================================
block : 2
data  : important info 2
actual hash : c43b45b6cef1055f8dfa995786279d3f603e4f3dd85e73dfe7f0e21fad362e1d
NODE prev hash : 01a0d24f7eb166a2ca006f23d34c0f13ddc9b25b1ef99e9a01d10f5d4d3917e1
prev hash : 01a0d24f7eb166a2ca006f23d34c0f13ddc9b25b1ef99e9a01d10f5d4d3917e1
=========================================================
=======================================================
checkChainConsistencyfor block : 9
DATA CHECKED 1: important info 9
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 8
DATA CHECKED 1: important info 8
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 7
DATA CHECKED 1: important info 7
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 6
DATA CHECKED 1: important info 6
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 5
DATA CHECKED 1: Superior LINUX
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 4
DATA CHECKED 1: important info 4
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 3
DATA CHECKED 1: important info 3
 HASH OK ==> Block Healthy !! 
==============================================================================
checkChainConsistencyfor block : 2
DATA CHECKED 1: important info 2
 HASH OK ==> Block Healthy !! 
==============================================================================



-------------------------------------------------------*/