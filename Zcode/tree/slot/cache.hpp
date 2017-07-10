#pragma once
#include <memory>

#include "tree/slot/slot.hpp"

template<std::size_t dim, typename value_type> 
struct Cache
{
    static const int size = 10;
    using slot_type = std::weak_ptr<slot<dim, value_type>>;
    using node_type = typename slot<dim, value_type>::node_type;

    Cache(const Cache&) = default;
    
    mutable std::size_t current, newest;
    std::array<slot_type, size> st; 
    std::array<std::size_t, size> pos;//!< last position found by SlotCollection::find() 
    //ZZ! pos is *not* initialized by SlotCollection::put !! use
    // this->getslot()->lastPos() for this. !!!

    //! constructor
    //! \note does not seem necessary. But gcc warns otherwise.
    Cache()
    {
        reset();
    }

    //! Is a Node in the slots pointed?
    //! \param hashN the Node
    //! \note hashN *must* be hashed!
    inline slot_type find(node_type hashN) const
    {
        auto index = std::find_if(st.cbegin(), st.cend(), [&](auto &n){return (hashN>=n.s1 && hashN<n.s2);});
        if (index != st.end())
        {
            current = std::distance(st.begin(), index);
            return st[current];
        }
        return nullptr;
    }

    //! return a reference to the Node pointed.
    inline node_type& reference() const  
    {
        return st[current][pos[current]];
    }
    //! return a pointer to the Node pointed.
    //inline Node* pointerTo() const  {return st[current]->pointerto(pos[current]);}
    
    //! return a pointer to the slot.
    inline slot_type& getslot() const 
    {
        return st[current];
    }
 
    //! return the rank in the slot.
    inline std::size_t rankInSlot() const 
    {
        return pos[current];
    }

    //! return global rank.
    inline std::size_t globalrank() const 
    {
        return st[current].Startrank() + pos[current];
    }

    //! return the rank of the pointed slot.
    inline std::size_t slotRank() const 
    {
        return st[current].Slotrank();
    }

    //! return the size of pointed slot
    inline std::size_t sizeOfSlot() const 
    {
        return st[current].size();
    }

    //! return s1
    inline node_type S1() const 
    {
        return st[current].s1;
    }

    //! return s2
    inline node_type S2() const 
    {
        return st[current].s2;
    }

    //! put the datas of a slot
    //! \param slo the slot
    inline void putSlot(std::shared_ptr<slot<dim, value_type>>& slo)
    {
        //find the oldest slot:
        //int oldest=(newest+size-1)%size;
        std::size_t oldest = (newest+1)%size;

        st[oldest] = slo;

        newest = oldest;
        current = newest;
    }

    //! set the rank to r.
    //! \param r
    inline void setrankInSlot(std::size_t r)
    {
        pos[current] = r;
    }
    
    //! reset.
    inline void reset()
    {

        // for(int i=0;i<size;i++)
        // {
        // st[i]=0; s1[i]=0;s2[i]=0;pos[i]=0;
        // }
        // current=0; newest=0;
    }
};

