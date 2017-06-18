

#ifndef YUE_CORE_SHUFFLE_HPP
#define YUE_CORE_SHUFFLE_HPP

#include <map>
#include <vector>
#include <iostream>
#include <random>
#include <cmath>
#include <functional>
#include <algorithm>

namespace yue {
namespace core {

namespace shuffle_util {
    template<typename elem>
    struct element_t {
        const elem* item;
        float score;
        int   index;
    };
}

/**
 * @brief randomly shuffle elements in a vector
 * @returns a new vector with the elements in data randomly distributed.
 *
 * This function randomly shuffles an input vector, and returns the new vector.
 * It attempts to evenly distribute based on the class of element.
 *
 * For example, if you had 9 balls, 3 of each color: red, blue, green.
 * then no ball will appear next to a ball of the same color.
 *
 * @param elem the type of element to shuffle
 * @param T    a group type T
 *             elements in data can be assigned to a group of type T
 * @param data a vector of elements
 * @param Key function/lambda accepts an elem* and returns type T
 *            use to group elements by a user defined similarity metric.
 *
 * @remark, the elements inside the data vector will be copied (once).
 *      use basic types when possible. This is so that duplicate
 *      elements in the original list are not lost.
 */

template<class elem>
void
fisher_yates(std::vector<elem>& data)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    size_t n = data.size() - 1;
    for (size_t i=0;i<n-1;i++) {
        std::uniform_int_distribution<size_t>dis(i, n);
        size_t j = dis(gen);
        std::iter_swap(data.begin()+i,data.begin()+j);
    }
}

/**
 * @brief randomly shuffle elements in a vector
 * @returns a new vector with the elements in data randomly distributed.
 *
 * this improves on the previous implementation by first randomly
 * shuffling the input. The original version shuffles the elements
 * but the order of the elements within a single group will not change.
 *
 *  https://labs.spotify.com/2014/02/28/how-to-shuffle-songs/
 *  Fisher-Yates
 *      for i in range(0,N)
 *          j = random.randint(0,i)
 *          a[j], a[i] = a[i], a[j]
 */
template<class container, class elem, class T>
container
shuffle(const container& data, std::function<const T (const elem&)> key)
{
    typedef struct shuffle_util::element_t<elem> elem_t;

    if (data.size()==0) {
        return container();
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0, 1);

    std::map<T,int> groups;
    std::map<T,float> group_offset;

    std::vector<elem_t> items;
    items.reserve(data.size());

    //

    // first copy the items over to a new array
    // this is done before group assignments so
    // that the data can be shuffled first.
    for (auto& e : data) {
        items.push_back({&e,0,0});
    }

    // shuffle this array, to randomize the order of the sub groups.
    fisher_yates<elem_t>(items);

    // O(N)
    // determine the groups in the data.
    // count the number of elements in each group
    // then assign an index to each element
    for (auto& e : items) {
        const T k = key(*e.item);
        if (groups.count(k)==0) {
            groups[k] = 1;
        } else {
            groups[k] += 1;
        }
        e.index = groups[k]-1;
    }

    // O(G)
    // assign an unique offset for each group
    for ( auto& it : groups) {
        float h = log(groups.size()) / log(2.0);
        group_offset[it.first] = h*dis(gen) - h/2.0;
    }

    // O(N)
    // assign a position between 0 and N for each element
    // randomly shift each position slightly and apply
    // the group offset
    for (auto& item : items) {
        const T k = key(*item.item);
        // determine the index for this element between 0 and N
        float i = (items.size() * (static_cast<float>(item.index) / groups[k]));
        // determine average delta between two points
        float d = ((float)items.size()) / groups[k] / 2.0;
        //
        float o = d*dis(gen) - d/2.0 + group_offset[k];
        item.score = o + i;
    }

    // O(NLOGN)
    // sort all elements by their score - "randomizing the order"
    std::sort(items.begin(), items.end(),
        [](const elem_t &e1, const elem_t &e2){
            return e1.score < e2.score;});

    // unpack the shuffled elements
    // note that this will copy the elements
    container new_data;
    new_data.reserve(data.size());

    // O(N)
    for (auto& item : items) {
        //std::cout << key(*item.item) << " : " << item.score << std::endl;
        new_data.push_back(*item.item);
    }

    return new_data;
}



} //core
} // yue

#endif

