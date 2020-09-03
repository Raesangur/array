/**
 * \file
 * \author  Pascal-Emmanuel Lachance
 * \p       https://www.github.com/Raesangur
 * ------------------------------------------------------------------------------------------------
 * MIT License
 * Copyright (c) 2020 Pascal-Emmanuel Lachance | Ràësangür
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#pragma once
#include "./array.hpp"

#include <ostream>
#include <sstream>

namespace pel
{

/*************************************************************************************************/
/* Defines ------------------------------------------------------------------------------------- */
#define ARRAY_TEMPLATE_DECLARATION__ typename ItemType, std::size_t ItemCount
#define ARRAY_CLASS_SCOPE__          array<ItemType, ItemCount>


/**
 **************************************************************************************************
 * \brief       Overload of the left-shift << operator to print an array's
 *              content to an output stream.
 *
 * \param       os_:  Left-hand-side output stream.
 * \param       arr_: Right-hand-side vector to print.
 *
 * \retval      std::ostream&: Reference the output stream after appending data.
 *
 * \note        This method is not directly part of the pel::array class, and is rather appended
 *              to the std::ostream class.
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
inline static std::ostream&
operator<<(std::ostream& os_, const array<ItemType, ItemCount>& arr_) noexcept
{
    /* Add capacity and length header */
    os_ << "Length: [" << arr_.length() << "]\n";

    for(ItemType& element : arr_)
    {
        os_ << element << '\n';
    }

    return os_;
}


/*************************************************************************************************/
/* CONSTRUCTORS & DESTRUCTORS ------------------------------------------------------------------ */
/*************************************************************************************************/


/**
 **************************************************************************************************
 * \brief       Default-value constructor for the array class.
 *
 * \param       value_:  Value to initialize all the elements initially allocated with.
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
constexpr ARRAY_CLASS_SCOPE__::array(const ItemType& value_)
{
    array_constructor();

    /* clang-format off */
    std::for_each(begin(), end(),
                  [&](ItemType& item)
                  {
                      AllocatorTraits::construct(m_allocator, &item, value_);
                  });
    /* clang-format on */
}


/**
 **************************************************************************************************
 * \brief       Iterator-based copy constructor for the array class.
 *
 * \param       beginIterator_: Begin iterator of another array to start copying from.
 * \param       endIterator_:   End iterator of another array to end the copy.
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
constexpr ARRAY_CLASS_SCOPE__::array(const IteratorType beginIterator_,
                                     const IteratorType endIterator_)
{
    if(std::is_constant_evaluated())
    {
        m_size = endIterator_ - beginIterator_;
    }
    else
    {
        check_fit(endIterator_ - beginIterator_);
    }

    array_constructor();

    std::copy(beginIterator_, endIterator_, begin());
}


/**
 **************************************************************************************************
 * \brief       Copy constructor for the array class.
 *
 * \param       otherArray_: Array to copy data from.
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
template<std::size_t OtherSize>
constexpr ARRAY_CLASS_SCOPE__::array(const array<ItemType, OtherSize>& otherArray_)
{
    if(std::is_constant_evaluated())
    {
        m_size = otherArray_.length();
    }
    else
    {
        check_fit(otherArray_.length());
    }

    array_constructor();

    std::copy(otherArray_.begin(), otherArray_.end(), begin());
}

template<ARRAY_TEMPLATE_DECLARATION__>
constexpr ARRAY_CLASS_SCOPE__::array(const array& otherArray_)
{
    array_constructor();

    std::copy(otherArray_.begin(), otherArray_.end(), begin());
}

/**
 **************************************************************************************************
 * \brief       Copy assignment operator for the array class.
 *
 * \param       copy_: Array to copy data from.
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
template<std::size_t OtherSize>
constexpr inline array<ItemType, ItemCount>&
ARRAY_CLASS_SCOPE__::operator=(const array<ItemType, OtherSize>& copy_)
{
    if(std::is_constant_evaluated())
    {
        m_size = copy_.length();
    }
    else
    {
        check_fit(copy_.length());
    }

    std::copy(copy_.begin(), copy_.begin(), begin());

    return *this;
}

template<ARRAY_TEMPLATE_DECLARATION__>
constexpr inline array<ItemType, ItemCount>&
ARRAY_CLASS_SCOPE__::operator=(const array<ItemType, ItemCount>& copy_)
{
    return operator=<ItemType, ItemCount>(copy_);
}


/**
 **************************************************************************************************
 * \brief       Move constructor for the vector class.
 *
 * \param       otherVector_: Vector to move data from.
 * \param       alloc_:       Allocator to use for all memory allocations
 *              [defaults : AllocatorType{}]
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
template<std::size_t OtherSize>
constexpr ARRAY_CLASS_SCOPE__::array(array<ItemType, OtherSize>&& move_)
{
    if(std::is_constant_evaluated())
    {
        m_size = move_.length();
    }
    else
    {
        check_fit(move_.length());
    }

    array_constructor();

    std::move(move_.begin(), move_.end(), begin());
}

/**
 **************************************************************************************************
 * \brief       Move assignment operator for the vector class.
 *
 * \param       move__: Vector to move data from.
 *
 * \note        Will do nothing if attempting to move a vector into itself
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
template<std::size_t OtherSize>
constexpr inline array<ItemType, ItemCount>&
ARRAY_CLASS_SCOPE__::operator=(array<ItemType, OtherSize>&& move_)
{
    if(this != std::addressof(move_))
    {
        /* Grab the other vector's resources */
        std::move(move_.begin(), move_.end(), begin());

        /* Invalidate the other vector */
        move_.m_beginIterator = IteratorType{nullptr};
        move_.m_endIterator   = IteratorType{nullptr};
    }
    return *this;
}

/**
 **************************************************************************************************
 * \brief       Initializer list constructor for the vector class.
 *
 * \param       ilist_: Initializer list of all the values to put in a new vector.
 * \param       alloc_: Allocator to use for all memory allocations
 *              [defaults : AllocatorType{}]
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
constexpr ARRAY_CLASS_SCOPE__::array(InitializerListType ilist_)
{
    check_fit(ilist_.size());

    array_constructor();

    std::copy(ilist_.begin(), ilist_.end(), begin());
}


/**
 **************************************************************************************************
 * \brief       Move-based constructor for the vector class.
 *              Takes the arguments used to  an `ItemType` object, and forwards them.
 *
 * \param       length_ : Number of elements to create
 * \param       args_ :   Arguments to forward to the `ItemType` constructor
 * \param       alloc_:   Allocator to use for all memory allocations
 *              [defaults : AllocatorType{}]
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
template<typename... Args>
constexpr ARRAY_CLASS_SCOPE__::array(Args&&... args_)
{
    /* clang-format off */
    auto builder = [&](ItemType& element)
                   {
                       AllocatorTraits::construct(m_allocator,
                                                  &element,
                                                  std::forward<Args>(args_)...);
                   };
    /* clang-format on */

    array_constructor();

    std::for_each(begin(), end(), builder);
}


/**
 **************************************************************************************************
 * \brief       Generator-taking constructor for the vector class.
 *              Takes a generator function as parameter to create `length_` items in the vector.
 *
 * \param       length_ :   Number of elements to create
 * \param       function_ : Function (returning `ItemType` and taking no arguments) called to
 *                          initialize all the values in the vector.
 * \param       alloc_:     Allocator to use for all memory allocations
 *              [defaults : AllocatorType{}]
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
constexpr ARRAY_CLASS_SCOPE__::array(std::function<ItemType(void)> function_)
{
    array_constructor();

    std::generate(begin(), end(), function_);
}


/**
 **************************************************************************************************
 * \brief       Destructor for the array class.
 *************************************************************************************************/
//template<ARRAY_TEMPLATE_DECLARATION__>
//ARRAY_CLASS_SCOPE__::~array()
//{
//    /* Destroy elements in the allocated memory */
//    std::destroy(begin(), end());
//}


/*************************************************************************************************/
/* ELEMENT ACCESSORS --------------------------------------------------------------------------- */
/*************************************************************************************************/


/**
 **************************************************************************************************
 * \brief       Get a pointer to the beginning of the vector's data space.
 *
 * \retval      ItemType*: Pointer to the beginning of the vector's data.
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
[[nodiscard]] constexpr inline ItemType*
ARRAY_CLASS_SCOPE__::data() noexcept
{
    return begin().ptr();
}


/**
 **************************************************************************************************
 * \brief       Get a const pointer to the beginning of the vector's data space.
 *
 * \retval      ItemType*: Const pointer to the beginning of the vector's data.
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
[[nodiscard]] constexpr inline const ItemType*
ARRAY_CLASS_SCOPE__::data() const noexcept
{
    return begin().ptr();
}


/**
 **************************************************************************************************
 * \brief       Assign a value to a certain offset in the vector for a certain amount of elements.
 *
 * \param       value_:  Value to assign to the vector.
 * \param       offset_: Offset at which data should be assigned.
 *              [defaults : 0]
 * \param       count_:  Number of elements to be assigned a new value.
 *              [defaults : 1]
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
constexpr inline void
ARRAY_CLASS_SCOPE__::assign(const ItemType& value_, DifferenceType offset_, SizeType count_)
{
    if constexpr(array_safeness == true)
    {
        check_fit(count_ + offset_);
    }

    std::fill_n(begin() + offset_, count_, value_);
}


/**
 **************************************************************************************************
 * \brief       Assign values to a certain offset in the vector through an initializer list.
 *
 * \param       ilist_:  Values to assign to the vector.
 * \param       offset_: Offset at which data should be assigned.
 *              [defaults : 0]
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
constexpr inline void
ARRAY_CLASS_SCOPE__::assign(InitializerListType ilist_, DifferenceType offset_)
{
    if constexpr(array_safeness == true)
    {
        check_fit(ilist_.size() + offset_);
    }

    std::copy(ilist_.begin(), ilist_.end(), begin() + offset_);
}


/*************************************************************************************************/
/* MISC ---------------------------------------------------------------------------------------- */
/*************************************************************************************************/

/**
 **************************************************************************************************
 * \brief       Convert the content of a vector to a string.
 *
 * \retval      A string containing the capacity, the size, and all the elements converted to a
 *              string.
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
[[nodiscard]] constexpr inline std::string
ARRAY_CLASS_SCOPE__::to_string() const
{
    std::ostringstream os;
    os << *this;
    return os.str();
}


/*************************************************************************************************/
/* PRIVATE METHODS ----------------------------------------------------------------------------- */
/*************************************************************************************************/

/**
 **************************************************************************************************
 * \brief       Check if the vector is big enough to hold the required extra elements.
 *              If it is not currently big enough, reserve some memory.
 *
 * \param       extraLength_: Numbers of elements to add to the current length.
 *************************************************************************************************/
template<ARRAY_TEMPLATE_DECLARATION__>
constexpr inline void
ARRAY_CLASS_SCOPE__::check_fit(SizeType size_) const
{
    if constexpr(array_safeness == true)
    {
        if(size_ > m_size)
        {
            throw(std::length_error("Data couldn't fit in array"));
        }
    }
}

template<ARRAY_TEMPLATE_DECLARATION__>
constexpr inline void
ARRAY_CLASS_SCOPE__::array_constructor()
{
    m_beginIterator.ptr() = &m_data[0];
    m_endIterator.ptr()   = m_data + m_size;
}


/*************************************************************************************************/
/* Undefines ----------------------------------------------------------------------------------- */
#undef ARRAY_TEMPLATE_DECLARATION__
#undef ARRAY_CLASS_SCOPE__

}        // namespace pel


/*************************************************************************************************/
/* END OF FILE --------------------------------------------------------------------------------- */
/*************************************************************************************************/
