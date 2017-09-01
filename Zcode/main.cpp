#include <iostream>
#include "tree/node/node_crtp.hpp"

int main()
{
    using node_type = Node<2, std::size_t>;
    
    node_type node{2};

    //node.print_value( std::cout );
    std::cout << node << std::endl;

    //using slot_type = Slot<node_type, 2, std::size_t>;
    using slot_type = Slot<node_type>;
    slot_type slot{3, 10};
    slot.push_back(node);
    std::cout << slot << std::endl;

    using sslot_type = Slot< slot_type >;
    sslot_type sslot{14, 10};
    sslot.push_back(slot);
    sslot.push_back(slot);
    std::cout << sslot << std::endl;

    return 0;
}
