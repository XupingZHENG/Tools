#pragma once

#include <vector>

template<typename Type>
struct Group
{
    Group(int threshold = 20) : thresh(threshold), avgVal(0), minVal(0), maxVal(0) {}

    Group(Type item, int threshold = 20) :
        thresh(threshold), avgVal(item), minVal(item), maxVal(item)
    {
        items.push_back(item);
    }

    bool tryAdd(Type item)
    {
        if (abs(item - avgVal) > thresh)
            return false;

        Type val = item;
        avgVal = (avgVal * items.size() + val) * (1.0 / (items.size() + 1));
        minVal = std::min(minVal, val);
        maxVal = std::max(maxVal, val);

        items.push_back(item);

        return true;
    }

    Type thresh;
    Type avgVal;
    Type minVal, maxVal;
    std::vector<Type> items;
};

template<typename Type>
void groupItems(const std::vector<Type>& items, Type thresh, std::vector<Group<Type> >& groups)
{
    groups.clear();

    for (const Type& item : items)
    {
        bool ok = false;
        for (Group<Type>& group : groups)
        {
            if (group.tryAdd(item))
            {
                ok = true;
                break;
            }
        }
        if (!ok)
            groups.push_back(Group<Type>(item, thresh));
    }

    std::sort(groups.begin(), groups.end(),
        [](const Group<Type>& lhs, const Group<Type>& rhs) { return lhs.avgVal > rhs.avgVal; });
}

template<typename ItemType, typename ComputeType, typename GetValueFunc>
struct Group2
{
    Group2(GetValueFunc func_, ComputeType threshold_ = 20) :
        thresh(threshold_), avgVal(0), minVal(0), maxVal(0), func(func_) {}

    Group2(ItemType item_, GetValueFunc func_, ComputeType threshold_ = 20) :
        thresh(threshold_), avgVal(func_(item_)), minVal(func_(item_)), maxVal(func_(item_)), func(func_)
    {
        items.push_back(item_);
    }

    bool tryAdd(ItemType item)
    {
        if (abs(func(item) - avgVal) > thresh)
            return false;

        ComputeType val = func(item);
        avgVal = (avgVal * items.size() + val) * (1.0 / (items.size() + 1));
        minVal = std::min(minVal, val);
        maxVal = std::max(maxVal, val);

        items.push_back(item);

        return true;
    }

    ComputeType thresh;
    ComputeType avgVal;
    ComputeType minVal, maxVal;
    std::vector<ItemType> items;
    GetValueFunc func;
};

template<typename ItemType, typename ComputeType, typename GetValueFunc>
void groupItems2(const std::vector<ItemType>& items, GetValueFunc func, ComputeType thresh, 
    std::vector<Group2<ItemType, ComputeType, GetValueFunc> >& groups)
{
    groups.clear();

    for (const ItemType& item : items)
    {
        bool ok = false;
        for (Group2<ItemType, ComputeType, GetValueFunc>& group : groups)
        {
            if (group.tryAdd(item))
            {
                ok = true;
                break;
            }
        }
        if (!ok)
            groups.push_back(Group2<ItemType, ComputeType, GetValueFunc>(item, func, thresh));
    }

    std::sort(groups.begin(), groups.end(),
        [](const Group2<ItemType, ComputeType, GetValueFunc>& lhs, const Group2<ItemType, ComputeType, GetValueFunc>& rhs) 
        { return lhs.avgVal > rhs.avgVal; });
}