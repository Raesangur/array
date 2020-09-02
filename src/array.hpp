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

/*************************************************************************************************/
/* File includes ------------------------------------------------------------------------------- */
#include "./container_base/src/container_base.hpp"

#include <algorithm>
#include <functional>
#include <type_traits>


namespace pel
{
constexpr bool array_safeness = true;

template<typename ItemType>
using array_iterator = iterator_base<ItemType>;

template<typename ItemType, std::size_t ItemCount>
class array : public container_base<ItemType, array_iterator<ItemType>>
{
public:
    /*********************************************************************************************/
    /* Type definitions ------------------------------------------------------------------------ */

    using SizeType            = std::size_t;
    using DifferenceType      = std::ptrdiff_t;
    using IteratorType        = array_iterator<ItemType>;
    using RIteratorType       = typename IteratorType::ReverseIteratorType;
    using InitializerListType = std::initializer_list<ItemType>;


    /*********************************************************************************************/
    /* Constructors ---------------------------------------------------------------------------- */
    explicit array() = default;
    explicit array(const ItemType& value_);
    explicit array(IteratorType beginIterator_, IteratorType endIterator_);

    /*-----------------------------------------------*/
    /* Copy constructor and copy-assignment operator */
    template<SizeType OtherSize>
    explicit array(const array<ItemType, OtherSize>& copy_);
    explicit array(const array& copy_);

    template<SizeType OtherSize>
    array& operator=(const array<ItemType, OtherSize>& copy_);
    array& operator=(const array& copy_);

    /*-----------------------------------------------*/
    /* Move constructor and move-assignment operator */
    template<SizeType OtherSize>
    explicit array(array<ItemType, OtherSize>&& move_);
    template<SizeType OtherSize>
    array& operator=(array<ItemType, OtherSize>&& move_);


    /*----------------------*/
    /* Special constructors */
    array(InitializerListType ilist_);

    template<typename... Args>
    explicit array(Args&&... args_);

    explicit array(std::function<ItemType(void)> function_);

    /*------------*/
    /* Destructor */
    ~array() override;


    /*********************************************************************************************/
    /* Element accessors ----------------------------------------------------------------------- */
    [[nodiscard]] ItemType*       data() noexcept;
    [[nodiscard]] const ItemType* data() const noexcept;

    void assign(const ItemType& value_, DifferenceType offset_ = 0, SizeType count_ = 1);
    void assign(InitializerListType ilist_, DifferenceType offset_ = 0);


    /*********************************************************************************************/
    /* Misc ------------------------------------------------------------------------------------ */
    [[nodiscard]] std::string to_string() const override;


    /*********************************************************************************************/
    /* Private methods ------------------------------------------------------------------------- */
private:
    void check_fit(SizeType size_) const;

    void array_constructor();


    /*********************************************************************************************/
    /* Variables ------------------------------------------------------------------------------- */
private:
    constexpr static SizeType m_size = ItemCount;
    ItemType           m_data[m_size];
};

}        // namespace pel


#include "./array.inl"

/*************************************************************************************************/
/* ----- END OF FILE ----- */
