/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "vephor_ext.h"

using namespace vephor;


struct Iris
{
    float sepal_length;
    float sepal_width;
    float petal_length;
    float petal_width;
    string species;
};

//
// The Fisher Iris data set
//
// [1] https://en.wikipedia.org/wiki/Iris_flower_data_set
//
const vector<Iris> iris_dataset{
    {5.1, 3.5, 1.4, 0.2, "setosa"},     {4.9, 3.0, 1.4, 0.2, "setosa"},     {4.7, 3.2, 1.3, 0.2, "setosa"},
    {4.6, 3.1, 1.5, 0.2, "setosa"},     {5.0, 3.6, 1.4, 0.2, "setosa"},     {5.4, 3.9, 1.7, 0.4, "setosa"},
    {4.6, 3.4, 1.4, 0.3, "setosa"},     {5.0, 3.4, 1.5, 0.2, "setosa"},     {4.4, 2.9, 1.4, 0.2, "setosa"},
    {4.9, 3.1, 1.5, 0.1, "setosa"},     {5.4, 3.7, 1.5, 0.2, "setosa"},     {4.8, 3.4, 1.6, 0.2, "setosa"},
    {4.8, 3.0, 1.4, 0.1, "setosa"},     {4.3, 3.0, 1.1, 0.1, "setosa"},     {5.8, 4.0, 1.2, 0.2, "setosa"},
    {5.7, 4.4, 1.5, 0.4, "setosa"},     {5.4, 3.9, 1.3, 0.4, "setosa"},     {5.1, 3.5, 1.4, 0.3, "setosa"},
    {5.7, 3.8, 1.7, 0.3, "setosa"},     {5.1, 3.8, 1.5, 0.3, "setosa"},     {5.4, 3.4, 1.7, 0.2, "setosa"},
    {5.1, 3.7, 1.5, 0.4, "setosa"},     {4.6, 3.6, 1.0, 0.2, "setosa"},     {5.1, 3.3, 1.7, 0.5, "setosa"},
    {4.8, 3.4, 1.9, 0.2, "setosa"},     {5.0, 3.0, 1.6, 0.2, "setosa"},     {5.0, 3.4, 1.6, 0.4, "setosa"},
    {5.2, 3.5, 1.5, 0.2, "setosa"},     {5.2, 3.4, 1.4, 0.2, "setosa"},     {4.7, 3.2, 1.6, 0.2, "setosa"},
    {4.8, 3.1, 1.6, 0.2, "setosa"},     {5.4, 3.4, 1.5, 0.4, "setosa"},     {5.2, 4.1, 1.5, 0.1, "setosa"},
    {5.5, 4.2, 1.4, 0.2, "setosa"},     {4.9, 3.1, 1.5, 0.1, "setosa"},     {5.0, 3.2, 1.2, 0.2, "setosa"},
    {5.5, 3.5, 1.3, 0.2, "setosa"},     {4.9, 3.1, 1.5, 0.1, "setosa"},     {4.4, 3.0, 1.3, 0.2, "setosa"},
    {5.1, 3.4, 1.5, 0.2, "setosa"},     {5.0, 3.5, 1.3, 0.3, "setosa"},     {4.5, 2.3, 1.3, 0.3, "setosa"},
    {4.4, 3.2, 1.3, 0.2, "setosa"},     {5.0, 3.5, 1.6, 0.6, "setosa"},     {5.1, 3.8, 1.9, 0.4, "setosa"},
    {4.8, 3.0, 1.4, 0.3, "setosa"},     {5.1, 3.8, 1.6, 0.2, "setosa"},     {4.6, 3.2, 1.4, 0.2, "setosa"},
    {5.3, 3.7, 1.5, 0.2, "setosa"},     {5.0, 3.3, 1.4, 0.2, "setosa"},     {7.0, 3.2, 4.7, 1.4, "versicolor"},
    {6.4, 3.2, 4.5, 1.5, "versicolor"}, {6.9, 3.1, 4.9, 1.5, "versicolor"}, {5.5, 2.3, 4.0, 1.3, "versicolor"},
    {6.5, 2.8, 4.6, 1.5, "versicolor"}, {5.7, 2.8, 4.5, 1.3, "versicolor"}, {6.3, 3.3, 4.7, 1.6, "versicolor"},
    {4.9, 2.4, 3.3, 1.0, "versicolor"}, {6.6, 2.9, 4.6, 1.3, "versicolor"}, {5.2, 2.7, 3.9, 1.4, "versicolor"},
    {5.0, 2.0, 3.5, 1.0, "versicolor"}, {5.9, 3.0, 4.2, 1.5, "versicolor"}, {6.0, 2.2, 4.0, 1.0, "versicolor"},
    {6.1, 2.9, 4.7, 1.4, "versicolor"}, {5.6, 2.9, 3.6, 1.3, "versicolor"}, {6.7, 3.1, 4.4, 1.4, "versicolor"},
    {5.6, 3.0, 4.5, 1.5, "versicolor"}, {5.8, 2.7, 4.1, 1.0, "versicolor"}, {6.2, 2.2, 4.5, 1.5, "versicolor"},
    {5.6, 2.5, 3.9, 1.1, "versicolor"}, {5.9, 3.2, 4.8, 1.8, "versicolor"}, {6.1, 2.8, 4.0, 1.3, "versicolor"},
    {6.3, 2.5, 4.9, 1.5, "versicolor"}, {6.1, 2.8, 4.7, 1.2, "versicolor"}, {6.4, 2.9, 4.3, 1.3, "versicolor"},
    {6.6, 3.0, 4.4, 1.4, "versicolor"}, {6.8, 2.8, 4.8, 1.4, "versicolor"}, {6.7, 3.0, 5.0, 1.7, "versicolor"},
    {6.0, 2.9, 4.5, 1.5, "versicolor"}, {5.7, 2.6, 3.5, 1.0, "versicolor"}, {5.5, 2.4, 3.8, 1.1, "versicolor"},
    {5.5, 2.4, 3.7, 1.0, "versicolor"}, {5.8, 2.7, 3.9, 1.2, "versicolor"}, {6.0, 2.7, 5.1, 1.6, "versicolor"},
    {5.4, 3.0, 4.5, 1.5, "versicolor"}, {6.0, 3.4, 4.5, 1.6, "versicolor"}, {6.7, 3.1, 4.7, 1.5, "versicolor"},
    {6.3, 2.3, 4.4, 1.3, "versicolor"}, {5.6, 3.0, 4.1, 1.3, "versicolor"}, {5.5, 2.5, 4.0, 1.3, "versicolor"},
    {5.5, 2.6, 4.4, 1.2, "versicolor"}, {6.1, 3.0, 4.6, 1.4, "versicolor"}, {5.8, 2.6, 4.0, 1.2, "versicolor"},
    {5.0, 2.3, 3.3, 1.0, "versicolor"}, {5.6, 2.7, 4.2, 1.3, "versicolor"}, {5.7, 3.0, 4.2, 1.2, "versicolor"},
    {5.7, 2.9, 4.2, 1.3, "versicolor"}, {6.2, 2.9, 4.3, 1.3, "versicolor"}, {5.1, 2.5, 3.0, 1.1, "versicolor"},
    {5.7, 2.8, 4.1, 1.3, "versicolor"}, {6.3, 3.3, 6.0, 2.5, "virginica"},  {5.8, 2.7, 5.1, 1.9, "virginica"},
    {7.1, 3.0, 5.9, 2.1, "virginica"},  {6.3, 2.9, 5.6, 1.8, "virginica"},  {6.5, 3.0, 5.8, 2.2, "virginica"},
    {7.6, 3.0, 6.6, 2.1, "virginica"},  {4.9, 2.5, 4.5, 1.7, "virginica"},  {7.3, 2.9, 6.3, 1.8, "virginica"},
    {6.7, 2.5, 5.8, 1.8, "virginica"},  {7.2, 3.6, 6.1, 2.5, "virginica"},  {6.5, 3.2, 5.1, 2.0, "virginica"},
    {6.4, 2.7, 5.3, 1.9, "virginica"},  {6.8, 3.0, 5.5, 2.1, "virginica"},  {5.7, 2.5, 5.0, 2.0, "virginica"},
    {5.8, 2.8, 5.1, 2.4, "virginica"},  {6.4, 3.2, 5.3, 2.3, "virginica"},  {6.5, 3.0, 5.5, 1.8, "virginica"},
    {7.7, 3.8, 6.7, 2.2, "virginica"},  {7.7, 2.6, 6.9, 2.3, "virginica"},  {6.0, 2.2, 5.0, 1.5, "virginica"},
    {6.9, 3.2, 5.7, 2.3, "virginica"},  {5.6, 2.8, 4.9, 2.0, "virginica"},  {7.7, 2.8, 6.7, 2.0, "virginica"},
    {6.3, 2.7, 4.9, 1.8, "virginica"},  {6.7, 3.3, 5.7, 2.1, "virginica"},  {7.2, 3.2, 6.0, 1.8, "virginica"},
    {6.2, 2.8, 4.8, 1.8, "virginica"},  {6.1, 3.0, 4.9, 1.8, "virginica"},  {6.4, 2.8, 5.6, 2.1, "virginica"},
    {7.2, 3.0, 5.8, 1.6, "virginica"},  {7.4, 2.8, 6.1, 1.9, "virginica"},  {7.9, 3.8, 6.4, 2.0, "virginica"},
    {6.4, 2.8, 5.6, 2.2, "virginica"},  {6.3, 2.8, 5.1, 1.5, "virginica"},  {6.1, 2.6, 5.6, 1.4, "virginica"},
    {7.7, 3.0, 6.1, 2.3, "virginica"},  {6.3, 3.4, 5.6, 2.4, "virginica"},  {6.4, 3.1, 5.5, 1.8, "virginica"},
    {6.0, 3.0, 4.8, 1.8, "virginica"},  {6.9, 3.1, 5.4, 2.1, "virginica"},  {6.7, 3.1, 5.6, 2.4, "virginica"},
    {6.9, 3.1, 5.1, 2.3, "virginica"},  {5.8, 2.7, 5.1, 1.9, "virginica"},  {6.8, 3.2, 5.9, 2.3, "virginica"},
    {6.7, 3.3, 5.7, 2.5, "virginica"},  {6.7, 3.0, 5.2, 2.3, "virginica"},  {6.3, 2.5, 5.0, 1.9, "virginica"},
    {6.5, 3.0, 5.2, 2.0, "virginica"},  {6.2, 3.4, 5.4, 2.3, "virginica"},  {5.9, 3.0, 5.1, 1.8, "virginica"},
};

void plotIrisesSize()
{
    //
    // Aggregate the data by species
    //

    unordered_map<string, vector<Iris>> irises_by_species;
    for (const auto& iris : iris_dataset)
    {
        irises_by_species[iris.species].emplace_back(iris);
    }

    //
    // Plot dimensions with a separate legend entry for each species
    //

    Plot plt;
    plt.title("Iris Dataset");
    plt.xlabel("Sepal Width");
    plt.ylabel("Sepal Length");

    for (const auto& [species, irises] : irises_by_species)
    {
        vector<float> sepal_lengths;
        vector<float> sepal_widths;
        vector<float> petal_lengths;
        for (const auto& iris : irises)
        {
            sepal_lengths.emplace_back(iris.sepal_length);
            sepal_widths.emplace_back(iris.sepal_width);
            petal_lengths.emplace_back(iris.petal_length);
        }

        plt.scatter_cs(sepal_widths,
                       sepal_lengths,
                       vector<float>{},
                       petal_lengths,
                       PlotScatterOptions{.label = species,
                                          .size_in_screen_perc = 2.0F,
                                          .marker = PlotScatterMarker::CIRCLE,
                                          .size_axis_limits = {0.5, 2.0}});
    }

    plt.show(false);
}

void plotIrisesColormap()
{
    //
    // Collect relevant data
    //

    vector<float> sepal_lengths;
    vector<float> sepal_widths;
    vector<float> petal_lengths;
    for (const auto& iris : iris_dataset)
    {
        sepal_lengths.emplace_back(iris.sepal_length);
        sepal_widths.emplace_back(iris.sepal_width);
        petal_lengths.emplace_back(iris.petal_length);
    }

    //
    // Plot dimensions with color assigned based on petal length
    //

    Plot plt;
    plt.title("Iris Dataset");
    plt.xlabel("Sepal Width");
    plt.ylabel("Sepal Length");

    plt.scatter_cs(
        sepal_widths,
        sepal_lengths,
        petal_lengths,
        vector<float>{},
        PlotScatterOptions{.label = "Petal Length", .size_in_screen_perc = 2.0F, .marker = PlotScatterMarker::CIRCLE});

    plt.show();
}

int main()
{
    plotIrisesSize();
    plotIrisesColormap();
}
