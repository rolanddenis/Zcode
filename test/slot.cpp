#include <iostream>
#include <array>
#include <vector>
#include <tree/node/node.hpp>
#include <tree/node/neighbor.hpp>
#include <tree/slot/slot.hpp>

int main()
{
    typedef unsigned short node_type;
    std::size_t const dim = 2;
    slot<2, node_type> s(10);
    Node<dim, node_type> n, n1;
    n += Node<dim, node_type>::Xbit; 
    n.set_level(1);

    std::size_t const stencil = 1;
    std::array<Node<dim, node_type>, ipow(2*stencil+1, dim)> box;
    boxNeighbors<stencil>(n, box);
    std::vector<Node<dim, node_type>> vbox;
    vbox.insert(vbox.end(), box.cbegin(), box.cend());
    s.put(vbox);

    std::cout << "Avant\n";
    for(auto &ss: s)
        std::cout << ss << "\n";
    
    s.setMark(Node<dim, node_type>::voidbit);
    // s.compress(Node<dim, node_type>::voidbit);


    s.changeMark(Node<dim, node_type>::voidbit, Node<dim, node_type>::voidbit>>1);

    std::cout << s.countAndEq(Node<dim, node_type>::voidbit>>1) << "\n";
    std::cout << s.countAndEq(Node<dim, node_type>::voidbit>>1) << "\n";
    s.And(Node<dim, node_type>::maskpos);

    auto news = s.cut(2);
    for(auto &ns: news)
    {
        std::cout << "\n**********************\n";
        std::cout << "s1: " << ns.s1 << "\n";
        std::cout << "s2: " << ns.s2 << "\n";
        for(auto &ss: ns)
            std::cout << ss << "\n";
    }

    auto nnews = news[1].cutBefore(2, news[0].s1);

    std::cout << "\n**********************\n";
    std::cout << "s1: " << nnews.s1 << "\n";
    std::cout << "s2: " << nnews.s2 << "\n";
    for(auto &ss: nnews)
        std::cout << ss << "\n";
    
    std::cout << "\n**********************\n";
    std::cout << "s1: " << news[1].s1 << "\n";
    std::cout << "s2: " << news[1].s2 << "\n";
    for(auto &ss: news[1])
        std::cout << ss << "\n";

    // s.insert(s.end(), {3, 2, 1});
    // s.slotMark = 16;
    // std::cout << s.slotMark << "\n";
    // //s.vCanBeDeleted = false;
    // slot<2> s1(s);

    // std::cout << s1.vCanBeDeleted << "\n";

    // for(auto &ss: s1)
    //     std::cout << ss << "\n";

    // Node<2> n{5};
    // auto result = s1.find(n);
    // if (result == std::end(s1))
    //     std::cout << "not found\n";
    // else
    //     std::cout << "found\n";
    
    // //std::cout << s1.find(n) << "\n";
        
}