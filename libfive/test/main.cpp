/*
libfive: a CAD kernel for modeling with implicit functions

Copyright (C) 2017  Matt Keeter

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.
*/
#define CATCH_CONFIG_RUNNER
#include <libfive/render/brep/mesh.hpp>
#include <libfive/render/brep/settings.hpp>
#include <libfive/render/discrete/heightmap.hpp>
#include <libfive/render/discrete/voxels.hpp>
#include <util/shapes.hpp>

#include "catch.hpp"
#include "libfive/render/brep/region.hpp"

using namespace libfive;


int main()
{
    int level = 2;
    libfive::Tree sponge = menger(level);

    libfive::Voxels r({-2.5, -2.5, -2.5}, {2.5, 2.5, 2.5}, 250);

    Eigen::Matrix3f m;
    m = Eigen::AngleAxisf(float(M_PI/4), Eigen::Vector3f::UnitY()) *
        Eigen::AngleAxisf(float(atan(1/sqrt(2))), Eigen::Vector3f::UnitX());

    auto sponge_ = sponge.remap(
        m(0,0)* libfive::Tree::X() + m(0,1)* libfive::Tree::Y() + m(0,2)* libfive::Tree::Z(),
        m(1,0)* libfive::Tree::X() + m(1,1)* libfive::Tree::Y() + m(1,2)* libfive::Tree::Z(),
        m(2,0)* libfive::Tree::X() + m(2,1)* libfive::Tree::Y() + m(2,2)* libfive::Tree::Z());
    // // size_t workers = 8;
    // const std::atomic_bool& abort(false);
    // auto heightmap = libfive::Heightmap::render(sponge_, r, abort);
    // // heightmap->savePNG("sponge.png");
    // heightmap->saveNormalPNG("spongeNormal2.png");


    Region<3> r2({-10, -10, -10}, {10, 10, 10});
    std::cout<<"sponge level: " << level << std::endl;
    std::cout<<"Without openMP, the time of mesh-render costing: " << std::endl;
    auto start1 = std::chrono::high_resolution_clock::now();
    auto mesh = Mesh::render(sponge_, r2, BRepSettings());
    // 记录结束时间
    auto end1 = std::chrono::high_resolution_clock::now();

     // 计算时间差
    std::chrono::duration<double> elapsed1 = end1 - start1;
    std::cout << elapsed1.count() << " seconds" << std::endl;

    std::cout<<"Without openMP, the time of saveSTL costing: " << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    mesh->saveSTL("sponge_reigon_10.stl");
    // 记录结束时间
    auto end = std::chrono::high_resolution_clock::now();

    // 计算时间差
    std::chrono::duration<double> elapsed = end - start;

    // 打印 saveSTL 执行前后的时间
    std::cout << elapsed.count() << " seconds" << std::endl;


    return 0;
}
