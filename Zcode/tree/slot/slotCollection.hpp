#pragma once
#include <algorithm>
#include <array>
#include <vector>
#include <memory>
#include <set>
#include <iomanip>
#include <utility>
#include <tbb/tbb.h>
#include <tree/slot/slot.hpp>
#include <tree/slot/cache.hpp>


template < std::size_t dim, typename node_value_type >
struct slotCollection : private std::vector< std::shared_ptr< slot<dim, node_value_type> > >,
                        public definitions<dim, node_value_type>
// struct slotCollection:private std::vector<slot<dim, node_value_type>>,
//                       public definitions<dim, node_value_type>
{
    using slot_type = slot<dim, node_value_type>;
    using node_type = typename slot_type::node_type;
    using definition = definitions<dim, node_value_type>;

    using definition::nlevels;
    using definition::AllOnes;
    
    using parent = std::vector<std::shared_ptr<slot_type>>;
    //using parent = std::vector<slot_type>;
    using parent::operator[];
    using parent::push_back;
    using parent::insert;
    using parent::erase;
    using parent::reserve;
    using parent::resize;
    using parent::begin;
    using parent::end;
    using parent::cbegin;
    using parent::cend;
    using parent::size;
    using parent::capacity;
    using parent::shrink_to_fit;


    // static const std::size_t nlevels = node_type::nlevels;
    // static constexpr std::array<node_value_type, nlevels> AllOnes = node_type::AllOnes;

    //static const std::size_t sizestt=SLOTCOLLECTION_SIZESTT;

    std::array<std::size_t, nlevels+1> countlev;//! used to count the Nodes level by level
    std::size_t breaksize;//!< size of slot which triggers decomposition of a slot.
    //! cache used to avoid repetitive search of slots.
    Cache<dim, node_value_type> cache;
    std::size_t countNodes;//!< number of Nodes stored in slots.
    std::size_t maxslotsize;//! maximum size of slots stored.
    std::size_t dupsize;//!< size of slot which triggers fusion of two slots.
    node_type smax; //!< max. value of hash function for Nodes.

    //! define order on the Nodes. We use the Peano-Hilbert curve for indexation,
      //! and thus, we must suppress all what is not position.
    struct ltNode
    {
        inline bool operator()(const node_type n1,const node_type n2) const
        {
        return (n1&node_type::maskpos)<(n2&node_type::maskpos);
        }
    };

    using setNode = std::set<node_type, ltNode>;

    slotCollection() = default;
    
    slotCollection(std::size_t _nslots, 
                   std::size_t _slotsize, 
                   std::size_t _dupsize, 
                   std::size_t _breaksize):
                   dupsize{_dupsize},
                   breaksize{_breaksize},
                   smax{AllOnes[nlevels-1]},
                   countNodes{0}
    {
        reserve(_nslots);
        push_back(std::make_shared<slot_type>(0, smax, _slotsize));
        cache.putSlot((*this)[0]);
    }

    // remark: this copy constructor doesn't set the same capacity of the copied slotCollection
    slotCollection(slotCollection const& SC):
        dupsize{SC.dupsize},
        breaksize{SC.breaksize},
        smax{AllOnes[nlevels-1]},
        countNodes{SC.countNodes}
    {
        for(std::size_t i=0; i<SC.size(); ++i)
            push_back(std::make_shared<slot_type>(*SC[i]));
        cache.putSlot((*this)[0]);
    }
    
    //! An other "copy init". Here we put the Nodes of each slot in a global
    //! array. This is supposed to reduce the number of allocations. We use
    //! this to store a local copy of a SlotCollection.
    //! \param SC slot collection to be copyed.
    //! \param G global array.
    void copyInArray(std::vector<node_type>& array)
    {
        tbb::parallel_for_each(cbegin(), cend(), [&array](auto &sl){sl->copyInArray(array.data()+sl->Startrank());});
    }

    //! make a "clone", ie copy all, but not the Nodes!
    // \parameter  C SlotCollection to be "cloned"
    inline void clone(const slotCollection& C)
    {
        countNodes=0; //! not a copy of C!
        dupsize=C.dupsize; breaksize=C.breaksize;
        smax=C.smax;
        for(std::size_t i=0; i<C.size(); ++i)
            push_back(std::make_shared<slot_type>(C[i]->s1, C[i]->s2, C[i]->size()*node_type::treetype));
        reinitCache();
    }

    //! swap content of two SlotCollections:
    //! \param C "partner" SlotCollection.
    inline void swap(slotCollection& C)
    {
        // FIX ?? : do we have to also swap the capacity ?
        std::swap(*this, C);
        reinitCache(); 
        C.reinitCache();
    }

    //---------------------------------------------------------------------
    //! Reinitialise cache when modifications make it invalid.
    inline void reinitCache()
    {
        cache.putSlot((*this)[0]);
    }

    //! is a Node abscissa in the interval [s1,s2[ ?
    // \param s1
    // \param s2
    // \param x Node to check.
    //! \note x must be *not* hashed.
    inline bool inInterval(node_type s1, node_type s2, node_type x) const
    {
        node_type xabs = x.hash()&node_type::maskpos;
        return (s1<=xabs) && (s2>xabs);
    }

    //! store one node.
    //! \param x Node
    //! \param cach this version  uses an external Cache (this is thread safe)
    inline void put(node_type x, Cache<dim, node_value_type>& cach)
    {
        node_type xh = x.hash(), xabs = xh&node_type::maskpos;
        auto stloc = cach.find(xabs);
        if (stloc == nullptr)
        {
            stloc = *this[findSlot(xabs, 0, size()-1)];
            cach.putSlot(stloc);
        }
        stloc->put(xh);
        ++countNodes;
    }

    //! number of Nodes stored.
    inline std::size_t nbNodes() const 
    {
        return countNodes;
    }
    
    //! Set the Number of Nodes stored (dangerous!)
    //! \param n number of Nodes
    inline void setnbNodes(int n) const 
    {
        countNodes=n;
    }

    //! number of Nodes, by level
    inline std::size_t* nbNodesByLevel() const 
    {
        return countlev;
    }

    //! return a pointer to a slot which *possibly* contains a Node.
    //! \param x Node *not* *hashed*
    inline auto ubound(node_type x) const
    {
        typename node_type::type maskpos = node_type::maskpos;
        return (*this)[findSlot(x.hash()&maskpos, 0, size()-1)];
    }
    //! return a pointer to a slot which *possibly* contains a Node.
    //! \param x Node *hashed*
    inline auto ubound_hashed(node_type x) const
    {
        typename node_type::type maskpos = node_type::maskpos;
        return (*this)[findSlot(x&maskpos, 0, size()-1)];
    }

    //! Given a Node, find his slot.
    //! \param N the Node
    //! \param left
    //! \param right
    //! \note we search in range [left,right] of the SlotCollection
    //! \note returns the rank of the slot which contains N
    inline std::size_t findSlot(node_type N, std::size_t left, std::size_t right) const
    {
        std::size_t middle;
        while(true)
        {
            middle = (left+right)/2;
            if ( N < (*this)[middle]->s1)
                right = middle;
            else if( N >= (*this)[middle]->s2)
                left = middle + 1;
            else
            {
                left = middle; 
                break;
            }
        }
        return left;
    }

    //! find a Node.
    //! \param x Node non hashed.
    //! \param cach cache updated.
    //! \note we do not directly check if the Node is really non hashed, but
    // this is checked in "xh=hash(x)".
    inline bool find(node_type x, Cache<dim, node_value_type>& cach) const
    {
        node_type xh = x.hash(), xabs = xh&node_type::maskpos;
        auto stloc = cach.find(xabs);
        if(stloc==nullptr)
        {
            stloc = *this[findSlot(xabs, 0, size()-1)];
            cach.putSlot(stloc);
        }
        std::size_t pos = stloc->find(xh);
        cach.setrankInSlot(pos);
        return pos != -1;
    }

    //! remove all free bits from all nodes.
    inline void forgetFreeBits()
    {
        std::for_each(begin(), end(), [](auto& st){st->forgetFreeBits();});
    }

    //! suppress void Nodes, if any.
    //! update the count of leaves.
    inline void compress(node_type val=node_type::voidbit)
    {
        countNodes = 0;
        std::for_each(begin(), end(), [&val, this](auto& st){st->compress(val); countNodes+=st->Size();});
    }
    //! empty all the slots.
    inline void clear()
    {
        std::for_each(begin(), end(), [](auto& st){st->empty();});
    }

    //! make a copy (in a set) of the Nodes.
    //! \param setN  the set.
    inline void makeExtern(setNode& setN)
    {
        for (auto&st: this)
            for(std::size_t i=0; i<st->size(); ++i)
                setN.insert(st[i]);
    }

    //! finalize: compute cumulsize (to allow rank function to work), and maximum
    //! size of slots;
    inline void finalize()
    {
        countlev.fill(0);

        //cache is probably invalid; reinitialize it.
        reinitCache();

        std::size_t wmax=0;

        countNodes = 0;
        maxslotsize = 0;
        *this[0]->setStartrank(0);
        *this[0]->setSlotrank(0);
        for(std::size_t i=0; i<size(); ++i)
        {
            if (i > 0)
                *this[i]->setStartrank(*this[i-1]->Startrank()+wmax);
            wmax = *this[i]->size();
            countNodes += wmax;
            for(std::size_t j=0; j<wmax; ++j)
                ++countlev[(*this[i])[j].level()];
            maxslotsize = std::max(maxslotsize, wmax);
            *this[i]->setSlotrank(i);
        }
    }

    //! put the countlevs values... not very clean. See TTree.
    //! \param p
    inline void putMaxs(const std::size_t* p)
    {
        for(std::size_t i=0; i<nlevels; ++i)
            countlev[i] = p[i];
    }
    
    //! get the vector of the number of Nodes by level.
    inline auto getMaxs() const 
    {
        return countlev;
    }

    //! compute ranks, maxslotsize...
    inline void relink()
    {
        countNodes=0;
        maxslotsize=0;
        std::size_t wmax=0;
        *this[0]->setStartrank(0);
        *this[0]->setSlotrank(0);
        for(std::size_t i=0; i<size(); ++i)
        {
            if(i>0)
                *this[i]->setStartrank(*this[i-1]->Startrank()+wmax);
            wmax = *this[i]->Size();
            countNodes += wmax;
            maxslotsize = std::max(maxslotsize,wmax);
            *this[i]->setSlotrank(i);
        }
    }
    //!return maximum size of slots.
    inline std::size_t MaxSlotSize() const 
    {
        return maxslotsize;
    }
    

};

template<std::size_t dim, typename node_value_type>
std::ostream& operator<<(std::ostream& os, const slotCollection<dim, node_value_type>& st)
{
    os << "slotCollection\n";
    os << st.smax << "\n";
    for_each(st.begin(), st.end(), [](auto &sl){std::cout << *sl.get() << "\n";});
    return os;
}
